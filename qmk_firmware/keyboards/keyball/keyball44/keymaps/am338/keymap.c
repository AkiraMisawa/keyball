/*
Copyright 2022 @Yowkees
Copyright 2022 MURAOKA Taro (aka KoRoN, @kaoriya)

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 2 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <http://www.gnu.org/licenses/>.
*/

#include QMK_KEYBOARD_H
#include "quantum.h"

enum custom_keycodes
{
  KC_MY_BTN1 = KEYBALL_SAFE_RANGE,
  KC_MY_BTN2,
  KC_MY_BTN3,
  KC_MY_BTN4,
  KC_MY_BTN5,
  KC_DOUBLE_CLICK_BTN1,
  KC_TRIPLE_CLICK_BTN1,
};

enum click_state
{
  NONE = 0,
  WAITING,
  CLICKABLE,
  CLICKING,
  CLICKED,
};

enum click_state state;
uint16_t click_timer;

uint16_t clicked_stay_time = 150;
uint16_t clickable_stay_time = 1400;

const int16_t to_clickable_movement = 0;
const uint16_t click_layer = 6;

int16_t scroll_v_mouse_interval_counter;
int16_t scroll_h_mouse_interval_counter;

int16_t scroll_v_threshold = 50;
int16_t scroll_h_threshold = 50;

int16_t mouse_record_threshold = 30;
int16_t mouse_move_count_ratio = 5;

int16_t mouse_movement;

void enable_click_layer(void)
{
  layer_on(click_layer);
  click_timer = timer_read();
  state = CLICKABLE;
}

void disable_click_layer(void)
{
  state = NONE;
  layer_off(click_layer);
}

int16_t my_abs(int16_t num)
{
  if (num < 0)
  {
    num = -num;
  }

  return num;
}

bool process_record_user(uint16_t keycode, keyrecord_t *record)
{
  static bool is_gui_active = false;
  static bool is_ctrl_active = false;

  static bool is_lt1_pressed = false;
  static bool is_lt2_pressed = false;
  static bool is_lt3_pressed = false;

  switch (keycode)
  {
  case KC_MS_BTN1:
  case KC_MS_BTN2:
  case KC_MS_BTN3:
  case KC_MS_BTN4:
  case KC_MS_BTN5:
  {
    if (click_layer && get_highest_layer(layer_state) == click_layer)
    {
      if (record->event.pressed)
      {
        state = CLICKING;
      }
      else
      {
        enable_click_layer();
        state = CLICKED;
      }
    }
    return true;
  }

  case KC_LALT:
  case KC_LSFT:
  case KC_LCTL:
  {
    return true;
  }

    static bool is_lt1_lang2_pressed = false;
    static bool is_lt1_lang1_pressed = false;

  case LT(1, KC_LNG2):
  {
    if (record->event.pressed)
    {
      click_timer = timer_read();
      is_lt1_pressed = true;
      is_lt1_lang2_pressed = true;
      layer_on(1);
      disable_click_layer();

      if (is_lt2_pressed)
      {
        layer_off(2);
      }
      else if (is_lt3_pressed)
      {
        layer_off(3);
      }
    }
    else
    {
      is_lt1_pressed = false;
      is_lt1_lang2_pressed = false;

      if (!is_lt1_lang2_pressed && !is_lt1_lang1_pressed)
      {
        layer_off(1);
      }
      if (is_lt2_pressed)
      {
        layer_on(2);
      }
      else if (is_lt3_pressed)
      {
        layer_on(3);
      }

      if (is_gui_active)
      {
        unregister_code(KC_LGUI);
        is_gui_active = false;
      }
    }
    return false;
  }

  case LT(2, KC_SPC):
  {
    if (record->event.pressed)
    {
      click_timer = timer_read();
      is_lt2_pressed = true;
      layer_on(2);
      disable_click_layer();

      if (is_lt1_pressed)
      {
        layer_off(1);
      }
      else if (is_lt3_pressed)
      {
        layer_off(3);
      }
    }
    else
    {
      is_lt2_pressed = false;
      layer_off(2);

      if (is_lt1_pressed)
      {
        layer_on(1);
      }
      else if (is_lt3_pressed)
      {
        layer_on(3);
      }

      if (timer_elapsed(click_timer) < TAPPING_TERM)
      {
          tap_code(keycode);
      }
    }
    return false;
  }

  case LT(3, KC_ESC):
  case LT(3, KC_F):
  {
    if (record->event.pressed)
    {
      click_timer = timer_read();
      is_lt3_pressed = true;
      layer_on(3);
      disable_click_layer();

      if (is_lt1_pressed)
      {
        layer_off(1);
      }
      else if (is_lt2_pressed)
      {
        layer_off(2);
      }
    }
    else
    {
      is_lt3_pressed = false;
      layer_off(3);

      if (is_lt1_pressed)
      {
        layer_on(1);
      }
      else if (is_lt2_pressed)
      {
        layer_on(2);
      }

      if (timer_elapsed(click_timer) < TAPPING_TERM)
      {
        tap_code(keycode);
      }

      if (is_ctrl_active)
      {
        unregister_code(KC_LCTL);
        is_ctrl_active = false;
      }
    }

    return false;
  }

  default:
  {
    if (record->event.pressed)
    {
      disable_click_layer();
    }
  }
  }
  return true;
}

report_mouse_t pointing_device_task_user(report_mouse_t mouse_report)
{
  int16_t current_x = mouse_report.x;
  int16_t current_y = mouse_report.y;

  if (current_x != 0 || current_y != 0)
  {
    switch (state)
    {
    case CLICKABLE:
    {
      click_timer = timer_read();
      break;
    }

    case CLICKING:
    {
      break;
    }

    case WAITING:
    {
      mouse_movement += my_abs(current_x) + my_abs(current_y);

      if (mouse_movement >= to_clickable_movement)
      {
        mouse_movement = 0;
        enable_click_layer();
      }
      break;
    }

    default:
    {
      click_timer = timer_read();
      state = WAITING;
      mouse_movement = 0;
    }
    }
  }
  else
  {
    switch (state)
    {
    case CLICKING:
    {
      break;
    }

    case CLICKABLE:
    {
      if (timer_elapsed(click_timer) > clickable_stay_time)
      {
        disable_click_layer();
      }
      break;
    }

    case CLICKED:
    {
      if (timer_elapsed(click_timer) > clicked_stay_time)
      {
        disable_click_layer();
      }
      break;
    }

    case WAITING:
    {
      if (timer_elapsed(click_timer) > 50)
      {
        mouse_movement = 0;
        state = NONE;
      }
      break;
    }

    default:
    {
      mouse_movement = 0;
      state = NONE;
    }
    }
  }

  mouse_report.x = current_x;
  mouse_report.y = current_y;

  return mouse_report;
}

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // keymap for default
  [0] = LAYOUT_universal(
    KC_TAB , KC_Q , KC_W , KC_E , KC_R , KC_T ,                     KC_Y, KC_U , KC_I , KC_O , KC_P , KC_BSPC ,
    KC_RCTL , KC_A , KC_S , KC_D , LT(3, KC_F) , KC_G ,             KC_H , LT(3, KC_J) , KC_K , KC_L , KC_SCLN , KC_ENT ,
    KC_LSFT , KC_Z , KC_X , KC_C , KC_V , KC_B ,                    KC_N , KC_M , KC_COMM , KC_DOT , KC_MINS , KC_RSFT ,
    KC_LGUI , KC_LALT , LT(1, KC_LNG2) , LT(2, KC_SPC), KC_LSFT,    KC_BSPC , KC_ENT , _______ , _______ , LT(3, KC_ESC)
  ),

  [1] = LAYOUT_universal(
    KC_TAB , LSFT(KC_5) , LSFT(KC_4) , LSFT(KC_7) , LSFT(KC_6) , KC_SLSH ,          LSFT(KC_NUBS) , LSFT(KC_9) , LSFT(KC_0) , LSFT(KC_COMM) , LSFT(KC_DOT) , KC_BSPC ,
    KC_RCTL , LSFT(KC_QUOT) , KC_NUHS , LSFT(KC_8) , LSFT(KC_EQL) , KC_EQL ,        LSFT(KC_MINS) , LSFT(KC_LBRC) , LSFT(KC_RBRC) , KC_SCLN , LSFT(KC_SCLN) , KC_ENT ,
    KC_LSFT , KC_GRV , LSFT(KC_NUHS) , LSFT(KC_3) , LSFT(KC_1) , LSFT(KC_SLSH) ,    KC_NUBS , KC_LBRC , KC_RBRC , KC_QUOT , LSFT(KC_2) , KC_RSFT ,
    _______ , _______ , _______ , _______ , _______ ,                               KC_DEL , _______ , _______ , _______ , _______
  ),

  [2] = LAYOUT_universal(
    KC_TAB , _______ , KC_7 , KC_8 , KC_9 , _______ ,    KC_INS  , KC_HOME , KC_UP , KC_END , _______ , KC_BSPC ,
    KC_RCTL , _______ , KC_4 , KC_5 , KC_6 , KC_0  ,     KC_PGUP , KC_LEFT , KC_DOWN , KC_RIGHT , KC_PGDN , KC_ENT ,
    KC_LSFT , KC_0 , KC_1 , KC_2 , KC_3 , KC_0 ,         _______  , _______ , _______ , _______ , _______ , KC_RSFT ,
    _______ , _______ , _______ , _______ , _______ ,    KC_DEL , _______ , _______ , _______  , _______
  ),

  [3] = LAYOUT_universal(
    _______ , _______ , KC_F7 , KC_F8 , KC_F9 , KC_F10 ,     KC_F9 , KC_F10 , KC_F11 , KC_F12 , _______ , _______ ,
    _______ , _______ , KC_F4 , KC_F5 , KC_F6 , KC_F11 ,     _______ , _______ , KC_F5 , _______ , _______ , _______ ,
    KBC_SAVE , _______ , KC_F1 , KC_F2 , KC_F3 , KC_F12 ,    AML_TO , CPI_D100 , KC_F2 , CPI_I100 , KBC_SAVE , KBC_SAVE ,
    QK_BOOT , _______ , _______  , _______  , _______ ,      _______ , _______  , _______ , _______ , AML_TO
  ),

  [4] = LAYOUT_universal(
    _______ , _______ , _______ , _______ , _______ , _______ ,    _______ , _______ , _______ , _______ , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ , _______ ,    _______ , _______ , _______ , _______ , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ , _______ ,    _______ , _______ , _______ , _______ , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ ,              _______ , _______ , _______ , _______ , _______
  ),

  [5] = LAYOUT_universal(
    _______ , _______ , _______ , _______ , _______ , _______ ,    _______ , _______ , _______ , _______ , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ , _______ ,    _______ , _______ , _______ , _______ , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ , _______ ,    _______ , _______ , _______ , _______ , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ ,              _______ , _______ , _______ , _______ , _______
  ),

  [6] = LAYOUT_universal(
    AML_TO , _______ , _______ , _______ , _______ , _______ ,     _______ , KC_DOUBLE_CLICK_BTN1 , KC_TRIPLE_CLICK_BTN1 , _______ , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ , _______ ,    KC_MY_BTN4 , KC_MY_BTN1 , KC_MY_BTN3 , KC_MY_BTN2 , KC_MY_BTN5 , _______ ,
    _______ , _______ , _______ , _______ , _______ , _______ ,    _______ , _______ , _______ , _______ , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ ,              _______ , _______ , _______ , _______ , _______
  )
};
// clang-format on

layer_state_t layer_state_set_user(layer_state_t state)
{
  keyball_set_scroll_mode(get_highest_layer(state) == 1 || get_highest_layer(state) == 3);

  return state;
}

#ifdef OLED_ENABLE

#include "lib/oledkit/oledkit.h"

void oledkit_render_info_user(void)
{
  keyball_oled_render_keyinfo();  // キー情報を表示
  keyball_oled_render_ballinfo(); // トラックボール情報を表示

  oled_write_P(PSTR("Layer:"), false);
  oled_write(get_u8_str(get_highest_layer(layer_state), ' '), false);

  switch (state)
  {
  case WAITING:
    oled_write_ln_P(PSTR("  WAITING"), false);
    break;
  case CLICKABLE:
    oled_write_ln_P(PSTR("  CLICKABLE"), false);
    break;
  case CLICKING:
    oled_write_ln_P(PSTR("  CLICKING"), false);
    break;
  case CLICKED:
    oled_write_ln_P(PSTR("  CLICKED"), false);
    break;
  case SCROLLING:
    oled_write_ln_P(PSTR("  SCROLLING"), false);
    break;
  case NONE:
    oled_write_ln_P(PSTR("  NONE"), false);
    break;
  }
}

#endif
