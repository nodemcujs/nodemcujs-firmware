/**
 * Copyright nodemcujs
 * 
 * Licensed under MIT License.
 * you may not use this file except in compliance with the License.
 */

/**
 * for st7735 resolution 128*128 by using 565 color format.
 * change mode value to numbe 3 if chip is st7735r.
 * data will be lost if speed more than 26MHz.
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
	mode: 2,
	activeDC: 1,
	clockHz: 20000000,
	maxTransferSize: 128 * 2
});

var ok = spi.setup();
console.log('setup spi:', ok);

function lcd_init() {
	gpio.write(RST_PIN, LOW);
	process.delay(120);
	gpio.write(RST_PIN, HIGH);
	// LCD Init For 1.44Inch LCD Panel with ST7735R.
	spi.sendCmdSync(0x11) //Sleep exit

	// ST7735R Frame Rate: Frame rate=fosc/((RTNA x 2 + 40) x (LINE + FPA + BPA +2)) fosc=850khz
	spi.sendCmdSync(0xB1)
	spi.sendDataSync(0x00) // RNTA
	spi.sendDataSync(0x00) // FPA
	spi.sendDataSync(0x00) // BPA

	// Frame Rate Control (In Idle mode/ 8-colors)
	// spi.sendCmdSync(0xB2)
	// spi.sendDataSync(0x01)
	// spi.sendDataSync(0x2C)
	// spi.sendDataSync(0x2D)

	// Frame Rate Control (In Partial mode/ full colors)
	// spi.sendCmdSync(0xB3)
	// spi.sendDataSync(0x01)
	// spi.sendDataSync(0x2C)
	// spi.sendDataSync(0x2D)
	// spi.sendDataSync(0x01)
	// spi.sendDataSync(0x2C)
	// spi.sendDataSync(0x2D)

	// Display Inversion Control
	spi.sendCmdSync(0xB4)
	spi.sendDataSync(0x07)

	// ST7735R Power Control 1
	spi.sendCmdSync(0xC0)
	spi.sendDataSync(0xA2)
	spi.sendDataSync(0x02)
	spi.sendDataSync(0x44)
	// Power Control 2
	spi.sendCmdSync(0xC1)
	spi.sendDataSync(0xC5)

	// Power Control 3 (in Normal mode/ Full colors)
	spi.sendCmdSync(0xC2)
	spi.sendDataSync(0x0A)
	spi.sendDataSync(0x00)
	// Power Control 4 (in Idle mode/ 8-colors)
	spi.sendCmdSync(0xC3)
	spi.sendDataSync(0x8A)
	spi.sendDataSync(0x2A)
	// Power Control 5 (in Partial mode/ full-colors)
	spi.sendCmdSync(0xC4)
	spi.sendDataSync(0x8A)
	spi.sendDataSync(0xEE)

	// VCOM Control 1
	spi.sendCmdSync(0xC5)
	spi.sendDataSync(0x0E)

	spi.sendCmdSync(0x36) //MX, MY, RGB mode
	spi.sendDataSync(0xC8)

	//ST7735R Gamma ('+'polarity) Correction Characteristics Setting
	spi.sendCmdSync(0xe0)
	spi.sendDataSync(0x0f)
	spi.sendDataSync(0x1a)
	spi.sendDataSync(0x0f)
	spi.sendDataSync(0x18)
	spi.sendDataSync(0x2f)
	spi.sendDataSync(0x28)
	spi.sendDataSync(0x20)
	spi.sendDataSync(0x22)
	spi.sendDataSync(0x1f)
	spi.sendDataSync(0x1b)
	spi.sendDataSync(0x23)
	spi.sendDataSync(0x37)
	spi.sendDataSync(0x00)
	spi.sendDataSync(0x07)
	spi.sendDataSync(0x02)
	spi.sendDataSync(0x10)
	// Gamma '-'polarity Correction Characteristics Setting
	spi.sendCmdSync(0xe1)
	spi.sendDataSync(0x0f)
	spi.sendDataSync(0x1b)
	spi.sendDataSync(0x0f)
	spi.sendDataSync(0x17)
	spi.sendDataSync(0x33)
	spi.sendDataSync(0x2c)
	spi.sendDataSync(0x29)
	spi.sendDataSync(0x2e)
	spi.sendDataSync(0x30)
	spi.sendDataSync(0x30)
	spi.sendDataSync(0x39)
	spi.sendDataSync(0x3f)
	spi.sendDataSync(0x00)
	spi.sendDataSync(0x07)
	spi.sendDataSync(0x03)
	spi.sendDataSync(0x10)

	// Column Address Set
	spi.sendCmdSync(0x2a)
	spi.sendDataSync(0x00)
	spi.sendDataSync(0x00 + 2)
	spi.sendDataSync(0x00)
	spi.sendDataSync(0x80 + 2)
	// Row Address Set
	spi.sendCmdSync(0x2b)
	spi.sendDataSync(0x00)
	spi.sendDataSync(0x00 + 3)
	spi.sendDataSync(0x00)
	spi.sendDataSync(0x80 + 3)

	spi.sendCmdSync(0xF0) //Enable test command
	spi.sendDataSync(0x01)
	spi.sendCmdSync(0xF6) //Disable ram power save mode
	spi.sendDataSync(0x00)

	spi.sendCmdSync(0x3A) // Data Color Coding
	spi.sendDataSync(0x05) // mode map: 0x03: 4k(444), 0x05: 65k(565), 0x06: 262k(666)

	spi.sendCmdSync(0x29) //Display on
}

function lcd_setRegion(x1, y1, x2, y2) {
	spi.sendCmdSync(0x2a)
	spi.sendDataSync(0x00)
	spi.sendDataSync(x1 + 2)
	spi.sendDataSync(0x00)
	spi.sendDataSync(x2 + 2)

	spi.sendCmdSync(0x2b)
	spi.sendDataSync(0x00)
	spi.sendDataSync(y1 + 3)
	spi.sendDataSync(0x00)
	spi.sendDataSync(y2 + 3)
	spi.sendCmdSync(0x2c)
}

function lcd_fill(color_h, color_l) {
	lcd_setRegion(0, 0, 127, 127);

	var buffer = [];
	for (var repeat = 0; repeat < 128; repeat++) {
		buffer = buffer.concat([color_h, color_l]);
	}
	for (var row = 0; row < 128; row++) {
		spi.sendDataSync(buffer);
	}
}

lcd_init();
while (true) {
	lcd_fill(0x00, 0x10);
	process.delay(600);
	lcd_fill(0x03, 0x30);
	process.delay(600);
	lcd_fill(0x03, 0x3f);
	process.delay(600);
	lcd_fill(0x63, 0x20);
	process.delay(600);
	lcd_fill(0x64, 0xc0);
	process.delay(600);
	lcd_fill(0xb8, 0x11);
	process.delay(600);
	lcd_fill(0xb8, 0x1f);
	process.delay(600);
	lcd_fill(0xfc, 0x80);
	process.delay(600);
	lcd_fill(0xfc, 0x9c);
	process.delay(600);
	lcd_fill(0x00, 0x00);
	process.delay(600);
	lcd_fill(0xff, 0xff);
	process.delay(600);
}
