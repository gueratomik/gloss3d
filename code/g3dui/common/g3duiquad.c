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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
#include <g3dui.h>

/******************************************************************************/
void common_g3duiquad_divideSegments ( G3DUISEGMENT *seg, uint32_t xmid , 
                                                          uint32_t ymid,
                                                          uint32_t width, 
                                                          uint32_t height ) {
    uint32_t x1 = 0x00, y1 = 0x00,
             x2 = ( width  ) ? ( ( x1 + width  ) - 0x01 ) : 0x00,
             y2 = ( height ) ? ( ( y1 + height ) - 0x01 ) : 0x00;

    seg[0x00].x1 = 0x00 + MARGIN;
    seg[0x00].y1 = 0x00 + MARGIN;
    seg[0x00].x2 = xmid - MARGIN;
    seg[0x00].y2 = ymid - MARGIN;

    seg[0x01].x1 = xmid + MARGIN + 0x01;
    seg[0x01].y1 = 0x00 + MARGIN;
    seg[0x01].x2 = x2   - MARGIN;
    seg[0x01].y2 = ymid - MARGIN;

    seg[0x02].x1 = 0x00 + MARGIN;
    seg[0x02].y1 = ymid + MARGIN + 0x01;
    seg[0x02].x2 = xmid - MARGIN;
    seg[0x02].y2 = y2   - MARGIN;

    seg[0x03].x1 = xmid + MARGIN + 0x01;
    seg[0x03].y1 = ymid + MARGIN + 0x01;
    seg[0x03].x2 = x2   - MARGIN;
    seg[0x03].y2 = y2   - MARGIN;

    seg[0x04].x1 = seg[0x00].x1;
    seg[0x04].y1 = seg[0x00].y1;
    seg[0x04].x2 = seg[0x03].x2;
    seg[0x04].y2 = seg[0x03].y2;
}

/******************************************************************************/
void common_g3duiquad_resize ( G3DUIQUAD *quad, uint32_t width, 
                                                uint32_t height ) {
    uint32_t rw = ( width  * quad->Xratio ) >> 0x08,
             rh = ( height * quad->Yratio ) >> 0x08;

    common_g3duiquad_divideSegments ( quad->seg, rw, rh, width, height );
}

/******************************************************************************/
void common_g3duiquad_init ( G3DUIQUAD *quad, uint32_t width,
                                              uint32_t height ) {
    quad->Xratio = 0x80;
    quad->Yratio = 0x80;

    common_g3duiquad_resize ( quad, width, height );
}
