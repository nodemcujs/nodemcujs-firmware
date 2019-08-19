#include "include/nodemcujs_module_gpio.h"

#include "esp_err.h"

#include "driver/gpio.h"

static jerry_value_t mode_handler(const jerry_value_t func_value, /**< function object */
                                  const jerry_value_t this_value, /**< this arg */
                                  const jerry_value_t args[],     /**< function arguments */
                                  const jerry_length_t args_cnt)  /**< number of function arguments */
{
  int pin_num = (int)jerry_get_number_value(args[0]);
  int pin_mode = (int)jerry_get_number_value(args[1]);
  esp_err_t status = gpio_set_direction(pin_num, pin_mode);
  if (status != ESP_OK)
  {
    printf("gpio_set_direction error!\n");
    return jerry_create_boolean(false);
  }
  return jerry_create_boolean(true);
}

static jerry_value_t write_handler(const jerry_value_t func_value, /**< function object */
                                   const jerry_value_t this_value, /**< this arg */
                                   const jerry_value_t args[],     /**< function arguments */
                                   const jerry_length_t args_cnt)  /**< number of function arguments */
{
  int pin_num = (int)jerry_get_number_value(args[0]);
  uint32_t pin_level = (uint32_t)jerry_get_number_value(args[1]);
  esp_err_t status = gpio_set_level(pin_num, pin_level);
  if (status != ESP_OK)
  {
    printf("gpio_set_level error!\n");
    return jerry_create_boolean(false);
  }
  return jerry_create_boolean(true);
}

static jerry_value_t read_handler(const jerry_value_t func_value, /**< function object */
                                  const jerry_value_t this_value, /**< this arg */
                                  const jerry_value_t args[],     /**< function arguments */
                                  const jerry_length_t args_cnt)  /**< number of function arguments */
{
  int pin_num = (int)jerry_get_number_value(args[0]);
  int level = gpio_get_level(pin_num);
  return jerry_create_number((double)level);
}

jerry_value_t nodemcujs_init_gpio()
{
  jerry_value_t gpio = jerry_create_object();
  jerry_value_t prop_name = jerry_create_string((const jerry_char_t *)"mode");
  jerry_value_t value = jerry_create_external_function(mode_handler);
  jerry_release_value(jerry_set_property(gpio, prop_name, value));
  jerry_release_value(prop_name);
  jerry_release_value(value);

  prop_name = jerry_create_string((const jerry_char_t *)"write");
  value = jerry_create_external_function(write_handler);
  jerry_release_value(jerry_set_property(gpio, prop_name, value));
  jerry_release_value(prop_name);
  jerry_release_value(value);

  prop_name = jerry_create_string((const jerry_char_t *)"read");
  value = jerry_create_external_function(read_handler);
  jerry_release_value(jerry_set_property(gpio, prop_name, value));
  jerry_release_value(prop_name);
  jerry_release_value(value);

  return gpio;
}