'use strict';

var Native = require('native');
var path = Native.require('path');

function nodemcujs_module_t(id, parent) {
  this.id = id;
  this.exports = {};
  this.filename = null;
  this.parent = parent;
}

nodemcujs_module_t.cache = {};

nodemcujs_module_t.resolveModulePath = function(id, parent) {
  if (process.builtin_modules[id]) {
    return id;
  }

  if (parent && id === parent.id) {
    return false;
  }

  var filepath = false;
  if (id[0] === '/') {
    filepath = id;
  } else if (!parent) {
    filepath = path.join(process.cwd(), id);
  } else {
    var root = path.dirname(parent.filename);
    filepath = path.join(root, id);
  }

  return filepath;
}

nodemcujs_module_t.prototype.require = function(id) {
  return nodemcujs_module_t.load(id, this);
}

nodemcujs_module_t.load = function(id, parent) {
  var modulePath = nodemcujs_module_t.resolveModulePath(id, parent);
  var cachedModule = nodemcujs_module_t.cache[modulePath];
  if (cachedModule) {
    return cachedModule.exports;
  }
  if (process.builtin_modules[id]) {
    return Native.require(id)
  }

  if (!modulePath) {
    throw new Error('Module not found: ' + id)
  }
  
  var module = new nodemcujs_module_t(id, parent);
  module.filename = modulePath;
  nodemcujs_module_t.cache[modulePath] = module;

  var ext = path.extname(modulePath);
  if (ext === '.json') {
    var source = process.readSource(modulePath);
    module.exports = JSON.parse(source);
  } else {
    module.compile();
  }

  return module.exports;
}

nodemcujs_module_t.prototype.compile = function() {
  var __filename = this.filename;
  var __dirname = path.dirname(__filename);
  var fn = process.compile(__filename);

  var _require = this.require.bind(this);
  _require.cache = nodemcujs_module_t.cache;

  fn.apply(this.exports, [
    this.exports,             // exports
    _require,                 // require
    this,                     // module
    undefined,                // native
    __filename,               // __filename
    __dirname                 // __dirname
  ]);
}

nodemcujs_module_t.runMain = function() {
  nodemcujs_module_t.load('/index.js', undefined);
}

module.exports = nodemcujs_module_t;