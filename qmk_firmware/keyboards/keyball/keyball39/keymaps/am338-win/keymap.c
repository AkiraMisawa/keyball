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


enum custom_keycodes {
    CTRL_ALT_DEL = SAFE_RANGE,
};

// clang-format off
const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
  // default layer
  [0] = LAYOUT_universal(
    ALT_T(KC_Q) , KC_W    , KC_E    , KC_R         , KC_T         ,                               KC_Y         , KC_U    , KC_I       , KC_O    , KC_P           ,
    CTL_T(KC_A) , KC_S    , KC_D    , KC_F         , KC_G         ,                               KC_H         , KC_J    , LT(3,KC_K) , KC_L    , LT(3,KC_COMM)  ,
    SFT_T(KC_Z) , KC_X    , KC_C    , KC_V         , KC_B         ,                               KC_N         , KC_M    , KC_BTN1    , KC_BTN2 , SFT_T(KC_SLSH) ,
    KC_LGUI     , KC_LCTL , KC_LALT , LT(1,KC_TAB) , LT(2,KC_SPC) , LT(3,KC_ESC) ,      KC_BSPC , LT(2,KC_ENT) , _______ , _______    ,_______  , CTL_T(KC_MINS)
  ),
  // symbol layer
  [1] = LAYOUT_universal(
    S(KC_5) , S(KC_4)    , S(KC_7) , S(KC_6)   , KC_SLSH    ,                         S(KC_BSLS) , S(KC_9)    , S(KC_0)    , S(KC_COMM) , S(KC_DOT)  ,
    S(KC_2) , S(KC_NUHS) , S(KC_8) , S(KC_EQL) , KC_EQL     ,                         S(KC_MINS) , S(KC_LBRC) , S(KC_RBRC) , KC_SCLN    , S(KC_SCLN) ,
    KC_GRV  , S(KC_GRV)  , S(KC_3) , S(KC_1)   , S(KC_SLSH) ,                         KC_BSLS    , KC_LBRC    , KC_RBRC    , KC_QUOT    , S(KC_QUOT) ,
    _______ , _______    , _______ , _______   , _______    , _______ ,      KC_DEL , KC_ENT     , _______    , _______    , _______    , KC_ESC
  ),
  // number layer
  [2] = LAYOUT_universal(
    C(KC_W) , KC_7    , KC_8    , KC_9    , KC_PLUS ,                         C(KC_PGUP) , KC_HOME , KC_UP   , KC_END  , C(KC_PGDN) ,
    KC_LCTL , KC_4    , KC_5    , KC_6    , KC_0    ,                         KC_PGUP    , KC_LEFT , KC_DOWN , KC_RGHT , KC_PGDN    ,
    KC_LSFT , KC_1    , KC_2    , KC_3    , KC_DOT  ,                         KC_INS     , KC_BTN4 , KC_BTN1 , KC_APP  , KC_BTN5    ,
    _______ , _______ , _______ , _______ , _______ , _______ ,      KC_DEL , KC_ENT     , _______ , _______ , _______ , KC_ESC
  ),
  // function layer
  [3] = LAYOUT_universal(
    C(KC_W) , KC_F7   , KC_F8  , KC_F9    , KC_F10  ,                         KC_F9       , KC_F10  , KC_F11  , KC_F12  , A(KC_F4) ,
    KC_LCTL , KC_F4   , KC_F5  , KC_F6    , KC_F11  ,                         _______     , _______ , KC_F5   , _______ , _______  ,
    KC_LSFT , KC_F1   , KC_F2  , KC_F3    , KC_F12  ,                         LSG(KC_S) , KC_BTN4 , KC_F2   , KC_APP  , KC_BTN5  ,
    _______ , _______ , LALT(LCTL(KC_DEL)) , _______ , _______ , _______ ,      KC_DEL , KC_ENT      , _______ , _______ , _______ , KC_ESC
  ),
  // mouse layer
  [4] = LAYOUT_universal(
    _______ , _______ , _______ , _______ , _______ ,                          _______ , _______ , _______ , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ ,                          KC_BTN4 , KC_BTN1 , KC_BTN3 , KC_BTN2 , KC_BTN5 ,
    _______ , _______ , _______ , _______ , _______ ,                          _______ , _______ , _______ , _______ , _______ ,
    _______ , _______ , _______ , _______ , _______ , _______ ,      _______ , _______ , _______ , _______ , _______ , _______
  ),
};
// clang-format on

layer_state_t layer_state_set_user(layer_state_t state) {
    // Auto enable scroll mode when the highest layer is 3
    keyball_set_scroll_mode(get_highest_layer(state) == 3);
    return state;
}

#ifdef OLED_ENABLE

#    include "lib/oledkit/oledkit.h"

void oledkit_render_info_user(void) {
    keyball_oled_render_keyinfo();
    keyball_oled_render_ballinfo();
    keyball_oled_render_layerinfo();
}
#endif


#ifdef COMBO_ENABLE
const uint16_t PROGMEM my_tab[] = { KC_S, KC_D, COMBO_END };
const uint16_t PROGMEM my_stab[] = { KC_D, KC_F, COMBO_END };
const uint16_t PROGMEM my_comma[] = { KC_M, KC_BTN1, COMBO_END };
const uint16_t PROGMEM my_dot[] = { KC_BTN1, KC_BTN2, COMBO_END };

combo_t key_combos[] = {
    COMBO(my_tab, KC_TAB),
    COMBO(my_stab, S(KC_TAB)),
    COMBO(my_comma, KC_COMMA),
    COMBO(my_dot, KC_DOT),
};
#endif