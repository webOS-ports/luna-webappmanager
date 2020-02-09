/*
 * Some LuneOS apps don't specify any viewport width, which cause the rendering
 * to be incorrect.
 * Therefore, force the viewport to contain a "width=device-width" statement.
 * Also set the user-scalable to 'no' if it's not there specified yet.
 */

var viewport = document.querySelector("meta[name=viewport]");
if(viewport) {
    var viewportContent = viewport.getAttribute('content');
    viewportContent = "initial-scale=__SCALING__, user-scalable=no"
    viewport.setAttribute('content', viewportContent);
}
else {
    var metaTag=document.createElement('meta');
    metaTag.name = "viewport";
    metaTag.content = "initial-scale=__SCALING__, user-scalable=no"
    document.getElementsByTagName('head')[0].appendChild(metaTag);
}
