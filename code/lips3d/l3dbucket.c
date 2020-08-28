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
#include <lips3d/lips3d.h>

static int l3dbucket_reset ( L3DBUCKET *sel,
                               uint64_t     engine_flags );
static int l3dbucket_press ( L3DOBJECT     *obj,
                               L3DPATTERN    *pattern,
                               uint32_t       fgcolor,
                               uint32_t       bgcolor,
                               int32_t        x,
                               int32_t        y,
                               unsigned char *buffer, 
                               uint32_t       width, 
                               uint32_t       height,
                               uint32_t       bpp,
                               unsigned char *mask,
                               unsigned char *zbuffer,
                               uint32_t      *updcoord,
                               uint64_t engine_flags );
static int l3dbucket_move ( L3DOBJECT     *obj,
                              L3DPATTERN    *pattern,
                              uint32_t       fgcolor,
                              uint32_t       bgcolor,
                              int32_t        x,
                              int32_t        y,
                              unsigned char *buffer, 
                              uint32_t       width, 
                              uint32_t       height,
                              uint32_t       bpp,
                              unsigned char *mask,
                              unsigned char *zbuffer,
                              uint32_t      *updcoord,
                              uint64_t engine_flags );
static int l3dbucket_release ( L3DOBJECT     *obj,
                                 L3DPATTERN    *pattern,
                                 uint32_t       fgcolor,
                                 uint32_t       bgcolor,
                                 int32_t        x,
                                 int32_t        y,
                                 unsigned char *buffer, 
                                 uint32_t       width, 
                                 uint32_t       height,
                                 uint32_t       bpp,
                                 unsigned char *mask,
                                 unsigned char *zbuffer,
                                 uint32_t      *updcoord,
                                 uint64_t engine_flags );

/******************************************************************************/
/* https://www.thecrazyprogrammer.com/2017/02/flood-fill-algorithm-in-c.html */
/* I use the naive implementation because I'm too tired to find something */
/* better right now. Thw work on Gloss3D has been exhausting. */
static void floodFill ( int32_t        x,
                        int32_t        y,
                        unsigned char *buffer,
                        uint32_t       width,
                        uint32_t       height,
                        uint32_t       bpp,
                        unsigned char *mask,
                        unsigned char *zbuffer,
                        uint8_t        oldR,
                        uint8_t        oldG,
                        uint8_t        oldB,
                        uint8_t        newR,
                        uint8_t        newG,
                        uint8_t        newB,
                        uint8_t        tolerance ) {
    if ( ( x >= 0x00 ) && ( x < width  ) &&
         ( y >= 0x00 ) && ( y < height ) ) {
        uint32_t offset = ( y * width ) + x;

        if ( mask[offset] ) {
            if ( zbuffer[offset] == 0x00 ) {
                switch ( bpp ) {
                    case 0x18 : {
                        unsigned char (*buffer24)[0x03] = buffer;

                        if ( ( buffer24[offset][0x00] >= ( oldR - tolerance ) ) &&
                             ( buffer24[offset][0x00] <= ( oldR + tolerance ) ) &&
                             ( buffer24[offset][0x01] >= ( oldG - tolerance ) ) &&
                             ( buffer24[offset][0x01] <= ( oldG + tolerance ) ) &&
                             ( buffer24[offset][0x02] >= ( oldB - tolerance ) ) &&
                             ( buffer24[offset][0x02] <= ( oldB + tolerance ) ) ) {
                            buffer24[offset][0x00] = newR;
                            buffer24[offset][0x01] = newG;
                            buffer24[offset][0x02] = newB;

                            zbuffer[offset] = 0xFF;

                            floodFill ( x + 0x01, 
                                        y,
                                        buffer,
                                        width,
                                        height,
                                        bpp,
                                        mask,
                                        zbuffer,
                                        oldR, oldG, oldB,
                                        newR, newG, newB,
                                        tolerance );

                            floodFill ( x, 
                                        y + 0x01,
                                        buffer,
                                        width,
                                        height,
                                        bpp,
                                        mask,
                                        zbuffer,
                                        oldR, oldG, oldB,
                                        newR, newG, newB,
                                        tolerance );

                            floodFill ( x - 0x01, 
                                        y,
                                        buffer,
                                        width,
                                        height,
                                        bpp,
                                        mask,
                                        zbuffer,
                                        oldR, oldG, oldB,
                                        newR, newG, newB,
                                        tolerance );

                            floodFill ( x, 
                                        y - 0x01,
                                        buffer,
                                        width,
                                        height,
                                        bpp,
                                        mask,
                                        zbuffer,
                                        oldR, oldG, oldB,
                                        newR, newG, newB,
                                        tolerance );
	                    }
                    } break;

                    default :
                    break;
                }
            }
        }
    }
}

/******************************************************************************/
L3DBUCKET* l3dbucket_new ( ) {
    uint32_t structSize = sizeof ( L3DBUCKET );
    L3DBUCKET *bkt = ( L3DBUCKET * ) calloc ( 0x01, structSize );

    if ( bkt == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    l3dobject_init ( bkt,
                     l3dbucket_reset,
                     l3dbucket_press,
                     l3dbucket_move,
                     l3dbucket_release );

    return bkt;
}

/******************************************************************************/
static int l3dbucket_reset ( L3DBUCKET *bkt,
                             uint64_t   engine_flags ) {

    return 0x00;
}

/******************************************************************************/
static int l3dbucket_press ( L3DOBJECT     *obj,
                             L3DPATTERN    *pattern,
                             uint32_t       fgcolor,
                             uint32_t       bgcolor,
                             int32_t        x,
                             int32_t        y,
                             unsigned char *buffer, 
                             uint32_t       width, 
                             uint32_t       height,
                             uint32_t       bpp,
                             unsigned char *mask,
                             unsigned char *zbuffer,
                             uint32_t      *updcoord,
                             uint64_t       engine_flags ) {
    L3DBUCKET *bkt = ( L3DBUCKET * ) obj;

    if ( updcoord ) {
        updcoord[0x00] = 0x00;
        updcoord[0x01] = 0x00;
        updcoord[0x02] = width  - 1;
        updcoord[0x03] = height - 1;
    }

    return 0x00;
}

/******************************************************************************/
static int l3dbucket_move ( L3DOBJECT     *obj,
                            L3DPATTERN    *pattern,
                            uint32_t       fgcolor,
                            uint32_t       bgcolor,
                            int32_t        x,
                            int32_t        y,
                            unsigned char *buffer, 
                            uint32_t       width, 
                            uint32_t       height,
                            uint32_t       bpp,
                            unsigned char *mask,
                            unsigned char *zbuffer,
                            uint32_t      *updcoord,
                            uint64_t       engine_flags ) {
    L3DBUCKET *bkt = ( L3DBUCKET * ) obj;

    if ( updcoord ) {
        updcoord[0x00] = 0x00;
        updcoord[0x01] = 0x00;
        updcoord[0x02] = width  - 1;
        updcoord[0x03] = height - 1;
    }

    return 0x00;
}

/******************************************************************************/
static int l3dbucket_release ( L3DOBJECT     *obj,
                               L3DPATTERN    *pattern,
                               uint32_t       fgcolor,
                               uint32_t       bgcolor,
                               int32_t        x,
                               int32_t        y,
                               unsigned char *buffer, 
                               uint32_t       width, 
                               uint32_t       height,
                               uint32_t       bpp,
                               unsigned char *mask,
                               unsigned char *zbuffer,
                               uint32_t      *updcoord,
                               uint64_t       engine_flags ) {
    L3DBUCKET *bkt = ( L3DBUCKET * ) obj;

    memset ( zbuffer, 0x00, width * height );

    if ( ( x >= 0x00 ) && ( x < width  ) &&
         ( y >= 0x00 ) && ( y < height ) ) {
        uint32_t offset = ( y * width ) + x;

        switch ( bpp ) {
            case 0x18 : {
                unsigned char (*buffer24)[0x03] = buffer;
                uint8_t newR = ( fgcolor & 0x00FF0000 ) >> 0x10,
                        newG = ( fgcolor & 0x0000FF00 ) >> 0x08,
                        newB = ( fgcolor & 0x000000FF ) >> 0x00;

                floodFill ( x,
                            y,
                            buffer,
                            width,
                            height,
                            bpp,
                            mask,
                            zbuffer,
                            buffer24[offset][0x00],
                            buffer24[offset][0x01],
                            buffer24[offset][0x02],
                            newR,
                            newG,
                            newB,
                            bkt->tolerance );
            } break;

            default :
            break;
        }
    }

    if ( updcoord ) {
        updcoord[0x00] = 0x00;
        updcoord[0x01] = 0x00;
        updcoord[0x02] = width  - 1;
        updcoord[0x03] = height - 1;
    }

    return L3DUPDATEIMAGE;
}
