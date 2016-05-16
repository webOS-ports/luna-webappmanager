/*
 * Some LuneOS apps don't specify any viewport width, which cause the rendering
 * to be incorrect.
 * Therefore, force the viewport to contain a "width=device-width" statement.
 * Also set the user-scalable to 'no' if it's not there specified yet.
 */

var viewport = document.querySelector("meta[name=viewport]");
if(viewport) {
    var viewportContent = viewport.getAttribute('content');
    if(viewportContent.indexOf('device-width')<0) {
        viewportContent += ', width=device-width';
    }
    if(viewportContent.indexOf('user-scalable')<0) {
        viewportContent += ', user-scalable=no';
    }
    if(viewportContent.indexOf('initial-scale')<0) {
        viewportContent += ', initial-scale=1.0';
    }
    viewport.setAttribute('content', viewportContent);
}
else {
    var metaTag=document.createElement('meta');
    metaTag.name = "viewport";
    metaTag.content = "width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=no"
    document.getElementsByTagName('head')[0].appendChild(metaTag);
}
