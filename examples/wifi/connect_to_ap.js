var SSID = 'your xxx_2.4G wifi';
var PASS = 'your password here';

var wifi = require('wifi');
var nvsFlash = require('nvs_flash');
var espERROR = require('esp_error');

console.log('wifi app init');

function wifi_event(name, id, data) {
  console.log('receive event: %s %d %j', name, id, data);
  var ret;
  if (name === wifi.EVENT_NAME.WIFI_EVENT) {
    if (id === wifi.WIFI_EVENT_CODE.WIFI_EVENT_STA_START) {
      ret = wifi.connect();
      console.log('wifi connecting to AP', ret);
    }
  }
}

var ret;
ret = nvsFlash.init();
console.log('nvs flash init', ret, espERROR.esp_err_to_name(ret));
ret = wifi.init(wifi.WIFI_MODE.STA);
console.log('wifi init', ret, espERROR.esp_err_to_name(ret));
ret = wifi.setEventListener(wifi_event);
console.log('wifi setEventListener', ret);
ret = wifi.addListenEvent(wifi.EVENT_NAME.WIFI_EVENT);
console.log('wifi addListenEvent WIFI_EVENT', ret);
ret = wifi.addListenEvent(wifi.EVENT_NAME.IP_EVENT);
console.log('wifi addListenEvent IP_EVENT', ret);
ret = wifi.setMode(wifi.WIFI_MODE.STA);
console.log('wifi setMode', ret);
ret = wifi.setConfig(wifi.WIFI_MODE.STA, {
  ssid: SSID,
  password: PASS,
  auth: wifi.WIFI_AUTH_MODE.WIFI_AUTH_WPA2_PSK
});
console.log('wifi setConfig', ret);
ret = wifi.start();
console.log('wifi start', ret);