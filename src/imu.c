#include "imu.h"


int16_t offset[NOF_IMU_PARAMS];

/**
 * @get sensor data
 *
 * 1. set mode
 * _________________________________________________________________
 * | start | slave_addr + wr_bit + ack | write 1 byte + ack  | stop |
 * --------|---------------------------|---------------------|------|
 * 2. wait more than 24 ms
 * 3. read data
 * ______________________________________________________________________________________
 * | start | slave_addr + rd_bit + ack | read byte 1 + ack  |...| read byte n + nack | stop |
 * --------|---------------------------|--------------------|--------------------|------|
 */
static esp_err_t i2c_get_sensor_data(i2c_port_t i2c_num, uint8_t* data_h, uint8_t* data_l, uint8_t register_addr, int nof_bytes)
{
    int ret;
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU6050_SENSOR_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, MPU6050_CMD_START1, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, 0x00, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        return ret;
    }

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU6050_SENSOR_ADDR << 1 | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, register_addr, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    if (ret != ESP_OK) {
        return ret;
    }

    vTaskDelay(30 / portTICK_RATE_MS);
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, MPU6050_SENSOR_ADDR << 1 | READ_BIT, ACK_CHECK_EN);
    for (int byte=0; byte<nof_bytes-1; byte++)
    {
        i2c_master_read_byte(cmd, data_h+byte, ACK_VAL);
        i2c_master_read_byte(cmd, data_l+byte, ACK_VAL);
    }
    i2c_master_read_byte(cmd, data_h+(nof_bytes-1), ACK_VAL);
    i2c_master_read_byte(cmd, data_l+(nof_bytes-1), NACK_VAL);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(i2c_num, cmd, 1000 / portTICK_RATE_MS);
    i2c_cmd_link_delete(cmd);
    return ret;
}

uint8_t get_imu_value(uint8_t register_addr, int16_t* sensor_value, int nof_bytes)
{
    int ret;
    uint8_t sensor_data_h[nof_bytes];
    uint8_t sensor_data_l[nof_bytes];
    ret = i2c_get_sensor_data(IMU_I2C_NUM, sensor_data_h, sensor_data_l, register_addr, nof_bytes);
    if(ret == ESP_ERR_TIMEOUT) {
        printf("I2C timeout\n");
    } else if(ret == ESP_OK) {
        for(int byte=0; byte<nof_bytes; byte++)
        {
            sensor_value[byte] = (sensor_data_h[byte] << 8 | sensor_data_l[byte]);
        }
        return 1;
    } else {
        printf("%s: No ack, sensor not connected...skip...\n", esp_err_to_name(ret));
    }
    return 0;
}

void imu_init(int scl_pin, int sda_pin)
{
    int i2c_master_port = IMU_I2C_NUM;
    i2c_config_t conf;
    conf.mode = I2C_MODE_MASTER;
    conf.sda_io_num = sda_pin;
    conf.sda_pullup_en = GPIO_PULLUP_ENABLE;
    conf.scl_io_num = scl_pin;
    conf.scl_pullup_en = GPIO_PULLUP_ENABLE;
    conf.master.clk_speed = I2C_EXAMPLE_MASTER_FREQ_HZ;
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode,
                       I2C_EXAMPLE_MASTER_RX_BUF_DISABLE,
                       I2C_EXAMPLE_MASTER_TX_BUF_DISABLE, 0);
}

void imu_data(int16_t* sensor_data)
{
    get_imu_value(MPU6050_ACCEL_XOUT_H, sensor_data, NOF_IMU_PARAMS);
    for(int param_counter=0; param_counter<NOF_IMU_PARAMS; param_counter++)
    {
        sensor_data[param_counter]-=offset[param_counter];
    }
}

void imu_calibrate()
{
    int16_t sensor_value[NOF_IMU_PARAMS];
    int32_t data[NOF_IMU_PARAMS]={0};
    for(int cycle_counter=0; cycle_counter<IMU_CALIBRATION_CYCLES; cycle_counter++)
    {
        imu_data(sensor_value);
        for(int param_counter=0; param_counter<NOF_IMU_PARAMS; param_counter++)
        {
            data[param_counter]+=sensor_value[param_counter];
        }
    }
    for(int param_counter=0; param_counter<NOF_IMU_PARAMS; param_counter++)
    {
        offset[param_counter]=data[param_counter]/IMU_CALIBRATION_CYCLES;
        printf("%i: %i\n",param_counter, offset[param_counter]);
    }
    printf("IMU calibration done.\n");
}