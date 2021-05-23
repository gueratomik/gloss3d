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

/******************************************************************************/
/*** From http://www.siggraph.org/education/materials/HyperGraph/raytrace/  ***/
/*** rayplane_intersection.htm                                              ***/
/******************************************************************************/
float q3dplane_intersectLine ( Q3DPLANE    *qpla, 
                               Q3DLINE     *qlin, 
                               Q3DVECTOR3F *qpnt ) {
    float vo = ( qpla->x * qlin->src.x ) +
               ( qpla->y * qlin->src.y ) +
               ( qpla->z * qlin->src.z ) + qpla->w,
          vd = ( qpla->x * qlin->dir.x ) + 
               ( qpla->y * qlin->dir.y ) +
               ( qpla->z * qlin->dir.z );
    uint32_t ret;
    float t;

    if ( vd == 0.0f ) return 0.0f;

    t = - ( vo / vd );

    if ( t > 0.0f ) {
        qpnt->x = qlin->src.x + ( qlin->dir.x * t );
        qpnt->y = qlin->src.y + ( qlin->dir.y * t );
        qpnt->z = qlin->src.z + ( qlin->dir.z * t );

        return t;
    }

    return 0.0f;
}

/******************************************************************************/
float q3dplane_intersectSegment ( Q3DPLANE    *qpla, 
                                  Q3DVECTOR3F *pnt0,
                                  Q3DVECTOR3F *pnt1,
                                  Q3DVECTOR3F *qpnt ) {
    Q3DLINE qlin = { .src = { .x = pnt0->x,
                              .y = pnt0->y,
                              .z = pnt0->z },
                     .dir = { .x = pnt1->x - pnt0->x,
                              .y = pnt1->y - pnt0->y,
                              .z = pnt1->z - pnt0->z } };
    float vo = ( qpla->x * qlin.src.x ) +
               ( qpla->y * qlin.src.y ) +
               ( qpla->z * qlin.src.z ) + qpla->w,
          vd = ( qpla->x * qlin.dir.x ) + 
               ( qpla->y * qlin.dir.y ) +
               ( qpla->z * qlin.dir.z );
    uint32_t ret;
    float t;

    if ( vd == 0.0f ) return 0.0f;

    t = - ( vo / vd );

    if ( ( t > 0.0f ) && ( t < 1.0f ) ) {
        qpnt->x = qlin.src.x + ( qlin.dir.x * t );
        qpnt->y = qlin.src.y + ( qlin.dir.y * t );
        qpnt->z = qlin.src.z + ( qlin.dir.z * t );

        return t;
    }

    return 0.0f;
}
