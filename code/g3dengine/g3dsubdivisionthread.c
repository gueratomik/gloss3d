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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2020         */
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
#include <g3dengine/g3dengine.h>

/******************************************************************************/
void g3dsubdivisionthread_free ( G3DSUBDIVISIONTHREAD *sdt ) {
    free ( sdt );
}

/******************************************************************************/
void g3dsubdivisionthread_init ( G3DSUBDIVISIONTHREAD *std,
                                 G3DMESH              *mes,
                                 G3DVECTOR3F            *stkpos,
                                 G3DVECTOR3F            *stknor,
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
                                 uint64_t              engine_flags ) {
    std->mes                   = mes;
    std->stkpos                = stkpos;
    std->stknor                = stknor;
    std->rtvermem              = rtvermem;
    std->nbrtver               = nbrtver;
    std->rtedgmem              = rtedgmem;
    std->nbrtedg               = nbrtedg;
    std->rtquamem              = rtquamem;
    std->nbrtfac               = nbrtfac;
    std->rtluim                = rtluim;
    std->nbrtuv                = nbrtuv;
    std->nbVerticesPerTriangle = nbVerticesPerTriangle;
    std->nbVerticesPerQuad     = nbVerticesPerQuad;
    std->nbEdgesPerTriangle    = nbEdgesPerTriangle;
    std->nbEdgesPerQuad        = nbEdgesPerQuad;
    std->nbFacesPerTriangle    = nbFacesPerTriangle;
    std->nbFacesPerQuad        = nbFacesPerQuad;
    std->cpuID                 = cpuID;
    std->sculptExtensionName   = sculptExtensionName;
    std->sculptMode            = sculptMode;
    std->subdiv_level          = subdiv_level;
    std->engine_flags          = engine_flags;

    /*** this is here so that we dont have to use mutexes if it were in ***/
    /*** g3dsubdivisionV3_subdivide. ***/
    std->qua_indexes  = g3dsubindex_get ( 0x04, subdiv_level );
    std->tri_indexes  = g3dsubindex_get ( 0x03, subdiv_level );
}


/******************************************************************************/
G3DSUBDIVISIONTHREAD *g3dsubdivisionthread_new ( G3DMESH     *mes,
                                                 G3DVECTOR3F   *stkpos,
                                                 G3DVECTOR3F   *stknor,
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
                                                 uint64_t     engine_flags ) {
    uint32_t structSize = sizeof ( G3DSUBDIVISIONTHREAD );
    G3DSUBDIVISIONTHREAD *std = (G3DSUBDIVISIONTHREAD *) calloc ( 0x01,
                                                                  structSize );

    if ( std == NULL ) {
        fprintf ( stderr, "g3dsubdivisionthread_new: calloc failed\n" );

        return NULL;
    }

    g3dsubdivisionthread_init ( std,
                                mes,
                                stkpos,
                                stknor,
                                rtvermem,
                                nbrtver,
                                rtedgmem,
                                nbrtedg,
                                rtquamem,
                                nbrtfac,
                                rtluim,
                                nbrtuv,
                                nbVerticesPerTriangle,
                                nbVerticesPerQuad,
                                nbEdgesPerTriangle,
                                nbEdgesPerQuad,
                                nbFacesPerTriangle,
                                nbFacesPerQuad,
                                cpuID,
                                subdiv_level,
                                sculptExtensionName,
                                sculptMode,
                                engine_flags );

    return std;
}

/******************************************************************************/
void *g3dsubdivisionV3_subdivide_t ( G3DSUBDIVISIONTHREAD *sdt ) {
    G3DSYSINFO *sif = g3dsysinfo_get ( );
    G3DSUBDIVISION *sdv = g3dsysinfo_getSubdivision ( sif, sdt->cpuID );
    G3DMESH *mes = sdt->mes;
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    uint32_t nbuvmap = g3dmesh_getUVMapCount ( mes );
    G3DFACE *fac;

    while ( ( fac = g3dmesh_getNextFace ( sdt->mes, NULL ) ) ) {
        G3DRTUV     *rtluim  = NULL;
        SHADERVERTEX *rtvermem = NULL;
        G3DRTEDGE   *rtedgmem = NULL;
        G3DRTQUAD   *rtquamem = NULL;

        uint32_t nbpos = 0x00;

        if ( fac->nbver == 0x03 ) {
            rtvermem = sdt->rtvermem + ( fac->typeID * sdt->nbVerticesPerTriangle );
            rtquamem = sdt->rtquamem + ( fac->typeID * sdt->nbFacesPerTriangle );
            rtluim   = sdt->rtluim   + ( fac->typeID * sdt->nbVerticesPerTriangle * nbuvmap );
        } else {
            rtvermem = sdt->rtvermem + ( mes->nbtri  * sdt->nbVerticesPerTriangle ) + 
                                       ( fac->typeID * sdt->nbVerticesPerQuad );
            rtquamem = sdt->rtquamem + ( mes->nbtri  * sdt->nbFacesPerTriangle ) + 
                                       ( fac->typeID * sdt->nbFacesPerQuad );
            rtluim   = sdt->rtluim   + ( mes->nbtri  * sdt->nbVerticesPerTriangle * nbuvmap ) +
                                       ( fac->typeID * sdt->nbVerticesPerQuad * nbuvmap );

        }

        fac->rtvermem = rtvermem;
        fac->rtquamem = rtquamem;

        g3dsubdivisionV3_subdivide ( sdv, 
                                     mes,
                                     sdt->stkpos,
                                     sdt->stknor,
                                     fac,
                                     NULL,
                                     rtquamem,
                                     rtedgmem,
                                     rtvermem,
                                     rtluim,
                                     NULL,
                                     NULL,
                                     NULL,
                                     mes->ltex,
                                     sdt->sculptExtensionName,
                                     sdt->sculptMode,
                    (uint32_t (*)[4])sdt->qua_indexes,
                    (uint32_t (*)[4])sdt->tri_indexes,
                                     sdt->subdiv_level,
                                     SUBDIVISIONCOMPUTE,
                                     sdt->engine_flags );

    }

    if ( sdt->engine_flags & G3DMULTITHREADING ) {
        /*** this is needed for memory release ***/
        pthread_exit ( NULL );
    }

    /** COMMENTED OUT - now done by the caller ***/
    /*g3dsubdivisionthread_free ( sdt );*/
    return NULL;
}
