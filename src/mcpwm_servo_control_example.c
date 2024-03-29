/* servo motor control example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_attr.h"

#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

#include "mcpwm_servo_control.h"

//You can get these value from the datasheet of servo you use, in general pulse width varies between 1000 to 2000 mocrosecond
#define SERVO_MIN_PULSEWIDTH 500 //Minimum pulse width in microsecond
#define SERVO_MAX_PULSEWIDTH 2500 //Maximum pulse width in microsecond
#define SERVO_MAX_DEGREE 90 //Maximum angle in degree upto which servo can rotate

#define BLINK_GPIO 12
#define BUTTON_GPIO 34

void mcpwm_example_gpio_initialize(int PWM_Pin)
{
    printf("initializing mcpwm servo control gpio......\n");
    mcpwm_gpio_init(MCPWM_UNIT_1, MCPWM0A, PWM_Pin);    //Set GPIO 18 as PWM0A, to which servo is connected
}

/**
 * @brief Use this function to calcute pulse width for per degree rotation
 *
 * @param  degree_of_rotation the angle in degree to which servo has to rotate
 *
 * @return
 *     - calculated pulse width
 */
uint32_t servo_per_degree_init(uint32_t degree_of_rotation)
{
    uint32_t cal_pulsewidth = 0;
    cal_pulsewidth = (SERVO_MIN_PULSEWIDTH + (((SERVO_MAX_PULSEWIDTH - SERVO_MIN_PULSEWIDTH) * (degree_of_rotation)) / (SERVO_MAX_DEGREE)));
    return cal_pulsewidth;
}

/**
 * @brief Configure MCPWM module
 */
void mcpwm_example_servo_control(void *arg)
{
    uint32_t angle, count;
    //1. mcpwm gpio initialization
    mcpwm_example_gpio_initialize(14);

    //2. initial mcpwm configuration
    printf("Configuring Initial Parameters of mcpwm......\n");
    mcpwm_config_t pwm_config;
    pwm_config.frequency = 50;    //frequency = 50Hz, i.e. for every servo motor time period should be 20ms
    pwm_config.cmpr_a = 0;    //duty cycle of PWMxA = 0
    pwm_config.cmpr_b = 0;    //duty cycle of PWMxb = 0
    pwm_config.counter_mode = MCPWM_UP_COUNTER;
    pwm_config.duty_mode = MCPWM_DUTY_MODE_0;
    mcpwm_init(MCPWM_UNIT_1, MCPWM_TIMER_0, &pwm_config);    //Configure PWM0A & PWM0B with above settings
    while (1) {
        // for (count = 0; count < SERVO_MAX_DEGREE; count++) {
        //     //printf("Angle of rotation: %d\n", count);
        //     angle = servo_per_degree_init(count);
        //     //printf("pulse width: %dus\n", angle);
        //     mcpwm_set_duty_in_us(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A, angle);
        //     vTaskDelay(40 / portTICK_PERIOD_MS);     //Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation at 5V
        // }
        // for (count = SERVO_MAX_DEGREE; count > 0; count--) {
        //     //printf("Angle of rotation: %d\n", count);
        //     angle = servo_per_degree_init(count);
        //     //printf("pulse width: %dus\n", angle);
        //     mcpwm_set_duty_in_us(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A, angle);
        //     vTaskDelay(20 / portTICK_PERIOD_MS);     //Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation at 5V
        // }
        mcpwm_set_duty_in_us(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A, 500);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        while(gpio_get_level(BUTTON_GPIO)==0);
        while(gpio_get_level(BUTTON_GPIO)!=0);        
                
        mcpwm_set_duty_in_us(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A, 1390);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        while(gpio_get_level(BUTTON_GPIO)==0);
        while(gpio_get_level(BUTTON_GPIO)!=0);

        mcpwm_set_duty_in_us(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A, 2380);
        vTaskDelay(100 / portTICK_PERIOD_MS);
        while(gpio_get_level(BUTTON_GPIO)==0);
        while(gpio_get_level(BUTTON_GPIO)!=0);
        // for(int pulsewidth=SERVO_MIN_PULSEWIDTH;pulsewidth<SERVO_MAX_PULSEWIDTH;pulsewidth+=500)
        // {
        //     mcpwm_set_duty_in_us(MCPWM_UNIT_1, MCPWM_TIMER_0, MCPWM_OPR_A, pulsewidth);
        //     vTaskDelay(100 / portTICK_PERIOD_MS);
        //     while(gpio_get_level(BUTTON_GPIO)==0);
        //     while(gpio_get_level(BUTTON_GPIO)!=0);
        //     //vTaskDelay(10/portTICK_PERIOD_MS);
        // }
    }
}

// void app_main()
// {
//     printf("Testing servo motor.......\n");
//     xTaskCreate(mcpwm_example_servo_control, "mcpwm_example_servo_control", 4096, NULL, 5, NULL);
// }