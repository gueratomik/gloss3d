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

/******************************************************************************/
typedef struct _L3DBRUSH {
    int dummy;
} L3DBRUSH;

/******************************************************************************/
typedef struct _L3DPATTERN {
    int (*paint)( struct _L3DPATTERN *pattern,
                  uint32_t            color,
                  int32_t             size,
                  float               pressure,
                  int32_t             x,
                  int32_t             y,
                  unsigned char      *buffer, 
                  uint32_t            width, 
                  uint32_t            height,
                  uint32_t            bpp,
                  unsigned char      *mask,
                  unsigned char      *zbuffer,
                  uint32_t            engineFlags );
} L3DPATTERN;

/******************************************************************************/
typedef struct _L3DTOOL {
    L3DPATTERN *pattern;
    int (*init)( struct _L3DTOOL *tool,
                  int32_t          x,
                  int32_t          y,
                  unsigned char   *buffer, 
                  uint32_t         width, 
                  uint32_t         height,
                  uint32_t         bpp,
                  unsigned char   *mask,
                  unsigned char   *zbuffer,
                  uint32_t         engineFlags );
    int (*paint)( struct _L3DTOOL *tool,
                  int32_t          x,
                  int32_t          y,
                  unsigned char            *buffer, 
                  uint32_t         width, 
                  uint32_t         height,
                  uint32_t         bpp,
                  unsigned char   *mask,
                  unsigned char   *zbuffer,
                  int32_t         *updx,
                  int32_t         *updy,
                  int32_t         *updw,
                  int32_t         *updh,
                  uint32_t         engineFlags );
    int (*done)( struct _L3DTOOL *tool,
                  int32_t          x,
                  int32_t          y,
                  unsigned char            *buffer, 
                  uint32_t         width, 
                  uint32_t         height,
                  uint32_t         bpp,
                  unsigned char    *mask,
                  unsigned char   *zbuffer,
                  uint32_t         engineFlags );
} L3DTOOL;

/******************************************************************************/
typedef struct _L3DLINETOOL {
    L3DTOOL tool;
} L3DLINETOOL;

/******************************************************************************/
typedef struct _L3DRECTANGLETOOL {
    L3DTOOL tool;
} L3DRECTANGLETOOL;

/******************************************************************************/
typedef struct _L3DCIRCLETOOL {
    L3DTOOL tool;
} L3DCIRCLETOOL;

/******************************************************************************/
typedef struct _L3DSPRAYTOOL {
    L3DTOOL tool;
    int32_t oldx;
    int32_t oldy;
} L3DSPRAYTOOL;

/******************************************************************************/
typedef struct _L3DBUCKETTOOL {
    L3DTOOL tool;
} L3DBUCKETTOOL;

/******************************************************************************/
typedef struct _L3DERASERTOOL {
    L3DTOOL tool;
} L3DERASERTOOL;

/******************************************************************************/
typedef struct _L3DSELECTIONTOOL {
    L3DTOOL tool;
} L3DSELECTIONTOOL;

/******************************************************************************/
typedef struct _L3DPENTOOL {
    L3DTOOL tool;
} L3DPENTOOL;

/******************************************************************************/
typedef struct _L3DBRUSHTOOL {
    L3DTOOL   tool;
    L3DBRUSH *brush;
} L3DBRUSHTOOL;

/******************************************************************************/
int l3dcore_paintPoint ( L3DPATTERN    *pattern,
                         uint32_t       color,
                         int32_t        size,
                         float          pressure,
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
int l3core_paintCircle ( L3DPATTERN    *pattern,
                         uint32_t       color,
                         int32_t        size,
                         float          pressure,
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
int l3core_paintRectangle ( L3DPATTERN    *pattern,
                            uint32_t       color,
                            int32_t        size,
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
                            uint32_t       engineFlags );

/******************************************************************************/
int l3core_paintLine ( L3DPATTERN    *pat,
                       uint32_t       color,
                       int32_t        size,
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
                       uint32_t       engineFlags );

/******************************************************************************/
L3DSPRAYTOOL *l3dspraytool_new ( );

/******************************************************************************/
void l3dpattern_init ( L3DPATTERN *pattern );
L3DPATTERN *l3dpattern_new ( );

#endif
