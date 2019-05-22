#include "cyrill_test.h"
#include "motController.h"

#define WINCH_STEPS_MAX 3850

extern int32_t winch_steps;

void init_cyrill()
{
    gpio_pad_select_gpio(P_LEDRED);
    gpio_set_direction(P_LEDRED, GPIO_MODE_OUTPUT);

    gpio_set_level(P_LEDRED, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    tof_init();
    crane_init(P_SERVO);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    gpio_set_level(P_LEDRED, 1);
}

void stop_task(void *pyParameter)
{
    int distance=0;
    setMotDir(REVERSE);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    setRPM(200);
    do
    {
        distance=tof_get_average_distance(STOP_SIGNAL_SENSOR,1);
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }while(distance==0||distance>160);
    //MOTOR_BRAKE();
    setRPM(0);
    vTaskDelay(10000 / portTICK_PERIOD_MS);
    vTaskDelete(NULL); 
}

void crane_task(void *pyParameter)
{
    int distance=0;
    setMotDir(REVERSE);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    setRPM(160);
    mark1:
    do
    {
        distance=tof_get_average_distance(CUBE_SENSOR,1);
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }while(distance==0||distance>150);
    //MOTOR_BRAKE();
    setRPM(0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    distance=tof_get_average_distance(CUBE_SENSOR,5);
    if(distance==0||distance>150) goto mark1;



    // vTaskDelay(200 / portTICK_PERIOD_MS);
    // setMotDir(FORWARD);
    // setRPM(200);
    // vTaskDelay(500 / portTICK_PERIOD_MS);
    // setRPM(0);

    crane_set_position(CRANE_POSITION_EXTENDED, CRANE_SPEED_FAST);
    vTaskDelay(300 / portTICK_PERIOD_MS);
    setMotDir(REVERSE);
    setRPM(150);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    //MOTOR_BRAKE();
    setRPM(0);



    winch_steps+=WINCH_STEPS_MAX;
    while(winch_steps!=0) vTaskDelay(100 / portTICK_PERIOD_MS);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    crane_set_position(CRANE_POSITION_LOCKED, CRANE_SPEED_SLOW);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    crane_set_position(CRANE_POSITION_RETRACTED, CRANE_SPEED_SLOW);
    vTaskDelay(2000 / portTICK_PERIOD_MS);
    winch_steps-=WINCH_STEPS_MAX;
    while(winch_steps!=0) vTaskDelay(100 / portTICK_PERIOD_MS);
    crane_set_position(CRANE_POSITION_LOCKED, CRANE_SPEED_SLOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    setRPM(150);
    vTaskDelay(20000 / portTICK_PERIOD_MS);
    vTaskDelete(NULL);          //end task

    // while(1)
    // {
    //     crane_set_position(CRANE_POSITION_EXTENDED, CRANE_SPEED_FAST);
    //     vTaskDelay(2000 / portTICK_PERIOD_MS);
    //     winch_steps+=WINCH_STEPS_MAX;
    //     while(winch_steps!=0) vTaskDelay(100 / portTICK_PERIOD_MS);
    //     vTaskDelay(2000 / portTICK_PERIOD_MS);
    //     winch_steps-=WINCH_STEPS_MAX;
    //     while(winch_steps!=0) vTaskDelay(100 / portTICK_PERIOD_MS);
    // }

    // while(1)
    // {
    //     crane_set_position(CRANE_POSITION_RETRACTED, CRANE_SPEED_SLOW);
    //     vTaskDelay(2000 / portTICK_PERIOD_MS);

    //     crane_set_position(CRANE_POSITION_EXTENDED, CRANE_SPEED_FAST);
    //     vTaskDelay(2000 / portTICK_PERIOD_MS);

    //     crane_set_position(CRANE_POSITION_LOCKED, CRANE_SPEED_SLOW);
    //     vTaskDelay(1000 / portTICK_PERIOD_MS);   
    // }
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
        //printf("Cube Sensor: %i  \n",tof_get_average_distance(CUBE_SENSOR,5));
        printf("Stop Sensor: %i\n",tof_get_average_distance(STOP_SIGNAL_SENSOR,1));
        vTaskDelay(100 / portTICK_RATE_MS);
    }
}