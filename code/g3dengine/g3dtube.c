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
void g3dtube_build ( G3DPRIMITIVE *pri, 
                     int           slice,
                     int           capx,
                     int           capy,
                     float         radius, 
                     float         thickness,
                     float         length ) {
    TUBEDATASTRUCT *data = ( TUBEDATASTRUCT * ) pri->data;
    G3DOBJECT *obj = ( G3DOBJECT * ) pri;
    G3DMESH *mes = ( G3DMESH * ) pri;
    float sliang = ( slice ) ? ( ( float ) PRAD / slice ) : 0.0f,
          sliacc = 0.0f,
          capxstp = ( capx ) ? ( length / capx ) : 0.0f,
          capxacc = 0.0f;
    uint32_t nbver = ( slice * ( capx + 0x01 ) );
    G3DVERTEX **innerVer = ( G3DVERTEX ** ) calloc ( nbver, sizeof ( G3DVERTEX * ) );
    G3DVERTEX **outerVer = ( G3DVERTEX ** ) calloc ( nbver, sizeof ( G3DVERTEX * ) );
    uint32_t vid = 0x00;
    int i, j;
    float innerRadius = radius;
    float outerRadius = innerRadius + thickness;

    data->thickness = thickness;
    data->radius    = radius;
    data->length    = length;
    data->slice     = slice;
    data->capx      = capx;
    data->capy      = capy;

    g3dmesh_empty ( mes );

    /*** Inner Vertices creation ***/
    for ( i = 0x00; i < ( capx + 0x01 ); i++ ) {
        for ( j = 0x00; j < slice; j++, vid++ ) {
            float cossliacc = cos ( sliacc ),
                  sinsliacc = sin ( sliacc );
            G3DVECTOR3F nor, pos;

            g3dvector3f_init ( &nor, ( cossliacc ),
                                     ( sinsliacc ),
                                     ( 0.0f   ) );

            /*** Inner vertices ***/
            g3dvector3f_init ( &pos, ( nor.x * innerRadius ),
                                     ( nor.y * innerRadius ),
                                     ( capxacc ) );

            innerVer[vid] = g3dvertex_new ( pos.x, pos.y, pos.z );

            g3dmesh_addVertex ( mes, innerVer[vid] );

            /*** Outer vertices ***/
            g3dvector3f_init ( &pos, ( nor.x * outerRadius ),
                                     ( nor.y * outerRadius ),
                                     ( capxacc ) );

            outerVer[vid] = g3dvertex_new ( pos.x, pos.y, pos.z );

            g3dmesh_addVertex ( mes, outerVer[vid] );

            sliacc = ( sliacc + sliang );
        }

        capxacc = ( capxacc + capxstp );
    }

    /*** Faces creation ***/
    for ( i = 0x00; i < capx; i++ ) {
        int n = ( i + 0x01 );

        for ( j = 0x00; j < slice; j++ ) {
            int k = ( j + 0x01 ) % slice;
            G3DFACE *innerFac = g3dquad_new ( innerVer[(i*slice)+j],
                                              innerVer[(n*slice)+j],
                                              innerVer[(n*slice)+k],
                                              innerVer[(i*slice)+k] ),
                    *outerFac = g3dquad_new ( outerVer[(n*slice)+j],
                                              outerVer[(i*slice)+j],
                                              outerVer[(i*slice)+k],
                                              outerVer[(n*slice)+k] );

            if ( i == 0x00 ) {
                G3DFACE *thickFac = g3dquad_new ( innerVer[(i*slice)+k],
                                                  outerVer[(i*slice)+k],
                                                  outerVer[(i*slice)+j],
                                                  innerVer[(i*slice)+j] );
                g3dmesh_addFace ( mes, thickFac );
            }

            if ( i == ( capx - 1 ) ) {
                G3DFACE *thickFac = g3dquad_new ( innerVer[(n*slice)+j],
                                                  outerVer[(n*slice)+j],
                                                  outerVer[(n*slice)+k],
                                                  innerVer[(n*slice)+k] );
                g3dmesh_addFace ( mes, thickFac );
            }

            g3dmesh_addFace ( mes, innerFac  );
            g3dmesh_addFace ( mes, outerFac  );
        }
    }

    g3dobject_invalidate( G3DOBJECTCAST(pri), INVALIDATE_TOPOLOGY );

    obj->bbox.xmin = obj->bbox.ymin = - outerRadius;
    obj->bbox.zmin = 0.0f;
    obj->bbox.xmax = obj->bbox.ymax =   outerRadius;
    obj->bbox.zmax = length;

    if ( innerVer ) free ( innerVer );
    if ( outerVer ) free ( outerVer );
}

/******************************************************************************/
G3DPRIMITIVE *g3dtube_new ( uint32_t id, 
                            char    *name ) {
    TUBEDATASTRUCT *data;
    G3DPRIMITIVE *pri;

    data = ( TUBEDATASTRUCT * ) calloc (0x01,sizeof(TUBEDATASTRUCT));

    if ( data == NULL ) {
        fprintf ( stderr, "g3dtube_new: memory allocation failed\n" );

        return NULL;
    }

    pri = g3dprimitive_new ( id, name, data, sizeof ( TUBEDATASTRUCT ) );

    ((G3DOBJECT*)pri)->type = G3DTUBETYPE;

    /*g3dtube_build ( pri, slice, capx, capy, innerRadius, thickness, length );*/

    return pri;
}
