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
#include <g3d.h>

/******************************************************************************/

#ifdef TEMPLATE_SCHEME

#######################
#      #       #      #
#  @   @   @   @   @  #
#      |   |   |      #
###@---@---@---@---@###
#      |   |   |      #
#  @---@---@---@---@  #
#      |   |   |      #
###@---@---@---@---@###
#      |   |   |      #
#  @   @   @   @   @  #
#      #       #      #
#######################

#endif
#ifdef VERTEX_SCHEME
#######################
#      #       #      #
#  24  9  10  11  12  #
#      |   |   |      #
###23--1---5---2--13###
#      |   |   |      #
#  22--8---0---6--14  #
#      |   |   |      #
###21--4---7---3--15###
#      |   |   |      #
#  20 19  18  17  16  #
#      #       #      #
#######################
#endif
#ifdef EDGE_SCHEME
#######################
#      #       #      #
#  @   @   @   @   @  #
#     12  13  14      #
###@23-@-4-@-5-@-15@###
#     11   0   6      #
#  @22-@-3-@-1-@-16@  #
#     10   2   7      #
###@21-@-9-@-8-@-17@###
#     20  19  18      #
#  @   @   @   @   @  #
#      #       #      #
#######################
#endif
#ifdef FACE_SCHEME
#######################
#      #       #      #
#  @   @   @   @   @  #
#   15 | 4 | 5 | 6    #
###@---@---@---@---@###
#   14 | 0 | 1 | 7    #
#  @---@---@---@---@  #
#   13 | 3 | 2 | 8    #
###@---@---@---@---@###
#   12 |11 |10 | 9    #
#  @   @   @   @   @  #
#      #       #      #
#######################
#endif

/******************************************************************************/
void g3dsubpattern_getCoordsFromVertex ( G3DVERTEX *ver,
                                         G3DFACE   *cmp0,
                                         G3DFACE   *cmp1,
                                         G3DFACE   *cmp2,
                                         G3DVECTOR *fc, G3DVECTOR *fn ) {
    LIST *ltmpfac = ver->lfac;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        if ( ( fac != cmp0 ) &&
             ( fac != cmp1 ) &&
             ( fac != cmp2 ) ) {

            memcpy ( fc, &fac->pos, sizeof ( G3DVECTOR ) );
            memcpy ( fn, &fac->nor, sizeof ( G3DVECTOR ) );

            return;
        }

        ltmpfac = ltmpfac->next;
    }
}

/******************************************************************************/
G3DFACE *g3dsubpattern_getCoordsFromEdge ( G3DVERTEX *v0, G3DVERTEX *v1,
                                           G3DEDGE   *edg,
                                           G3DFACE   *cmp,
                                           G3DVECTOR *fc, G3DVECTOR *fn,
                                           G3DVECTOR *p0, G3DVECTOR *n0,
                                           G3DVECTOR *p1, G3DVECTOR *n1 ) {
    LIST *ltmpfac = edg->lfac;

    while ( ltmpfac ) {
        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

        if ( fac != cmp ) {
            uint32_t i, nb;

            for ( i = 0x00, nb = 0x00; i < fac->nbver, nb < 0x02; i++ ) {
                if ( ( ( fac->edg[i]->ver[0x00] == v0 ) && 
                       ( fac->edg[i]->ver[0x01] != v1 ) ) ||
                     ( ( fac->edg[i]->ver[0x01] == v0 ) && 
                       ( fac->edg[i]->ver[0x00] != v1 ) ) ) {
                    memcpy ( p0, &fac->edg[i]->pos, sizeof ( G3DVECTOR ) );
                    memcpy ( n0, &fac->edg[i]->nor, sizeof ( G3DVECTOR ) );

                    nb++;
                }

                if ( ( ( fac->edg[i]->ver[0x00] == v1 ) && 
                       ( fac->edg[i]->ver[0x01] != v0 ) ) ||
                     ( ( fac->edg[i]->ver[0x01] == v1 ) && 
                       ( fac->edg[i]->ver[0x00] != v0 ) ) ) {
                    memcpy ( p1, &fac->edg[i]->pos, sizeof ( G3DVECTOR ) );
                    memcpy ( n1, &fac->edg[i]->nor, sizeof ( G3DVECTOR ) );

                    nb++;
                }
            }

            memcpy ( fc, &fac->pos, sizeof ( G3DVECTOR ) );
            memcpy ( fn, &fac->nor, sizeof ( G3DVECTOR ) );

            return fac;
        }

        ltmpfac = ltmpfac->next;
    }

    return NULL;
}

/******************************************************************************/
void g3dsuppattern_fill ( G3DSUBPATTERN *spn, G3DFACE *fac ) {
    G3DFACE *cmp[0x05] = { NULL, NULL, NULL, NULL, fac };
    G3DVERTEX **vertab = spn->vertab;
    uint32_t i;

    cmp[0x00] = g3dsubpattern_getCoordsFromEdge ( fac->ver[0], fac->ver[1],
                                                  fac->edg[0],
                                                  fac,
                                                  &vertab[10]->pos, 
                                                  &vertab[10]->nor,
                                                  &vertab[ 9]->pos,
                                                  &vertab[ 9]->nor,
                                                  &vertab[11]->pos,
                                                  &vertab[11]->nor );

    cmp[0x01] = g3dsubpattern_getCoordsFromEdge ( fac->ver[1], fac->ver[2],
                                                  fac->edg[1],
                                                  fac,
                                                  &vertab[14]->pos,
                                                  &vertab[14]->nor,
                                                  &vertab[13]->pos,
                                                  &vertab[13]->nor,
                                                  &vertab[15]->pos,
                                                  &vertab[15]->nor );

    cmp[0x02] = g3dsubpattern_getCoordsFromEdge ( fac->ver[2], fac->ver[3],
                                                  fac->edg[2],
                                                  fac,
                                                  &vertab[18]->pos,
                                                  &vertab[18]->nor,
                                                  &vertab[17]->pos,
                                                  &vertab[17]->nor,
                                                  &vertab[19]->pos,
                                                  &vertab[19]->nor );

    cmp[0x03] = g3dsubpattern_getCoordsFromEdge ( fac->ver[3], fac->ver[0],
                                                  fac->edg[3],
                                                  fac,
                                                  &vertab[22]->pos,
                                                  &vertab[22]->nor,
                                                  &vertab[21]->pos,
                                                  &vertab[21]->nor,
                                                  &vertab[23]->pos,
                                                  &vertab[23]->nor );

    g3dsubpattern_getCoordsFromVertex ( fac->ver[0x00],
                                        cmp[5],
                                        cmp[3],
                                        cmp[0],
                                        &vertab[24]->pos,
                                        &vertab[24]->nor );

    g3dsubpattern_getCoordsFromVertex ( fac->ver[0x01],
                                        cmp[5],
                                        cmp[0],
                                        cmp[1],
                                        &vertab[12]->pos,
                                        &vertab[12]->nor );

    g3dsubpattern_getCoordsFromVertex ( fac->ver[0x02],
                                        cmp[5],
                                        cmp[1],
                                        cmp[2],
                                        &vertab[16]->pos,
                                        &vertab[16]->nor );

    g3dsubpattern_getCoordsFromVertex ( fac->ver[0x03],
                                        cmp[5],
                                        cmp[2],
                                        cmp[3],
                                        &vertab[20]->pos,
                                        &vertab[20]->nor );
 
    memcpy ( &vertab[0]->pos, &fac->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &vertab[0]->pos, &fac->nor, sizeof ( G3DVECTOR ) );



    memcpy ( &vertab[1]->pos, &fac->ver[0x00]->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &vertab[1]->pos, &fac->ver[0x00]->nor, sizeof ( G3DVECTOR ) );

    memcpy ( &vertab[2]->pos, &fac->ver[0x01]->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &vertab[2]->pos, &fac->ver[0x01]->nor, sizeof ( G3DVECTOR ) );

    memcpy ( &vertab[3]->pos, &fac->ver[0x02]->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &vertab[3]->pos, &fac->ver[0x02]->nor, sizeof ( G3DVECTOR ) );

    memcpy ( &vertab[4]->pos, &fac->ver[0x03]->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &vertab[4]->pos, &fac->ver[0x03]->nor, sizeof ( G3DVECTOR ) );



    memcpy ( &vertab[5]->pos, &fac->edg[0x00]->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &vertab[5]->pos, &fac->edg[0x00]->nor, sizeof ( G3DVECTOR ) );

    memcpy ( &vertab[6]->pos, &fac->edg[0x01]->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &vertab[6]->pos, &fac->edg[0x01]->nor, sizeof ( G3DVECTOR ) );

    memcpy ( &vertab[7]->pos, &fac->edg[0x02]->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &vertab[7]->pos, &fac->edg[0x02]->nor, sizeof ( G3DVECTOR ) );

    memcpy ( &vertab[8]->pos, &fac->edg[0x03]->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &vertab[8]->pos, &fac->edg[0x03]->nor, sizeof ( G3DVECTOR ) );
}

/******************************************************************************/
void g3dsubpattern_free ( G3DSUBPATTERN *spn ) {
    uint32_t i;

    /*** delete faces ***/
    for ( i = 0; i < 16; i++ ) {
        g3dface_free ( spn->factab[i] );
    }

    /*** delete edges ***/
    for ( i = 0; i < 24; i++ ) {
        g3dedge_free ( spn->edgtab[i] );
    }

    /*** delete vertices ***/
    for ( i = 0; i < 25; i++ ) {
        g3dvertex_free ( spn->vertab[i] );
    }
}

/******************************************************************************/
G3DSUBPATTERN *g3dsubpattern_new ( ) {
    G3DSUBPATTERN *spn = ( G3DSUBPATTERN * ) calloc ( 0x01, sizeof ( G3DSUBPATTERN ) );
    uint32_t edgverID[24][2] = { {0, 5},{0, 6},{0, 7},{0, 8},{1, 5},{2, 5},
                                 {2, 6},{3, 6},{3, 7},{4, 7},{4, 8},{1, 8},
                                 {1, 9},{5,10},{2,11},{2,13},{6,14},{3,15},
                                 {3,17},{7,18},{4,19},{4,21},{8,22},{1,23} };
    uint32_t facverID[16][4] = { {1 , 5, 0, 8},{ 5, 2, 6, 0},
                                 {0 , 6, 3, 7},{ 8, 0, 7, 4},
                                 {0 ,10, 5, 1},{10,11, 2, 5},
                                 {11,12,13, 2},{ 2,13,14, 6},
                                 {6 ,14,15, 3},{ 3,15,16,17},
                                 {7 , 3,17,18},{ 4, 7,18,19},
                                 {21, 4,19,20},{22, 8, 4,21},
                                 {23, 1, 8,22},{24, 9, 1,23} };
    uint32_t i;

    if ( spn == NULL ) {
        fprintf ( stderr, "g3dsuppattern_new: calloc failed\n" );

        return NULL;
    }

    /*** create vertices ***/
    for ( i = 0; i < 25; i++ ) {
        spn->vertab[i] = g3dvertex_new ( 0.0f, 0.0f, 0.0f );
    }

    /*** create edges ***/
    for ( i = 0; i < 24; i++ ) {
        spn->edgtab[i] = g3dedge_new ( spn->vertab[edgverID[i][0]],
                                       spn->vertab[edgverID[i][1]] );
    }

    /*** create faces ***/
    for ( i = 0; i < 16; i++ ) {
        spn->factab[i] = g3dquad_new ( spn->vertab[facverID[i][0]], 
                                       spn->vertab[facverID[i][1]],
                                       spn->vertab[facverID[i][2]],
                                       spn->vertab[facverID[i][3]] );
    }

    return spn;
}
