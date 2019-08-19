#include "include/jerry-module.h"

#include <string.h>

#include "jerryscript.h"
#include "jerryscript-ext/handler.h"
#include "jerryscript-port.h"

extern jerry_value_t nodemcujs_module_get(const char* name);

static jerry_value_t require_handler(const jerry_value_t func_value, /**< function object */
                                     const jerry_value_t this_value, /**< this arg */
                                     const jerry_value_t args[],     /**< function arguments */
                                     const jerry_length_t args_cnt)  /**< number of function arguments */
{
  jerry_size_t strLen = jerry_get_string_size(args[0]);
  jerry_char_t name[strLen + 1];
  jerry_string_to_char_buffer(args[0], name, strLen);
  name[strLen] = '\0';

  jerry_value_t native = nodemcujs_module_get((char *)name);
  if (native != NULL)
  {
    return native;
  }

  size_t size = 0;
  jerry_char_t *script = jerry_port_read_source((char *)name, &size);

  if (script == NULL)
  {
    printf("No such file: %s\n", name);
    return jerry_create_undefined();
  }
  if (size == 0)
  {
    return jerry_create_undefined();
  }

  static const char *jargs = "exports, module, __filename";
  jerry_value_t res = jerry_parse_function((jerry_char_t *)name, strLen,
                                           (jerry_char_t *)jargs, strlen(jargs),
                                           (jerry_char_t *)script, size, JERRY_PARSE_NO_OPTS);
  jerry_port_release_source(script);
  jerry_value_t module = jerry_create_object();
  jerry_value_t exports = jerry_create_object();
  jerry_value_t prop_name = jerry_create_string((jerry_char_t *)"exports");
  jerry_release_value(jerry_set_property(module, prop_name, exports));
  jerry_value_t filename = jerry_create_string((jerry_char_t *)name);
  jerry_value_t jargs_p[] = { exports, module, filename };
  jerry_value_t jres = jerry_call_function(res, NULL, jargs_p, 3);

  jerry_release_value(res);
  jerry_release_value(filename);
  jerry_release_value(jres);

  return jerry_get_property(module, prop_name);
}

void module_module_init()
{
  jerry_value_t global = jerry_get_global_object();

  jerry_value_t prop_name = jerry_create_string((const jerry_char_t *)"require");
  jerry_value_t value = jerry_create_external_function(require_handler);
  jerry_release_value(jerry_set_property(global, prop_name, value));
  jerry_release_value(prop_name);
  jerry_release_value(value);

  jerry_release_value(global);
}
