var SerialPort = require("serialport");


function findArduinoPortName (callback) {
    var isFound;

    SerialPort.list(function (error, ports) {
        var arduinoPattern = /Arduino/;
        var index, port, isFound;
        if (error) {
            return callback(error);
        }

        for (index = 0; index < ports.length; index++) {
            port = ports[index];
            if (arduinoPattern.exec(port.pnpId)) {
                callback(undefined, port.comName);
                isFound = true;
                break;
            }
        }
        if (!isFound) {
            callback('not found');
        }
    });
}

function handler (error, portName) {
    if (error) {
        console.log(error);
        return;
    }
    if (portName) {
        console.log("connecting to: ", portName);
        connect(portName);
    }
};

findArduinoPortName(handler);

function connect(portName) {
    var serial = new SerialPort.SerialPort(portName, {
        baudrate: 9600
    });

    serial.on("open", function () {
        var index, bitsStringList, bits, bitsString, position;
        console.log('open');
        bitsStringList = ['00000000','00000000','00000000','00000000'];
        serial.on('data', function(buffer) {
            var changed;

            changed = [];
            for (index = 0; index < buffer.length; index++) {
                bits = buffer.get(index);
                position = (bits & 0xc0) >> 6;
                bitsString = bits.toString(2);
                bitsString = ("00000000" + bitsString).slice(-8);
                bitsStringList[position] = bitsString;
                changed[position] = true;
            }
            process.stdout.write("'");
            for (position = 0; position < bitsStringList.length; position++) {
                if (position > 0) {
                    process.stdout.write(',');
                }
                if (changed[position]) {
                    process.stdout.write('\x1b[7m');
                }
                process.stdout.write(bitsStringList[position]);
                if (changed[position]) {
                    process.stdout.write('\x1b[0m');
                }
            }
            process.stdout.write("'\n");
        });
    });
    serial.on('close', function () {
        console.log(arguments);
    });
    serial.on('error', function (error) {
        console.log(error);
    });
}
