navigator.InAppBrowser = {};
navigator.InAppBrowser.open = function(url) {
    _webOS.execWithoutCallback("InAppBrowser", "open", [url]);
}
navigator.InAppBrowser.close = function() {
    _webOS.execWithoutCallback("InAppBrowser", "close");
}

navigator.InAppBrowser.ontitlechanged = null;
navigator.InAppBrowser.ondoneclicked = null;

__InAppBrowser = {};
__InAppBrowser.setTitle = function(title) {
    if (typeof navigator.InAppBrowser.ontitlechanged === 'function')
      navigator.InAppBrowser.ontitlechanged(title);
}
__InAppBrowser.userClickedDone = function() {
    if (typeof navigator.InAppBrowser.ondoneclicked === 'function')
      navigator.InAppBrowser.ondoneclicked();
}
