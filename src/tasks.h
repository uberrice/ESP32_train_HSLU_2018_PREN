#ifndef TASKS_H_
#define TASKS_H_
#include "esp_types.h"

void helloSender(void *pvParameter);
void teleUpdateTask(void *pvParameter);
void tTestAlarmSet(void);
void timerInitTask(void* pv);
void beepTask(void*pv);
void winchTask(void* pv);
void pingTask(void* pv);

extern int32_t winch_steps;


#endif