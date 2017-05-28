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

/**
 * @file
 */

/******************************************************************************/
#ifndef _G3DSPLINE_H_
#define _G3DSPLINE_H_

#ifdef __cplusplus
extern "C" {
#endif

/************************** Spline drawing flags ******************************/
#define DRAW_FOR_TESSELLATION ( 1 << 0 )

/**
 * @struct G3DSPLINE
 * @brief A structure to store a cubic bezier spline.
 */
typedef struct _G3DSPLINE {
    G3DMESH mes;
    LIST *lseg; /** list of segments ***/
    uint32_t nbseg;
    uint32_t nbStepsPerSegment;
} G3DSPLINE;

/******************************************************************************/
typedef struct _G3DSPLINESEGMENT {
    uint32_t id;
    void (*getPoint)( struct _G3DSPLINESEGMENT *seg, 
                             float              factor, 
                             G3DVECTOR         *pout );
    G3DVERTEX *pt[0x04];
} G3DSPLINESEGMENT;

/******************************************************************************/
typedef struct _G3DSPLINEPOINT {
    G3DVERTEX ver;
    LIST     *lseg;
    uint32_t  nbseg;
} G3DSPLINEPOINT;

/******************************************************************************/
typedef struct _G3DCUBICHANDLE {
    G3DVERTEX ver;
    G3DVERTEX *pt;
} G3DCUBICHANDLE;

/******************************************************************************/
typedef struct _G3DCUBICSEGMENT {
    G3DSPLINESEGMENT seg; /*** OOP part ***/
    G3DCUBICHANDLE handle[0x02];
} G3DCUBICSEGMENT;

/******************************************************************************/
typedef struct _G3DQUADRATICHANDLE {
    G3DVERTEX ver;
    G3DVERTEX *pt[0x02];
} G3DQUADRATICHANDLE;

/******************************************************************************/
typedef struct _G3DQUADRATICSEGMENT {
    G3DSPLINESEGMENT seg; /*** OOP part***/
    G3DQUADRATICHANDLE handle;
} G3DQUADRATICSEGMENT;

/******************************************************************************/

void g3dsplinesegment_draw ( G3DSPLINESEGMENT *seg,
                             float             from, /* range 0 - 1 */
                             float             to,   /* range 0 - 1 */
                             float             nbSteps,
                             GLUtesselator    *tobj,
                             double          (*coords)[0x03],
                             uint32_t          spline_flags,
                             uint32_t          engine_flags );

uint32_t g3dspline_draw ( G3DOBJECT *obj, 
                          G3DCAMERA *curcam,
                          uint32_t   engine_flags );

void g3dspline_free ( G3DOBJECT *obj );

void g3dspline_addSegment ( G3DSPLINE        *spline, 
                            G3DSPLINESEGMENT *seg );
void g3dspline_revert ( G3DSPLINE *spline, uint32_t engine_flags );
void g3dspline_init ( G3DSPLINE *spline, 
                      uint32_t   id, 
                      char      *name,
                      uint32_t   type,
                      uint32_t   engine_flags );

G3DSPLINE *g3dspline_new ( uint32_t id, 
                           char    *name, 
                           uint32_t type, 
                           uint32_t engine_flags );

void g3dquadraticsegment_free ( G3DQUADRATICSEGMENT *csg );

void g3dquadraticsegment_getPoint ( G3DQUADRATICSEGMENT *qsg, 
                                    float                factor, /* range 0 - 1 */
                                    G3DVECTOR           *pout );

void g3dquadraticsegment_init ( G3DQUADRATICSEGMENT *csg,
                                G3DSPLINEPOINT      *pt0,
                                G3DSPLINEPOINT      *pt1,
                                float                hx, 
                                float                hy,
                                float                hz );

G3DQUADRATICSEGMENT *g3dquadraticsegment_new ( G3DSPLINEPOINT *pt0,
                                               G3DSPLINEPOINT *pt1,
                                               float           hx, 
                                               float           hy,
                                               float           hz );

void g3dcubicsegment_getPoint ( G3DCUBICSEGMENT *seg, 
                                float            factor, /* range 0 - 1 */
                                G3DVECTOR       *pout );

void g3dcubicsegment_free ( G3DCUBICSEGMENT *seg );

G3DCUBICSEGMENT *g3dcubicsegment_new ( G3DSPLINEPOINT *pt0,
                                       G3DSPLINEPOINT *pt1,
                                       float           hx1, 
                                       float           hy1,
                                       float           hz1,
                                       float           hx2,
                                       float           hy2,
                                       float           hz2 );

G3DSPLINEPOINT *g3dsplinepoint_new ( float x, 
                                     float y, 
                                     float z );

void g3dsplinepoint_free ( G3DSPLINEPOINT * );


#ifdef __cplusplus
}
#endif

#endif
