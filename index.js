

var Socket = require('./socket');
var Serial = require('./serial');

var socket, serial;

serial = new Serial();
serial.start();
serial.on('pinChange', function (info) {

});

/*
socket = new Socket(2600);
socket.start(function (error, connection) {

});
*/
