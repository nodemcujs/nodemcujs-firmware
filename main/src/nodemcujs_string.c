#include "nodemcujs_string.h"
#include "nodemcujs_def.h"

#include <stdlib.h>
#include <string.h>

nodemcujs_string_t nodemcujs_string_create() {
  nodemcujs_string_t str;

  str.size = 0;
  str.data = NULL;

  return str;
}

nodemcujs_string_t nodemcujs_string_create_with_size(const char* data, size_t size) {
  nodemcujs_string_t str;

  str.size = size;

  if (size > 0) {
    NODEMCUJS_ASSERT(data != NULL);
    str.data = malloc(size);
    memcpy(str.data, data, size);
  } else {
    str.data = NULL;
  }

  return str;
}

nodemcujs_string_t nodemcujs_string_create_with_buffer(char* buffer, size_t size) {
  nodemcujs_string_t str;

  str.size = size;

  if (size > 0) {
    NODEMCUJS_ASSERT(buffer != NULL);
    str.data = buffer;
  } else {
    str.data = NULL;
  }

  return str;
}

void nodemcujs_string_destroy(nodemcujs_string_t* str) {
  if (str->data != NULL) {
    free(str->data);
    str->size = 0;
  }
}

const char* nodemcujs_string_data(const nodemcujs_string_t* str) {
  if (str->data == NULL) {
    return "";
  }
  return str->data;
}

size_t nodemcujs_string_size(const nodemcujs_string_t* str) {
  return str->size;
}