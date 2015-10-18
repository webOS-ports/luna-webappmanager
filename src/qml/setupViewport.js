/*
 * Some LuneOS apps don't specify any viewport width, which cause the rendering
 * to be incorrect.
 * Therefore, force the viewport to contain a "width=device-width" statement.
 */

var viewport = document.querySelector("meta[name=viewport]");
if(viewport) {
    var viewportContent = viewport.getAttribute('content');
    if(viewportContent.indexOf('device-width')<0) {
        viewport.setAttribute('content', viewportContent+', width=device-width');
    }
}
else {
    var metaTag=document.createElement('meta');
    metaTag.name = "viewport";
    metaTag.content = "width=device-width, initial-scale=1.0, maximum-scale=1.0, user-scalable=0"
    document.getElementsByTagName('head')[0].appendChild(metaTag);
}
