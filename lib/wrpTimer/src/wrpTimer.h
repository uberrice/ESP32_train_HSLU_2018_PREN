#ifndef WRP_TIMER_H_
#define WRP_TIMER_H_

#define TIMER_DIVIDER         16  //  Hardware timer clock divider
#define TIMER_SCALE           ((TIMER_BASE_CLK / TIMER_DIVIDER) / 1000)  // convert counter value to milliseconds
//at base clock assumed at 80MHz with Prescaler 16, 1 step = 0.2us
//multiply with this value to get a proper output!

#include "driver/timer.h"

void timerInit(timer_group_t tGroup, timer_idx_t tVal);
void tTriggerInUs(uint32_t micros, timer_group_t tGroup, timer_idx_t tVal);
void tTriggerInMs(uint16_t millis, timer_group_t tGroup, timer_idx_t tVal);

#define timerInitXY(grp,tmr)    timerInit(TIMER_GROUP_##grp,TIMER_##tmr)
#define timerInit00()   timerInitXY(0,0)

#define tTriggerInUsXX(micros,grp,tmr)     tTriggerInUs(micros,TIMER_GROUP_##grp,TIMER_##tmr)

#define tTriggerInMsXX(millis,grp,tmr)     tTriggerInMs(millis,TIMER_GROUP_##grp,TIMER_##tmr)

#define tEnableAlarmXX(grp,tmr)            timer_set_alarm(grp,tmr,TIMER_ALARM_EN)

#define tDisableAlarmXX(grp,tmr)           timer_set_alarm(grp,tmr,TIMER_ALARM_DIS)


//#define tTriggerInUs00(micros) tTriggerInUs(micros, TIMER_GROUP_0, TIMER_0)



#endif