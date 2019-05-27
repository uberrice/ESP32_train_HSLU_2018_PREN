#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "taskhandles.h"

char* p;
void btParse(char* buf){
    if(strcmp("Hello",buf) == 0){
    xTaskNotify(beepHandle,3,eSetValueWithOverwrite);
    }

    if(strncmp("beep ", buf, sizeof("beep ")-1) == 0){
        p = buf + sizeof("beep ") - 1;
        uint32_t b = (uint32_t)abs(atoi(p));
        xTaskNotify(beepHandle,b,eSetValueWithOverwrite);
    }
}