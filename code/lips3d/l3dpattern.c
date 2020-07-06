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
static int l3dpattern_defaultPaint ( L3DPATTERN *pattern,
                                     uint32_t    color,
                                     int32_t     size,
                                     float       pressure,
                                     int32_t     x,
                                     int32_t     y,
                                     char       *buffer,
                                     uint32_t    width,
                                     uint32_t    height,
                                     uint32_t    bpp,
                                     char       *mask,
                                     char       *zbuffer,
                                     uint32_t    engineFlags ) {
    int32_t rect[0x04];

    if ( size == 0x01 ) {
        l3dcore_paintPoint( pattern,
                            color,
                            size,
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
    } else {
        int32_t halfSize = size / 0x02;

        if ( ( size % 0x02 ) == 0x01 ) { /*** uneven size ***/
            rect[0x00] = x - halfSize;
            rect[0x01] = y - halfSize;
            rect[0x02] = x + halfSize;
            rect[0x03] = y + halfSize;

        } else { /*** even size ***/
            rect[0x00] = x - halfSize + 1;
            rect[0x01] = y - halfSize + 1;
            rect[0x02] = x + halfSize;
            rect[0x03] = y + halfSize;
        }

        l3core_paintRectangle ( pattern,
                                color,
                                size,
                                pressure,
                                rect[0x00],
                                rect[0x01],
                                rect[0x02],
                                rect[0x03],
                                buffer,
                                width,
                                height, 
                                bpp, 
                                mask,
                                zbuffer,
                                engineFlags );

        /*l3core_paintCircle ( pattern,
                                color,
                                size,
                                pressure,
                                x,
                                y,
                                buffer,
                                width,
                                height, 
                                bpp, 
                                mask,
                                zbuffer,
                                engineFlags );*/
    }

    return 0x00;
}

/******************************************************************************/
void l3dpattern_init ( L3DPATTERN *pattern ) {
    pattern->paint = l3dpattern_defaultPaint;
}

/******************************************************************************/
L3DPATTERN *l3dpattern_new ( ) {
    uint32_t structSize = sizeof ( L3DPATTERN );
    L3DPATTERN *pattern = ( L3DPATTERN * ) calloc ( 0x01, structSize );
 
    if ( pattern == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    l3dpattern_init ( pattern );

    return pattern;
}

