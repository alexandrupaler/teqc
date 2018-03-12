// IE9
if(typeof console === "undefined") { var console = { log: function (logMsg) { } }; }

var camera, controls, scene, renderer, material;
var spheregeometry;
var vertices=[];

/*objects in the 3d scene*/
var primalDefects = new THREE.Geometry();
var dualDefects = new THREE.Geometry();
var notWorkingDefect = new THREE.Geometry();
var debugDefects1 = new THREE.Geometry();
var debugDefects2 = new THREE.Geometry();

/*Materials*/
var materialPrimal = new THREE.LineBasicMaterial({ color: 0xFF0000, linewidth: 2, transparent: true, opacity: 1});
var materialDual = new THREE.LineBasicMaterial({ color: 0x0000FF, linewidth: 2, transparent: true, opacity: 1 });
var materialInjection = new THREE.MeshLambertMaterial({color: "green"});
var materialNotWorking = new THREE.LineBasicMaterial({ color: 0xFFFF00, linewidth: 20, transparent: false, opacity: 1 });
var materialDebug1 = new THREE.LineBasicMaterial({ color: 0x00FF00, linewidth: 5, transparent: false, opacity: 1 });
var materialDebug2 = new THREE.LineBasicMaterial({ color: 0xf442eb, linewidth: 5, transparent: false, opacity: 1 });

/*bounding box*/
var bBox = [Number.MAX_SAFE_INTEGER, Number.MAX_SAFE_INTEGER, Number.MAX_SAFE_INTEGER,
             Number.MIN_SAFE_INTEGER, Number.MIN_SAFE_INTEGER, Number.MIN_SAFE_INTEGER];//dimension 6: 3 mins, 3 maxes

/*statistics*/
var stats = new Stats();
var objectsToClick = [];

/*standby render*/
var renderLastMove = Date.now();
var renderRunning = false;
var renderStandbyAfter = 2000; // ms

/*visualisation parameters*/
var drawBoxes = true;
var drawGeometry = true;

var primalGeometry = null;
var dualGeometry = null;
var boxesGeometry = null;

// Run on startup
$(document).ready(main);
window.addEventListener("mousedown", function(){/*console.log("start"); */renderRunning = false; startAnimation()}, true);
window.addEventListener("mouseup", function(){/*console.log("stop"); */renderRunning = false}, true);
window.addEventListener("wheel", function(){/*console.log("scroll");*/renderRunning = false; startAnimation()}, true);

window.onunload = function()
{
  scene = null;
}


function readGetParameters()
{
	var GET = {};
	var query = window.location.search.substring(1).split("&");
	for (var i = 0, max = query.length; i < max; i++)
	{
		  if (query[i] === "") // check for trailing & with no param
		      continue;

		  var param = query[i].split("=");
		  if(param.length == 1)
		  {
		  	GET[decodeURIComponent(param[0])] = 1;
		 	}
		 	else
		 	{
		  	GET[decodeURIComponent(param[0])] = decodeURIComponent(param[1] || "");
	  	}
	}
	
	if(GET.noboxes)
	{
		drawBoxes = false;
	}
	if(GET.nogeom)
	{
		drawGeometry = false;
	}
}

// ************ STARTUP *****************
function main()
{
  stats.showPanel( 1 ); // 0: fps, 1: ms, 2: mb, 3+: custom
  document.body.appendChild( stats.dom );

	readGetParameters();

  init();
  animate();
  //startAnimation();
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

function add_edge2(x1, y1, z1, x2, y2, z2, lineSegmentsGeometry)
{
    lineSegmentsGeometry.vertices.push(new THREE.Vector3(x1, y1, z1));
    lineSegmentsGeometry.vertices.push(new THREE.Vector3(x2, y2, z2));
    
    updateBoundingBox(x1, y1, z1);
    updateBoundingBox(x2, y2, z2);
}

function add_edge2Index(index, graphobject, lineSegmentsGeometry)
{
    var a = graphobject.nodes[graphobject.edges[index][0] - 1];
    var b = graphobject.nodes[graphobject.edges[index][1] - 1];
    
    add_edge2(a[1], a[2], a[3], b[1], b[2], b[3], lineSegmentsGeometry);
}

function makePins(boxparam, bigcube)
{
	var geometry = new THREE.BoxGeometry(1, 1, 1);

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

function makeBoundingBox()
{
  var width = bBox[3] - bBox[0];
  var height = bBox[4] - bBox[1];
  var depth = bBox[5] - bBox[2];
  
  console.log(width + " " + height + " " + depth);
  for(var i=0; i<6; i++)
    console.log(bBox[i]);
    
  console.log(Math.ceil(width/6) + " " + Math.ceil(height/6) + " " + Math.ceil(depth/6));
  
  var geometry = new THREE.BoxGeometry(width, height, depth);
		
	geometry.translate(bBox[0] + width/2, 
	                bBox[1] + height/2, 
	                bBox[2] + depth/2);
                    
  var cube = new THREE.Mesh( geometry, 
   new THREE.MeshBasicMaterial({wireframe: true, color: 'black'})
   );
	cube.updateMatrix();
	
	scene.add(cube);
}

function updateBoundingBox(cw, ch, cd)
{
  if(cw < bBox[0])
    bBox[0] = cw;
  else if (cw > bBox[3])
    bBox[3] = cw;
    
  if(ch < bBox[1])
    bBox[1] = ch;
  else if (ch > bBox[4])
    bBox[4] = ch;
    
  if(cd < bBox[2])
    bBox[2] = cd;
  else if (cd > bBox[5])
    bBox[5] = cd;
}

function makeBoxes(sched)
{
  var boxes = new THREE.Geometry();
	
	for (var i=0; i<sched.coords.length; i++)
  {
		var boxparam = sched.coords[i];
		var width = sched.types[boxparam[0]][0];
		var height = sched.types[boxparam[0]][1];
		var depth = sched.types[boxparam[0]][2];
		
		var geometry = new THREE.BoxGeometry(width, height, depth);
		
		geometry.translate(boxparam[2] + width/2,
		                    boxparam[3] + height/2,
		                    boxparam[4] + depth/2);
		                    
		//for box start coordinates
		updateBoundingBox(boxparam[2], boxparam[3], boxparam[4]);
		//include box dimensions, too
		updateBoundingBox(boxparam[2] + width, boxparam[3] + height, boxparam[4] + depth);
		
		var cube = new THREE.Mesh( geometry);
		cube.updateMatrix();
		
		boxes.merge(cube.geometry, cube.matrix);
		cube = null;
		
		/*debugMessage(i+"b", boxparam[2] + geometry.parameters.width/2,
		                    boxparam[3] + geometry.parameters.height/2,
		                    boxparam[4] + geometry.parameters.depth/2);*/
	}
	
	var cubes = new THREE.Mesh( boxes, 
	
	  //new THREE.MeshBasicMaterial({wireframe: true, color: 'green'})
      
    new THREE.MeshLambertMaterial({color: 'green', side: THREE.DoubleSide, shading: THREE.FlatShading})
  );
	
	scene.add(cubes);
	
	boxesGeometry = cubes;
}

function debugMessage(message, x, y, z)
{
  var s1 = makeTextSprite2(message);

  s1.position.x = x;
  s1.position.y = y;
  s1.position.z = z;
  scene.add(s1);
}

function findPoint(point, graphobject, objToFill)
{
  //make edges
  var found = false;
  var i = -1;
  for (i = 0; i < graphobject.edges.length && !found; i++)
  {
      var a = graphobject.nodes[graphobject.edges[i][0] - 1];
      var b = graphobject.nodes[graphobject.edges[i][1] - 1];
      
      var eq = 0;
      var included = true;
      for(var j=0; j<3; j++)
      {
        var compj = point.getComponent(j);
        if(a[j+1] == compj && b[j+1] == compj)
        {
          eq++;
        }
        
        if (a[j+1] < b[j+1])
        {
          included = (a[j+1] <= compj) && (compj <= b[j+1])
        }
        else if(a[j+1] > b[j+1])
        {
          included = (b[j+1] <= compj) && (compj <= a[j+1])
        }
      }
              
      if(eq == 2 && included)
      {
        found = true;
        console.log("found");
        //add_edge2(a[1], a[2], a[3], b[1], b[2], b[3], notWorkingDefect);
      }
  }
  
  if(found)
  {
    var idx1 = graphobject.edges[i][0];
    var idx2 = graphobject.edges[i][1];
    
    add_edge2Index(i, graphobject, notWorkingDefect);
    
    for (var j = i+1; j < graphobject.edges.length; j++)
    {
      if(graphobject.edges[j][0] == idx1)
      {
        idx1 = graphobject.edges[j][1];
        add_edge2Index(j, graphobject, notWorkingDefect);
      }
      else if(graphobject.edges[j][1] == idx1)
      {
        idx1 = graphobject.edges[j][0];
        add_edge2Index(j, graphobject, notWorkingDefect);
      }
      else if(graphobject.edges[j][0] == idx2)
      {
        idx2 = graphobject.edges[j][1];
        add_edge2Index(j, graphobject, notWorkingDefect);
      }
      else if(graphobject.edges[j][1] == idx2)
      {
        idx2 = graphobject.edges[j][0];
        add_edge2Index(j, graphobject, notWorkingDefect);
      }
    }
    
    for (var j = i-1; j >= 0; j--)
    {
      if(graphobject.edges[j][0] == idx1)
      {
        idx1 = graphobject.edges[j][1];
        add_edge2Index(j, graphobject, notWorkingDefect);
      }
      else if(graphobject.edges[j][1] == idx1)
      {
        idx1 = graphobject.edges[j][0];
        add_edge2Index(j, graphobject, notWorkingDefect);
      }
      else if(graphobject.edges[j][0] == idx2)
      {
        idx2 = graphobject.edges[j][1];
        add_edge2Index(j, graphobject, notWorkingDefect);
      }
      else if(graphobject.edges[j][1] == idx2)
      {
        idx2 = graphobject.edges[j][0];
        add_edge2Index(j, graphobject, notWorkingDefect);
      }
    }
    
  }
  
  return found;
}

function makeGeometry(graphobject, objToFill)
{
		
		//make injections
		for (var i = 0; i < graphobject.inj.length; i++)
    {
				var n = graphobject.nodes[graph.inj[i] - 1];
        add_vertex(n[1], n[2], n[3], materialInjection);
		}
		

	  //make edges
    for (var i = 0; i < graphobject.edges.length; i++)
    {
        var a = graphobject.nodes[graphobject.edges[i][0] - 1];
        var b = graphobject.nodes[graphobject.edges[i][1] - 1];

				if(objToFill != null)
				{
				  //add_edge2(a[1], a[2], a[3], b[1], b[2], b[3], objToFill);
				  add_edge2Index(i, graphobject, objToFill);
				}
				else
				{
					//if(otherside >= 2)//asta e pentru qubits si nu pentru celule. eu calculez in celule. in pula mea iar am incurcat...
				  otherside = 0;
				  for(var j=1; j<4; j++)
				  {
					  if(Math.abs(a[j]) % 2 != 0)
					  {
						  otherside++;
					  }
				  }
						
				  if(otherside == 3)
				  {//celulele primare au coordonate impare, iar cele duale au coordonate pare
				    //add_edge2(a[1], a[2], a[3], b[1], b[2], b[3], primalDefects);
  				  add_edge2Index(i, graphobject, primalDefects);
				  }
				  //else
				  if(otherside == 0)
				  {
				    //add_edge2(a[1], a[2], a[3], b[1], b[2], b[3], dualDefects);
            add_edge2Index(i, graphobject, dualDefects);
				  }
				  
				  if(i == graphobject.edges.length - 1)
				  {
				    //the connection that was not constructed should be highlighted with a dual segment
				    //to be distinct from the green blocks
				    //add_edge2(a[1], a[2], a[3], b[1], b[2], b[3], notWorkingDefect);
				    add_edge2Index(i, graphobject, notWorkingDefect);
				  }
        }
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
    camera = new THREE.PerspectiveCamera( 45, WIDTH/HEIGHT, 0.1, 1000000);
    //camera.position.set(sx/2, sy/2, sz/2+sx*10);
    camera.position.set(sx/2, sy/2, sz + sx*5);
    console.log("xs" + sx);
    camera.up = new THREE.Vector3(-1,0,0);
    
    scene.add(camera);

    // Models
    spheregeometry = new THREE.SphereGeometry(.5, 3, 3)

    // Lighting
    var ambientLight = new THREE.AmbientLight(0x552222);
    ambientLight.intensity=.1;
    scene.add(ambientLight);

    var pointLight1 = new THREE.PointLight(0xFFFFFF);
    pointLight1.position.x = 1000;
    pointLight1.position.y = 1000;
    pointLight1.position.z = 1300;
    scene.add(pointLight1);
    
    /*var pointLight2 = new THREE.PointLight(0xFFFFFF);
    pointLight2.position.x = -1000;
    pointLight2.position.y = 1000;
    pointLight2.position.z = 1300;
    scene.add(pointLight2);*/

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
	  
	  if(drawBoxes)
	  {
			makeBoxes(boxes0);
			//makeBoxes(boxes1);
			//makeBoxes(boxes2);
		}
		
  	makeGeometry(graph2, null);//connects pins to boxes
  	
  	if(drawGeometry)
  	{
			makeGeometry(graph, null);//the circuit
			makeGeometry(graph3, debugDefects1 /*all is in debug*/);
			makeGeometry(graph4, debugDefects2 /*all is in debug*/);
		}
		
    makeLineSegments(makebuffered(debugDefects1), materialDebug1);
    makeLineSegments(makebuffered(debugDefects2), materialDebug2);
    makeLineSegments(makebuffered(notWorkingDefect), materialNotWorking);
    
    primalGeometry = makeLineSegments(makebuffered(primalDefects), materialPrimal);
    dualGeometry = makeLineSegments(makebuffered(dualDefects), materialDual);

    makeBoundingBox();
    
    /*http://stackoverflow.com/questions/32955103/selecting-object-with-mouse-three-js*/
    /*select new defect : mouse down + click*/
    /*
    //linePrimal and lineDual disappered from the code
    objectsToClick = [linePrimal, lineDual];
    
    renderer.domElement.addEventListener( 'mousedown', onDocumentMouseDown );
    function onDocumentMouseDown( event ) 
    {
      if (!event.shiftKey)
      {
        return;
      }
      
      event.preventDefault();
      var found = false;
      //dimension of the mouse coordinate window to search
      var dim = 5;
      for(var i=-dim/2; i<dim/2+1 && !found; i++)
      {
        for(var j=-dim/2; j<dim/2+1 && !found; j++)
        {
          found = mouse3dray(event.clientX + i, event.clientY + j);
        }
      }
      
      if(found)
      {
        //linePrimal.material.color.setHex( Math.random() * 0xffffff );
        linePrimal.material.opacity = 0.5;
      }
      else
      {
        linePrimal.material.opacity = 1;
      }
    }    
    */
}

function mouse3dray(x, y)
{
  //transform screen coordinates into opengl coordinates
  var mouse3D = new THREE.Vector3( ( x / renderer.domElement.innerWidth ) * 2 - 1,   
                            -( y / renderer.domElement.innerHeight ) * 2 + 1,  
                            0.01/*z coordinate is back*/ );    
                             
  var raycaster =  new THREE.Raycaster();                                        
  raycaster.setFromCamera( mouse3D, camera );
  var intersects = raycaster.intersectObjects( objectsToClick );
  
  var found = false;
  if ( intersects.length > 0 ) 
  {
    console.log(intersects[0].point);

    scene.remove(lineNotWorking);
    notWorkingDefect.vertices = [];
    
    found = findPoint(intersects[0].point, graph2, null);
    if(!found)
    {
      found = findPoint(intersects[0].point, graph, null);
    }

    if(found)
    {
      makeLineSegments(makebuffered(notWorkingDefect), materialNotWorking);
    }
  }

  return found;
}

function makeLineSegments(arrBuffered, material)
{
	var lastLineSeg;
	
	for(var i=0; i<arrBuffered.length; i++)
	{
		var lineSegs = new THREE.LineSegments(arrBuffered[i], material);
		
    scene.add(lineSegs); 
    
    lastLineSeg = lineSegs;
  }
  
  return lastLineSeg;
}

function makebuffered(normalgeom)
{
	var retArrays = new Array();
	
	var chunkLength = normalgeom.vertices.length;
	console.log("nr vertices:" + chunkLength);
	var remainingVertices = chunkLength;
	var processedVertices = 0;
	if(chunkLength > 4000000)
	{
		chunkLength = 4000000;
	}
	
	while(remainingVertices > 0)
	{
		if(remainingVertices <= chunkLength)
		{
			chunkLength = remainingVertices;
		}
	
		var positions = new Float32Array(chunkLength * 3);
		var indices = new Uint32Array(chunkLength);
		for (var i = 0; i < chunkLength; i++)
		{
				var posInNormalGeom = processedVertices + i;
		    positions[i * 3] = normalgeom.vertices[posInNormalGeom].x;
		    positions[i * 3 + 1] = normalgeom.vertices[posInNormalGeom].y;
		    positions[i * 3 + 2] = normalgeom.vertices[posInNormalGeom].z;
		    indices[i] = i;
		}

		var buffGeom = new THREE.BufferGeometry();
		buffGeom.addAttribute('position', new THREE.BufferAttribute(positions, 3));
		buffGeom.setIndex(new THREE.BufferAttribute(indices, 1));
		
		retArrays.push(buffGeom);
		
		remainingVertices -= chunkLength;
		processedVertices += chunkLength;
	}
	
	return retArrays;
}

function checkTime()
{
  /*either use the standby time 
    or return true when renderRunning is a semaphore*/
  return true;
  //return (renderLastMove + renderStandbyAfter >= Date.now());
}

function startAnimation()
{
  renderLastMove = Date.now();
  
  /*daca nu ruleaza inca animatia*/
  if (!renderRunning)
  {
    //verifica de are voie sa ruleze
    if(checkTime())
    {
      renderRunning = true;
      requestAnimationFrame(animate);
    }
  }
}

function animate()
{
  renderer.render(scene, camera);
  
  if (!checkTime() || !renderRunning)
  {
    renderRunning = false;
  }
  else
  {
    requestAnimationFrame(animate);
  }
  
  controls.update();
  stats.update();
  
  /*console.log(renderRunning);
  if(renderRunning)
  {
    renderer.render(scene, camera);
    requestAnimationFrame(animate);
    controls.update();
    stats.update()
  }*/
}

function makeTextSprite2(message)
{
	var canvas = document.createElement('canvas');
	var size = 1024; // CHANGED
	canvas.width = size;
	canvas.height = size;
	var context = canvas.getContext('2d');
	context.fillStyle = '#000000'; // CHANGED
	context.textAlign = 'center';
	context.font = '200px Arial';
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
	sp.scale.set( 100, 100, 1 ); // CHANGED

	return sp;
}

function checkboxClick(chkname, geometry)
{
	var c = document.getElementById(chkname);
  if (c.checked)
  {
    scene.add(geometry);
  } 
  else 
  { 
		scene.remove(geometry);
  }
}
