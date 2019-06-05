#include "cyrill_test.h"
#include "motController.h"

#define WINCH_STEPS_MAX 4150
#define WINCH_STEPS_PART 2600
#define WINCH_STEPS_PART2 3500
#define STOP_DISTANCE_WHITE 75          //for white signs (75)
#define STOP_DISTANCE_BLACK 95          //for black signs (95)
#define STOP_DISTANCE_FAR 220
#define CUBE_DISTANCE 120               //120 or 150 ?
#define MOTOR_SLOW_DUTY 20              //duty cycle for PWM control (20)

void init_cyrill()
{
    gpio_pad_select_gpio(P_LEDRED);
    gpio_set_direction(P_LEDRED, GPIO_MODE_OUTPUT);

    gpio_set_level(P_LEDRED, 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    tof_init();
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    gpio_set_level(P_LEDRED, 1);
}

void crane_task_accurate(void *pyParameter)
{
    //init_cyrill();
    int distance=0;
    crane_init(P_SERVO);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    enableMotorControl();
    setMotDir(FORWARD);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    setRPM(160);
    do
    {
        distance=tof_get_average_distance(CUBE_SENSOR,1);
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }while(distance==0||distance>CUBE_DISTANCE);
    setRPM(0);
    vTaskDelay(100 / portTICK_PERIOD_MS);
    //distance=tof_get_average_distance(CUBE_SENSOR,5);
    //if(distance==0||distance>CUBE_DISTANCE) goto mark1;
    // disableMotorControl();
    // setMotDir(BACKWARD);

    // do
    // {
    //     mcpwm_set_duty(C_MCPWMUNIT,C_MCPWMTIMER,MCPWM_OPR_A,(float)MOTOR_SLOW_DUTY);
    //     vTaskDelay(300);
    //     mcpwm_set_duty(C_MCPWMUNIT,C_MCPWMTIMER,MCPWM_OPR_A,0);
    //     distance=tof_get_average_distance(CUBE_SENSOR,3);
    //     vTaskDelay(50);
    // }while(distance!=0 && distance<CUBE_DISTANCE);
    // setMotDir(FORWARD);
    // enableMotorControl();

    //---
    setMotDir(BACKWARD);
    setRPM(150);
    do
    {
        distance=tof_get_average_distance(CUBE_SENSOR,1);
        vTaskDelay(10);
    }while(distance!=0 && distance<CUBE_DISTANCE);
    MOTOR_BRAKE();
    setRPM(0);
    setMotDir(FORWARD);
    //---

    crane_set_position(CRANE_POSITION_EXTENDED, CRANE_SPEED_FAST);
    vTaskDelay(300 / portTICK_PERIOD_MS);
    setRPM(160);
    vTaskDelay(650 / portTICK_PERIOD_MS);
    setRPM(0);



    winch_steps-=WINCH_STEPS_MAX;
    while(winch_steps!=0) vTaskDelay(100 / portTICK_PERIOD_MS);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    crane_set_position(CRANE_POSITION_LOCKED, CRANE_SPEED_SLOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    crane_set_position(CRANE_POSITION_RETRACTED, CRANE_SPEED_SLOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    winch_steps+=WINCH_STEPS_PART;
    while(winch_steps!=0) vTaskDelay(100 / portTICK_PERIOD_MS);
    crane_set_position(CRANE_POSITION_LOCKED, CRANE_SPEED_SLOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    winch_steps-=WINCH_STEPS_PART;
    while(winch_steps!=0) vTaskDelay(100 / portTICK_PERIOD_MS);
    crane_set_position(CRANE_POSITION_PART_EXTENDED, CRANE_SPEED_SLOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    winch_steps+=WINCH_STEPS_PART2;
    while(winch_steps!=0) vTaskDelay(100 / portTICK_PERIOD_MS);
    crane_set_position(CRANE_POSITION_LOCKED, CRANE_SPEED_SLOW);
    winch_steps+=WINCH_STEPS_MAX-WINCH_STEPS_PART2;
    while(winch_steps!=0) vTaskDelay(100 / portTICK_PERIOD_MS);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    distance=tof_get_average_distance(CUBE_SENSOR,10);
    while(1)
    {
        if(distance==0||distance>150) publish_u8("cube",1);
        else publish_u8("cube",0);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);          //end task
}

void crane_task(void *pyParameter)
{
    //init_cyrill();
    int distance=0;
    crane_init(P_SERVO);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    crane_set_position(CRANE_POSITION_EXTENDED, CRANE_SPEED_FAST);
    enableMotorControl();
    setMotDir(FORWARD);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    setRPM(160);
    do
    {
        distance=tof_get_average_distance(CUBE_SENSOR,1);
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }while(distance==0||distance>CUBE_DISTANCE);
    vTaskDelay(600 / portTICK_PERIOD_MS);
    setRPM(0);
    vTaskDelay(100 / portTICK_PERIOD_MS);

    winch_steps+=WINCH_STEPS_MAX;
    while(winch_steps!=0) vTaskDelay(100 / portTICK_PERIOD_MS);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    crane_set_position(CRANE_POSITION_LOCKED, CRANE_SPEED_SLOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    crane_set_position(CRANE_POSITION_RETRACTED, CRANE_SPEED_SLOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    winch_steps-=WINCH_STEPS_PART;
    while(winch_steps!=0) vTaskDelay(100 / portTICK_PERIOD_MS);
    crane_set_position(CRANE_POSITION_LOCKED, CRANE_SPEED_SLOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    winch_steps+=WINCH_STEPS_PART;
    while(winch_steps!=0) vTaskDelay(100 / portTICK_PERIOD_MS);
    crane_set_position(CRANE_POSITION_PART_EXTENDED, CRANE_SPEED_SLOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    winch_steps-=WINCH_STEPS_PART2;
    while(winch_steps!=0) vTaskDelay(100 / portTICK_PERIOD_MS);
    crane_set_position(CRANE_POSITION_LOCKED, CRANE_SPEED_SLOW);
    winch_steps-=WINCH_STEPS_MAX-WINCH_STEPS_PART2;
    while(winch_steps!=0) vTaskDelay(100 / portTICK_PERIOD_MS);

    vTaskDelay(1000 / portTICK_PERIOD_MS);
    distance=tof_get_average_distance(CUBE_SENSOR,10);
    while(1)
    {
        if(distance==0||distance>150) publish_u8("cube",0);     //Success
        else publish_u8("cube",1);                              //No success
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL);          //end task
}

void unload_task(void* pyParameter)
{
    //!!
    crane_init(P_SERVO);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    crane_set_position(CRANE_POSITION_LOCKED, CRANE_SPEED_SLOW);
    vTaskDelay(5000 / portTICK_PERIOD_MS);
    //!!

    winch_steps+=WINCH_STEPS_MAX-WINCH_STEPS_PART2;
    while(winch_steps!=0) vTaskDelay(100 / portTICK_PERIOD_MS);
    crane_set_position(CRANE_POSITION_PART_EXTENDED, CRANE_SPEED_SLOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    winch_steps+=WINCH_STEPS_PART2;
    while(winch_steps!=0) vTaskDelay(100 / portTICK_PERIOD_MS);
    crane_set_position(CRANE_POSITION_LOCKED, CRANE_SPEED_SLOW);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
    winch_steps-=WINCH_STEPS_PART;
    while(winch_steps!=0) vTaskDelay(100 / portTICK_PERIOD_MS);
    crane_set_position(CRANE_POSITION_RETRACTED, CRANE_SPEED_SLOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    winch_steps+=WINCH_STEPS_PART;
    while(winch_steps!=0) vTaskDelay(100 / portTICK_PERIOD_MS);
    crane_set_position(CRANE_POSITION_EXTENDED, CRANE_SPEED_SLOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    winch_steps-=WINCH_STEPS_MAX;
    while(winch_steps!=0) vTaskDelay(100 / portTICK_PERIOD_MS);
    crane_set_position(CRANE_POSITION_RETRACTED, CRANE_SPEED_SLOW);
    vTaskDelay(500 / portTICK_PERIOD_MS);
    vTaskDelete(NULL);
}

void stop_task(void *pyParameter)
{
    int distance=0;
    //int stop_distance=0;
    int stop_distance=90;
    //if(stopsignal==1) { stop_distance=STOP_DISTANCE_WHITE; }
    //else { stop_distance=STOP_DISTANCE_BLACK; }
    enableMotorControl();               //Using PID-control
    setMotDir(FORWARD);
    setRPM(150);
    do
    {
        distance=tof_get_average_distance(STOP_SIGNAL_SENSOR,1);
        vTaskDelay(5 / portTICK_PERIOD_MS);
    }while(distance==0||distance>STOP_DISTANCE_FAR);
    setRPM(0);
    disableMotorControl();              //Using direct PWM control
    vTaskDelay(500 / portTICK_PERIOD_MS);

    do
    {
        mcpwm_set_duty(C_MCPWMUNIT,C_MCPWMTIMER,MCPWM_OPR_A,MOTOR_SLOW_DUTY);
        vTaskDelay(100);
        mcpwm_set_duty(C_MCPWMUNIT,C_MCPWMTIMER,MCPWM_OPR_A,0);
        distance=tof_get_average_distance(STOP_SIGNAL_SENSOR,3);
        vTaskDelay(50);
    }while(distance==0||distance>stop_distance);
    gpio_set_level(P_LEDRED, 0);
    enableMotorControl();

    while(1)
    {
        publish_u8("stop",1);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    vTaskDelete(NULL); 
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
        printf("IMU: %i\n",tof_get_average_distance(IMU,1));
        vTaskDelay(100 / portTICK_RATE_MS);
    }
}