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
uint32_t q3dsphere_intersect ( Q3DMESH *qmes,
                               Q3DRAY  *qray, 
                               float    frame,
                               uint64_t query_flags,
                               uint64_t render_flags ) {
    G3DPRIMITIVE     *pri = ( G3DPRIMITIVE * ) ((G3DOBJECT*)qmes)->obj;
    SPHEREDATASTRUCT *sds = ( SPHEREDATASTRUCT * ) pri->data;
    float r2 = sds->radius * sds->radius;
    Q3DVERTEXSET *qverset = q3dmesh_getVertexSet ( qmes, frame );
    float a =       q3dvector3_scalar ( &qray->dir, &qray->dir );
    float b = 2.0 * q3dvector3_scalar ( &qray->ori, &qray->dir );
    float c =       q3dvector3_scalar ( &qray->ori, &qray->ori ) - r2;
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
            newqray.ori.x = qray->ori.x + ( qray->dir.x * distance );
            newqray.ori.y = qray->ori.y + ( qray->dir.y * distance );
            newqray.ori.z = qray->ori.z + ( qray->dir.z * distance );

            newqray.dir.x = - newqrqy.ori.x;
            newqray.dir.y = - newqrqy.ori.y;
            newqray.dir.z = - newqrqy.ori.z;

            newqray.distance = INFINITY;

            q3dvector_normalize ( &newqray.dir, NULL );

            q3dmesh_intersect ( qmes,
                                newqray, 
                                frame,
                                query_flags,
                                render_flags );

            qray->surface     = newqray.surface;
            qray->distance    = distance;

            qray->ratio[0x00] = newqray.ratio[0x00];
            qray->ratio[0x01] = newqray.ratio[0x01];
            qray->ratio[0x02] = newqray.ratio[0x02];

            return 0x01;
        }
    }

    return 0x00;
}
