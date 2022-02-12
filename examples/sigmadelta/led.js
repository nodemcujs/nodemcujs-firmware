var sigmadelta = require('sigmadelta');

var LED_PIN = 2;
var CHANNEL = 0;

sigmadelta.setup(CHANNEL, duty, 0, LED_PIN);

var duty = 0;
var inc = 1;
setInterval(function() {
  sigmadelta.setDuty(CHANNEL, duty);

  duty += inc;
  if (duty == 127 || duty == -127) {
    inc = inc == 1 ? -1 : 1;
  }
}, 10);