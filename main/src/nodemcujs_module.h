#ifndef NODEMCUJS_MODULE_H
#define NODEMCUJS_MODULE_H

#include <jerryscript.h>

typedef jerry_value_t (*register_func)();

typedef struct nodemcujs_module
{
  const char* name;
  register_func fn_register;
} nodemcujs_module_t;

typedef struct nodemcujs_module_objects
{
  jerry_value_t jmodule;
} nodemcujs_module_objects_t;

extern const unsigned nodemcujs_modules_count;

extern const nodemcujs_module_t nodemcujs_modules[];

extern nodemcujs_module_objects_t nodemcujs_module_objects[];

#endif