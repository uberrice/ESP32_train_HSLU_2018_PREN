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
#include "taskhandles.h"

#define FLAG_DEBUG (0)
#define FLAG_PER_TIMEOUT (1)
#define FLAG_STALL_STOP (1) //attention; only works if FLAG_PER_TIMEOUT is also set

static double period = 10;
static double oldtime = 0;
int32_t targetRPM = 0;
int32_t accel = 5;
int32_t intrig = 0;
uint8_t controlEnable = 0;
motDir_t motdir = FORWARD;

void setAccel(int32_t a){
    accel = a;
}

void setMotDir(motDir_t t){
    motdir = t;
    if(t == FORWARD){
        MOTOR_FORWARD();
    } else if(t == BACKWARD){
        MOTOR_BACKWARD();
    } else{
        MOTOR_BRAKE();
    }
}
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
void setRPM(int32_t rpm){
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

int32_t* getRPMref(void){
    return (&targetRPM);
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
    pid->ki = 0.04f; //purely experimental KI, set to 0 to disable
    pid->integral = 0;
    targetRPM = 0; //CYRILL: Hier werden die target RPM initialisiert
    vTaskDelay(3000/portTICK_PERIOD_MS);
    while(1){
        if (pid->targetRPM != targetRPM){
            pid->integral = 0;
        }
        if (pid->targetRPM < targetRPM){
            if (pid->targetRPM+accel > targetRPM){
                pid->targetRPM = targetRPM;
            } else{
                pid->targetRPM += accel;
            }
        } else {
            pid->targetRPM = targetRPM;
        }
        pid->currRPM = PERIOD_IN_RPM(period); // TODO: if it errors, put in conditional that puts current RPM to zero if period is fast enough
        /*if (pid->currRPM > 500){
            pid->currRPM = 500;
        }*/
        pid->error = pid->targetRPM - pid->currRPM;

        pid->pwm = (pid->error * pid->kp) + (pid->integral * pid->ki);
        // pid->pwm = (pid->error * pid->kp);
        // pid->pwm = 15;
        //Motor direction controller
        if(controlEnable){

        if(pid->pwm >= 0.0f){
            //Bounds for PWM
            if(pid->pwm > M_MAXPWM) pid->pwm = M_MAXPWM;
            if(motdir == FORWARD){
                MOTOR_FORWARD();
            } else{
                MOTOR_BACKWARD();
            }
        } 
        else if(pid->pwm < 0.0f){
            //Bounds for PWM
            //if(pid->pwm < -M_MAXPWM) pid->pwm = -M_MAXPWM;

            // pid->pwm = -(pid->pwm);
            // MOTOR_BACKWARD();

            //debug: set pid to 0 if pwm is negative
            pid->pwm = 0;
            MOTOR_BRAKE();
        }

        //Sets the duty cycle for the PWM unit
        mcpwm_set_duty(C_MCPWMUNIT,C_MCPWMTIMER,MCPWM_OPR_A,pid->pwm);
        } else{
            pid->integral = 0;
        }


        //adds up the integral error and the current RPM
        pid->prevRPM = pid->currRPM;
        pid->integral += pid->error;
        #if FLAG_PER_TIMEOUT
        double mytime = 0;
        timer_get_counter_time_sec(C_TIMERG,C_TIMER,&mytime);
        if((mytime - oldtime) > M_TIMEOUT){
            period = M_TIMEOUT;
            #if FLAG_STALL_STOP
            if(targetRPM != 0) disableMotorControl();
            #endif
        }
        #endif
        #if FLAG_DEBUG
        outputtim++;
        if(outputtim == 200){ //CYRILL: Multiplier für tasks hier; 2 -> alle 10 millisekunden; 10 -> alle 50 etc.
            itervar++;
            printf("Motor params: pwm: %2f, period: %2f error: %i, current rpm: %i, target rpm %i, intrig: %i\n integral: %2f, dist since last: %2f mm\n"
            , pid->pwm, period, pid->error, pid->currRPM, pid->targetRPM,intrig-oldintrig, pid->integral, ((intrig-oldintrig)*ONESTEP_DIST));
            oldintrig = intrig;
            // targetRPM+=100;
            if(targetRPM < 1100){ //CYRILL: Hier werden die RPM höher geschaltet bis sie 1400 erreichen
                targetRPM+=5;
            }else{
                targetRPM=0;
            }
            outputtim=0;
        }
        #endif
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }
    vPortFree(pid);
}

void disableMotorControl(void){
    controlEnable = 0;
    mcpwm_set_duty(C_MCPWMUNIT,C_MCPWMTIMER,MCPWM_OPR_A,0);
}

void enableMotorControl(void){
    controlEnable = 1;
}