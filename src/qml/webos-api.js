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

/**
 * Execute a synchronous call to a plugin function
 * @return string response data
 */
_webOS.execSync = function(pluginName, functionName, parameters) {
    if (typeof parameters === 'undefined')
      parameters = [];

    return navigator.qt.postSyncMessage(JSON.stringify({messageType: "callSyncPluginFunction", plugin: pluginName, func: functionName, params: parameters}));
}

var unusedCallback = function() { }

_webOS.relaunch = function(parameters) {
    __PalmSystem.launchParams = parameters;
    if (Mojo && Mojo.relaunch)
        Mojo.relaunch();
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
__PalmSystem.launchParams = "{}";
__PalmSystem.hasAlphaHole = false;
__PalmSystem.locale = "en";
__PalmSystem.localeRegion = "us";
__PalmSystem.timeFormat = "HH12";
__PalmSystem.timeZone = "Etc/UTC";
__PalmSystem.isMinimal = false;
__PalmSystem.identifier = "";
__PalmSystem.version = "";
__PalmSystem.screenOrientation = "";
__PalmSystem.windowOrientation = "";
__PalmSystem.specifiedWindowOrientation = "";
__PalmSystem.videoOrientation = "";
__PalmSystem.deviceInfo = "{\"modelName\":\"unknown\",\"platformVersion\":\"0.0.0\"}";
__PalmSystem.isActivated = true;
__PalmSystem.activityId = 0;
__PalmSystem.phoneRegion = "";

/* Some internal settings we need for the implementation */
__PalmSystem.bannerMessageCounter = 0;

/* Retrieve values for all properties on startup */
_webOS.exec(function(properties) {
        if (typeof properties === "undefined")
            return;

        __PalmSystem.launchParams = properties.launchParams;
        __PalmSystem.hasAlphaHole = properties.hasAlphaHole;
        __PalmSystem.locale = properties.locale;
        __PalmSystem.localeRegion = properties.localeRegion;
        __PalmSystem.timeFormat = properties.timeFormat;
        __PalmSystem.timeZone = properties.timeZone;
        __PalmSystem.isMinimal = properties.isMinimal;
        __PalmSystem.identifier = properties.identifier;
        __PalmSystem.version = properties.version;
        __PalmSystem.screenOrientation = properties.screenOrientation;
        __PalmSystem.windowOrientation = properties.windowOrientation;
        __PalmSystem.specifiedWindowOrientation = properties.specifiedWindowOrientation;
        __PalmSystem.videoOrientation = properties.videoOrientation;
        __PalmSystem.deviceInfo = properties.deviceInfo;
        __PalmSystem.isActivated = properties.isActivated;
        __PalmSystem.activityId = properties.activityId;
        __PalmSystem.phoneRegion = properties.phoneRegion;
    },
    unusedCallback, "PalmSystem", "initializeProperties");

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

Object.defineProperty(window.PalmSystem, "hasAlphaHole", {
  get: function() { return __PalmSystem.hasAlphaHole; },
  set: function(value) {
      _webOS.exec(unusedCallback, unusedCallback, "PalmSystem", "setProperty", ["hasAlphaHole", value]);
  }
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

Object.defineProperty(window.PalmSystem, "windowOrientation", {
  get: function() { return __PalmSystem.windowOrientation; },
  set: function(value) {
      _webOS.exec(unusedCallback, unusedCallback, "PalmSystem", "setProperty", ["windowOrientation", value]);
  }
});

Object.defineProperty(window.PalmSystem, "specifiedWindowOrientation", {
  get: function() { return __PalmSystem.specifiedWindowOrientation; }
});

Object.defineProperty(window.PalmSystem, "videoOrientation", {
  get: function() { return __PalmSystem.videoOrientation; }
});

Object.defineProperty(window.PalmSystem, "deviceInfo", {
  get: function() { return __PalmSystem.deviceInfo; }
});

Object.defineProperty(window.PalmSystem, "isActivated", {
  get: function() { return __PalmSystem.isActivated; }
});

Object.defineProperty(window.PalmSystem, "activityId", {
  get: function() { return __PalmSystem.activityId; }
});

Object.defineProperty(window.PalmSystem, "phoneRegion", {
  get: function() { return __PalmSystem.phoneRegion; }
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

PalmSystem.removeBannerMessage = function(id) {
    _webOS.execWithoutCallback("PalmSystem", "removeBannerMessage", [id]);
}

PalmSystem.clearBannerMessages = function() {
    _webOS.execWithoutCallback("PalmSystem", "clearBannerMessages");
}

PalmSystem.playSoundNotification = function(soundClass, file, duration, wakeUpScreen) {
    _webOS.execWithoutCallback("PalmSystem", "playSoundNotification", [soundClass, file, duration, wakeUpScreen]);
}

PalmSystem.simulateMouseClick = function(pageX, pageY, pressed) {
    _webOS.execWithoutCallback("PalmSystem", "simulateMouseClick", [pageX, pageY, pressed]);
}

PalmSystem.paste = function() {
    _webOS.execWithoutCallback("PalmSystem", "paste");
}

PalmSystem.copiedToClipboard = function() {
    _webOS.execWithoutCallback("PalmSystem", "copiedToClipboard");
}

PalmSystem.pastedFromClipboard = function() {
    _webOS.execWithoutCallback("PalmSystem", "pastedFromClipboard");
}

PalmSystem.setWindowOrientation = function(orientation) {
    _webOS.execWithoutCallback("PalmSystem", "setWindowOrientation", [orientation]);
}

PalmSystem.encrypt = function(key, str) {
    return "";
}

PalmSystem.decrypt = function(key, str) {
    return "";
}

PalmSystem.shutdown = function() {
    _webOS.execWithoutCallback("PalmSystem", "shutdown");
}

PalmSystem.markFirstUseDone = function() {
    _webOS.execWithoutCallback("PalmSystem", "markFirstUseDone");
}

PalmSystem.enableFullScreenMode = function(enable) {
    _webOS.execWithoutCallback("PalmSystem", "enableFullScreenMode", [enable]);
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

PalmSystem.setAlertSound = function(soundClass, file) {
    _webOS.execWithoutCallback("PalmSystem", "setAlertSound", [soundClass, file]);
}

PalmSystem.receivePageUpDownInLandscape = function(enable) {
    _webOS.execWithoutCallback("PalmSystem", "receivePageUpDownInLandscape", [enable]);
}

PalmSystem.show = function() {
    _webOS.execWithoutCallback("PalmSystem", "show");
}

PalmSystem.hide = function() {
    _webOS.execWithoutCallback("PalmSystem", "hide");
}

PalmSystem.enableDockMode = function(enable) {
    _webOS.execWithoutCallback("PalmSystem", "enableDockMode", [enable]);
}

PalmSystem.getLocalizedString = function(str) {
    return "";
}

PalmSystem.addNewContentIndicator = function() {
    return "";
}

PalmSystem.removeNewContentIndicator = function(id) {
}

PalmSystem.runAnimationLoop = function(domObj, onStep, onComplete, curve, duration, start, end) {
}

PalmSystem.setActiveBannerWindowWidth = function() {
    _webOS.execWithoutCallback("PalmSystem", "setActiveBannerWindowWidth");
}

PalmSystem.cancelVibrations = function() {
    _webOS.execWithoutCallback("PalmSystem", "cancelVibrations");
}

PalmSystem.setWindowProperties = function(props) {
    _webOS.execWithoutCallback("PalmSystem", "setWindowProperties", [props]);
}

PalmSystem.addActiveCallBanner = function(icon, message, timeStart) {
    return true;
}

PalmSystem.removeActiveCallBanner = function() {
}

PalmSystem.updateActiveCallBanner = function(icon, message, timeStart) {
}

PalmSystem.applyLaunchFeedback = function(offsetX, offsetY) {
    _webOS.execWithoutCallback("PalmSystem", "applyLaunchFeedback");
}

PalmSystem.launcherReady = function() {
    _webOS.execWithoutCallback("PalmSystem", "launcherReady");
}

PalmSystem.getDeviceKeys = function(key) {
    return "";
}

PalmSystem.repaint = function() {
    _webOS.execWithoutCallback("PalmSystem", "repaint");
}

PalmSystem.hideSpellingWidget = function() {
    _webOS.execWithoutCallback("PalmSystem", "hideSpellingWidget");
}

PalmSystem.printFrame = function(frameName, lpsJobid, widthPx, heightPx, printDpi, landscape, reverseOrder) {
}

PalmSystem.editorFocused = function(focused, fieldType, fieldActions) {
    _webOS.execWithoutCallback("PalmSystem", "editorFocused", [focused, fieldType, fieldActions]);
}

PalmSystem.allowResizeOnPositiveSpaceChange = function(allowResize) {
    _webOS.execWithoutCallback("PalmSystem", "allowResizeOnPositiveSpaceChange", [allowResize]);
}

PalmSystem.keepAlive = function(keep) {
    _webOS.execWithoutCallback("PalmSystem", "keepAlive", [keep]);
}

PalmSystem.useSimulatedMouseClicks = function(uses) {
    _webOS.execWithoutCallback("PalmSystem", "useSimulatedMouseClicks", [uses]);
}

PalmSystem.handleTapAndHoldEvent = function(pageX, pageY) {
    _webOS.execWithoutCallback("PalmSystem", "handleTapAndHoldEvent", [pageX, pageY]);
}

PalmSystem.setManualKeyboardEnabled = function(enabled) {
    _webOS.execWithoutCallback("PalmSystem", "setManualKeyboardEnabled", [enabled]);
}

PalmSystem.keyboardShow = function(fieldType) {
    _webOS.execWithoutCallback("PalmSystem", "keyboardShow", [fieldType]);
}

PalmSystem.keyboardHide = function() {
    _webOS.execWithoutCallback("PalmSystem", "keyboardHide");
}

PalmSystem.getResource = function(a, b) {
    return _webOS.execSync("PalmSystem", "getResource", [a, b]);
}

function palmGetResource(a, b) {
    return PalmSystem.getResource(a, b);
}
