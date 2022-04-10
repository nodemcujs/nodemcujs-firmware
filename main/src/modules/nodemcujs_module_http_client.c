#include "nodemcujs.h"
#include "nodemcujs_def.h"
#include "nodemcujs_string.h"
#include "nodemcujs_binding.h"

#include "esp_err.h"
#include "esp_http_client.h"

#include <string.h>

static const jerry_object_native_info_t NativeInfoHttpClient;

JS_FUNCTION(ClientRequest) {
  jerry_value_t options = JS_GET_ARG(0, object);

  jerry_value_t jurl = nodemcujs_jval_get_property(options, "url");
  nodemcujs_string_t url = nodemcujs_jval_as_string(jurl);
  jerry_release_value(jurl);
  jerry_value_t jauthType = nodemcujs_jval_get_property(options, "authType");
  uint8_t auth_type = nodemcujs_jval_as_number(jauthType);
  jerry_release_value(jauthType);
  jerry_value_t jquery = nodemcujs_jval_get_property(options, "query");
  nodemcujs_string_t query = nodemcujs_jval_as_string(jquery);
  jerry_release_value(jquery);
  jerry_value_t jmethod = nodemcujs_jval_get_property(options, "method");
  uint8_t method = nodemcujs_jval_as_number(jmethod);
  jerry_release_value(jmethod);
  jerry_value_t jtransportType = nodemcujs_jval_get_property(options, "transportType");
  uint8_t transport_type = nodemcujs_jval_as_number(jtransportType);
  jerry_release_value(jtransportType);

  esp_http_client_config_t cfg = {
    .url = nodemcujs_string_data(&url),
    .query = nodemcujs_string_data(&query),
    .method = method,
    .transport_type = transport_type
  };
  esp_http_client_handle_t client = esp_http_client_init(&cfg);
  nodemcujs_string_destroy(&url);
  nodemcujs_string_destroy(&query);
  jerry_set_object_native_pointer(jthis, client, &NativeInfoHttpClient);
  return jerry_create_undefined();
}

JS_FUNCTION(Open) {
  esp_http_client_handle_t client;
  bool has_p = jerry_get_object_native_pointer(jthis, &client, &NativeInfoHttpClient);
  if (!has_p) {
    return jerry_create_error(JERRY_ERROR_REFERENCE, (jerry_char_t*)"The native http client is undefiend");
  }

  int length = JS_GET_ARG(0, number);
  esp_err_t err = esp_http_client_open(client, length);
  return jerry_create_number(err);
}

JS_FUNCTION(Write) {
  esp_http_client_handle_t client;
  bool has_p = jerry_get_object_native_pointer(jthis, &client, &NativeInfoHttpClient);
  if (!has_p) {
    return jerry_create_error(JERRY_ERROR_REFERENCE, (jerry_char_t*)"The native http client is undefiend");
  }

  nodemcujs_string_t data = JS_GET_ARG(0, string);
  int wlen = esp_http_client_write(client, nodemcujs_string_data(&data), nodemcujs_string_size(&data));
  nodemcujs_string_destroy(&data);
  return jerry_create_number(wlen);
}

JS_FUNCTION(FetchHeaders) {
  esp_http_client_handle_t client;
  bool has_p = jerry_get_object_native_pointer(jthis, &client, &NativeInfoHttpClient);
  if (!has_p) {
    return jerry_create_error(JERRY_ERROR_REFERENCE, (jerry_char_t*)"The native http client is undefiend");
  }

  int content_length = esp_http_client_fetch_headers(client);
  return jerry_create_number(content_length);
}

JS_FUNCTION(GetStatusCode) {
  esp_http_client_handle_t client;
  bool has_p = jerry_get_object_native_pointer(jthis, &client, &NativeInfoHttpClient);
  if (!has_p) {
    return jerry_create_error(JERRY_ERROR_REFERENCE, (jerry_char_t*)"The native http client is undefiend");
  }

  int code = esp_http_client_get_status_code(client);
  return jerry_create_number(code);
}

JS_FUNCTION(Read) {
  esp_http_client_handle_t client;
  bool has_p = jerry_get_object_native_pointer(jthis, &client, &NativeInfoHttpClient);
  if (!has_p) {
    return jerry_create_error(JERRY_ERROR_REFERENCE, (jerry_char_t*)"The native http client is undefiend");
  }

  uint32_t len = JS_GET_ARG(0, number);
  char *buffer = (char*)malloc(len + 1);
  if (buffer == NULL) {
    return jerry_create_error(JERRY_ERROR_RANGE, (jerry_char_t*)NODE_OUT_OF_MEMORY);
  }
  int rlen = esp_http_client_read(client, buffer, len);
  if (rlen < 0) {
    free(buffer);
    return jerry_create_number(rlen);
  }
  if (rlen == 0) {
    free(buffer);
    return jerry_create_string((jerry_char_t*)"");
  }
  buffer[rlen] = '\0';
  jerry_value_t jstr = jerry_create_string((jerry_char_t*)buffer);
  free(buffer);
  return jstr;
}

jerry_value_t nodemcujs_module_init_http_client() {
  jerry_value_t httpClient = jerry_create_object();
  jerry_value_t jClientRequest = jerry_create_external_function(ClientRequest);
  jerry_value_t prototype = jerry_create_object();
  
  nodemcujs_jval_set_method(prototype, "open", Open);
  nodemcujs_jval_set_method(prototype, "write", Write);
  nodemcujs_jval_set_method(prototype, "read", Read);
  nodemcujs_jval_set_method(prototype, "fetchHeaders", FetchHeaders);
  nodemcujs_jval_set_method(prototype, "getStatusCode", GetStatusCode);
  nodemcujs_jval_set_property_jval(jClientRequest, "prototype", prototype);
  nodemcujs_jval_set_property_jval(httpClient, "ClientRequest", jClientRequest);

  jerry_release_value(prototype);
  jerry_release_value(jClientRequest);
  return httpClient;
}
