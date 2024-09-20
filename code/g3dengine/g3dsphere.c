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
void g3dsphere_size ( G3DPRIMITIVE *pri, float radius ) {
    SPHEREDATASTRUCT *data = ( SPHEREDATASTRUCT * ) pri->data;
    G3DOBJECT *obj = ( G3DOBJECT * ) pri;
    G3DMESH *mes = ( G3DMESH * ) pri;
    LIST *ltmpver = mes->vertexList;

    data->radius = radius;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        g3dvector3f_init ( &ver->pos, ( ver->nor.x * radius ),
                                      ( ver->nor.y * radius ),
                                      ( ver->nor.z * radius ) );

        ltmpver = ltmpver->next;
    }
/*
    mes->obj.invalidationFlags |= UPDATEFACEPOSITION;
*/
    g3dobject_invalidate( obj, INVALIDATE_SHAPE );

    /*** update the precomputed values for Catmull-Clark Subdivision ***/
    g3dmesh_update ( mes, 0x00, 0x00 );

    /*** Set bounding box dimensions ***/
    obj->bbox.xmax = obj->bbox.ymax = obj->bbox.zmax =  radius;
    obj->bbox.xmin = obj->bbox.ymin = obj->bbox.zmin = -radius;
}

/******************************************************************************/
void g3dsphere_build ( G3DPRIMITIVE *pri, int slice, 
                                          int cap, 
                                          float radius ) {
    SPHEREDATASTRUCT *data = ( SPHEREDATASTRUCT * ) pri->data;
    G3DOBJECT *obj = ( G3DOBJECT * ) pri;
    G3DMESH *mes = ( G3DMESH * ) pri;
    float sliang = ( slice ) ? ( ( float ) PRAD / slice ) : 0.0f,
          sliacc = 0.0f,
          capang = ( cap ) ? ( ( float ) M_PI / cap ) : 0.0f,
          capacc = capang;
    int i, j;
    uint32_t nbver = ( slice * ( cap - 0x01 ) ) + 0x02;
    G3DVERTEX **ver = ( G3DVERTEX ** ) calloc ( nbver, sizeof ( G3DVERTEX * ) );
    G3DVERTEX **capver = ver;        /*** 2 Cap vertices ***/
    G3DVERTEX **bdyver = ver + 0x02; /*** N Body vertices ***/
    uint32_t vid = 0x00; /*** 0 is the first vertex, 1 is the last vertex ***/
    G3DVECTOR3F nor, pos;
    G3DFACE *fac;

    g3dmesh_empty ( mes );

    data->radius = radius;
    data->slice  = slice;
    data->cap    = cap;

    /*** first vertex ***/
    capver[0x00] = g3dvertex_new ( 0.0f, 0.0f, 0.0f );
    g3dvector3f_init    ( &capver[0x00]->nor, 0.0f,  1.0f, 0.0f );
    g3dmesh_addVertex ( mes, capver[0x00] );

    /*** last vertex ***/
    capver[0x01] = g3dvertex_new ( 0.0f, 0.0f, 0.0f );
    g3dvector3f_init    ( &capver[0x01]->nor, 0.0f, -1.0f, 0.0f );
    g3dmesh_addVertex ( mes, capver[0x01] );

    /*** Vertices creation ***/
    for ( i = 0x00; i < ( cap - 0x01 ); i++ ) {
        float coscapacc = cos ( capacc ),
              sincapacc = sin ( capacc );

        slice = data->slice;

        for ( j = 0x00; j < slice; j++ ) {
            float cossliacc = cos ( sliacc ),
                  sinsliacc = sin ( sliacc );

            g3dvector3f_init ( &nor, ( sinsliacc * sincapacc ),
                                     ( coscapacc             ),
                                     ( cossliacc * sincapacc ) );

            bdyver[vid] = g3dvertex_new ( 0.0f, 0.0f, 0.0f );
            g3dvector3f_init    ( &bdyver[vid]->nor, nor.x, nor.y, nor.z );
            g3dmesh_addVertex ( mes, bdyver[vid] );

            vid++;


            sliacc = ( sliacc + sliang );
        }

        capacc = ( capacc + capang );
    }

    g3dsphere_size ( pri, radius );

    vid -= slice;

    /*** Cap creation ***/
    for ( j = 0x00; j < slice; j++ ) {
        int k = ( j + 0x01 ) % slice;
        g3dmesh_addFace ( mes, g3dtriangle_new ( capver[0x00],
                                                 bdyver[j],
                                                 bdyver[k] ) );

        g3dmesh_addFace ( mes, g3dtriangle_new ( capver[0x01],
                                                 bdyver[vid+k],
                                                 bdyver[vid+j] ) );
    }

    /*** body ***/
    for ( i = 0x00; i < ( cap - 0x02 ); i++ ) {
        int n = ( i + 0x01 );

        for ( j = 0x00; j < slice; j++ ) {
            int k = ( j + 0x01 ) % slice;
            G3DFACE *fac = g3dquad_new ( bdyver[(i*slice)+j],
                                         bdyver[(n*slice)+j],
                                         bdyver[(n*slice)+k],
                                         bdyver[(i*slice)+k] );

            g3dmesh_addFace ( mes, fac );
        }
    }

    /*** Set bounding box dimensions ***/
    obj->bbox.xmax = obj->bbox.ymax = obj->bbox.zmax =  radius;
    obj->bbox.xmin = obj->bbox.ymin = obj->bbox.zmin = -radius;

    free ( ver );
}

/*****************************************************************************/
G3DPRIMITIVE *g3dsphere_new ( uint32_t id, 
                              char    *name ) {
    SPHEREDATASTRUCT *data;
    G3DPRIMITIVE *pri;

    data = ( SPHEREDATASTRUCT * ) calloc ( 0x01, sizeof ( SPHEREDATASTRUCT ) );

    if ( data == NULL ) {
        fprintf ( stderr, "g3dsphere_new: sphere calloc failed\n" );

        return NULL;
    }

    pri = g3dprimitive_new ( id, name, data, sizeof ( SPHEREDATASTRUCT ) );

    ((G3DOBJECT*)pri)->type = G3DSPHERETYPE;

    ((G3DOBJECT*)pri)->flags |= SPHEREISPERFECT;

    /*g3dsphere_build ( pri, slice, cap, radius );*/


    return pri;
}
