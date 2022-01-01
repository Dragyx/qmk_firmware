

// Step 1.
// Declare custom effects using the RGB_MATRIX_EFFECT macro
// (note the lack of semicolon after the macro!)
RGB_MATRIX_EFFECT(fading_indicators)
RGB_MATRIX_EFFECT(my_cool_effect2)

// Step 2.
// Define effects inside the `RGB_MATRIX_CUSTOM_EFFECT_IMPLS` ifdef block
#ifdef RGB_MATRIX_CUSTOM_EFFECT_IMPLS

const HSV home_row = {HSV_WHITE};
const HSV layer_indicator = {HSV_BLUE};
static uint8_t hsv_counter = 0;
static uint8_t increment_counter_each = 10;
static uint8_t increment_counter = 0;

bool fading_indicators(effect_params_t* params) {
    RGB_MATRIX_USE_LIMITS(led_min, led_max);

    uint16_t max_tick = 65535 / rgb_matrix_config.speed;
    uint8_t layer = get_highest_layer(layer_state);

    for (uint8_t row = 0; row < MATRIX_ROWS; ++row) {
        for (uint8_t col = 0; col < MATRIX_COLS; ++col) {
            // get the layer and index of the current led
            uint8_t i = g_led_config.matrix_co[row][col];

            HSV hsv = rgb_matrix_get_hsv();

            RGB_MATRIX_TEST_LED_FLAGS();
            uint16_t tick = max_tick;
            // Reverse search to find most recent key hit
            for (int8_t j = g_last_hit_tracker.count - 1; j >= 0; j--) {
                if (g_last_hit_tracker.index[j] == i && g_last_hit_tracker.tick[j] < tick) {
                    tick = g_last_hit_tracker.tick[j];
                    break;
                }
            }
            // for the fade out effect
            uint16_t offset = scale16by8(tick, rgb_matrix_config.speed);
            // calculate the keycode from position and layer
            uint16_t keycode = keymap_key_to_keycode(layer, (keypos_t){col,row});
            switch(keycode) {
                // for these keys, keep the default color
                case KC_DELETE:
                case KC_PAGE_UP:
                case KC_PAGE_DOWN:
                case KC_END:
                    break;
                // Home row
                case KC_A:
                case KC_S:
                case KC_D:
                case KC_F:
                case KC_J:
                case KC_K:
                case KC_L:
                case KC_SEMICOLON:
                case KC_SPACE:
                    hsv = home_row;
                    break;
                // For the Hue, Saturation and value, rotate the color around.
                case RGB_HUI:
                case RGB_HUD:
                    hsv.h = hsv_counter;
                    increment_counter++;
                    if (increment_counter % increment_counter_each == 0) {
                        hsv_counter++;
                        increment_counter = 1;
                    }
                    break;
                case RGB_SAI:
                case RGB_SAD:
                    hsv.s = hsv_counter;
                    increment_counter++;
                    if (increment_counter % increment_counter_each == 0) {
                        hsv_counter++;
                        increment_counter = 1;
                    }
                    break;
                case RGB_VAI:
                case RGB_VAD:
                    hsv.v = hsv_counter;
                    increment_counter++;
                    if (increment_counter % increment_counter_each == 0) {
                        hsv_counter++;
                        increment_counter = 1;
                    }
                    break;
                default:
                    hsv.v = scale8(255 - offset, rgb_matrix_get_val());
                    break;
            }
           // light up the function layer with no regard to the layer
            uint16_t keycode_layer0 = keymap_key_to_keycode(0, (keypos_t){col,row});
            switch(keycode_layer0) {
                case KC_F1: if (layer == 0) { hsv = layer_indicator;   } break;
                case KC_F2: if (layer == 1) { hsv = layer_indicator;   } break;
                case KC_F3: if (layer == 2) { hsv = layer_indicator;   } break;
                case KC_F4: if (layer == 3) { hsv = layer_indicator;   } break;
                case KC_F5: if (layer == 4) { hsv = layer_indicator;   } break;
                case KC_F6: if (layer == 5) { hsv = layer_indicator;   } break;
                case KC_F7: if (layer == 6) { hsv = layer_indicator;   } break;
                case KC_F8: if (layer == 7) { hsv = layer_indicator;   } break;
                case KC_F9: if (layer == 8) { hsv = layer_indicator;   } break;
                case KC_F10: if (layer == 9) { hsv = layer_indicator;  } break;
                case KC_F11: if (layer == 10) { hsv = layer_indicator; } break;
                case KC_F12: if (layer == 11) { hsv = layer_indicator; } break;
            }
            RGB rgb = hsv_to_rgb(hsv);

            rgb_matrix_set_color(i, rgb.r, rgb.g, rgb.b);
        }
    }
    return rgb_matrix_check_finished_leds(led_max);
}

// e.g: A more complex effect, relying on external methods and state, with
// dedicated init and run methods
static uint8_t some_global_state;
static void my_cool_effect2_complex_init(effect_params_t* params) {
  some_global_state = 1;
}
static bool my_cool_effect2_complex_run(effect_params_t* params) {
  RGB_MATRIX_USE_LIMITS(led_min, led_max);
  for (uint8_t i = led_min; i < led_max; i++) {
    rgb_matrix_set_color(i, 0xff, some_global_state++, 0xff);
  }
  return rgb_matrix_check_finished_leds(led_max);
}
static bool my_cool_effect2(effect_params_t* params) {
  if (params->init) my_cool_effect2_complex_init(params);
  return my_cool_effect2_complex_run(params);
}

#endif // RGB_MATRIX_CUSTOM_EFFECT_IMPLS
