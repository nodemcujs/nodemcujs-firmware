#include "include/nodemcujs_module_inl.h"

#include <string.h>

jerry_value_t nodemcujs_module_get(const char* name)
{
  for (unsigned i = 0; i < nodemcujs_modules_count; i++)
  {
    if (!strcmp(name, nodemcujs_modules[i].name))
    {
      if (nodemcujs_module_objects[i].jmodule == 0)
      {
        nodemcujs_module_objects[i].jmodule = nodemcujs_modules[i].fn_register();
      }
      return nodemcujs_module_objects[i].jmodule;
    }
  }
  // buildin module not found.
  return NULL;
}
