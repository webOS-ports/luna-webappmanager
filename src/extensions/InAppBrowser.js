navigator.InAppBrowser = {};
navigator.InAppBrowser.open = function(url) {
    window.top._webOS.execWithoutCallback("InAppBrowser", "open", [url, window.name]);
}
navigator.InAppBrowser.close = function() {
    window.top._webOS.execWithoutCallback("InAppBrowser", "close");
}

navigator.InAppBrowser.ontitlechanged = null;
navigator.InAppBrowser.ondoneclicked = null;

__InAppBrowser = {};
__InAppBrowser.setTitle = function(title, frameName) {
    var navObj = null;

    if (window.top.name === frameName) {
        navObj = window.top.navigator;
    } else {
        for (var n = 0; n < window.top.frames.length; n++) {
            console.log(window.top.frames[n].name);
            if (window.top.frames[n].name === frameName) {
                navObj = window.top.frames[n].navigator;
                break;
            } else {
                console.log("'" + JSON.stringify(window.top.frames[n].name) + "' !== '" + JSON.stringify(frameName) + "'");
            }
        }
    }

    if (navObj === null) {
        console.error("Did not find the right frame " + frameName);
        return;
    }

    if (typeof navObj.InAppBrowser.ontitlechanged === 'function')
      navObj.InAppBrowser.ontitlechanged(title);
}
__InAppBrowser.userClickedDone = function(frameName) {
    var navObj = null;

    if (window.top.name === frameName) {
        navObj = window.top.navigator;
    } else {
        for (var n = 0; n < window.top.frames.length; n++) {
            console.log(window.top.frames[n].name);
            if (window.top.frames[n].name === frameName) {
                navObj = window.top.frames[n].navigator;
                break;
            }
        }
    }

    if (navObj === null) {
        console.error("Did not find the right frame " + frameName);
        return;
    }

    if (typeof navObj.InAppBrowser.ondoneclicked === 'function')
      navObj.InAppBrowser.ondoneclicked();
}
