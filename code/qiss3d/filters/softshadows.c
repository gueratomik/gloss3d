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
typedef struct _FILTERSOFTSHADOWS {
    uint32_t radius;
} FILTERSOFTSHADOWS;

/******************************************************************************/
static void sss ( Q3DSOFTSHADOW *qssh,
                  unsigned char *srcimg,
                  unsigned char *dstimg,
                  uint32_t       width,
                  uint32_t       height,
                  uint32_t       depth,
                  uint32_t       x,
                  uint32_t       y ) {
    /*static int32_t block[0x08][0x02] = { { -1,  1 }, {  0,  1 }, {  1,  1 },
                                         { -1,  0 },             {  1,  0 },
                                         { -1, -1 }, {  0, -1 }, {  1, -1 } };*/

    /*static int32_t block[0x18][0x02] = { { -2,  2 }, { -1,  2 }, {  0,  2 }, {  1,  2 }, {  2,  2 },
                                         { -2,  1 }, { -1,  1 }, {  0,  1 }, {  1,  1 }, {  2,  1 },
                                         { -2,  0 }, { -1,  0 },             {  1,  0 }, {  2,  0 },
                                         { -2, -1 }, { -1, -1 }, {  0, -1 }, {  1, -1 }, {  2, -1 },
                                         { -2, -2 }, { -1, -2 }, {  0, -2 }, {  1, -2 }, {  2, -2 } };*/

    /*static int32_t block[0x50][0x02] = { { -4,  4 }, { -3,  4 }, { -2,  4 }, { -1,  4 }, {  0,  4 }, {  1,  4 }, {  2,  4 }, { 3,  4 }, { 4,  4 },
                                         { -4,  3 }, { -3,  3 }, { -2,  3 }, { -1,  3 }, {  0,  3 }, {  1,  3 }, {  2,  3 }, { 3,  3 }, { 4,  3 },
                                         { -4,  2 }, { -3,  2 }, { -2,  2 }, { -1,  2 }, {  0,  2 }, {  1,  2 }, {  2,  2 }, { 3,  2 }, { 4,  2 },
                                         { -4,  1 }, { -3,  1 }, { -2,  1 }, { -1,  1 }, {  0,  1 }, {  1,  1 }, {  2,  1 }, { 3,  1 }, { 4,  1 },
                                         { -4,  0 }, { -3,  0 }, { -2,  0 }, { -1,  0 },             {  1,  0 }, {  2,  0 }, { 3,  0 }, { 4,  0 },
                                         { -4, -1 }, { -3, -1 }, { -2, -1 }, { -1, -1 }, {  0, -1 }, {  1, -1 }, {  2, -1 }, { 3, -1 }, { 4, -1 },
                                         { -4, -2 }, { -3, -2 }, { -2, -2 }, { -1, -2 }, {  0, -2 }, {  1, -2 }, {  2, -2 }, { 3, -2 }, { 4, -2 },
                                         { -4, -3 }, { -3, -3 }, { -2, -3 }, { -1, -3 }, {  0, -3 }, {  1, -3 }, {  2, -3 }, { 3, -3 }, { 4, -3 },
                                         { -4, -4 }, { -3, -4 }, { -2, -4 }, { -1, -4 }, {  0, -4 }, {  1, -4 }, {  2, -4 }, { 3, -4 }, { 4, -4 }, }; */
    static int32_t block[120][0x02] = { { -5,  5 }, { -4,  5 }, { -3,  5 }, { -2,  5 }, { -1,  5 }, {  0,  5 }, {  1,  5 }, {  2,  5 }, { 3,  5 }, { 4,  5 }, { 5,  5 },
                                        { -5,  4 }, { -4,  4 }, { -3,  4 }, { -2,  4 }, { -1,  4 }, {  0,  4 }, {  1,  4 }, {  2,  4 }, { 3,  4 }, { 4,  4 }, { 5,  4 },
                                        { -5,  3 }, { -4,  3 }, { -3,  3 }, { -2,  3 }, { -1,  3 }, {  0,  3 }, {  1,  3 }, {  2,  3 }, { 3,  3 }, { 4,  3 }, { 5,  3 },
                                        { -5,  2 }, { -4,  2 }, { -3,  2 }, { -2,  2 }, { -1,  2 }, {  0,  2 }, {  1,  2 }, {  2,  2 }, { 3,  2 }, { 4,  2 }, { 5,  2 },
                                        { -5,  1 }, { -4,  1 }, { -3,  1 }, { -2,  1 }, { -1,  1 }, {  0,  1 }, {  1,  1 }, {  2,  1 }, { 3,  1 }, { 4,  1 }, { 5,  1 },
                                        { -5,  0 }, { -4,  0 }, { -3,  0 }, { -2,  0 }, { -1,  0 },             {  1,  0 }, {  2,  0 }, { 3,  0 }, { 4,  0 }, { 5,  0 },
                                        { -5, -1 }, { -4, -1 }, { -3, -1 }, { -2, -1 }, { -1, -1 }, {  0, -1 }, {  1, -1 }, {  2, -1 }, { 3, -1 }, { 4, -1 }, { 5, -1 },
                                        { -5, -2 }, { -4, -2 }, { -3, -2 }, { -2, -2 }, { -1, -2 }, {  0, -2 }, {  1, -2 }, {  2, -2 }, { 3, -2 }, { 4, -2 }, { 5, -2 },
                                        { -5, -3 }, { -4, -3 }, { -3, -3 }, { -2, -3 }, { -1, -3 }, {  0, -3 }, {  1, -3 }, {  2, -3 }, { 3, -3 }, { 4, -3 }, { 5, -3 },
                                        { -5, -4 }, { -4, -4 }, { -3, -4 }, { -2, -4 }, { -1, -4 }, {  0, -4 }, {  1, -4 }, {  2, -4 }, { 3, -4 }, { 4, -4 }, { 5, -4 },
                                        { -5, -5 }, { -4, -5 }, { -3, -5 }, { -2, -5 }, { -1, -5 }, {  0, -5 }, {  1, -5 }, {  2, -5 }, { 3, -5 }, { 4, -5 }, { 5, -5 } };
    uint32_t offset = ( y * width ) + x;


    switch ( depth ) {
        case 0x18 :
        case 0x20 : {
            unsigned char (*refimg)[0x03] = srcimg,
                          (*sssimg)[0x03] = dstimg;
            /*** the pixel weighs x times more than adjacent pixels ***/
            uint32_t weight = 0x01;
            uint32_t dstR = refimg[offset][0x00] * weight, 
                     dstG = refimg[offset][0x01] * weight,
                     dstB = refimg[offset][0x02] * weight;
            uint32_t nbs = weight;
            float shadow = qssh[offset].shadow * weight;
            uint32_t i;

            for ( i = 0x00; i < 120; i++ ) {
                int32_t tx = ( int32_t ) x + block[i][0x00],
                        ty = ( int32_t ) y + block[i][0x01];

                if ( ( tx > 0x00 ) && ( tx < width  ) &&
                     ( ty > 0x00 ) && ( ty < height ) ) {
                    uint32_t toffset = ( ty * width ) + tx;

                    if ( qssh[toffset].qobjID == qssh[offset].qobjID ) {
                        shadow +=  qssh[toffset].shadow;

                        nbs++;
                    }
                }
            }

            sssimg[offset][0x00] *= ( 1.0f - ( shadow / nbs ) );
            sssimg[offset][0x01] *= ( 1.0f - ( shadow / nbs ) );
            sssimg[offset][0x02] *= ( 1.0f - ( shadow / nbs ) );
        } break;

        default :
        break;
    }
}

/******************************************************************************/
static void filtersoftshadows_free ( Q3DFILTER *fil ) {
    FILTERSOFTSHADOWS *fss = ( FILTERSOFTSHADOWS * ) fil->data;

    free ( fss );
}

/******************************************************************************/
static FILTERSOFTSHADOWS *filtersoftshadows_new ( ) {

    uint32_t structsize = sizeof ( FILTERSOFTSHADOWS );
    FILTERSOFTSHADOWS *fss = ( FILTERSOFTSHADOWS * ) calloc ( 0x01, structsize );

    if ( fss == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    return fss;
}

/******************************************************************************/
static uint32_t filtersoftshadows_draw ( Q3DFILTER     *fil, 
                                         Q3DJOB        *qjob,
                                         float          frameID,
                                         unsigned char *img, 
                                         uint32_t       from, 
                                         uint32_t       to, 
                                         uint32_t       depth, 
                                         uint32_t       width ) {
    uint32_t bytesperline = ( depth >> 0x03 ) * width;
    FILTERSOFTSHADOWS *fss = ( FILTERSOFTSHADOWS * ) fil->data;
    int i, j;

    if ( qjob->qarea.qzen.width && 
         qjob->qarea.qzen.height ) {
        uint32_t imgsize = qjob->qarea.qzen.width * 
                           qjob->qarea.qzen.height * ( depth / 8 );
        unsigned char *refimg = ( unsigned char * ) malloc ( imgsize );

        memcpy ( refimg, img, imgsize );

        if ( qjob->qarea.qssh ) {
            for ( i = from; i < to; i++ ) {
                uint32_t offset = ( i * width );

                for ( j = 0x00; j < width; j++ ) {
                    if ( qjob->qarea.qssh[offset+j].shadow ) {
                        sss ( qjob->qarea.qssh,
                              refimg,
                              img,
                              qjob->qarea.qzen.width,
                              qjob->qarea.qzen.height,
                              depth,
                              j,
                              i );
                    }
                }
            }
        }

        free ( refimg );
    }

    return 0x00;
}

/******************************************************************************/
Q3DFILTER *q3dfilter_softshadows_new ( ) {
    Q3DFILTER *fil;

    fil = q3dfilter_new ( FILTERIMAGE, 
                          SOFTSHADOWSFILTERNAME,
                          filtersoftshadows_draw,
                          filtersoftshadows_free,
                          filtersoftshadows_new ( ) );

    return fil;
}
