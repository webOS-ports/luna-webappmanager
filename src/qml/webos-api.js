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
    extensions: {},
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
 * Execute a call to a extension function
 * @return bool true on success, false on error (e.g. function doesn't exist)
 */
_webOS.exec = function(successCallback, errorCallback, extensionName, functionName, parameters) {
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

    navigator.qt.postMessage(JSON.stringify({messageType: "callExtensionFunction", extension: extensionName, func: functionName, params: parameters}))
    return true;
}

/**
 * Execute a call to a extension function
 * @return bool true on success, false on error (e.g. function doesn't exist)
 */
_webOS.execWithoutCallback = function(extensionName, functionName, parameters) {
    // if no parameters are supplied create an empty array
    if (typeof parameters === 'undefined')
        parameters = [];

    navigator.qt.postMessage(JSON.stringify({messageType: "callExtensionFunction", extension: extensionName, func: functionName, params: parameters}))
    return true;
}

/**
 * Execute a synchronous call to a extension function
 * @return string response data
 */
_webOS.execSync = function(extensionName, functionName, parameters) {
    if (typeof parameters === 'undefined')
      parameters = [];

    return navigator.qt.postSyncMessage(JSON.stringify({messageType: "callSyncExtensionFunction", extension: extensionName, func: functionName, params: parameters}));
}

var unusedCallback = function() { }
