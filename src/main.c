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
#include <telemetry_core.h>
#include <wrpTimer.h>
#include "esp_types.h"
#include "driver/periph_ctrl.h"
#include "soc/timer_group_struct.h"
#include <motController.h>
#include "pindef.h"
#include "motController.h"

#include "bluetooth.h"

#include "tasks.h"
#include "taskhandles.h"

#include "cyrill_test.h"

// void timerAlarmSet(void){
//     //timer_isr_register(TIMER_GROUP_0,TIMER_0,t00isr,(void*)TIMER_0,ESP_INTR_FLAG_IRAM,NULL);
//     timerInit00();
//     tEnableAlarm00();
//     tInterruptEnable00();
//     timer_isr_register(TIMER_GROUP_0,TIMER_0,timer_group0_isr,(void*)TIMER_0,ESP_INTR_FLAG_IRAM,NULL);
//     //tTriggerinMs00(1000);
// }


void app_main()
{
    beepHandle = malloc(sizeof(xTaskHandle));
    cubeHandle = malloc(sizeof(xTaskHandle));
    stopHandle = malloc(sizeof(xTaskHandle));
    motCtrlHandle = malloc(sizeof(xTaskHandle));
    xTaskCreate(beepTask,"beepTask", 4096, NULL, 5, &beepHandle);
    xTaskCreate(motCntrlTask, "motCntrlTask", 8192, NULL, 5, &motCtrlHandle);
    xTaskCreate(timerInitTask,"timerInitTask", 4096, NULL, 5, NULL); //needed for motor control
    xTaskCreate(winchTask, "winchTask", 4096, NULL, 5, NULL);
    xTaskCreatePinnedToCore(teleUpdateTask, "teleUpdateTask", 4096, NULL, 4, NULL, 1);
    // xTaskCreatePinnedToCore(helloSender, "helloSender", 4096, NULL, 5, NULL, 1); //sends an incrementing number on topic helloWorldTopic every 500ms
    xTaskCreate(pingTask,"pingTask",4096, NULL, 4, NULL);

//TODO: Update priorities; teleUpdate should have a relatively low one



//Code Cyrill
    // init_cyrill();
    // xTaskCreate(motCntrlTask, "motCntrlTask", 8192, NULL, 5, NULL);
    // xTaskCreate(crane_task, "crane_task", 4096, NULL, 4, NULL);
    //xTaskCreate(imu_task, "imu_task", 4096, NULL, 5, NULL);
    //xTaskCreate(tof_task, "tof_task", 4096, NULL, 5, NULL);
    bt_start();
}