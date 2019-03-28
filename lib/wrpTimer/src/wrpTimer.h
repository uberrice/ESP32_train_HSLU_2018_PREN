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

#define tTriggerInUsXX(micros,grp,tmr)      tTriggerInUs(micros,TIMER_GROUP_##grp,TIMER_##tmr)
#define tTriggerInUs00(micros)              tTriggerInUsXX(micros,0,0)


#define tTriggerInMsXX(millis,grp,tmr)      tTriggerInMs(millis,TIMER_GROUP_##grp,TIMER_##tmr)
#define tTriggerinMs00(millis)              tTriggerInMsXX(millis,0,0)

#define tEnableAlarmXX(grp,tmr)             timer_set_alarm(grp,tmr,TIMER_ALARM_EN)
#define tEnableAlarm00()                    tEnableAlarmXX(0,0)

#define tDisableAlarmXX(grp,tmr)            timer_set_alarm(grp,tmr,TIMER_ALARM_DIS)
#define tDisableAlarm00()                   tDisableAlarmXX(0,0)

#define tStartXX(grp,tmr)                   timer_start(TIMER_GROUP_##grp,TIMER_##tmr)
#define tStart00()                          tStartXX(0,0)


#define clearIntFlag(grp,tmr)               TIMERG##grp.int_clr_timers.t##tmr = 1
#define clearIntFlag00()                    clearIntFlag(0,0)

#define tISRRegisterXX(fun,grp,tmr)         timer_isr_register(TIMER_GROUP_##grp,TIMER_##tmr,t00isr,(void*)TIMER_##tmr,ESP_INTR_FLAG_IRAM,NULL);
#define tISRRegister00(fun)                 tISRRegisterXX(fun,0,0)

#define tInterruptEnableXX(grp,tmr)         timer_enable_intr(TIMER_GROUP_##grp,TIMER_##tmr)
#define tInterruptEnable00()                tInterruptEnableXX(0,0)

//#define tTriggerInUs00(micros) tTriggerInUs(micros, TIMER_GROUP_0, TIMER_0)

#endif