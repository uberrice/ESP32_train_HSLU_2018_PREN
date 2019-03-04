/* Blink Example

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "mcp.h"
#include "driver/mcpwm.h"
#include "ser_init.h"
#include "telemetry/headers/telemetry_core.h"

/* Can run 'make menuconfig' to choose the GPIO to blink,
   or you can edit the following line and set a number here.
*/
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
    int8_t ramp_cntr = 1;
    bool dir = true;
    while(1) {
        if(ramp_cntr == 0){
            dir = true;
        }
        if(ramp_cntr == 100){
            dir = false;
        }



        mcpwm_set_duty(0,0,MCPWM_OPR_A,(float)ramp_cntr);
        vTaskDelay(50 / portTICK_PERIOD_MS);

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
    vTaskDelay(500 / portTICK_PERIOD_MS);
    ctr++;
}


void app_main()
{
    tel_init();
    mcpwm_example_config();
    //xTaskCreate(mcpwm_example_config, "mcpwm_example_config", 4096, NULL, 5, NULL);
    xTaskCreate(blink_task, "blink_task", 4096, NULL, 5, NULL);
    xTaskCreate(ramp_task, "ramp_task", 4096, NULL, 5, NULL);
    xTaskCreate(helloSender, "helloSender", 4096, NULL, 5, NULL);

}



/* void app_main()
{
    printf("Testing MCPWM...\n");
    cap_queue = xQueueCreate(1, sizeof(capture)); //comment if you don't want to use capture module
    xTaskCreate(disp_captured_signal, "mcpwm_config", 4096, NULL, 5, NULL);  //comment if you don't want to use capture module
    xTaskCreate(gpio_test_signal, "gpio_test_signal", 4096, NULL, 5, NULL); //comment if you don't want to use capture module
    xTaskCreate(mcpwm_example_config, "mcpwm_example_config", 4096, NULL, 5, NULL);
} */