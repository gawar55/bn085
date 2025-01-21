#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/uart.h"
#include "esp_log.h"
#include "driver/gpio.h"
#define TAG "BNO085_UART"
#define UART_NUM UART_NUM_1
#define TX_PIN GPIO_NUM_19
#define RX_PIN GPIO_NUM_18
#define BUF_SIZE (1024)

void app_main(void) {
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
    // printf("Started reading");
    while (1) {
        // Read data from BNO085
        uint16_t len = uart_read_bytes(UART_NUM, data, sizeof(data), pdMS_TO_TICKS(10));
        // for(int i;i <= len-1;i++){
        //     uint8_t actual_data = data[i];
        // }
        // printf("%02X ", data[0]);
        if (data[0] == 0xAA && data[1] == 0xAA) {
            // ESP_LOGI(TAG, "Received data: ");
            // for (int i = 0; i < len; i++) {
            //     printf("%02X ", data[0]);
            // }
            // printf(" ");
            uint8_t received_checksum = data[18]; // Assuming checksum is the last byte
            uint8_t calculated_checksum = 0;

            // Sum all bytes except the header (data[0] and data[1]) and the checksum byte (data[18])
            for (int i = 2; i < 18; i++) {
                calculated_checksum += data[i];
            }

            // Compare the calculated checksum with the received checksum
            if (received_checksum == calculated_checksum) {
                // Extract acceleration values if checksum is valid
                // int16_t yaw = (data[4] << 8) | data[3];
                // int16_t pitch = (data[6] << 8) | data[5];
                // int16_t roll = (data[8] << 8) | data[7];
                int16_t x_Acc = (data[10] << 8) | data[9];
                int16_t y_Acc = (data[12] << 8) | data[11];
                int16_t z_Acc = (data[14] << 8) | data[13];

                // float actual_yaw = yaw * 0.01f;
                // float actual_pitch = pitch * 0.01f;
                // float actual_roll = roll * 0.01f;
                float acc_x = x_Acc * (9.80665 / 1000.0f);
                float acc_y = y_Acc * (9.80665 / 1000.0f);
                float acc_z = z_Acc * (9.80665 / 1000.0f);
                
                // printf("Yaw %f\n",actual_yaw);
                // printf("pitch %f\n",actual_pitch);
                // printf("roll %f\n",actual_roll);
                printf("Acceleration in X: %f\n",acc_x);
                printf("Acceleration in Y: %f\n",acc_y);
                printf("Acceleration in Z: %f\n",acc_z);
            } else {
                printf("Checksum validation failed. Received: 0x%02X, Calculated: 0x%02X\n", received_checksum, calculated_checksum);
            }
         }

        // // Optionally, write data to BNO085
        // const char *cmd = "Your Command Here";
        // uart_write_bytes(UART_NUM, cmd, strlen(cmd));
        
        vTaskDelay(pdMS_TO_TICKS(100)); // Delay for 1 second
    }
}
