#include "cyrill_test.h"

void crane_task(void *pyParameter)
{
    crane_init(P_SERVO);
    vTaskDelay(1000 / portTICK_PERIOD_MS);

    while(1)
    {
        crane_set_position(CRANE_POSITION_RETRACTED, CRANE_SPEED_SLOW);
        vTaskDelay(2000 / portTICK_PERIOD_MS);

        crane_set_position(CRANE_POSITION_EXTENDED, CRANE_SPEED_FAST);
        vTaskDelay(2000 / portTICK_PERIOD_MS);

        crane_set_position(CRANE_POSITION_LOCKED, CRANE_SPEED_SLOW);
        vTaskDelay(1000 / portTICK_PERIOD_MS);   
    }
}

void imu_task(void* pyParameter)
{
    int16_t sensor_value[NOF_IMU_PARAMS];
    imu_init(IMU_SCL_PIN, IMU_SDA_PIN);
    imu_calibrate();
    while(1)
    {
        imu_data(sensor_value);
        printf("Accelerometer X: \t%i\n", sensor_value[ACCEL_X]);
        printf("Accelerometer Y: \t%i\n", sensor_value[ACCEL_Y]);
        printf("Accelerometer Z: \t%i\n", sensor_value[ACCEL_Z]);
        printf("Temperature: \t\t%i\n", sensor_value[TEMP]);
        printf("Gyroscope X: \t\t%i\n", sensor_value[GYRO_X]);
        printf("Gyroscope Y: \t\t%i\n", sensor_value[GYRO_Y]);
        printf("Gyroscope Z: \t\t%i\n", sensor_value[GYRO_Z]);
        vTaskDelay(2000 / portTICK_RATE_MS);
    }
}

void tof_task(void* pyParameter)
{
    tof_init();
    while(1)
    {
        printf("Cube Sensor: %i  \n",tof_get_average_distance(CUBE_SENSOR,5));
        printf("Stop Sensor: %i\n",tof_get_average_distance(STOP_SIGNAL_SENSOR,5));
        vTaskDelay(100 / portTICK_RATE_MS);
    }
}