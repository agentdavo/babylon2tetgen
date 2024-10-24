#include <stdlib.h>
#include <stdio.h>
#include <cstdint>
#include <math.h>
#include <iostream>
#include <fstream>
#include <iomanip>

#include "tetgen/tetgen.h"
#include <emscripten.h>

extern "C" {

// Ensure the function is exported and kept alive for Emscripten
EMSCRIPTEN_KEEPALIVE
int babylon2tetgen(
    uint32_t  bnumpositions,     // Number of points from BabylonJS
    double    bpositions[],      // Array of coordinates (x, y, z) [size: bnumpositions * 3]
    uint32_t  bnumindices,       // Number of indices (should be multiple of 3 for facets)
    uint32_t  bindices[],        // Array of indices [size: bnumindices]
    uint32_t* numVerticesOut,    // Output: number of vertices
    double    verticesOut[],     // Output: array of vertex coordinates [size: out.numberofpoints * 3]
    uint64_t* numTetrahedraOut,  // Output: number of tetrahedra
    uint32_t  tetrahedraOut[]    // Output: array of tetrahedra indices [size: out.numberoftetrahedra * 4]
)
{
    // Debug inputs to ensure data is correctly received from BabylonJS
    printf("startBabylon2Tetgen!\n");
    printf("Number of points: %u\n", bnumpositions);
    printf("Number of indices: %u\n", bnumindices);

    // Initialize TetGen input and output structures
    tetgenio in, out;

    //////////////////////////////
    // TetGen options setup
    //////////////////////////////
    tetgenbehavior b;
    b.plc = 1;                 // Use PLC (Piecewise Linear Complex) input
    b.quality = 1;             // Enable quality mesh generation
    b.minratio = 1.1;          // Minimum ratio constraint
    b.mindihedral = 10.0;      // Minimum dihedral angle in degrees
    b.verbose = 2;             // Verbosity level
    b.nobisect = 1;            // Disable bisection
    b.steinerleft = 100000;    // Maximum number of Steiner points
    b.order = 1;               // Order of the mesh (1 for linear)
    b.maxvolume = 0.1;         // Maximum volume constraint for tetrahedra
    b.object = tetgenbehavior::POLY; // Input is a polyhedron

    //////////////////////////////
    // Preparing TetGen input
    //////////////////////////////

    // Set the number of points
    in.numberofpoints = bnumpositions;
    
    // Allocate memory for point coordinates (x, y, z)
    in.pointlist = new REAL[in.numberofpoints * 3];
    
    // Copy the input positions to TetGen's point list
    // Assuming bpositions has 3 * bnumpositions elements
    memcpy(in.pointlist, bpositions, sizeof(REAL) * in.numberofpoints * 3);

    // Set the number of facets (each facet is a triangle, hence indices are in multiples of 3)
    in.numberoffacets = bnumindices / 3;
    
    // Allocate memory for facets
    in.facetlist = new tetgenio::facet[in.numberoffacets];
    in.facetmarkerlist = new int[in.numberoffacets];

    // Populate the facets
    for(uint32_t i = 0; i < in.numberoffacets; i++) {
        tetgenio::facet* f = &in.facetlist[i];
        f->holelist = nullptr;
        f->numberofholes = 0;
        f->numberofpolygons = 1;
        f->polygonlist = new tetgenio::polygon[1];
        
        tetgenio::polygon *p = &f->polygonlist[0];
        p->numberofvertices = 3;
        p->vertexlist = new int[3];
        p->vertexlist[0] = static_cast<int>(bindices[i * 3 + 0]);
        p->vertexlist[1] = static_cast<int>(bindices[i * 3 + 1]);
        p->vertexlist[2] = static_cast<int>(bindices[i * 3 + 2]);
        
        // Optionally, set facet markers if needed
        in.facetmarkerlist[i] = 0; // Example marker
    }

    // Optional: Save input for debugging
    // in.save_nodes("debug_in.node");
    // in.save_poly("debug_in.poly");

    //////////////////////////////
    // TetGen Tetrahedralization
    //////////////////////////////
    printf("Starting tetrahedralization...\n");
    tetrahedralize(&b, &in, &out);
    printf("Tetrahedralization completed.\n");

    //////////////////////////////
    // Preparing Outputs
    //////////////////////////////

    // Copy the generated vertices to the output array
    // Ensure that verticesOut has enough space: out.numberofpoints * 3
    for(uint64_t i = 0; i < (uint64_t)(3) * out.numberofpoints; i++) {
        verticesOut[i] = out.pointlist[i];
    }
    *numVerticesOut = out.numberofpoints;

    // Copy the generated tetrahedra to the output array
    // Ensure that tetrahedraOut has enough space: out.numberoftetrahedra * 4
    for(uint64_t i = 0; i < (uint64_t)(4) * out.numberoftetrahedra; i++) {
        tetrahedraOut[i] = out.tetrahedronlist[i];
    }
    *numTetrahedraOut = out.numberoftetrahedra;

    //////////////////////////////
    // Cleaning Up
    //////////////////////////////

    // Free allocated memory for input
    delete[] in.pointlist;
    delete[] in.facetmarkerlist;
    for(uint32_t i = 0; i < in.numberoffacets; i++) {
        tetgenio::facet* f = &in.facetlist[i];
        for(int j = 0; j < f->numberofpolygons; j++) {
            delete[] f->polygonlist[j].vertexlist;
        }
        delete[] f->polygonlist;
    }
    delete[] in.facetlist;

    // Optional: Free output memory if not needed
    // out.deinitialize();

    printf("Clean up completed.\n");

    return 0;
}

}

// Optional: Keep the main function for debugging purposes
EMSCRIPTEN_KEEPALIVE
int main() {
    printf("TetGen WebGPU Module Initialized!\n");
    return 0;
}
