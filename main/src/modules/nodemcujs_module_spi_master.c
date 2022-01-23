#include "nodemcujs.h"
#include "nodemcujs_def.h"
#include "nodemcujs_binding.h"
#include "nodemcujs_magic_strings.h"

#include "esp_system.h"
#include "esp_attr.h"
#include "driver/gpio.h"
#include "driver/spi_master.h"

#include <string.h>

typedef struct nodemcujs_spi_device_handle {
  uint8_t pinDC;
  uint8_t activeDC;
  spi_device_handle_t spi;
} nodemcujs_spi_device_handle_t;

typedef struct nodemcujs_spi_transaction_context {
  uint8_t DC;
  uint8_t RW;
  nodemcujs_spi_device_handle_t *device;
} nodemcujs_spi_transaction_context_t;

nodemcujs_spi_device_handle_t spi2 = { -1, 1, NULL};

void IRAM_ATTR spiTransferCallbackPRE(spi_transaction_t *t) {
  nodemcujs_spi_transaction_context_t *transaction = (nodemcujs_spi_transaction_context_t*)t->user;
  gpio_set_level(transaction->device->pinDC, transaction->DC ? transaction->device->activeDC : transaction->device->activeDC ? 0 : 1);
}

void IRAM_ATTR spiTransferCallbackPOST(spi_transaction_t *t) {
  nodemcujs_spi_transaction_context_t *transaction = (nodemcujs_spi_transaction_context_t*)t->user;
  if (transaction->RW == 1) {
    free((void*)t->tx_buffer);
  }
  free(transaction);
  free(t);
}

JS_FUNCTION(SendSync) {
  int id = JS_GET_ARG(0, number);
  int dc = JS_GET_ARG(1, number);
  jerry_value_t data = JS_GET_ARG(2, array);

  jerry_value_t jlength = nodemcujs_jval_get_property(data, NODEMCUJS_MAGIC_STRING_LENGTH);
  int length = nodemcujs_jval_as_number(jlength);
  uint8_t *buffer = (uint8_t*)malloc(length);
  NODEMCUJS_ASSERT(buffer != NULL);

  for (int i = 0; i < length; i++) {
    jerry_value_t jdata = jerry_get_property_by_index(data, i);
    buffer[i] = (uint8_t)nodemcujs_jval_as_number(jdata);
    jerry_release_value(jdata);
  }
  jerry_release_value(jlength);

  spi_transaction_t *t = (spi_transaction_t*)malloc(sizeof(spi_transaction_t));
  NODEMCUJS_ASSERT(t != NULL);
  memset(t, 0, sizeof(spi_transaction_t));
  t->length = length * 8;
  t->tx_buffer = buffer;

  esp_err_t ret;
  if (id == 2) {
    nodemcujs_spi_transaction_context_t *transaction = (nodemcujs_spi_transaction_context_t*)malloc(sizeof(nodemcujs_spi_transaction_context_t));
    transaction->DC = dc;
    transaction->RW = 1;
    transaction->device = &spi2;
    t->user = (void*)transaction;
    ret = spi_device_polling_transmit(spi2.spi, t);
    if (ret != ESP_OK) {
      ESP_ERROR_CHECK(ret);
      return jerry_create_boolean(false);
    }
    return jerry_create_boolean(true);
  }
  return jerry_create_error(JERRY_ERROR_RANGE, (jerry_char_t*)"SPI Host not found");
}

JS_FUNCTION(Setup) {
  int id = JS_GET_ARG(0, number);
  jerry_value_t spiConfig = JS_GET_ARG(1, object);
  jerry_value_t jpinMOSI = nodemcujs_jval_get_property(spiConfig, "pinMOSI");
  int pinMOSI = nodemcujs_jval_as_number(jpinMOSI);
  jerry_release_value(jpinMOSI);
  jerry_value_t jpinMISO = nodemcujs_jval_get_property(spiConfig, "pinMISO");
  int pinMISO = nodemcujs_jval_as_number(jpinMISO);
  jerry_release_value(jpinMISO);
  jerry_value_t jpinCLK = nodemcujs_jval_get_property(spiConfig, "pinCLK");
  int pinCLK = nodemcujs_jval_as_number(jpinCLK);
  jerry_release_value(jpinCLK);
  jerry_value_t jmaxTransferSize = nodemcujs_jval_get_property(spiConfig, "maxTransferSize");
  int maxTransferSize = nodemcujs_jval_as_number(jmaxTransferSize);
  jerry_release_value(jmaxTransferSize);

  jerry_value_t jclockHz = nodemcujs_jval_get_property(spiConfig, "clockHz");
  int clockHz = nodemcujs_jval_as_number(jclockHz);
  jerry_release_value(jclockHz);
  jerry_value_t jmode = nodemcujs_jval_get_property(spiConfig, "mode");
  int mode = nodemcujs_jval_as_number(jmode);
  jerry_release_value(jmode);
  jerry_value_t jpinCS = nodemcujs_jval_get_property(spiConfig, "pinCS");
  int pinCS = nodemcujs_jval_as_number(jpinCS);
  jerry_release_value(jpinCS);
  jerry_value_t jpinDC = nodemcujs_jval_get_property(spiConfig, "pinDC");
  jerry_value_t jactiveDC = nodemcujs_jval_get_property(spiConfig, "activeDC");
  if (id == 2) {
    spi2.pinDC = nodemcujs_jval_as_number(jpinDC);
    spi2.activeDC = nodemcujs_jval_as_number(jactiveDC);
    gpio_set_direction(spi2.pinDC, GPIO_MODE_OUTPUT);
  }
  jerry_release_value(jpinDC);
  jerry_release_value(jactiveDC);

  spi_bus_config_t buscfg = {
    .mosi_io_num = pinMOSI,
    .miso_io_num = pinMISO,
    .sclk_io_num = pinCLK,
    .quadhd_io_num = -1,
    .quadwp_io_num = -1,
    .max_transfer_sz = maxTransferSize
  };
  spi_device_interface_config_t devcfg = {
    .clock_speed_hz = clockHz,
    .mode = mode,
    .spics_io_num = pinCS,
    .queue_size = 10,
    .pre_cb = spiTransferCallbackPRE,
    .post_cb = spiTransferCallbackPOST
  };

  esp_err_t ret;
  if (id == 2) {
    ret = spi_bus_initialize(HSPI_HOST, &buscfg, SPI_DMA_CH2);
    ESP_ERROR_CHECK(ret);
    if (ret != ESP_OK) {
      return jerry_create_boolean(false);
    }
    ret = spi_bus_add_device(HSPI_HOST, &devcfg, &spi2.spi);
    ESP_ERROR_CHECK(ret);
    if (ret != ESP_OK) {
      return jerry_create_boolean(false);
    }
    return jerry_create_boolean(true);
  }
  return jerry_create_error(JERRY_ERROR_RANGE, (jerry_char_t*)"SPI Host not found");
}

jerry_value_t nodemcujs_module_init_spi_master() {
  jerry_value_t spi_master = jerry_create_object();
  nodemcujs_jval_set_method(spi_master, "setup", Setup);
  nodemcujs_jval_set_method(spi_master, "sendSync", SendSync);
  return spi_master;
}