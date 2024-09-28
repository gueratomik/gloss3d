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

typedef struct _G3DSPLINEVTABLE G3DSPLINEVTABLE;

/**
 * @struct G3DSPLINE
 * @brief A structure to store a cubic bezier spline.
 */
typedef struct _G3DSPLINE {
    G3DOBJECT    obj;
    G3DCURVE    *curve;
    G3DMODIFIER *lastmod;
} G3DSPLINE;

/******************************************************************************/
typedef G3DCURVESEGMENT G3DSPLINESEGMENT;
typedef G3DCURVEPOINT   G3DSPLINEPOINT;

/******************************************************************************/
void g3dspline_init ( G3DSPLINE       *spline,
                      uint32_t         id,
                      const char      *name,
                      uint64_t         objectFlags,
                      G3DSPLINEVTABLE *vtable );
G3DSPLINE *g3dspline_new ( uint32_t    id, 
                           const char *name, 
                           uint64_t    type );
void g3dspline_moveAxis ( G3DSPLINE *spl, 
                          float    *PREVMVX, /* previous world matrix */
                          uint64_t engine_flags );

void g3dspline_modify ( G3DSPLINE  *spl,
                        G3DMODIFYOP op,
                        uint64_t    engine_flags );

void g3dcurvepoint_getAveragePositionFromList ( LIST *lpt, G3DVECTOR3F *pos );

#ifdef __cplusplus
}
#endif

#endif
