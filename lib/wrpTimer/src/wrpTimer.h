#ifndef WRP_TIMER_H_
#define WRP_TIMER_H_

#define TIMER_DIVIDER         16  //  Hardware timer clock divider
#define TIMER_SCALE           (TIMER_BASE_CLK / TIMER_DIVIDER * 1000)  // convert counter value to milliseconds
//at base clock assumed at 80MHz with Prescaler 16, 1 step = 0.2us

#include "driver/timer.h"

void timerInit(int timer_idx);

#endif