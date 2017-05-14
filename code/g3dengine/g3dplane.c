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

/*****************************************************************************/
void g3dplane_boundaries ( G3DPRIMITIVE *pri ) {
    PLANEDATASTRUCT *data = ( PLANEDATASTRUCT * ) pri->data;
    G3DOBJECT *obj = ( G3DOBJECT * ) pri;

    switch ( data->orientation ) {
        case PLANEXY :
            obj->bbox.xmax =  data->radu;
            obj->bbox.xmin = -data->radu;
            obj->bbox.ymax =  data->radv;
            obj->bbox.ymin = -data->radv;
            obj->bbox.zmax = obj->bbox.zmin = 0.0f;
        break;

        case PLANEYZ :
            obj->bbox.ymax =  data->radu;
            obj->bbox.ymin = -data->radu;
            obj->bbox.zmax =  data->radv;
            obj->bbox.zmin = -data->radv;
            obj->bbox.xmax = obj->bbox.xmin = 0.0f;
        break;

        case PLANEZX :
            obj->bbox.zmax =  data->radu;
            obj->bbox.zmin = -data->radu;
            obj->bbox.xmax =  data->radv;
            obj->bbox.xmin = -data->radv;
            obj->bbox.ymax = obj->bbox.ymin = 0.0f;
        break;

        default :
        break;
    }
}

/******************************************************************************/
void g3dplane_build ( G3DPRIMITIVE *pri, int orientation,
                                         int nbu, int nbv,
                                         float radu, float radv ) {
    PLANEDATASTRUCT *data = ( PLANEDATASTRUCT * ) pri->data;
    G3DOBJECT *obj = ( G3DOBJECT * ) pri;
    G3DMESH *mes = ( G3DMESH * ) pri;
    float difu = ( ( radu * 2.0f ) / nbu ),
          difv = ( ( radv * 2.0f ) / nbv );
    float posu, posv;
    int i, j;
    uint32_t nbver = ( ( nbu + 0x01 ) * ( nbv + 0x01 ) );
    G3DVERTEX **ver = ( G3DVERTEX ** ) calloc ( nbver, sizeof ( G3DVERTEX * ) );
    uint32_t vid = 0x00;
    uint32_t nbverv = nbv + 0x01;

    data->radu = radu;
    data->radv = radv;
    data->nbu  = nbu;
    data->nbv  = nbv;
    data->orientation = orientation;

    g3dmesh_empty ( mes );

    posu = -radu;
    /*** Vertices creation ***/
    for ( i = 0x00; i <= nbu; i++, posu += difu ) {
        posv = -radv;

        for ( j = 0x00; j <= nbv; j++, posv += difv, vid++ ) {
            G3DVECTOR nor, pos;

            switch ( data->orientation ) {
                case PLANEXY :
                    g3dvector_init ( &nor, 0.0f, 0.0f, 1.0f, 1.0f );
                    g3dvector_init ( &pos, posu, posv, 0.0f, 1.0f );
                break;

                case PLANEYZ :
                    g3dvector_init ( &nor, 1.0f, 0.0f, 0.0f, 1.0f );
                    g3dvector_init ( &pos, 0.0f, posu, posv, 1.0f );
                break;

                case PLANEZX :
                    g3dvector_init ( &nor, 0.0f, 1.0f, 0.0f, 1.0f );
                    g3dvector_init ( &pos, posv, 0.0f, posu, 1.0f );
                break;

                default :
                break;
            }

            ver[vid] = g3dvertex_new ( pos.x, pos.y, pos.z );
            g3dvector_init    ( &ver[vid]->nor, nor.x, nor.y, nor.z, 1.0f );
            g3dmesh_addVertex ( mes, ver[vid] );
        }
    }

    for ( i = 0x00; i < nbu; i++ ) {
        int n = ( i + 0x01 );

        for ( j = 0x00; j < nbv; j++ ) {
            G3DFACE *fac = g3dquad_new ( ver[(n*nbverv)+j+0x00],
                                         ver[(n*nbverv)+j+0x01],
                                         ver[(i*nbverv)+j+0x01],
                                         ver[(i*nbverv)+j+0x00] );

            g3dmesh_addFace ( mes, fac );
        }
    }

    g3dplane_boundaries  ( pri );

    free ( ver );
}

/*****************************************************************************/
G3DPRIMITIVE *g3dplane_new ( uint32_t id, char *name,
                                          int orientation,
                                          int nbu,
                                          int nbv,
                                          float radu,
                                          float radv ) {
    PLANEDATASTRUCT *data;
    G3DPRIMITIVE *pri;

    data = ( PLANEDATASTRUCT * ) calloc ( 0x01, sizeof ( PLANEDATASTRUCT ) );

    if ( data == NULL ) {
        fprintf ( stderr, "g3dplane_new: plane calloc failed\n" );

        return NULL;
    }

    pri = g3dprimitive_new ( id, name, data, sizeof ( PLANEDATASTRUCT ) );

    ((G3DOBJECT*)pri)->type = G3DPLANETYPE;

    g3dplane_build ( pri, orientation, nbu, nbv, radu, radv );


    return pri;
}
