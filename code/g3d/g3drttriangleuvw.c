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
void g3drttriangleuvw_free ( G3DRTTRIANGLEUVW *uvw ) {
    free ( uvw );
}

/******************************************************************************/
G3DRTTRIANGLEUVW *g3drttriangleuvw_new ( float u0, float v0,
                                         float u1, float v1,
                                         float u2, float v2, G3DUVMAP *map ) {
    uint32_t structsize = sizeof ( G3DRTTRIANGLEUVW );
    G3DRTTRIANGLEUVW *uvw = ( G3DRTTRIANGLEUVW * ) calloc ( 0x01, structsize );

    if ( uvw == NULL ) {
        fprintf ( stderr, "g3drttriangleuvw_new: memory allocation failed\n" );

        return NULL;
    }

    uvw->u[0x00] = u0;
    uvw->v[0x00] = v0;
    uvw->u[0x01] = u1;
    uvw->v[0x01] = v1;
    uvw->u[0x02] = u2;
    uvw->v[0x02] = v2;

    uvw->map     = map;


    return uvw;
}
