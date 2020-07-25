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

static int l3dbasepen_press ( L3DOBJECT     *obj,
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
                              uint64_t engine_flags );
static int l3dbasepen_move ( L3DOBJECT     *obj,
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
                             int32_t       *updx,
                             int32_t       *updy,
                             int32_t       *updw,
                             int32_t       *updh,
                             uint64_t engine_flags );
static int l3dbasepen_release ( L3DOBJECT     *obj,
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
                                uint64_t engine_flags );

/******************************************************************************/
L3DBASEPEN* l3dbasepen_new ( ) {
    uint32_t structSize = sizeof ( L3DBASEPEN );
    L3DBASEPEN *basepen = ( L3DBASEPEN * ) calloc ( 0x01, structSize );

    if ( basepen == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    l3dobject_init ( basepen,
                     1.0f,
                     NULL,
                     l3dbasepen_press,
                     l3dbasepen_move,
                     l3dbasepen_release );

    return basepen;
}

/******************************************************************************/
static int l3dbasepen_press ( L3DOBJECT     *obj,
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
                              uint64_t engine_flags ) {
    L3DBASEPEN *basepen = ( L3DBASEPEN * ) obj;

    basepen->oldx = x;
    basepen->oldy = y;

    memset ( zbuffer, 0x00, width * height );

    return 0x00;
}

/******************************************************************************/
static int l3dbasepen_move ( L3DOBJECT     *obj,
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
                             int32_t       *updx,
                             int32_t       *updy,
                             int32_t       *updw,
                             int32_t       *updh,
                             uint64_t engine_flags ) {
    if ( engine_flags & L3DBUTTON1PRESSED ) {
        /*y = 100;*/
        L3DBASEPEN *basepen = ( L3DBASEPEN * ) obj;
        uint32_t size = pattern->size, half = size / 0x02;
        /*** we use some margin because fro some unknown reason, glTexSubImage2D ***/
        /*** does not seem to copy the subimage completely ***/
        uint32_t margin = 10; 
        int32_t x1 = ( x < basepen->oldx ) ? x - half : basepen->oldx - half,
                y1 = ( y < basepen->oldy ) ? y - half : basepen->oldy - half,
                x2 = ( x > basepen->oldx ) ? x + half : basepen->oldx + half,
                y2 = ( y > basepen->oldy ) ? y + half : basepen->oldy + half;

        if ( updx ) (*updx) = x1;
        if ( updy ) (*updy) = y1;
        if ( updw ) (*updw) = ( x2 - x1 ) + 1;
        if ( updh ) (*updh) = ( y2 - y1 ) + 1;

        if ( (*updx) < 0x00 ) (*updx) = 0x00;
        if ( (*updy) < 0x00 ) (*updy) = 0x00;

        if ( ( (*updx) + (*updw) ) > width  ) (*updw) = width  - (*updx);
        if ( ( (*updy) + (*updh) ) > height ) (*updh) = height - (*updy);

        l3core_paintLine ( pattern,
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
                           engine_flags );

        basepen->oldx = x;
        basepen->oldy = y;
    }

    return L3DUPDATESUBIMAGE;
}

/******************************************************************************/
static int l3dbasepen_release ( L3DOBJECT     *obj,
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
                                uint64_t engine_flags ) {
    L3DBASEPEN *basepen = ( L3DBASEPEN * ) obj;

    return 0x00;
}
