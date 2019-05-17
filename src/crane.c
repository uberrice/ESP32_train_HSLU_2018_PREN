#include "crane.h"

#include "servo.h"

static int current_crane_position=CRANE_POSITION_RETRACTED;

void crane_init(int pin)
{
    servo_init(pin);
    servo_set_angle(CRANE_POSITION_RETRACTED);
}

void crane_set_position(int position_degree, int speed)
{
    if(position_degree<current_crane_position)
    {
        for (int angle = current_crane_position; angle > position_degree; angle--) {
            servo_set_angle(angle);            //printf("pulse width: %dus\n", angle);
            vTaskDelay(speed / portTICK_PERIOD_MS);     //Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation at 5V
        }
    }
    else if(position_degree>current_crane_position)
    {
        for (int angle = current_crane_position; angle < position_degree; angle++) {
            servo_set_angle(angle);            //printf("pulse width: %dus\n", angle);
            vTaskDelay(speed / portTICK_PERIOD_MS);     //Add delay, since it takes time for servo to rotate, generally 100ms/60degree rotation at 5V
        }
    }
    current_crane_position=position_degree;
}