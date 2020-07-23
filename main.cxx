#include <stdlib.h>
#include <stdio.h>
#include <cstdint>
#include <math.h>
#include <iostream>

#include "tetgen/tetgen.h"
#include <emscripten.h>

extern "C" int EMSCRIPTEN_KEEPALIVE babylon2tetgen(
    uint32_t  bnumpositions,                     //  data from babylon
    double    bpositions[],                      //
    uint32_t  bnumindices,                       // 
    uint32_t  bindices[],                        //
    uint32_t* noVerticesOut,                     //  data to babylon
    double    verticesOut[],                     //
    uint64_t* noTetrahedraOut,                   //
    uint32_t  tetrahedraOut[])                 //
{

    printf("startBabylon2Tetgen!\n");

    //printf("numpos=%d\n",bnumpositions);
    //int s = bnumpositions;
    //for(int i=0;i<s;i++)
    //  std::cout << "Pos:" << bpositions[i] << std::endl;
    //int s2 = bnumindices;
    //for(int i=0;i<s2;i++)
    //  std::cout << "Idx:" << bindices[i] << std::endl;
    
    tetgenio in, out;

    // set tetgen options
    tetgenbehavior b;
    b.plc = 1;
    b.quality = 1;
    // b.minratio = 1.2;
    // b.fixedvolume = 1;
    // b.maxvolume = 1.0;
    b.verbose = 2;
    b.nomergefacet = 1;
    b.nomergevertex = 1;
    //b.diagnose = 1;
    b.mindihedral=20;
    b.minratio=1.5;
    
#if 1
    b.plc = 1;
    b.quality = 1;
    b.refine = 1;
    b.coarsen = 0;
    b.minratio = 1.414;
    b.mindihedral = 165.0;
    //b.mindihedral = 15.0;
    b.epsilon = 1.0e-8;
#endif
    
    // All indices start from 0
    in.firstnumber = 1;
    in.mesh_dim = 3;

    // initialize in with BABYLON counts
    in.numberofpoints = bnumpositions;
    in.pointlist = new REAL[bnumpositions * 3];
    in.numberoffacets = bnumindices/3;
    in.facetlist = new tetgenio::facet[in.numberoffacets];

    // copy BABYLON pointlist to in
    memcpy(in.pointlist, bpositions, sizeof(REAL) * bnumpositions);

    uint64_t numfacets = in.numberoffacets;
    for(int i=0;i<numfacets;i++) {
      tetgenio::facet* f;
      tetgenio::polygon *p;
      f= &in.facetlist[i];
      f->numberofpolygons = 1;
      f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
      f->numberofholes = 0;
      f->holelist = (REAL*)NULL;

      p = &f->polygonlist[0];
      p->numberofvertices = 3;
      p->vertexlist = new int[3];
      p->vertexlist[0] = (int)bindices[i*3+0];
      p->vertexlist[1] = (int)bindices[i*3+1];
      p->vertexlist[2] = (int)bindices[i*3+2];
    }
    
#if 0
    uint64_t numfacets = in.numberoffacets;

    // use BABYLON vertice data to populate in_tegenio
    for (int i = 0; i < numfacets; i++)
    {
        tetgenio::facet* f;
        tetgenio::polygon* p;

        f = &in.facetlist[i];
        f->numberofpolygons = 1;
        f->polygonlist = new tetgenio::polygon[f->numberofpolygons];
        f->numberofholes = 0;
        f->holelist = (REAL*)NULL;

        p = &f->polygonlist[0];
        p->numberofvertices = 3;
        p->vertexlist = new int[3];
        p->vertexlist[0] = (int)(&in.facetlist[i * 3]);
        p->vertexlist[1] = (int)(&in.facetlist[i * 3 + 1]);
        p->vertexlist[2] = (int)(&in.facetlist[i * 3 + 2]);
    }
#endif
    

    ///////////////////////////////////////////////////////////////////

    printf("tetrahedralizeStart!\n");
    tetrahedralize(&b, &in, &out);
    printf("tetrahedralizeEnd!\n");

    ////////////////////////////////////////////////////////////////////


    // push back to BABYLON usable polyhedra data
	
    *noVerticesOut = out.numberofpoints;
    memcpy(noVerticesOut, out.pointlist, sizeof(REAL) * out.numberofpoints * 3);

    uint64_t i;
    *noTetrahedraOut = out.numberoftetrahedra;

    for (i = 0; i < out.numberoftetrahedra; i++)
    {
        tetrahedraOut[i * 4] = out.tetrahedronlist[i * 4];
        tetrahedraOut[i * 4 + 1] = out.tetrahedronlist[i * 4 + 1];
        tetrahedraOut[i * 4 + 2] = out.tetrahedronlist[i * 4 + 2];
        tetrahedraOut[i * 4 + 3] = out.tetrahedronlist[i * 4 + 3];
    }

    return 0;

}


int main() {
  printf("main!\n");
  return 0;
}
