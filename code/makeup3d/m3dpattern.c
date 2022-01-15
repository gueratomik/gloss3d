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
#include <makeup3d/makeup3d.h>

/******************************************************************************/
int m3dpattern_paint ( M3DPATTERN    *pattern,
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
                       uint64_t       engine_flags ) {
    int32_t x1, x2, y1, y2, xm, ym, xp, yp;
    uint8_t A = ( color & 0xFF000000 ) >> 24;
    uint8_t R = ( color & 0x00FF0000 ) >> 16;
    uint8_t G = ( color & 0x0000FF00 ) >>  8;
    uint8_t B = ( color & 0x000000FF ) >>  0;

    if ( pattern->size == 0x01 ) {
        m3dcore_paintPoint( pattern,
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
                            engine_flags );
    } else {
        int32_t halfSize = pattern->size / 0x02;

        if ( ( pattern->size % 0x02 ) == 0x01 ) { /*** uneven size ***/
            x1 = x - halfSize;
            y1 = y - halfSize;
            x2 = x + halfSize;
            y2 = y + halfSize;

        } else { /*** even size ***/
            x1 = x - halfSize + 1;
            y1 = y - halfSize + 1;
            x2 = x + halfSize;
            y2 = y + halfSize;
        }

        for ( ym = y1, yp = 0x00; ym <= y2; ym++, yp++ ) {
            for ( xm = x1, xp = 0x00; xm <= x2; xm++, xp++ ) {

                if ( ( xm > 0x00 ) && ( xm < width  ) &&
                     ( ym > 0x00 ) && ( ym < height ) ) {
                    /*** image buffer offset ***/
                    uint32_t boffset = ( ym * width         ) + xm;
                    /*** pattern offset ***/
                    uint32_t poffset = ( yp * pattern->size ) + xp;

                    if ( mask[boffset] ) {
                        if ( zbuffer[boffset] < pattern->buffer[poffset] ) {
                            unsigned char maxDiff = 0xFF - zbuffer[boffset];
                            unsigned char diff = pattern->buffer[poffset] - zbuffer[boffset];

                            if ( pattern->buffer[poffset] > 0x00 ) {
                                float patternPressure = ( ( float ) pattern->buffer[poffset] / 255 ) * ( 1.0f - ( ( float ) zbuffer[boffset] / pattern->buffer[poffset] ) );
                                float P = pressure * patternPressure;
                                float invP = 1.0f - P;

                                switch ( bpp ) {
                                    case 0x20 :
                                    break;

                                    case 0x18 : {
                                        unsigned char (*b24)[0x03] = buffer;
                                        unsigned char BR = b24[boffset][0x00],
                                                      BG = b24[boffset][0x01],
                                                      BB = b24[boffset][0x02];

                                            b24[boffset][0x00] = ( R * P ) + ( BR * invP );
                                            b24[boffset][0x01] = ( G * P ) + ( BG * invP );
                                            b24[boffset][0x02] = ( B * P ) + ( BB * invP );
    /*if ( ( xm == x ) && ( ym == y ) ) {
        printf ( "P:%f Pat:%d Z:%d\n", P, pattern->buffer[poffset], zbuffer[boffset] );
        printf ( "%d %d %d\n", b24[boffset][0x00], 
                               b24[boffset][0x01],
                               b24[boffset][0x02]);
    }*/
                                    } break;

                                    default :
                                        fprintf ( stderr, "Unsupported depth\n");
                                    break;
                                }

                                zbuffer[boffset] = pattern->buffer[poffset];
                            }
                        }
                    }
                }
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
void m3dpattern_generatePlainRectangle ( M3DPATTERN *pattern ) {
    memset ( pattern->buffer, 0xFF, pattern->size * pattern->size );
}

/******************************************************************************/
void m3dpattern_generatePlainCircle ( M3DPATTERN *pattern ) {
    uint32_t radius = pattern->size / 0x02;
    uint32_t xp = radius, yp = radius;
    uint32_t x, y, rdiff = ( radius * radius );
    uint32_t x1 = xp - radius, x2 = xp + radius;
    uint32_t y1 = yp - radius, y2 = yp + radius;

    for ( y = 0x00; y < pattern->size; y++ ) {
        int32_t ydiff = ( y - yp )  * ( y - yp );

        for ( x = 0x00; x < pattern->size; x++ ) {
            int32_t xdiff = ( x - xp ) * ( x - xp );

            uint32_t offset = ( y * pattern->size ) + x;

            if ( ( xdiff + ydiff ) <= rdiff ) {
                pattern->buffer[offset] = 0xFF;
            }
        }
    }
}

/******************************************************************************/
void m3dpattern_generateBrush ( M3DPATTERN *pattern ) {
    M3DBRUSHPATTERN *bp = ( M3DBRUSHPATTERN * ) pattern;
    float hratio = ( float ) bp->brush->height / pattern->size,
          wratio = ( float ) bp->brush->width  / pattern->size;
    uint32_t i, j;

    for ( i = 0x00; i < pattern->size; i++ ) {
        uint32_t bi = i * hratio;

        for ( j = 0x00; j < pattern->size; j++ ) {
            uint32_t bj = j * wratio;
            uint32_t poffset = (  i * pattern->size    ) +  j;
            uint32_t boffset = ( bi * bp->brush->width ) + bj;

            switch ( bp->brush->bytes_per_pixel ) {
                case 0x03 : {
                    unsigned char (*bdata)[0x03] = bp->brush->pixel_data;

                    pattern->buffer[poffset] = ( bdata[boffset][0x00] + 
                                                 bdata[boffset][0x01] + 
                                                 bdata[boffset][0x02] ) / 0x03;
                } break;

                default :
                break;
            }
        }
    }
}

/******************************************************************************/
void m3dpattern_generateFadedCircle ( M3DPATTERN *pattern ) {
    M3DFADEDCIRCLEPATTERN *fcp = ( M3DFADEDCIRCLEPATTERN * ) pattern;
    uint32_t rad = pattern->size / 0x02;
    uint32_t xp = rad, yp = rad;
    uint32_t x, y, rdiff = ( rad * rad ) * fcp->radius;
    uint32_t x1 = 0x00, x2 = pattern->size;
    uint32_t y1 = 0x00, y2 = pattern->size;

    for ( y = 0x00; y < pattern->size; y++ ) {
        int32_t ydiff = ( y - yp )  * ( y - yp );

        for ( x = 0x00; x < pattern->size; x++ ) {
            int32_t xdiff = ( x - xp ) * ( x - xp );

            uint32_t offset = ( y * pattern->size ) + x;

            if ( ( xdiff + ydiff ) <= rdiff ) {
                float fade = ( 1.0f - sqrt ( xdiff + 
                                             ydiff ) / rad );

                if ( fcp->fullPartRate ) fade /= fcp->fullPartRate;

                if ( fade > 1.0f ) {
                    pattern->buffer[offset] = 0xFF;
                } else {
                    pattern->buffer[offset] = 0xFF * fade;
                }
            }
        }
    }
}

/******************************************************************************/
void m3dpattern_resize ( M3DPATTERN *pattern, uint32_t size ) {
    if ( size ) {
        pattern->size = size;

        pattern->buffer = realloc ( pattern->buffer, pattern->size * 
                                                     pattern->size );

        memset ( pattern->buffer, 0x00, pattern->size * pattern->size );

        pattern->generate ( pattern );
    }
}

/******************************************************************************/
void m3dpattern_init ( M3DPATTERN *pattern,
                       uint32_t    size,
                       void(*generate)(M3DPATTERN*)) {
    pattern->generate = generate;

    m3dpattern_resize ( ( M3DPATTERN * ) pattern, size );
}

/******************************************************************************/
M3DPLAINRECTANGLEPATTERN *m3dplainrectanglepattern_new ( uint32_t size ) {
    uint32_t structSize = sizeof ( M3DPLAINRECTANGLEPATTERN );
    M3DPLAINRECTANGLEPATTERN *prp;

    prp = ( M3DPLAINRECTANGLEPATTERN * ) calloc ( 0x01, structSize );

    if ( prp == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    m3dpattern_init ( ( M3DPATTERN * ) prp, 
                                       size, 
                                       m3dpattern_generatePlainRectangle );


    return prp;
}

/******************************************************************************/
M3DPLAINCIRCLEPATTERN *m3dplaincirclepattern_new ( uint32_t size ) {
    uint32_t structSize = sizeof ( M3DPLAINCIRCLEPATTERN );
    M3DPLAINCIRCLEPATTERN *pcp;

    pcp = ( M3DPLAINCIRCLEPATTERN * ) calloc ( 0x01, structSize );

    if ( pcp == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    m3dpattern_init ( ( M3DPATTERN * ) pcp, 
                                       size, 
                                       m3dpattern_generatePlainCircle );


    return pcp;
}

/******************************************************************************/
M3DFADEDCIRCLEPATTERN *m3dfadedcirclepattern_new ( uint32_t size, 
                                                   float    radius,
                                                   float    fullPartRate ) {
    uint32_t structSize = sizeof ( M3DFADEDCIRCLEPATTERN );
    M3DFADEDCIRCLEPATTERN *fcp;

    fcp = ( M3DFADEDCIRCLEPATTERN * ) calloc ( 0x01, structSize );

    if ( fcp == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    fcp->radius = radius;
    fcp->fullPartRate = fullPartRate;

    m3dpattern_init ( ( M3DPATTERN * ) fcp, 
                                       size, 
                                       m3dpattern_generateFadedCircle );


    return fcp;
}

/******************************************************************************/
M3DBRUSHPATTERN *m3dbrushpattern_new ( uint32_t      size, 
                                       M3DGIMPBRUSH *brush ) {
    uint32_t structSize = sizeof ( M3DBRUSHPATTERN );
    M3DBRUSHPATTERN *bp = ( M3DBRUSHPATTERN * ) calloc ( 0x01, structSize );

    if ( bp == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    bp->brush = brush;

    m3dpattern_init ( ( M3DPATTERN * ) bp, 
                                       size, 
                                       m3dpattern_generateBrush );


    return bp;
}
