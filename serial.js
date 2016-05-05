var SerialPort = require("serialport");
var Events = require('events');
var Util = require('util');

module.exports = Serial;

function Serial () {
    this.initialize.apply(this, arguments);
}

Util.inherits(Serial, Events.EventEmitter);

Serial.findArduinoPortName = function (callback) {
    var isFound;

    SerialPort.list(function (error, ports) {
        var arduinoPattern = /Arduino/;
        var osxPattern = /tty\.usbmodem/;
        var index, port, isFound;
        if (error) {
            return callback(error);
        }

        if (ports.length === 0) {
            return callback(new Error('no serial ports found'));
        }
        for (index = 0; index < ports.length; index++) {
            port = ports[index];
            if (arduinoPattern.exec(port.pnpId) ||
                osxPattern.exec(port.pnpId)) {
                callback(undefined, port.comName);
                isFound = true;
                break;
            }
        }
        if (!isFound) {
            callback('not found');
        }
    });
};

(function (proto) {
    proto.initialize = function () {
        this.bitsStringList = ['00000000','01000000','10000000','11000000'];

        this.arduinoFoundHandler = this.arduinoFoundHandler.bind(this);
        this.dataHandler = this.dataHandler.bind(this);
        this.closeHandler = this.closeHandler.bind(this);
        this.errorHandler = this.errorHandler.bind(this);
    };

    proto.start = function (portName) {
        var self = this;
        if (portName) {
            process.nextTick(function () {
                self.connect(portName);
            });
        } else {
            Serial.findArduinoPortName(this.arduinoFoundHandler);
        }
    };

    proto.arduinoFoundHandler = function (error, portName) {
        if (error) {
            console.log(error);
            return;
        }
        if (portName) {
            console.log("connecting to: ", portName);
            this.connect(portName);
        }
    };

    proto.connect = function (portName) {
        var self = this;
        var serial, options;
        options = {
            baudrate: 9600
        };

        serial = new SerialPort.SerialPort(portName, options);
        serial.on("open", function () {
            console.log('open');

            serial.on('data', self.dataHandler);
            serial.on('close', self.closeHandler);
            serial.on('error', self.errorHandler);
        });
    };

    proto.dataHandler = function(buffer) {
        var index, bits, bitsString, position;
        var changedBytes, changedFields;

        changedBytes = [];
        for (index = 0; index < buffer.length; index++) {
            bits = buffer.get(index);
            position = (bits & 0xc0) >> 6;
            bitsString = bits.toString(2);
            bitsString = ("00000000" + bitsString).slice(-8);
            if (this.bitsStringList[position] != bitsString) {
                changedBytes[position] = true;
                this.emitChanges(position, bitsString);
            }
            this.bitsStringList[position] = bitsString;
        }

        this.logBitsStringList(changedBytes);
    };

    proto.emitChanges = function (position, bitsString) {
        var priorBitsString, index, priorBit, bit, address;

        priorBitsString = this.bitsStringList[position];

        for (index = 0; index <= 6; index++) {
            bit = bitsString[8 - index];
            priorBit = priorBitsString[8 - index];
            if (bit !== priorBit) {
                address = position * 6 + index;
                this.emit('pinChange', {
                    address: address,
                    value: bit === "1"
                });
            }
        }
    };

    proto.logBitsStringList = function (changed) {
        var position;

        changed = changed || {};
        process.stdout.write("'");
        for (position = 0; position < this.bitsStringList.length; position++) {
            if (position > 0) {
                process.stdout.write(',');
            }
            if (changed[position]) {
                process.stdout.write('\x1b[7m');
            }
            process.stdout.write(this.bitsStringList[position]);
            if (changed[position]) {
                process.stdout.write('\x1b[0m');
            }
        }
        process.stdout.write("'\n");
    };

    proto.closeHandler = function () {
        console.log(arguments);
    };

    proto.errorHandler = function (error) {
        console.log(error);
    };
}(Serial.prototype));

