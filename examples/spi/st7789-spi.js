/**
 * Copyright nodemcujs
 * 
 * Licensed under MIT License.
 * you may not use this file except in compliance with the License.
 */

/**
 * for st7789 resolution 240*240 by using 565 color format.
 * data will be lost if speed more than 40MHz.
 */

 var LED_PIN = 2;
 var CS_PIN = 15;
 var MOSI_PIN = 13;
 var MISO_PIN = 12;
 var CLK_PIN = 14;
 var DC_PIN = 4;
 var RST_PIN = 2;
 var INPUT = 1;
 var OUTPUT = 2;
 var HIGH = 1;
 var LOW = 0;
 
 var gpio = require('gpio');
 var SPI = require('spi');
 
 gpio.mode(RST_PIN, OUTPUT);
 
 var spi = new SPI.MASTER(2, {
   pinMOSI: MOSI_PIN,
   pinMISO: MISO_PIN,
   pinCLK: CLK_PIN,
   pinCS: CS_PIN,
   pinDC: DC_PIN,
   mode: 3,
   activeDC: 1,
   clockHz: 40000000,
   maxTransferSize: 120 * 2
 });
 
 var ok = spi.setup();
 console.log('setup spi:', ok);
 
 function lcd_init() {
   gpio.write(RST_PIN, LOW);
   process.delay(120);
   gpio.write(RST_PIN, HIGH);
 
   spi.sendCmdSync(0x36)
   spi.sendDataSync(0x00)
   spi.sendCmdSync(0x3A)
   spi.sendDataSync(0x05)
   spi.sendCmdSync(0xB2)
   spi.sendDataSync(0x0C)
   spi.sendDataSync(0x0C)
   spi.sendDataSync(0x00)
   spi.sendDataSync(0x33)
   spi.sendDataSync(0x33)
   spi.sendCmdSync(0xB7)
   spi.sendDataSync(0x35)
   spi.sendCmdSync(0xBB)
   spi.sendDataSync(0x19)
   spi.sendCmdSync(0xC0)
   spi.sendDataSync(0x2C)
   spi.sendCmdSync(0xC2)
   spi.sendDataSync(0x01)
   spi.sendCmdSync(0xC3)
   spi.sendDataSync(0x12)
   spi.sendCmdSync(0xC4)
   spi.sendDataSync(0x20)
   spi.sendCmdSync(0xC6) // normal mode
   spi.sendDataSync(0x0F)
   spi.sendCmdSync(0xD0)
   spi.sendDataSync(0xA4)
   spi.sendDataSync(0xA1)
   spi.sendCmdSync(0xE0)
   spi.sendDataSync(0xD0)
   spi.sendDataSync(0x04)
   spi.sendDataSync(0x0D)
   spi.sendDataSync(0x11)
   spi.sendDataSync(0x13)
   spi.sendDataSync(0x2B)
   spi.sendDataSync(0x3F)
   spi.sendDataSync(0x54)
   spi.sendDataSync(0x4C)
   spi.sendDataSync(0x18)
   spi.sendDataSync(0x0D)
   spi.sendDataSync(0x0B)
   spi.sendDataSync(0x1F)
   spi.sendDataSync(0x23)
   spi.sendCmdSync(0xE1)
   spi.sendDataSync(0xD0)
   spi.sendDataSync(0x04)
   spi.sendDataSync(0x0C)
   spi.sendDataSync(0x11)
   spi.sendDataSync(0x13)
   spi.sendDataSync(0x2C)
   spi.sendDataSync(0x3F)
   spi.sendDataSync(0x44)
   spi.sendDataSync(0x51)
   spi.sendDataSync(0x2F)
   spi.sendDataSync(0x1F)
   spi.sendDataSync(0x1F)
   spi.sendDataSync(0x20)
   spi.sendDataSync(0x23)
   spi.sendCmdSync(0x21) // Display Inversion On
   spi.sendCmdSync(0x11) // sleep out
   spi.sendCmdSync(0x29) // display on
 }
 
 function lcd_setRotate(direction) {
   spi.sendCmdSync(0x36); // MX, MY, RGB mode
   if (direction === 0) {
     spi.sendDataSync(0x00);
   } else {
     spi.sendDataSync(0x68);
   }
 }
 
 function lcd_setRegion(x1, y1, x2, y2) {
   spi.sendCmdSync(0x2a); // Column address set
   spi.sendDataSync(0x00);
   spi.sendDataSync(x1);
   spi.sendDataSync(0x00);
   spi.sendDataSync(x2);
   spi.sendCmdSync(0x2b); // Column address set
   spi.sendDataSync(0x00);
   spi.sendDataSync(y1);
   spi.sendDataSync(0x00);
   spi.sendDataSync(y2);
   spi.sendCmdSync(0x2c); // Memory write
 }
 
 function lcd_fill(color_h, color_l) {
   lcd_setRegion(0, 0, 239, 239);
   var buffer = [];
   for (var repeat = 0; repeat < 120; repeat++) {
     buffer = buffer.concat([color_h, color_l]);
   }
   for (var row = 0; row < 480; row++) {
     spi.sendDataSync(buffer);
   }
 }
 
 lcd_init();
 while (true) {
   lcd_setRotate(0);
   lcd_fill(0x00, 0x10);
   lcd_setRotate(1);
   // process.delay(600);
   lcd_fill(0x03, 0x30);
   lcd_setRotate(0);
   // process.delay(600);
   lcd_fill(0x03, 0x3f);
   lcd_setRotate(1);
   // process.delay(600);
   lcd_fill(0x63, 0x20);
   lcd_setRotate(0);
   // process.delay(600);
   lcd_fill(0x64, 0xc0);
   lcd_setRotate(1);
   // process.delay(600);
   lcd_fill(0xb8, 0x11);
   lcd_setRotate(0);
   // process.delay(600);
   lcd_fill(0xb8, 0x1f);
   lcd_setRotate(1);
   // process.delay(600);
   lcd_fill(0xfc, 0x80);
   lcd_setRotate(0);
   // process.delay(600);
   lcd_fill(0xfc, 0x9c);
   lcd_setRotate(1);
   // process.delay(600);
   lcd_fill(0x00, 0x00);
   lcd_setRotate(0);
   // process.delay(600);
   lcd_fill(0xff, 0xff);
   // process.delay(600);
 }
 