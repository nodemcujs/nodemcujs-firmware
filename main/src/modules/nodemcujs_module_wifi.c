#include <string.h>

#include "nodemcujs.h"
#include "nodemcujs_def.h"
#include "nodemcujs_string.h"
#include "nodemcujs_binding.h"

#include "esp_event.h"
#include "esp_wifi.h"

#include "esp_err.h"

static jerry_value_t JS_EVENT_CB = NULL;

static void event_handler(void* arg, esp_event_base_t base, int32_t id, void* data) {
  if (!JS_EVENT_CB) {
    return;
  }

  jerry_value_t jdata = jerry_create_object();
  jerry_value_t args[] = {
    jerry_create_string((jerry_char_t*)base),
    jerry_create_number(id),
    jdata
  };

  if (base == IP_EVENT) {
    if (id == IP_EVENT_STA_GOT_IP) {
      ip_event_got_ip_t* event = (ip_event_got_ip_t*) data;
      NLOG_ERR("got ip: "IPSTR"\n", IP2STR(&event->ip_info.ip));
      nodemcujs_jval_set_property_jval(jdata, "ip_changed", jerry_create_boolean(event->ip_changed));
      nodemcujs_jval_set_property_jval(jdata, "ip", jerry_create_number((uint32_t)event->ip_info.ip.addr));
      nodemcujs_jval_set_property_jval(jdata, "netmask", jerry_create_number((uint32_t)event->ip_info.netmask.addr));
      nodemcujs_jval_set_property_jval(jdata, "gw", jerry_create_number((uint32_t)event->ip_info.gw.addr));
    }
  }

  if (base == WIFI_EVENT) {

  }

  jerry_call_function(JS_EVENT_CB, NULL, args, 3);
}

JS_FUNCTION(Init) {
  uint8_t mode = JS_GET_ARG(0, number);

  esp_err_t err = esp_netif_init();
  NESP_CHECK_OK(err);
  err = esp_event_loop_create_default();
  NESP_CHECK_OK(err);

  // STA
  if (mode == 1) {
    esp_netif_create_default_wifi_sta();
  } else if (mode == 2) {
    esp_netif_create_default_wifi_ap();
  } else {
    return jerry_create_error(JERRY_ERROR_RANGE, (jerry_char_t*)"Unsupported WIFI Mode");
  }

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  err = esp_wifi_init(&cfg);
  NESP_CHECK_OK(err);

  return jerry_create_number(ESP_OK);
}

JS_FUNCTION(SetMode) {
  wifi_mode_t mode = JS_GET_ARG(0, number);
  esp_err_t err = esp_wifi_set_mode(mode);

  return jerry_create_number(err);
}

JS_FUNCTION(setConfig) {
  uint8_t mode = JS_GET_ARG(0, number);
  jerry_value_t config = JS_GET_ARG(1, object);
  jerry_value_t jssid = nodemcujs_jval_get_property(config, "ssid");
  nodemcujs_string_t ssid = nodemcujs_jval_as_string(jssid);
  jerry_release_value(jssid);
  jerry_value_t jpass = nodemcujs_jval_get_property(config, "password");
  nodemcujs_string_t pass = nodemcujs_jval_as_string(jpass);
  jerry_release_value(jpass);
  jerry_value_t jauth = nodemcujs_jval_get_property(config, "auth");
  uint8_t auth = nodemcujs_jval_as_number(jauth);
  jerry_release_value(jauth);

  wifi_interface_t interface = WIFI_IF_STA;
  wifi_config_t cfg = {};

  if (mode == 1) {
    wifi_sta_config_t sta = {
      .threshold = {
        .authmode = auth
      },
      .pmf_cfg = {
        .capable = true,
        .required = false
      },
    };
    strcpy((char*)sta.ssid, nodemcujs_string_data(&ssid));
    strcpy((char*)sta.password, nodemcujs_string_data(&pass));
    cfg.sta = sta;
  } else if (mode == 2) {
    interface = WIFI_IF_AP;
    wifi_ap_config_t ap = {
      .ssid_len = nodemcujs_string_size(&ssid),
      .max_connection = 4,
      .authmode = auth
    };
    strcpy((char*)ap.ssid, nodemcujs_string_data(&ssid));
    strcpy((char*)ap.password, nodemcujs_string_data(&pass));
    cfg.ap = ap;
  }

  nodemcujs_string_destroy(&ssid);
  nodemcujs_string_destroy(&pass);

  esp_err_t err = esp_wifi_set_config(interface, &cfg);

  return jerry_create_number(err);
}

JS_FUNCTION(Start) {
  esp_err_t err = esp_wifi_start();

  return jerry_create_number(err);
}

JS_FUNCTION(SetEventListener) {
  jerry_value_t jsFunc = JS_GET_ARG(0, function);
  JS_EVENT_CB = jerry_acquire_value(jsFunc);

  return jerry_create_boolean(true);
}

JS_FUNCTION(AddListenEvent) {
  uint8_t code = JS_GET_ARG(0, number);
  esp_event_base_t base;

  switch (code)
  {
  case 1:
    base = WIFI_EVENT;
    break;
  case 2:
    base = IP_EVENT;
    break;
  default:
    return jerry_create_error(JERRY_ERROR_RANGE, (jerry_char_t*)"Unknow event id");
    break;
  }

  esp_err_t err = esp_event_handler_instance_register(
    base,
    ESP_EVENT_ANY_ID,
    &event_handler,
    NULL,
    NULL
  );

  return jerry_create_number(err);
}

JS_FUNCTION(Connect) {
  esp_err_t err = esp_wifi_connect();
  return jerry_create_number(err);
}

jerry_value_t nodemcujs_module_init_wifi() {
  jerry_value_t wifi = jerry_create_object();
  nodemcujs_jval_set_method(wifi, "init", Init);
  nodemcujs_jval_set_method(wifi, "setMode", SetMode);
  nodemcujs_jval_set_method(wifi, "setConfig", setConfig);
  nodemcujs_jval_set_method(wifi, "start", Start);
  nodemcujs_jval_set_method(wifi, "setEventListener", SetEventListener);
  nodemcujs_jval_set_method(wifi, "addListenEvent", AddListenEvent);
  nodemcujs_jval_set_method(wifi, "connect", Connect);
  return wifi;
}
