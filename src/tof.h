#include "uart_arduino.h"

typedef enum {CUBE_SENSOR=1, STOP_SIGNAL_SENSOR=2, IMU=3} sensor_t;

//Get distance from single measurement
int tof_get_distance(sensor_t);

//Get distance from multiple measurements
int tof_get_average_distance(sensor_t, int);

//Initialize tof functions
void tof_init();