#include "nodemcujs.h"
#include "nodemcujs_def.h"
#include "nodemcujs_binding.h"

#include "esp_system.h"
#include "esp_attr.h"
#include "driver/sigmadelta.h"

JS_FUNCTION(Setup) {
  uint8_t channel = JS_GET_ARG(0, number);
  int8_t duty = JS_GET_ARG(1, number);
  uint8_t prescale = JS_GET_ARG(2, number);
  uint8_t gpio = JS_GET_ARG(3, number);

  sigmadelta_config_t cfg = {
    .channel = channel,
    .sigmadelta_duty = duty,
    .sigmadelta_prescale = prescale,
    .sigmadelta_gpio = gpio
  };
  esp_err_t ret = sigmadelta_config(&cfg);
  if (ret != ESP_OK) {
    return jerry_create_boolean(false);
  }
  return jerry_create_boolean(true);
}

JS_FUNCTION(SetDuty) {
  uint8_t channel = JS_GET_ARG(0, number);
  int8_t duty = JS_GET_ARG(1, number);
  esp_err_t ret = sigmadelta_set_duty(channel, duty);
  if (ret != ESP_OK) {
    return jerry_create_boolean(false);
  }
  return jerry_create_boolean(true);
}

jerry_value_t nodemcujs_module_init_sigmadelta() {
  jerry_value_t sigmadelta = jerry_create_object();
  nodemcujs_jval_set_method(sigmadelta, "setup", Setup);
  nodemcujs_jval_set_method(sigmadelta, "setDuty", SetDuty);
  return sigmadelta;
}
