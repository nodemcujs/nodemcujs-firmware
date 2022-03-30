#ifndef NODEMCUJS_DEF_H
#define NODEMCUJS_DEF_H

#include "nodemcujs_utils.h"

#include <stdio.h>

#define STRINGIFY(x) #x

#define TOSTRING(x) STRINGIFY(x)

// #ifndef NODE_MAJOR_VERSION
#define NODE_MAJOR_VERSION 0
// #endif
// #ifndef NODE_MINOR_VERSION
#define NODE_MINOR_VERSION 0
// #endif
// #ifndef NODE_PATCH_VERSION
#define NODE_PATCH_VERSION 0
// #endif

#define NODEMCUJS_VERSION                                                        \
  TOSTRING(NODE_MAJOR_VERSION)"."                                                \
  TOSTRING(NODE_MINOR_VERSION)"."                                                \
  TOSTRING(NODE_PATCH_VERSION)

// works for gcc and IAR's compiler
#define NLOG_ERR(message, ...)                                                   \
  printf("%s:%d: "message"\n", __FILE__, __LINE__, ## __VA_ARGS__)

#define NODEMCUJS_ASSERT(x)                                                      \
  do {                                                                           \
    if (!(x)) {                                                                  \
      NLOG_ERR("%s:%d: Assertion '%s' failed.\n", __FILE__, __LINE__,            \
               #x);                                                              \
      force_terminate();                                                         \
    }                                                                            \
  } while (0)

#endif

#define NESP_CHECK_OK(err)                                                       \
  do {                                                                           \
    if (err != ESP_OK) {                                                         \
      return jerry_create_number(err);                                           \
    }                                                                            \
  } while (0)
