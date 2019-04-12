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
#include "telemetry/headers/telemetry_core.h"
#include <wrpTimer.h>
#include "esp_types.h"
#include "driver/periph_ctrl.h"
#include "soc/timer_group_struct.h"
#include <motController.h>
#include "pindef.h"
#include "driver/gpio.h"

//Defines the pin which should blink for the blink task
#define BLINK_GPIO 2

void blink_task(void *pvParameter)
{
    /* Configure the IOMUX register for pad BLINK_GPIO (some pads are
       muxed to GPIO on reset already, but some default to other
       functions and need to be switched to GPIO. Consult the
       Technical Reference for a list of pads and their default
       functions.)
    */
    gpio_pad_select_gpio(BLINK_GPIO);
    /* Set the GPIO as a push/pull output */
    gpio_set_direction(BLINK_GPIO, GPIO_MODE_OUTPUT);
    while(1) {
        /* Blink off (output low) */
        gpio_set_level(BLINK_GPIO, 0);
        vTaskDelay(500 / portTICK_PERIOD_MS);
        /* Blink on (output high) */
        gpio_set_level(BLINK_GPIO, 1);
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
}

void ramp_task(void *pvParameter)
{
    int8_t ramp_cntr = 12;
    bool dir = true;
    mcpwm_example_config();
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1ULL<<MC_FORWARD | 1ULL<<MC_REVERSE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    MOTOR_FORWARD(); //initially brakes the motor


    while(1) {
        if(ramp_cntr == 10){
            dir = true;
            printf("Motor dir reversed! at 10\n");
        }
        if(ramp_cntr == 30){
            dir = false;
            printf("Motor dir reversed! at 30\n");
        }



        mcpwm_set_duty(0,0,MCPWM_OPR_A,(float)ramp_cntr);
        vTaskDelay(100 / portTICK_PERIOD_MS);

        if(dir){
            ramp_cntr++;
        } else{
            ramp_cntr--;
        }
    }
}
uint16_t ctr = 0;
void helloSender(void *pvParameter){
    publish_u16("helloWorldTopic",ctr);
    printf("sent hello world!\n");
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    ctr++;
}

void teleUpdateTask(void *pvParameter){
    while(1){
        update_telemetry();
        vTaskDelay(100 / portTICK_PERIOD_MS);
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
    //Setup Code, Initialization of components
    //tel_init();
    //printf("\ntesting functionality\n");
    // timerAlarmSet();
    tTestAlarmSet();
    printf("timer alarm set\n");

    //xTaskCreate(tel_init,"tel_init",4096, NULL, 5, NULL);


    xTaskCreate(timerInitTask,"timerInitTask", 4096, NULL, 5, NULL);
    xTaskCreate(blink_task, "blink_task", 4096, NULL, 3, NULL); //blinks on port 2
    //xTaskCreate(ramp_task, "ramp_task", 4096, NULL, 5, NULL); //Ramps the MCPWM up and down
     xTaskCreate(motCntrlTask, "motCntrlTask", 8192, NULL, 5, NULL);



    //xTaskCreate(helloSender, "helloSender", 4096, NULL, 5, NULL); //sends an incrementing number on topic helloWorldTopic every 500ms
    //xTaskCreate(teleUpdateTask, "teleUpdateTask", 4096, NULL, 4, NULL); //Calls the update_telemetry function as often as possible

}



/* void app_main()
{
    printf("Testing MCPWM...\n");
    cap_queue = xQueueCreate(1, sizeof(capture)); //comment if you don't want to use capture module
    xTaskCreate(disp_captured_signal, "mcpwm_config", 4096, NULL, 5, NULL);  //comment if you don't want to use capture module
    xTaskCreate(gpio_test_signal, "gpio_test_signal", 4096, NULL, 5, NULL); //comment if you don't want to use capture module
    xTaskCreate(mcpwm_example_config, "mcpwm_example_config", 4096, NULL, 5, NULL);
} */