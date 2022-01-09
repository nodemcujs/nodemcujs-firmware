#ifndef NODEMCUJS_STRING_H
#define NODEMCUJS_STRING_H

#include <stddef.h>

typedef struct nodemcujs_string
{
  size_t size;
  char* data;
} nodemcujs_string_t;

nodemcujs_string_t nodemcujs_string_create();
nodemcujs_string_t nodemcujs_string_create_with_size(const char* data, size_t size);
nodemcujs_string_t nodemcujs_string_create_with_buffer(char* buffer, size_t size);

void nodemcujs_string_destroy(nodemcujs_string_t* str);

// Returns pointer to the bytes (never returns NULL)
const char* nodemcujs_string_data(const nodemcujs_string_t* str);

size_t nodemcujs_string_size(const nodemcujs_string_t* str);

#endif