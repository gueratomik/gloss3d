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
void l3dobject_init ( L3DOBJECT  *obj,
                      L3DPATTERN *pattern,
                      float       pressure,
                      int (*init) ( L3DOBJECT     *obj,
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
                                    uint32_t       engineFlags ),
                     int (*paint) ( L3DOBJECT     *obj,
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
                                    uint32_t       engineFlags ),
                     int (*done) ( L3DOBJECT     *obj,
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
                                   uint32_t       engineFlags ) ) {
    obj->init  = init;
    obj->paint = paint;
    obj->done  = done;

    obj->pattern = pattern;
    obj->pressure = pressure;
}
