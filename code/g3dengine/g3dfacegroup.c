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
void g3dfacegroup_free ( G3DFACEGROUP *facgrp ) {
    free ( facgrp );
}

/******************************************************************************/
void g3dfacegroup_unsetSelected ( G3DFACEGROUP *facgrp ) {
    facgrp->flags &= (~FACEGROUPSELECTED);
}

/******************************************************************************/
void g3dfacegroup_addTextureSlot ( G3DFACEGROUP *facgrp,
                                   uint32_t      slotBit ) {
    facgrp->textureSlots |= slotBit;
}

/******************************************************************************/
void g3dfacegroup_removeTextureSlot ( G3DFACEGROUP *facgrp, 
                                      uint32_t      slotBit ) {
    facgrp->textureSlots &= (~slotBit);
}

/******************************************************************************/
G3DFACEGROUP *g3dfacegroup_new ( const char *name, LIST *lfac ) {
    uint32_t structSize = sizeof ( G3DFACEGROUP );
    G3DFACEGROUP *facgrp = ( G3DFACEGROUP * ) calloc ( 0x01, structSize );
    LIST *ltmpfac = lfac;

    if ( facgrp == NULL ) {
        fprintf ( stderr, "g3dtexture_new(): calloc failed\n" );

        return NULL;
    }

    facgrp->name = strdup ( name );

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        g3dface_addFacegroup ( fac, facgrp );

        ltmpfac = ltmpfac->next;
    }
 
    return facgrp;
}
