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
};

var webOSApiChannel = new QWebChannel(qt.webChannelTransport, function(channel) {
    // all published objects are available in channel.objects under
    // the identifier set in their attached WebChannel.id property
    _webOS.objects = channel.objects;
/*
    // access a property
    alert(foo.hello);

    // connect to a signal
    foo.someSignal.connect(function(message) {
        alert("Got signal: " + message);
    });

    // invoke a method, and receive the return value asynchronously
    foo.someMethod("bar", function(ret) {
        alert("Got return value: " + ret);
    });
*/
});

/**
 * Execute a call to a extension function
 * @return bool true on success, false on error (e.g. function doesn't exist)
 */
_webOS.exec = function(successCallback, errorCallback, extensionName, functionName, parameters) {
    // if no parameters are supplied create an empty array
    if (typeof parameters === 'undefined')
        parameters = [];

    if( _webOS.objects.hasOwnProperty(extensionName) ) {
        var extensionObj = _webOS.objects[extensionName];
        if( extensionObj.hasOwnProperty(functionName) ) {

            var callId = getNextCallId();
            // Create a contextual callback function for this specific call
            var callbackFunction = function(callbackId, keepCallback, success, payload) {
                if( callbackId === callId ) {
                    if( success && typeof(successCallback) === "function" ) {
                        successCallback.apply(this, payload);
                    }
                    else if( !success && typeof(errorCallback) === "function" ) {
                        errorCallback.apply(this, payload);
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

    if( _webOS.objects.hasOwnProperty(extensionName) ) {
        var extensionObj = _webOS.objects[extensionName];
        if( extensionObj.hasOwnProperty(functionName) ) {

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
    if (typeof parameters === 'undefined')
      parameters = [];

    // Tofe remark: how to achieve that ??
    return navigator.qt.postSyncMessage(JSON.stringify({messageType: "callSyncExtensionFunction", extension: extensionName, func: functionName, params: parameters}));
}

var unusedCallback = function() { }
