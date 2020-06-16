var url = "https://cdn.rawgit.com/BabylonJS/Extensions/master/Polyhedron/polyhedra.js";
var s = document.createElement("script");
s.type = "text/javascript";
s.src = url;
document.head.appendChild(s);


WEBTETGEN = function (mesh) {

    var orgVertexData;
    var source;

    if (mesh) {

        if (mesh.isAnInstance) {
            Logger.Warn("Cannot operate on instance meshes.");
            return null;
        }

        const wm = mesh.computeWorldMatrix(true);

        orgVertexData = BABYLON.VertexData.ExtractFromMesh(mesh, true, true);
        orgVertexData.transform(wm);
        var indices = orgVertexData.indices;
        var positions = orgVertexData.positions;

        ////////////////////parameter parsing////////////////////////////
        var positionDblArray = new Float64Array(positions);
        var indexIntArray = new Uint32Array(indices);
        console.log(positionDblArray);
        console.log(indexIntArray);

        ///////////////// Make buffer for vertices and indices ///////////////////////
        var positionBuffer = Module._malloc(positionDblArray.length * positionDblArray.BYTES_PER_ELEMENT);
        var indexBuffer = Module._malloc(indexIntArray.length * indexIntArray.BYTES_PER_ELEMENT);
        Module.HEAPF64.set(positionDblArray, positionBuffer >> 3);
        Module.HEAP32.set(indexIntArray, indexBuffer >> 2);

        ////////////////////////////prepare to retrieve variables //////////////////////
        var newVerticesCountBuffer = Module._malloc(4);
        var newTetCountBuffer = Module._malloc(4);
        var TetBuffer = Module._malloc(positions.length * 4 * 20);
        var newVerticesBuffer = Module._malloc(positions.length * 8 * 2);

        ////////////////////////////// Call tetgen function ////////////////////////////
        console.log("tetgen starts");
        var result = Module.ccall(
            'babylon2tetgen',	// name of C function 
            'number',	// return type
            ['number', 'number', 'number', 'number',
             'number', 'number', 'number', 'number'],	// argument types
            [
                positions.length, positionBuffer, indices.length, indexBuffer,
                newVerticesCountBuffer, newVerticesBuffer, newTetCountBuffer, TetBuffer
            ]	// arguments
        );
        console.log("tetgen ends");

        ///////////////////////////////Parse result ///////////////////////////////////
        var newIndices = [];
        var newPositions = [];
        var newVCount = getValue(newVerticesCountBuffer, 'i32');
        var newTCount = getValue(newTetCountBuffer, 'i32');

        //////// make new vertices /////////////////
        for (let v = 0; v < newVCount * 3;) {
            newPositions.push(Module.HEAPF64[newVerticesBuffer / Float64Array.BYTES_PER_ELEMENT + v]); v++
        }

        for (let f = 0; f < newTCount * 4;) {
            var tetraInx0 = Module.HEAP32[TetBuffer / Uint32Array.BYTES_PER_ELEMENT + f]; f++;
            var tetraInx1 = Module.HEAP32[TetBuffer / Uint32Array.BYTES_PER_ELEMENT + f]; f++;
            var tetraInx2 = Module.HEAP32[TetBuffer / Uint32Array.BYTES_PER_ELEMENT + f]; f++;
            var tetraInx3 = Module.HEAP32[TetBuffer / Uint32Array.BYTES_PER_ELEMENT + f]; f++;
            newIndices.push(tetraInx0, tetraInx1, tetraInx2);
            newIndices.push(tetraInx0, tetraInx2, tetraInx3);
            newIndices.push(tetraInx0, tetraInx3, tetraInx1);
            newIndices.push(tetraInx1, tetraInx3, tetraInx2);
        }

        ///////////////////////////////free buffers//////////////////////////////////////
        Module._free(positionBuffer);
        Module._free(indexBuffer);
        Module._free(newVerticesCountBuffer);
        Module._free(newTetCountBuffer);
        Module._free(newVerticesBuffer);
        Module._free(TetBuffer);

        ///////////////////////////make new vertex////////////////////////////////////////
        vertexData = new BABYLON.VertexData();
        vertexData.indices = newIndices;
        vertexData.positions = newPositions;
        return vertexData;
    }

    return null;

}