#include "nodemcujs.h"
#include "nodemcujs_binding.h"
#include "nodemcujs_magic_strings.h"

#include <stdio.h>

// This function should be able to print utf8 encoded string
// as utf8 is internal string representation in Jerryscript
static jerry_value_t Print(const jerry_value_t* jargv, const jerry_length_t jargc) {
  jerry_size_t len = jerry_get_string_size(jargv[0]);
  jerry_char_t str[len + 1];
  jerry_string_to_char_buffer(jargv[0], str, len);
  str[len] = '\0';
  printf("%s", str);
  return jerry_create_undefined();
}


JS_FUNCTION(Stdout) {
  return Print(jargv, jargc);
}


JS_FUNCTION(Stderr) {
  return Print(jargv, jargc);
}


jerry_value_t nodemcujs_module_init_console() {
  jerry_value_t console = jerry_create_object();

  nodemcujs_jval_set_method(console, NODEMCUJS_MAGIC_STRING_STDOUT, Stdout);
  nodemcujs_jval_set_method(console, NODEMCUJS_MAGIC_STRING_STDERR, Stderr);

  return console;
}
