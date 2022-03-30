'use strict';

var Native = require('native');
var wifi = native

var WIFI_MODE = {
  STA: 1,
  AP: 2,
  APSTA: 3
};

var WIFI_AUTH_MODE = {
  WIFI_AUTH_OPEN: 0,
  WIFI_AUTH_WEP: 2,
  WIFI_AUTH_WPA_PSK: 2,
  WIFI_AUTH_WPA2_PSK: 3,
  WIFI_AUTH_WPA_WPA2_PSK: 4,
  WIFI_AUTH_WPA2_ENTERPRISE: 5,
  WIFI_AUTH_WPA3_PSK: 6,
  WIFI_AUTH_WPA2_WPA3_PSK: 7,
  WIFI_AUTH_WAPI_PSK: 8,
  WIFI_AUTH_MAX: 9
};

var EVENT_NAME_MAP = {
  WIFI_EVENT: 1,
  IP_EVENT: 2
};

var EVENT_NAME = Object.keys(EVENT_NAME_MAP).reduce(function(acc, cur) {
  acc[cur] = cur;
  return acc;
}, {});

var WIFI_EVENT_CODE = {
  WIFI_EVENT_WIFI_READY: 0,                /**< ESP32 WiFi ready */
  WIFI_EVENT_SCAN_DONE: 1,                 /**< ESP32 finish scanning AP */
  WIFI_EVENT_STA_START: 2,                 /**< ESP32 station start */
  WIFI_EVENT_STA_STOP: 3,                  /**< ESP32 station stop */
  WIFI_EVENT_STA_CONNECTED: 4,             /**< ESP32 station connected to AP */
  WIFI_EVENT_STA_DISCONNECTED: 5,          /**< ESP32 station disconnected from AP */
  WIFI_EVENT_STA_AUTHMODE_CHANGE: 6,       /**< the auth mode of AP connected by ESP32 station changed */

  WIFI_EVENT_STA_WPS_ER_SUCCESS: 7,        /**< ESP32 station wps succeeds in enrollee mode */
  WIFI_EVENT_STA_WPS_ER_FAILED: 8,         /**< ESP32 station wps fails in enrollee mode */
  WIFI_EVENT_STA_WPS_ER_TIMEOUT: 9,        /**< ESP32 station wps timeout in enrollee mode */
  WIFI_EVENT_STA_WPS_ER_PIN: 10,           /**< ESP32 station wps pin code in enrollee mode */
  WIFI_EVENT_STA_WPS_ER_PBC_OVERLAP: 11,   /**< ESP32 station wps overlap in enrollee mode */

  WIFI_EVENT_AP_START: 12,                 /**< ESP32 soft-AP start */
  WIFI_EVENT_AP_STOP: 13,                  /**< ESP32 soft-AP stop */
  WIFI_EVENT_AP_STACONNECTED: 14,          /**< a station connected to ESP32 soft-AP */
  WIFI_EVENT_AP_STADISCONNECTED: 15,       /**< a station disconnected from ESP32 soft-AP */
  WIFI_EVENT_AP_PROBEREQRECVED: 16,        /**< Receive probe request packet in soft-AP interface */

  WIFI_EVENT_FTM_REPORT: 17,               /**< Receive report of FTM procedure */

  /* Add next events after this only */
  WIFI_EVENT_STA_BSS_RSSI_LOW: 18,         /**< AP's RSSI crossed configured threshold */
  WIFI_EVENT_ACTION_TX_STATUS: 19,         /**< Status indication of Action Tx operation */
  WIFI_EVENT_ROC_DONE: 20,                 /**< Remain-on-Channel operation complete */

  WIFI_EVENT_STA_BEACON_TIMEOUT: 21,       /**< ESP32 station beacon timeout */

  WIFI_EVENT_MAX: 22                       /**< Invalid WiFi event ID */
};

var IP_EVENT_CODE = {
  IP_EVENT_STA_GOT_IP: 0,               /*!< station got IP from connected AP */
  IP_EVENT_STA_LOST_IP: 1,              /*!< station lost IP and the IP is reset to 0 */
  IP_EVENT_AP_STAIPASSIGNED: 2,         /*!< soft-AP assign an IP to a connected station */
  IP_EVENT_GOT_IP6: 3,                  /*!< station or ap or ethernet interface v6IP addr is preferred */
  IP_EVENT_ETH_GOT_IP: 4,               /*!< ethernet got IP from connected AP */
  IP_EVENT_PPP_GOT_IP: 5,               /*!< PPP interface got IP */
  IP_EVENT_PPP_LOST_IP: 6,              /*!< PPP interface lost IP */
};

function addListenEvent(name) {
  var code = EVENT_NAME_MAP[name];
  wifi.addListenEvent(code);
}

module.exports = {
  WIFI_MODE: WIFI_MODE,
  WIFI_AUTH_MODE: WIFI_AUTH_MODE,
  EVENT_NAME: EVENT_NAME,
  WIFI_EVENT_CODE: WIFI_EVENT_CODE,
  IP_EVENT_CODE: IP_EVENT_CODE,

  init: wifi.init,
  setMode: wifi.setMode,
  setConfig: wifi.setConfig,
  start: wifi.start,
  setEventListener: wifi.setEventListener,
  addListenEvent: addListenEvent,
  connect: wifi.connect
};
