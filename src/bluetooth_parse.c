#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void btParse(char* buf){
    printf("Result of thingy: %i\n",strcmp("Hello",buf));
}