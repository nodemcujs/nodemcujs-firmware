var wifi = require('wifi');
var nvsFlash = require('nvs_flash');
var espERROR = require('esp_error');

console.log('wifi app init');

function wifi_event(name, id, data) {
  console.log('receive event: %s %d %j', name, id, data);

  if (name === wifi.EVENT_NAME.WIFI_EVENT) {
    if (id === wifi.WIFI_EVENT_CODE.WIFI_EVENT_SCAN_DONE) {
      console.log('wifi scan done');
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
ret = wifi.setMode(wifi.WIFI_MODE.STA);
console.log('wifi setMode', ret);
ret = wifi.start();
console.log('wifi start', ret);
ret = wifi.scanSync({
  channel: 0,
  showHidden: false,
  scanType: wifi.SCAN_TYPE.ACTIVE
});
if (typeof ret === 'number') {
  console.log('wifi scan error', espERROR.esp_err_to_name(ret));
} else {
  console.log('wifi scan result', ret);
}