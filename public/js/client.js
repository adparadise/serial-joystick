

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

    proto.setBinding = function (binding, allInverted) {
        this.binding = binding;
        this.allInverted = allInverted;
    };

    proto.setEventListener = function (eventListener) {
        this.eventListener = eventListener;
    };

    proto.log = function () {
        console.log.apply(console, arguments);
    };

    proto.connect = function () {
        this.sock = new SockJS('http://' + this.hostname + '/joystick');
        this.sock.onopen = this.onopen;
        this.sock.onmessage = this.onmessage;
        this.sock.onclose = this.onclose;
    };

    proto.onopen = function () {
        this.log('connection established');
    };

    proto.onmessage = function (event) {
        var info, keybind;
        var isOn, eventName, keyboardEvent;

        if (!this.binding) {
            console.log('no bindings bound!');
            return;
        }
        if (!this.eventListener) {
            console.log('no eventListener bound!');
            return;
        }

        info = JSON.parse(event.data);
        keybind = this.binding[info.address];
        if (!keybind) {
            this.log('no binding found for address: ', info.address);
            return;
        }

        isOn = info.value;
        if (this.allInverted || keybind.isInverted) {
            isOn = !isOn;
        }
        eventName = isOn ? 'keydown' : 'keyup';

        keyboardEvent = new CustomEvent(eventName);
        keyboardEvent.keyCode = keybind.keyCode;
        this.eventListener.dispatchEvent(keyboardEvent);
    };

    proto.onclose = function () {
        this.log('connection broken');
    };
}(Client.prototype));
