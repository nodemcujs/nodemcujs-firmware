#ifndef NODEMCUJS_H
#define NODEMCUJS_H

#include <jerryscript.h>

int nodemcujs_entry();

extern jerry_value_t nodemcujs_module_get(const char* name);

#endif