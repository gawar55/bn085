#ifndef BNO085_HPP
#define BNO085_HPP

#include <driver/gpio.h>
#include "driver/uart.h"

#define TAG "BNO085_UART"

#define UART_NUM UART_NUM_1
#define TX_PIN GPIO_NUM_17
#define RX_PIN GPIO_NUM_18
// int BUF_SIZE = 1024;

class bno{
public:
    
    uint8_t data[512];
    bno(gpio_num_t TX_PIN,gpio_num_t RX_PIN,int BUF_SIZE,uart_port_t UART_NUM); 
    void read(uint8_t* data , size_t buffer_size);

};

#endif //BNO085_HPP