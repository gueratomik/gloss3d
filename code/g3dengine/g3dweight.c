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
void g3dweightgroup_name ( G3DWEIGHTGROUP *grp, char *name ) {
    if ( name ) {
        uint32_t len = strlen ( name ) + 0x01;

        grp->name = realloc ( grp->name, len );

        memset ( grp->name, 0x00, len );
        memcpy ( grp->name, name, len );
    }
}

/******************************************************************************/
G3DWEIGHTGROUP *g3dweightgroup_new ( G3DMESH *mes, char *name ) {
    uint32_t structsize = sizeof ( G3DWEIGHTGROUP );
    G3DWEIGHTGROUP *grp = ( G3DWEIGHTGROUP * ) calloc ( 0x01, structsize );


    if ( grp == NULL ) {
        fprintf ( stderr, "g3dweightgroup_new: calloc failed\n" );
    }

    g3dweightgroup_name ( grp, name );

    grp->mes = mes;

    return grp;
}

/******************************************************************************/
void g3dweightgroup_free ( G3DWEIGHTGROUP *grp ) {
    free ( grp );
}

/******************************************************************************/
void g3dweight_free ( G3DWEIGHT *wei ) {
    free ( wei );
}

/******************************************************************************/
G3DWEIGHT *g3dweight_new ( float           weight,
                           G3DWEIGHTGROUP *grp ) {
    G3DWEIGHT *wei = ( G3DWEIGHT * ) calloc ( 0x01, sizeof ( G3DWEIGHT ) );

    if ( wei == NULL ) {
        fprintf ( stderr, "g3dweight_new: calloc failed\n" );
    }

    wei->weight = weight;
    wei->grp = grp;

    return wei;
}

