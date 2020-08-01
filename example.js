      var g_emscripten_alive = false;
	  
      function check_em() { return function() { g_emscripten_alive=true; } }
	  
      var check_emscripten = function() {
	  if (Module) {
	      Module['onRuntimeInitialized'] = check_em();
	  } else {
	      window.setTimeout(function() { check_emscripten(); }, 200);
	  }
      }

      var g_call_it_count=100;

      var call_it = function(sphere, resolve,reject) {

	  if (!g_emscripten_alive) {
	      g_call_it_count--;
	      if (g_call_it_count<=0) { reject("Error, emscripten didn't start"); g_call_it_count=100; return; }
	      window.setTimeout(function() { call_it(sphere,resolve,reject); },200);
	      return;
	  }

      var sphereVertexData = BABYLON.VertexData.ExtractFromMesh(sphere, true, true);

	  var pos = sphereVertexData.positions;
	  var ind = sphereVertexData.indices;

	  var posData = new Float64Array(pos);
	  var indData = new Uint32Array(ind);

          console.log(posData);
          console.log(indData);
		  
      // Allocate some space in the heap for the data
      var posBuffer = Module._malloc(posData.length * posData.BYTES_PER_ELEMENT);
      var indBuffer = Module._malloc(indData.length * indData.BYTES_PER_ELEMENT);

	  // Assign the data to the heap
      Module.HEAPF64.set(posData, posBuffer >> 3);
      Module.HEAPU32.set(indData, indBuffer >> 2);
	  

	  var posDataOutCount = Module._malloc(1 * Uint32Array.BYTES_PER_ELEMENT);
      var indDataOutCount = Module._malloc(1 * Uint32Array.BYTES_PER_ELEMENT);
	  var posDataOut = Module._malloc(posData.length * posData.BYTES_PER_ELEMENT);
	  var indDataOut = Module._malloc(posData.length * indData.BYTES_PER_ELEMENT);
	  
	  Module.ccall('babylon2tetgen',
                        null,
                      ['number','number', 'number', 'number',
                       'number', 'number', 'number', 'number'],
                      [
                          posData.length, posBuffer, indData.length, indBuffer,
                          posDataOutCount, posDataOut, indDataOutCount, indDataOut
                      ]);
	  
          var newPosCount = getValue(posDataOutCount, 'i32');
          var newIndCount = getValue(indDataOutCount, 'i32');

          // check output counts match tetgen
	      console.log("newPosCount:");    
          console.log(newPosCount);
	      console.log("newIndCount:");    
          console.log(newIndCount);

		  var tetVertexDataPos = [];
		  var tetVertexDataInd = [];
	      
          console.log("populating vertex points array");
	      
          for (let v = 0; v < newPosCount * 3 ;) {
			  tetVertexDataPos.push(Module.HEAPF64[posDataOut / Float64Array.BYTES_PER_ELEMENT + v]); v++;
          }
          console.log(tetVertexDataPos);

         ////////////////////////////////////////////////////////////////////

          console.log("populating face indices array");
	      
          for (let f = 0; f < newIndCount * 4 ;) {
	          var tetraInd0 = Module.HEAP32[indDataOut / Uint32Array.BYTES_PER_ELEMENT + f]; f++;
              var tetraInd1 = Module.HEAP32[indDataOut / Uint32Array.BYTES_PER_ELEMENT + f]; f++;
              var tetraInd2 = Module.HEAP32[indDataOut / Uint32Array.BYTES_PER_ELEMENT + f]; f++;
              var tetraInd3 = Module.HEAP32[indDataOut / Uint32Array.BYTES_PER_ELEMENT + f]; f++;
              tetVertexDataInd.push(tetraInd0, tetraInd1, tetraInd2);
              tetVertexDataInd.push(tetraInd1, tetraInd3, tetraInd2);
              tetVertexDataInd.push(tetraInd0, tetraInd2, tetraInd3);
              tetVertexDataInd.push(tetraInd0, tetraInd3, tetraInd1);
          }
          console.log(tetVertexDataInd);
		  
		  
		 Module._free(posBuffer);
         Module._free(indBuffer);
		 Module._free(posDataOutCount);
		 Module._free(posDataOut);
		 Module._free(indDataOutCount);
		 Module._free(indDataOut);

		  
		  
		 ////////////////////////////////////////////////////////////////////////////////////
	     // BABYLON.MeshBuilder.CreatePolyhedron custom expecting
         // vertexPoints = [ [0,0,1.7] , [1.6,0,-0.5] , [-0.8,1.4,-0.5] , [-0.8,-1.4,-0.5] ]
         // facePoints   = [ [0,1,2] , [0,2,3] , [0,3,1] , [1,3,2] ]
         ////////////////////////////////////////////////////////////////////////////////////
		  	  	    
			tetraVertexData = new BABYLON.VertexData();
			tetraVertexData.positions = tetVertexDataPos;
			tetraVertexData.indices = tetVertexDataInd;
			
		    var tetraMesh = new BABYLON.Mesh("tet", scene );
			tetraVertexData.applyToMesh(tetraMesh);
			
			tetraMesh.material = sphere.material;
	        tetraMesh.enableEdgesRendering();
		    tetraMesh.edgesWidth = 1.0;
	        tetraMesh.edgesColor = new BABYLON.Color4(0, 1, 0, 1);

	  g_call_it_count=100;
      };

      var processmesh = function(mesh) {
	  var prom = new Promise(function(resolve,reject) {
	      window.setTimeout(function() { call_it(mesh,resolve,reject); },200);
	  });
	  return prom;
      }
      
      var Module = {
	  print: (function() { return function(text) { console.log(text); } })()
      };
