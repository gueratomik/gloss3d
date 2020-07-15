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

static int l3dbasepen_init ( L3DOBJECT     *obj,
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
                             uint32_t       engineFlags );
static int l3dbasepen_paint ( L3DOBJECT     *obj,
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
                              int32_t       *updx,
                              int32_t       *updy,
                              int32_t       *updw,
                              int32_t       *updh,
                              uint32_t       engineFlags );
static int l3dbasepen_done ( L3DOBJECT     *obj,
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
                             uint32_t       engineFlags );

/******************************************************************************/
L3DBASEPEN* l3dbasepen_new ( ) {
    uint32_t structSize = sizeof ( L3DBASEPEN );
    L3DBASEPEN *basepen = ( L3DBASEPEN * ) calloc ( 0x01, structSize );

    if ( basepen == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    l3dobject_init ( basepen,
                     l3dplainrectanglepattern_new ( 0x10 ),
                     1.0f,
                     l3dbasepen_init,
                     l3dbasepen_paint,
                     l3dbasepen_done );

    return basepen;
}

/******************************************************************************/
static int l3dbasepen_init ( L3DOBJECT     *obj,
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
                             uint32_t       engineFlags ) {
    L3DBASEPEN *basepen = ( L3DBASEPEN * ) obj;

    basepen->oldx = x;
    basepen->oldy = y;

    memset ( zbuffer, 0x00, width * height );

    return 0x00;
}

/******************************************************************************/
static int l3dbasepen_paint ( L3DOBJECT     *obj,
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
                              int32_t       *updx,
                              int32_t       *updy,
                              int32_t       *updw,
                              int32_t       *updh,
                              uint32_t       engineFlags ) {
    /*y = 100;*/
    L3DBASEPEN *basepen = ( L3DBASEPEN * ) obj;
    uint32_t size = obj->pattern->size, half = size / 0x02;
    /*** we use some margin because fro some unknown reason, glTexSubImage2D ***/
    /*** does not seem to copy the subimage completely ***/
    uint32_t margin = 10; 
    int32_t x1 = ( x < basepen->oldx ) ? x - half : basepen->oldx - half - margin,
            y1 = ( y < basepen->oldy ) ? y - half : basepen->oldy - half - margin,
            x2 = ( x > basepen->oldx ) ? x + half : basepen->oldx + half + margin,
            y2 = ( y > basepen->oldy ) ? y + half : basepen->oldy + half + margin;

    if ( updx ) (*updx) = x1;
    if ( updy ) (*updy) = y1;
    if ( updw ) (*updw) = ( x2 - x1 ) + 1;
    if ( updh ) (*updh) = ( y2 - y1 ) + 1;

    if ( (*updx) < 0x00 ) (*updx) = 0x00;
    if ( (*updy) < 0x00 ) (*updy) = 0x00;

    if ( ( (*updx) + (*updw) ) > width  ) (*updw) = width  - (*updx);
    if ( ( (*updy) + (*updh) ) > height ) (*updh) = height - (*updy);

    static int test = 1;

    l3core_paintLine ( obj->pattern,
                       fgcolor,
                       obj->pressure,
                       basepen->oldx,
                       basepen->oldy,
                       x,
                       y,
                       buffer,
                       width,
                       height, 
                       bpp, 
                       mask,
                       zbuffer,
                       engineFlags );

    if ( test == 0x00 ) {
        /*l3core_paintLine ( tool->pattern,
                           0x00000000,
                           0.1f,
                           100,
                           200,
                           200,
                           200,
                           buffer,
                           width,
                           height, 
                           bpp, 
                           mask,
                           zbuffer,
                           engineFlags );*/

        /*l3dpattern_paint ( tool->pattern,
                           0x00000000,
                           0.2f,
                           100,
                           200,
                           buffer,
                           width,
                           height,
                           bpp,
                           mask,
                           zbuffer,
                           engineFlags );

        l3dpattern_paint ( tool->pattern,
                           0x00000000,
                           0.2f,
                           106,
                           200,
                           buffer,
                           width,
                           height,
                           bpp,
                           mask,
                           zbuffer,
                           engineFlags );

        l3dpattern_paint ( tool->pattern,
                           0x00000000,
                           0.2f,
                           112,
                           200,
                           buffer,
                           width,
                           height,
                           bpp,
                           mask,
                           zbuffer,
                           engineFlags );

        l3dpattern_paint ( tool->pattern,
                           0x00000000,
                           0.2f,
                           118,
                           200,
                           buffer,
                           width,
                           height,
                           bpp,
                           mask,
                           zbuffer,
                           engineFlags );

        l3dpattern_paint ( tool->pattern,
                           0x00000000,
                           0.2f,
                           124,
                           200,
                           buffer,
                           width,
                           height,
                           bpp,
                           mask,
                           zbuffer,
                           engineFlags );

        l3dpattern_paint ( tool->pattern,
                           0x00000000,
                           0.2f,
                           130,
                           200,
                           buffer,
                           width,
                           height,
                           bpp,
                           mask,
                           zbuffer,
                           engineFlags );

        l3dpattern_paint ( tool->pattern,
                           0x00000000,
                           0.2f,
                           136,
                           200,
                           buffer,
                           width,
                           height,
                           bpp,
                           mask,
                           zbuffer,
                           engineFlags );*/

        /*l3dpattern_paint ( tool->pattern,
                           0x00000000,
                           1.0f,
                           148,
                           200,
                           buffer,
                           width,
                           height,
                           bpp,
                           mask,
                           zbuffer,
                           engineFlags );*/

        g3dcore_writeJpeg ( "test.jpg", width, height, bpp, buffer );

        test = 0x01;
    }

    /*l3dpattern_paint ( tool->pattern,
                       0x00000000,
                       1.0f,
                       100,
                       216,
                       buffer,
                       width,
                       height,
                       bpp,
                       mask,
                       zbuffer,
                       engineFlags );

    l3dpattern_paint ( tool->pattern,
                       0x00000000,
                       1.0f,
                       110,
                       216,
                       buffer,
                       width,
                       height,
                       bpp,
                       mask,
                       zbuffer,
                       engineFlags );*/

    /*l3dpattern_paint ( tool->pattern,
                       0x00000000,
                       1.0f,
                       102,
                       216,
                       buffer,
                       width,
                       height,
                       bpp,
                       mask,
                       zbuffer,
                       engineFlags );*/

    basepen->oldx = x;
    basepen->oldy = y;

    return 0x00;
}

/******************************************************************************/
static int l3dbasepen_done ( L3DOBJECT     *obj,
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
                             uint32_t       engineFlags ) {
    L3DBASEPEN *basepen = ( L3DBASEPEN * ) obj;

    return 0x00;
}
