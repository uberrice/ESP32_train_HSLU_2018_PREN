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
    double testime;
    timer_get_counter_time_sec(C_TIMERG,C_TIMER,&newtime);
    testime = newtime - oldtime;
    if(1){
        period = testime;
        oldtime = newtime;
        
    }
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

int oldintrig = 0;
int outputtim = 0;
int itervar = 0;
void motCntrlTask(void* pv){
    motorInit();
    pid_control_t* pid = pvPortMalloc(sizeof(pid_control_t));
    pid->kp = (1.0f)/(10.0f); //experimental KP, assuming 100% duty cycle when 10RPM off
    pid->ki = 0.01f; //purely experimental KI, set to 0 to disable
    pid->integral = 0;
    targetRPM = 100; //CYRILL: Hier werden die target RPM initialisiert
    while(1){
        pid->targetRPM = targetRPM;
        pid->currRPM = PERIOD_IN_RPM(period); // TODO: if it errors, put in conditional that puts current RPM to zero if period is fast enough
        /*if (pid->currRPM > 500){
            pid->currRPM = 500;
        }*/
        pid->error = pid->targetRPM - pid->currRPM;

        pid->pwm = (pid->error * pid->kp) + (pid->integral * pid->ki);
        // pid->pwm = (pid->error * pid->kp);
        // pid->pwm = 15;
        //Motor direction controller
        if(pid->pwm >= 0.0f){
            //Bounds for PWM
            if(pid->pwm > 100.0f) pid->pwm = 100.0f;
            MOTOR_FORWARD();
        } 
        else if(pid->pwm < 0.0f){
            //Bounds for PWM
            if(pid->pwm < -100.0f) pid->pwm = -100.0f;

            // pid->pwm = -(pid->pwm);
            // MOTOR_REVERSE();

            //debug: set pid to 0 if pwm is negative
            pid->pwm = 0;
            MOTOR_BRAKE();
        }

        //Sets the duty cycle for the PWM unit
        mcpwm_set_duty(C_MCPWMUNIT,C_MCPWMTIMER,MCPWM_OPR_A,pid->pwm);


        //adds up the integral error and the current RPM
        pid->prevRPM = pid->currRPM;
        pid->integral += pid->error;
        #if FLAG_DEBUG
        outputtim++;
        if(outputtim == 2){ //CYRILL: Multiplier für tasks hier; 2 -> alle 10 millisekunden; 10 -> alle 50 etc.
            itervar++;
            printf("Motor params: pwm: %2f, period: %2f error: %i, current rpm: %i, target rpm %i, intrig: %i\n integral: %2f, dist since last: %2f mm\n"
            , pid->pwm, period, pid->error, pid->currRPM, pid->targetRPM,intrig-oldintrig, pid->integral, ((intrig-oldintrig)*ONESTEP_DIST));
            oldintrig = intrig;
            // targetRPM+=100;
            if(targetRPM < 1400){ //CYRILL: Hier werden die RPM höher geschaltet bis sie 1400 erreichen
                targetRPM++;
            }
            outputtim=0;
        }
        #endif
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
    vPortFree(pid);
}