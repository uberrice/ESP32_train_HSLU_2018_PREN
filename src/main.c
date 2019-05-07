/*  PREN Projekt "Selbstfahrender Zug"
    Peter Allenspach, Cyrill Durrer
    verwendete Libraries mit Lizenzen:
        telemetry von https://github.com/Overdrivr, verteilt unter der MIT-Lizenz
*/


#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include <mcp.h>
#include "driver/mcpwm.h"
#include "ser_init.h"
#include <telemetry_core.h>
#include <wrpTimer.h>
#include "esp_types.h"
#include "driver/periph_ctrl.h"
#include "soc/timer_group_struct.h"
#include <motController.h>
#include "pindef.h"
#include "driver/gpio.h"


uint8_t ctr = 0;
uint8_t county = 0;
void helloSender(void *pvParameter){
    // uint16_t county = 0;
    // attach_u16("count",&county);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    // attach_u8("uint",&county);
    // attach_u8("uint",&county);
    // attach_u8("uint",&county);
    // attach_u8("uint",&county);
    // attach_u8("uint",&county);
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
int intset = 0;
void IRAM_ATTR timer_group0_isr(void *para){
    clearIntFlag00();
    // TIMERG0.int_clr_timers.t0 = 1;

    int timer_idx = (int) para;
    TIMERG0.hw_timer[timer_idx].update = 1;
    intset++;
    if(intset<100000){
    tTriggerInUs(10, TIMER_GROUP_0,TIMER_0);
    TIMERG0.hw_timer[timer_idx].config.alarm_en = TIMER_ALARM_EN;
    }
}

void timerAlarmSet(void){
    //timer_isr_register(TIMER_GROUP_0,TIMER_0,t00isr,(void*)TIMER_0,ESP_INTR_FLAG_IRAM,NULL);
    timerInit00();
    tEnableAlarm00();
    tInterruptEnable00();
    timer_isr_register(TIMER_GROUP_0,TIMER_0,timer_group0_isr,(void*)TIMER_0,ESP_INTR_FLAG_IRAM,NULL);
    //tTriggerinMs00(1000);
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

void app_main()
{
    tTestAlarmSet();
    printf("timer alarm set\n");


    xTaskCreate(timerInitTask,"timerInitTask", 4096, NULL, 5, NULL);
    xTaskCreate(motCntrlTask, "motCntrlTask", 8192, NULL, 5, NULL);
    xTaskCreatePinnedToCore(teleUpdateTask, "teleUpdateTask", 4096, NULL, 4, NULL, 1);
    xTaskCreatePinnedToCore(helloSender, "helloSender", 4096, NULL, 5, NULL, 1); //sends an incrementing number on topic helloWorldTopic every 500ms

}

