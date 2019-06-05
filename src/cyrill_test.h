#include <stdio.h>
#include <telemetry_core.h>
//#include "freertos/FreeRTOS.h"
//#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"

#include "esp_attr.h"

#include "driver/mcpwm.h"
#include "soc/mcpwm_reg.h"
#include "soc/mcpwm_struct.h"

#include "servo.h"
#include "crane.h"
#include "imu.h"
#include "tof.h"
#include "pindef.h"
#include "motController.h"

extern int32_t winch_steps;
extern uint8_t stopsignal;

void init_cyrill(void);

void stop_task(void *pyParameter);

void crane_task_accurate(void *pyParameter);

void crane_task(void *pyParameter);

void unload_task(void* pyParameter);

void imu_task(void* pyParameter);

void tof_task(void* pyParameter);