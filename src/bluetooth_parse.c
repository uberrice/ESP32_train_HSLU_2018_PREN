#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "taskhandles.h"
#include "cyrill_test.h"
#include "tasks.h"

void btParse(char* buf){
    char* ptr;
    if(strcmp("Hello",buf) == 0){
    xTaskNotify(beepHandle,3,eSetValueWithOverwrite);
    }

    if(strncmp("beep ", buf, sizeof("beep ")-1) == 0){
        ptr = buf + sizeof("beep ") - 1;
        uint32_t b = (uint32_t)abs(atoi(ptr));
        if(b < 10){
            printf("beeped %i times \n", b);
            xTaskNotify(beepHandle,b,eSetValueWithOverwrite);
        }
    }

    if(strncmp("cube ", buf, sizeof("cube ")-1) == 0){
        ptr = buf + sizeof("cube ") - 1;
        if(strncmp("start",ptr,sizeof("start")-1) == 0){
            //start cube pickup
            xTaskCreate(crane_task, "crane_task", 4096, NULL, 4, cubeHandle);
        }
        if(strncmp("stop",ptr,sizeof("stop")-1) == 0){
            //stop cube pickup
            vTaskDelete(cubeHandle);
        }
    }

    if(strncmp("winch ", buf, sizeof("winch ")-1) == 0){
        ptr = buf + sizeof("winch ") - 1;
        if(strncmp("up",ptr,sizeof("up")-1) == 0){
            printf("winch up\n");
            winch_steps = 4000;
        }
        if(strncmp("down",ptr,sizeof("down")-1) == 0){
            printf("winch down\n");
            winch_steps = -4000;
        }
        if(strncmp("stop",ptr,sizeof("stop")-1) == 0){
            printf("winch stop\n");
            winch_steps = 0;
        }
    }
}