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
#include <config.h>
#include <g3dengine/g3dengine.h>


/******************************************************************************/
void g3dmeshpose_name ( G3DMESHPOSE *mps, char *name ) {
    if ( mps->name ) free ( mps->name );

    mps->name = strdup ( name );
}

/******************************************************************************/
G3DMESHPOSE *g3dmeshpose_new ( char *name, uint32_t id ) {
    G3DMESHPOSE *mps = ( G3DMESHPOSE * ) calloc ( 0x01, 
                                                  sizeof ( G3DMESHPOSE ) );

    if ( mps == NULL ) {
        fprintf ( stderr, "g3dmeshpose_new: calloc failed\n" );

        return NULL;
    }

    g3dmeshpose_name ( mps, name );

    mps->id   = id;

    return mps;
}

