#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "telemetry/headers/telemetry_core.h"
#include "driver/uart.h"
#include "ser_init.h"
    
const int uart_num = USED_UART_NUM;
//UART0 should be configured like this:
//Port 22 is RTS, whatever that is
//The other ports are labelled as RX and TX

void ser_init(void){
    // TODO: System uses UART0 for USB Comm - change this to another UART port!
    uart_config_t uart_config = {
    .baud_rate = SER_BAUDRATE,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_CTS_RTS,
    .rx_flow_ctrl_thresh = 122,
    };
    uart_param_config(uart_num, &uart_config);
}

int32_t uart_myread(uint8_t* buf, uint32_t sizeToRead){
    return uart_read_bytes(uart_num,buf,sizeToRead,1000);
}

int32_t uart_mywrite(uint8_t* buf, uint32_t sizeToWrite){
    return uart_write_bytes(uart_num,(char*)buf,sizeToWrite);
}

int32_t uart_myreadable(void){ //adapts uart buffered data length function to return an int32
    size_t* readable_size;
    readable_size = malloc(sizeof(size_t));
    uart_get_buffered_data_len(uart_num, readable_size);
    return (int32_t)(*readable_size);
}

int32_t uart_mywriteable(void){
    return 0xFFFFFFFF;
}

void tel_init(void){
    ser_init();
    TM_transport transport;
    transport.read = uart_myread;
    transport.write = uart_mywrite;
    transport.readable = uart_myreadable;
    transport.writeable = uart_mywriteable;
    init_telemetry(&transport);
}