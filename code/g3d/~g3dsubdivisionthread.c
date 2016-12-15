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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/
#include <config.h>
#include <g3d.h>

/******************************************************************************/
void g3dsubdivisionthread_free ( G3DSUBDIVISIONTHREAD *sdt ) {
    free ( sdt );
}

/******************************************************************************/
void g3dsubdivisionthread_init ( G3DSUBDIVISIONTHREAD *std,
                                 G3DMESH              *mes,
                                 G3DRTVERTEX          *rtvermem,
                                 uint32_t              nbrtver,
                                 G3DRTEDGE            *rtedgmem,
                                 uint32_t              nbrtedg,
                                 G3DRTQUAD            *rtquamem,
                                 uint32_t              nbrtfac,
                                 uint32_t              nbVerticesPerTriangle,
                                 uint32_t              nbVerticesPerQuad,
                                 uint32_t              nbEdgesPerTriangle,
                                 uint32_t              nbEdgesPerQuad,
                                 uint32_t              nbFacesPerTriangle,
                                 uint32_t              nbFacesPerQuad,
                                 uint32_t              cpuID,
                                 uint32_t              subdiv_level,
                                 uint32_t              engine_flags ) {
    std->mes                   = mes;
    std->rtvermem              = rtvermem;
    std->nbrtver               = nbrtver;
    std->rtedgmem              = rtedgmem;
    std->nbrtedg               = nbrtedg;
    std->rtquamem              = rtquamem;
    std->nbrtfac               = nbrtfac;
    std->nbVerticesPerTriangle = nbVerticesPerTriangle;
    std->nbVerticesPerQuad     = nbVerticesPerQuad;
    std->nbEdgesPerTriangle    = nbEdgesPerTriangle;
    std->nbEdgesPerQuad        = nbEdgesPerQuad;
    std->nbFacesPerTriangle    = nbFacesPerTriangle;
    std->nbFacesPerQuad        = nbFacesPerQuad;
    std->cpuID                 = cpuID;
    std->subdiv_level          = subdiv_level;
    std->engine_flags          = engine_flags;

    /*** this is here so that we dont have to use mutexes if it were in ***/
    /*** g3dsubdivisionV3_subdivide. ***/
    std->qua_indexes  = g3dsubindex_get ( 0x04, subdiv_level );
    std->tri_indexes  = g3dsubindex_get ( 0x03, subdiv_level );
}


/******************************************************************************/
G3DSUBDIVISIONTHREAD *g3dsubdivisionthread_new ( G3DMESH     *mes,
                                                 G3DRTVERTEX *rtvermem,
                                                 uint32_t     nbrtver,
                                                 G3DRTEDGE   *rtedgmem,
                                                 uint32_t     nbrtedg,
                                                 G3DRTQUAD   *rtquamem,
                                                 uint32_t     nbrtfac,
                                                 uint32_t     nbVerticesPerTriangle,
                                                 uint32_t     nbVerticesPerQuad,
                                                 uint32_t     nbEdgesPerTriangle,
                                                 uint32_t     nbEdgesPerQuad,
                                                 uint32_t     nbFacesPerTriangle,
                                                 uint32_t     nbFacesPerQuad,
                                                 uint32_t     cpuID,
                                                 uint32_t     subdiv_level,
                                                 uint32_t     engine_flags ) {
    uint32_t structSize = sizeof ( G3DSUBDIVISIONTHREAD );
    G3DSUBDIVISIONTHREAD *std = (G3DSUBDIVISIONTHREAD *) calloc ( 0x01,
                                                                  structSize );

    if ( std == NULL ) {
        fprintf ( stderr, "g3dsubdivisionthread_new: calloc failed\n" );

        return NULL;
    }

    g3dsubdivisionthread_init ( std, mes,
                                     rtvermem,
                                     nbrtver,
                                     rtedgmem,
                                     nbrtedg,
                                     rtquamem,
                                     nbrtfac,
                                     nbVerticesPerTriangle,
                                     nbVerticesPerQuad,
                                     nbEdgesPerTriangle,
                                     nbEdgesPerQuad,
                                     nbFacesPerTriangle,
                                     nbFacesPerQuad,
                                     cpuID,
                                     subdiv_level,
                                     engine_flags );

    return std;
}
