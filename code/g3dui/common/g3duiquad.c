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
#include <g3dui.h>

/******************************************************************************/
void g3duiquad_resize ( G3DUIQUAD *quad,
                        uint32_t   width, 
                        uint32_t   height ) {
    uint32_t x1 = 0x00, x2 = width  - 0x01,
             y1 = 0x00, y2 = height - 0x01;
    uint32_t mx = ( width  * quad->xratio ),
             my = ( height * quad->yratio );
    uint32_t i;

    quad->mx     = mx;
    quad->my     = my;
    quad->width  = width;
    quad->height = height;

    if ( ( mx > MARGIN ) && 
         ( my > MARGIN ) ) {
        quad->rect[0x00].x      = 0x00 + quad->margin;
        quad->rect[0x00].y      = 0x00 + quad->margin;
        quad->rect[0x00].width  = ( mx - quad->margin ) - quad->rect[0x00].x;
        quad->rect[0x00].height = ( my - quad->margin ) - quad->rect[0x00].y;

        quad->rect[0x01].x      = mx   + quad->margin;
        quad->rect[0x01].y      = 0x00 + quad->margin;
        quad->rect[0x01].width  = ( x2 - quad->margin ) - quad->rect[0x01].x;
        quad->rect[0x01].height = ( my - quad->margin ) - quad->rect[0x01].y;

        quad->rect[0x02].x      = mx   + quad->margin;
        quad->rect[0x02].y      = my   + quad->margin;
        quad->rect[0x02].width  = ( x2 - quad->margin ) - quad->rect[0x02].x;
        quad->rect[0x02].height = ( y2 - quad->margin ) - quad->rect[0x02].y;

        quad->rect[0x03].x      = 0x00 + quad->margin;
        quad->rect[0x03].y      = my   + quad->margin;
        quad->rect[0x03].width  = ( mx - quad->margin ) - quad->rect[0x03].x;
        quad->rect[0x03].height = ( y2 - quad->margin ) - quad->rect[0x03].y;
    }
}

/******************************************************************************/
void g3duiquad_init ( G3DUIQUAD *quad ) {
    quad->xratio = 0.5f;
    quad->yratio = 0.5f;
    quad->margin = 0x02;

    g3duiquad_resize ( quad, 0x00, 0x00 );
}
