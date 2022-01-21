'use strict';

var Native = require('native');
var spi = Native.require('spi_master');
var util = Native.require('util');

function MASTER(id, options) {
  this.id = id;
  this.options = options;
}

MASTER.prototype.setup = function() {
  return spi.setup(this.id, this.options);
}

MASTER.prototype.sendDataSync = function(data) {
  if (typeof data !== 'number' && !util.isArray(data)) {
    throw new TypeError('The parameter data must be a Number or Array');
  }
  return spi.sendSync(this.id, 1, typeof data === 'number' ? [data] : data);
}

MASTER.prototype.sendCmdSync = function(cmd) {
  if (typeof cmd !== 'number' && !util.isArray(cmd)) {
    throw new TypeError('The parameter cmd must be a Number or Array');
  }
  return spi.sendSync(this.id, 0, typeof cmd === 'number' ? [cmd] : cmd);
}

module.exports = {
  MASTER: MASTER
};