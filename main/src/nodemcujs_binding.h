#ifndef NODEMCUJS_BINDING_H
#define NODEMCUJS_BINDING_H

#include "nodemcujs_string.h"

#include <jerryscript.h>

#include <stdbool.h>


#define JS_FUNCTION(name)                                                   \
  static jerry_value_t name(const jerry_value_t jfunc,                      \
                            const jerry_value_t jthis,                      \
                            const jerry_value_t jargv[],                    \
                            const jerry_length_t jargc)

/* Type Converters */
bool nodemcujs_jval_as_boolean(jerry_value_t);
double nodemcujs_jval_as_number(jerry_value_t);
jerry_value_t nodemcujs_jval_as_object(jerry_value_t);
jerry_value_t nodemcujs_jval_as_array(jerry_value_t);
jerry_value_t nodemcujs_jval_as_function(jerry_value_t);
nodemcujs_string_t nodemcujs_jval_as_string(jerry_value_t);

void nodemcujs_jval_set_property_jval(jerry_value_t jobj, const char* name,
                                      jerry_value_t value);

jerry_value_t nodemcujs_jval_get_property(jerry_value_t jobj, const char* name);

void nodemcujs_jval_set_method(jerry_value_t jobj, const char* name,
                               jerry_external_handler_t handler);

void nodemcujs_jval_set_string(jerry_value_t jobj, const char* name, const char* value);

// Evaluates javascript source file.
jerry_value_t nodemcujs_jhelper_eval(const char* name, size_t name_len,
                                 const uint8_t* data, size_t size,
                                 bool strict_mode, bool* throws);

#define JS_GET_ARG(index, type) nodemcujs_jval_as_##type(jargv[index])

#endif