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
#include <g3dengine/g3dengine.h>

/******************************************************************************/
void g3dprocedural_getNormal ( G3DPROCEDURAL *proc,
                               float          u,
                               float          v,
                               G3DVECTOR     *nor,
                               float          prec ) {
/* Patterns for our vector depending on were we are on the image, to prevent:
   a forbidden memory access.

  0 for upper left, 1 for upper right, 2 for lower left, 3 for lower right.

0 = **    1 = **
    *          *

2 = *     3 =  *
    **        **
*/
    G3DVECTOR pattern[0x04][0x03] = { { { .x =  0.0f, .y =  0.0f },
                                        { .x =  prec, .y =  0.0f },
                                        { .x =  0.0f, .y =  prec } },
                                      { { .x =  0.0f, .y =  0.0f },
                                        { .x =  0.0f, .y =  prec },
                                        { .x = -prec, .y =  0.0f } },
                                      { { .x =  0.0f, .y =  0.0f },
                                        { .x =  0.0f, .y = -prec },
                                        { .x =  prec, .y =  0.0f } },
                                      { { .x =  0.0f, .y =  0.0f },
                                        { .x = -prec, .y =  0.0f },
                                        { .x =  0.0f, .y = -prec } } };
    float bu = ( u > 0.0f ) ? u - (int) u : 1.0f - ( u - (int) u ),
          bv = ( v > 0.0f ) ? v - (int) v : 1.0f - ( v - (int) v );
    uint32_t m = (bv*2);
    uint32_t n = (bu*2);
    /*** we divide the image in 4 areas to pick the right pattern ***/
    uint32_t patidx = ( m * 0x02 ) + n;
    G3DVECTOR *curpat = pattern[patidx];
    G3DVECTOR pt[0x03] = { { .x = bu + curpat[0].x, 
                             .y = bv + curpat[0].y },
                           { .x = bu + curpat[1].x,
                             .y = bv + curpat[1].y },
                           { .x = bu + curpat[2].x, 
                             .y = bv + curpat[2].y } };
    G3DCOLOR color0, color1, color2;
    G3DRGBA rgba0, rgba1, rgba2;
    int32_t AVG0, AVG1, AVG2;
    G3DVECTOR vec[0x02];


    proc->getColor ( proc, pt[0x00].x, 
                           pt[0x00].y, 0.0f, &color0 );

    proc->getColor ( proc, pt[0x01].x, 
                           pt[0x01].y, 0.0f, &color1 );

    proc->getColor ( proc, pt[0x02].x, 
                           pt[0x02].y, 0.0f, &color2 );

    g3dcolor_toRGBA ( &color0, &rgba0 );
    g3dcolor_toRGBA ( &color1, &rgba1 );
    g3dcolor_toRGBA ( &color2, &rgba2 );

    AVG0 = ( rgba0.r + rgba0.g + rgba0.b ) / 3;
    AVG1 = ( rgba1.r + rgba1.g + rgba1.b ) / 3;
    AVG2 = ( rgba2.r + rgba2.g + rgba2.b ) / 3;

    vec[0x00].x = ( float ) curpat[0x01].x;
    vec[0x00].y = ( float ) curpat[0x01].y;
    vec[0x00].z = ( float ) ( AVG1 - AVG0 ) / 255.0f;

    vec[0x01].x = ( float ) curpat[0x02].x;
    vec[0x01].y = ( float ) curpat[0x02].y;
    /*** Note: negated to use blacks as creases ***/
    vec[0x01].z = ( float ) ( AVG2 - AVG0 ) / 255.0f;

    g3dvector_cross ( &vec[0x00], &vec[0x01], nor );

    g3dvector_normalize ( nor, NULL );
}

/******************************************************************************/
void g3dprocedural_init ( G3DPROCEDURAL *proc, 
                          uint32_t       type,
                          void         (*func)( G3DPROCEDURAL *, 
                                                double, 
                                                double, 
                                                double, 
                                                G3DCOLOR * ) ) {
    proc->type       = type;
    proc->getColor   = func;

    glGenTextures ( 0x01, &proc->image.id );
}

/******************************************************************************/
void g3dprocedural_fill ( G3DPROCEDURAL *proc, uint32_t resx,
                                               uint32_t resy,
                                               uint32_t bpp,
                                               uint32_t bindGL ) {
    uint32_t i, j;

    proc->image.width  = resx;
    proc->image.height = resy;
    proc->image.bytesPerPixel = bpp >> 0x03;
    proc->image.bytesPerLine = proc->image.bytesPerPixel * proc->image.width;
    proc->image.wratio = 1;
    proc->image.hratio = 1;
    proc->image.data   = realloc ( proc->image.data, resx * 
                                                     resy * ( bpp >> 0x03 ) );

    for ( j = 0x00; j < proc->image.height; j++ ) {
        for ( i = 0x00; i < proc->image.width; i++ ) {
            uint32_t offset = ( j * proc->image.width ) + i;
            G3DCOLOR color;
            G3DRGBA rgba;

            proc->getColor ( proc, (double) i / proc->image.width, 
                                   (double) j / proc->image.height,
                                   (double) 0, &color );

            g3dcolor_toRGBA ( &color, &rgba );

            switch ( bpp ) {
                case 0x18 : {
                    unsigned char (*data)[0x03] = (unsigned char (*)[3])proc->image.data;

                    data[offset][0x00] = rgba.r;
                    data[offset][0x01] = rgba.g;
                    data[offset][0x02] = rgba.b;
                } break;

                case 0x08 : {
                    unsigned char (*data)[0x01] = (unsigned char (*)[1])proc->image.data;
                    uint32_t col = ( rgba.r + rgba.g + rgba.b ) / 0x03;

                    data[offset][0x00] = col;
                } break;

                default:
                break;
            }
        }
    }

    if ( bindGL ) g3dimage_bind ( &proc->image );
}
