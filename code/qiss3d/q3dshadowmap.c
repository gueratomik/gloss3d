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
#include <qiss3d/q3d.h>

#ifdef unused

typedef struct _Q3DSHADOWUV {
    double u, v;
} Q3DSHADOWUV;

typedef struct _Q3DSHADOWMAP {
    Q3DVECTOR3F wpos;
    Q3DSHLINE  *hlines;
    Q3DSHADOWUV min;
    Q3DSHADOWUV max;
} Q3DSHADOWMAP

/******************************************************************************/
/******* http://en.wikipedia.org/wiki/UV_mapping#Finding_UV_on_a_sphere *******/
static void q3dshadowmap_mapMinMax ( Q3DSHADOWMAP *qsmp,
                                     Q3DMESH      *qmes,
                                     float         frame ) {
    Q3DVERTEXSET *qverset = q3dmesh_getVertexSet ( qmes, frame );
    Q3DVECTOR3F boundaries[0x08] = { { qverset->min.x, qverset->min.y, qverset->min.z },
                                     { qverset->max.x, qverset->min.y, qverset->min.z },
                                     { qverset->min.x, qverset->max.y, qverset->min.z },
                                     { qverset->max.x, qverset->max.y, qverset->min.z },
                                     { qverset->min.x, qverset->min.y, qverset->max.z },
                                     { qverset->max.x, qverset->min.y, qverset->max.z },
                                     { qverset->min.x, qverset->max.y, qverset->max.z },
                                     { qverset->max.x, qverset->max.y, qverset->max.z } };
    uint32_t i;

    qsmp->min.u = FLT_MAX;
    qsmp->min.v = FLT_MAX;
    qsmp->max.u = FLT_MIN;
    qsmp->max.v = FLT_MIN;

    for ( i = 0x00; i < 0x08; i++ ) {
        Q3DVECTOR3D bnor = { boundaries[i].x - qsmp->wpos.x,
                             boundaries[i].y - qsmp->wpos.y,
                             boundaries[i].z - qsmp->wpos.z };
        Q3DSHADOWUV buv;

        q3dvector3d_normalize ( &bnor, NULL );

        buv.u =   ( atan2f ( bnor.z, bnor.x ) / ( 2.0f * M_PI ) ) + 0.5f;
        buv.v = - ( asin   ( bnor.y         ) / (        M_PI ) ) + 0.5f;

        if ( qsmp->min.u > buv.u ) qsmp->min.u = buv.u;
        if ( qsmp->min.v > buv.v ) qsmp->min.v = buv.v;
        if ( qsmp->max.u > buv.u ) qsmp->max.u = buv.u;
        if ( qsmp->max.v > buv.v ) qsmp->max.v = buv.v;
    }
}

/******************************************************************************/
/******* http://en.wikipedia.org/wiki/UV_mapping#Finding_UV_on_a_sphere *******/
void q3dshadowmap_mapMesh ( Q3DSHADOWMAP *qsmp,
                            Q3DMESH      *qmes,
                            float         frame ) {
    if ( qmes->nbqtri ) {
        Q3DVERTEX *qver = q3dmesh_getVertices ( qmes, frame );
        uint32_t q3dvector3fSize = sizeof ( Q3DVECTOR3F );
        Q3DVECTOR3F *qverwpos = calloc ( qmes->nbqver, q3dvector3fSize );
        Q3DVECTOR3F *qtriwpos = calloc ( qmes->nbqtri, q3dvector3fSize );
        uint32_t i;

        q3dshadowmap_mapMinMax ( qsmp, qmes, frame );

        for ( i = 0x00; i < qmes->nbqver; i++ ) {
            q3dvector3f ( &qverwpos[i], &qver[i].pos, q3dvector3fSize );
        }

        for ( i = 0x00; i < qmes->nbqtri; i++ ) {
            uint32_t qverID[0x03] = { qmes->qtri[i].qverID[0x00],
                                      qmes->qtri[i].qverID[0x01],
                                      qmes->qtri[i].qverID[0x02] };
            Q3DSHADOWUV veruv[0x03];
            Q3DVECTOR3D trinor;
            Q3DSHADOWUV triuv;
            float avgu = 0.0f,
                  avgv = 0.0f;

            qtriwpos[i].x = ( qverwpos[qverID[0x00]].x + 
                              qverwpos[qverID[0x00]].y +
                              qverwpos[qverID[0x00]].z ) / 3.0f;
            qtriwpos[i].y = ( qverwpos[qverID[0x01]].x + 
                              qverwpos[qverID[0x01]].y +
                              qverwpos[qverID[0x01]].z ) / 3.0f;
            qtriwpos[i].z = ( qverwpos[qverID[0x02]].x + 
                              qverwpos[qverID[0x02]].y +
                              qverwpos[qverID[0x02]].z ) / 3.0f;

            trinor.x = qtriwpos[i].x - qsmp->wpos.x;
            trinor.y = qtriwpos[i].y - qsmp->wpos.y;
            trinor.z = qtriwpos[i].z - qsmp->wpos.z;

            q3dvector3d_normalize ( &trinor, NULL );

            /*** UV Spherical coordinates ****/
            triuv.u =   ( atan2f ( trinor.z, trinor.x ) / ( 2.0f * M_PI ) ) + 0.5f;
            triuv.v = - ( asin   ( trinor.y           ) / (        M_PI ) ) + 0.5f;

            for ( j = 0x00; j < 0x03; j++ ) {
                Q3DVECTOR3D vernor = { qverwpos[qverID[j]].x - qsmp->wpos.x,
                                       qverwpos[qverID[j]].y - qsmp->wpos.y,
                                       qverwpos[qverID[j]].z - qsmp->wpos.z };

                q3dvector3d_normalize ( &vernor, NULL );

                /*** UV Spherical coordinates ****/
                veruv[j].u =   ( atan2f ( vernor.z, vernor.x ) / ( 2.0f * M_PI ) ) + 0.5f;
                veruv[j].v = - ( asin   ( vernor.y           ) / (        M_PI ) ) + 0.5f;

                avgu += veruv[j].u;
                avgv += veruv[j].v;

                veruv[j].u *= qsmp->width;
                veruv[j].v *= qsmp->height;
            }

            triuv.u *= qsmp->width;
            triuv.v *= qsmp->height;

            /*** if the triangle is splitted in both sides of the map ***/
            /*** we have to test all best matches ***/
           /*if ( ( int32_t ) triuv.u != ( int32_t ) avgu ) ) {
                float best[0x03][0x02] = { { veruv[0x00].u, 1.0f - veruv[0x00].u,
                                             veruv[0x01].u, 1.0f - veruv[0x01].u,
                                             veruv[0x02].u, 1.0f - veruv[0x02].u };
            }*/
        }

        free ( qtriwpos );
        free ( qverwpos );
    }
}

/******************************************************************************/
void q3dshadowmap_map_r ( Q3DSHADOWMAP *qsmp,
                          Q3DLIGHT     *qlig,
                          Q3DSCENE     *qsce ) {

}

/******************************************************************************/
void q3dshadowmap_init ( Q3DSHADOWMAP *qsmp,
                         uint32_t      width, 
                         uint32_t      height ) {

}

/******************************************************************************/
Q3DSHADOWMAP *q3dshadowmap_new ( Q3DSCENE *qsce,
                                 uint32_t  width,
                                 uint32_t  height  ) {
    Q3DSHADOWMAP *qlig = ( Q3DSHADOWMAP * ) calloc ( 0x01, sizeof ( Q3DSHADOWMAP ) );

    if ( qlig == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    q3dlight_init ( qlig, lig, id, object_flags );


    return qlig;
}

#endif
