/* PalmSystem */

window.PalmSystem = {}
window.PalmSystem.locales = {}

Object.defineProperty(window.PalmSystem, "launchParams", {
  get: function() { return _webOS.execSync("PalmSystem", "getProperty", ["launchParams"]); }
});

Object.defineProperty(window.PalmSystem, "hasAlphaHole", {
  get: function() { return JSON.parse(_webOS.execSync("PalmSystem", "getProperty", ["hasAlphaHole"])); },
  set: function(value) { _webOS.exec(unusedCallback, unusedCallback, "PalmSystem", "setProperty", ["hasAlphaHole", value]); }
});

Object.defineProperty(window.PalmSystem, "locale", {
  get: function() { return _webOS.execSync("PalmSystem", "getProperty", ["locale"]); }
});

Object.defineProperty(window.PalmSystem, "localeRegion", {
  get: function() { return _webOS.execSync("PalmSystem", "getProperty", ["localeRegion"]); }
});

/* enyo-ilib requires PalmSystem.locales.UI on webOS */
Object.defineProperty(window.PalmSystem.locales, "UI", {
  get: function() { return _webOS.execSync("PalmSystem", "getProperty", ["locales.UI"]); }
});

Object.defineProperty(window.PalmSystem, "timeFormat", {
  get: function() { return _webOS.execSync("PalmSystem", "getProperty", ["timeFormat"]); }
});

Object.defineProperty(window.PalmSystem, "timeZone", {
  get: function() { return _webOS.execSync("PalmSystem", "getProperty", ["timeZone"]); }
});

/* enyo-ilib requires PalmSystem.timezone on webOS */
Object.defineProperty(window.PalmSystem, "timezone", {
  get: function() { return _webOS.execSync("PalmSystem", "getProperty", ["timezone"]); }
});

Object.defineProperty(window.PalmSystem, "isMinimal", {
  get: function() { return JSON.parse(_webOS.execSync("PalmSystem", "getProperty", ["isMinimal"])); }
});

Object.defineProperty(window.PalmSystem, "identifier", {
  get: function() { return _webOS.execSync("PalmSystem", "getProperty", ["identifier"]); }
});

Object.defineProperty(window.PalmSystem, "version", {
  get: function() { return _webOS.execSync("PalmSystem", "getProperty", ["version"]); }
});

Object.defineProperty(window.PalmSystem, "screenOrientation", {
  get: function() { return _webOS.execSync("PalmSystem", "getProperty", ["screenOrientation"]); }
});

Object.defineProperty(window.PalmSystem, "windowOrientation", {
  get: function() { return _webOS.execSync("PalmSystem", "getProperty", ["windowOrientation"]); },
  set: function(value) { _webOS.exec(unusedCallback, unusedCallback, "PalmSystem", "setProperty", ["windowOrientation", value]); }
});

Object.defineProperty(window.PalmSystem, "specifiedWindowOrientation", {
  get: function() { return _webOS.execSync("PalmSystem", "getProperty", ["specifiedWindowOrientation"]); }
});

Object.defineProperty(window.PalmSystem, "videoOrientation", {
  get: function() { return _webOS.execSync("PalmSystem", "getProperty", ["videoOrientation"]); }
});

Object.defineProperty(window.PalmSystem, "deviceInfo", {
  get: function() { return _webOS.execSync("PalmSystem", "getProperty", ["deviceInfo"]); }
});

Object.defineProperty(window.PalmSystem, "isActivated", {
  get: function() { return JSON.parse(_webOS.execSync("PalmSystem", "getProperty", ["isActivated"])); }
});

Object.defineProperty(window.PalmSystem, "activityId", {
  get: function() { return parseInt(_webOS.execSync("PalmSystem", "getProperty", ["activityId"])); }
});

Object.defineProperty(window.PalmSystem, "phoneRegion", {
  get: function() { return _webOS.execSync("PalmSystem", "getProperty", ["phoneRegion"]); }
});

PalmSystem.getIdentifier = function() {
    return __PalmSystem.identifier;
}

PalmSystem.getIdentifierForFrame = function(id, url) {
    return _webOS.execSync("PalmSystem", "getIdentifierForFrame", [id, url]);
}

PalmSystem.addBannerMessage = function(msg, params, icon, soundClass, soundFile, duration, doNotSuppress) {
   return  _webOS.execSync("PalmSystem", "addBannerMessage",
        [msg, params, icon, soundClass, soundFile, duration, doNotSuppress]);
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
    var result = _webOS.execSync("PalmSystem", "getResource", [a, b]);

    if (result.length == 0)
      return "";

    if (b === "const json")
        return JSON.parse(result);

    return result;
}

function palmGetResource(a, b) {
    return PalmSystem.getResource(a, b);
}
