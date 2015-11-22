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
G3DSUBDIVISIONTHREAD *g3dsubdivisionthread_new ( G3DMESH *mes, 
                                                 uint32_t cpuID,
                                                 uint32_t engine_flags ) {
    G3DSUBDIVISIONTHREAD *std = ( G3DSUBDIVISIONTHREAD * ) calloc ( 0x01, sizeof ( G3DSUBDIVISIONTHREAD ) );

    if ( std == NULL ) {

        return NULL;
    }

    std->mes          = mes;
    std->cpuID        = cpuID;
    std->engine_flags = engine_flags;

    /*** this is here so that we dont have to use mutexes if it were in ***/
    /*** g3dsubdivisionV3_subdivide. ***/
    std->qua_indexes  = g3dsubindex_get ( 0x04, mes->subdiv );
    std->tri_indexes  = g3dsubindex_get ( 0x03, mes->subdiv );

    return std;
}
