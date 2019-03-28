#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_types.h"
#include <mcp.h>
#include <wrpTimer.h>
#include "driver/timer.h"
#include "driver/mcpwm.h"
#include "motController.h"

static double period = 0;
static double oldtime = 0;
static int16_t sysRPM = 0;

void motorInit(void){
    printf("INITIALIZING MOTOR CONTROL...\n");
    gpio_pad_select_gpio(MC_SENSE);
    gpio_set_direction(MC_SENSE, GPIO_MODE_INPUT);
    mcpwm_example_config();
}

void setRPM(int16_t rpm){
    sysRPM = rpm;
}
// TODO: set this up for the MC_SENSE pin to trigger on rising edge!
void myISR(void){
    double newtime;
    timer_get_counter_time_sec(C_TIMERG,C_TIMER,&newtime);
    period = newtime - oldtime;
    oldtime = newtime;
}

void motCntrlTask(void* pv){
    motorInit();
    int16_t currentRPM = 0;
    int16_t rpmDiff = 0;
    float dCycle;
    while(1){
        currentRPM = period * PERIOD_IN_RPM;
        rpmDiff = sysRPM - currentRPM;

        if(rpmDiff > RPM_TOLERANCE){
            if(dCycle < (100-DUTY_STEP)) dCycle+=DUTY_STEP;
        } else if(rpmDiff < -RPM_TOLERANCE)
        {
            if(dCycle > DUTY_STEP) dCycle-=DUTY_STEP;
        }
        mcpwm_set_duty(C_MCPWMUNIT,C_MCPWMTIMER,MCPWM_OPR_A,dCycle);
        vTaskDelay(100 / portTICK_PERIOD_MS)
    }
}