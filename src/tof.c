#include "tof.h"

int tof_get_distance(sensor_t sensor)
{
    int distance=0;
    int measurements_counter=0;
    do
    {
        distance = uart_arduino_get_sensor_value((int)sensor);
        if(measurements_counter>=20)
        {
            printf("ToF sensor data unavailable.\n");
            return -1;
        }
        else measurements_counter++;
    } while (distance==-1);
    return distance;
}

int tof_get_average_distance(sensor_t sensor, int nof_measurements)
{
    int average_distance=0;
    int no_object_counter=0;
    for(int measurement_counter=0;measurement_counter<nof_measurements;measurement_counter++)
    {
        int measured_distance=tof_get_distance(sensor);
        if(measured_distance!=0)
        {
            average_distance+=measured_distance;
        }
        else no_object_counter++;
    }
    if(no_object_counter>(nof_measurements/2)) return 0;
    average_distance/=(nof_measurements-no_object_counter);
    return average_distance;
}

int tof_init()
{
    uart_arduino_init();
    return(0);
}