/* Copyright 2019 monksoffunk
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */
#include QMK_KEYBOARD_H

enum layer_names {
    _AUDIO,
    _HUE,
    _SAT,
    _VAL
};

const uint16_t PROGMEM keymaps[][MATRIX_ROWS][MATRIX_COLS] = {
    // LAYOUT(LeftEncoder, RightEncoder, LeftSwitch, CenterLeftSwitch, CenterRightSwitch, RightSwitch)
    [_AUDIO] = LAYOUT(KC_A, KC_B, KC_C, KC_D, KC_E),
    [_HUE]   = LAYOUT(KC_A, KC_B, KC_C, KC_D, KC_E),
    [_SAT]   = LAYOUT(KC_A, KC_B, KC_C, KC_D, KC_E),
    [_VAL]   = LAYOUT(KC_A, KC_B, KC_C, KC_D, KC_E),
};

#ifdef ENCODER_MAP_ENABLE
const uint16_t PROGMEM encoder_map[][NUM_ENCODERS][NUM_DIRECTIONS] = {
    [_AUDIO] = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [_HUE]   = { ENCODER_CCW_CW(KC_A, KC_Y) },
    [_SAT]  = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },
    [_VAL]  = { ENCODER_CCW_CW(KC_VOLD, KC_VOLU) },

};
#endif

