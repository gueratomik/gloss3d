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
static uint32_t triangleIn ( Q3DOCTREE   *qoct,
                             Q3DTRIANGLE *qtri,
                             Q3DVERTEX   *qver ) {
    uint32_t qverID0 = qtri->qverID[0x00],
             qverID1 = qtri->qverID[0x01],
             qverID2 = qtri->qverID[0x02];
    Q3DVECTOR3F min = { .x = qver[qverID0].pos.x,
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
        static Q3DPLANE qpln[0x06] = { { .x =  0.0f, .y =  0.0f, .z =  1.0f },
                                       { .x =  0.0f, .y =  0.0f, .z = -1.0f },
                                       { .x =  0.0f, .y =  1.0f, .z =  0.0f },
                                       { .x =  0.0f, .y = -1.0f, .z =  0.0f },
                                       { .x =  1.0f, .y =  0.0f, .z =  0.0f },
                                       { .x = -1.0f, .y =  0.0f, .z =  0.0f } };
        uint32_t i;

        qpln[0x00].w = qoct->max.z;
        qpln[0x01].w = qoct->min.z;
        qpln[0x02].w = qoct->max.y;
        qpln[0x03].w = qoct->min.y;
        qpln[0x04].w = qoct->max.x;
        qpln[0x05].w = qoct->min.x;

        for ( i = 0x00; i < 0x06; i++ ) {
            Q3DVECTOR3F vout;
            uint32_t j;

            for ( j = 0x00; j < 0x03; j++ ) {
                uint32_t k = ( j + 0x01 ) % 0x03;

                if ( q3dplane_intersectSegment ( &qpln[i], 
                                                 &qver[j].pos,
                                                 &qver[k].pos,
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
static uint32_t pointIn ( Q3DOCTREE  *qoct, 
                          Q3DVECTOR3F *qpnt ) {
    if ( ( qpnt->x >= ( qoct->min.x - qoct->epsilon.x ) ) && 
         ( qpnt->y >= ( qoct->min.y - qoct->epsilon.y ) ) &&
         ( qpnt->z >= ( qoct->min.z - qoct->epsilon.z ) ) &&
         ( qpnt->x <= ( qoct->max.x - qoct->epsilon.x ) ) &&
         ( qpnt->y <= ( qoct->max.y - qoct->epsilon.y ) ) &&
         ( qpnt->z <= ( qoct->max.z - qoct->epsilon.z ) ) ) {

        return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
uint32_t q3doctree_free_r ( Q3DOCTREE *qoct ) {
    uint32_t i;

    if ( qoct->flags & Q3DOCTREE_HASNODES ) {
        for ( i = 0x00; i < 0x08; i++ ) {
            q3doctree_free_r ( qoct->children.node[i] );
        }
    }

    free ( qoct );
}

/******************************************************************************/
uint32_t q3doctree_intersect_r ( Q3DOCTREE   *qoct, 
                                 Q3DRAY      *qray,
                                 Q3DTRIANGLE *qtri,
                                 Q3DVERTEX   *qver,
                                 uint64_t     query_flags,
                                 uint64_t     render_flags ) {
    Q3DLINE qlin = { .src = { .x = qray->src.x,
                              .y = qray->src.y,
                              .z = qray->src.z },
                     .dir = { .x = qray->dir.x,
                              .y = qray->dir.y,
                              .z = qray->dir.z } };
    uint32_t hit = 0x00;

    if ( qoct->flags & Q3DOCTREE_HASNODES ) {
        uint32_t i;

        for ( i = 0x00; i < 0x08; i++ ) {
            q3doctree_intersect_r ( qoct->children.node[i],
                                    qray,
                                    qtri,
                                    qver,
                                    query_flags,
                                    render_flags );
        }
    }

    if ( qoct->flags & Q3DOCTREE_HASTRIANGLES ) {
        static Q3DPLANE qpln[0x06] = { { .x =  0.0f, .y =  0.0f, .z =  1.0f },
                                       { .x =  0.0f, .y =  0.0f, .z = -1.0f },
                                       { .x =  0.0f, .y =  1.0f, .z =  0.0f },
                                       { .x =  0.0f, .y = -1.0f, .z =  0.0f },
                                       { .x =  1.0f, .y =  0.0f, .z =  0.0f },
                                       { .x = -1.0f, .y =  0.0f, .z =  0.0f } };
        uint32_t i, hit = 0x00;

        qpln[0x00].w = qoct->max.z;
        qpln[0x01].w = qoct->min.z;
        qpln[0x02].w = qoct->max.y;
        qpln[0x03].w = qoct->min.y;
        qpln[0x04].w = qoct->max.x;
        qpln[0x05].w = qoct->min.x;

        if ( pointIn ( qoct, &qlin.src ) ) {
            hit++;
        }

        for ( i = 0x00, hit = 0x00; i < 0x06, hit < 0x02; i++ ) {
            Q3DVECTOR3F qpnt;

            if ( q3dplane_intersectLine ( &qpln[i], 
                                          &qlin, 
                                          &qpnt ) ) {

                if ( pointIn ( qoct, &qpnt ) ) {
                    hit++;
                }
            }
        }

        if ( hit == 0x02 ) {
            for ( i = 0x00; i < qoct->nbqtri; i++ ) {
                uint32_t qtriID = qoct->children.qtriID[i];

                hit = q3dtriangle_intersect ( &qtri[qtriID],
                                               qver, 
                                               qray,
                                               query_flags );
            }
        }
    }

    return ( hit ) ? 0x01 : 0x00;
}

/******************************************************************************/
static void q3doctree_setNodes ( Q3DOCTREE *qoct, 
                                 uint32_t   capacity ) {
    float xmin = qoct->min.x,
          ymin = qoct->min.y,
          zmin = qoct->min.z;
    float xmax = qoct->max.x,
          ymax = qoct->max.y,
          zmax = qoct->max.z;
    float xmid = ( xmin + xmax ) / 2.0f,
          ymid = ( ymin + ymax ) / 2.0f,
          zmid = ( zmin + zmax ) / 2.0f;
    Q3DVECTOR3F coord[0x08][0x02] = { { { .x = xmin, .y = ymin, .z = zmin }, 
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
    uint32_t i;

    qoct->flags &= (~Q3DOCTREE_HASTRIANGLES);
    qoct->flags |=   Q3DOCTREE_HASNODES;

    for ( i = 0x00; i < 0x08; i++ ) {
        qoct->children.node[i] = q3doctree_new ( coord[i][0x00].x,
                                                 coord[i][0x00].y,
                                                 coord[i][0x00].z,
                                                 coord[i][0x01].x,
                                                 coord[i][0x01].y,
                                                 coord[i][0x01].z );
    }
}

/******************************************************************************/
/******************* returns updated pointer in case of a realloc *************/
static Q3DOCTREE *q3doctree_build ( Q3DOCTREE   *qoct, 
                                    Q3DTRIANGLE *qtri,
                                    uint32_t    *qtriID,
                                    uint32_t     nbqtri,
                                    Q3DVERTEX   *qver,
                                    uint32_t     capacity ) {
    if ( nbqtri ) {
        uint32_t *currentID;
        uint32_t i;

        currentID = ( uint32_t * ) calloc ( nbqtri, sizeof ( uint32_t ) );

        if ( currentID == NULL ) {
            fprintf ( stderr, "%s: calloc failed\n", __func__);

            return qoct;
        }

        qoct->nbqtri = 0x00;

        for ( i = 0x00; i < nbqtri; i++ ) {
            uint32_t triangleID = ( qtriID ) ? qtriID[i] : i;

            if ( triangleIn (  qoct,
                              &qtri[triangleID],
                               qver ) ) {
                currentID[qoct->nbqtri++] = i;
            }
        }

        /*** adjust octree's size to fit items if ***/
        /***  it won't have no children nodes     ***/
        /*** Note: id nbID == nbtri, then we are in a loop ***/
        if ( ( qoct->nbqtri <= capacity ) || 
             ( qoct->nbqtri == nbqtri   ) ) {
            uint32_t arraySize = sizeof ( uint32_t ) * qoct->nbqtri;
            uint32_t structSize;

            structSize = sizeof ( Q3DOCTREE ) -
                         sizeof ((Q3DOCTREE*)NULL)->children +
                       ( sizeof ( uint32_t ) * qoct->nbqtri );

            qoct = ( Q3DOCTREE * ) realloc ( qoct, structSize );

            memcpy ( qoct->children.qtriID, currentID, arraySize );
        } else {
            q3doctree_setNodes ( qoct, capacity );

            for ( i = 0x00; i < 0x08; i++ ) {
                Q3DOCTREE *child = qoct->children.node[i];

                qoct->children.node[i] = q3doctree_build ( child,
                                                           qtri,
                                                           currentID,
                                                           qoct->nbqtri,
                                                           qver,
                                                           capacity );
            }
        }

        free ( currentID );
    }

    return qoct;
}

/******************************************************************************/
/****************** return updated pointer in case of a realloc ***************/
Q3DOCTREE *q3doctree_buildRoot ( Q3DOCTREE   *qoct,
                                 Q3DTRIANGLE *qtri,
                                 uint32_t     nbqtri,
                                 Q3DVERTEX   *qver,
                                 uint32_t     capacity ) {
    return q3doctree_build ( qoct, qtri, NULL, nbqtri, qver, capacity );
}

/******************************************************************************/
void q3doctree_init ( Q3DOCTREE *qoct,
                      float xmin, 
                      float ymin,
                      float zmin,
                      float xmax,
                      float ymax, 
                      float zmax ) {
    qoct->flags = Q3DOCTREE_HASTRIANGLES;

    qoct->min.x = xmin;
    qoct->min.y = ymin;
    qoct->min.z = zmin;

    qoct->max.x = xmax;
    qoct->max.y = ymax;
    qoct->max.z = zmax;

    qoct->epsilon.x = ( xmax - xmin ) * 0.05f;
    qoct->epsilon.y = ( ymax - ymin ) * 0.05f;
    qoct->epsilon.z = ( zmax - zmin ) * 0.05f;
}

/******************************************************************************/
Q3DOCTREE *q3doctree_new ( float xmin, 
                           float ymin,
                           float zmin,
                           float xmax,
                           float ymax, 
                           float zmax ) {
    uint32_t structSize = sizeof ( Q3DOCTREE );
    Q3DOCTREE *qoct = ( Q3DOCTREE * ) calloc ( 0x01, structSize );

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

    return qoct;
}
