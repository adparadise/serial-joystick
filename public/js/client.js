

function Client () {
    this.initialize.apply(this, arguments);
}

(function (proto) {
    proto.initialize = function (hostname) {
        this.hostname = hostname;
        this.onopen = this.onopen.bind(this);
        this.onmessage = this.onmessage.bind(this);
        this.onclose = this.onclose.bind(this);
    };

    proto.connect = function () {
        this.sock = new SockJS('http://' + this.hostname + '/joystick');
        this.sock.onopen = this.onopen;
        this.sock.onmessage = this.onmessage;
        this.sock.onclose = this.onclose;
    };

    proto.onopen = function () {
        console.log('connection established');
    };

    proto.onmessage = function (event) {
        console.log(event.data);
    };

    proto.onclose = function () {
        console.log('connection broken');
    };
}(Client.prototype));
