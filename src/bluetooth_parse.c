#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "taskhandles.h"
#include "cyrill_test.h"

void btParse(char* buf){
    char* ptr;
    if(strcmp("Hello",buf) == 0){
    xTaskNotify(beepHandle,3,eSetValueWithOverwrite);
    }

    if(strncmp("beep ", buf, sizeof("beep ")-1) == 0){
        ptr = buf + sizeof("beep ") - 1;
        uint32_t b = (uint32_t)abs(atoi(ptr));
        if(b < 10){
            xTaskNotify(beepHandle,b,eSetValueWithOverwrite);
        }
    }

    if(strncmp("cube ", buf, sizeof("cube ")-1) == 0){
        ptr = buf + sizeof("beep ") - 1;
        if(strncmp("start",ptr,sizeof("start")-1) == 0){
            //start cube pickup
            xTaskCreate(crane_task, "crane_task", 4096, NULL, 4, cubeHandle);
        }
        if(strncmp("stop",ptr,sizeof("stop")-1) == 0){
            //stop cube pickup
            vTaskDelete(cubeHandle);
        }
    }
}