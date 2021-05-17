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
                              Q3DVECTOR3  *qpnt ) {
    if ( ( qpnt->x >= qnbd->min.x ) && 
         ( qpnt->z >= qnbd->min.y ) &&
         ( qpnt->z >= qnbd->min.z ) &&
         ( qpnt->x <= qnbd->max.x ) &&
         ( qpnt->y <= qnbd->max.y ) &&
         ( qpnt->z <= qnbd->max.z ) ) {

        return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
uint32_t q3dbouding_intersect ( Q3DBOUNDING *qbnd, 
                                Q3DRAY      *qray ) {
    Q3DLINE qlin = { .ori = { .x = qray->ori.x,
                              .y = qray->ori.y,
                              .z = qray->ori.z },
                     .dir = { .x = qray->dir.x,
                              .y = qray->dir.y,
                              .z = qray->dir.z } };
    Q3DVECTOR3 qpnt;
    uint32_t i, hit;

    if ( pointInBBox ( qbnd, &qlin.ori ) {
        hit++;
    }

    if ( qbnd->flags & Q3DBOUNDING_FLAGS_BBOX_BIT ) {
        for ( i = 0x00, hit = 0x00; i < 0x06, hit < 0x02; i++ ) {
            if ( q3dplane_intersectLine ( &qbnd->bounding.box.pla[i], 
                                          &qlin, 
                                          &qpnt ) {

                if ( pointInBBox ( qbnd, &pnt ) ) {
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
    Q3DVECTOR3 epsilon = { .x = ( xmax - xmin ) * 0.05f;
                           .y = ( ymax - ymin ) * 0.05f;
                           .z = ( zmax - zmin ) * 0.05f };

    memset ( qbnd, 0x00, sizeof ( Q3DBOUNDING ) );

    qbnd->flags = Q3DBOUNDING_FLAGS_BBOX_BIT;

    qbnd->bounding.box.pla[0x00].z =  1.0f;
    qbnd->bounding.box.pla[0x00].w =  zmax;

    qbnd->bounding.box.pla[0x01].z = -1.0f;
    qbnd->bounding.box.pla[0x01].w =  zmin;

    qbnd->bounding.box.pla[0x02].y =  1.0f;
    qbnd->bounding.box.pla[0x02].w =  ymax;

    qbnd->bounding.box.pla[0x03].y = -1.0f;
    qbnd->bounding.box.pla[0x03].w =  ymin;

    qbnd->bounding.box.pla[0x04].x =  1.0f;
    qbnd->bounding.box.pla[0x04].w =  xmax;

    qbnd->bounding.box.pla[0x05].x = -1.0f;
    qbnd->bounding.box.pla[0x05].w =  xmin;

    qbnd->bounding.box.min.x = xmin - epsilon.x;
    qbnd->bounding.box.min.y = ymin - epsilon.y;
    qbnd->bounding.box.min.z = zmin - epsilon.z;

    qbnd->bounding.box.max.x = xmax + epsilon.x;
    qbnd->bounding.box.max.y = ymax + epsilon.y;
    qbnd->bounding.box.max.z = zmax + epsilon.z;
}

/******************************************************************************/
void q3dbounding_initBSphere ( Q3DBOUNDING     *qbnd,
                               float            radius ) {
    memset ( qbnd, 0x00, sizeof ( Q3DBOUNDING ) );

    qbnd->flags = Q3DBOUNDING_FLAGS_BSPHERE_BIT;

    qbnd->bounding.sphere.radius = radius;
}
