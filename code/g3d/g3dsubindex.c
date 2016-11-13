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
uint32_t *g3dsubindex_get ( uint32_t nbver, uint32_t subdiv_level ) {
    /*** maximum subdiv level is 0x10. No one will go this deep anyway :) ***/
    static G3DSUBINDEX *subindexes[0x10];

    if ( subdiv_level >= 0x10 ) {
        fprintf ( stderr, "g3dsubindex_get: requested subdiv level too high!\n" );

        return NULL;
    }

    if ( subindexes[subdiv_level] == NULL ) {
        G3DSYSINFO *sif = g3dsysinfo_get ( );
        /*** Get the temporary subdivision arrays for CPU #0 ***/
        G3DSUBDIVISION *sdv = g3dsysinfo_getSubdivision ( sif, 0x00 );
        uint32_t nbfac = pow ( 0x04, subdiv_level );
        G3DVERTEX ver[0x04];
        G3DEDGE   edg[0x04];
        G3DFACE   fac;

        memset ( &ver, 0x00, sizeof ( G3DVERTEX ) * 0x04 );
        memset ( &edg, 0x00, sizeof ( G3DEDGE   ) * 0x04 );
        memset ( &fac, 0x00, sizeof ( G3DFACE   ) );

        subindexes[subdiv_level] = ( G3DSUBINDEX * ) calloc ( 0x01, sizeof ( G3DSUBINDEX ) );

        subindexes[subdiv_level]->qua = ( G3DSUBINDEX * ) calloc ( nbfac, sizeof ( uint32_t ) * 0x04 );
        subindexes[subdiv_level]->tri = ( G3DSUBINDEX * ) calloc ( nbfac, sizeof ( uint32_t ) * 0x04 );

        fac.ver[0x00] = &ver[0x00];
        fac.ver[0x01] = &ver[0x01];
        fac.ver[0x02] = &ver[0x02];
        fac.ver[0x03] = &ver[0x03];
        fac.edg[0x00] = &edg[0x00];
        fac.edg[0x01] = &edg[0x01];
        fac.edg[0x02] = &edg[0x02];
        fac.edg[0x03] = &edg[0x03];

        fac.nbver     = 0x04;

        g3dsubdivisionV3_subdivide ( sdv, &fac, NULL,
                                                NULL,
                                                NULL,
                                                NULL,
                                                subindexes[subdiv_level]->qua,
                                                subindexes[subdiv_level]->tri,
                                                subdiv_level,
                                                0x00,
                                                BUILDSUBINDEX );

        fac.nbver     = 0x03;

        g3dsubdivisionV3_subdivide ( sdv, &fac, NULL,
                                                NULL,
                                                NULL,
                                                NULL,
                                                subindexes[subdiv_level]->qua,
                                                subindexes[subdiv_level]->tri,
                                                subdiv_level,
                                                0x00,
                                                BUILDSUBINDEX );
    }

    if ( nbver == 0x04 ) return subindexes[subdiv_level]->qua;
    if ( nbver == 0x03 ) return subindexes[subdiv_level]->tri;


    return NULL;
}
