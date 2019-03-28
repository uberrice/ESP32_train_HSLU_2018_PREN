#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_types.h"
#include <mcp.h>
#include <wrpTimer.h>
#include "driver/timer.h"
#include "driver/mcpwm.h"
#include "motController.h"

static double period = 10;
static double oldtime = 0;
static int32_t targetRPM = 0;

void IRAM_ATTR senseISR(void* pv){
    double newtime;
    timer_get_counter_time_sec(C_TIMERG,C_TIMER,&newtime);
    period = newtime - oldtime;
    oldtime = newtime;
}

static void motorInit(void){
    printf("INITIALIZING MOTOR CONTROL...\n");
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = 1ULL<<MC_SENSE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    gpio_pad_select_gpio(MC_SENSE);
    gpio_set_direction(MC_SENSE, GPIO_MODE_INPUT);
    gpio_isr_handler_add(MC_SENSE,senseISR,(void*)MC_SENSE);
    mcpwm_example_config();
}

void setRPM(int16_t rpm){
    targetRPM = rpm;
}

int32_t getRPM(void){
    return PERIOD_IN_RPM(period);
}

void motCntrlTask(void* pv){
    motorInit();
    int32_t currentRPM = 0;
    int32_t rpmDiff = 0;
    float dCycle = 0;
    while(1){
        currentRPM = PERIOD_IN_RPM(period);
        rpmDiff = targetRPM - currentRPM;

        if(rpmDiff > RPM_TOLERANCE){
            if(dCycle < (100-DUTY_STEP)) dCycle+=DUTY_STEP;
        } else if(rpmDiff < -RPM_TOLERANCE)
        {
            if(dCycle > DUTY_STEP) dCycle-=DUTY_STEP;
        }
        mcpwm_set_duty(C_MCPWMUNIT,C_MCPWMTIMER,MCPWM_OPR_A,dCycle);
        vTaskDelay(100 / portTICK_PERIOD_MS);
    }
}