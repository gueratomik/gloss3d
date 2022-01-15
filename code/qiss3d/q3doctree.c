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
static Q3DPLANE BOXPLN[0x06] = { { .x =  0.0f, .y =  0.0f, .z =  1.0f },
                                 { .x =  0.0f, .y =  0.0f, .z = -1.0f },
                                 { .x =  0.0f, .y =  1.0f, .z =  0.0f },
                                 { .x =  0.0f, .y = -1.0f, .z =  0.0f },
                                 { .x =  1.0f, .y =  0.0f, .z =  0.0f },
                                 { .x = -1.0f, .y =  0.0f, .z =  0.0f } };

/******************************************************************************/
static uint32_t pointIn ( Q3DOCTREE  *qoct, 
                          Q3DVECTOR3F *qpnt ) {
    if ( ( qpnt->x >= ( qoct->min.x - qoct->epsilon.x ) ) && 
         ( qpnt->y >= ( qoct->min.y - qoct->epsilon.y ) ) &&
         ( qpnt->z >= ( qoct->min.z - qoct->epsilon.z ) ) &&
         ( qpnt->x <= ( qoct->max.x + qoct->epsilon.x ) ) &&
         ( qpnt->y <= ( qoct->max.y + qoct->epsilon.y ) ) &&
         ( qpnt->z <= ( qoct->max.z + qoct->epsilon.z ) ) ) {

        return 0x01;
    }

    return 0x00;
}

#define  POINTIN(qoct,qpnt) \
     ( ( (qpnt)->x >= ( (qoct)->min.x - (qoct)->epsilon.x ) ) && \
       ( (qpnt)->y >= ( (qoct)->min.y - (qoct)->epsilon.y ) ) && \
       ( (qpnt)->z >= ( (qoct)->min.z - (qoct)->epsilon.z ) ) && \
       ( (qpnt)->x <= ( (qoct)->max.x + (qoct)->epsilon.x ) ) && \
       ( (qpnt)->y <= ( (qoct)->max.y + (qoct)->epsilon.y ) ) && \
       ( (qpnt)->z <= ( (qoct)->max.z + (qoct)->epsilon.z ) ) ) ? 0x01 : 0x00

/******************************************************************************/
static uint32_t triangleIn ( Q3DOCTREE   *qoct,
                             Q3DTRIANGLE *qtri,
                             Q3DVERTEX   *qver ) {
    uint32_t qverID[0x03] = { qtri->qverID[0x00],
                              qtri->qverID[0x01],
                              qtri->qverID[0x02] };
    Q3DVECTOR3F min = { .x = qver[qverID[0x00]].pos.x,
                        .y = qver[qverID[0x00]].pos.y,
                        .z = qver[qverID[0x00]].pos.z },
                max = { .x = qver[qverID[0x00]].pos.x,
                        .y = qver[qverID[0x00]].pos.y,
                        .z = qver[qverID[0x00]].pos.z };

    /*** Rerieve  triangle's boundaries and leave immediately ***/
    /*** if outside the octree ***/

    if ( qver[qverID[0x01]].pos.x < min.x ) min.x = qver[qverID[0x01]].pos.x;
    if ( qver[qverID[0x02]].pos.x < min.x ) min.x = qver[qverID[0x02]].pos.x;

    if ( min.x > qoct->max.x ) return 0x00;

    if ( qver[qverID[0x01]].pos.x > max.x ) max.x = qver[qverID[0x01]].pos.x;
    if ( qver[qverID[0x02]].pos.x > max.x ) max.x = qver[qverID[0x02]].pos.x;

    if ( max.x < qoct->min.x ) return 0x00;

    if ( qver[qverID[0x01]].pos.y < min.y ) min.y = qver[qverID[0x01]].pos.y;
    if ( qver[qverID[0x02]].pos.y < min.y ) min.y = qver[qverID[0x02]].pos.y;

    if ( min.y > qoct->max.y ) return 0x00;

    if ( qver[qverID[0x01]].pos.y > max.y ) max.y = qver[qverID[0x01]].pos.y;
    if ( qver[qverID[0x02]].pos.y > max.y ) max.y = qver[qverID[0x02]].pos.y;

    if ( max.y < qoct->min.y ) return 0x00;

    if ( qver[qverID[0x01]].pos.z < min.z ) min.z = qver[qverID[0x01]].pos.z;
    if ( qver[qverID[0x02]].pos.z < min.z ) min.z = qver[qverID[0x02]].pos.z;

    if ( min.z > qoct->max.z ) return 0x00;

    if ( qver[qverID[0x01]].pos.z > max.z ) max.z = qver[qverID[0x01]].pos.z;
    if ( qver[qverID[0x02]].pos.z > max.z ) max.z = qver[qverID[0x02]].pos.z;

    if ( max.z < qoct->min.z ) return 0x00;

     /*** if the polygon is entirely in the octree ***/
    if ( ( min.x >= ( qoct->min.x - qoct->epsilon.x ) ) &&
         ( min.y >= ( qoct->min.y - qoct->epsilon.y ) ) &&
         ( min.z >= ( qoct->min.z - qoct->epsilon.z ) ) &&
         ( max.x <= ( qoct->max.x + qoct->epsilon.x ) ) &&
         ( max.y <= ( qoct->max.y + qoct->epsilon.y ) ) &&
         ( max.z <= ( qoct->max.z + qoct->epsilon.z ) ) ) {

        return 0x01;
    } else {
    /*** Otherwise, check if it crosses the sides of the octree ***/
        uint32_t i;

        for ( i = 0x00; i < 0x06; i++ ) {
            Q3DPLANE qpln = { .x = BOXPLN[i].x,
                              .y = BOXPLN[i].y,
                              .z = BOXPLN[i].z,
                              .w = qoct->d[i] };
            Q3DVECTOR3F vout;
            uint32_t j;

            for ( j = 0x00; j < 0x03; j++ ) {
                uint32_t k = ( j + 0x01 ) % 0x03;

                if ( q3dplane_intersectSegment ( &qpln, 
                                                 &qver[qverID[j]].pos,
                                                 &qver[qverID[k]].pos,
                                                 &vout ) ) {
                    if ( POINTIN(qoct,&vout)) {
                        return 0x01;
                    }
                }
            }
        }
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
                                 Q3DTRIANGLE *discard,
                                 Q3DVERTEX   *qver,
                                 uint64_t     query_flags,
                                 uint64_t     render_flags ) {
    Q3DLINE qlin = { .src = { .x = qray->src.x,
                              .y = qray->src.y,
                              .z = qray->src.z },
                     .dir = { .x = qray->dir.x,
                              .y = qray->dir.y,
                              .z = qray->dir.z } };
    uint32_t hit = 0x00, trihit = 0x00, i;

    /*** check if ray is in octrree ***/
    if ( POINTIN ( qoct, &qlin.src ) ) {
        hit++;
    }

    for ( i = 0x00; i < 0x06; i++ ) {
        Q3DPLANE qpln = { .x = BOXPLN[i].x,
                          .y = BOXPLN[i].y,
                          .z = BOXPLN[i].z,
                          .w = qoct->d[i] };
        Q3DVECTOR3F qpnt;

        if ( q3dplane_intersectLine ( &qpln, 
                                      &qlin, 
                                      &qpnt ) ) {

            if ( POINTIN ( qoct, &qpnt ) ) {
                hit++;

                if ( hit == 0x02 ) break;
            }
        }
    }

    if ( hit == 0x00 ) return 0x00;
    /*********************************/

    if ( qoct->flags & Q3DOCTREE_HASNODES ) {
        for ( i = 0x00; i < 0x08; i++ ) {
            /*** Optimization: no need to check if octree empty ***/
            /*** We do it here to avoid a time-consuming function call ***/
            if ( qoct->children.node[i]->nbqtri ) {
                trihit += q3doctree_intersect_r ( qoct->children.node[i],
                                                  qray,
                                                  qtri,
                                                  discard,
                                                  qver,
                                                  query_flags,
                                                  render_flags );
                /*** small optimization. At most we can only hit four nodes ***/
                if ( trihit == 0x04 ) break;
            }
        }
    }

    if ( qoct->flags & Q3DOCTREE_HASTRIANGLES ) {
        for ( i = 0x00; i < qoct->nbqtri; i++ ) {
            uint32_t qtriID = qoct->children.qtriID[i];

            if ( &qtri[qtriID] != discard ) {
                uint32_t ret = q3dtriangle_intersect ( &qtri[qtriID],
                                                        qver, 
                                                        qray,
                                                        query_flags );

                if ( ret ) {
                    qray->isx.qsur = ( Q3DSURFACE * ) &qtri[qtriID];
                }

                trihit += ret;
            }
        }
    }

    return ( trihit ) ? 0x01 : 0x00;
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
    Q3DVECTOR3F coord[0x08] = { { .x = xmin, .y = ymin, .z = zmin }, 
                                { .x = xmin, .y = ymin, .z = zmax },
                                { .x = xmin, .y = ymax, .z = zmin }, 
                                { .x = xmin, .y = ymax, .z = zmax },
                                { .x = xmax, .y = ymin, .z = zmin }, 
                                { .x = xmax, .y = ymin, .z = zmax },
                                { .x = xmax, .y = ymax, .z = zmin }, 
                                { .x = xmax, .y = ymax, .z = zmax } };
    uint32_t i;

    qoct->flags &= (~Q3DOCTREE_HASTRIANGLES);
    qoct->flags |=   Q3DOCTREE_HASNODES;

    for ( i = 0x00; i < 0x08; i++ ) {
        qoct->children.node[i] = q3doctree_new ( coord[i].x,
                                                 coord[i].y,
                                                 coord[i].z,
                                                 xmid,
                                                 ymid,
                                                 zmid );
    }
}

/******************************************************************************/
/******************* returns updated pointer in case of a realloc *************/
static Q3DOCTREE *q3doctree_build_r ( Q3DOCTREE   *qoct, 
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
                currentID[qoct->nbqtri++] = triangleID;
            }
        }

        /*** adjust octree's size to fit items if ***/
        /***  it won't have no children nodes     ***/
        /*** Note: id nbID == nbtri, then we are in a loop ***/
        if ( ( qoct->nbqtri <= capacity ) || 
             /*** warning, for root octree, nbqtri always ***/
             /*** equals to qoct->nbqtri, we have to check it ***/
             ( ( qoct->nbqtri == nbqtri ) && ( ( qoct->flags & Q3DOCTREE_ISROOT ) == 0x00 ) ) ) {
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

                qoct->children.node[i] = q3doctree_build_r ( child,
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
    qoct->flags |= Q3DOCTREE_ISROOT;

    return q3doctree_build_r ( qoct, qtri, NULL, nbqtri, qver, capacity );
}

/******************************************************************************/
void q3doctree_init ( Q3DOCTREE *qoct,
                      float x1, 
                      float y1,
                      float z1,
                      float x2,
                      float y2, 
                      float z2 ) {
    qoct->flags = Q3DOCTREE_HASTRIANGLES;

    qoct->min.x = ( x1 < x2 ) ? x1 : x2;
    qoct->min.y = ( y1 < y2 ) ? y1 : y2;
    qoct->min.z = ( z1 < z2 ) ? z1 : z2;

    qoct->max.x = ( x1 < x2 ) ? x2 : x1;
    qoct->max.y = ( y1 < y2 ) ? y2 : y1;
    qoct->max.z = ( z1 < z2 ) ? z2 : z1;

    qoct->d[0x00] = - ( qoct->max.z * BOXPLN[0x00].z );
    qoct->d[0x01] = - ( qoct->min.z * BOXPLN[0x01].z );
    qoct->d[0x02] = - ( qoct->max.y * BOXPLN[0x02].y );
    qoct->d[0x03] = - ( qoct->min.y * BOXPLN[0x03].y );
    qoct->d[0x04] = - ( qoct->max.x * BOXPLN[0x04].x );
    qoct->d[0x05] = - ( qoct->min.x * BOXPLN[0x05].x );


/*
    qoct->epsilon.x = ( qoct->max.x - qoct->min.x ) * 0.01f;
    qoct->epsilon.y = ( qoct->max.y - qoct->min.y ) * 0.01f;
    qoct->epsilon.z = ( qoct->max.z - qoct->min.z ) * 0.01f;
*/

    /*** for perfectly flat surfaces ***/
/*** Commented out : float comparison not reliable ***/
   /* if ( qoct->epsilon.x == 0.0f ) qoct->epsilon.x = 0.01f;
    if ( qoct->epsilon.y == 0.0f ) qoct->epsilon.y = 0.01f;
    if ( qoct->epsilon.z == 0.0f ) qoct->epsilon.z = 0.01f;*/

    qoct->epsilon.x = 0.01f;
    qoct->epsilon.y = 0.01f;
    qoct->epsilon.z = 0.01f;
}

/******************************************************************************/
Q3DOCTREE *q3doctree_new ( float x1, 
                           float y1,
                           float z1,
                           float x2,
                           float y2, 
                           float z2 ) {
    uint32_t structSize = sizeof ( Q3DOCTREE );
    Q3DOCTREE *qoct = ( Q3DOCTREE * ) calloc ( 0x01, structSize );

    if ( qoct == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    q3doctree_init ( qoct,
                     x1,
                     y1,
                     z1,
                     x2,
                     y2,
                     z2 );

    return qoct;
}
