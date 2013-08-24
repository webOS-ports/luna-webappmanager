/*
 *  Copyright 2011 Wolfgang Koller - http://www.gofg.at/
 *            2013 Simon Busch <morphis@gravedo.de>
 *
 *  Licensed under the Apache License, Version 2.0 (the "License");
 *  you may not use this file except in compliance with the License.
 *  You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 *  Unless required by applicable law or agreed to in writing, software
 *  distributed under the License is distributed on an "AS IS" BASIS,
 *  WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 *  See the License for the specific language governing permissions and
 *  limitations under the License.
 */

/*******************************************************************************
 * Private native interface to communicate with the application process
 ******************************************************************************/

_webOS = {
    plugins: {},
    constructors: {},
    callbacks: {},
};

var callbackId = 1;

_webOS.callback = function() {
    var scId = arguments[0];
    var callbackRef = null;

    var parameters = [];
    for (var i = 1; i < arguments.length; i++) {
        parameters[i-1] = arguments[i];
    }
    callbackRef = _webOS.callbacks[scId];

    // Even IDs are success-, odd are error-callbacks - make sure we remove both
    if ((scId % 2) !== 0) {
        scId = scId - 1;
    }
    // Remove both the success as well as the error callback from the stack
    delete _webOS.callbacks[scId];
    delete _webOS.callbacks[scId + 1];

    if (typeof callbackRef == "function") callbackRef.apply(this, parameters);
};

_webOS.callbackWithoutRemove = function() {
    var scId = arguments[0];
    var callbackRef = null;

    var parameters = [];
    for (var i = 1; i < arguments.length; i++) {
        parameters[i-1] = arguments[i];
    }
    callbackRef = _webOS.callbacks[scId];

    if (typeof(callbackRef) == "function") callbackRef.apply(this, parameters);
};

/**
 * Execute a call to a plugin function
 * @return bool true on success, false on error (e.g. function doesn't exist)
 */
_webOS.exec = function(successCallback, errorCallback, pluginName, functionName, parameters) {
    if (callbackId % 2) {
        callbackId++;
    }

    // Store a reference to the callback functions
    var scId = callbackId++;
    var ecId = callbackId++;
    _webOS.callbacks[scId] = successCallback;
    _webOS.callbacks[ecId] = errorCallback;

    // if no parameters are supplied create an empty array
    if (typeof parameters === 'undefined')
        parameters = [];

    parameters.unshift(ecId);
    parameters.unshift(scId);

    navigator.qt.postMessage(JSON.stringify({messageType: "callPluginFunction", plugin: pluginName, func: functionName, params: parameters}))
    return true;
}

/**
 * Execute a call to a plugin function
 * @return bool true on success, false on error (e.g. function doesn't exist)
 */
_webOS.execWithoutCallback = function(pluginName, functionName, parameters) {
    // if no parameters are supplied create an empty array
    if (typeof parameters === 'undefined')
        parameters = [];

    navigator.qt.postMessage(JSON.stringify({messageType: "callPluginFunction", plugin: pluginName, func: functionName, params: parameters}))
    return true;
}

var unusedCallback = function() { }

/*******************************************************************************
 * Extensions to the DOM window object
 *******************************************************************************/

__systemWindowOpen = window.open;

window.open = function(url, name, properties) {
    var newWindow = {
        events: { },
        addEventListener: function (name, callback) { },
        removeEventListener: function(name, callback) { },
        focus: function() { },
        close: function() { },
    };

    return newWindow;
}

/*******************************************************************************
 * PalmServiceBridge
 ******************************************************************************/

var __PalmSericeBridgeInstanceCounter = 0;

function PalmServiceBridge() {
    this.onservicecallback = null;

    // As we're creating a class here we need to manage mutiple instances on
    // both sites.
    this.instanceId = ++__PalmSericeBridgeInstanceCounter;
    _webOS.exec(unusedCallback, unusedCallback, "PalmServiceBridge", "createInstance", [this.instanceId]);
}

PalmServiceBridge.prototype.destroy = function() {
    _webOS.exec(unusedCallback, unusedCallback, "PalmServiceBridge", "releaseInstance", [this.instanceId]);
}

PalmServiceBridge.prototype.call = function(method, url) {
    function callback(msg) {
        if (this.onservicecalback !== null)
            this.onservicecallback(msg)
    };

    _webOS.exec(callback, callback, "PalmServiceBridge", "call", [this.instanceId, method, url]);
}

PalmServiceBridge.prototype.cancel = function() {
    _webOS.exec(unusedCallback, unusedCallback, "PalmServiceBridge", "cancel", [this.instanceId]);
}


/*******************************************************************************
 * PalmSystem
 ******************************************************************************/

window.PalmSystem = {}

/**
 * Application launch parameters as stringified json
 **/

/* This is our internal reprensentation for all properties */
__PalmSystem = {};
__PalmSystem.launchParams = "";
__PalmSystem.locale = "";
__PalmSystem.localeRegion = "";
__PalmSystem.timeFormat = "";
__PalmSystem.timeZone = "";
__PalmSystem.isMinimal = false;
__PalmSystem.identifier = "";
__PalmSystem.version = "";
__PalmSystem.screenOrientation = "";
__PalmSystem.specifiedWindowOrientation = "";
__PalmSystem.videoOrientation = "";
__PalmSystem.isActivated = true;
__PalmSystem.activityId = 0;
__PalmSystem.phoneRegion = "";
__PalmSystem.windowOrientation = "";
__PalmSystem.hasAlphaHole = false;

/* Some internal settings we need for the implementation */
__PalmSystem.bannerMessageCounter = 0;

/* Register our property change event handler */
_webOS.exec(function(name, value) {
        if (!__PalmSystem.hasOwnProperty(name))
            return;
        __PalmSystem[name] = value;
    },
    unusedCallback, "PalmSystem", "registerPropertyChangeHandler");

/* read-only */

Object.defineProperty(window.PalmSystem, "launchParams", {
  get: function() { return __PalmSystem.launchParams; }
});

Object.defineProperty(window.PalmSystem, "locale", {
  get: function() { return __PalmSystem.locale; }
});

Object.defineProperty(window.PalmSystem, "localeRegion", {
  get: function() { return __PalmSystem.localeRegion; }
});

Object.defineProperty(window.PalmSystem, "timeFormat", {
  get: function() { return __PalmSystem.timeFormat; }
});

Object.defineProperty(window.PalmSystem, "timeZone", {
  get: function() { return __PalmSystem.timeZone; }
});

Object.defineProperty(window.PalmSystem, "isMinimal", {
  get: function() { return __PalmSystem.isMinimal; }
});

Object.defineProperty(window.PalmSystem, "identifier", {
  get: function() { return __PalmSystem.identifier; }
});

Object.defineProperty(window.PalmSystem, "version", {
  get: function() { return __PalmSystem.version; }
});

Object.defineProperty(window.PalmSystem, "screenOrientation", {
  get: function() { return __PalmSystem.screenOrientation; }
});

Object.defineProperty(window.PalmSystem, "specifiedWindowOrientation", {
  get: function() { return __PalmSystem.specifiedWindowOrientation; }
});

Object.defineProperty(window.PalmSystem, "videoOrientation", {
  get: function() { return __PalmSystem.specifiedWindowOrientation; }
});
Object.defineProperty(window.PalmSystem, "isActivated", {
  get: function() { return __PalmSystem.specifiedWindowOrientation; }
});

Object.defineProperty(window.PalmSystem, "activityId", {
  get: function() { return __PalmSystem.specifiedWindowOrientation; }
});

Object.defineProperty(window.PalmSystem, "phoneRegion", {
  get: function() { return __PalmSystem.specifiedWindowOrientation; }
});

/* read-write */
Object.defineProperty(window.PalmSystem, "windowOrientation", {
  get: function() { return __PalmSystem.windowOrientation; },
  set: function(value) {
      _webOS.exec(unusedCallback, unusedCallback, "PalmSystem", "setProperty", ["hasAlphaRole", value]);
  }
});

Object.defineProperty(window.PalmSystem, "hasAlphaHole", {
  get: function() { return __PalmSystem.hasAlphaHole; },
  set: function(value) {
      _webOS.exec(unusedCallback, unusedCallback, "PalmSystem", "setProperty", ["hasAlphaRole", value]);
  }
});

PalmSystem.getIdentifier = function() {
    return __PalmSystem.identifier;
}

PalmSystem.addBannerMessage = function(msg, params, icon, soundClass, soundFile, duration, doNotSuppress) {
    var id = ++__PalmSystem.bannerMessageCounter;
    _webOS.execWithoutCallback("PalmSystem", "addBannerMessage",
        [id, msg, params, icon, soundClass, soundFile, duration, doNotSuppress]);
    return id.toString();
}

PalmSystem.activate = function() {
    _webOS.execWithoutCallback("PalmSystem", "activate");
}

PalmSystem.deactivate = function() {
    _webOS.execWithoutCallback("PalmSystem", "deactivate");
}

PalmSystem.stagePreparing = function() {
    _webOS.execWithoutCallback("PalmSystem", "stagePreparing");
}

PalmSystem.stageReady = function() {
    _webOS.execWithoutCallback("PalmSystem", "stageReady");
}

PalmSystem.show = function() {
    _webOS.execWithoutCallback("PalmSystem", "show");
}

PalmSystem.hide = function() {
    _webOS.execWithoutCallback("PalmSystem", "hide");
}

PalmSystem.setWindowProperties = function(props) {
    webOS.execWithoutCallback("PalmSystem", "setWindowProperties", [props]);
}

PalmSystem.enableFullScreenMode = function() {
    webOS.execWithoutCallback("PalmSystem", "enableFullScreenMode");
}
