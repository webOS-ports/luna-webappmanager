/* PalmSystemBridge */

function PalmServiceBridge() {
    // identify uniquely this object within the web app
    this.palmServiceBridgeId = window.PalmSystem.__nextPalmServiceBridgeId;
    window.PalmSystem.__nextPalmServiceBridgeId++;

    this.onservicecallback = function(message) {/*do nothing*/}
    this.call = function(uri, payload) {
        var _uri = uri || "";
        var _payload = payload || "";
        var result = window.PalmSystem._webOS.exec(this.onservicecallback, this.onservicecallback, "PalmSystem", "LS2Call", [this.palmServiceBridgeId, _uri, _payload]);
    }
    this.cancel = function() {
        window.PalmSystem._webOS.execWithoutCallback("PalmSystem", "LS2Cancel", [this.palmServiceBridgeId]);
    }
}

