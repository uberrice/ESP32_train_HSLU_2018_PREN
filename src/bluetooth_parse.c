#include <stdint.h>
#include <string.h>
#include <stdbool.h>
#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "taskhandles.h"
#include "cyrill_test.h"
#include "tasks.h"
#include "motController.h"

void btParse(char* buf){
    char* ptr;
    if(strcmp("Hello",buf) == 0){
    xTaskNotify(beepHandle,3,eSetValueWithOverwrite);
    }

    if(strncmp("beep ", buf, sizeof("beep ")-1) == 0){
        ptr = buf + sizeof("beep ") - 1;
        uint32_t b = (uint32_t)abs(atoi(ptr));
        if(b < 15){
            printf("beeped %i times \n", b);
            xTaskNotify(beepHandle,b,eSetValueWithOverwrite);
        }
    }

    if(strncmp("cube ", buf, sizeof("cube ")-1) == 0){
        ptr = buf + sizeof("cube ") - 1;
        if(strncmp("start",ptr,sizeof("start")-1) == 0){
            //start cube pickup
            xTaskCreate(crane_task, "crane_task", 4096, NULL, 4, &cubeHandle);
        }
        if(strncmp("stop",ptr,sizeof("stop")-1) == 0){
            //stop cube pickup
            vTaskDelete(cubeHandle);
        }
    }
    if(strncmp("stop ", buf, sizeof("stop ")-1) == 0){
        ptr = buf + sizeof("stop ") - 1;
        if(strncmp("start",ptr,sizeof("start")-1) == 0){
            //start stopping task
            xTaskCreate(stop_task, "stop_task", 4096, NULL, 4, &stopHandle);
        }
        if(strncmp("stop",ptr,sizeof("stop")-1) == 0){
            //stop stopping task
            vTaskDelete(stopHandle);
        }
    }

    if(strncmp("winch ", buf, sizeof("winch ")-1) == 0){
        ptr = buf + sizeof("winch ") - 1;
        if(strncmp("up",ptr,sizeof("up")-1) == 0){
            printf("winch up\n");
            winch_steps = -4000;
        }
        if(strncmp("down",ptr,sizeof("down")-1) == 0){
            printf("winch down\n");
            winch_steps = 4000;
        }
        if(strncmp("stop",ptr,sizeof("stop")-1) == 0){
            printf("winch stop\n");
            winch_steps = 0;
        }
    }

    if(strncmp("motor ", buf, sizeof("motor ")-1) == 0){
        ptr = buf + sizeof("motor ") - 1;
        if(strncmp("rpm ",ptr,sizeof("rpm ")-1) == 0){
            ptr = ptr + sizeof("rpm ") - 1;
            int32_t myrpm = (int32_t)atoi(ptr);
            if(0 <= myrpm && myrpm <= 2000){
                setRPM(myrpm);
                printf("Motor rpm set to %i\n",myrpm);
            }
        }
        if(strncmp("accel ",ptr,sizeof("accel ")-1) == 0){
            ptr = ptr + sizeof("accel ") - 1;
            int32_t accel = (int32_t)atoi(ptr);
            if(0 <= accel && accel <= 2000){
                setAccel(accel);
                printf("Motor accel set to %i\n",accel);
            }
        }
        if(strncmp("dir ",ptr,sizeof("dir ")-1) == 0){
            ptr = ptr + sizeof("dir ") - 1;
            if(strncmp("forward",ptr,sizeof("forward")-1) == 0){
                setMotDir(FORWARD);
                printf("Motor direction set to forward\n");
            }
            if(strncmp("backward",ptr,sizeof("backward")-1) == 0){
                setMotDir(BACKWARD);
                printf("Motor direction set to backward\n");
            }
        }
        if(strncmp("enable",ptr,sizeof("enable")-1) == 0){
            enableMotorControl();
            printf("Motor control enabled\n");
        }
        if(strncmp("disable",ptr,sizeof("disable")-1) == 0){
            disableMotorControl();
            printf("Motor control disabled\n");
        }
    }
}