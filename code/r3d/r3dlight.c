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
#include <r3d.h>

/******************************************************************************/
void r3dlight_free ( R3DOBJECT *rob ) {
    R3DLIGHT *rlt = ( R3DLIGHT * ) rob;
}

/******************************************************************************/
R3DLIGHT *r3dlight_new ( G3DLIGHT *lig, uint32_t id ) {
    int structsize = sizeof ( R3DLIGHT );
    R3DLIGHT *rlt = ( R3DLIGHT * ) calloc ( 0x01, structsize );
    R3DOBJECT *rob = ( R3DOBJECT * ) rlt;
    G3DOBJECT *obj = ( G3DOBJECT * ) lig;
    G3DVECTOR pos;

    if ( rlt == NULL ) {
        fprintf ( stderr, "r3dlight_new: memory allocation failed\n" );

        return NULL;
    }

    rob->obj = obj;
    rlt->lig = lig;

    g3dvector_init ( &pos, 0.0f, 0.0f, 0.0f, 1.0f );
    g3dvector_matrix ( &pos, ((G3DOBJECT*)lig)->wmatrix, &rlt->pos );

    r3dobject_init ( rob, id, obj->type, obj->flags, r3dlight_free );


    return rlt;
}
