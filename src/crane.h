#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"


#define CRANE_POSITION_RETRACTED 142    //Crane retracted on board of the train [initial position]
#define CRANE_POSITION_EXTENDED 50      //Crane extended 90 degrees to the train
#define CRANE_POSITION_LOCKED 162       //Absolute maximum angle, holding payload in place

#define CRANE_SPEED_FAST 7              //Without payload (extending)
#define CRANE_SPEED_SLOW 23             //With payload (retracting)

//#define SERVO_SPEED_DELAY 25 //Servo step delay in ms (minimum 7->very fast, 25->slow)

void crane_init(int pin);
void crane_set_position(int position_degree, int speed);