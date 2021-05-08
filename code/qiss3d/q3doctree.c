/******************************************************************************/
/*  GLOSS3D is free software: you can redistribute it and/or modify           */
/*  it under the terms of the GNU General Public License as published by      */
/*  the Free Software Foundation, either version 3 of the License, or         */
/*  (at your option) any later version.                                        */
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
uint32_t q3doctree_triangleIn ( Q3DOCTREE   *qoct,
                                Q3DTRIANGLE *qtri,
                                Q3DVERTEX   *qver ) {
    uint32_t qverID0 = qtri->qverID[0x00],
             qverID1 = qtri->qverID[0x01],
             qverID2 = qtri->qverID[0x02];
    Q3DVECTOR min = { .x = qver[qverID0].pos.x,
                      .y = qver[qverID0].pos.y,
                      .z = qver[qverID0].pos.z },
              max = { .x = qver[qverID0].pos.x,
                      .y = qver[qverID0].pos.y,
                      .z = qver[qverID0].pos.z };

    /*** Rerieve  triangle's boundaries and leave immediately ***/
    /*** if outside the octree ***/

    if ( qver[qverID1].pos.x < min.x ) min.x = qver[qverID1].pos.x;
    if ( qver[qverID2].pos.x < min.x ) min.x = qver[qverID2].pos.x;

    if ( min.x > qoct->max.x ) return 0x00;

    if ( qver[qverID1].pos.x > max.x ) max.x = qver[qverID1].pos.x;
    if ( qver[qverID2].pos.x > max.x ) max.x = qver[qverID2].pos.x;

    if ( max.x < qoct->min.x ) return 0x00;

    if ( qver[qverID1].pos.y < min.y ) min.y = qver[qverID1].pos.y;
    if ( qver[qverID2].pos.y < min.y ) min.y = qver[qverID2].pos.y;

    if ( min.y > qoct->max.y ) return 0x00;

    if ( qver[qverID1].pos.y > max.y ) max.y = qver[qverID1].pos.y;
    if ( qver[qverID2].pos.y > max.y ) max.y = qver[qverID2].pos.y;

    if ( max.y < qoct->min.y ) return 0x00;

    if ( qver[qverID1].pos.z < min.z ) min.z = qver[qverID1].pos.z;
    if ( qver[qverID2].pos.z < min.z ) min.z = qver[qverID2].pos.z;

    if ( min.z > qoct->max.z ) return 0x00;

    if ( qver[qverID1].pos.z > max.z ) max.z = qver[qverID1].pos.z;
    if ( qver[qverID2].pos.z > max.z ) max.z = qver[qverID2].pos.z;

    if ( max.z < qoct->min.z ) return 0x00;

     /*** if the polygon is entirely in the octree ***/
    if ( ( min.x >= qoct->min.x ) &&
         ( min.y >= qoct->min.y ) &&
         ( min.z >= qoct->min.z ) &&
         ( max.x <= qoct->max.x ) &&
         ( max.y <= qoct->max.y ) &&
         ( max.z <= qoct->max.z ) ) {

        return 0x01;
    } else {
    /*** Otherwise, check if it crosses the sides of the octree ***/
        Q3DVECTOR v0v1 = { .x = qver[qverID1].pos.x - qver[qverID0].pos.x,
                           .y = qver[qverID1].pos.y - qver[qverID0].pos.y, 
                           .z = qver[qverID1].pos.z - qver[qverID0].pos.z },
                  v1v2 = { .x = qver[qverID2].pos.x - qver[qverID1].pos.x,
                           .y = qver[qverID2].pos.y - qver[qverID1].pos.y, 
                           .z = qver[qverID2].pos.z - qver[qverID1].pos.z },
                  v2v0 = { .x = qver[qverID0].pos.x - qver[qverID2].pos.x,
                           .y = qver[qverID0].pos.y - qver[qverID2].pos.y, 
                           .z = qver[qverID0].pos.z - qver[qverID2].pos.z };
        Q3DLINE lin[0x03] = { { .src = { .x = qver[qverID0].pos.x, 
                                         .y = qver[qverID0].pos.y,
                                         .z = qver[qverID0].pos.z },
                                .dir = { .x = v0v1.x, 
                                         .y = v0v1.y, 
                                         .z = v0v1.z } },
                              { .src = { .x = qver[qverID1].pos.x, 
                                         .y = qver[qverID1].pos.y,
                                         .z = qver[qverID1].pos.z },
                                .dir = { .x = v1v2.x, 
                                         .y = v1v2.y, 
                                         .z = v1v2.z } },
                              { .src = { .x = qver[qverID2].pos.x, 
                                         .y = qver[qverID2].pos.y,
                                         .z = qver[qverID2].pos.z },
                                .dir = { .x = v2v0.x, 
                                         .y = v2v0.y, 
                                         .z = v2v0.z } } };
        static Q3DPLANE plan[0x06] = { { .x = -1.0f, .y =  0.0f, .z =  0.0f },
                                       { .x =  1.0f, .y =  0.0f, .z =  0.0f },
                                       { .x =  0.0f, .y = -1.0f, .z =  0.0f },
                                       { .x =  0.0f, .y =  1.0f, .z =  0.0f },
                                       { .x =  0.0f, .y =  0.0f, .z = -1.0f },
                                       { .x =  0.0f, .y =  0.0f, .z =  1.0f } };
        uint32_t i;

        plan[0x00].w = qoct->min.x;
        plan[0x01].w = qoct->max.x;
        plan[0x02].w = qoct->min.y;
        plan[0x03].w = qoct->max.y;
        plan[0x04].w = qoct->min.z;
        plan[0x05].w = qoct->max.z;

        for ( i = 0x00; i < 0x06; i++ ) {
            G3DVECTOR vout;
            uint32_t j;

            for ( j = 0x00; j < 0x03; j++ ) {
                if ( q3dplane_intersectLine ( &pla[i], 
                                              &lin[j], 
                                              &vout ) ) {
                    if ( ( vout.x >= ( qoct->min.x - qoct->epsilon.x ) ) &&
                         ( vout.x <= ( qoct->max.x + qoct->epsilon.x ) ) &&
                         ( vout.y >= ( qoct->min.y - qoct->epsilon.y ) ) &&
                         ( vout.y <= ( qoct->max.y + qoct->epsilon.y ) ) &&
                         ( vout.z >= ( qoct->min.z - qoct->epsilon.z ) ) &&
                         ( vout.z <= ( qoct->max.z + qoct->epsilon.z ) ) ) {
                        return 0x01;
                    }
                }
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
void q3doctree_init ( Q3DOCTREE *qoct,
                      double     xmin, 
                      double     ymin,
                      double     zmin,
                      double     xmax,
                      double     ymax, 
                      double     zmax ) {
    qoct->min.x = xmin;
    qoct->min.y = ymin;
    qoct->min.z = zmin;
    qoct->max.x = xmax;
    qoct->max.y = ymax;
    qoct->max.z = zmax;
}

/******************************************************************************/
Q3DOCTREE *q3doctree_new ( uint32_t maxTriangles,
                           double   xmin, 
                           double   ymin,
                           double   zmin,
                           double   xmax,
                           double   ymax, 
                           double   zmax ) {
    uint32_t structSize = sizeof ( Q3DOCTREE );
    Q3DOCTREE *qoct = ( Q3DOCTREE * ) calloc ( 0x01, structSize );

    if ( qoct == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );
    }

    q3doctree_init ( qoct,
                     maxTriangles,
                     xmin,
                     ymin,
                     zmin,
                     xmax,
                     ymax,
                     zmax );

    return qoct;
}
