#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/queue.h"


#include "testing.h"
#include "motController.h"
#include "cyrill_test.h"

mystates_t currState = START;

void test_task(void* pv){
    for(;;){
    switch(currState){
        case START:
            xTaskCreate(crane_task, "crane_task", 4096, NULL, 4, NULL);
            currState = PICKUP;
        break;
        case PICKUP:
        break;
        case RUN:
            enableMotorControl();
            setMotDir(FORWARD);
            setRPM(700);
            vTaskDelay(pdMS_TO_TICKS(23000));
            setRPM(0);
            disableMotorControl();
            setMotDir(BRAKE);
            vTaskDelay(pdMS_TO_TICKS(3000));
            currState = STOP;
        break;
        case STOP:
            xTaskCreate(stop_task, "stop_task", 4096, NULL, 5, NULL);
            currState = STOPPING;
        break;
        case STOPPING:
        break;
        case DONE:
        break;
    }
    vTaskDelay(pdMS_TO_TICKS(100));
    }
}