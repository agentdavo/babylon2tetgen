#include <stdlib.h>
#include <stdio.h>
#include <cstdint>
#include <math.h>

#include "tetgen/tetgen.h"
#include <emscripten.h>


int EMSCRIPTEN_KEEPALIVE babylon2tetgen(
    uint32_t  bnumpositions,                     //
    double    bpositions[],                      //
    uint32_t  bnumindices,                       // 
    uint32_t  bindices[],                        //
    uint32_t* nhpos,                             //
    double    hpos[],                            //
    uint64_t* ntet,                              //
    uint32_t  tets[])                            //
{

    printf("startBabylon2Tetgen!\n");
	
    tetgenio in, out;

    // set tetgen options
    tetgenbehavior b;
    b.plc = 1;
    b.quality = 1;
    // b.minratio = 1.2;
    // b.fixedvolume = 1;
    // b.maxvolume = 1.0;

    // All indices start from 0
    in.firstnumber = 0;
    in.mesh_dim = 3;

    // initialize in with BABYLON counts
    in.numberofpoints = bnumpositions;
    in.pointlist = new REAL[bnumpositions * 3];
    in.numberoffacets = bnumindices;
    in.facetlist = new tetgenio::facet[in.numberoffacets];

    // copy BABYLON pointlist to in
    memcpy(in.pointlist, bpositions, sizeof(REAL) * bnumpositions);

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


    ///////////////////////////////////////////////////////////////////

    printf("tetrahedralizeStart!\n");
    tetrahedralize(&b, &in, &out);
    printf("tetrahedralizeEnd!\n");

    ////////////////////////////////////////////////////////////////////


    // push back to BABYLON usable polyhedra data

    *hpos = out.numberofpoints;
    memcpy(hpos, out.pointlist, sizeof(REAL) * out.numberofpoints * 3);

    uint64_t i;
    *ntet = out.numberoftetrahedra;

    for (i = 0; i < out.numberoftetrahedra; i++)
    {
        tets[i * 4] = out.tetrahedronlist[i * 4];
        tets[i * 4 + 1] = out.tetrahedronlist[i * 4 + 1];
        tets[i * 4 + 2] = out.tetrahedronlist[i * 4 + 2];
        tets[i * 4 + 3] = out.tetrahedronlist[i * 4 + 3];
    }

    return 0;

}


int main() {
  return 0;
}