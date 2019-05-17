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
    vTaskDelay(1000 / portTICK_PERIOD_MS); //delay to allow data structure to initialize
    attach_u8("this is a test of a very long topic",&county);
    attach_i32("motorrpm",getRPMref());
    county++;
    while(1){
        publish_u8("uint",ctr);
        publish_u8("this is a test of a very long topic", ctr);
        //publish_u16("count",ctr);
        ctr++;
        printf("sent hello world! County currently: %i\n", county);
        vTaskDelay(10000 / portTICK_PERIOD_MS);
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
    TIMERG0.int_clr_timers.t0 = 1;

    int timer_idx = (int) para;
    TIMERG0.hw_timer[0].update = 1;
    intset++;
    if(intset>0){
        intset--;
    }
        tTriggerInUs(200, TIMER_GROUP_0,TIMER_0);
        TIMERG0.hw_timer[timer_idx].config.alarm_en = TIMER_ALARM_EN;
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

//extern int intset; //from interrupts.c
void timerInitTask(void* pv){
    timerInit(TIMER_GROUP_0,TIMER_0);
    //tTestAlarmSet();
    printf("Timer initialized\n");
    double timerval = 0;
    while(1){
        timer_get_counter_time_sec(TIMER_GROUP_0,TIMER_0,&timerval);
        printf("current time value: %lf %i\n",timerval,intset);
        printf("Rev dist: %f ; Step dist: %f\n", ONEREV_DIST, ONESTEP_DIST);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

int32_t winch_steps = 4096;  //how many steps to take
void winchTask(void* pv){
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1ULL<<P_WINCHDIR | 1ULL<<P_WINCH;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    gpio_set_level(P_WINCHDIR, 1);

    for(;;){
        //invert winch for set number of times

        if(winch_steps>0){
            winch_steps--;
            gpio_set_level(P_WINCHDIR, 1);
        } else if(winch_steps<0){
            winch_steps++;
            gpio_set_level(P_WINCHDIR, 0);
        }
        if(winch_steps==0){
            //do nothing
            vTaskDelay(100/portTICK_PERIOD_MS);
        }else {
        gpio_set_level(P_WINCH, 1);
        vTaskDelay(1/portTICK_PERIOD_MS);
        gpio_set_level(P_WINCH, 0);
        vTaskDelay(1/portTICK_PERIOD_MS);
        }
    }
    
}

void beepTask(void*pv){
    int8_t numtobeep = 20;
    //Configure forward/reverse pins for motor controller
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1ULL<<P_BEEPER;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    vTaskDelay(3000/portTICK_PERIOD_MS);
    for(;;){
        if(numtobeep > 0){
            gpio_set_level(P_BEEPER, 1);
            vTaskDelay(500/portTICK_PERIOD_MS);
            gpio_set_level(P_BEEPER, 0);
            vTaskDelay(500/portTICK_PERIOD_MS);
            numtobeep--;
        }else{
            vTaskDelay(1000/portTICK_PERIOD_MS);
        }
    }
}
