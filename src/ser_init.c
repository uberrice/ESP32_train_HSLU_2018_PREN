#include <stdio.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "telemetry/headers/telemetry_core.h"
#include "driver/uart.h"
#include "ser_init.h"
    
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
    uart_param_config(USED_UART_NUM, &uart_config);
    uart_set_pin(USED_UART_NUM, UART_TX_PIN, UART_RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

int32_t uart_myread(uint8_t* buf, uint32_t sizeToRead){
    return uart_read_bytes(USED_UART_NUM,buf,sizeToRead,1000);
}

int32_t uart_mywrite(uint8_t* buf, uint32_t sizeToWrite){
    return uart_write_bytes(USED_UART_NUM,(char*)buf,sizeToWrite);
}

int32_t uart_myreadable(void){ //adapts uart buffered data length function to return an int32
    size_t* readable_size;
    readable_size = malloc(sizeof(size_t));
    uart_get_buffered_data_len(USED_UART_NUM, readable_size);
    return (int32_t)(*readable_size);
}

int32_t uart_mywriteable(void){
    return 0xFFFFFFFF;
}

void tel_init(void* pv){
    ser_init();
    printf("serial initialized!\n");
    TM_transport transport;
    transport.read = uart_myread;
    transport.write = uart_mywrite;
    transport.readable = uart_myreadable;
    transport.writeable = uart_mywriteable;
    printf("telemetry data structure initialized!\n");
    init_telemetry(&transport);
    printf("Exiting telemetry initialization!\n");
    vTaskDelete(NULL);
}