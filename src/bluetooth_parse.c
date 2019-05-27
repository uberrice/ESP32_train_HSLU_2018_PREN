#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "taskhandles.h"

void btParse(char* buf){
    if(strcmp("Hello",buf) == 0){
    xTaskNotify(beepHandle,3,eSetValueWithOverwrite);
    }
}