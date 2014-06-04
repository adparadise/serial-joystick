var http = require('http');
var sockjs = require('sockjs');

module.exports = Socket;

function Socket () {
    this.initialize.apply(this, arguments);
}

(function (proto) {
    proto.initialize = function (port) {
        this.port = port;
        this.urlPrefix = '/joystick';
    };

    proto.start = function (callback) {
        var self = this;

        this.echo = sockjs.createServer();
        this.echo.on('connection', function(conn) {
            self.conn = conn;
            conn.on('close', function() {
                console.log("Game disconnected");
            });

            callback(undefined, conn);
        });

        this.server = http.createServer();
        this.echo.installHandlers(this.server, {
            prefix: this.urlPrefix
        });
        this.server.listen(this.port, '0.0.0.0');
        console.log('listening on port: ' + this.port);
    };
}(Socket.prototype));
