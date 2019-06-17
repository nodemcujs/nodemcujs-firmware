var f = 'hello, i am b.js'

module.exports = function () {
    setInterval(function () {
        console.log('b.js: %s', f)
    }, 2000);
}
