#ifndef MOTCONTROLLER_H_
#define MOTCONTROLLER_H_

#include "pindef.h"


void senseISR(void*);
#define MC_PWMPIN       P_MOTORPWM
#define MC_SENSE        P_MOTORSENSE
#define MC_FORWARD      P_MOTORFORWARD
#define MC_REVERSE      P_MOTORREVERSE

#define C_TIMERG        TIMER_GROUP_0
#define C_TIMER         TIMER_0

#define C_MCPWMUNIT     MCPWM_UNIT_0
#define C_MCPWMTIMER    MCPWM_TIMER_0

#define STEPS_PER_REV   (12*4.4f)
#define PERIOD_IN_RPM(per)   ((60.0f)/(per*STEPS_PER_REV))
#define DUTY_STEP       (5)
#define RPM_TOLERANCE   (20)
#define WHEEL_RADIUS    (17.5) //in millimetres
#define ONEREV_DIST     (2.0f*3.14f*WHEEL_RADIUS)
#define ONESTEP_DIST    (ONEREV_DIST/STEPS_PER_REV)
#define SPEED_IN_RPM(spd)    (ONEREV_DIST/spd*1000)
#define RPM_IN_M_S(rpm) (rpm/60 * ONEREV_DIST / 1000)

#define MOTOR_FORWARD() gpio_set_level(MC_FORWARD, 0); gpio_set_level(MC_REVERSE, 1);
#define MOTOR_REVERSE() gpio_set_level(MC_FORWARD, 1); gpio_set_level(MC_REVERSE, 0);
#define MOTOR_BRAKE()   gpio_set_level(MC_FORWARD, 1); gpio_set_level(MC_REVERSE, 1);
#define MOTOR_COAST()   gpio_set_level(MC_FORWARD, 0); gpio_set_level(MC_REVERSE, 0);

void motCntrlTask(void* pv);
void motStepping(void* pv);
void motPulse(void* pv);
void setRPM(int32_t rpm);
int32_t* getRPMref(void);
void disableMotorControl(void);
void enableMotorControl(void);


typedef enum
{
    FORWARD = 0,
    REVERSE
} motDir_t;
void setMotDir(motDir_t t);
typedef struct {
    int32_t currRPM;
    int32_t prevRPM;
    int32_t targetRPM;
    int32_t error;
    double integral;
    float kp;
    float ki;
    float pwm;
    motDir_t direction;
} pid_control_t;


#endif //MOTCONTROLLER_H