// IE9
if(typeof console === "undefined") { var console = { log: function (logMsg) { } }; }

var camera, controls, scene, renderer, material;
var spheregeometry;
var vertices=[];
var edges=[];
var pointLight;

// Run on startup
$(document).ready(main);

// ************ STARTUP *****************
function main() {
    init();
}

function de2ra(degree)  { return degree*(Math.PI/180); }

function add_vertex(x,y,z, material) {

    var sphere = new THREE.Mesh( spheregeometry, material);
    sphere.position.x=x;
    sphere.position.y=y;
    sphere.position.z=z;
    scene.add(sphere);
    vertices.push(sphere);
}

function add_edge(x1,y1,z1, x2,y2,z2, material) {
    var length = Math.pow((x2-x1), 2) + Math.pow((y2-y1), 2) + Math.pow((z2-z1), 2)
    length = Math.sqrt(length);
   // var radius=.06;
    var radius = .2;
    var geometry = new THREE.CylinderGeometry( radius, radius, length, 3);

    var cylinder = new THREE.Mesh(geometry, material);
    cylinder.position.x=(x1+x2)/2;
    cylinder.position.y=(y1+y2)/2;
    cylinder.position.z=(z1+z2)/2;
    cylinder.rotation.z=-Math.atan2(x2-x1, y2-y1);
    cylinder.rotation.x=Math.atan2(z2-z1, y2-y1);
    
    scene.add(cylinder);
    edges.push(cylinder);
}

function makePins(boxparam, bigcube)
{
	var geometry = new THREE.BoxGeometry(1,1,1);

	for(var i=0; i<2; i++)
	{
		var cube = new THREE.Mesh( geometry, new THREE.MeshBasicMaterial({
		    color: 'red'
		  }) );

		cube.position.x = 0.5/*coordonata nu colt ci centrul*/ 
				+ boxparam[2] + boxes.types[boxparam[1]][0] - (1-i)*2*2/*dist*/- 1/*acuma e cu o unitate prea in dreapta*/;
		cube.position.y = 0.5 + boxparam[3] + boxes.types[boxparam[1]][1] - 1/*acuma e cu o unitate prea in sus*/;
		cube.position.z = 0.5 + boxparam[4] + boxes.types[boxparam[1]][2];

		scene.add(cube);
	}
	
}

function makeBoxes(sched)
{
	
	for (var i=0; i<sched.coords.length; i++)
  {
		var boxparam = sched.coords[i];
		var geometry = new THREE.BoxGeometry(sched.types[boxparam[0]][0], sched.types[boxparam[0]][1], sched.types[boxparam[0]][2]);
		var cube = new THREE.Mesh( geometry, new THREE.MeshBasicMaterial({
        wireframe: true,
        color: 'blue'
      }) );

		scene.add( cube );

		cube.position.x = boxparam[2] + geometry.parameters.width/2;
		cube.position.y = boxparam[3] + geometry.parameters.height/2;
		cube.position.z = boxparam[4] + geometry.parameters.depth/2;

		//makePins(boxparam);
	}
}

function makeGeometry(graphobject) {

		//this is going to explode!


		//make injections
		for (var i=0; i<graphobject.inj.length; i++)
    {
				var n=graphobject.nodes[graph.inj[i] - 1];
        add_vertex(n[1], n[2], n[3], material3);

				//if(isNaN(graphobject.names[i][1]))
				{
				var s1= makeTextSprite2(graphobject.names[i][1]);

				s1.position.x = n[1];
				s1.position.y = n[2];
				s1.position.z = n[3] + 1;
				scene.add(s1);
}
		}

		//make normal vertices - does not work - fuck
/*    for (var i=0; i<graph.nodes.length; i++)
    {
				var skip = false;
				for (var j=0; j<graph.inj.length; j++)
    		{
					if(graph.inj[j] == graph.nodes[i][0])//primul element este numele
					{
						skip = true;
						break;
					}
						
				}
				if(skip) break;

        var n=graph.nodes[i];
        add_vertex(n[1], n[2], n[3], material1);
    }
*/

		//make edges
    for (var i=0; i<graphobject.edges.length; i++)
    {
        var a=graphobject.nodes[graphobject.edges[i][0] - 1];
        var b=graphobject.nodes[graphobject.edges[i][1] - 1];

				//var a=graph.edges[i][0];
        //var b=graph.edges[i][1];

				var materialx = material1;
				otherside = 0;
				for(var j=1; j<4; j++)
					if(a[j]%2 != 0)
						otherside++;

				if(otherside >= 2)
					materialx = material2;

				//if(otherside >= 2)
	        add_edge2(a[1], a[2], a[3], b[1], b[2], b[3], materialx);
    }
}

function init() {
    // Set up the container and renderer
    var $container = document.getElementById('container');
    var w=window,d=document,e=d.documentElement,g=d.getElementsByTagName('container')[0],x=w.innerWidth||e.clientWidth||g.clientWidth,y=w.innerHeight||e.clientHeight||g.clientHeight;
    var WIDTH=x;
    var HEIGHT=y;


    // Renderer
		renderer = Detector.webgl? new THREE.WebGLRenderer({preserveDrawingBuffer: true}): new THREE.CanvasRenderer();
    //renderer = new THREE.WebGLRenderer( { antialias: true });
    renderer.setSize(WIDTH, HEIGHT-20);
    renderer.setClearColor( 0xffffff, 1 );
    $container.appendChild(renderer.domElement);
    scene = new THREE.Scene();

// Center the camera
    var sx=0; var sy=0; var sz=0;
    for (var i=0; i<graph.nodes.length; ++i)
    {
        var n=graph.nodes[i];
        if (n[1]>sx){ sx=n[1]; }
        if (n[2]>sy){ sy=n[2]; }
        if (n[3]>sz){ sz=n[3]; }
    }
    
    // Camera
    camera = new THREE.PerspectiveCamera( 45, WIDTH/HEIGHT, 0.1,10000);
    camera.position.set(sx/2, sy/2, sz/2+sx*1.5);
    camera.up = new THREE.Vector3(-1,0,0);
    
    scene.add(camera);

    // Models
    spheregeometry=new THREE.SphereGeometry(.5, 3, 3)

    // Lighting
    var ambientLight = new THREE.AmbientLight(0x552222);
    ambientLight.intensity=.1;
    scene.add(ambientLight);
    //scene.fog = new THREE.FogExp2( 0xffffff, -0.0025 );
    pointLight = new THREE.PointLight(0xFFFFFF);
    pointLight.position.x = 100;
    pointLight.position.y = 0;
    pointLight.position.z = 130;
    scene.add(pointLight);

    // Materials
    //material1 = new THREE.MeshLambertMaterial({color: "red"});
    material1 = new THREE.LineBasicMaterial({ color: 0xFF0000, linewidth: 2, transparent: true, opacity: 1});

    //material2 = new THREE.MeshLambertMaterial({color: "blue"});
    material2 = new THREE.LineBasicMaterial({ color: 0x0000FF, linewidth: 2, transparent: true, opacity: 1 });

    material3 = new THREE.MeshLambertMaterial({color: "green"});

    //material2.opacity=0;
    //material2.transparent=true;

    

    // Add controls
    controls = new THREE.TrackballControls(camera, $container);
    controls.target.set( sx/2, sy/2, sz/2 - sx*1.5);
	controls.rotateSpeed = 2.0;
	controls.zoomSpeed = 1.2;
	controls.panSpeed = 0.8;

	controls.noZoom = false;
	controls.noPan = false;

	controls.staticMoving = true;
	controls.dynamicDampingFactor = 0.3;
		
		
		makeBoxes(boxes0);
		makeBoxes(boxes1);
		makeBoxes(boxes2);
  	makeGeometry(graph2);//connects pins to boxes
		makeGeometry(graph);//the circuit

    // Render
    $(window).load(function(){
        animate();
    });
}

function animate()
{
    renderer.render(scene, camera);
    controls.update();
    requestAnimationFrame(animate);
}

function makeTextSprite2(message)
{
	var canvas = document.createElement('canvas');
	var size = 256; // CHANGED
	canvas.width = size;
	canvas.height = size;
	var context = canvas.getContext('2d');
	context.fillStyle = '#000000'; // CHANGED
	context.textAlign = 'center';
	context.font = '24px Arial';
	context.fillText(message, size / 2, size / 2);

	var amap = new THREE.Texture(canvas);
	amap.needsUpdate = true;

	var mat = new THREE.SpriteMaterial({
 		map: amap,
  	transparent: false,
  	useScreenCoordinates: false,
  	color: 0xffffff // CHANGED
	});

	var sp = new THREE.Sprite(mat);
	sp.scale.set( 10, 10, 1 ); // CHANGED

	return sp;
}

function add_edge2(x1, y1, z1, x2, y2, z2, material) {
    var length = Math.sqrt(Math.pow((x2-x1), 2) + Math.pow((y2-y1), 2) + Math.pow((z2-z1), 2));

    var geometry = new THREE.Geometry();

    geometry.vertices.push(new THREE.Vector3(x1, y1, z1));
    geometry.vertices.push(new THREE.Vector3(x2, y2, z2));

    var line = new THREE.Line(geometry, material);
    line.oldMaterial = material;
    scene.add(line);
    edges.push(line);
}

