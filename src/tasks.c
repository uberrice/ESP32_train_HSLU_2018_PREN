#include "tasks.h"


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "pindef.h"
#include <mcp.h>
#include <telemetry_core.h>
#include <wrpTimer.h>
#include <motController.h>
#include "interrupts.h"

#include "driver/mcpwm.h"
#include "ser_init.h"
#include "esp_types.h"
#include "driver/periph_ctrl.h"
#include "soc/timer_group_struct.h"
#include "driver/gpio.h"

uint8_t ctr = 0;
uint8_t county = 0;
void helloSender(void *pvParameter){
    vTaskDelay(100 / portTICK_PERIOD_MS); //delay to allow data structure to initialize
    attach_u8("this is a test of a very long topic",&county);
    county++;
    while(1){
        publish_u8("uint",ctr);
        publish_u8("this is a test of a very long topic", ctr);
        //publish_u16("count",ctr);
        ctr++;
        printf("sent hello world! County currently: %i\n", county);
        vTaskDelay(10 / portTICK_PERIOD_MS);
    }
}


void teleUpdateTask(void *pvParameter){
    tel_init(NULL);
    while(1){
        update_telemetry();
        vTaskDelay(1 / portTICK_PERIOD_MS);
    }
}


void tTestAlarmSet(void){
    timer_config_t config;
    config.divider = TIMER_DIVIDER;
    config.counter_dir = TIMER_COUNT_UP;
    config.counter_en = TIMER_PAUSE;
    config.alarm_en = TIMER_ALARM_EN;
    config.intr_type = TIMER_INTR_LEVEL;
    config.auto_reload = 0;
    timer_init(TIMER_GROUP_0, TIMER_0, &config);

    /* Timer's counter will initially start from value below.
       Also, if auto_reload is set, this value will be automatically reload on alarm */
    timer_set_counter_value(TIMER_GROUP_0, TIMER_0, 0x00000000ULL);

    /* Configure the alarm value and the interrupt on alarm. */
    tTriggerinMs00(1);
    timer_enable_intr(TIMER_GROUP_0, TIMER_0);
    timer_isr_register(TIMER_GROUP_0, TIMER_0, timer_group0_isr, (void *) TIMER_0, ESP_INTR_FLAG_IRAM, NULL);

    timer_start(TIMER_GROUP_0, TIMER_0);
}

extern int intset; //from interrupts.c
void timerInitTask(void* pv){
    //timerInit(TIMER_GROUP_0,TIMER_0);
    printf("Timer initialized\n");
    double timerval = 0;
    while(1){
        timer_get_counter_time_sec(TIMER_GROUP_0,TIMER_0,&timerval);
        printf("current time value: %lf %i\n",timerval,intset);
        printf("Rev dist: %f ; Step dist: %f\n", ONEREV_DIST, ONESTEP_DIST);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
    }
}