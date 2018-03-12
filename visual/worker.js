importScripts('libs/three.js');
importScripts('libs/PalerBoxBufferGeometry.js');


var d = new Date();
var n = d.getTime(); 

function profile(msg)
{
  console.log(new Date().getTime() + " " + msg);
}

function echoGeometries()
{
  console.log("vtx " + self.totalBounding.getAttribute('position').array.length);
  console.log("vtx " + self.totalPrimal.getAttribute('position').array.length);
  console.log("vtx " + self.totalDual.getAttribute('position').array.length);
  
  console.log("idx " + self.totalBounding.getIndex().array.length);
  console.log("idx " + self.totalPrimal.getIndex().array.length);
  console.log("idx " + self.totalDual.getIndex().array.length);
}

function copyToBuffer(buffer, vectors)
{
  var offset = 0;
  for ( var i = 0, l = vectors.length; i < l; i ++ ) 
  {
    buffer[i] = vectors[i];
  }
}
/*
.position = .P
.defects = .D
.defects.primal = .D.p
.defects.dual = .D.d
.corrs = .C
*/

function makeBoundingBoxes(plumb)
{
  self.totalBounding = new THREE.PalerBoxBufferGeometry(1,1,1);
  self.totalPrimal = new THREE.PalerBoxBufferGeometry(1,1,1);
  self.totalDual = new THREE.PalerBoxBufferGeometry(1,1,1);
  
	for (var i=0; i<plumb.plumbs.length; i++)
  {
		var boxparam = plumb.plumbs[i].P;
		
		self.totalBounding.addBox(boxparam, [1, 1, 1]);
		
		var ppd = plumb.plumbs[i].D;
		
		//if(ppd.p[0] == 1)
		var bitPosition = 8;
		
		if((ppd.p & bitPosition) == bitPosition)
		{
		  var dim = [0.3, 0.3, 0.3];
		  
		  //var b1 = constructBox(boxparam, dim, 'orange', false);
		  //self.totalPrimal.merge(b1);
		  self.totalPrimal.addBox(boxparam, dim);
		  
		  for(var j=1; j<4; j++)
		  {
		    //if(ppd.p[j] == 0)
		    bitPosition = 1 << (3 - j);
		    if((ppd.p & bitPosition) == bitPosition)
		    {
				  dim[j - 1] = 0.7;
				  boxparam[ j - 1] += 0.3;
				  
				  //var b2 = constructBox(boxparam, dim, 'red', false);
				  //self.totalPrimal.merge(b2);
				  self.totalPrimal.addBox(boxparam, dim);
				   
				  dim[j - 1] = 0.3;
				  boxparam[ j - 1] -= 0.3;
		    }
		  }
		}
		
		//if(ppd.d[0] == 1)
		bitPosition = 8;
		if((ppd.d & bitPosition) == bitPosition)
		{
		  boxparam[0] += 0.5;
		  boxparam[1] += 0.5;
		  boxparam[2] += 0.5;
		  
		  var dim = [0.3, 0.3, 0.3];
		  //var b3 = constructBox(boxparam, dim, 'violet', false);
		  //self.totalDual.merge(b3);
		  self.totalDual.addBox(boxparam, dim);
		
		  for(var j=1; j<4; j++)
		  {
	      //if(ppd.d[j] == 0)
	      bitPosition = 1 << (3 - j);
      	if((ppd.d & bitPosition) == 0)
	        continue;

	      dim[j - 1]= 0.7;
	      boxparam[ j - 1] += 0.3;
	      
		    //var b4 = constructBox(boxparam, dim, 'blue', false);
		    //self.totalDual.merge(b4);
		    self.totalDual.addBox(boxparam, dim);
		    
		    boxparam[ j - 1] -= 0.3;
		    dim[j - 1] = 0.3;
		  }
		}
	}
	
	profile("struct");
	
	self.totalBounding.addAttributes();
	self.totalPrimal.addAttributes();
	self.totalDual.addAttributes();
	
	profile("built");
}

self.addEventListener('message', function(msg) {
  // We received a message from the main thread!
  // do some computation that may normally cause the browser to hang
  
  if(msg.data.type == 'load')
  {
    profile("start");
    
    importScripts('circuit.raw.in.adam?id='+n);
    
    var ldata = makeBoundingBoxes(plumb);
    echoGeometries();
    profile("made boxes");
    
    self.postMessage({
      type: 'loaded',
      b1v: self.totalBounding.getAttribute('position').array.length,
      b2v: self.totalPrimal.getAttribute('position').array.length,
      b3v: self.totalDual.getAttribute('position').array.length,
      b1i: self.totalBounding.getIndex().array.length,
      b2i: self.totalPrimal.getIndex().array.length,
      b3i: self.totalDual.getIndex().array.length
    });
    
  }
  else if(msg.data.type == 'fill')
  {
    profile("copy1");
    copyToBuffer( msg.data.buff1v, self.totalBounding.getAttribute('position').array);
    copyToBuffer( msg.data.buff2v, self.totalPrimal.getAttribute('position').array );
    copyToBuffer( msg.data.buff3v, self.totalDual.getAttribute('position').array );
    copyToBuffer( msg.data.buff1i, self.totalBounding.getIndex().array);
    copyToBuffer( msg.data.buff2i, self.totalPrimal.getIndex().array );
    copyToBuffer( msg.data.buff3i, self.totalDual.getIndex().array );
    profile("copy2");
        
    //  now send back the results
    self.postMessage({
      type: 'results',
          buff1v : msg.data.buff1v,
          buff2v : msg.data.buff2v,
          buff3v : msg.data.buff3v,
          buff1i : msg.data.buff1i,
          buff2i : msg.data.buff2i,
          buff3i : msg.data.buff3i,
        }, [msg.data.buff1v.buffer, msg.data.buff2v.buffer, msg.data.buff3v.buffer, msg.data.buff1i.buffer, msg.data.buff2i.buffer, msg.data.buff3i.buffer]);
  }
})
