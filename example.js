      var g_emscripten_alive = false;
      function check_em() { return function() { g_emscripten_alive=true; } }
      var check_emscripten = function() {
	  if (Module) {
	      Module['onRuntimeInitialized'] = check_em();
	  } else {
	      window.setTimeout(function() { check_emscripten(); }, 100);
	  }
      }
      var g_call_it_count=50;
      var call_it = function(sphere, resolve,reject) {
	  if (!g_emscripten_alive) {
	      g_call_it_count--;
	      if (g_call_it_count<=0) { reject("Error, emscripten didn't start"); g_call_it_count=50; return; }
	      window.setTimeout(function() { call_it(sphere,resolve,reject); },100);
	      return;
	  }
	  
	    var pos = sphere.geometry._vertexBuffers.position._buffer._data;
	    var pos_size = sphere.geometry._totalVertices;
	    var indices = sphere.geometry._indices;
	    var indices_size = sphere.geometry._indices.length;

	  var data = new Float64Array(pos);
	    var nDataBytes = data.length * data.BYTES_PER_ELEMENT;
	    var dataPtr = Module._malloc(nDataBytes);
	    var pos_alloc = new Uint8Array(Module.HEAPU8.buffer, dataPtr, nDataBytes);
	  pos_alloc.set(new Uint8Array(data.buffer));

	  
	  var idata = new Int32Array(indices);
	    var inDataBytes = idata.length * idata.BYTES_PER_ELEMENT;
	    var idataPtr = Module._malloc(inDataBytes);
	    var indices_alloc = new Uint8Array(Module.HEAPU8.buffer, idataPtr, inDataBytes);
	  indices_alloc.set(new Uint8Array(idata.buffer));

	  var nhposptr = Module._malloc(8);
          var nhpos_alloc = new Uint8Array(Module.HEAPU8.buffer, nhposptr, 8);

	  var hposptr = Module._malloc(data.length*data.BYTES_PER_ELEMENT*10);
	  var hpos_alloc = new Uint8Array(Module.HEAPU8.buffer, hposptr, data.length*data.BYTES_PER_ELEMENT*10);
	  
	  
	  var ntetptr = Module._malloc(8);
          var ntet_alloc = new Uint8Array(Module.HEAPU8.buffer, nhposptr, 8);

	  var tetsptr = Module._malloc(idata.length*idata.BYTES_PER_ELEMENT*10);
	  var tets_alloc = new Uint8Array(Module.HEAPU8.buffer, tetsptr, idata.length*idata.BYTES_PER_ELEMENT*10);

	  
	    Module.ccall('babylon2tetgen', null, ['number','number', 'number', 'number', 'number', 'number', 'number', 'number'], [pos_size, pos_alloc.byteOffset, indices_size, indices_alloc.byteOffset, nhpos_alloc.byteOffset, hpos_alloc.byteOffset, ntet_alloc.byteOffset, tets_alloc.byteOffset]); 
	  
	  var pos = new Float64Array(hpos_alloc.buffer, hpos_alloc.byteOffset, data.length*8);
	  var idx = new Int32Array(tets_alloc.buffer, tets_alloc.byteOffset, tets_alloc.length);	  
	  
		// Parse positions vertices result
	  var newPositions = [];
	  var vcountarr = new Int32Array(nhpos_alloc.buffer, nhpos_alloc.byteOffset, nhpos_alloc.length);
	  var newVCount = vcountarr[0]; //getValue(nhpos_alloc, 'i32');

	  console.log(newVCount);
		// make new vertices from tetgen data
          for (let v = 0; v < newVCount * 3;) {
	      newPositions.push(pos[v]); v++
	      newPositions.push(pos[v]); v++
	      newPositions.push(pos[v]); v++
            // newPositions.push(); v++
        }
		
		// Parse tetrahedra result
		var newIndices = [];
	  var tcountarr = new Int32Array(ntet_alloc.buffer, ntet_alloc.byteOffset, ntet_alloc.length);
	  var newTCount = tcountarr[0]; //getValue(ntet_alloc, 'i32');
          for (let f = 0; f < newTCount * 4;) {
	      var tetraInx0 = idx[f]; f++;
var tetraInx1 = idx[f]; f++;
var tetraInx2 = idx[f]; f++;
var tetraInx3 = idx[f]; f++;

newIndices.push(tetraInx0, tetraInx1, tetraInx2);
newIndices.push(tetraInx0, tetraInx2, tetraInx3);
newIndices.push(tetraInx0, tetraInx3, tetraInx1);
newIndices.push(tetraInx1, tetraInx3, tetraInx2);
          }

        // free buffers
        //Module._free();
        //Module._free();
        //Module._free();
        //Module._free();
        //Module._free();
        //Module._free();
	//	Module._free();
//		Module._free();

        // make new babylon vertex dataset and apply to mesh structure
        vertexData = new BABYLON.VertexData();
        vertexData.indices = newIndices;
        vertexData.positions = newPositions;
	
	  resolve(vertexData);
	  g_call_it_count=50;
      };

      var processmesh = function(mesh) {
	  var prom = new Promise(function(resolve,reject) {
	      window.setTimeout(function() { call_it(mesh,resolve,reject); },100);
	  });
	  return prom;
      }
      
      var Module = {
	  print: (function() { return function(text) { console.log(text); } })()
      };
