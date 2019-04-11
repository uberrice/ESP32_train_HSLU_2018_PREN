#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"
#include "esp_types.h"
#include <mcp.h>
#include <wrpTimer.h>
#include "driver/timer.h"
#include "driver/mcpwm.h"
#include "driver/gpio.h"
#include "motController.h"

#define FLAG_DEBUG (1)

static double period = 10;
static double oldtime = 0;
static int32_t targetRPM = 0;
int32_t intrig = 0;

/**
 * @brief ISR for the MC_SENSE input pin, used to set the time passed since the last trigger
 * to a variable.
 * 
 * @param pv unused
 */
void IRAM_ATTR senseISR(void* pv){
    double newtime;
    timer_get_counter_time_sec(C_TIMERG,C_TIMER,&newtime);
    period = newtime - oldtime;
    oldtime = newtime;
    intrig++;
}


/**
 * @brief Initializes the Pololu HP 12V 4.4:1 Metal Gearmotor 25Dx48L (part no. 3213)
 * for use with the controller TB67H420FTG Dual/Single Motor Driver Carrier (part no. 2999)
 * Pin Parameters are set in the pindef.h file
 */
static void motorInit(void){
    printf("INITIALIZING MOTOR CONTROL...\n");
    //Configure MC_SENSE Interrupt input pin
    gpio_install_isr_service(0);
    gpio_config_t io_conf;
    io_conf.intr_type = GPIO_INTR_POSEDGE;
    io_conf.mode = GPIO_MODE_INPUT;
    io_conf.pin_bit_mask = 1ULL<<MC_SENSE;
    io_conf.pull_down_en = 1;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    // gpio_pad_select_gpio(MC_SENSE);
    // gpio_set_direction(MC_SENSE, GPIO_MODE_INPUT);
    gpio_isr_handler_add(MC_SENSE,senseISR,(void*)MC_SENSE);


    //Configure forward/reverse pins for motor controller
    io_conf.intr_type = GPIO_INTR_DISABLE;
    io_conf.mode = GPIO_MODE_OUTPUT;
    io_conf.pin_bit_mask = 1ULL<<MC_FORWARD | 1ULL<<MC_REVERSE;
    io_conf.pull_down_en = 0;
    io_conf.pull_up_en = 0;
    gpio_config(&io_conf);
    MOTOR_BRAKE(); //initially brakes the motor

    //Configure motor
    mcpwm_example_config();
}


/**
 * @brief sets motor target RPM
 * @param rpm RPM which will get set
 * */
void setRPM(int16_t rpm){
    targetRPM = rpm;
}


/**
 * @brief returns the current RPM value
 * 
 * @return int32_t current RPM value
 */
int32_t getRPM(void){
    return PERIOD_IN_RPM(period);
}


/**
 * @brief Task intended to be used with FreeRTOS that implements a simple PI-Loop
 * using a pid_control_t data structure.
 * 
 * @param pv unused
 */
void motCntrlTask(void* pv){
    motorInit();
    pid_control_t* pid = pvPortMalloc(sizeof(pid_control_t));
    pid->kp = (1.0f)/(1500.0f); //experimental KP, assuming 100% duty cycle when doing a 1500RPM jump
    pid->ki = 0.01f; //purely experimental KI, set to 0 to disable
    targetRPM = 400;
    while(1){
        pid->targetRPM = targetRPM;
        pid->currRPM = PERIOD_IN_RPM(period); // TODO: if it errors, put in conditional that puts current RPM to zero if period is fast enough
        pid->error = pid->currRPM - pid->targetRPM;

        pid->pwm = (pid->error * pid->kp) + (pid->integral * pid->ki);

        //Motor direction controller
        if(pid->pwm >= 0.0f){
            //Bounds for PWM
            if(pid->pwm > 100.0f) pid->pwm = 100.0f;
            MOTOR_FORWARD();
        }else if(pid->pwm < 0.0f){
            //Bounds for PWM
            if(pid->pwm < -100.0f) pid->pwm = -100.0f;
            pid->pwm = -(pid->pwm);
            MOTOR_REVERSE();
        }

        //Sets the duty cycle for the PWM unit
        mcpwm_set_duty(C_MCPWMUNIT,C_MCPWMTIMER,MCPWM_OPR_A,pid->pwm);


        //adds up the integral error and the current RPM
        pid->prevRPM = pid->currRPM;
        pid->integral += pid->error;
        #if FLAG_DEBUG
        printf("Motor params: pwm: %2f, period: %2f i: %2f, current rpm: %i, target rpm %i, intrig: %i\n", pid->pwm, period, pid->integral, pid->currRPM, pid->targetRPM,intrig);
        #endif
        vTaskDelay(500 / portTICK_PERIOD_MS);
    }
    vPortFree(pid);
}