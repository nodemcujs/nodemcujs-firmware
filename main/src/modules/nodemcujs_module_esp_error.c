#include "nodemcujs.h"
#include "nodemcujs_binding.h"

#include "esp_err.h"

JS_FUNCTION(EspErrToName) {
  esp_err_t code = JS_GET_ARG(0, number);
  char *name = esp_err_to_name(code);
  return jerry_create_string((jerry_char_t*)name);
}

jerry_value_t nodemcujs_module_init_esp_error() {
  jerry_value_t esp_err = jerry_create_object();
  nodemcujs_jval_set_method(esp_err, "esp_err_to_name", EspErrToName);
  return esp_err;
}
