/**
 * @author Mugen87 / https://github.com/Mugen87
 */

THREE.PalerBoxBufferGeometry = function ( widthSegments, heightSegments, depthSegments) {

	THREE.BufferGeometry.call( this );

	this.type = 'PalerBoxBufferGeometry';

	this.parameters = {
		widthSegments: widthSegments,
		heightSegments: heightSegments,
		depthSegments: depthSegments
	};

	var scope = this;

	// segments
	widthSegments = Math.floor( widthSegments ) || 1;
	heightSegments = Math.floor( heightSegments ) || 1;
	depthSegments = Math.floor( depthSegments ) || 1;

	var vertexCount = 50000000;

	// buffers
	this.vertices = new Float32Array( vertexCount * 3 );

	// offset variables
	this.vertexBufferOffset = 0;
	
	//this.vvv = new Array(gridY1 * gridX1);
	this.vvv = new Array(4);

	THREE.PalerBoxBufferGeometry.prototype.addBox = function (pos, dim)
	{
    var width   = dim[0];
    var height  = dim[1];
    var depth   = dim[2];
    
    var poss = {};
    poss ['x'] = pos[0] + width/2;
    poss ['y'] = pos[1] + height/2;
    poss ['z'] = pos[2] + depth/2;

    // build each side of the box geometry
    this.buildPlane( poss, 'z', 'y', 'x', - 1, - 1, depth, height,   width,  depthSegments, heightSegments, 0 ); // px
    this.buildPlane( poss, 'z', 'y', 'x',   1, - 1, depth, height, - width,  depthSegments, heightSegments, 1 ); // nx
    this.buildPlane( poss, 'x', 'z', 'y',   1,   1, width, depth,    height, widthSegments, depthSegments,  2 ); // py
    this.buildPlane( poss, 'x', 'z', 'y',   1, - 1, width, depth,  - height, widthSegments, depthSegments,  3 ); // ny
    this.buildPlane( poss, 'x', 'y', 'z',   1, - 1, width, height,   depth,  widthSegments, heightSegments, 4 ); // pz
    this.buildPlane( poss, 'x', 'y', 'z', - 1, - 1, width, height, - depth,  widthSegments, heightSegments, 5 ); // nz
	}
	
	THREE.PalerBoxBufferGeometry.prototype.addAttributes = function()
	{
	  var vertices2 = new Float32Array( this.vertexBufferOffset );

  	for(var i=0; i<this.vertexBufferOffset; i++)
  	{
	  	vertices2[i] = this.vertices[i];
	  }
	  
	  //clean ?
	  this.vertices = [];
	  	
	  // build geometry
	  this.addAttribute( 'position', new THREE.BufferAttribute( vertices2, 3 ) );
	}

	THREE.PalerBoxBufferGeometry.prototype.addVertex = function(vector, vbo)
  {
			this.vertices[ vbo + 0] = vector.x;
			this.vertices[ vbo + 1 ] = vector.y;
			this.vertices[ vbo + 2 ] = vector.z;
			
			vbo += 3;
			
			return vbo;
  }

  THREE.PalerBoxBufferGeometry.prototype.addFace = function(idx1, idx2, idx3, list)
  {
    this.vertexBufferOffset = this.addVertex(list[idx1], this.vertexBufferOffset);
    this.vertexBufferOffset = this.addVertex(list[idx2], this.vertexBufferOffset);
    this.vertexBufferOffset = this.addVertex(list[idx3], this.vertexBufferOffset);
  }

	THREE.PalerBoxBufferGeometry.prototype.buildPlane = function( pos, u, v, w, udir, vdir, width, height, depth, gridX, gridY, materialIndex ) {

		var segmentWidth	= width / gridX;
		var segmentHeight = height / gridY;

		var widthHalf = width / 2;
		var heightHalf = height / 2;
		var depthHalf = depth / 2;

		var gridX1 = gridX + 1;
		var gridY1 = gridY + 1;

		var vertexCounter = 0;
		


		for ( var iy = 0; iy < gridY1; iy ++ ) {

			var y = iy * segmentHeight - heightHalf;
			
			for ( var ix = 0; ix < gridX1; ix ++ ) {

    		var vector = new THREE.Vector3();
		
				var x = ix * segmentWidth - widthHalf;
        
				// set values to correct vector component
				vector[ u ] = x * udir + pos[ u ];
				vector[ v ] = y * vdir + pos[ v ];
				vector[ w ] = depthHalf + pos[ w ];
				
				this.vvv[iy*gridX1 + ix] = vector;
			}
		}

		for ( iy = 0; iy < gridY; iy ++ ) {

			for ( ix = 0; ix < gridX; ix ++ ) {

				// indices
				var a = ix + gridX1 * iy;
				var b = ix + gridX1 * ( iy + 1 );
				var c = ( ix + 1 ) + gridX1 * ( iy + 1 );
				var d = ( ix + 1 ) + gridX1 * iy;

        // face one
        this.addFace(a, b, d, this.vvv);
        
        //face two
        this.addFace(b, c, d, this.vvv);
			}
		}
	}
};

THREE.PalerBoxBufferGeometry.prototype = Object.create( THREE.BufferGeometry.prototype );
THREE.PalerBoxBufferGeometry.prototype.constructor = THREE.PalerBoxBufferGeometry;
