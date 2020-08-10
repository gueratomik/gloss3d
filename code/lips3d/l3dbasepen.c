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
                              uint32_t      *updcoord,
                              uint64_t       engine_flags );
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
                             uint32_t      *updcoord,
                             uint64_t       engine_flags );
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
                                uint32_t      *updcoord,
                                uint64_t       engine_flags );

/******************************************************************************/
L3DBASEPEN* l3dbasepen_new ( ) {
    uint32_t structSize = sizeof ( L3DBASEPEN );
    L3DBASEPEN *basepen = ( L3DBASEPEN * ) calloc ( 0x01, structSize );

    if ( basepen == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    basepen->pressure    = 1.0f;

    l3dobject_init ( basepen,
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
                              uint32_t      *updcoord,
                              uint64_t       engine_flags ) {
    L3DBASEPEN *basepen = ( L3DBASEPEN * ) obj;

    basepen->oldx = x;
    basepen->oldy = y;

    memset ( zbuffer, 0x00, width * height );

    if ( updcoord ) {
        if ( l3dcore_setUpdateArea ( x,
                                     y,
                                     x, 
                                     y, 
                                     width, 
                                     height, 
                                     updcoord, 
                                     pattern->size ) == 0x00 ) {
            return 0x00;
        }
    }

    l3dpattern_paint ( pattern,
                       fgcolor,
                       basepen->pressure,
                       x,
                       y,
                       buffer,
                       width,
                       height,
                       bpp,
                       mask,
                       zbuffer,
                       engine_flags );

    return L3DUPDATESUBIMAGE;
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
                             uint32_t      *updcoord,
                             uint64_t       engine_flags ) {
    if ( engine_flags & L3DBUTTON1PRESSED ) {
        /*y = 100;*/
        L3DBASEPEN *basepen = ( L3DBASEPEN * ) obj;

        if ( updcoord ) {
            if ( l3dcore_setUpdateArea ( basepen->oldx,
                                         basepen->oldy,
                                         x, 
                                         y, 
                                         width, 
                                         height, 
                                         updcoord, 
                                         pattern->size ) == 0x00 ) {
                return 0x00;
            }
        }

        l3core_paintLine ( pattern,
                           fgcolor,
                           basepen->pressure,
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

        return L3DUPDATESUBIMAGE;
    }

    return 0x00;
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
                                uint32_t      *updcoord,
                                uint64_t       engine_flags ) {
    L3DBASEPEN *basepen = ( L3DBASEPEN * ) obj;

    if ( updcoord ) {
        if ( l3dcore_setUpdateArea ( x,
                                     y,
                                     x, 
                                     y, 
                                     width, 
                                     height, 
                                     updcoord, 
                                     pattern->size ) == 0x00 ) {
            return 0x00;
        }
    }

    return L3DUPDATESUBIMAGE;
}
