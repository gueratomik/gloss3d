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
    uint32_t maxItems = 0xFFFF;
    int32_t (*stack)[0x02] = calloc ( 0x01, sizeof ( int32_t ) * 2 * maxItems );

    if ( ( x >= 0x00 ) && ( x < width  ) &&
         ( y >= 0x00 ) && ( y < height ) ) {
        uint32_t offset = ( y * width ) + x;
        uint32_t nbItems = 0x00;
        uint32_t curItem = 0x00;
        uint32_t i;

        if ( mask[offset] ) {
            stack[nbItems][0x00] = x;
            stack[nbItems][0x01] = y;

            nbItems++;

            zbuffer[offset] = 0xFF;
        }

        while ( curItem < nbItems ) {
            int32_t sx = stack[curItem][0x00],
                    sy = stack[curItem][0x01];
            int32_t pos[0x04][0x02] = {{ sx + 1, sy     },
                                       { sx    , sy + 1 },
                                       { sx - 1, sy     },
                                       { sx    , sy - 1 }};

            for ( i = 0x00; i < 0x04; i++ ) {
                int32_t nx = pos[i][0x00],
                        ny = pos[i][0x01];

                if ( ( nx >= 0x00 ) && ( nx < width  ) &&
                     ( ny >= 0x00 ) && ( ny < height ) ) {
                    uint32_t noffset = ( ny * width ) + nx;

                    if ( mask[noffset] ) {
                        if ( zbuffer[noffset] == 0x00 ) {
                            switch ( bpp ) {
                                case 0x18 : {
                                    unsigned char (*buffer24)[0x03] = buffer;

                                    if ( ( buffer24[noffset][0x00] >= ( oldR - tolerance ) ) &&
                                         ( buffer24[noffset][0x00] <= ( oldR + tolerance ) ) &&
                                         ( buffer24[noffset][0x01] >= ( oldG - tolerance ) ) &&
                                         ( buffer24[noffset][0x01] <= ( oldG + tolerance ) ) &&
                                         ( buffer24[noffset][0x02] >= ( oldB - tolerance ) ) &&
                                         ( buffer24[noffset][0x02] <= ( oldB + tolerance ) ) ) {

                                        stack[nbItems][0x00] = nx;
                                        stack[nbItems][0x01] = ny;

                                        nbItems++;

                                        zbuffer[noffset] = 0xFF;

                                        if ( nbItems == maxItems ) {
                                            maxItems += 0xFFFF;

                                            stack = realloc ( stack, sizeof ( int32_t ) * 2 * maxItems );
                                        }
                                    }
                                } break;

                                default : 
                                break;
                            }
                        }
                    }
                }
            }

            curItem++;
        }

        /*** Note: the 2nd step is not necessary and the whole algo could ***/
        /*** fit in one step. this is just for better code clarity ***/
        for ( i = 0x00; i < nbItems; i++ ) {
            int32_t sx = stack[i][0x00],
                    sy = stack[i][0x01];
            uint32_t soffset = ( sy * width ) + sx;

            switch ( bpp ) {
                case 0x18 : {
                    unsigned char (*buffer24)[0x03] = buffer;

                    buffer24[soffset][0x00] = newR;
                    buffer24[soffset][0x01] = newG;
                    buffer24[soffset][0x02] = newB;
                } break;

                default :
                break;
            }
        }
    }

    free ( stack );
}

/******************************************************************************/
L3DBUCKET* l3dbucket_new ( ) {
    uint32_t structSize = sizeof ( L3DBUCKET );
    L3DBUCKET *bkt = ( L3DBUCKET * ) calloc ( 0x01, structSize );

    if ( bkt == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    bkt->tolerance = 0x04;

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
