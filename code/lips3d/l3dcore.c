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

/******************************************************************************/
int l3dcore_paintPoint ( L3DPATTERN    *pattern,
                         uint32_t       color,
                         float          pressure,
                         int32_t        x,
                         int32_t        y,
                         unsigned char *buffer,
                         uint32_t       width,
                         uint32_t       height,
                         uint32_t       bpp,
                         unsigned char *mask,
                         unsigned char *zbuffer,
                         uint32_t       engineFlags ) {
    if ( ( x > 0x00 ) && ( x < width  ) &&
         ( y > 0x00 ) && ( y < height ) ) {
        uint8_t A = ( color & 0xFF000000 ) >> 24;
        uint8_t R = ( color & 0x00FF0000 ) >> 16;
        uint8_t G = ( color & 0x0000FF00 ) >>  8;
        uint8_t B = ( color & 0x000000FF ) >>  0;
        uint32_t offset = ( y * width ) + x;

        if ( zbuffer[offset] == 0x00 ) {
            switch ( bpp ) {
                case 0x20 :
                break;

                case 0x18 : {
                    unsigned char (*buffer24)[0x03] = buffer;

                    buffer24[offset][0x00] = R;
                    buffer24[offset][0x01] = G;
                    buffer24[offset][0x02] = B;
                } break;

                default :
                    fprintf ( stderr, "Unsupported depth\n");
                break;
            }

            zbuffer[offset] = 0xFF;
        }
    }

    return 0x00;
}

/******************************************************************************/
int l3core_paintCircle ( L3DPATTERN    *pattern,
                         uint32_t       color,
                         float          pressure,
                         int32_t        xp,
                         int32_t        yp,
                         unsigned char *buffer,
                         uint32_t       width,
                         uint32_t       height, 
                         uint32_t       bpp, 
                         unsigned char *mask,
                         unsigned char *zbuffer,
                         uint32_t    engineFlags ) {
    uint8_t A = ( color & 0xFF000000 ) >> 24;
    uint8_t R = ( color & 0x00FF0000 ) >> 16;
    uint8_t G = ( color & 0x0000FF00 ) >>  8;
    uint8_t B = ( color & 0x000000FF ) >>  0;
    int32_t radius = pattern->size / 0x02;
    int32_t x, y, rdiff = ( radius * radius );
    float invPressure = 1.0f - pressure;
    int32_t x1 = xp - radius, x2 = xp + radius;
    int32_t y1 = yp - radius, y2 = yp + radius;

    for ( y = y1; y <= y2; y++ ) {
        int32_t ydiff = ( y - yp )  * ( y - yp );

        for ( x = x1; x <= x2; x++ ) {
            int32_t xdiff = ( x - xp ) * ( x - xp );

            if ( ( x > 0x00 ) && ( x < width  ) &&
                 ( y > 0x00 ) && ( y < height ) ) {
                uint32_t offset = ( y * width ) + x;

                if ( ( xdiff + ydiff ) <= rdiff ) {
                    if ( zbuffer[offset] == 0x00 ) {
                        switch ( bpp ) {
                            case 0x20 :
                            break;

                            case 0x18 : {
                                unsigned char (*buffer24)[0x03] = buffer;

                                buffer24[offset][0x00] = ( R * pressure ) + ( buffer24[offset][0x00] * invPressure ) ;
                                buffer24[offset][0x01] = ( G * pressure ) + ( buffer24[offset][0x01] * invPressure ) ;
                                buffer24[offset][0x02] = ( B * pressure ) + ( buffer24[offset][0x02] * invPressure ) ;
                            } break;

                            default :
                                fprintf ( stderr, "Unsupported depth\n");
                            break;
                        }

                        zbuffer[offset] = 0xFF;
                    }
                }
            }
        }
    }
}

/******************************************************************************/
int l3core_paintRectangle ( L3DPATTERN    *pattern,
                            uint32_t       color,
                            float          pressure,
                            int32_t        x1,
                            int32_t        y1,
                            int32_t        x2,
                            int32_t        y2,
                            unsigned char *buffer,
                            uint32_t       width,
                            uint32_t       height, 
                            uint32_t       bpp, 
                            unsigned char *mask,
                            unsigned char *zbuffer,
                            uint32_t    engineFlags ) {
    uint8_t A = ( color & 0xFF000000 ) >> 24;
    uint8_t R = ( color & 0x00FF0000 ) >> 16;
    uint8_t G = ( color & 0x0000FF00 ) >>  8;
    uint8_t B = ( color & 0x000000FF ) >>  0;
    int32_t x, y;

    for ( y = y1; y <= y2; y++ ) {
        for ( x = x1; x <= x2; x++ ) {
            if ( ( x > 0x00 ) && ( x < width  ) &&
                 ( y > 0x00 ) && ( y < height ) ) {
                uint32_t offset = ( y * width ) + x;

                if ( zbuffer[offset] == 0x00 ) {
                    switch ( bpp ) {
                        case 0x20 :
                        break;

                        case 0x18 : {
                            unsigned char (*buffer24)[0x03] = buffer;

                            buffer24[offset][0x00] = R;
                            buffer24[offset][0x01] = G;
                            buffer24[offset][0x02] = B;
                        } break;

                        default :
                            fprintf ( stderr, "Unsupported depth\n");
                        break;
                    }

                    zbuffer[offset] = 0xFF;
                }
            }
        }
    }
}

/******************************************************************************/
int l3core_paintLine ( L3DPATTERN    *pattern,
                       uint32_t       color,
                       float          pressure,
                       int32_t        x1,
                       int32_t        y1,
                       int32_t        x2,
                       int32_t        y2,
                       unsigned char *buffer,
                       uint32_t       width,
                       uint32_t       height, 
                       uint32_t       bpp, 
                       unsigned char *mask,
                       unsigned char *zbuffer,
                       uint32_t       engineFlags ) {

    int32_t dx  = ( x2 - x1 ),
            ddx = ( dx == 0x00 ) ? 0x01 : abs ( dx ),
            dy  = ( y2 - y1 ),
            ddy = ( dy == 0x00 ) ? 0x01 : abs ( dy ),
            dd  = ( ddx > ddy ) ? ddx : ddy;
    int px = ( dx > 0x00 ) ? 1 : -1, 
        py = ( dy > 0x00 ) ? 1 : -1;
    int32_t x = x1, 
            y = y1;
    int i, cumul = 0x00;

    if ( ddx > ddy ) {
        for ( i = 0x00; i <= ddx; i++ ) {
            if ( cumul >= ddx ) {
                cumul -= ddx;
                y     += py;
            }

            l3dpattern_paint ( pattern,
                               color,
                               pressure,
                               x,
                               y,
                               buffer,
                               width,
                               height,
                               bpp,
                               mask,
                               zbuffer,
                               engineFlags );

            cumul += ddy;
            x     += px;
        }
    } else {
        for ( i = 0x00; i <= ddy; i++ ) {
            if ( cumul >= ddy ) {
                cumul -= ddy;
                x     += px;
            }

            l3dpattern_paint ( pattern,
                               color,
                               pressure,
                               x,
                               y,
                               buffer,
                               width,
                               height,
                               bpp,
                               mask,
                               zbuffer,
                               engineFlags );

            cumul += ddx;
            y     += py;
        }
    }

    return 0x00;
}
