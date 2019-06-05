#include "uart_arduino.h"

uint8_t *data;

const char* IMU_VALUES[]={"AcX","AcY","AcZ","Temp","GyX","GyY","GyZ"};

int uart_arduino_get_sensor_value(int sensor_number)
{
    if(sensor_number==1) uart_write_bytes(UART_NUM_1, (const char *)"1", 1);
    else if(sensor_number==2) uart_write_bytes(UART_NUM_1, (const char *)"2", 1);
    else if(sensor_number==3) uart_write_bytes(UART_NUM_1, (const char *)"3", 1);
    else
    {
        printf("Invalid sensor number, must be 1, 2 or 3\n");
        return -2;
    }
    
    vTaskDelay(40 / portTICK_RATE_MS);                 //Wait for Arduino to get sensor a sensor value (default: 40)
    int distance=0;

    int len = uart_read_bytes(UART_NUM_1, data, BUF_SIZE, 20 / portTICK_RATE_MS);
    if(len<=0)
    {
        //printf("No sensor data available.\n");
        return -1;
    }
    if(sensor_number==3)
    {
        int value_nr=0;
        for(int c=0;c<len;c+=2)
        {
            publish_i16(IMU_VALUES[value_nr],(data[c]<<8)|data[c+1]);
            //printf("%s: %i\n", IMU_VALUES[value_nr],(data[c]<<8)|data[c+1]);
            value_nr++;
        }
        return 0;
    }
    for(int c=0;c<len;c++)
    {
        int nof_digits=0;
        int digits[3]={0,0,0};

        for(int c2=0;data[c]!=13;c2++)
        {
            digits[c2]=data[c]-48;
            c++;
            nof_digits=c2+1;
        }
        c++;
        switch(nof_digits) {
            case 1: distance=digits[0];
            break;
            case 2: distance=digits[0]*10+digits[1];
            break;
            case 3: distance=digits[0]*100+digits[1]*10+digits[2];
            break;
        }
    }
    return distance;
}

void uart_arduino_init()
{
    // Configure a temporary buffer for the incoming data
    data = (uint8_t *) malloc(BUF_SIZE);

    int len=0;
        /* Configure parameters of an UART driver,
     * communication pins and install the driver */
    uart_config_t uart_config = {
        .baud_rate = 9600,
        .data_bits = UART_DATA_8_BITS,
        .parity    = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    uart_param_config(UART_NUM_1, &uart_config);
    uart_set_pin(UART_NUM_1, ECHO_TEST_TXD, ECHO_TEST_RXD, ECHO_TEST_RTS, ECHO_TEST_CTS);
    uart_driver_install(UART_NUM_1, BUF_SIZE * 2, 0, 0, NULL, 0);
    printf("UART connection to Arduino established.\n");
    do
    {
        uart_write_bytes(UART_NUM_1, (const char *)"a", 1);                 //Start ToF sensor init on Arduino
        vTaskDelay(500 / portTICK_RATE_MS);
        len = uart_read_bytes(UART_NUM_1, data, BUF_SIZE, 20 / portTICK_RATE_MS);
        if(data[0]=='c') printf("Cube sensor init failed (Arduino)\n");
        else if(data[0]=='s') printf("Stop sensor init failed (Arduino)\n");
        else printf("Sending ToF init command 'a' to Arduino\n");
    } while(data[0]!=1);                                                    //Arduino returns 1 if init was successful
    printf("ToF sensors initalized (Arduino)\n");
}