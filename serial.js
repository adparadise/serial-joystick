var SerialPort = require("serialport").SerialPort

var serial = new SerialPort("/dev/ttyACM0", {
  baudrate: 9600
});

serial.on("open", function () {
  console.log('open');
  serial.on('data', function(data) {
    console.log('data received: "' + data + '"');
  });
});
