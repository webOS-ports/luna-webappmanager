/*
 * Licensed to the Apache Software Foundation (ASF) under one
 * or more contributor license agreements.  See the NOTICE file
 * distributed with this work for additional information
 * regarding copyright ownership.  The ASF licenses this file
 * to you under the Apache License, Version 2.0 (the
 * "License"); you may not use this file except in compliance
 * with the License.  You may obtain a copy of the License at
 *
 *   http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing,
 * software distributed under the License is distributed on an
 * "AS IS" BASIS, WITHOUT WARRANTIES OR CONDITIONS OF ANY
 * KIND, either express or implied.  See the License for the
 * specific language governing permissions and limitations
 * under the License.
 */

var extensionObjects = {}


function addExtension(extensionName, pluginObject) {
    extensionObjects[extensionName] = pluginObject
}

function messageHandler(message) {
    var received = JSON.parse(message.data);
    if (typeof received === 'undefined' || typeof received.messageType === 'undefined')
        return false;
    if (received.messageType === "callExtensionFunction") {
        if (typeof received.extension === 'undefined' || typeof received.func === 'undefined')
            return false;
        execMethod(received.extension, received.func, received.params);
    }
    return true;
}

function execMethod(extensionName, functionName, params) {
    if (typeof extensionObjects[extensionName][functionName] != "function")
        return false;
    extensionObjects[extensionName][functionName].apply(this, params);
    return true;
}
