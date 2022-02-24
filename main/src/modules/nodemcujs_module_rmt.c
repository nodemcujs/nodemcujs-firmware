#include "nodemcujs.h"
#include "nodemcujs_def.h"
#include "nodemcujs_binding.h"
#include "nodemcujs_magic_strings.h"

#include "esp_attr.h"
#include "driver/rmt.h"

#include <string.h>

IRAM_ATTR rmt_item32_t PATTERN0 = {{{ 0, 0, 0, 0 }}};
IRAM_ATTR rmt_item32_t PATTERN1 = {{{ 0, 0, 0, 0 }}};

JS_FUNCTION(Setup) {
  uint8_t channel = JS_GET_ARG(0, number);
  uint8_t pin = JS_GET_ARG(1, number);
  jerry_value_t jconfig = JS_GET_ARG(2, object);
  jerry_value_t jclkDiv = nodemcujs_jval_get_property(jconfig, "clock_div");
  uint8_t clkDiv = nodemcujs_jval_as_number(jclkDiv);
  jerry_release_value(jclkDiv);
  jerry_value_t jidleOutputEN = nodemcujs_jval_get_property(jconfig, "idle_output_en");
  bool idleOutputEN = nodemcujs_jval_as_boolean(jidleOutputEN);
  jerry_release_value(idleOutputEN);

  rmt_config_t cfg = RMT_DEFAULT_CONFIG_TX(pin, channel);
  cfg.clk_div = clkDiv;
  cfg.tx_config.idle_output_en = idleOutputEN;
  rmt_config(&cfg);
  rmt_driver_install(cfg.channel, 0, 0);

  return jerry_create_boolean(true);
}

JS_FUNCTION(SetPattern) {
  uint8_t pattern = JS_GET_ARG(0, number);
  uint16_t t0 = JS_GET_ARG(1, number);
  uint8_t l0 = JS_GET_ARG(2, number);
  uint16_t t1 = JS_GET_ARG(3, number);
  uint8_t l1 = JS_GET_ARG(4, number);

  if (pattern == 0) {
    PATTERN0 = (rmt_item32_t){{{ t0, l0, t1, l1 }}};
  } else {
    PATTERN1 = (rmt_item32_t){{{ t0, l0, t1, l1 }}};
  }

  return jerry_create_boolean(true);
}

JS_FUNCTION(SendSync) {
  uint8_t channel = JS_GET_ARG(0, number);
  jerry_value_t data = JS_GET_ARG(1, array);

  jerry_value_t jlength = nodemcujs_jval_get_property(data, NODEMCUJS_MAGIC_STRING_LENGTH);
  int length = nodemcujs_jval_as_number(jlength);

  for (int i = 0; i < length; i++) {
    jerry_value_t jdata = jerry_get_property_by_index(data, i);
    uint8_t value = (uint8_t)nodemcujs_jval_as_number(jdata);
    jerry_release_value(jdata);
    for (size_t j = 0; j < 8; j++) {
      if ((value >> (7 - j)) & 0x01) {
        rmt_write_items(channel, &PATTERN1, 1, true);
      } else {
        rmt_write_items(channel, &PATTERN0, 1, true);
      }
    }
  }
  jerry_release_value(jlength);

  return jerry_create_boolean(true);
}

jerry_value_t nodemcujs_module_init_rmt() {
  jerry_value_t rmt = jerry_create_object();
  nodemcujs_jval_set_method(rmt, "setup", Setup);
  nodemcujs_jval_set_method(rmt, "setPattern", SetPattern);
  nodemcujs_jval_set_method(rmt, "sendSync", SendSync);
  return rmt;
}