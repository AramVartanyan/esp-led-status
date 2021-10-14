esp-led-status
==============

Library for [esp-idf](https://github.com/espressif/esp-idf) and [ESP8266_RTOS_SDK](https://github.com/espressif/ESP8266_RTOS_SDK)
to communicate device status through different LED blink patterns.

Patterns are defined as a list of delays in milliseconds with positive values
being periods when LED is on and negative values - periods when LED is off.

```c
// 1000ms ON, 1000ms OFF
led_status_pattern_t waiting_wifi = LED_STATUS_PATTERN({1000, -1000});

// one short blink every 3 seconds
led_status_pattern_t normal_mode = LED_STATUS_PATTERN({100, -2900});

// three short blinks
led_status_pattern_t three_short_blinks = LED_STATUS_PATTERN({100, -100, 100, -100, 100, -700});


static led_status_t status = led_status_init(STATUS_LED_PIN, 1);

// execute one time signal
led_status_signal(status, &three_short_blinks, false);

// execute looped signal
led_status_signal(status, &waiting_wifi, true);
```

License
=======

MIT licensed. See the bundled [LICENSE](https://github.com/maximkulkin/esp-led-status/blob/master/LICENSE) file for more details.
