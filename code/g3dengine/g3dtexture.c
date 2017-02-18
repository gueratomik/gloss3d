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
#include <g3dengine/g3dengine.h>

/******************************************************************************/
G3DTEXTURE *g3dtexture_getFromUVMap ( LIST *ltex, G3DUVMAP *map ) {
    LIST *ltmptex = ltex;

    while ( ltmptex ) {
        G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmptex->data;

        if ( tex->map == map ) return tex;

        ltmptex = ltmptex->next;
    }

    return NULL;
}

/******************************************************************************/
void g3dtexture_unsetSelected ( G3DTEXTURE *tex ) {
    tex->flags &= (~TEXTURESELECTED);
}

/******************************************************************************/
G3DTEXTURE *g3dtexture_new ( G3DMATERIAL *mat, G3DUVMAP *map,
                                               G3DFACEGROUP *facgrp ) {
    G3DTEXTURE *tex = ( G3DTEXTURE * ) calloc ( 0x01, sizeof ( G3DTEXTURE ) );

    if ( tex == NULL ) {
        fprintf ( stderr, "g3dtexture_new(): calloc failed\n" );

        return NULL;
    }

    /*** By default, texture displacement channel affects subdivisions ***/
    tex->flags  = TEXTUREDISPLACE;

    tex->mat    = mat;
    tex->map    = map;
    tex->facgrp = facgrp;


    return tex;
}
