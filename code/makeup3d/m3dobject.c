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
void m3dobject_init ( M3DOBJECT  *obj,
                      int (*reset) ( M3DOBJECT     *obj,
                                     uint64_t engine_flags ),
                      int (*press) ( M3DOBJECT     *obj,
                                     M3DPATTERN    *pattern,
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
                                     uint64_t engine_flags ),
                     int (*move) ( M3DOBJECT     *obj,
                                   M3DPATTERN    *pattern,
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
                                   uint64_t engine_flags ),
                     int (*release) ( M3DOBJECT     *obj,
                                      M3DPATTERN    *pattern,
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
                                      uint64_t engine_flags ) ) {
    obj->reset    = reset;
    obj->press    = press;
    obj->move     = move;
    obj->release  = release;
}
