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
	b.plc = 1;
    b.psc = 0;
    b.refine = 0;
    b.quality = 1;
    b.nobisect = 1;
    b.coarsen = 0;
    b.metric = 0;
    b.weighted = 0;
    b.brio_hilbert = 1;
    b.incrflip = 0;
    b.flipinsert = 0;
    b.varvolume = 0;
    b.fixedvolume = 0;
    b.noexact = 0;
    b.nostaticfilter = 0;
    b.insertaddpoints = 0;
    b.regionattrib = 0;
    b.cdtrefine = 0;
    b.diagnose = 1;
    b.convex = 0;
    b.zeroindex = 0;
    b.facesout = 0;
    b.edgesout = 0;
    b.neighout = 0;
    b.voroout = 0;
    b.meditview = 0;
    b.vtkview = 0;
    b.nobound = 0;
    b.noiterationnum = 0;
    b.nomergefacet = 0;
    b.nomergevertex = 0;
    b.nojettison = 0;
    b.docheck = 0;
    b.quiet = 0;
    b.verbose = 0;
    b.nobisect_nomerge = 1;
    b.supsteiner_level = 2;
    b.addsteiner_algo = 1;
    b.coarsen_param = 0;
    b.weighted_param = 0;
    b.reflevel = 3;
    b.optscheme = 7;
    b.optlevel = 2;
    b.delmaxfliplevel = 1;
    b.order = 1;
    b.reversetetori = 0;
    b.steinerleft = 10000;
    b.no_sort = 0;
    b.hilbert_order = 52;
    b.hilbert_limit = 8;
    b.brio_threshold = 64;
    b.brio_ratio = 0.125;
    b.facet_separate_ang_tol = 179.9;
    b.facet_overlap_ang_tol = 0.001;
    b.facet_small_ang_tol = 15.0;
    b.maxvolume = -1.0;
    b.minratio = 1.5;
    b.mindihedral = 20.0;
    b.optmaxdihedral = 165.0;
    b.optminsmtdihed = 179.0;
    b.optminslidihed = 179.0;
    b.epsilon = 1.0e-8;
    b.coarsen_percent = 1.0;
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

    ////////////////////////////////////////////////////////////////////

	

    for(int i=0; i < 3*out.numberofpoints; i++){
  	    verticesOut[i]   = out.pointlist[i];
    }
    *numVerticesOut = out.numberofpoints;

    for(int i=0; i < 4*out.numberoftetrahedra; i++){
  	    tetrahedraOut[i] = out.tetrahedronlist[i];
    }
    *numTetrahedraOut = out.numberoftetrahedra;

    //printf("Debug Outputs\n");
    //printf("pts:%d dim:%d ptatrib:%d ptsml:%d\n", out.numberofpoints, out.mesh_dim, out.numberofpointattributes, out.pointmarkerlist != NULL ? 1 : 0);
    //printf("tets:%d  corners:%d  tetatrib:%d\n", out.numberoftetrahedra, out.numberofcorners, out.numberoftetrahedronattributes);
	//printf("tris:%d trisml:%d\n", out.numberoftrifaces, out.trifacemarkerlist != NULL ? 1 : 0);
    //printf("edges:%d  edgesml:%d\n", out.numberofedges, out.edgemarkerlist != NULL ? 1 : 0);
    //printf("first pts: %.16g  %.16g  %.16g\n", out.pointlist[0], out.pointlist[1], out.pointlist[2]);
    //printf("first tet: todo \n");

    printf("tetrahedralizeEnd!\n");

    return 0;

}


int main() {
  printf("main!\n");
  return 0;
}
