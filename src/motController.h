#ifndef MOTCONTROLLER_H_
#define MOTCONTROLLER_H_

#include "pindef.h"

#define MC_PWMPIN       P_MOTORPWM
#define MC_SENSE        P_MOTORSENSE

#define C_TIMERG        TIMER_GROUP_0
#define C_TIMER         TIMER_0

#define C_MCPWMUNIT     MCPWM_UNIT_0
#define C_MCPWMTIMER    MCPWM_TIMER_0

#define STEPS_PER_REV   (12)
#define PERIOD_IN_RPM(per)   ((60*STEPS_PER_REV)/per)
#define DUTY_STEP       (5)
#define RPM_TOLERANCE   (20)

void motCntrlTask(void* pv);
void setRPM(int16_t rpm);


#endif //MOTCONTROLLER_H