/* This THREE.js demo was adapted from:
 *
 *   http://solutiondesign.com/blog/-/blogs/webgl-and-three-js-texture-mappi-1/
 *
 * The idea for rendering to SVG came from:
 *
 *   http://blog.felixbreuer.net/2014/08/05/using-threejs-to-create-vector-graphics-from-3d-visualizations.html
 */
var camera;
var scene;
var renderer;
var mesh;
  
 
/* The following discussion on StackOverflow shows discusses how to remove all
 * elements from a DOM
 *
 *  http://stackoverflow.com/questions/3955229/remove-all-child-elements-of-a-dom-node-in-javascript
 */
		
function removeChildrenFromNode(node) {
	var fc = node.firstChild;

	while( fc ) {
		node.removeChild( fc );
		fc = node.firstChild;
	}
}

function svgSnapshot() {
	var svgContainer = document.getElementById("svg");
	removeChildrenFromNode(svgContainer);
	var width  = svgContainer.getBoundingClientRect().width;
	var height = svgContainer.getBoundingClientRect().height;
	
	svgRenderer = new THREE.SVGRenderer();
	svgRenderer.setClearColor( 0xffffff );
	svgRenderer.setSize(width,height );
	svgRenderer.setQuality( 'high' );
	svgContainer.appendChild( svgRenderer.domElement );
	svgRenderer.render( scene, camera );
	
	/* The following discussion shows how to scale an SVG to fit its contained
	 *
	 *  http://stackoverflow.com/questions/4737243/fit-svg-to-the-size-of-object-container
	 *
	 * Another useful primer is here
	 *  https://sarasoueidan.com/blog/svg-coordinate-systems/
	 */
	svgRenderer.domElement.removeAttribute("width");
	svgRenderer.domElement.removeAttribute("height");
	
	document.getElementById("source").value = svgContainer.innerHTML.replace(/<path/g,"\n<path");
}
