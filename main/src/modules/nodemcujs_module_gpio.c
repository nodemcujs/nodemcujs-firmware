#include "nodemcujs.h"
#include "nodemcujs_binding.h"
#include "nodemcujs_magic_strings.h"

#include "driver/gpio.h"

#include "esp_err.h"

JS_FUNCTION(Mode) {
  int pin_num = (int)JS_GET_ARG(0, number);
  int pin_mode = (int)JS_GET_ARG(1, number);
  esp_err_t status = gpio_set_direction(pin_num, pin_mode);
  if (status != ESP_OK)
  {
    return jerry_create_boolean(false);
  }
  return jerry_create_boolean(true);
}

JS_FUNCTION(Write) {
  int pin_num = (int)JS_GET_ARG(0, number);
  uint32_t pin_level = (uint32_t)JS_GET_ARG(1, number);
  esp_err_t status = gpio_set_level(pin_num, pin_level);
  if (status != ESP_OK)
  {
    return jerry_create_boolean(false);
  }
  return jerry_create_boolean(true);
}

JS_FUNCTION(Read) {
  int pin_num = (int)JS_GET_ARG(0, number);
  int level = gpio_get_level(pin_num);
  return jerry_create_number((double)level);
}

jerry_value_t nodemcujs_module_init_gpio()
{
  jerry_value_t gpio = jerry_create_object();

  nodemcujs_jval_set_method(gpio, NODEMCUJS_MAGIC_STRING_MODE, Mode);
  nodemcujs_jval_set_method(gpio, NODEMCUJS_MAGIC_STRING_WRITE, Write);
  nodemcujs_jval_set_method(gpio, NODEMCUJS_MAGIC_STRING_READ, Read);

  return gpio;
}