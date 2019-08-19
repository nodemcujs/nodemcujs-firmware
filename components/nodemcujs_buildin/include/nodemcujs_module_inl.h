#ifndef MODULE_BUILDIN_H
#define MODULE_BUILDIN_H

#include "jerryscript.h"

// buildin modules
extern jerry_value_t nodemcujs_init_gpio();


typedef jerry_value_t (*register_func)();

typedef struct
{
  const char* name;
  register_func fn_register;
} nodemcujs_module_t;

typedef struct
{
  jerry_value_t jmodule;
} nodemcujs_module_objects_t;

const nodemcujs_module_t nodemcujs_modules[] = {
  { "gpio", nodemcujs_init_gpio }
};

const unsigned nodemcujs_modules_count = sizeof(nodemcujs_modules) / sizeof(nodemcujs_module_t);

nodemcujs_module_objects_t nodemcujs_module_objects[sizeof(nodemcujs_modules) / sizeof(nodemcujs_module_t)];

jerry_value_t nodemcujs_module_get(const char* name);

#endif
