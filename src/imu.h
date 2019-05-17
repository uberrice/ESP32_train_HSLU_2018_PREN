#include <stdio.h>
#include "driver/i2c.h"

/*
Read data from MPU-6050 (IMU)
*/

#define IMU_SCL_PIN                        17               /*!< gpio number for I2C master clock */
#define IMU_SDA_PIN                        18               /*!< gpio number for I2C master data  */
#define IMU_I2C_NUM                        I2C_NUM_1        /*!< I2C port number for master dev */
#define I2C_EXAMPLE_MASTER_TX_BUF_DISABLE  0                /*!< I2C master do not need buffer */
#define I2C_EXAMPLE_MASTER_RX_BUF_DISABLE  0                /*!< I2C master do not need buffer */
#define I2C_EXAMPLE_MASTER_FREQ_HZ         100000           /*!< I2C master clock frequency */

#define WRITE_BIT                          I2C_MASTER_WRITE /*!< I2C master write */
#define READ_BIT                           I2C_MASTER_READ  /*!< I2C master read */
#define ACK_CHECK_EN                       0x1              /*!< I2C master will check ack from slave*/
#define ACK_CHECK_DIS                      0x0              /*!< I2C master will not check ack from slave */
#define ACK_VAL                            0x0              /*!< I2C ack value */
#define NACK_VAL                           0x1              /*!< I2C nack value */

//MPU6050 specific
#define MPU6050_SENSOR_ADDR                0x68
#define MPU6050_CMD_START1                 0x6B
#define MPU6050_CMD_START2                 0x00

#define MPU6050_ACCEL_XOUT_H       0x3B 
#define MPU6050_ACCEL_XOUT_L       0x3C
#define MPU6050_ACCEL_YOUT_H       0x3D
#define MPU6050_ACCEL_YOUT_L       0x3E
#define MPU6050_ACCEL_ZOUT_H       0x3F
#define MPU6050_ACCEL_ZOUT_L       0x40
#define MPU6050_TEMP_OUT_H         0x41
#define MPU6050_TEMP_OUT_L         0x42
#define MPU6050_GYRO_XOUT_H        0x43
#define MPU6050_GYRO_XOUT_L        0x44
#define MPU6050_GYRO_YOUT_H        0x45
#define MPU6050_GYRO_YOUT_L        0x46
#define MPU6050_GYRO_ZOUT_H        0x47
#define MPU6050_GYRO_ZOUT_L        0x48

#define NOF_IMU_PARAMS             7
#define ACCEL_X                    0 
#define ACCEL_Y                    1
#define ACCEL_Z                    2
#define TEMP                       3
#define GYRO_X                     4
#define GYRO_Y                     5
#define GYRO_Z                     6

#define IMU_CALIBRATION_CYCLES     10

void imu_init(int scl_pin, int sda_pin);
void imu_data(int16_t* sensor_data);
void imu_calibrate();