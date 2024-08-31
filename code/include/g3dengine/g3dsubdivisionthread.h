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
#ifndef _G3DSUBDIVISIONTHREAD_H_
#define _G3DSUBDIVISIONTHREAD_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct G3DSUBDIVISIONTHREAD
 * @brief A structure for multithreaded subdivision. Do not work in 
 * SUBDIVISIONCOMMIT mode.
 */
typedef struct _G3DSUBDIVISIONTHREAD {
    G3DMESH     *mes;
    G3DVECTOR   *stkpos;
    G3DVECTOR   *stknor;
    SHADERVERTEX *rtvermem;
    uint32_t     nbrtver;
    G3DRTEDGE   *rtedgmem;
    uint32_t     nbrtedg;
    G3DRTQUAD   *rtquamem;
    uint32_t     nbrtfac;
    G3DRTUV     *rtluim;
    uint32_t     nbrtuv;
    uint32_t     nbVerticesPerTriangle;
    uint32_t     nbVerticesPerQuad;
    uint32_t     nbEdgesPerTriangle;
    uint32_t     nbEdgesPerQuad;
    uint32_t     nbFacesPerTriangle;
    uint32_t     nbFacesPerQuad;
    uint32_t     cpuID;
    uint32_t     subdiv_level;
    uint64_t     sculptExtensionName;
    uint64_t     sculptMode;
    uint64_t     engine_flags;
    uint32_t    *qua_indexes; /*** Quad subindexes - used for sculpting ***/
    uint32_t    *tri_indexes; /*** Triangles subindexes  - used for sculpting ***/
} G3DSUBDIVISIONTHREAD;

/******************************************************************************/
/**
 * Free a previsously allocated G3DSUBDIVISIONTHREAD structure.
 * @param a pointer to the G3DSUBDIVISIONTHREAD structure that must be freed.
 */
void g3dsubdivisionthread_free ( G3DSUBDIVISIONTHREAD * );

/**
 * Init a G3DSUBDIVISIONTHREAD structure.
 * @param std a pointer to the G3DSUBDIVISIONTHREAD structure.
 * @param mes a pointer to the G3DMESH.
 * @param rtvermem SHADERVERTEX array to fill.
 * @param nbrtver number of vertices in the array.
 * @param rtedgmem G3DRTEDGE array to fill. Indexes vertices by Id.
 * @param nbrtedg number of edges in the array.
 * @param rtquamem G3DRTQUAD array to fill. Indexes vertices by Id.
 * @param nbrtfac number of quads in the array.
 * @param rtluim G3DRTUV array to fill.
 * @param nbrtuv number of uvs in the array.
 * @param nbVerticesPerTriangle number of vertices per triangle.
 * @param nbVerticesPerQuad number of vertices per quad.
 * @param nbEdgesPerTriangle number of edges per triangle.
 * @param nbEdgesPerQuad number of edges per quad.
 * @param nbFacesPerTriangle number of faces per triangle.
 * @param nbFacesPerQuad number of faces per quad.
 * @param cpuID ID of the CPU where to run the thread.
 * @param subdiv_level subdivision level.
 * @param engine_flags 3D engine flags.
 */
void g3dsubdivisionthread_init ( G3DSUBDIVISIONTHREAD *std,
                                 G3DMESH              *mes,
                                 G3DVECTOR            *stkpos,
                                 G3DVECTOR            *stknor,
                                 SHADERVERTEX          *rtvermem,
                                 uint32_t              nbrtver,
                                 G3DRTEDGE            *rtedgmem,
                                 uint32_t              nbrtedg,
                                 G3DRTQUAD            *rtquamem,
                                 uint32_t              nbrtfac,
                                 G3DRTUV              *rtluim,
                                 uint32_t              nbrtuv,
                                 uint32_t              nbVerticesPerTriangle,
                                 uint32_t              nbVerticesPerQuad,
                                 uint32_t              nbEdgesPerTriangle,
                                 uint32_t              nbEdgesPerQuad,
                                 uint32_t              nbFacesPerTriangle,
                                 uint32_t              nbFacesPerQuad,
                                 uint32_t              cpuID,
                                 uint32_t              subdiv_level,
                                 uint64_t              sculptExtensionName,
                                 uint32_t              sculptMode,
                                 uint64_t              engine_flags );

/**
 * Create mesa new G3DSUBDIVISIONTHREAD structure.
 * @param mes a pointer to the G3DMESH.
 * @param rtvermem SHADERVERTEX array to fill.
 * @param nbrtver number of vertices in the array.
 * @param rtedgmem G3DRTEDGE array to fill. Indexes vertices by Id.
 * @param nbrtedg number of edges in the array.
 * @param rtquamem G3DRTQUAD array to fill. Indexes vertices by Id.
 * @param nbrtfac number of quads in the array.
 * @param rtluim G3DRTUV array to fill.
 * @param nbrtuv number of uvs in the array.
 * @param nbVerticesPerTriangle number of vertices per triangle.
 * @param nbVerticesPerQuad number of vertices per quad.
 * @param nbEdgesPerTriangle number of edges per triangle.
 * @param nbEdgesPerQuad number of edges per quad.
 * @param nbFacesPerTriangle number of faces per triangle.
 * @param nbFacesPerQuad number of faces per quad.
 * @param cpuID ID of the CPU where to run the thread.
 * @param subdiv_level subdivision level.
 * @param engine_flags 3D engine flags.
 * @return the allocated G3DSUBDIVISIONTHREAD structure.
 */
G3DSUBDIVISIONTHREAD *g3dsubdivisionthread_new ( G3DMESH     *mes,
                                                 G3DVECTOR   *stkpos,
                                                 G3DVECTOR   *stknor,
                                                 SHADERVERTEX *rtvermem,
                                                 uint32_t     nbrtver,
                                                 G3DRTEDGE   *rtedgmem,
                                                 uint32_t     nbrtedg,
                                                 G3DRTQUAD   *rtquamem,
                                                 uint32_t     nbrtfac,
                                                 G3DRTUV     *rtluim,
                                                 uint32_t     nbrtuv,
                                                 uint32_t     nbVerticesPerTriangle,
                                                 uint32_t     nbVerticesPerQuad,
                                                 uint32_t     nbEdgesPerTriangle,
                                                 uint32_t     nbEdgesPerQuad,
                                                 uint32_t     nbFacesPerTriangle,
                                                 uint32_t     nbFacesPerQuad,
                                                 uint32_t     cpuID,
                                                 uint32_t     subdiv_level,
                                                 uint64_t     sculptExtensionName,
                                                 uint32_t     sculptMode,
                                                 uint64_t     engine_flags );

/**
 * Multithreaded version of g3dsubdivisionV3_subdivide()
 * @param std a pointer to the G3DSUBDIVISIONTHREAD structure.
 */
void *g3dsubdivisionV3_subdivide_t ( G3DSUBDIVISIONTHREAD *std );

#ifdef __cplusplus
}
#endif

#endif
