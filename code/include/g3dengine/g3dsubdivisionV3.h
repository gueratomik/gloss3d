/******************************************************************************/
/*  GLOSS3D is free software: you can redistribute it and/or modify           */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation, either version 3 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  GLOSS3D is distributed in the hope that it will be useful,                */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU General Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU General Public License         */
/*  along with GLOSS3D.  If not, see http://www.gnu.org/licenses/.            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/

/**
 * @file
 */

/******************************************************************************/
#ifndef _G3DSUBDIVISION_H_
#define _G3DSUBDIVISION_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct G3DSUBDIVISION
 * @brief A structure to store temporary array needed by the Catmull-Clark
 * subdivision algorithm.
 */
typedef struct _G3DSUBDIVISION {
    G3DSUBFACE    *innerFaces; 
    G3DSUBFACE    *outerFaces; 
    G3DSUBEDGE    *innerEdges; 
    G3DSUBEDGE    *outerEdges; 
    G3DSUBVERTEX  *innerVertices; 
    G3DSUBVERTEX  *outerVertices; 
    G3DSUBUVSET   *innerUVSets; 
    G3DSUBUVSET   *outerUVSets; 
    G3DLOOKUP      vertexLookup;
    G3DLOOKUP      edgeLookup;
    G3DLOOKUP      faceLookup;
    uint32_t       nbEdgeLookup;
    uint32_t       nbVertexLookup;
    uint32_t       nbInnerFaces;
    uint32_t       nbOuterFaces;
    uint32_t       nbInnerEdges;
    uint32_t       nbOuterEdges;
    uint32_t       nbInnerVertices;
    uint32_t       nbOuterVertices;
    uint32_t       nbInnerUVSets;
    uint32_t       nbOuterUVSets;
} G3DSUBDIVISION;

/******************************************************************************/

/**
 * Free a previsously allocated G3DSUBDIVISION structure.
 * @param a pointer to the G3DSUBDIVISION structure that must be freed.
 */
void g3dsubdivisionV3_free ( G3DSUBDIVISION *sdv );

/**
 * Create a new G3DSUBDIVISION structure.
 */
G3DSUBDIVISION *g3dsubdivisionV3_new ( );

/**
 * Convert the subdivision of a G3DFACE (fac).
 * if the subdivision flag SUBDIVISIONCOMMIT is used, commitVertices, 
 * commitEdges and commitFaces are arrays that must be allocated
 * prior to calling this function, and must be large enough to contain all the
 * vertices edges and faces of the final mesh. It is possible to compute the 
 * needed array size big enough using g3dtriangle_evalSubdivision() and 
 * g3dquad_evalSubdivision() and multiply them by the number of triangles/quads.
 * Otherwise, the face is converted to RTFACE, simple arrays that can be
 * displayed with OpenGL vertex array features. In that case, if set,
 * rtTriangles, rtEdges, rtQuads, rtVertices, rtUvs are filled with the
 * corresponding values. These arrays must be allocated by the caller and 
 * must be big enough to receive the data for a single subdivided face (not
 * for the whole mesh, unlike in SUBDIVISIONCOMMIT mode).
 *
 * @param a pointer to a G3DSUBDIVISION structure holding temporary arrays
 * that. One can get it with g3dsysinfo_getSubdivision().
 * @param a pointer to the G3DMESH owning the face.
 * @param a pointer to the G3DFACE to subdivide.
 * @param output G3DRTTRIANGLE array. Filled only if subdivision level is 0.
 * @param output G3DRTQUAD array. Indexes vertices by Id.
 * @param output G3DRTEDGE array. Indexes vertices by Id.
 * @param output G3DRTVERTEX array.
 * @param output G3DRTUVS array.
 * @param output G3DVERTEX array.
 * @param output G3DEDGE array.
 * @param output G3DFACE array.
 * @param list of textures.
 * @param qua_indexes is an internal object needed for sculpt mode.
 * One can get it using g3dsubindex_get(4, subdiv_level).
 * @param tri_indexes is an internal object needed for sculpt mode.
 * One can get it using g3dsubindex_get(3, subdiv_level).
 * @param subdivision level.
 * @param subdivision flags.
 * @param 3D engine flags.
 * @return the number of faces created by the subdivision process.
 */
uint32_t g3dsubdivisionV3_subdivide ( G3DSUBDIVISION *sdv,
                                      G3DMESH        *mes,
                                      G3DVECTOR      *stkpos,
                                      G3DFACE        *fac,
                                      G3DRTTRIANGLE  *rtTriangles,
                                      G3DRTQUAD      *rtQuads,
                                      G3DRTEDGE      *rtEdges,
                                      G3DRTVERTEX    *rtVertices,
                                      G3DRTUV        *rtUVs,
                                      G3DVERTEX     **commitVertices,
                                      G3DEDGE       **commitEdges,
                                      G3DFACE       **commitFaces,
                                      LIST           *ltex,
                                      uint32_t      (*qua_indexes)[0x04], /*** for sculpt mode ***/
                                      uint32_t      (*tri_indexes)[0x04], /*** for sculpt mode ***/
                                      uint32_t        subdiv_level,
                                      uint32_t        subdiv_flags,
                                      uint64_t        engine_flags );

#ifdef __cplusplus
}
#endif

#endif
