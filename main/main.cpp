#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "driver/gpio.h"
#include <iostream>
#include <vector>

#define TAG "BNO085_UART"

#define UART_NUM UART_NUM_1
#define TX_PIN GPIO_NUM_18
#define RX_PIN GPIO_NUM_17
#define BUF_SIZE (1024)

extern "C" void app_main(void) {
    // UART configuration
    uart_config_t uart_config = {
        .baud_rate = 115200,
        .data_bits = UART_DATA_8_BITS,
        .parity = UART_PARITY_DISABLE,
        .stop_bits = UART_STOP_BITS_1,
        .flow_ctrl = UART_HW_FLOWCTRL_DISABLE
    };
    ESP_ERROR_CHECK(uart_driver_install(UART_NUM, BUF_SIZE , 0, 0, NULL, 0));
    ESP_ERROR_CHECK(uart_param_config(UART_NUM, &uart_config));
    ESP_ERROR_CHECK(uart_set_pin(UART_NUM, TX_PIN, RX_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE));

    uint8_t data[512];

    while (1) {
        // Read data from BNO085
        int len = uart_read_bytes(UART_NUM, data, sizeof(data), pdMS_TO_TICKS(10));
        // for(int i=0;i <= len-1;i++){
        //     uint8_t actual_data = data[i];
        //     printf("%02X ", actual_data);
        // }
        for(int i=0 ; i < 512; i++){
            if(data[i] == 0xAA && data[i+1] == 0xAA){
                uint8_t received_checksum = data[i + 18]; // Assuming checksum is the last byte
                uint8_t calculated_checksum = 0;

                // Sum all bytes except the header (data[0] and data[1]) and the checksum byte (data[18])
                for (int j = i+2; j < i + 18; j++) {
                    calculated_checksum += data[j];
                }

                // Compare the calculated checksum with the received checksum
                if (received_checksum == calculated_checksum) {
                    int16_t yaw = (data[4] << 8) | data[3];
                    int16_t pitch = (data[6] << 8) | data[5];
                    int16_t roll = (data[8] << 8) | data[7];
                    int16_t x_Acc = (data[i + 10] << 8) | data[i + 9];
                    int16_t y_Acc = (data[i + 12] << 8) | data[i + 11];
                    int16_t z_Acc = (data[i + 14] << 8) | data[i + 13];

                    float actual_yaw = yaw * 0.01f;
                    float actual_pitch = pitch * 0.01f;
                    float actual_roll = roll * 0.01f;
                    float acc_x = x_Acc * (9.80665 / 1000.0f);
                    float acc_y = y_Acc * (9.80665 / 1000.0f);
                    float acc_z = z_Acc * (9.80665 / 1000.0f);
                    
                    printf("Yaw %f\n",actual_yaw);
                    printf("pitch %f\n",actual_pitch);
                    printf("roll %f\n",actual_roll);
                    printf("Acceleration in X: %f ",acc_x);
                    printf("Acceleration in Y: %f ",acc_y);
                    printf("Acceleration in Z: %f\n",acc_z);
                } else {
                    // printf("Checksum validation failed. Received: 0x%02X, Calculated: 0x%02X\n", received_checksum, calculated_checksum);
                }
            }
        }
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 1 second
    }
}
