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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2013         */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/

/**
 * @file
 */

/******************************************************************************/
#ifndef _G3DSPLINE_H_
#define _G3DSPLINE_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct G3DSPLINE
 * @brief A structure to store a cubic bezier spline.
 */
typedef struct _G3DSPLINE {
    G3DMESH mes;
    LIST *lseg; /** list of segments ***/
    uint32_t nbseg;
} G3DSPLINE;

/******************************************************************************/
typedef struct _G3DSPLINEPOINT {
    G3DVERTEX ver;
    LIST     *lseg;
    uint32_t  nbseg;
} G3DSPLINEPOINT;

/******************************************************************************/
typedef struct _G3DCUBICSPLINEHANDLE {
    G3DVERTEX ver;
    G3DVERTEX *pt;
} G3DCUBICSPLINEHANDLE;

/******************************************************************************/
typedef struct _G3DSPLINESEGMENT {
    uint32_t id;
    G3DCUBICSPLINEHANDLE handle[0x02];
    G3DVERTEX *pt[0x04];
} G3DSPLINESEGMENT;

/******************************************************************************/
void g3dsplinesegment_free ( G3DSPLINESEGMENT *seg );
void g3dsplinesegment_drawQuadratic ( G3DSPLINESEGMENT *seg,
                                      G3DSPLINEPOINT   *pori,
                                      G3DSPLINEPOINT   *pend,
                                      float from, /* range 0 - 1 */
                                      float to,   /* range 0 - 1 */
                                      float maxAngle,
                                      uint32_t engine_flags );
uint32_t g3dspline_draw ( G3DOBJECT *obj, G3DCAMERA *curcam, 
                                          uint32_t   engine_flags );
void g3dspline_free ( G3DOBJECT *obj );
void g3dspline_addSegment ( G3DSPLINE *spline, G3DSPLINESEGMENT *seg );
void g3dspline_init ( G3DSPLINE *spline, uint32_t id, 
                                         char    *name,
                                         uint32_t type,
                                         uint32_t engine_flags );
G3DSPLINE *g3dspline_new ( uint32_t id, 
                           char    *name, 
                           uint32_t type, 
                           uint32_t engine_flags );
G3DSPLINESEGMENT *g3dsplinesegment_new ( G3DSPLINEPOINT *pt0,
                                         G3DSPLINEPOINT *pt1,
                                         float hx1, float hy1, float hz1,
                                         float hx2, float hy2, float hz2 );
G3DSPLINEPOINT *g3dsplinepoint_new ( float x, float y, float z );
void g3dsplinepoint_free ( G3DSPLINEPOINT * );

#ifdef __cplusplus
}
#endif

#endif
