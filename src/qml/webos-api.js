/*
 *  Copyright 2011 Wolfgang Koller - http://www.gofg.at/
 *            2013 Simon Busch <morphis@gravedo.de>
 *            2015 Christophe Chapuis <chris.chapuis@gmail.com>
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

var QWebChannelMessageTypes = {
    signal: 1,
    propertyUpdate: 2,
    init: 3,
    idle: 4,
    debug: 5,
    invokeMethod: 6,
    connectToSignal: 7,
    disconnectFromSignal: 8,
    setProperty: 9,
    response: 10,
};

var QWebChannel = function(transport, initCallback)
{
    if (typeof transport !== "object" || typeof transport.send !== "function") {
        console.error("The QWebChannel expects a transport object with a send function and onmessage callback property." +
                      " Given is: transport: " + typeof(transport) + ", transport.send: " + typeof(transport.send));
        return;
    }

    var channel = this;
    this.transport = transport;

    this.send = function(data)
    {
        if (typeof(data) !== "string") {
            data = JSON.stringify(data);
        }
        channel.transport.send(data);
    }

    this.sendSync = function(data)
    {
        if (typeof(data) !== "string") {
            data = JSON.stringify(data);
        }
        return channel.transport.sendSync(data);
    }

    this.transport.onmessage = function(message)
    {
        var data = message.data;
        if (typeof data === "string") {
            data = JSON.parse(data);
        }
        switch (data.type) {
            case QWebChannelMessageTypes.signal:
                channel.handleSignal(data);
                break;
            case QWebChannelMessageTypes.response:
                channel.handleResponse(data);
                break;
            case QWebChannelMessageTypes.propertyUpdate:
                channel.handlePropertyUpdate(data);
                break;
            default:
                console.error("invalid message received:", message.data);
                break;
        }
    }

    this.execCallbacks = {};
    this.execId = 0;
    this.exec = function(data, callback)
    {
        if (!callback) {
            // if no callback is given, send directly
            channel.send(data);
            return;
        }
        if (channel.execId === Number.MAX_VALUE) {
            // wrap
            channel.execId = 0;
        }
        if (data.hasOwnProperty("id")) {
            console.error("Cannot exec message with property id: " + JSON.stringify(data));
            return;
        }
        data.id = channel.execId++;
        channel.execCallbacks[data.id] = callback;
        channel.send(data);
    };

    this.execSync = function(data)
    {
        data.id = -1; // unused
        var response = channel.sendSync(data);
        var dataResponse = response.data;
        if (typeof dataResponse === "string") {
            dataResponse = JSON.parse(dataResponse);
        }
        return dataResponse;
    };

    this.objects = {};

    this.handleSignal = function(message)
    {
        var object = channel.objects[message.object];
        if (object) {
            object.signalEmitted(message.signal, message.args);
        } else {
            console.warn("Unhandled signal: " + message.object + "::" + message.signal);
        }
    }

    this.handleResponse = function(message)
    {
        if (!message.hasOwnProperty("id")) {
            console.error("Invalid response message received: ", JSON.stringify(message));
            return;
        }
        if(typeof channel.execCallbacks[message.id] === 'undefined') {
            console.log("INVALID ID for message: " + JSON.stringify(message));
        }
        else {
            channel.execCallbacks[message.id](message.data);
            delete channel.execCallbacks[message.id];
        }
    }

    this.handlePropertyUpdate = function(message)
    {
        message.data.map(function(data) {
            var object = channel.objects[data.object];
            if (object) {
                object.propertyUpdate(data.signals, data.properties);
            } else {
                console.warn("Unhandled property update: " + data.object + "::" + data.signal);
            }
        });
        channel.exec({type: QWebChannelMessageTypes.idle});
    }

    this.debug = function(message)
    {
        channel.send({type: QWebChannelMessageTypes.debug, data: message});
    };

    this.finalizeInit = function(data) {
        var objectName;
        for (objectName in data) {
            var object = new QObject(objectName, data[objectName], channel);
        }
        // now unwrap properties, which might reference other registered objects
        for (objectName in channel.objects) {
            channel.objects[objectName].unwrapProperties();
        }
        if (initCallback) {
            initCallback(channel);
        }
        channel.exec({type: QWebChannelMessageTypes.idle});
    }

    var dataInit = channel.execSync({type: QWebChannelMessageTypes.init});
    channel.finalizeInit(dataInit.data);
};

function QObject(name, data, webChannel)
{
    this.__id__ = name;
    webChannel.objects[name] = this;

    // List of callbacks that get invoked upon signal emission
    this.__objectSignals__ = {};

    // Cache of all properties, updated when a notify signal is emitted
    this.__propertyCache__ = {};

    var object = this;

    // ----------------------------------------------------------------------

    this.unwrapQObject = function(response)
    {
        if (response instanceof Array) {
            // support list of objects
            var ret = new Array(response.length);
            for (var i = 0; i < response.length; ++i) {
                ret[i] = object.unwrapQObject(response[i]);
            }
            return ret;
        }
        if (!response
            || !response["__QObject*__"]
            || response["id"] === undefined) {
            return response;
        }

        var objectId = response.id;
        if (webChannel.objects[objectId])
            return webChannel.objects[objectId];

        if (!response.data) {
            console.error("Cannot unwrap unknown QObject " + objectId + " without data.");
            return;
        }

        var qObject = new QObject( objectId, response.data, webChannel );
        qObject.destroyed.connect(function() {
            if (webChannel.objects[objectId] === qObject) {
                delete webChannel.objects[objectId];
                // reset the now deleted QObject to an empty {} object
                // just assigning {} though would not have the desired effect, but the
                // below also ensures all external references will see the empty map
                // NOTE: this detour is necessary to workaround QTBUG-40021
                var propertyNames = [];
                for (var propertyName in qObject) {
                    propertyNames.push(propertyName);
                }
                for (var idx in propertyNames) {
                    delete qObject[propertyNames[idx]];
                }
            }
        });
        // here we are already initialized, and thus must directly unwrap the properties
        qObject.unwrapProperties();
        return qObject;
    }

    this.unwrapProperties = function()
    {
        for (var propertyIdx in object.__propertyCache__) {
            object.__propertyCache__[propertyIdx] = object.unwrapQObject(object.__propertyCache__[propertyIdx]);
        }
    }

    function addSignal(signalData, isPropertyNotifySignal)
    {
        var signalName = signalData[0];
        var signalIndex = signalData[1];
        object[signalName] = {
            connect: function(callback) {
                if (typeof(callback) !== "function") {
                    console.error("Bad callback given to connect to signal " + signalName);
                    return;
                }

                object.__objectSignals__[signalIndex] = object.__objectSignals__[signalIndex] || [];
                object.__objectSignals__[signalIndex].push(callback);

                if (!isPropertyNotifySignal && signalName !== "destroyed") {
                    // only required for "pure" signals, handled separately for properties in propertyUpdate
                    // also note that we always get notified about the destroyed signal
                    webChannel.exec({
                        type: QWebChannelMessageTypes.connectToSignal,
                        object: object.__id__,
                        signal: signalIndex
                    });
                }
            },
            disconnect: function(callback) {
                if (typeof(callback) !== "function") {
                    console.error("Bad callback given to disconnect from signal " + signalName);
                    return;
                }
                object.__objectSignals__[signalIndex] = object.__objectSignals__[signalIndex] || [];
                var idx = object.__objectSignals__[signalIndex].indexOf(callback);
                if (idx === -1) {
                    console.error("Cannot find connection of signal " + signalName + " to " + callback.name);
                    return;
                }
                object.__objectSignals__[signalIndex].splice(idx, 1);
                if (!isPropertyNotifySignal && object.__objectSignals__[signalIndex].length === 0) {
                    // only required for "pure" signals, handled separately for properties in propertyUpdate
                    webChannel.exec({
                        type: QWebChannelMessageTypes.disconnectFromSignal,
                        object: object.__id__,
                        signal: signalIndex
                    });
                }
            }
        };
    }

    /**
     * Invokes all callbacks for the given signalname. Also works for property notify callbacks.
     */
    function invokeSignalCallbacks(signalName, signalArgs)
    {
        var connections = object.__objectSignals__[signalName];
        if (connections) {
            connections.forEach(function(callback) {
                callback.apply(callback, signalArgs);
            });
        }
    }

    this.propertyUpdate = function(signals, propertyMap)
    {
        // update property cache
        for (var propertyIndex in propertyMap) {
            var propertyValue = propertyMap[propertyIndex];
            object.__propertyCache__[propertyIndex] = propertyValue;
        }

        for (var signalName in signals) {
            // Invoke all callbacks, as signalEmitted() does not. This ensures the
            // property cache is updated before the callbacks are invoked.
            invokeSignalCallbacks(signalName, signals[signalName]);
        }
    }

    this.signalEmitted = function(signalName, signalArgs)
    {
        invokeSignalCallbacks(signalName, signalArgs);
    }

    function addMethod(methodData)
    {
        var methodName = methodData[0];
        var methodIdx = methodData[1];
        object[methodName] = function() {
            var args = [];
            var callback;
            for (var i = 0; i < arguments.length; ++i) {
                if (typeof arguments[i] === "function")
                    callback = arguments[i];
                else
                    args.push(arguments[i]);
            }

            webChannel.exec({
                "type": QWebChannelMessageTypes.invokeMethod,
                "object": object.__id__,
                "method": methodIdx,
                "args": args
            }, function(response) {
                if (response !== undefined) {
                    var result = object.unwrapQObject(response);
                    if (callback) {
                        (callback)(result);
                    }
                }
            });
        };

        // Declare also the fonction with "_Sync" suffix, dedicated to synchronous calls
        var syncMethodName = methodName + "_Sync";
        object[syncMethodName] = function() {
            var args = [];
            for (var i = 0; i < arguments.length; ++i) {
                args.push(arguments[i]);
            }
            var response = webChannel.execSync({
                "type": QWebChannelMessageTypes.invokeMethod,
                "object": object.__id__,
                "method": methodIdx,
                "args": args
            });
            return object.unwrapQObject(response.data);
        };
    }

    function bindGetterSetter(propertyInfo)
    {
        var propertyIndex = propertyInfo[0];
        var propertyName = propertyInfo[1];
        var notifySignalData = propertyInfo[2];
        // initialize property cache with current value
        // NOTE: if this is an object, it is not directly unwrapped as it might
        // reference other QObject that we do not know yet
        object.__propertyCache__[propertyIndex] = propertyInfo[3];

        if (notifySignalData) {
            if (notifySignalData[0] === 1) {
                // signal name is optimized away, reconstruct the actual name
                notifySignalData[0] = propertyName + "Changed";
            }
            addSignal(notifySignalData, true);
        }

        Object.defineProperty(object, propertyName, {
            get: function () {
                var propertyValue = object.__propertyCache__[propertyIndex];
                if (propertyValue === undefined) {
                    // This shouldn't happen
                    console.warn("Undefined value in property cache for property \"" + propertyName + "\" in object " + object.__id__);
                }

                return propertyValue;
            },
            set: function(value) {
                if (value === undefined) {
                    console.warn("Property setter for " + propertyName + " called with undefined value!");
                    return;
                }
                object.__propertyCache__[propertyIndex] = value;
                webChannel.exec({
                    "type": QWebChannelMessageTypes.setProperty,
                    "object": object.__id__,
                    "property": propertyIndex,
                    "value": value
                });
            }
        });

    }

    // ----------------------------------------------------------------------

    data.methods.forEach(addMethod);

    data.properties.forEach(bindGetterSetter);

    data.signals.forEach(function(signal) { addSignal(signal, false); });

    for (var nameObj in data.enums) {
        object[nameObj] = data.enums[nameObj];
    }
}

//required for use with nodejs
if (typeof module === 'object') {
    module.exports = {
        QWebChannel: QWebChannel
    };
}

var _webOS = {
};

var webOSApiChannel = new QWebChannel(qt.webChannelTransport, function(channel) {
    // all published objects are available in channel.objects under
    // the identifier set in their attached WebChannel.id property
    _webOS.objects = channel.objects;

    // Handle relaunch requests here
    if( _webOS.objects.hasOwnProperty("PalmSystem") ) {
        _webOS.objects.PalmSystem.launchParamsChanged.connect(function(needRelaunch) {
            if( needRelaunch ) {
                console.log("relaunchRequested with params '" + _webOS.objects.PalmSystem.launchParams + "'");
                Mojo.relaunch();
            }
        });
    }
});

var _callId = 0;
function getNextCallId() { _callId++; return _callId; }

/**
 * Execute a call to a extension function
 * @return bool true on success, false on error (e.g. function doesn't exist)
 */
_webOS.exec = function(successCallback, errorCallback, extensionName, functionName, parameters) {
    // if no parameters are supplied create an empty array
    if (typeof parameters === 'undefined')
        parameters = [];

    console.log("_webOS.exec("+extensionName+","+functionName+","+JSON.stringify(parameters)+")");

    if( _webOS.objects.hasOwnProperty(extensionName) ) {
        var extensionObj = _webOS.objects[extensionName];
        if( extensionObj.hasOwnProperty(functionName) ) {

            var callId = getNextCallId();
            // Create a contextual callback function for this specific call
            var callbackFunction = function(callbackId, keepCallback, success, payload) {
                if( callbackId === callId ) {
                    if( success && typeof(successCallback) === "function" ) {
                        successCallback.call(this, payload);
                    }
                    else if( !success && typeof(errorCallback) === "function" ) {
                        errorCallback.call(this, payload);
                    }

                    if( !keepCallback ) {
                        // unsubscribe ourself from the signal
                        extensionObj.callback.disconnect(callbackFunction);
                    }
                };
            };
            // Associate the callback signal with our contextual function
            extensionObj.callback.connect(callbackFunction);

            // Give the unique call id to the method we are calling
            parameters.unshift(callId);
            // ... And do the call
            extensionObj[functionName].apply(this, parameters);

            return true;
        }
    }

    return false;
}

/**
 * Execute a call to a extension function
 * @return bool true on success, false on error (e.g. function doesn't exist)
 */
_webOS.execWithoutCallback = function(extensionName, functionName, parameters) {
    // if no parameters are supplied create an empty array
    if (typeof parameters === 'undefined')
        parameters = [];

    console.log("_webOS.execWithoutCallback("+extensionName+","+functionName+","+JSON.stringify(parameters)+")");

    if( _webOS.objects.hasOwnProperty(extensionName) ) {
        var extensionObj = _webOS.objects[extensionName];
        if( extensionObj.hasOwnProperty(functionName) ) {

            parameters.push(function (ret) {console.log(functionName + " returned " + ret)});
            extensionObj[functionName].apply(this, parameters);
            return true;
        }
    }

    return false;
}

/**
 * Get the property of en extension
 * @return property value
 */
_webOS.getProperty = function(extensionName, propertyName) {
    if( _webOS.objects.hasOwnProperty(extensionName) ) {
        var extensionObj = _webOS.objects[extensionName];
        console.log("_webOS.getProperty ("+extensionName+"."+propertyName+")="+extensionObj[propertyName]);
        return extensionObj[propertyName];
    }

    return null;
}

/**
 * Set the property of en extension
 */
_webOS.setProperty = function(extensionName, propertyName, value) {
    if( _webOS.objects.hasOwnProperty(extensionName) ) {
        var extensionObj = _webOS.objects[extensionName];
        extensionObj[propertyName] = value;
    }
}

/**
 * Execute a synchronous call to a extension function
 * @return string response data
 */
_webOS.execSync = function(extensionName, functionName, parameters) {
    // if no parameters are supplied create an empty array
    if (typeof parameters === 'undefined')
        parameters = [];

    console.log("_webOS.execSync("+extensionName+","+functionName+","+JSON.stringify(parameters)+")");

    var syncFunctionName = functionName + "_Sync";

    if( _webOS.objects.hasOwnProperty(extensionName) ) {
        var extensionObj = _webOS.objects[extensionName];
        if( extensionObj.hasOwnProperty(syncFunctionName) ) {
            var retValue = extensionObj[syncFunctionName].apply(this, parameters);
            return retValue;
        }
    }

    return "";
}

var unusedCallback = function() { }
