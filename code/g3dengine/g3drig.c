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
void g3drig_free ( G3DRIG *rig ) {
    list_free ( &rig->lweightgroup, NULL );

    free ( rig );
}

/******************************************************************************/
G3DRIG *g3drig_new ( G3DSKIN *skn ) {
    uint32_t structsize = sizeof ( G3DRIG );
    G3DRIG *rig;

    if ( ( rig = ( G3DRIG * ) calloc ( 0x01, structsize ) ) == NULL ) {
        fprintf ( stderr, "g3drig_new: calloc failed\n" );

        return NULL;
    }

    rig->skn = skn;

    g3dcore_identityMatrix ( rig->isknmatrix );

    return rig;
}
