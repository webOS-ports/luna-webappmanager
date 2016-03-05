__BluetoothManager = {};
__BluetoothManager.powered = false;

navigator.BluetoothManager = {};

navigator.BluetoothManager.onenabled = null;
navigator.BluetoothManager.ondisabled = null;

navigator.BluetoothManager.ondevicefound = null;
navigator.BluetoothManager.ondevicechanged = null;
navigator.BluetoothManager.ondeviceremoved = null;
navigator.BluetoothManager.ondevicedisappeared = null;
navigator.BluetoothManager.onpropertychanged = null;

navigator.BluetoothManager.onrequestpincode = null;
navigator.BluetoothManager.onrequestpasskey = null;
navigator.BluetoothManager.onconfirmpasskey = null;

__BluetoothManager.setPowered = function(powered) {
    __BluetoothManager.powered = powered;

    _webOS.execWithoutCallback("BluetoothManager", "setPowered", [powered]);

    if (powered && typeof navigator.BluetoothManager.onenabled === 'function')
        navigator.BluetoothManager.onenabled();
    else if (!powered && typeof navigator.BluetoothManager.ondisabled === 'function')
        navigator.BluetoothManager.ondisabled();
}

Object.defineProperty(navigator.BluetoothManager, "enabled", {
  get: function() { return __BluetoothManager.powered; },
  set: function(value) { __BluetoothManager.setPowered(value); }
});

__BluetoothManager.deviceFound = function(deviceInfo) {
    if (typeof navigator.BluetoothManager.ondevicefound === 'function')
      navigator.BluetoothManager.ondevicefound(deviceInfo);
}

__BluetoothManager.deviceChanged = function(deviceInfo) {
    if (typeof navigator.BluetoothManager.ondevicechanged === 'function')
      navigator.BluetoothManager.ondevicechanged(deviceInfo);
}

__BluetoothManager.deviceRemoved = function(address) {
    if (typeof navigator.BluetoothManager.ondeviceremoved === 'function')
      navigator.BluetoothManager.ondeviceremoved(address);
}

__BluetoothManager.deviceDisappeared= function(address) {
    if (typeof navigator.BluetoothManager.ondevicedisappeared === 'function')
      navigator.BluetoothManager.ondevicedisappeared(address);
}

__BluetoothManager.propertyChanged = function(key, value) {
    if (typeof navigator.BluetoothManager.onpropertychanged === 'function')
      navigator.BluetoothManager.onpropertychanged(key, value);
}

__BluetoothManager.requestPinCode= function(deviceInfo) {
    if (typeof navigator.BluetoothManager.onrequestpincode === 'function')
      navigator.BluetoothManager.onrequestpincode(deviceInfo);
}

__BluetoothManager.requestPasskey = function(deviceInfo) {
    if (typeof navigator.BluetoothManager.onrequestpasskey === 'function')
      navigator.BluetoothManager.onrequestpasskey(deviceInfo);
}

__BluetoothManager.requestConfirmPasskey= function(deviceInfo) {
    if (typeof navigator.BluetoothManager.onconfirmpasskey === 'function')
      navigator.BluetoothManager.onconfirmpasskey(deviceInfo);
}

navigator.BluetoothManager.discover = function(value) {
    _webOS.execWithoutCallback("BluetoothManager", "discover", [value]);
}

navigator.BluetoothManager.connectDevice = function(address) {
    _webOS.execWithoutCallback("BluetoothManager", "connectDevice", [address]);
}

navigator.BluetoothManager.disconnectDevice = function(address) {
    _webOS.execWithoutCallback("BluetoothManager", "disconnectDevice", [address]);
}

navigator.BluetoothManager.removeDevice = function(address) {
    _webOS.execWithoutCallback("BluetoothManager", "removeDevice", [address]);
}

navigator.BluetoothManager.resetDevicesList = function() {
    _webOS.execWithoutCallback("BluetoothManager", "resetDevicesList");
}

navigator.BluetoothManager.providePinCode = function(tag, provided, pinCode) {
    _webOS.execWithoutCallback("BluetoothManager", "providePinCode", [tag, provided, pinCode]);
}

navigator.BluetoothManager.providePasskey = function(tag, provided, passkey) {
    _webOS.execWithoutCallback("BluetoothManager", "providePasskey", [tag, provided, passkey]);
}

navigator.BluetoothManager.confirmPasskey = function(tag, confirmed) {
    _webOS.execWithoutCallback("BluetoothManager", "confirmPasskey", [tag, confirmed]);
}

navigator.BluetoothManager.displayPasskeyCallback = function(tag,confirmed) {
    _webOS.execWithoutCallback("BluetoothManager", "displayPasskeyCallback", [tag]);
}
