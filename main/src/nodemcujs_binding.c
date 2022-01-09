#include "nodemcujs_binding.h"
#include "nodemcujs_def.h"

#include <stdlib.h>

bool nodemcujs_jval_as_boolean(jerry_value_t jval) {
  return jerry_get_boolean_value(jval);
}

double nodemcujs_jval_as_number(jerry_value_t jval) {
  return jerry_get_number_value(jval);
}

jerry_value_t nodemcujs_jval_as_object(jerry_value_t jval) {
  return jval;
}

jerry_value_t nodemcujs_jval_as_array(jerry_value_t jval) {
  return jval;
}

jerry_value_t nodemcujs_jval_as_function(jerry_value_t jval) {
  return jval;
}

nodemcujs_string_t nodemcujs_jval_as_string(jerry_value_t jval) {
  NODEMCUJS_ASSERT(jerry_value_is_string(jval));
  jerry_size_t size = jerry_get_utf8_string_size(jval);
  if (size == 0) {
    return nodemcujs_string_create();
  }
  char* buffer = malloc(size + 1);
  jerry_char_t* str = (jerry_char_t*)(buffer);
  size_t check = jerry_string_to_utf8_char_buffer(jval, str, size);
  NODEMCUJS_ASSERT(size == check);
  buffer[size] = '\0';

  nodemcujs_string_t res = nodemcujs_string_create_with_buffer(buffer, size);
  return res;
}

void nodemcujs_jval_set_property_jval(jerry_value_t jobj, const char* name,
                                      jerry_value_t value) {
  jerry_value_t prop_name = jerry_create_string((const jerry_char_t*)(name));
  jerry_value_t ret_val = jerry_set_property(jobj, prop_name, value);
  jerry_release_value(prop_name);

  jerry_release_value(ret_val);
}

void nodemcujs_jval_set_method(jerry_value_t jobj, const char* name,
                               jerry_external_handler_t handler) {
  jerry_value_t jfunc = jerry_create_external_function(handler);
  nodemcujs_jval_set_property_jval(jobj, name, jfunc);
  jerry_release_value(jfunc);
}

void nodemcujs_jval_set_string(jerry_value_t jobj, const char* name, const char* value) {
  jerry_value_t jval = jerry_create_string((jerry_char_t*)value);
  nodemcujs_jval_set_property_jval(jobj, name, jval);
  jerry_release_value(jval);
}

jerry_value_t nodemcujs_jhelper_eval(const char* name, size_t name_len,
                                 const uint8_t* data, size_t size,
                                 bool strict_mode, bool* throws) {
  jerry_value_t res =
      jerry_parse((const jerry_char_t*)name, name_len,
                  (const jerry_char_t*)data, size,
                  strict_mode ? JERRY_PARSE_STRICT_MODE : JERRY_PARSE_NO_OPTS);

  *throws = jerry_value_is_error(res);

  if (!*throws) {
    jerry_value_t func = res;
    res = jerry_run(func);
    jerry_release_value(func);

    *throws = jerry_value_is_error(res);
  }

  return res;
}

jerry_value_t nodemcujs_jval_get_property(jerry_value_t jobj, const char* name) {
  NODEMCUJS_ASSERT(jerry_value_is_object(jobj));

  jerry_value_t prop_name = jerry_create_string((const jerry_char_t*)(name));
  jerry_value_t res = jerry_get_property(jobj, prop_name);
  jerry_release_value(prop_name);

  if (jerry_value_is_error(res)) {
    jerry_release_value(res);
    return jerry_acquire_value(jerry_create_undefined());
  }

  return res;
}