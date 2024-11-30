ln -s $(pwd)/qmk_firmware/keyboards/keyball ../__qmk__/keyboards/keyball
qmk compile -j 8 -kb keyball/keyball44 -km am338
mkdir -p tmp
cp ../__qmk__/keyball_keyball44_am338.hex tmp/keyball44.hex