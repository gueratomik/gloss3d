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

#ifndef _LIPS3D_H_
#define _LIPS3D_H_

/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <string.h>

/******************************************************************************/
#include <math.h>

/******************************************************************************/
#include <list.h>
#include <g3dengine/g3dengine.h>

#define L3DBUTTON1PRESSED  ( 1 << 0 )
#define L3DUPDATESUBIMAGE  ( 1 << 1 )
#define L3DUPDATEIMAGE     ( 1 << 2 )
#define L3DBUTTON1DOUBLED  ( 1 << 3 )

/******************************************************************************/
typedef struct _L3DPATTERN {
    int (*generate)( struct _L3DPATTERN *pattern );
    uint32_t size;
    unsigned char *buffer;
} L3DPATTERN;

/******************************************************************************/
typedef struct _L3DSYSINFO {
    L3DPATTERN *pattern;
    uint32_t    fgcolor;
    uint32_t    bgcolor;
    uint32_t    debug; /* yes or no */
} L3DSYSINFO;

/******************************************************************************/
void l3dsysinfo_reset ( L3DSYSINFO *sysinfo );
L3DSYSINFO *l3dsysinfo_get ( );

/******************************************************************************/
void l3dsysinfo_setPattern ( L3DSYSINFO *sysinfo, L3DPATTERN *pattern );
void l3dsysinfo_setForegroundColor ( L3DSYSINFO *sysinfo, uint32_t fgcolor );
void l3dsysinfo_setBackgroundColor ( L3DSYSINFO *sysinfo, uint32_t bgcolor );

/******************************************************************************/
typedef struct _L3DOBJECT {
    float       pressure;
    int (*reset)   ( struct _L3DOBJECT *obj,
                     uint64_t engine_flags );
    int (*press)   ( struct _L3DOBJECT *obj,
                     L3DPATTERN        *pattern,
                     uint32_t           fgcolor,
                     uint32_t           bgcolor,
                     int32_t            x,
                     int32_t            y,
                     unsigned char     *buffer, 
                     uint32_t           width, 
                     uint32_t           height,
                     uint32_t           bpp,
                     unsigned char     *mask,
                     unsigned char     *zbuffer,
                     uint64_t engine_flags );
    int (*move)    ( struct _L3DOBJECT *obj,
                     L3DPATTERN        *pattern,
                     uint32_t           fgcolor,
                     uint32_t           bgcolor,
                     int32_t            x,
                     int32_t            y,
                     unsigned char     *buffer, 
                     uint32_t           width, 
                     uint32_t           height,
                     uint32_t           bpp,
                     unsigned char     *mask,
                     unsigned char     *zbuffer,
                     int32_t           *updx,
                     int32_t           *updy,
                     int32_t           *updw,
                     int32_t           *updh,
                     uint64_t engine_flags );
    int (*release) ( struct _L3DOBJECT *obj,
                     L3DPATTERN        *pattern,
                     uint32_t           fgcolor,
                     uint32_t           bgcolor,
                     int32_t            x,
                     int32_t            y,
                     unsigned char     *buffer, 
                     uint32_t           width, 
                     uint32_t           height,
                     uint32_t           bpp,
                     unsigned char     *mask,
                     unsigned char     *zbuffer,
                     uint64_t engine_flags );
} L3DOBJECT;

/******************************************************************************/
typedef struct _L3DBASEPEN {
    L3DOBJECT obj;
    int32_t   oldx;
    int32_t   oldy;
} L3DBASEPEN;

/******************************************************************************/
typedef struct _L3DSELECTORPOINT {
    int32_t x;
    int32_t y;
    float   u;
    float   v;
} L3DSELECTORPOINT;

/******************************************************************************/
typedef struct _L3DSELECTORLINE {
    L3DSELECTORPOINT *srcpt;
    L3DSELECTORPOINT *dstpt;
} L3DSELECTORLINE;

/******************************************************************************/
typedef struct _L3DSELECTOR {
    L3DOBJECT         obj;
    L3DSELECTORPOINT *firstPoint;
    L3DSELECTORPOINT *lastPoint;
    L3DSELECTORLINE  *firstLine;
    L3DSELECTORLINE  *lastLine;
    LIST             *llines;
    LIST             *lpoints;
    uint32_t          closed;
    int32_t           xmax;
    int32_t           ymax;
    int32_t           xmin;
    int32_t           ymin;
} L3DSELECTOR;

/******************************************************************************/
typedef struct _L3DPLAINRECTANGLEPATTERN {
    L3DPATTERN pattern;
} L3DPLAINRECTANGLEPATTERN;

/******************************************************************************/
typedef struct _L3DPLAINCIRCLEPATTERN {
    L3DPATTERN pattern;
} L3DPLAINCIRCLEPATTERN;

/******************************************************************************/
typedef struct _L3DFADEDCIRCLEPATTERN {
    L3DPATTERN pattern;
} L3DFADEDCIRCLEPATTERN;

/******************************************************************************/
int l3dcore_paintPoint ( L3DPATTERN    *pattern,
                         uint32_t       color,
                         float          pressure,
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
int l3core_paintCircle ( L3DPATTERN    *pattern,
                         uint32_t       color,
                         float          pressure,
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
int l3core_paintRectangle ( L3DPATTERN    *pattern,
                            uint32_t       color,
                            float          pressure,
                            int32_t        x1,
                            int32_t        y1,
                            int32_t        x2,
                            int32_t        y2,
                            unsigned char *buffer,
                            uint32_t       width,
                            uint32_t       height, 
                            uint32_t       bpp, 
                            unsigned char *mask,
                            unsigned char *zbuffer,
                            uint64_t engine_flags );

/******************************************************************************/
int l3core_paintLine ( L3DPATTERN    *pat,
                       uint32_t       color,
                       float          pressure,
                       int32_t        x1,
                       int32_t        y1,
                       int32_t        x2,
                       int32_t        y2,
                       unsigned char *buffer,
                       uint32_t       width,
                       uint32_t       height, 
                       uint32_t       bpp, 
                       unsigned char *mask,
                       unsigned char *zbuffer,
                       uint64_t engine_flags );

/******************************************************************************/
int l3dpattern_paint ( L3DPATTERN    *pattern,
                       uint32_t       color,
                       float          pressure,
                       int32_t        x,
                       int32_t        y,
                       unsigned char *buffer,
                       uint32_t       width,
                       uint32_t       height,
                       uint32_t       bpp,
                       unsigned char *mask,
                       unsigned char *zbuffer,
                       uint64_t engine_flags );
void l3dpattern_generatePlainRectangle ( L3DPATTERN *pattern );
void l3dpattern_generatePlainCircle ( L3DPATTERN *pattern );
void l3dpattern_resize ( L3DPATTERN *pattern, uint32_t size );
void l3dpattern_init ( L3DPATTERN *pattern,
                       uint32_t    size,
                       void(*generate)(L3DPATTERN*));
L3DPLAINRECTANGLEPATTERN *l3dplainrectanglepattern_new ( uint32_t size );
L3DFADEDCIRCLEPATTERN    *l3dfadedcirclepattern_new    ( uint32_t size );
L3DPLAINCIRCLEPATTERN    *l3dplaincirclepattern_new    ( uint32_t size );

/******************************************************************************/
void l3dobject_init ( L3DOBJECT  *obj,
                      float       pressure,
                      int (*reset)  ( L3DOBJECT     *obj,
                                      uint64_t       engine_flags ),
                      int (*press)  ( L3DOBJECT     *obj,
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
                                      uint64_t       engine_flags ),
                      int (*move)   ( L3DOBJECT     *obj,
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
                                      uint64_t       engine_flags ),
                     int (*release) ( L3DOBJECT     *obj,
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
                                      uint64_t       engine_flags ) );

/******************************************************************************/
L3DBASEPEN* l3dbasepen_new ( );
L3DSELECTOR* l3dselector_new ( );

#endif
