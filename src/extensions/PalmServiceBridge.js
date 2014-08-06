/* PalmServiceBridge */

var __PalmSericeBridgeInstanceCounter = 0;
var __PalmServiceBridgeInstances = {};

__PalmServiceBridge_handleServiceResponse = function(instanceId, response) {
    var instance = __PalmServiceBridgeInstances[instanceId];
    if (typeof instance == "undefined")
        return;

    instance.onservicecallback(response);
}

function PalmServiceBridge() {
    this.onservicecallback = function(msg) { };

    // As we're creating a class here we need to manage mutiple instances on
    // both sites.
    this.instanceId = ++__PalmSericeBridgeInstanceCounter;
    __PalmServiceBridgeInstances[this.instanceId] = this;

    _webOS.execWithoutCallback("PalmServiceBridge", "createInstance", [this.instanceId]);
}

PalmServiceBridge.prototype.version = function() {
    return "1.1";
}

PalmServiceBridge.prototype.destroy = function() {
    _webOS.execWithoutCallback("PalmServiceBridge", "releaseInstance", [this.instanceId]);
}

PalmServiceBridge.prototype.call = function(method, url) {
    _webOS.execWithoutCallback("PalmServiceBridge", "call", [this.instanceId, method, url]);
}

PalmServiceBridge.prototype.cancel = function() {
    _webOS.execWithoutCallback("PalmServiceBridge", "cancel", [this.instanceId]);
}
