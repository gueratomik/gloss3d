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
###23--0---4---1--13###
#      |   |   |      #
#  22--7---8---5--14  #
#      |   |   |      #
###21--3---6---2--15###
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
            /*memcpy ( fn, &fac->nor, sizeof ( G3DVECTOR ) );*/

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
            uint32_t i;

            for ( i = 0x00; i < fac->nbver; i++ ) {
                uint32_t n = ( i + 0x01 ) % fac->nbver;
                uint32_t p = ( i + fac->nbver - 0x01 ) % fac->nbver;

              if ( fac->edg[i] ) {
                if ( ( fac->ver[i] == v0 ) && 
                     ( fac->ver[n] == v1 ) ) {
                    memcpy ( p0, &fac->edg[p]->pos, sizeof ( G3DVECTOR ) );
                    /*memcpy ( n0, &fac->edg[p]->nor, sizeof ( G3DVECTOR ) );*/
                    memcpy ( p1, &fac->edg[n]->pos, sizeof ( G3DVECTOR ) );
                    /*memcpy ( n1, &fac->edg[n]->nor, sizeof ( G3DVECTOR ) );*/

                    break;
                }

                if ( ( fac->ver[i] == v1 ) && 
                     ( fac->ver[n] == v0 ) ) {
                    memcpy ( p0, &fac->edg[n]->pos, sizeof ( G3DVECTOR ) );
                    /*memcpy ( n0, &fac->edg[n]->nor, sizeof ( G3DVECTOR ) );*/
                    memcpy ( p1, &fac->edg[p]->pos, sizeof ( G3DVECTOR ) );
                    /*memcpy ( n1, &fac->edg[p]->nor, sizeof ( G3DVECTOR ) );*/

                    break;
                }
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
void g3dsubpattern_fill ( G3DSUBPATTERN *spn, G3DFACE *fac,
                                              G3DSUBVERTEX *orivercpy, 
                                              uint32_t curdiv,
                                              uint32_t object_flags,
                                              uint32_t engine_flags ) {
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
                                        cmp[4],
                                        cmp[3],
                                        cmp[0],
                                        &vertab[24]->pos,
                                        &vertab[24]->nor );

    g3dsubpattern_getCoordsFromVertex ( fac->ver[0x01],
                                        cmp[4],
                                        cmp[0],
                                        cmp[1],
                                        &vertab[12]->pos,
                                        &vertab[12]->nor );

    g3dsubpattern_getCoordsFromVertex ( fac->ver[0x02],
                                        cmp[4],
                                        cmp[1],
                                        cmp[2],
                                        &vertab[16]->pos,
                                        &vertab[16]->nor );

    g3dsubpattern_getCoordsFromVertex ( fac->ver[0x03],
                                        cmp[4],
                                        cmp[2],
                                        cmp[3],
                                        &vertab[20]->pos,
                                        &vertab[20]->nor );

    memcpy ( &vertab[0]->pos, &orivercpy[0x00].ver.pos, sizeof ( G3DVECTOR ) );
    memcpy ( &vertab[1]->pos, &orivercpy[0x01].ver.pos, sizeof ( G3DVECTOR ) );
    memcpy ( &vertab[2]->pos, &orivercpy[0x02].ver.pos, sizeof ( G3DVECTOR ) );
    memcpy ( &vertab[3]->pos, &orivercpy[0x03].ver.pos, sizeof ( G3DVECTOR ) );

    memcpy ( &vertab[4]->pos, &fac->edg[0x00]->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &vertab[5]->pos, &fac->edg[0x01]->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &vertab[6]->pos, &fac->edg[0x02]->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &vertab[7]->pos, &fac->edg[0x03]->pos, sizeof ( G3DVECTOR ) );

    memcpy ( &vertab[8]->pos, &fac->pos, sizeof ( G3DVECTOR ) );

    if ( curdiv ) {
        for ( i = 0x00; i < 16; i++ ) {
            g3dface_position ( spn->factab[i] );
        }

        for ( i = 0x00; i < 24; i++ ) {
            g3dedge_position ( spn->edgtab[i], 0x00 );
        }

        g3dvertex_normal ( spn->vertab[0], NOVERTEXNORMAL | COMPUTEFACEPOINT | COMPUTEEDGEPOINT );
        g3dvertex_normal ( spn->vertab[1], NOVERTEXNORMAL | COMPUTEFACEPOINT | COMPUTEEDGEPOINT );
        g3dvertex_normal ( spn->vertab[2], NOVERTEXNORMAL | COMPUTEFACEPOINT | COMPUTEEDGEPOINT );
        g3dvertex_normal ( spn->vertab[3], NOVERTEXNORMAL | COMPUTEFACEPOINT | COMPUTEEDGEPOINT );
        g3dvertex_normal ( spn->vertab[4], NOVERTEXNORMAL | COMPUTEFACEPOINT | COMPUTEEDGEPOINT );
        g3dvertex_normal ( spn->vertab[5], NOVERTEXNORMAL | COMPUTEFACEPOINT | COMPUTEEDGEPOINT );
        g3dvertex_normal ( spn->vertab[6], NOVERTEXNORMAL | COMPUTEFACEPOINT | COMPUTEEDGEPOINT );
        g3dvertex_normal ( spn->vertab[7], NOVERTEXNORMAL | COMPUTEFACEPOINT | COMPUTEEDGEPOINT );
        g3dvertex_normal ( spn->vertab[8], NOVERTEXNORMAL | COMPUTEFACEPOINT | COMPUTEEDGEPOINT );
    } else {
        for ( i = 0x00; i < 16; i++ ) {
            g3dface_normal ( spn->factab[i] );
        }

        g3dvertex_normal ( spn->vertab[0], 0x00 );
        g3dvertex_normal ( spn->vertab[1], 0x00 );
        g3dvertex_normal ( spn->vertab[2], 0x00 );
        g3dvertex_normal ( spn->vertab[3], 0x00 );
        g3dvertex_normal ( spn->vertab[4], 0x00 );
        g3dvertex_normal ( spn->vertab[5], 0x00 );
        g3dvertex_normal ( spn->vertab[6], 0x00 );
        g3dvertex_normal ( spn->vertab[7], 0x00 );
        g3dvertex_normal ( spn->vertab[8], 0x00 );
    }

    if ( fac->flags & FACESELECTED ) {
        spn->factab[0]->flags |= FACESELECTED;
        spn->factab[1]->flags |= FACESELECTED;
        spn->factab[2]->flags |= FACESELECTED;
        spn->factab[3]->flags |= FACESELECTED;
    } else {
        spn->factab[0]->flags &= (~FACESELECTED);
        spn->factab[1]->flags &= (~FACESELECTED);
        spn->factab[2]->flags &= (~FACESELECTED);
        spn->factab[3]->flags &= (~FACESELECTED);
    }
}

/******************************************************************************/
void g3dsubpattern_free ( G3DSUBPATTERN *spn ) {
    uint32_t i;

    /*** delete faces ***/
    for ( i = 0; i < 16; i++ ) {
        g3dface_free   ( spn->factab[i] );
    }

    /*** delete edges ***/
    for ( i = 0; i < 24; i++ ) {
        g3dedge_free   ( spn->edgtab[i] );
    }

    /*** delete vertices ***/
    for ( i = 0; i < 25; i++ ) {
        g3dvertex_free ( spn->vertab[i] );
    }
}

/******************************************************************************/
G3DSUBPATTERN *g3dsubpattern_new ( ) {
    G3DSUBPATTERN *spn = ( G3DSUBPATTERN * ) calloc ( 0x01, sizeof ( G3DSUBPATTERN ) );
    uint32_t edgverID[24][2] = { {8, 4},{8, 5},{8, 6},{8, 7},{0, 4},{4, 1},
                                 {1, 5},{5, 2},{2, 6},{6, 3},{3, 7},{7, 0},
                                 {0, 9},{4,10},{1,11},{1,13},{5,14},{2,15},
                                 {2,17},{6,18},{3,19},{3,21},{7,22},{0,23} };
    uint32_t edgfacID[24][2] = { {0, 1},{ 1, 2},{ 2, 3},{ 3, 0},{ 0, 4},{ 1, 5},
                                 {1, 7},{ 2, 8},{ 2,10},{ 3,11},{ 3,13},{ 0,14},
                                 {4,15},{ 4, 5},{ 5, 6},{ 6, 7},{ 7, 8},{ 8, 9},
                                 {9,10},{10,11},{11,12},{12,13},{13,14},{14,15} };
    uint32_t facverID[16][4] = { {0 , 4, 8, 7},{ 4, 1, 5, 8},
                                 {8 , 5, 2, 6},{ 7, 8, 6, 3},
                                 {9 ,10, 4, 0},{10,11, 1, 4},
                                 {11,12,13, 1},{ 1,13,14, 5},
                                 {5 ,14,15, 2},{ 2,15,16,17},
                                 {6 , 2,17,18},{ 3, 6,18,19},
                                 {21, 3,19,20},{22, 7, 3,21},
                                 {23, 0, 7,22},{24, 9, 0,23} };
     int32_t facedgID[16][4] = { {4 , 0, 3,11},{ 5, 6, 1, 0},
                                 {1 , 7, 8, 2},{ 3, 2, 9,10},
                                 {-1,13, 4,12},{-1,14, 5,13},
                                 {-1,-1,15,14},{15,-1,16, 6},
                                 {16,-1,17, 7},{17,-1,-1,18},
                                 {8 ,18,-1,19},{ 9,19,-1,20},
                                 {21,20,-1,-1},{22,10,21,-1},
                                 {23,11,22,-1},{-1,12,23,-1} };
    uint32_t i, j;

    if ( spn == NULL ) {
        fprintf ( stderr, "g3dsubpattern_new: calloc failed\n" );

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

    /*** add face to edges ***/
    for ( i = 0; i < 24; i++ ) {
        g3dedge_addFace ( spn->edgtab[i], spn->factab[edgfacID[i][0]] );
        g3dedge_addFace ( spn->edgtab[i], spn->factab[edgfacID[i][1]] );
    }

    for ( i = 0; i < 16; i++ ) {
    /*** associate edges to faces ***/
        for ( j = 0; j < 4; j++ ) {
            int32_t edgID = facedgID[i][j];

            spn->factab[i]->edg[j] = ( edgID >= 0 ) ? spn->edgtab[edgID] : NULL;
        }
    }

    return spn;
}
