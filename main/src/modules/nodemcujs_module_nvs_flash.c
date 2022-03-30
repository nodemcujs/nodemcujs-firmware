#include "nodemcujs.h"
#include "nodemcujs_def.h"
#include "nodemcujs_binding.h"

#include "nvs_flash.h"
#include "esp_err.h"

JS_FUNCTION(Init) {
  esp_err_t err = nvs_flash_init();
  if (err == ESP_ERR_NVS_NO_FREE_PAGES || err == ESP_ERR_NVS_NEW_VERSION_FOUND) {
    err = nvs_flash_erase();
    NESP_CHECK_OK(err);
    err = nvs_flash_init();
    NESP_CHECK_OK(err);
  }
  return jerry_create_number(ESP_OK);
}

jerry_value_t nodemcujs_module_init_nvs_flash() {
  jerry_value_t nvs_flash = jerry_create_object();
  nodemcujs_jval_set_method(nvs_flash, "init", Init);
  return nvs_flash;
}