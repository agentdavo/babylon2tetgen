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
	  
          

	  var pos = sphere.getVerticesData(BABYLON.VertexBuffer.PositionKind);
	  var ind = sphere.getIndices();

	  var posData = new Float64Array(pos);
	  var indData = new Int32Array(ind);

          console.log(posData);
          console.log(indData);

	  var posDataBytes = posData.length * posData.BYTES_PER_ELEMENT;
	  var posDataPtr = Module._malloc(posDataBytes);
	  var posData_alloc = new Uint8Array(Module.HEAPU8.buffer, posDataPtr, posDataBytes);
	  posData_alloc.set(new Uint8Array(posData.buffer));

	  var indDataBytes = indData.length * indData.BYTES_PER_ELEMENT;
	  var indDataPtr = Module._malloc(indDataBytes);
	  var indData_alloc = new Uint8Array(Module.HEAPU8.buffer, indDataPtr, indDataBytes);
	  indData_alloc.set(new Uint8Array(indData.buffer));

	  var posDataOutCount = Module._malloc(4);
      var indDataOutCount = Module._malloc(4);
	  var posDataOut = Module._malloc(posData.length * 8 * 2);
	  var indDataOut = Module._malloc(posData.length * 4 * 20);
	  
	  Module.ccall('babylon2tetgen',
                        null,
                      ['number','number', 'number', 'number',
                       'number', 'number', 'number', 'number'],
                      [
                          posData.length, posData_alloc.byteOffset, indData.length, indData_alloc.byteOffset,
                          posDataOutCount, posDataOut, indDataOutCount, indDataOut
                      ]);
	  
          var newPosCount = getValue(posDataOutCount, 'i32');
          var newIndCount = getValue(indDataOutCount, 'i32');

          // check output counts match tetgen
	      console.log("newPosCount:");    
          console.log(newPosCount);
	      console.log("newIndCount:");    
          console.log(newIndCount);


	      
	      // BABYLON.MeshBuilder.CreatePolyhedron custom expecting
          // vertexPoints = [ [0,0,1.7] , [1.6,0,-0.5] , [-0.8,1.4,-0.5] , [-0.8,-1.4,-0.5] ]
          // facePoints   = [ [0,1,2] , [0,2,3] , [0,3,1] , [1,3,2] ]


	      
         ////////////////////////////////////////////////////////////////////	 
	      
          console.log("populating vertex points array");
	      
          var vertexPoints = [];
          for (let v = 0; v < newPosCount ;) {
			  
			  var tetraPositions = [];
			  var temp = [];
			  
			  var tetraPosX = Module.HEAPF64[posDataOut / Float64Array.BYTES_PER_ELEMENT + v]; v++;  // X
			  var tetraPosY = Module.HEAPF64[posDataOut / Float64Array.BYTES_PER_ELEMENT + v]; v++;  // Y
			  var tetraPosZ = Module.HEAPF64[posDataOut / Float64Array.BYTES_PER_ELEMENT + v]; v++;  // Z
              tetraPositions.push(tetraPosX,tetraPosY,tetraPosZ);
			  
			  temp = [];
			  tetraPosX = Module.HEAPF64[posDataOut / Float64Array.BYTES_PER_ELEMENT + v]; v++;  // X
			  tetraPosY = Module.HEAPF64[posDataOut / Float64Array.BYTES_PER_ELEMENT + v]; v++;  // Y
			  tetraPosZ = Module.HEAPF64[posDataOut / Float64Array.BYTES_PER_ELEMENT + v]; v++;  // Z
              tetraPositions.push(tetraPosX,tetraPosY,tetraPosZ);
			  
			  temp = [];
			  tetraPosX = Module.HEAPF64[posDataOut / Float64Array.BYTES_PER_ELEMENT + v]; v++;  // X
			  tetraPosY = Module.HEAPF64[posDataOut / Float64Array.BYTES_PER_ELEMENT + v]; v++;  // Y
			  tetraPosZ = Module.HEAPF64[posDataOut / Float64Array.BYTES_PER_ELEMENT + v]; v++;  // Z
              tetraPositions.push(tetraPosX,tetraPosY,tetraPosZ);
			  
			  temp = [];
			  tetraPosX = Module.HEAPF64[posDataOut / Float64Array.BYTES_PER_ELEMENT + v]; v++;  // X
			  tetraPosY = Module.HEAPF64[posDataOut / Float64Array.BYTES_PER_ELEMENT + v]; v++;  // Y
			  tetraPosZ = Module.HEAPF64[posDataOut / Float64Array.BYTES_PER_ELEMENT + v]; v++;  // Z
              tetraPositions.push(tetraPosX,tetraPosY,tetraPosZ);
			  
			  vertexPoints.push(tetraPositions);
			  
          }
          console.log(vertexPoints);

         ////////////////////////////////////////////////////////////////////

          console.log("populating face indices array");
	      
          var faceIndices = [];
          for (let f = 0; f < newIndCount ;) {

              var tetraFaceIndices = [];
			  var temp = [];

	          var tetraInd0 = Module.HEAP32[indDataOut / Uint32Array.BYTES_PER_ELEMENT + f]; f++;
              var tetraInd1 = Module.HEAP32[indDataOut / Uint32Array.BYTES_PER_ELEMENT + f]; f++;
              var tetraInd2 = Module.HEAP32[indDataOut / Uint32Array.BYTES_PER_ELEMENT + f]; f++;
              var tetraInd3 = Module.HEAP32[indDataOut / Uint32Array.BYTES_PER_ELEMENT + f]; f++;

              temp.push(tetraInd0, tetraInd1, tetraInd2);
              tetraFaceIndices.push(temp);

              temp = [];
              temp.push(tetraInd1, tetraInd3, tetraInd2);
              tetraFaceIndices.push(temp);

              temp = [];
              temp.push(tetraInd0, tetraInd2, tetraInd3);
              tetraFaceIndices.push(temp);

              temp = [];
              temp.push(tetraInd0, tetraInd3, tetraInd1);
              tetraFaceIndices.push(temp);

              faceIndices.push(tetraFaceIndices);

          }
          console.log(faceIndices);

	 ////////////////////////////////////////////////////////////////////

          var numOfTets = 10;   // TODO

          for ( var i = 0; i < numOfTets ; i++ ) {

            var v = vertexPoints[i];
            var f = faceIndices[i];

            console.log(v);
            console.log(f);

            var tetrahedron = { "name":"Tetrahedron",
                                "category":["Platonic Solid"],
                                "vertex": [v],
                                "face": [f]
                               }; 

            var tetMesh = BABYLON.MeshBuilder.CreatePolyhedron("tet_" +i, {custom: tetrahedron}, scene);

          }
               
     
	 
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
