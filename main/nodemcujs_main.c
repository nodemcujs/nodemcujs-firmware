/* nodemcu.js entry

   This example code is in the Public Domain (or CC0 licensed, at your option.)

   Unless required by applicable law or agreed to in writing, this
   software is distributed on an "AS IS" BASIS, WITHOUT WARRANTIES OR
   CONDITIONS OF ANY KIND, either express or implied.
*/

#include "include/nodemcujs_config.h"
#include "nodemcujs.h"

#include <stdio.h>
#include <string.h>

#include "freertos/FreeRTOS.h"
#include "freertos/queue.h"
#include "freertos/task.h"

#include "jerryscript.h"
#include "jerryscript-ext/handler.h"
#include "jerryscript-port.h"

#include "esp_system.h"
#include "esp_spi_flash.h"
#include "esp_heap_caps.h"
#include "esp_err.h"

#include "driver/uart.h"
#include "esp_spiffs.h"

void nodemcujs_init(void);

static QueueHandle_t uart_queue;

static void print_chip_info()
{
  size_t free_size = heap_caps_get_free_size(MALLOC_CAP_DEFAULT);
  printf("heap free: %d\n", free_size);

  /* Print chip information */
  esp_chip_info_t chip_info;
  esp_chip_info(&chip_info);
  printf("This is ESP32 chip with %d CPU cores, WiFi%s%s, ",
         chip_info.cores,
         (chip_info.features & CHIP_FEATURE_BT) ? "/BT" : "",
         (chip_info.features & CHIP_FEATURE_BLE) ? "/BLE" : "");

  printf("silicon revision %d, ", chip_info.revision);

  printf("%dMB %s flash\n", spi_flash_get_chip_size() / (1024 * 1024),
         (chip_info.features & CHIP_FEATURE_EMB_FLASH) ? "embedded" : "external");

  fflush(stdout);
}

static void uart_event_task(void *pvParameters)
{
  uart_event_t event;
  uint8_t *dtmp = (uint8_t *)malloc(INTERACTIVE_UART_RX_BUF_SIZE);
  for (;;)
  {
    // Waiting for UART event.
    if (xQueueReceive(uart_queue, (void *)&event, (portTickType)portMAX_DELAY))
    {
      bzero(dtmp, INTERACTIVE_UART_RX_BUF_SIZE);
      switch (event.type)
      {
      /** 
       * We'd better handler data event fast, there would be much more data events than
       * other types of events. If we take too much time on data event, the queue might
       * be full.
       */
      case UART_DATA:
        uart_read_bytes(INTERACTIVE_UART_NUM, dtmp, event.size, portMAX_DELAY);
        /* Setup Global scope code */
        jerry_value_t parsed_code = jerry_parse(NULL, 0, dtmp, event.size, JERRY_PARSE_NO_OPTS);

        if (!jerry_value_is_error(parsed_code))
        {
          /* Execute the parsed source code in the Global scope */
          jerry_value_t ret_value = jerry_run(parsed_code);

          /* Returned value must be freed */
          jerry_release_value(ret_value);
        }
        else
        {
          const char *ohno = "something was wrong!";
          uart_write_bytes(INTERACTIVE_UART_NUM, ohno, strlen(ohno));
        }

        /* Parsed source code must be freed */
        jerry_release_value(parsed_code);
        // free(dtmp);
        break;
      //Event of UART ring buffer full
      case UART_BUFFER_FULL:
        // If buffer full happened, you should consider encreasing your buffer size
        // As an example, we directly flush the rx buffer here in order to read more data.
        uart_flush_input(INTERACTIVE_UART_NUM);
        xQueueReset(uart_queue);
        break;
      //Others
      default:
        break;
      }
    }
  }
  free(dtmp);
  dtmp = NULL;
  vTaskDelete(NULL);
}

/**
 * Configure parameters of an UART driver, communication pins and install the driver
 * 
 * - Port: UART0
 * - Receive (Rx) buffer: on
 * - Transmit (Tx) buffer: off
 * - Flow control: off
 * - Event queue: on
 * - Pin assignment: TxD (default), RxD (default)
 */
static void handle_uart_input()
{
  uart_config_t uart_config = {
      .baud_rate = INTERACTIVE_UART_BAUDRATE,
      .data_bits = UART_DATA_8_BITS,
      .parity = UART_PARITY_DISABLE,
      .stop_bits = UART_STOP_BITS_1,
      .flow_ctrl = UART_HW_FLOWCTRL_DISABLE};
  uart_param_config(INTERACTIVE_UART_NUM, &uart_config);

  //Set UART pins (using UART0 default pins ie no changes.)
  uart_set_pin(INTERACTIVE_UART_NUM, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE, UART_PIN_NO_CHANGE);
  //Install UART driver, and get the queue.
  uart_driver_install(INTERACTIVE_UART_NUM, INTERACTIVE_UART_RX_BUF_SIZE, INTERACTIVE_UART_TX_BUF_SIZE, 20, &uart_queue, 0);

  //Create a task to handler UART event from ISR
  xTaskCreate(uart_event_task, "uart_event_task", INTERACTIVE_UART_TASK_STACK_DEPTH, NULL, 12, NULL);
}

static void mount_spiffs()
{
  esp_vfs_spiffs_conf_t conf = {
    .base_path = "",
    .partition_label = NULL,
    .max_files = 5,
    .format_if_mount_failed = true
  };

  esp_err_t ret = esp_vfs_spiffs_register(&conf);

  if (ret != ESP_OK)
  {
    if (ret == ESP_FAIL)
    {
      printf("Failed to mount or format filesystem\n");
    }
    else if (ret == ESP_ERR_NOT_FOUND)
    {
      printf("Failed to find SPIFFS partition\n");
    }
    else
    {
      printf("Failed to initialize SPIFFS (%s)\n", esp_err_to_name(ret));
    }
    return;
  }

  size_t total = 0, used = 0;
  ret = esp_spiffs_info(NULL, &total, &used);
  if (ret != ESP_OK) {
    printf("Failed to get SPIFFS partition information (%s)\n", esp_err_to_name(ret));
  } else {
    printf("Partition size: total: %d, used: %d\n", total, used);
  }

  fflush(stdout);
}

void app_main()
{
  fflush(stdout);
  // handle uart input
  handle_uart_input();
  // mount spiffs
  mount_spiffs();
  // user code init
  nodemcujs_init();
  // nodemcujs entry
  nodemcujs_entry();

  while (true)
  {
    // alive check here. but nothing to do now!
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }

  /* Cleanup engine */
  jerry_cleanup();
}

void nodemcujs_init(void)
{
#ifdef PRINT_CHIP_INFO
  print_chip_info();
#endif
}
