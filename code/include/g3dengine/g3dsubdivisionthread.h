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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2013         */
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
    G3DRTVERTEX *rtvermem;
    uint32_t     nbrtver;
    G3DRTEDGE   *rtedgmem;
    uint32_t     nbrtedg;
    G3DRTQUAD   *rtquamem;
    uint32_t     nbrtfac;
    G3DRTUV     *rtuvmem;
    uint32_t     nbrtuv;
    uint32_t     nbVerticesPerTriangle;
    uint32_t     nbVerticesPerQuad;
    uint32_t     nbEdgesPerTriangle;
    uint32_t     nbEdgesPerQuad;
    uint32_t     nbFacesPerTriangle;
    uint32_t     nbFacesPerQuad;
    uint32_t     cpuID;
    uint32_t     subdiv_level;
    uint32_t     engine_flags;
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
 * Init a new G3DSUBDIVISIONTHREAD structure.
 */
void g3dsubdivisionthread_init ( G3DSUBDIVISIONTHREAD *,
                                 G3DMESH              *,
                                 G3DRTVERTEX          *,
                                 uint32_t              ,
                                 G3DRTEDGE            *,
                                 uint32_t              ,
                                 G3DRTQUAD            *,
                                 uint32_t              ,
                                 G3DRTUV              *,
                                 uint32_t              ,
                                 uint32_t              ,
                                 uint32_t              ,
                                 uint32_t              ,
                                 uint32_t              ,
                                 uint32_t              ,
                                 uint32_t              ,
                                 uint32_t              ,
                                 uint32_t,
                                 uint32_t               );

/**
 * Create a new G3DSUBDIVISIONTHREAD structure.
 */
G3DSUBDIVISIONTHREAD *g3dsubdivisionthread_new ( G3DMESH     *mes,
                                                 G3DRTVERTEX *rtvermem,
                                                 uint32_t     nbrtver,
                                                 G3DRTEDGE   *rtedgmem,
                                                 uint32_t     nbrtedg,
                                                 G3DRTQUAD   *rtquamem,
                                                 uint32_t     nbrtfac,
                                                 G3DRTUV     *rtuvmem,
                                                 uint32_t     nbrtuv,
                                                 uint32_t     nbVerticesPerTriangle,
                                                 uint32_t     nbVerticesPerQuad,
                                                 uint32_t     nbEdgesPerTriangle,
                                                 uint32_t     nbEdgesPerQuad,
                                                 uint32_t     nbFacesPerTriangle,
                                                 uint32_t     nbFacesPerQuad,
                                                 uint32_t     cpuID,
                                                 uint32_t     subdiv_level,
                                                 uint32_t     engine_flags );

#ifdef __cplusplus
}
#endif

#endif
