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

#ifndef _MAKEUP3D_H_
#define _MAKEUP3D_H_

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

#define M3DBUTTON1PRESSED  ( 1 << 0 )
#define M3DUPDATESUBIMAGE  ( 1 << 1 )
#define M3DUPDATEIMAGE     ( 1 << 2 )
#define M3DBUTTON1DOUBLED  ( 1 << 3 )

/******************************************************************************/
typedef struct _M3DPATTERN {
    void (*generate)( struct _M3DPATTERN *pattern );
    uint32_t size;
    unsigned char *buffer;
} M3DPATTERN;

/******************************************************************************/
typedef struct _M3DSYSINFO {
    M3DPATTERN *pattern;
    uint32_t    fgcolor;
    uint32_t    bgcolor;
    uint32_t    debug; /* yes or no */
} M3DSYSINFO;

/******************************************************************************/
void m3dsysinfo_reset ( M3DSYSINFO *sysinfo );
M3DSYSINFO *m3dsysinfo_get ( );

/******************************************************************************/
void m3dsysinfo_setPattern ( M3DSYSINFO *sysinfo, M3DPATTERN *pattern );
void m3dsysinfo_setForegroundColor ( M3DSYSINFO *sysinfo, uint32_t fgcolor );
void m3dsysinfo_setBackgroundColor ( M3DSYSINFO *sysinfo, uint32_t bgcolor );

#define M3DPRESS_CALLBACK(f)   ((int(*)(M3DOBJECT *, \
                                        M3DPATTERN *, \
                                        uint32_t, \
                                        uint32_t, \
                                        int32_t, \
                                        int32_t, \
                                        unsigned char *, \
                                        uint32_t ,  \
                                        uint32_t , \
                                        uint32_t , \
                                        unsigned char *, \
                                        unsigned char *, \
                                        int32_t *, \
                                        uint64_t ))f)

#define M3DMOVE_CALLBACK(f)    ((int(*)(M3DOBJECT *, \
                                        M3DPATTERN *, \
                                        uint32_t, \
                                        uint32_t, \
                                        int32_t, \
                                        int32_t, \
                                        unsigned char *, \
                                        uint32_t ,  \
                                        uint32_t , \
                                        uint32_t , \
                                        unsigned char *, \
                                        unsigned char *, \
                                        int32_t *, \
                                        uint64_t ))f)

#define M3DRELEASE_CALLBACK(f) ((int(*)(M3DOBJECT *, \
                                        M3DPATTERN *, \
                                        uint32_t, \
                                        uint32_t, \
                                        int32_t, \
                                        int32_t, \
                                        unsigned char *, \
                                        uint32_t ,  \
                                        uint32_t , \
                                        uint32_t , \
                                        unsigned char *, \
                                        unsigned char *, \
                                        int32_t *, \
                                        uint64_t ))f)

#define M3DRESET_CALLBACK(f)   ((int(*)(M3DOBJECT *, \
                                        uint64_t ))f)

/******************************************************************************/
typedef struct _M3DOBJECT {
    int (*reset)   ( struct _M3DOBJECT *obj,
                     uint64_t engine_flags );
    int (*press)   ( struct _M3DOBJECT *obj,
                     M3DPATTERN        *pattern,
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
                      int32_t          *updcoord,
                     uint64_t engine_flags );
    int (*move)    ( struct _M3DOBJECT *obj,
                     M3DPATTERN        *pattern,
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
                      int32_t          *updcoord,
                     uint64_t engine_flags );
    int (*release) ( struct _M3DOBJECT *obj,
                     M3DPATTERN        *pattern,
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
                      int32_t          *updcoord,
                     uint64_t engine_flags );
} M3DOBJECT;

/******************************************************************************/
typedef struct _M3DBUCKET {
    M3DOBJECT obj;
    uint8_t   tolerance;
} M3DBUCKET;

/******************************************************************************/
typedef struct _M3DBASEPEN {
    M3DOBJECT obj;
    float     pressure;
    int32_t   oldx;
    int32_t   oldy;
} M3DBASEPEN;

/******************************************************************************/
typedef struct _M3DSELECTORPOINT {
    int32_t x;
    int32_t y;
    float   u;
    float   v;
} M3DSELECTORPOINT;

/******************************************************************************/
typedef struct _M3DSELECTORLINE {
    M3DSELECTORPOINT *srcpt;
    M3DSELECTORPOINT *dstpt;
} M3DSELECTORLINE;

/******************************************************************************/

typedef enum {
    SELECTORMODERANDOM,
    SELECTORMODESQUARE,
    SELECTORMODECIRCLE,
    SELECTORMODELASSO
} M3DSELECTORMODEENUM;

typedef struct _M3DSELECTOR {
    M3DOBJECT           obj;
    M3DSELECTORMODEENUM mode;
    uint32_t            closed;
    M3DSELECTORPOINT   *firstPoint;
    M3DSELECTORPOINT   *lastPoint;
    M3DSELECTORLINE    *firstLine;
    M3DSELECTORLINE    *lastLine;
    LIST               *llines;
    LIST               *lpoints;
    int32_t             xmax;
    int32_t             ymax;
    int32_t             xmin;
    int32_t             ymin;
    /*** for square mode ***/
    M3DSELECTORPOINT    *sqpt[0x04];
    M3DSELECTORLINE     *sqlin[0x04];
} M3DSELECTOR;

/******************************************************************************/
typedef struct _M3DPLAINRECTANGLEPATTERN {
    M3DPATTERN pattern;
} M3DPLAINRECTANGLEPATTERN;

/******************************************************************************/
typedef struct _M3DPLAINCIRCLEPATTERN {
    M3DPATTERN pattern;
} M3DPLAINCIRCLEPATTERN;

/******************************************************************************/
typedef struct _M3DFADEDCIRCLEPATTERN {
    M3DPATTERN pattern;
    float radius;
    float fullPartRate;
} M3DFADEDCIRCLEPATTERN;

/******************************************************************************/
/*** for brushes generated and exported using GIMP C export feature ***/
typedef struct _M3DGIMPBRUSH {
  unsigned int  width;
  unsigned int  height;
  unsigned int  bytes_per_pixel; /* 2:RGB16, 3:RGB, 4:RGBA */ 
  unsigned char pixel_data[];
} M3DGIMPBRUSH;

/******************************************************************************/
typedef struct _M3DBRUSHPATTERN {
    M3DPATTERN    pattern;
    M3DGIMPBRUSH *brush;
} M3DBRUSHPATTERN;

/******************************************************************************/
int m3dcore_paintPoint ( M3DPATTERN    *pattern,
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
                         uint64_t       engine_flags );

/******************************************************************************/
int m3dcore_paintCircle ( M3DPATTERN    *pattern,
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
                         uint64_t       engine_flags );

/******************************************************************************/
int m3dcore_paintRectangle ( M3DPATTERN    *pattern,
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
                            uint64_t       engine_flags );

/******************************************************************************/
int m3dcore_paintLine ( M3DPATTERN    *pat,
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
                       uint64_t       engine_flags );

/******************************************************************************/
int m3dpattern_paint ( M3DPATTERN    *pattern,
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
                       uint64_t       engine_flags );
void m3dpattern_generatePlainRectangle ( M3DPATTERN *pattern );
void m3dpattern_generatePlainCircle ( M3DPATTERN *pattern );
void m3dpattern_resize ( M3DPATTERN *pattern, uint32_t size );
void m3dpattern_init ( M3DPATTERN *pattern,
                       uint32_t    size,
                       void(*generate)(M3DPATTERN*));
M3DPLAINRECTANGLEPATTERN *m3dplainrectanglepattern_new ( uint32_t size );
M3DFADEDCIRCLEPATTERN *m3dfadedcirclepattern_new ( uint32_t size, 
                                                   float    radius,
                                                   float    fullPartRate );
M3DPLAINCIRCLEPATTERN    *m3dplaincirclepattern_new    ( uint32_t size );

M3DBRUSHPATTERN *m3dbrushpattern_new ( uint32_t      size, 
                                       M3DGIMPBRUSH *brush );

/******************************************************************************/
void m3dobject_init ( M3DOBJECT  *obj,
                      int (*reset)  ( M3DOBJECT     *obj,
                                      uint64_t       engine_flags ),
                      int (*press)  ( M3DOBJECT     *obj,
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
                                       int32_t      *updcoord,
                                      uint64_t       engine_flags ),
                      int (*move)   ( M3DOBJECT     *obj,
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
                                       int32_t      *updcoord,
                                      uint64_t       engine_flags ),
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
                                       int32_t      *updcoord,
                                      uint64_t       engine_flags ) );

/******************************************************************************/
M3DBASEPEN* m3dbasepen_new ( );
M3DBUCKET* m3dbucket_new ( );
M3DSELECTOR* m3dselector_new ( );
void m3dselector_setMode ( M3DSELECTOR        *sel,
                           M3DSELECTORMODEENUM mode );
uint32_t m3dcore_setUpdateArea ( int32_t   x1,
                                 int32_t   y1,
                                 int32_t   x2,
                                 int32_t   y2,
                                 uint32_t  width,
                                 uint32_t  height,
                                 int32_t  *updcoord, 
                                 uint32_t  size );

#endif
