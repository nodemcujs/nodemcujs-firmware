#include <string.h>

#include "nodemcujs.h"
#include "nodemcujs_def.h"
#include "nodemcujs_string.h"
#include "nodemcujs_binding.h"

#include "esp_event.h"
#include "esp_wifi.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

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
      char ip[16];
      char netmask[16];
      char gw[16];
      uint8_t ip_len = sprintf(ip, IPSTR, IP2STR(&event->ip_info.ip));
      uint8_t netmask_len = sprintf(netmask, IPSTR, IP2STR(&event->ip_info.netmask));
      uint8_t gw_len = sprintf(gw, IPSTR, IP2STR(&event->ip_info.gw));
      ip[ip_len] = '\0';
      netmask[netmask_len] = '\0';
      gw[gw_len] = '\0';

      nodemcujs_jval_set_property_jval(jdata, "ip_changed", jerry_create_boolean(event->ip_changed));
      nodemcujs_jval_set_property_jval(jdata, "ip", jerry_create_string((jerry_char_t*)ip));
      nodemcujs_jval_set_property_jval(jdata, "netmask", jerry_create_string((jerry_char_t*)netmask));
      nodemcujs_jval_set_property_jval(jdata, "gw", jerry_create_string((jerry_char_t*)gw));
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

JS_FUNCTION(ScanASync) {
  jerry_value_t config = JS_GET_ARG(0, object);

  jerry_value_t jchannel = nodemcujs_jval_get_property(config, "channel");
  uint8_t channel = nodemcujs_jval_as_number(jchannel);
  jerry_release_value(jchannel);

  jerry_value_t jshowHidden = nodemcujs_jval_get_property(config, "showHidden");
  bool show_hidden = nodemcujs_jval_as_boolean(jshowHidden);
  jerry_release_value(jshowHidden);

  jerry_value_t jscanType = nodemcujs_jval_get_property(config, "scanType");
  uint8_t scan_type = nodemcujs_jval_as_number(jscanType);
  jerry_release_value(jscanType);

  wifi_scan_config_t cfg = {
    .ssid = NULL,
    .bssid = NULL,
    .channel = channel,
    .show_hidden = show_hidden,
    .scan_type = scan_type,
    .scan_time = {
      .passive = 120,
      .active = {
        .max = 0,
        .min = 0
      }
    }
  };
  esp_err_t err = esp_wifi_scan_start(&cfg, false);
  NESP_CHECK_OK(err);
  vTaskDelay(13 * 120 / portTICK_PERIOD_MS);

  uint16_t apCount = 0;
  err = esp_wifi_scan_get_ap_num(&apCount);
  NESP_CHECK_OK(err);
  wifi_ap_record_t apInfos[apCount];
  err = esp_wifi_scan_get_ap_records(&apCount, apInfos);
  NESP_CHECK_OK(err);
  jerry_value_t jarr = jerry_create_array(apCount);
  for (uint16_t i = 0; i < apCount; i++)
  {
    jerry_value_t japInfo = jerry_create_object();
    wifi_ap_record_t apInfo = apInfos[i];
    nodemcujs_jval_set_string(japInfo, "ssid", (char*)apInfo.ssid);
    nodemcujs_jval_set_string(japInfo, "bssid", (char*)apInfo.bssid);
    nodemcujs_jval_set_property_jval(japInfo, "rssi", jerry_create_number(apInfo.rssi));
    jerry_set_property_by_index(jarr, i, japInfo);
  }

  return jarr;
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
  nodemcujs_jval_set_method(wifi, "scanASync", ScanASync);
  return wifi;
}
