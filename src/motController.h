#ifndef MOTCONTROLLER_H_
#define MOTCONTROLLER_H_

#include "pindef.h"

#define MC_PWMPIN       P_MOTORPWM
#define MC_SENSE        P_MOTORSENSE

#define C_TIMERG        TIMER_GROUP_0
#define C_TIMER         TIMER_0

#define C_MCPWMUNIT     MCPWM_UNIT_0
#define C_MCPWMTIMER    MCPWM_TIMER_0

#define STEPS_PER_REV   12
#define PERIOD_IN_RPM   (60*STEPS_PER_REV)
#define RPM_IN_PERIOD   (1/PERIOD_IN_RPM)
#define DUTY_STEP       5
#define RPM_TOLERANCE   20



#endif //MOTCONTROLLER_H