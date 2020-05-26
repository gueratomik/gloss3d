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
void g3dtexture_restrict ( G3DTEXTURE *tex ) {
    tex->flags |= TEXTURERESTRICTED;
}

/******************************************************************************/
void g3dtexture_unrestrict ( G3DTEXTURE *tex ) {
    tex->flags &= (~TEXTURERESTRICTED);
}

/******************************************************************************/
void g3dtexture_restrictFacegroup ( G3DTEXTURE *tex, G3DFACEGROUP *facgrp ) {
    g3dfacegroup_addTextureSlot ( facgrp, tex->slotBit );

    /*** Actually remembering the facegroup pointer is needed when it comes ***/
    /*** to write the data file (.g3d) ***/
    list_insert ( &tex->lfacgrp, facgrp );

    tex->nbfacgrp++;
}

/******************************************************************************/
void g3dtexture_unrestrictFacegroup ( G3DTEXTURE *tex, G3DFACEGROUP *facgrp ) {
    g3dfacegroup_removeTextureSlot ( facgrp, tex->slotBit );

    /*** Actually remembering the facegroup pointer is needed when it comes ***/
    /*** to write the data file (.g3d) ***/
    list_remove ( &tex->lfacgrp, facgrp );

    tex->nbfacgrp--;
}

/******************************************************************************/
void g3dtexture_unrestrictAllFacegroups ( G3DTEXTURE *tex ) {
    LIST *ltmpfacgrp = tex->lfacgrp;

    while ( ltmpfacgrp ) {
        G3DFACEGROUP *facgrp = ( G3DFACEGROUP * ) ltmpfacgrp->data;
        LIST *ltmpfacgrpnext = ltmpfacgrp->next;

        g3dtexture_unrestrictFacegroup ( tex, facgrp );

        ltmpfacgrp = ltmpfacgrpnext;
    }
}

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
void g3dtexture_free ( G3DTEXTURE *tex ) {
    list_free ( &tex->lfacgrp, NULL );

    free ( tex );
}

/******************************************************************************/
G3DTEXTURE *g3dtexture_new ( G3DOBJECT   *obj, 
                             G3DMATERIAL *mat, 
                             G3DUVMAP    *map ) {
    G3DTEXTURE *tex = ( G3DTEXTURE * ) calloc ( 0x01, sizeof ( G3DTEXTURE ) );

    if ( tex == NULL ) {
        fprintf ( stderr, "g3dtexture_new(): calloc failed\n" );

        return NULL;
    }

    /*** By default, texture displacement channel affects subdivisions ***/
    tex->flags  = TEXTUREDISPLACE | TEXTUREREPEATED;

    tex->mat    = mat;
    tex->map    = map;
    tex->obj    = obj;

    return tex;
}
