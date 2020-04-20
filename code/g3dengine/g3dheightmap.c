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
void g3dheightmap_print ( G3DHEIGHTMAP *htm ) {
    uint32_t i;

    printf ( "Heigthmap\n" );
    for ( i = 0x00; i < htm->maxheights; i++ ) {
        printf ( "%f\n", htm->heights[i].elevation );
    }
    printf ( "\n" );
}

/******************************************************************************/
void g3dheightmap_realloc ( G3DHEIGHTMAP *htm, uint32_t nbheights ) {
    if ( nbheights > htm->maxheights ) {
        uint32_t structsize = sizeof ( G3DHEIGHT );
        htm->heights = ( G3DHEIGHT * ) realloc ( htm->heights, nbheights * structsize );

        if ( htm->heights == NULL ) {
            fprintf ( stderr, "g3dheightmap_realloc: realloc failed\n" );

            return;
        }

        memset ( htm->heights + htm->maxheights, 0x00, (nbheights - htm->maxheights) * sizeof ( G3DHEIGHT ) );

        htm->maxheights = nbheights;
    }
}

/******************************************************************************/
G3DHEIGHTMAP *g3dheightmap_new ( uint32_t nbheights ) {
    G3DHEIGHTMAP *htm = ( G3DHEIGHTMAP * ) calloc ( 0x01, sizeof ( G3DHEIGHTMAP ) );

    if ( htm == NULL ) {
        fprintf ( stderr, "g3dheightmap_new: calloc failed\n" );

        return NULL;
    }

    g3dheightmap_realloc ( htm, nbheights );


    return htm;
}
