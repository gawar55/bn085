#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "driver/gpio.h"

#include <bno085.hpp>

extern "C" void app_main(void) {

    bno sensor(TX_PIN,RX_PIN,1024,UART_NUM_1);
    uint8_t data[512];
    while (1) {
        sensor.read(data,sizeof(data));
        vTaskDelay(pdMS_TO_TICKS(10)); // Delay for 1 second
    }
}
