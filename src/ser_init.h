#ifndef SER_INIT_H_
#define SER_INIT_H_

#include "pindef.h"
#define USED_UART_NUM UART_NUM_2
#define UART_TX_PIN P_UART_TX
#define UART_RX_PIN P_UART_RX
#define SER_BAUDRATE 115200

void tel_init(void* pv);

#endif