'use strict';

var Native = require('native');
var util = Native.require('util');

function Console() {
  this.stdout = native.stdout;
  this.stderr = native.stderr;
};

Console.prototype.log =
Console.prototype.info =
Console.prototype.warn =
Console.prototype.error =
Console.prototype.debug = function() {
  if (arguments.length === 1) {
    native.stdout(util.formatValue(arguments[0]) + '\n');
  } else {
    native.stdout(util.format.apply(this, arguments) + '\n');
  }
};

var console = new Console();

module.exports = console;