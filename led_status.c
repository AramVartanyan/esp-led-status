#include <stdlib.h>

#include <freertos/FreeRTOS.h>
#include <freertos/timers.h>
#include <esp_system.h>
#include <driver/gpio.h>

#include "led_status_private.h"

#define ABS(x) (((x) < 0) ? -(x) : (x))

typedef struct {
    bool active;
    bool loop;
    uint8_t gpio;
    TimerHandle_t timer;

    int n;
    led_status_pattern_t *pattern;
    led_status_pattern_t *signal_pattern;
} led_status_t;

static void led_status_write(led_status_t *status, bool on) {
    gpio_set_level(status->gpio, on ? status->active : !status->active);
}

static void led_status_tick(led_status_t *status) {
    
    led_status_pattern_t *p;
    
    if (!status->signal_pattern) {
        if (status->loop) {
            status->signal_pattern = status->pattern;
            p = status->signal_pattern;
        } else {
            xTimerStop(status->timer, 1);
            led_status_write(status, false);
            return;
        }
        
    } else {
        p = status->signal_pattern;
    }
    
    led_status_write(status, p->delay[status->n] > 0);
    xTimerChangePeriod(status->timer, pdMS_TO_TICKS(ABS(p->delay[status->n])), 1);
    
    if (xTimerIsTimerActive(status->timer)) {
        xTimerReset(status->timer, 1);
    } else {
        xTimerStart(status->timer, 1);
    }
    status->n = (status->n + 1) % p->n;
    if (status->signal_pattern && status->n == 0) {
        status->signal_pattern = NULL;
    }
}

static void led_status_cb(TimerHandle_t xTimer) {
    led_status_t *status = (led_status_t*) pvTimerGetTimerID(xTimer);
    led_status_tick(status);
}

led_status_t *led_status_init(uint8_t gpio, bool active_level) {
    led_status_t *status = malloc(sizeof(led_status_t));
    status->gpio = gpio;
    status->active = active_level;
    
    status->timer = xTimerCreate("Status timer", pdMS_TO_TICKS(500), pdFALSE, status, led_status_cb);

    led_status_write(status, false);

    return status;
}

void led_status_done(led_status_t *status) {
    xTimerStop(status->timer, 1);
    xTimerDelete(status->timer, 1);
    led_status_write(status, false);
    free(status);
}

void led_status_signal(led_status_t *status, led_status_pattern_t *pattern, bool loop) {
    
    if (!status->signal_pattern && !pattern)
        return;
    
    status->pattern = pattern;
    status->signal_pattern = pattern;
    status->loop = loop;
    status->n = 0;  // whether signal pattern is NULL or not, just reset the state
    led_status_tick(status);
}
