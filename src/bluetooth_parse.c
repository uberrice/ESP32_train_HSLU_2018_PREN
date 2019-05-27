#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "taskhandles.h"


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
}