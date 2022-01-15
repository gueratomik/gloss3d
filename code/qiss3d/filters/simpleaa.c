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
typedef struct _FILTERSIMPLEAA {
    uint32_t dummy;
} FILTERSIMPLEAA;

/******************************************************************************/
static void saa ( unsigned char *srcimg,
                  unsigned char *dstimg,
                  uint32_t       width,
                  uint32_t       height,
                  uint32_t       depth,
                  uint32_t       x,
                  uint32_t       y ) {
    static int32_t block[0x08][0x02] = { { -1,  1 }, {  0,  1 }, {  1,  1 },
                                         { -1,  0 },             {  1,  0 },
                                         { -1, -1 }, {  0, -1 }, {  1, -1 } };


    uint32_t offset = ( y * width ) + x;

    switch ( depth ) {
        case 0x18 :
        case 0x20 : {
            unsigned char (*refimg)[0x03] = ( unsigned char (*)[0x03]) srcimg,
                          (*saaimg)[0x03] = ( unsigned char (*)[0x03]) dstimg;
            /*** the pixel weighs x times more than adjacent pixels ***/
            uint32_t weight = 0x01;
            uint32_t dstR = refimg[offset][0x00] * weight, 
                     dstG = refimg[offset][0x01] * weight,
                     dstB = refimg[offset][0x02] * weight;
            uint32_t nbs = weight;
            uint32_t i;

            for ( i = 0x00; i < 0x08; i++ ) {
                int32_t tx = ( int32_t ) x + block[i][0x00],
                        ty = ( int32_t ) y + block[i][0x01];

                if ( ( tx > 0x00 ) && ( tx < width  ) &&
                     ( ty > 0x00 ) && ( ty < height ) ) {
                    uint32_t toffset = ( ty * width ) + tx;

                    dstR += refimg[toffset][0x00];
                    dstG += refimg[toffset][0x01];
                    dstB += refimg[toffset][0x02];

                    nbs++;
                }
            }

            saaimg[offset][0x00] = dstR / nbs;
            saaimg[offset][0x01] = dstG / nbs;
            saaimg[offset][0x02] = dstB / nbs;
        } break;

        default :
        break;
    }
}

/******************************************************************************/
static void filtersimpleaa_free ( Q3DFILTER *fil ) {
    FILTERSIMPLEAA *fsa = ( FILTERSIMPLEAA * ) fil->data;


    free ( fsa );
}

/******************************************************************************/
static FILTERSIMPLEAA *filtersimpleaa_new ( ) {

    uint32_t structsize = sizeof ( FILTERSIMPLEAA );
    FILTERSIMPLEAA *fsa = ( FILTERSIMPLEAA * ) calloc ( 0x01, structsize );

    if ( fsa == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    return fsa;
}

/******************************************************************************/
static uint32_t filtersimpleaa_draw ( Q3DFILTER     *fil, 
                                      Q3DJOB        *qjob,
                                      uint32_t       cpuID,
                                      float          frameID,
                                      unsigned char *img, 
                                      uint32_t       from, 
                                      uint32_t       to, 
                                      uint32_t       depth, 
                                      uint32_t       width ) {
    uint32_t bytesperline = ( depth >> 0x03 ) * width;
    FILTERSIMPLEAA *fsa = ( FILTERSIMPLEAA * ) fil->data;
    int i, j;

    if ( qjob->qarea.qzen.width && 
         qjob->qarea.qzen.height ) {
        uint32_t imgsize = qjob->qarea.qzen.width * 
                           qjob->qarea.qzen.height * ( depth / 8 );
        unsigned char *refimg = ( unsigned char * ) malloc ( imgsize );

        memcpy ( refimg, img, imgsize );

        for ( i = from; i < to; i++ ) {
            uint32_t offset = ( i * width );

            for ( j = 0x00; j < width; j++ ) {
                if (  q3dzengine_isOutline ( &qjob->qarea.qzen,
                                              j,
                                              i ) ) {
                    saa ( refimg,
                          img,
                          qjob->qarea.qzen.width,
                          qjob->qarea.qzen.height,
                          depth,
                          j,
                          i );
                }
            }
        }

        free ( refimg );
    }

    return 0x00;
}

/******************************************************************************/
Q3DFILTER *q3dfilter_simpleaa_new ( ) {
    Q3DFILTER *fil;

    fil = q3dfilter_new ( FILTERIMAGE, 
                          SIMPLEAAFILTERNAME,
                          filtersimpleaa_draw,
                          filtersimpleaa_free,
                          filtersimpleaa_new ( ) );

    return fil;
}
