(function nodemcujs() {
  this.global = this;

  function Module(id) {
    this.id = id;
    this.exports = {};
  }
  Module.cache = {};
  Module.require = function(id) {
    if (id === 'native') {
      return Module;
    }

    if (Module.cache[id]) {
      return Module.cache[id].exports;
    }

    var module = new Module(id);

    Module.cache[id] = module;
    module.compile();

    return module.exports;
  };
  Module.prototype.compile = function() {
    process.compileModule(this, Module.require);
  };

  global.console = Module.require('console');
  process.stdout = {
    isTTY: false,
    write: console.stdout
  };
  process.stderr = {
    isTTY: false,
    write: console.stderr
  };
  var timers = Module.require('timers');
  global.setInterval = timers.setInterval;
  global.setTimeout = timers.setTimeout;
  global.clearInterval = timers.clearInterval;
  global.clearTimeout = timers.clearTimeout;
  process.delay = timers.delay;

  var EventEmitter = Module.require('events').EventEmitter;
  EventEmitter.call(process);
  Object.keys(EventEmitter.prototype).forEach(function(key) {
    if (!process[key]) {
      process[key] = EventEmitter.prototype[key];
    }
  });
  process.emitWarning = function(warning, type, code, ctor) {
    process.emit('warning', warning, type, code, ctor);
  };

  console.log("nodemcujs version: %s", process.version);

  Module.require('module').runMain();

})();
