var rmt = require("rmt");

rmt.setup(0, 2, { clock_div: 8, idle_output_en: false });
rmt.setPattern(0, 4, 1, 9, 0);
rmt.setPattern(1, 9, 1, 4, 0);

var leds = 12;

var buffer = new Array(3 * leds);

function drawPixel(index, r, g, b) {
  var i = index * 3;
  buffer[i] = g;
  buffer[i+1] = r;
  buffer[i+2] = b;
}

function clear(r, g, b) {
  for (var index = 0; index < leds; index++) {
    buffer[index * 3] = g;
    buffer[index * 3 + 1] = r;
    buffer[index * 3 + 2] = b;
  }
}

while(true) {
  for (var index = 0; index < leds; index++) {
    drawPixel(index, 0xff, 0, 0);
    rmt.sendSync(0, buffer);
    process.delay(300);
    drawPixel(index, 0, 0xff, 0);
    rmt.sendSync(0, buffer);
    process.delay(300);
    drawPixel(index, 0, 0, 0xff);
    rmt.sendSync(0, buffer);
    process.delay(300);
  }
  for (var delay = 0; delay < 300; delay += 20) {
    for (var pos = 0; pos < leds; pos++) {
      clear(0, 0, 0);
      drawPixel(pos, 255, 255, 255);
      rmt.sendSync(0, buffer);
      process.delay(300 - delay);
    }
  }
  for (var index = 0; index < 255; index++) {
    clear(index, 0, 0);
    rmt.sendSync(0, buffer);
    process.delay(16);
  }
  for (var index = 254; index >= 0; index--) {
    clear(index, 0, 0);
    rmt.sendSync(0, buffer);
    process.delay(16);
  }
  for (var index = 0; index < 255; index++) {
    clear(0, index, 0);
    rmt.sendSync(0, buffer);
    process.delay(16);
  }
  for (var index = 254; index >= 0; index--) {
    clear(0, index, 0);
    rmt.sendSync(0, buffer);
    process.delay(16);
  }
  for (var index = 0; index < 255; index++) {
    clear(0, 0, index);
    rmt.sendSync(0, buffer);
    process.delay(16);
  }
  for (var index = 254; index >= 0; index--) {
    clear(0, 0, index);
    rmt.sendSync(0, buffer);
    process.delay(16);
  }
}
