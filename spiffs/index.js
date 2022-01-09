var LED_PIN = 2;
var INPUT = 1;
var OUTPUT = 2;
var HIGH = 1;
var LOW = 0;

var gpio = require('gpio');
gpio.mode(LED_PIN, OUTPUT);

console.log("staring sort test...");

function sort(arr) {
  var n = arr.length;
  var temp = null;
  for (var i = 0; i < n - 1; i++) {
    for (var j = 0; j < n - 1 - i; j++) {
      if (arr[j] > arr[j + 1]) {
        temp = arr[j];
        arr[j] = arr[j + 1];
        arr[j + 1] = temp;
      }
    }
  }
  return arr;
}

var testArray = [49, 38, 65, 97, 76, 13, 27, 49, 38, 65, 97, 76, 13, 27];
var startTime = new Date();

var index = 0;
for (index = 0; index <= 99; index++) {
  sort(testArray);
}
console.log("sort 100 times: %dms", (new Date() - startTime));

var state = HIGH;

setInterval(function() {
  gpio.write(LED_PIN, state);
  state = state === HIGH ? LOW : HIGH;
}, 1000);