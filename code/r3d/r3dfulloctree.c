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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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
#include <r3d.h>

#ifdef __SCHEME__

  1_____________9____________2
  |\            \            \
  | \           |\           |\
  |  \          | \          | \
  |  17_________|_23_________|__18
  |   |\        |  |\        |  |\
  |   | \       |  | \       |  | \
  |   |  \      |  |  \      |  |  \
  |   |   5_____|__|__13_____|__|___6
  8___|___|____24__|___|_____10 |   |
  |\  |   |     \  |   |     \  |   |
  | \ |   |     |\ |   |     |\ |   |
  |  \|   |     | \|   |     | \|   |
  |  20___|_____|__26__|_____|__|21 |
  |   |\  |     |  |\  |     |  |\  |
  |   | \ |     |  | \ |     |  | \ |
  |   |  \|_____|__|__25_____|__|__\|14
  0___|__12____11__|___|_____|3 |   |
   \  |   |     \  |   |     \  |   |
    \ |   |      \ |   |      \ |   |
     \|   |       \|   |       \|   |
     16___|_______22___|________19  |
       \  |         \  |         \  |
        \ |          \ |          \ |
         \|           \|           \|
          \____________|____________| 
          4            15            7

#endif

#define MAXFULLOCTREEDEPTH 0x06
/******************************************************************************/
static uint32_t getNodeCount ( uint32_t depth ) {
    uint32_t nbNodes = 0x00;
    uint32_t i;

    for ( i = 0x00; i < depth; i++ ) {
        nbNodes += pow ( 8, i );
    }

    return nbNodes;
}

/******************************************************************************/
/*** Build an octree from a list of render faces. Maxfaces set the number of **/
/*** faces when it's necessary to split the octree into sub children octrees.**/
/******************************************************************************/

R3DFULLOCTREE *r3dfulloctree_new ( float xone, float yone, float zone,
                                   float xtwo, float ytwo, float ztwo,
                                   uint32_t nbrfac ) {
    R3DFULLOCTREE *rft = ( R3DFULLOCTREE * ) calloc ( 0x01, sizeof ( R3DFULLOCTREE ) );
    float xepsilon = fabs ( xone - xtwo ) * 1e-3f,
          yepsilon = fabs ( yone - ytwo ) * 1e-3f,
          zepsilon = fabs ( zone - ztwo ) * 1e-3f;
    uint32_t nbNodes = 0x00;
    uint32_t i;

    if ( rft == NULL ) {
        fprintf ( stderr, "r3dfulloctree_new: calloc failed\n" );

        return NULL;
    }

    nbNodes = getNodeCount ( MAXFULLOCTREEDEPTH );

    printf ( "NBNODES:%lu, %lu\n", nbNodes, nbNodes * sizeof ( R3DFULLOCTREENODE ) );

    rft->nodes = calloc ( nbNodes, sizeof ( R3DFULLOCTREENODE ) );

    rft->rfactab = calloc ( nbrfac, sizeof ( R3DFACE * ) );

    rft->xmin = ( xone > xtwo ) ? xtwo : xone;
    rft->xmax = ( xone < xtwo ) ? xtwo : xone;
    rft->ymin = ( yone > ytwo ) ? ytwo : yone;
    rft->ymax = ( yone < ytwo ) ? ytwo : yone;
    rft->zmin = ( zone > ztwo ) ? ztwo : zone;
    rft->zmax = ( zone < ztwo ) ? ztwo : zone;

    return rft;
}

/******************************************************************************/
void r3dfulloctree_assignFacePointers ( R3DFULLOCTREE *rft ) {
    uint32_t nbNodes = getNodeCount ( MAXFULLOCTREEDEPTH );
    R3DFACE **rfactab = rft->rfactab;
    uint32_t totalFaces = 0x00;
    uint32_t totalNodes = 0x00;
    uint32_t i;

    for ( i = 0x00; i < nbNodes; i++ ) {
        if ( rft->nodes[i].nbrfac ) {
            rft->nodes[i].rfac = rfactab;
printf(" %s assigning %lu faces\n", __func__, rft->nodes[i].nbrfac);
            rfactab += rft->nodes[i].nbrfac;

            totalFaces += rft->nodes[i].nbrfac;
            totalNodes++;
        }
    }
printf(" %s total Faces %lu %lu\n", __func__, totalFaces, totalFaces / totalNodes );
}

/******************************************************************************/
void r3dfulloctree_count ( R3DFULLOCTREE *rft, R3DFACE   *rfac, 
                                               R3DVERTEX *rver,
                                               uint32_t   nbrfac ) {
    uint32_t i;
printf("%s %d\n", __func__, nbrfac);
    for ( i = 0x00; i < nbrfac; i++ ) {
        r3dfulloctreenode_count_r ( rft->nodes, 
                                    0x00, 
                                    rft->xmin,
                                    rft->ymin,
                                    rft->zmin,
                                    rft->xmax,
                                    rft->ymax,
                                    rft->zmax,
                                    0x00,
                                   &rfac[i],
                                    rver );
    }
}

/******************************************************************************/
uint32_t r3dfulloctreenode_count_r ( R3DFULLOCTREENODE *fon,
                                     uint32_t           id,
                                     float              xmin, 
                                     float              ymin,
                                     float              zmin,
                                     float              xmax,
                                     float              ymax,
                                     float              zmax,
                                     uint32_t           depth,
                                     R3DFACE           *rfac,
                                     R3DVERTEX         *rver ) {
    if ( depth < MAXFULLOCTREEDEPTH ) {
        uint32_t i;

        /*** all 3 points must lie within the octree ***/
        for ( i = 0x00; i < 0x03; i++ ) {
            if ( ( rver[rfac->rverID[i]].pos.x < xmin ) ||
                 ( rver[rfac->rverID[i]].pos.x > xmax ) ||
                 ( rver[rfac->rverID[i]].pos.y < ymin ) ||
                 ( rver[rfac->rverID[i]].pos.y > ymax ) ||
                 ( rver[rfac->rverID[i]].pos.z < zmin ) ||
                 ( rver[rfac->rverID[i]].pos.z > zmax ) ) {
                return 0x00;
            }
        }

        fon[id].nbrfac++;

        /*** if that's the case, check if it also lies within a suboctree ***/
        float avgx = ( xmin + xmax ) * 0.5f,
              avgy = ( ymin + ymax ) * 0.5f,
              avgz = ( zmin + zmax ) * 0.5f;
        G3DVECTOR  p[0x1B] = { { .x = xmin, .y = ymin, .z = zmin },
                               { .x = xmin, .y = ymax, .z = zmin },
                               { .x = xmax, .y = ymax, .z = zmin },
                               { .x = xmax, .y = ymin, .z = zmin },
                               { .x = xmin, .y = ymin, .z = zmax },
                               { .x = xmin, .y = ymax, .z = zmax },
                               { .x = xmax, .y = ymax, .z = zmax },
                               { .x = xmax, .y = ymin, .z = zmax },

                               { .x = xmin, .y = avgy, .z = zmin },
                               { .x = avgx, .y = ymax, .z = zmin },
                               { .x = xmax, .y = avgy, .z = zmin },
                               { .x = avgx, .y = ymin, .z = zmin },

                               { .x = xmin, .y = avgy, .z = zmax },
                               { .x = avgx, .y = ymax, .z = zmax },
                               { .x = xmax, .y = avgy, .z = zmax },
                               { .x = avgx, .y = ymin, .z = zmax },

                               { .x = xmin, .y = ymin, .z = avgz },
                               { .x = xmin, .y = ymax, .z = avgz },
                               { .x = xmax, .y = ymax, .z = avgz },
                               { .x = xmax, .y = ymin, .z = avgz },

                               { .x = xmin, .y = avgy, .z = avgz },
                               { .x = xmax, .y = avgy, .z = avgz },
                               { .x = avgx, .y = ymin, .z = avgz },
                               { .x = avgx, .y = ymax, .z = avgz },
                               { .x = avgx, .y = avgy, .z = zmin },
                               { .x = avgx, .y = avgy, .z = zmax },

                               { .x = avgx, .y = avgy, .z = avgz } };
        G3DVECTOR *node[0x08][0x02] = { { &p[0] , &p[26] },
                                        { &p[8] , &p[23] },
                                        { &p[24], &p[18] },
                                        { &p[11], &p[21] },
                                        { &p[16], &p[25] },
                                        { &p[20], &p[13] },
                                        { &p[26], &p[6]  },
                                        { &p[22], &p[14] } };
        uint32_t nbSkipNodes = pow ( 8, depth );
        uint32_t withinChild = 0x00;

        for ( i = 0x00; i < 0x08; i++ ) {
            uint32_t offset = nbSkipNodes;

            if ( r3dfulloctreenode_count_r ( fon + offset,
                                             (id * 0x08) + i,
                                             node[i][0x00]->x,
                                             node[i][0x00]->y,
                                             node[i][0x00]->z,
                                             node[i][0x01]->x,
                                             node[i][0x01]->y,
                                             node[i][0x01]->z,
                                             depth + 0x01,
                                             rfac,
                                             rver ) ) {
                fon[id].nbrfac--;

                return 0x01;
            }
        }

        return 0x01;
    }

    return 0x00;
}




