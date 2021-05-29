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

/******************************************************************************
Thanks to :
 https://viclw17.github.io/2018/07/16/raytracing-ray-sphere-intersection/ 
/******************************************************************************/
uint32_t q3dsphere_intersect ( Q3DMESH    *qmes,
                               Q3DRAY     *qray, 
                               Q3DSURFACE *discard,
                               float       frame,
                               uint64_t    query_flags,
                               uint64_t    render_flags ) {
    G3DPRIMITIVE     *pri = ( G3DPRIMITIVE * ) ((Q3DOBJECT*)qmes)->obj;
    SPHEREDATASTRUCT *sds = ( SPHEREDATASTRUCT * ) pri->data;
    float r2 = sds->radius * sds->radius;
    Q3DVERTEXSET *qverset = q3dmesh_getVertexSet ( qmes, frame );
    float a =       q3dvector3f_scalar ( &qray->dir, &qray->dir );
    float b = 2.0 * q3dvector3f_scalar ( &qray->src, &qray->dir );
    float c =       q3dvector3f_scalar ( &qray->src, &qray->src ) - r2;
    float discriminant = ( b * b ) - ( 4 * a * c );

    if ( discriminant > 0.0f ) {
        float numerator = -b - sqrt ( discriminant );
        float distance;
        Q3DRAY newqray;

        memset ( &newqray, 0x00, sizeof ( Q3DRAY ) );

        if ( numerator > 0.0f ) {
            distance = numerator / ( 2.0 * a );
        } else {
            numerator = -b + sqrt ( discriminant );

            if ( numerator > 0.0f ) {
                distance = numerator / (2.0 * a);
            } else {
                return 0x00;
            }
        }

        if ( distance < qray->distance ) {
            newqray.src.x = qray->src.x + ( qray->dir.x * distance );
            newqray.src.y = qray->src.y + ( qray->dir.y * distance );
            newqray.src.z = qray->src.z + ( qray->dir.z * distance );

            newqray.dir.x = - newqray.src.x;
            newqray.dir.y = - newqray.src.y;
            newqray.dir.z = - newqray.src.z;

            newqray.distance = INFINITY;

            q3dvector3f_normalize ( &newqray.dir, NULL );

            /*** In case of a discarded face, the mesh would return 0 ***/
            /*** We only want the ray to intersect it does not hit a ***/
            /*** discarded face, otherwise there'll be small dots ***/
            if ( q3dmesh_intersect ( qmes,
                                    &newqray, 
                                     discard,
                                     frame,
                                     query_flags,
                                     render_flags ) ) {

                 qray->qobjID      = newqray.qobjID;
                 qray->qtriID      = newqray.qtriID;
                 qray->distance    = distance;

                 qray->ratio[0x00] = newqray.ratio[0x00];
                 qray->ratio[0x01] = newqray.ratio[0x01];
                 qray->ratio[0x02] = newqray.ratio[0x02];

                 /*** intersection occured, let's remember it ***/
                 qray->flags |= Q3DRAY_HAS_HIT_BIT;

                return 0x01;
            }
        }
    }

    return 0x00;
}
