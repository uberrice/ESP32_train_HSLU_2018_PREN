#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "esp_types.h"
#include "soc/timer_group_struct.h"

#include <wrpTimer.h>


int intset = 0;
void IRAM_ATTR timer_group0_isr(void *para){
    clearIntFlag00();
    // TIMERG0.int_clr_timers.t0 = 1;

    int timer_idx = (int) para;
    TIMERG0.hw_timer[timer_idx].update = 1;
    
    if(intset>0){
        intset--;
        tTriggerInUs(200, TIMER_GROUP_0,TIMER_0);
        TIMERG0.hw_timer[timer_idx].config.alarm_en = TIMER_ALARM_EN;
    }
}