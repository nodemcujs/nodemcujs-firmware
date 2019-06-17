var f = 'hello, i am a.js'

exports.a = function () {
    setInterval(function () {
        console.log('a.js: %s', f)
    }, 2000);
}
