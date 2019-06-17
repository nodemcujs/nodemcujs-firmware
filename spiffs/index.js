var a = require("/spiffs/a.js");
var b = require("/spiffs/b.js");

a.a();
b();

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
