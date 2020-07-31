#include <stdlib.h>
#include <stdio.h>
#include <cstdint>
#include <math.h>
#include <iostream>
#include <fstream>

#include "tetgen/tetgen.h"
#include <emscripten.h>

EMSCRIPTEN_KEEPALIVE
extern "C" int babylon2tetgen(
    uint32_t  bnumpositions,                     //  data from babylon
    double    bpositions[],                      //
    uint32_t  bnumindices,                       // 
    uint32_t  bindices[],                        //
    uint32_t* numVerticesOut,                    //  data to babylon
    double    verticesOut[],                     //
    uint64_t* numTetrahedraOut,                  //
    uint32_t  tetrahedraOut[])                   //
{

    // debug inputs to make sure transfer
    // of data from BabylonJS is OK
    printf("startBabylon2Tetgen!\n");
    printf("numpos=%d\n",bnumpositions);
    printf("numind=%d\n",bnumindices);
    // check input counts
    // for(int i=0;i<10;i++) std::cout << std::setprecision (17) << "Pos:" << bpositions[i] << std::endl;
    // for(int i=0;i<10;i++) std::cout << std::setprecision (17) << "Indices:" << bindices[i] << std::endl;
    
    tetgenio in, out;


    //////////////////////////////
    // tetgen options begin
    /////////////////////////////
    tetgenbehavior b;
    b.object = tetgenbehavior::POLY;
    b.zeroindex = 1;
    b.docheck = 1;
    b.verbose = 1;
    b.quiet = 1;
    b.diagnose = 0;
    b.mindihedral = 20;
    b.minratio = 1.414;
    b.plc = 1;
    b.quality = 1;
    b.epsilon = 1.e-10;
    //b.facesout = 1;
    //b.edgesout = 1;
    //b.neighout = 1;
    // Preserves the input surface mesh
    b.nobisect = 1;
    b.nobisect_nomerge = 1;
    // Disable removal of duplicate vertices and faces
    b.nomergefacet = 1;
    b.nomergevertex = 1;
    b.nojettison = 1;
    // Maximum tetrahedron volume constraint. Assumes uniform mesh density on the surface
    // b.fixedvolume = 1;
    // creates linear tetrahedrals
    b.order = 1;
    ////////////////////////////
    // tetgen options end
    ////////////////////////////


    // initialize in with BABYLON counts
    in.numberofpoints = bnumpositions;
    in.pointlist = new REAL[bnumpositions * 3];
    in.numberoffacets = bnumindices/3;
    in.facetlist = new tetgenio::facet[in.numberoffacets];
    in.facetmarkerlist = new int[in.numberoffacets];

    // copy BABYLON pointlist to in
    memcpy(in.pointlist, bpositions, sizeof(REAL) * bnumpositions);

    uint64_t numfacets = in.numberoffacets;
    for(int i = 0; i < numfacets; i++) {
      tetgenio::facet* f = &in.facetlist[i];
      f->holelist = (REAL*)NULL;
      f->numberofholes = 0;
      f->numberofpolygons = 1;
      f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
      tetgenio::polygon *p = &f->polygonlist[0];
      p->numberofvertices = 3;
      p->vertexlist = new int[p->numberofvertices];
      p->vertexlist[0] = (int)bindices[i*3+0];
      p->vertexlist[1] = (int)bindices[i*3+1];
      p->vertexlist[2] = (int)bindices[i*3+2];
      in.facetmarkerlist[i] = 0;
    }

    // printf("Writing to files...\n");
    // char out_name[] = "debug_in_";
    // in.save_nodes(out_name);
    // in.save_poly(out_name);
    // in.save_elements(out_name);
    
    ///////////////////////////////////////////////////////////////////

    printf("tetrahedralizeStart!\n");
    tetrahedralize(&b, &in, &out);
    printf("tetrahedralizeEnd!\n");

    ////////////////////////////////////////////////////////////////////

	

    for(int i=0; i < out.numberofpoints; i++){
        int vert_index = 3*i;
  	verticesOut[vert_index]   = out.pointlist[vert_index];
        verticesOut[vert_index+1] = out.pointlist[vert_index+1];
        verticesOut[vert_index+2] = out.pointlist[vert_index+2];
    }
    *numVerticesOut = out.numberofpoints;
    printf("numPosOut=%d\n",out.numberofpoints);



    for(int i=0; i < out.numberoftetrahedra; i++){
        int tet_index = 4*i;
  	tetrahedraOut[tet_index]   = out.tetrahedronlist[tet_index];
  	tetrahedraOut[tet_index+1] = out.tetrahedronlist[tet_index+1];
  	tetrahedraOut[tet_index+2] = out.tetrahedronlist[tet_index+2];  	  	
  	tetrahedraOut[tet_index+3] = out.tetrahedronlist[tet_index+3]; 
    }
    *numTetrahedraOut = out.numberoftetrahedra;
    printf("numTetOut=%d\n",out.numberoftetrahedra);



    return 0;

}


int main() {
  printf("main!\n");
  return 0;
}
