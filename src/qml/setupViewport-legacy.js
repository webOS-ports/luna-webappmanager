/*
 * Some LuneOS apps don't specify any viewport width, which cause the rendering
 * to be incorrect.
 * Therefore, force the viewport to contain a "width=device-width" statement.
 * Also set the user-scalable to 'no' if it's not there specified yet.
 */

var viewport = document.querySelector("meta[name=viewport]");
if(viewport) {
    viewport.setAttribute('content', "width=device-width/__LEGACY_SCALING__, initial-scale=__LEGACY_SCALING__, user-scalable=no");
}
else {
    var metaTag=document.createElement('meta');
    metaTag.name = "viewport";
    metaTag.content = "width=device-width/__LEGACY_SCALING__, initial-scale=__LEGACY_SCALING__, user-scalable=no"
    document.getElementsByTagName('head')[0].appendChild(metaTag);
}
