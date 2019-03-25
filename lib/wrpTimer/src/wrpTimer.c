#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "wrpTimer.h"
#include "driver/periph_ctrl.h"
typedef struct {
    int type;  // the type of timer's event
    int timer_group;
    int timer_idx;
    uint64_t timer_counter_value;
} timer_event_t;

void timerInit(timer_group_t tGroup, timer_idx_t tVal){
    timer_config_t config;
    config.divider = TIMER_DIVIDER;
    config.counter_dir = TIMER_COUNT_UP;
    config.counter_en = TIMER_PAUSE;
    config.alarm_en = TIMER_ALARM_EN;
    config.intr_type = TIMER_INTR_LEVEL;
    config.auto_reload = 0; //don't autoreload
    timer_init(tGroup, tVal, &config);

    timer_set_counter_value(tGroup, tVal, 0x00000000ULL); // starts timer from 0
    timer_start(tGroup, tVal); //starts the timer initially
}

void tTriggerInUs(uint32_t micros, timer_group_t tGroup, timer_idx_t tVal){
    uint64_t counter_old = 0;
    timer_get_counter_value(tGroup,tVal,&counter_old);
    uint64_t counter_add = (micros * TIMER_SCALE) / 1000;
    timer_set_alarm_value(tGroup, tVal, counter_old+counter_add);
}

void tTriggerInMs(uint16_t millis, timer_group_t tGroup, timer_idx_t tVal){
    uint64_t counter_old = 0;
    timer_get_counter_value(tGroup,tVal,&counter_old);
    uint64_t counter_add = millis * TIMER_SCALE;
    timer_set_alarm_value(tGroup, tVal, counter_old+counter_add);
}

void tEnableAlarm(timer_group_t tGroup, timer_idx_t tVal){
    timer_set_alarm(tGroup, tVal, TIMER_ALARM_EN);
}

void tDisableAlarm(timer_group_t tGroup, timer_idx_t tVal){
    timer_set_alarm(tGroup, tVal, TIMER_ALARM_DIS);
}