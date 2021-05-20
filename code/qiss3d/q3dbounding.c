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

#ifdef Q3DBOUNDING_SCHEME

                     (zmax)
                     (ymax)
 5 _________________4(xmax)
  |\               |\
  | \              | \
  |  \             |  \
  |   \            |   \
  |    \___________|____\
  |    0|          |     |1
  |     |          |     |
 6|_____|__________|7    |
  \     |          \     |
   \    |           \    |
    \   |            \   |
     \  |             \  |
      \ |              \ |
(xmin)3\|_______________\|2
(ymin)
(zmin)

                     (zmin)
                     (ymin)
   _________________ (xmin)
  |\               |\
  | \        |     | \
  |  \    \  3     |  \
  |   \    1       |   \
  |    \___________|____\
  |     |          |     | 
  | _4  |          |  5_ |
  |_____|__________|     |
  \     |      0   \     |
   \    |      \    \    |
    \   |    2       \   |
     \  |    |        \  |
      \ |              \ |
(xmax) \|_______________\| 
(ymax)
(zmax)

#endif

/******************************************************************************/
static uint32_t pointInBBox ( Q3DBOUNDING *qbnd, 
                              Q3DVECTOR3F *qpnt ) {
    if ( ( qpnt->x >= qbnd->box.min.x ) && 
         ( qpnt->z >= qbnd->box.min.y ) &&
         ( qpnt->z >= qbnd->box.min.z ) &&
         ( qpnt->x <= qbnd->box.max.x ) &&
         ( qpnt->y <= qbnd->box.max.y ) &&
         ( qpnt->z <= qbnd->box.max.z ) ) {

        return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
uint32_t q3dbouding_intersect ( Q3DBOUNDING *qbnd, 
                                Q3DRAY      *qray ) {
    Q3DLINE qlin = { .src = { .x = qray->src.x,
                              .y = qray->src.y,
                              .z = qray->src.z },
                     .dir = { .x = qray->dir.x,
                              .y = qray->dir.y,
                              .z = qray->dir.z } };
    Q3DVECTOR3F qpnt;
    uint32_t i, hit;

    if ( pointInBBox ( qbnd, &qlin.src ) ) {
        hit++;
    }

    if ( qbnd->flags & Q3DBOUNDING_FLAGS_BBOX_BIT ) {
        for ( i = 0x00, hit = 0x00; i < 0x06, hit < 0x02; i++ ) {
            if ( q3dplane_intersectLine ( &qbnd->box.qpln[i], 
                                          &qlin, 
                                          &qpnt ) ) {

                if ( pointInBBox ( qbnd, &qpnt ) ) {
                    hit++;
                }
            }
        }
    }

    if ( hit >= 0x02 ) return 0x01;


    return 0x00;
}

/******************************************************************************/
void q3dbounding_initBBox ( Q3DBOUNDING     *qbnd,
                            float            xmin, 
                            float            ymin, 
                            float            zmin,
                            float            xmax, 
                            float            ymax, 
                            float            zmax ) {
    Q3DVECTOR3F epsilon = { .x = ( xmax - xmin ) * 0.05f,
                            .y = ( ymax - ymin ) * 0.05f,
                            .z = ( zmax - zmin ) * 0.05f };

    memset ( qbnd, 0x00, sizeof ( Q3DBOUNDING ) );

    qbnd->flags = Q3DBOUNDING_FLAGS_BBOX_BIT;

    qbnd->box.qpln[0x00].z =  1.0f;
    qbnd->box.qpln[0x00].w =  zmax;

    qbnd->box.qpln[0x01].z = -1.0f;
    qbnd->box.qpln[0x01].w =  zmin;

    qbnd->box.qpln[0x02].y =  1.0f;
    qbnd->box.qpln[0x02].w =  ymax;

    qbnd->box.qpln[0x03].y = -1.0f;
    qbnd->box.qpln[0x03].w =  ymin;

    qbnd->box.qpln[0x04].x =  1.0f;
    qbnd->box.qpln[0x04].w =  xmax;

    qbnd->box.qpln[0x05].x = -1.0f;
    qbnd->box.qpln[0x05].w =  xmin;

    qbnd->box.min.x = xmin - epsilon.x;
    qbnd->box.min.y = ymin - epsilon.y;
    qbnd->box.min.z = zmin - epsilon.z;

    qbnd->box.max.x = xmax + epsilon.x;
    qbnd->box.max.y = ymax + epsilon.y;
    qbnd->box.max.z = zmax + epsilon.z;
}

/******************************************************************************/
void q3dbounding_initBSphere ( Q3DBOUNDING     *qbnd,
                               float            radius ) {
    memset ( qbnd, 0x00, sizeof ( Q3DBOUNDING ) );

    qbnd->flags = Q3DBOUNDING_FLAGS_BSPHERE_BIT;

    qbnd->sphere.radius = radius;
}
