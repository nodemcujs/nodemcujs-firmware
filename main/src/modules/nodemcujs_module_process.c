#include "nodemcujs.h"
#include "nodemcujs_js.h"
#include "nodemcujs_def.h"
#include "nodemcujs_string.h"
#include "nodemcujs_module.h"
#include "nodemcujs_binding.h"
#include "nodemcujs_magic_strings.h"

#include <jerryscript-port.h>

#include <string.h>

static jerry_value_t WrapEval(const char* name, size_t name_len, char* source,
                              size_t length) {
  static const char* args = "exports, require, module, native, __filename, __dirname";

  jerry_value_t res =
      jerry_parse_function((const jerry_char_t*)name, name_len,
                           (const jerry_char_t*)args, strlen(args),
                           (const jerry_char_t*)source, length, false);

  return res;
}

JS_FUNCTION(CompileModule) {
  jerry_value_t jmodule = JS_GET_ARG(0, object);
  jerry_value_t jrequire = JS_GET_ARG(1, function);

  jerry_value_t jid = nodemcujs_jval_get_property(jmodule, "id");
  nodemcujs_string_t id = nodemcujs_jval_as_string(jid);
  jerry_release_value(jid);
  const char* name = nodemcujs_string_data(&id);

  int i = 0;
  while (js_modules[i].name != NULL) {
    if (!strcmp(js_modules[i].name, name)) {
      break;
    }

    i++;
  }

  jerry_value_t native_module_jval = nodemcujs_module_get(name);
  if (jerry_value_is_error(native_module_jval)) {
    return native_module_jval;
  }

  jerry_value_t jexports = nodemcujs_jval_get_property(jmodule, "exports");
  jerry_value_t jres = jerry_create_undefined();

  if (js_modules[i].name != NULL) {
    jres = WrapEval(name, nodemcujs_string_size(&id), (char*)js_modules[i].code,
                    js_modules[i].length);
    if (!jerry_value_is_error(jres)) {
      jerry_value_t args[] = { jexports, jrequire, jmodule,
                               native_module_jval };

      jerry_value_t jfunc = jres;
      jres = jerry_call_function(jfunc, jerry_create_undefined(), args,
                                 sizeof(args) / sizeof(jerry_value_t));
      jerry_release_value(jfunc);
    }
  } else if (!jerry_value_is_undefined(native_module_jval)) {
    nodemcujs_jval_set_property_jval(jmodule, "exports", native_module_jval);
  } else {
    jres = jerry_create_error(JERRY_ERROR_COMMON, (jerry_char_t*)"native module not found");
  }

  jerry_release_value(jexports);
  nodemcujs_string_destroy(&id);
  return jres;
}

JS_FUNCTION(Compile) {
  nodemcujs_string_t path = JS_GET_ARG(0, string);
  const char* filename = nodemcujs_string_data(&path);

  size_t size = 0;
  jerry_char_t* script = jerry_port_read_source(filename, &size);

  if (script == NULL) {
    NLOG_ERR("No such file: %s", filename);
    return jerry_create_undefined();
  }
  if (size == 0) {
    NLOG_ERR("Can not read file: %s", filename);
    return jerry_create_undefined();
  }

  jerry_value_t jres = WrapEval(filename, strlen(filename), (char*)script, size);
  nodemcujs_string_destroy(&path);
  jerry_port_release_source(script);
  return jres;
}

JS_FUNCTION(Cwd) {
  return jerry_create_string_from_utf8((jerry_char_t*)"/");
}

JS_FUNCTION(ReadSource) {
  nodemcujs_string_t path = JS_GET_ARG(0, string);
  const char* filename = nodemcujs_string_data(&path);

  size_t size = 0;
  jerry_char_t* source = jerry_port_read_source(filename, &size);

  if (source == NULL) {
    nodemcujs_string_destroy(&path);
    return jerry_create_error(JERRY_ERROR_COMMON, (jerry_char_t*)"ReadSource error, not a regular file");
  }
  if (size == 0) {
    nodemcujs_string_destroy(&path);
    return jerry_create_string_from_utf8((jerry_char_t*)"");
  }

  jerry_value_t jres = jerry_create_string_from_utf8(source);
  nodemcujs_string_destroy(&path);
  jerry_port_release_source(source);

  return jres;
}

static void SetBuiltinModules(jerry_value_t builtin_modules) {
  for (unsigned i = 0; js_modules[i].name; i++) {
    nodemcujs_jval_set_property_jval(builtin_modules, js_modules[i].name, jerry_create_boolean(true));
  }
  for (unsigned i = 0; i < nodemcujs_modules_count; i++) {
    nodemcujs_jval_set_property_jval(builtin_modules, nodemcujs_modules[i].name, jerry_create_boolean(true));
  }
}

jerry_value_t nodemcujs_module_init_process() {
  jerry_value_t process = jerry_create_object();
  nodemcujs_jval_set_method(process, NODEMCUJS_MAGIC_STRING_COMPILEMODULE, CompileModule);
  nodemcujs_jval_set_method(process, NODEMCUJS_MAGIC_STRING_COMPILE, Compile);
  nodemcujs_jval_set_method(process, NODEMCUJS_MAGIC_STRING_CWD, Cwd);
  nodemcujs_jval_set_string(process, "version", "v"NODEMCUJS_VERSION);

  jerry_value_t builtin_modules = jerry_create_object();
  SetBuiltinModules(builtin_modules);
  nodemcujs_jval_set_property_jval(process, NODEMCUJS_MAGIC_STRING_BUILTIN_MODULES, builtin_modules);
  jerry_release_value(builtin_modules);

  nodemcujs_jval_set_method(process, NODEMCUJS_MAGIC_STRING_READSOURCE, ReadSource);

  return process;
}