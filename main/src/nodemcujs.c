#include "nodemcujs_binding.h"
#include "nodemcujs_js.h"
#include "nodemcujs_def.h"

#include <nodemcujs.h>

#include <stdbool.h>
#include <string.h>

static bool nodemcujs_init_jerry() {
  jerry_init(JERRY_INIT_EMPTY);
  return true;
}

static bool nodemcujs_run() {
  bool throws = false;
  jerry_value_t jmain = nodemcujs_jhelper_eval("nodemcujs.js", strlen("nodemcujs.js"),
                                               nodemcujs_s, nodemcujs_l, false, &throws);

  if (throws) {
    jerry_value_t error = jerry_get_value_from_error(jmain, false);
    jerry_value_t message = jerry_value_to_string (error);
    jerry_size_t size = jerry_get_string_size (message);
    jerry_char_t buffer[size];
    jerry_string_to_utf8_char_buffer(message, buffer, size);
    buffer[size] = '\0';
    NLOG_ERR("Script Error: %s", (char*)buffer);
    jerry_release_value(error);
    jerry_release_value(message);
  }

  jerry_release_value(jmain);
  return !throws;
}

static int nodemcujs_start() {
  const jerry_value_t global = jerry_get_global_object();
  const jerry_value_t process = nodemcujs_module_get("process");
  nodemcujs_jval_set_property_jval(global, "process", process);

  jerry_release_value(global);

  nodemcujs_run();
  return 0;
}

int nodemcujs_entry() {
  nodemcujs_init_jerry();

  int ret_code = nodemcujs_start();
  return ret_code;
}
