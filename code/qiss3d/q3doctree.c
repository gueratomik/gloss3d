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

#ifdef OCTREE_SCHEME
    _____________________O xmax, ymax, zmax
   |\         |\         |\
   | \        | \        | \
   |  \_______|__\_______|__\
   |   |      |  |\      |  |\
   |___|\_____|__|_\_____|  | \
   |\  | \    |\ |  \    \  |  \
   | \ |  \___|_\|___\___|\_|___\
   |  \|   |  |  \   |   | \|   |
   |   |___|__|__|\__|___|__\   |
   |___|\__|__|__|_\_|___|  |\  |
    \  | \ |   \ |  \|   \  | \ |
     \ |  \|____\|___\____\_|__\|
      \|___|_____|___|_____\|   |
       \   |      \  |      \   |
        \  |       \ |       \  |
         \ |        \|        \ |
          \O_________\_________\|
   xmin, ymin, zmin
#endif

/******************************************************************************/
static uint32_t triangleIn ( Q3DTRIANGLE *qtri,
                             Q3DVERTEX   *qver
                             double       xmin,
                             double       ymin,
                             double       zmin,
                             double       xmax,
                             double       ymax,
                             double       zmax ) {
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

    if ( min.x > xmax ) return 0x00;

    if ( qver[qverID1].pos.x > max.x ) max.x = qver[qverID1].pos.x;
    if ( qver[qverID2].pos.x > max.x ) max.x = qver[qverID2].pos.x;

    if ( max.x < xmin ) return 0x00;

    if ( qver[qverID1].pos.y < min.y ) min.y = qver[qverID1].pos.y;
    if ( qver[qverID2].pos.y < min.y ) min.y = qver[qverID2].pos.y;

    if ( min.y > ymax ) return 0x00;

    if ( qver[qverID1].pos.y > max.y ) max.y = qver[qverID1].pos.y;
    if ( qver[qverID2].pos.y > max.y ) max.y = qver[qverID2].pos.y;

    if ( max.y < ymin ) return 0x00;

    if ( qver[qverID1].pos.z < min.z ) min.z = qver[qverID1].pos.z;
    if ( qver[qverID2].pos.z < min.z ) min.z = qver[qverID2].pos.z;

    if ( min.z > zmax ) return 0x00;

    if ( qver[qverID1].pos.z > max.z ) max.z = qver[qverID1].pos.z;
    if ( qver[qverID2].pos.z > max.z ) max.z = qver[qverID2].pos.z;

    if ( max.z < zmin ) return 0x00;

     /*** if the polygon is entirely in the octree ***/
    if ( ( min.x >= xmin ) &&
         ( min.y >= ymin ) &&
         ( min.z >= zmin ) &&
         ( max.x <= xmax ) &&
         ( max.y <= ymax ) &&
         ( max.z <= zmax ) ) {

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

        plan[0x00].w = xmin;
        plan[0x01].w = xmax;
        plan[0x02].w = ymin;
        plan[0x03].w = ymax;
        plan[0x04].w = zmin;
        plan[0x05].w = zmax;

        for ( i = 0x00; i < 0x06; i++ ) {
            G3DVECTOR vout;
            uint32_t j;

            for ( j = 0x00; j < 0x03; j++ ) {
                if ( q3dplane_intersectLine ( &pla[i], 
                                              &lin[j], 
                                              &vout ) ) {
                    if ( ( vout.x >= xmin ) &&
                         ( vout.x <= xmax ) &&
                         ( vout.y >= ymin ) &&
                         ( vout.y <= ymax ) &&
                         ( vout.z >= zmin ) &&
                         ( vout.z <= zmax ) ) {
                        return 0x01;
                    }
                }
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
static void q3doctree_setNodes ( Q3DOCTREE *qoct, 
                                 uint32_t   capacity ) {
    double xmin = qoct->min.x,
           ymin = qoct->min.y,
           zmin = qoct->min.z;
    double xmax = qoct->max.x,
           ymax = qoct->max.y,
           zmax = qoct->max.z;
    double xmid = ( xmin + xmax ) / 2.0f,
           ymid = ( ymin + ymax ) / 2.0f,
           zmid = ( zmin + zmax ) / 2.0f;
    Q3DVECTOR coord[0x08][0x02] = { { { .x = xmin, .y = ymin, .z = zmin }, 
                                      { .x = xmid, .y = ymid, .z = zmid } },
                                    { { .x = xmid, .y = ymin, .z = zmin }, 
                                      { .x = xmax, .y = ymid, .z = zmid } },
                                    { { .x = xmin, .y = ymid, .z = zmin }, 
                                      { .x = xmid, .y = ymax, .z = zmid } },
                                    { { .x = xmid, .y = ymid, .z = zmin }, 
                                      { .x = xmax, .y = ymax, .z = zmid } },
                                    { { .x = xmin, .y = ymin, .z = zmid }, 
                                      { .x = xmid, .y = ymid, .z = zmax } },
                                    { { .x = xmid, .y = ymin, .z = zmid }, 
                                      { .x = xmax, .y = ymid, .z = zmax } },
                                    { { .x = xmin, .y = ymid, .z = zmid }, 
                                      { .x = xmid, .y = ymax, .z = zmax } },
                                    { { .x = xmid, .y = ymid, .z = zmid }, 
                                      { .x = xmax, .y = ymax, .z = zmax } } };

    qoct->flags &= (~Q3DOCTREE_HASTRIANGLES);

    qoct->flags |= Q3DOCTREE_HASNODES;

    for ( i = 0x00; i < 0x08; i++ ) {
        qoct->node[i] = q3doctree_new ( capacity, 
                                        coord[i][0x00].x,
                                        coord[i][0x00].y,
                                        coord[i][0x00].z,
                                        coord[i][0x01].x,
                                        coord[i][0x01].y,
                                        coord[i][0x01].z );
    }
}

/******************************************************************************/
static void q3doctree_build ( Q3DOCTREE   *qoct, 
                              Q3DTRIANGLE *qtri,
                              uint32_t    *qtriID,
                              uint32_t     nbtri,
                              Q3DVERTEX   *qver,
                              uint32_t     capacity ) {
    if ( nbtri ) {
        uint32_t *currentID;
        Q3DVECTOR epsilon = { .x = ( xmax - xmin ) * 0.05f;
                              .y = ( ymax - ymin ) * 0.05f;
                              .z = ( zmax - zmin ) * 0.05f };
        uint32_t i;

        currentID = ( uint32_t * ) calloc ( nbtri, sizeof ( uint32_t ) );

        if ( currentID == NULL ) {
            fprintf ( stderr, "%s: calloc failed\n", __func__);

            return;
        }

        qoct->nbtri = 0x00;

        for ( i = 0x00; i < nbtri; i++ ) {
            uint32_t triangleID = ( qtriID ) ? qtriID[i] : i;

            if ( triangleIn ( &qtri[triangleID],
                               qver,
                               xmin - epsilon.x,
                               ymin - epsilon.y,
                               zmin - epsilon.z,
                               xmax + epsilon.x,
                               ymax + epsilon.y,
                               zmax + epsilon.z ) ) {
                currentID[qoct->nbtri++] = i;
            }
        }

        /*** adjust octree's size to fit items if ***/
        /***  it won't have no children nodes     ***/
        /*** Note: id nbID == nbtri, then we are in a loop ***/
        if ( ( qoct->nbtri <= capacity ) || ( qoct->nbtri == nbtri ) ) {
            uint32_t arraySize = sizeof ( uint32_t ) * qoct->nbtri;

            structSize = sizeof ( Q3DOCTREE ) -
                         sizeof ((Q3DOCTREE*)NULL)->children.node +
                     ( ( sizeof ((Q3DOCTREE*)NULL)->children.qtriID) * qoct->nbtri );

            qoct = ( Q3DOCTREE * ) realloc ( structSize );

            memcpy ( qoct->children.qtriID, currentID, arraySize );
        } else {
            q3doctree_setNodes ( qoct, capacity );

            for ( i = 0x00; i < 0x08; i++ ) {
                q3doctree_distributeTriangles ( qoct->children.node[i],
                                                qtri,
                                                currentID,
                                                nbID,
                                                qver,
                                                capacity );
            }
        }

        free ( currentID );
    }
}

/******************************************************************************/
void q3doctree_buildRoot ( Q3DOCTREE   *qoct,
                           Q3DTRIANGLE *qtri,
                           uint32_t     nbtri,
                           Q3DVERTEX   *qver,
                           uint32_t     capacity ) {
    q3doctree_build ( qoct, qtri, NULL, nbtri, qver, capacity );
}

/******************************************************************************/
void q3doctree_init ( Q3DOCTREE *qoct,
                      double     xmin, 
                      double     ymin,
                      double     zmin,
                      double     xmax,
                      double     ymax, 
                      double     zmax ) {
    qoct->flags = Q3DOCTREE_HASTRIANGLES;

    qoct->min.x = xmin;
    qoct->min.y = ymin;
    qoct->min.z = zmin;
    qoct->max.x = xmax;
    qoct->max.y = ymax;
    qoct->max.z = zmax;
}

/******************************************************************************/
Q3DOCTREE *q3doctree_new ( double       xmin, 
                           double       ymin,
                           double       zmin,
                           double       xmax,
                           double       ymax, 
                           double       zmax ) {
    Q3DOCTREE *qoct = NULL;

    if ( nbtri ) {
        uint32_t *qtriID;
        uint32_t  nbqtriID = 0x00;
        Q3DVECTOR epsilon = { .x = ( xmax - xmin ) * 0.05f;
                              .y = ( ymax - ymin ) * 0.05f;
                              .z = ( zmax - zmin ) * 0.05f };
        uint32_t structSize = sizeof ( Q3DOCTREE );

        uint32_t i;

        qoct = ( Q3DOCTREE * ) calloc ( 0x01, structSize );

        if ( qoct == NULL ) {
            fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

            return NULL;
        }

        q3doctree_init ( qoct,
                         xmin,
                         ymin,
                         zmin,
                         xmax,
                         ymax,
                         zmax );
    }

    return qoct;
}
