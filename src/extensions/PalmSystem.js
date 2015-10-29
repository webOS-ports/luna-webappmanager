/* PalmSystem */

window.PalmSystem = {}
window.PalmSystem.locales = {}

Object.defineProperty(window.PalmSystem, "launchParams", {
  get: function() { return _webOS.getProperty("PalmSystem", "launchParams"); }
});

Object.defineProperty(window.PalmSystem, "hasAlphaHole", {
  get: function() { return _webOS.getProperty("PalmSystem", "hasAlphaHole"); },
  set: function(value) { _webOS.setProperty("PalmSystem", "hasAlphaHole", value); }
});

Object.defineProperty(window.PalmSystem, "locale", {
  get: function() { return _webOS.getProperty("PalmSystem", "locale"); }
});

Object.defineProperty(window.PalmSystem, "localeRegion", {
  get: function() { return _webOS.getProperty("PalmSystem", "localeRegion"); }
});

/* enyo-ilib requires PalmSystem.locales.UI on webOS */
Object.defineProperty(window.PalmSystem.locales, "UI", {
  get: function() { return _webOS.getProperty("PalmSystem", "locale"); }
});

Object.defineProperty(window.PalmSystem, "timeFormat", {
  get: function() { return _webOS.getProperty("PalmSystem", "timeFormat"); }
});

Object.defineProperty(window.PalmSystem, "timeZone", {
  get: function() { return _webOS.getProperty("PalmSystem", "timeZone"); }
});

/* enyo-ilib requires PalmSystem.timezone on webOS */
Object.defineProperty(window.PalmSystem, "timezone", {
  get: function() { return _webOS.getProperty("PalmSystem", "timeZone"); }
});

Object.defineProperty(window.PalmSystem, "isMinimal", {
  get: function() { return _webOS.getProperty("PalmSystem", "isMinimal"); }
});

Object.defineProperty(window.PalmSystem, "identifier", {
  get: function() { return _webOS.getProperty("PalmSystem", "identifier"); }
});

Object.defineProperty(window.PalmSystem, "version", {
  get: function() { return _webOS.getProperty("PalmSystem", "version"); }
});

Object.defineProperty(window.PalmSystem, "screenOrientation", {
  get: function() { return _webOS.getProperty("PalmSystem", "screenOrientation"); }
});

Object.defineProperty(window.PalmSystem, "windowOrientation", {
  get: function() { return _webOS.getProperty("PalmSystem", "windowOrientation"); },
  set: function(value) { _webOS.setProperty("PalmSystem", "windowOrientation", value); }
});

Object.defineProperty(window.PalmSystem, "specifiedWindowOrientation", {
  get: function() { return _webOS.getProperty("PalmSystem", "specifiedWindowOrientation"); }
});

Object.defineProperty(window.PalmSystem, "videoOrientation", {
  get: function() { return _webOS.getProperty("PalmSystem", "videoOrientation"); }
});

Object.defineProperty(window.PalmSystem, "deviceInfo", {
  get: function() { return _webOS.getProperty("PalmSystem", "deviceInfo"); }
});

Object.defineProperty(window.PalmSystem, "isActivated", {
  get: function() { return _webOS.getProperty("PalmSystem", "isActivated"); }
});

Object.defineProperty(window.PalmSystem, "activityId", {
  get: function() { return _webOS.getProperty("PalmSystem", "activityId"); }
});

Object.defineProperty(window.PalmSystem, "phoneRegion", {
  get: function() { return _webOS.getProperty("PalmSystem", "phoneRegion"); }
});

PalmSystem.getIdentifier = function() {
    return __PalmSystem.identifier;
}

PalmSystem.getIdentifierForFrame = function(id, url) {
    var _id = id || "";
    var _url = url || "";
    return _webOS.execSync("PalmSystem", "getIdentifierForFrame", [_id, _url]);
}

PalmSystem.addBannerMessage = function(msg, params, icon, soundClass, soundFile, duration, doNotSuppress) {
    // avoid sending null values to the webOS call
    var _msg = msg || "";
    var _params = params || "";
    var _icon = icon || "";
    var _soundClass = soundClass || "";
    var _soundFile = soundFile || "";
    var _duration = duration || 0;
    var _doNotSuppress = doNotSuppress || false;
   return  _webOS.execSync("PalmSystem", "addBannerMessage",
        [_msg, _params, _icon, _soundClass, _soundFile, _duration, _doNotSuppress]);
}

PalmSystem.removeBannerMessage = function(id) {
    var _id = id || "";
    _webOS.execWithoutCallback("PalmSystem", "removeBannerMessage", [_id]);
}

PalmSystem.clearBannerMessages = function() {
    _webOS.execWithoutCallback("PalmSystem", "clearBannerMessages");
}

PalmSystem.playSoundNotification = function(soundClass, file, duration, wakeUpScreen) {
    var _soundClass = soundClass || "";
    var _file = file || "";
    var _duration = duration || 0;
    var _wakeUpScreen = wakeUpScreen || false;
    _webOS.execWithoutCallback("PalmSystem", "playSoundNotification", [_soundClass, _file, _duration, _wakeUpScreen]);
}

PalmSystem.simulateMouseClick = function(pageX, pageY, pressed) {
    var _pageX = pageX || 0.;
    var _pageY = pageY || 0.;
    var _pressed = pressed || false;
    _webOS.execWithoutCallback("PalmSystem", "simulateMouseClick", [_pageX, _pageY, _pressed]);
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
    var _orientation = orientation || 0;
    _webOS.execWithoutCallback("PalmSystem", "setWindowOrientation", [_orientation]);
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
    var _enable = enable || false;
    _webOS.execWithoutCallback("PalmSystem", "enableFullScreenMode", [_enable]);
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
    var _soundClass = soundClass || "";
    var _file = file || "";
    _webOS.execWithoutCallback("PalmSystem", "setAlertSound", [_soundClass, _file]);
}

PalmSystem.receivePageUpDownInLandscape = function(enable) {
    var _enable = enable || false;
    _webOS.execWithoutCallback("PalmSystem", "receivePageUpDownInLandscape", [_enable]);
}

PalmSystem.show = function() {
    _webOS.execWithoutCallback("PalmSystem", "show");
}

PalmSystem.hide = function() {
    _webOS.execWithoutCallback("PalmSystem", "hide");
}

PalmSystem.enableDockMode = function(enable) {
    var _enable = enable || false;
    _webOS.execWithoutCallback("PalmSystem", "enableDockMode", [_enable]);
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
    var _props = props || "";
    _webOS.execWithoutCallback("PalmSystem", "setWindowProperties", [_props]);
}

PalmSystem.addActiveCallBanner = function(icon, message, timeStart) {
    return true;
}

PalmSystem.removeActiveCallBanner = function() {
}

PalmSystem.updateActiveCallBanner = function(icon, message, timeStart) {
}

PalmSystem.applyLaunchFeedback = function(offsetX, offsetY) {
    var _offsetX = offsetX || 0;
    var _offsetY = offsetY || 0;
    _webOS.execWithoutCallback("PalmSystem", "applyLaunchFeedback", [_offsetX, _offsetY]);
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
    var _focused = focused || false;
    var _fieldType = fieldType || 0;
    var _fieldActions = fieldActions || 0;
    _webOS.execWithoutCallback("PalmSystem", "editorFocused", [_focused, _fieldType, _fieldActions]);
}

PalmSystem.allowResizeOnPositiveSpaceChange = function(allowResize) {
    var _allowResize = allowResize || false;
    _webOS.execWithoutCallback("PalmSystem", "allowResizeOnPositiveSpaceChange", [_allowResize]);
}

PalmSystem.keepAlive = function(keep) {
    var _keep = keep || false;
    _webOS.execWithoutCallback("PalmSystem", "keepAlive", [_keep]);
}

PalmSystem.useSimulatedMouseClicks = function(uses) {
    var _uses = uses || false;
    _webOS.execWithoutCallback("PalmSystem", "useSimulatedMouseClicks", [_uses]);
}

PalmSystem.handleTapAndHoldEvent = function(pageX, pageY) {
    var _pageX = pageX || 0.;
    var _pageY = pageY || 0.;
    _webOS.execWithoutCallback("PalmSystem", "handleTapAndHoldEvent", [_pageX, _pageY]);
}

PalmSystem.setManualKeyboardEnabled = function(enabled) {
    var _enabled = enabled || false;
    _webOS.execWithoutCallback("PalmSystem", "setManualKeyboardEnabled", [_enabled]);
}

PalmSystem.keyboardShow = function(fieldType) {
    var _fieldType = fieldType || false;
    _webOS.execWithoutCallback("PalmSystem", "keyboardShow", [_fieldType]);
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
