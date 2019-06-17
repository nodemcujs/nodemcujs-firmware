#include "include/console.h"

#include <stdio.h>

#include "freertos/FreeRTOS.h"

#include "jerryscript.h"
#include "jerryscript-ext/handler.h"

static void print_value(const jerry_value_t value)
{
  if (jerry_value_is_undefined(value))
  {
    printf("undefined");
  }
  else if (jerry_value_is_null(value))
  {
    printf("null");
  }
  else if (jerry_value_is_boolean(value))
  {
    if (jerry_get_boolean_value(value))
    {
      printf("true");
    }
    else
    {
      printf("false");
    }
  }
  /* Float value */
  else if (jerry_value_is_number(value))
  {
    printf("%.0f", jerry_get_number_value(value));
  }
  /* String value */
  else if (jerry_value_is_string(value))
  {
    /* Determining required buffer size */
    jerry_size_t req_sz = jerry_get_string_size(value);
    jerry_char_t str_buf_p[req_sz + 1];

    jerry_string_to_char_buffer(value, str_buf_p, req_sz);
    str_buf_p[req_sz] = '\0';

    printf("%s", (const char *)str_buf_p);
  }
  /* Object reference */
  else if (jerry_value_is_object(value))
  {
    printf("[object]");
  }
}

static jerry_value_t log_handler(const jerry_value_t func_value, /**< function object */
                                 const jerry_value_t this_value, /**< this arg */
                                 const jerry_value_t args[],    /**< function arguments */
                                 const jerry_length_t args_cnt)  /**< number of function arguments */
{
  if (args_cnt == 0)
  {
    return jerry_create_undefined();
  }
  uint32_t argIndex = 0;
  if (!jerry_value_is_string(args[0]))
  {
    while (argIndex < args_cnt)
    {
      print_value(args[argIndex]);
      if (argIndex != args_cnt - 1)
      {
        printf(" ");
      }
      argIndex++;
    }
    printf("\n");
    return jerry_create_undefined();
  }
  jerry_size_t len = jerry_get_string_size(args[0]);
  jerry_char_t format[len];
  jerry_string_to_char_buffer(args[0], format, len);
  argIndex++;

  uint32_t end = 0;
  double num = 0;
  jerry_size_t substrLen = 0;
  jerry_char_t *substr;

  while (end < len)
  {
    if (argIndex >= args_cnt)
    {
      printf("%c", format[end]);
      end++;
      continue;
    }
    if (format[end] != '%')
    {
      printf("%c", format[end]);
      end++;
      continue;
    }
    switch (format[end + 1])
    {
    case 's':
      substrLen = jerry_get_string_size(args[argIndex]);
      substr = (jerry_char_t *) malloc(substrLen + 1);
      jerry_string_to_char_buffer(args[argIndex], substr, substrLen);
      substr[substrLen] = '\0';
      printf("%s", substr);
      free(substr);
      end+= 2;
      argIndex++;
      break;
    case 'd':
      num = jerry_get_number_value(args[argIndex]);
      printf("%.0f", num);
      end+= 2;
      argIndex++;
      break;
    default:
      printf("%c", format[end]);
      end++;
      break;
    }
  }
  if (len > 0)
  {
    printf("\n");
  }
  return jerry_create_undefined();
}

void module_console_init()
{

  jerry_value_t global = jerry_get_global_object();

  jerry_value_t module_name = jerry_create_string((const jerry_char_t *)"console");
  jerry_value_t console = jerry_create_object();

  jerry_value_t prop_name = jerry_create_string((const jerry_char_t *)"log");
  jerry_value_t value = jerry_create_external_function(log_handler);
  jerry_release_value(jerry_set_property(console, prop_name, value));
  jerry_release_value(prop_name);
  jerry_release_value(value);

  jerry_release_value(jerry_set_property(global, module_name, console));
  jerry_release_value(console);
  jerry_release_value(module_name);
  jerry_release_value(global);
}