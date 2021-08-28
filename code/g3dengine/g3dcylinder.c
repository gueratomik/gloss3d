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
#define PRAD 6.28318531f

/******************************************************************************/
void g3dcylinder_build ( G3DPRIMITIVE *pri, int slice,
                                            int capx, int capy,
                                            float radius, float length ) {
    CYLINDERDATASTRUCT *data = ( CYLINDERDATASTRUCT * ) pri->data;
    G3DOBJECT *obj = ( G3DOBJECT * ) pri;
    G3DMESH *mes = ( G3DMESH * ) pri;
    float sliang = ( slice ) ? ( ( float ) PRAD / slice ) : 0.0f,
          sliacc = 0.0f,
          capxstp = ( capx ) ? ( length / capx ) : 0.0f,
          capxacc = 0.0f;
    uint32_t nbver = ( slice * ( capx + 0x01 ) );
    G3DVERTEX **ver = ( G3DVERTEX ** ) calloc ( nbver, sizeof ( G3DVERTEX * ) );
    uint32_t vid = 0x00;
    int i, j;

    data->radius = radius;
    data->length = length;
    data->slice = slice;
    data->capx = capx;
    data->capy = capy;

    g3dmesh_empty ( mes );

    /*** Vertices creation ***/
    for ( i = 0x00; i < ( capx + 0x01 ); i++ ) {
        for ( j = 0x00; j < slice; j++, vid++ ) {
            float cossliacc = cos ( sliacc ),
                  sinsliacc = sin ( sliacc );
            G3DVECTOR nor, pos;

            g3dvector_init ( &nor, ( cossliacc ),
                                   ( sinsliacc ),
                                   ( 0.0f   ), 1.0f );

            g3dvector_init ( &pos, ( nor.x * radius ),
                                   ( nor.y * radius ),
                                   ( capxacc ), 1.0f );

            ver[vid] = g3dvertex_new ( pos.x, pos.y, pos.z );
            g3dvector_init    ( &ver[vid]->nor, nor.x, nor.y, nor.z, 1.0f );
            g3dmesh_addVertex ( mes, ver[vid] );

            sliacc = ( sliacc + sliang );
        }

        capxacc = ( capxacc + capxstp );
    }

    /*** Faces creation ***/
    for ( i = 0x00; i < capx; i++ ) {
        int n = ( i + 0x01 );

        for ( j = 0x00; j < slice; j++ ) {
            int k = ( j + 0x01 ) % slice;
            G3DFACE *fac = g3dquad_new ( ver[(n*slice)+j],
                                         ver[(i*slice)+j],
                                         ver[(i*slice)+k],
                                         ver[(n*slice)+k] );

            g3dmesh_addFace ( mes, fac );
        }
    }

    obj->bbox.xmin = obj->bbox.ymin = -radius;
    obj->bbox.zmin = 0.0f;
    obj->bbox.xmax = obj->bbox.ymax =  radius;
    obj->bbox.zmax = length;

    if ( ver ) free ( ver );
}

/******************************************************************************/
G3DPRIMITIVE *g3dcylinder_new ( uint32_t id, 
                                char *name ) {
    CYLINDERDATASTRUCT *data;
    G3DPRIMITIVE *pri;

    data = ( CYLINDERDATASTRUCT * ) calloc (0x01,sizeof(CYLINDERDATASTRUCT));

    if ( data == NULL ) {
        fprintf ( stderr, "g3dcylinder_new: memory allocation failed\n" );

        return NULL;
    }

    pri = g3dprimitive_new ( id, name, data, sizeof ( CYLINDERDATASTRUCT ) );

    ((G3DOBJECT*)pri)->type = G3DCYLINDERTYPE;

    /*g3dcylinder_build ( pri, slice, capx, capy, radius, length );*/

    return pri;
}
