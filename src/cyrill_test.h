#include <stdio.h>
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

void crane_task(void *pyParameter);

void imu_task(void* pyParameter);

void tof_task(void* pyParameter);