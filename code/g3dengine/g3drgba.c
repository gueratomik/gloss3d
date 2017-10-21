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
#include <g3dengine/g3dengine.h>

/******************************************************************************/
uint32_t g3drgba_toLong ( G3DRGBA *rgba ) {
    return ( rgba->r << 0x10 ) | ( rgba->g << 0x08 ) | rgba->b;
}

/******************************************************************************/
void g3drgba_fromLong ( G3DRGBA *rgba, uint32_t color ) {
    rgba->r = ( color >> 0x10 ) & 0x000000FF;
    rgba->g = ( color >> 0x08 ) & 0x000000FF;
    rgba->b = ( color         ) & 0x000000FF;
}

/******************************************************************************/
void g3drgba_init ( G3DRGBA *rgb, uint32_t r, 
                                  uint32_t g,
                                  uint32_t b, 
                                  uint32_t a ) {
    rgb->r = r;
    rgb->g = g;
    rgb->b = b;
    rgb->a = a;
}

/******************************************************************************/
void g3drgba_toColor ( G3DRGBA *rgba, G3DCOLOR *color ) {
    color->r = rgba->r / 255.0f;
    color->g = rgba->g / 255.0f;
    color->b = rgba->b / 255.0f;
    color->a = rgba->a / 255.0f;
}

/******************************************************************************/
void g3dcolor_toRGBA ( G3DCOLOR *color, G3DRGBA *rgba ) {
    rgba->r = color->r * 255.0f;
    rgba->g = color->g * 255.0f;
    rgba->b = color->b * 255.0f;
    rgba->a = color->a * 255.0f;
}
