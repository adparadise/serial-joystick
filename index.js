
var Socket = require('./socket');
var Serial = require('./serial');

var socket, serial;

console.log('Beginning serialport to WebSocket bridge');

var portName = process.argv[2];

socket = new Socket(2600);
serial = new Serial();
serial.start(portName);

socket.start(function (error, connection) {
    if (error) {
        process.stderr.write(error);
        process.exit(1);
    }
    console.log('new connection established');

    serial.on('pinChange', serialHandler);

    connection.on('close', function () {
        serial.removeListener('pinChange', serialHandler);
    });

    function serialHandler (info) {
        connection.write(JSON.stringify(info));
    }
});


