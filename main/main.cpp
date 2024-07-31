/*----------------------------------------------------------------

  Telemetry V0.8.1 main.cpp
     
  INA226
  MFRC522 
  LCD 20x4 I²c 
  Keypad 4x4 I²c 

  Compiler: VsCode 1.9.0
  MCU: ESP32
  Board: Dev module 38 pins

  Author: João  G. Uzêda & Victor Hugo
  date: 2024, June
-----------------------------------------------------------------*/

// ---------------------------------------------------------------- 
// ---Libraries---
#include "esp_system.h"
#include "esp_log.h"
#include "driver/uart.h"
#include "string.h"
#include "driver/gpio.h"
#include "unity_internals.h"
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "Arduino.h"

// ---------------------------------------------------------------- 
// ---Defines---
static const int RX_BUFFER_SIZE = 128;
#define   UART_NUM  UART_NUM_2

// ---------------------------------------------------------------- 
// ---Pins---
#define   TXD_PIN   17
#define   RXD_PIN   16

// ----------------------------------------------------------------
// ----init----
void init(void){
  const uart_config_t uart_config = {
    .baud_rate = 115200,
    .data_bits = UART_DATA_8_BITS,
    .parity = UART_PARITY_DISABLE,
    .stop_bits = UART_STOP_BITS_1,
    .flow_ctrl = UART_HW_FLOWCTRL_DISABLE,
    .source_clk = UART_SCLK_DEFAULT,
  };
    // We won't use a buffer for sending data.
  uart_driver_install(UART_NUM, RX_BUFFER_SIZE * 2, 0, 0, NULL, 0);
  uart_param_config(UART_NUM, &uart_config);
  uart_set_pin(UART_NUM, TXD_PIN, RXD_PIN, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
}

// ----------------------------------------------------------------
// ----rx task----
static void tx_task(void *arg){
  int num = 0;
  uint8_t *data = (uint8_t*) malloc(30);
  while(1){
    int len = sprintf((char *)data, "Hello World %d\n", num++);
    uart_write_bytes(UART_NUM, data, len);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  free(data);
}

// ----------------------------------------------------------------
// ----rx_task----
static void rx_task(void *arg){
  static const char *RX_TASK_TAG = "RX_TASk";
  esp_log_level_set(RX_TASK_TAG, ESP_LOG_INFO);
  uint8_t* data = (uint8_t*) malloc(RX_BUFFER_SIZE + 1);
  while(1){
    const int rxBytes = uart_read_bytes(UART_NUM, data, RX_BUFFER_SIZE, 200 / portTICK_PERIOD_MS);
    if (rxBytes > 0){
      data[rxBytes] = 0;
      ESP_LOGI(RX_TASK_TAG, "Read %d bytes: '%s'", rxBytes, data);  
    }
  }
  free(data);  
}

// ----------------------------------------------------------------
// ----main----
extern "C" void app_main(){
  init();
  xTaskCreate(rx_task, "uart_rx_task", 2048, NULL, configMAX_PRIORITIES - 1, NULL);
  xTaskCreate(tx_task, "uart_tx_task", 2048, NULL, configMAX_PRIORITIES - 2, NULL);
} 