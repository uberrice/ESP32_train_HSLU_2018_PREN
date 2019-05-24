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

#include "cyrill_test.h"
#include "taskhandles.h"

uint8_t ping = 0;
uint8_t ctr = 0;
uint8_t county = 0;
void helloSender(void *pvParameter){
    vTaskDelay(1000 / portTICK_PERIOD_MS); //delay to allow data structure to initialize
    attach_u8("helloworld",&county);
    county++;
    while(1){
        //publish_u8("uint",ctr);
        publish_u8("helloworld", ctr);
        //publish_u16("count",ctr);
        ctr++;
        printf("sent hello world! County currently: %i\n", county);
        vTaskDelay(1000 / portTICK_PERIOD_MS);
    }
}

uint8_t cube = 0;
uint8_t stopsignal = 0; //1 is white, 2 is black
uint16_t mycnt = 0;
void teleUpdateTask(void *pvParameter){
    tel_init(NULL);
    vTaskDelay(pdMS_TO_TICKS(500));
    uint8_t blockFlag = 0;
    uint8_t stopFlag = 0;
    //TODO: Register vars to use for communication here
    attach_u8("startcube",&cube); //todo: change block to cube
    attach_u8("stop",&stopsignal);
    attach_u8("ping",&ping);
    attach_i32("motorrpm",getRPMref());
    //
    while(1){
        update_telemetry();
        //TODO: Notify tasks whose values were updated with the update values
        if ((cube == 1) && (blockFlag == 0))
        {
            xTaskNotify(beepHandle,3,eSetValueWithOverwrite);
            xTaskCreatePinnedToCore(crane_task, "crane_task", 4096, NULL, 4, NULL,0);
            blockFlag = 1;
        }
        if ((stopsignal == 1) && (stopFlag == 0))
        {
            xTaskNotify(beepHandle,2,eSetValueWithOverwrite);
            xTaskCreatePinnedToCore(stop_task, "stop_task", 4096, NULL, 4, NULL,0);
            stopFlag = 1;
        }
        
        mycnt++;
        if(mycnt>1000){
            mycnt = 0;
            printf("cube is currently %i\n",cube);
        }
        vTaskDelay(1 / portTICK_PERIOD_MS); //TODO: Don't delay but yield
    }
}


int intset = 0;
void IRAM_ATTR timer_group0_isr(void *para){
    clearIntFlag00();
    TIMERG0.int_clr_timers.t0 = 1;

    int timer_idx = (int) para;
    TIMERG0.hw_timer[0].update = 1;
    //intset++;
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
    printf("Timer initialized\n");
    double timerval = 0;
    while(1){
        timer_get_counter_time_sec(TIMER_GROUP_0,TIMER_0,&timerval);
        //printf("current time value: %lf %i\n",timerval,intset);
        //printf("Rev dist: %f ; Step dist: %f\n", ONEREV_DIST, ONESTEP_DIST);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}

int32_t winch_steps = 0;  //how many steps to take
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



/**
 * @brief Beeps a number of times; defined from the variable "numtobeep"
 * 
 * @param pv not used
 */
void beepTask(void* pv){
    uint32_t numtobeep = 0;
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
            vTaskDelay(100/portTICK_PERIOD_MS);
            gpio_set_level(P_BEEPER, 0);
            vTaskDelay(100/portTICK_PERIOD_MS);
            numtobeep--;
        }else{
            vTaskDelay(1000/portTICK_PERIOD_MS);
            xTaskNotifyWait(0xFFFFFFFF,0,&numtobeep,portMAX_DELAY); //waits to get a new beep value
            printf("received notification value: %i",numtobeep);
            //call with this:
        }
    }
}

void pingTask(void* pv){
    for(;;){
    if(ping){
        publish_u8("pong",ping);
        ping = 0;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
    }
}
