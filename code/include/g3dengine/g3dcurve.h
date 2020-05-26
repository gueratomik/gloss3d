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
#ifndef _G3DCURVE_H_
#define _G3DCURVE_H_

#ifdef __cplusplus
extern "C" {
#endif

/************************** Spline drawing flags ******************************/
#define DRAW_FOR_TESSELLATION ( 1 << 0 )

#define CURVEPOINTISPOINT     ( 1 << 1 )
#define CURVEPOINTISHANDLE    ( 1 << 2 )
#define CURVEPOINTSELECTED    ( 1 << 3 )

#define P0IDX       0
#define P1IDX       1
#define P0HANDLEIDX (P0IDX+2)
#define P1HANDLEIDX (P1IDX+2)

/******************************************************************************/
typedef struct _G3DCURVEPOINT {
    G3DVECTOR pos;
    uint32_t  id;
    uint32_t  flags;
    LIST     *lseg;
    uint32_t  nbseg;
} G3DCURVEPOINT, G3DCUBICHANDLE, G3DQUADRATICHANDLE;

/******************************************************************************/
typedef struct _G3DCURVESEGMENT {
    uint32_t id;
    void (*getPoint)( struct _G3DCURVESEGMENT *seg, 
                             float             factor, 
                             G3DVECTOR        *pout );
    G3DCURVEPOINT *pt[0x04];
} G3DCURVESEGMENT;

/******************************************************************************/
/**
 * @struct G3DCURVE
 * @brief A structure to store a cubic or quadratic bezier curve.
 */
typedef struct _G3DCURVE {
    uint32_t  type;
    LIST     *lpt;
    uint32_t  nbpt;

    LIST     *lselpt;
    uint32_t  nbselpt;

    LIST     *lseg; /** list of segments ***/
    uint32_t  nbseg;
    uint32_t  nbStepsPerSegment;

    G3DVECTOR avgSelPtPos;

    G3DCURVEPOINT *curhan; /*** current handle ***/
} G3DCURVE;



/******************************************************************************/
/*typedef struct _G3DCUBICHANDLE {
    G3DCURVEPOINT pt;
} G3DCUBICHANDLE;*/

/******************************************************************************/
typedef struct _G3DCUBICSEGMENT {
    G3DCURVESEGMENT seg; /*** OOP part ***/
    G3DCUBICHANDLE handle[0x02];
} G3DCUBICSEGMENT;

/******************************************************************************/
/*typedef struct _G3DQUADRATICHANDLE {
    G3DCURVEPOINT pt;
} G3DQUADRATICHANDLE;*/

/******************************************************************************/
typedef struct _G3DQUADRATICSEGMENT {
    G3DCURVESEGMENT seg; /*** OOP part***/
    G3DQUADRATICHANDLE handle;
} G3DQUADRATICSEGMENT;

/******************************************************************************/
G3DCURVEPOINT *g3dcurve_getConnectablePoint ( G3DCURVE *curve );
void g3dcurve_cut ( G3DCURVE *curve, 
                    G3DFACE  *knife, 
                    LIST    **laddedPoints,
                    LIST    **laddedSegments,
                    LIST    **lremovedSegments,
                    uint32_t  engine_flags );
void g3dcurve_roundSelectedPoints ( G3DCURVE *curve );
void g3dcubicsegment_getHandlePositionFromList ( LIST *lseg, 
                                                 G3DVECTOR (*pos)[0x02] );
void g3dcubicsegment_setHandlePositionFromList ( LIST *lseg, 
                                                 G3DVECTOR (*pos)[0x02] );
LIST *g3dcurve_getSegmentsFromSelectedPoints ( G3DCURVE *curve );
LIST *g3dcurve_getSelectedPoints ( G3DCURVE *curve );
void g3dcurve_deletePoints ( G3DCURVE  *curve,
                             LIST      *lremovedPoints,
                             LIST     **lremovedSegments,
                             uint32_t   engine_flags );
void g3dcurve_revert ( G3DCURVE *curve,
                        uint32_t   engine_flags );
void g3dcurvepoint_free ( G3DCURVEPOINT *pt );
void g3dcurvepoint_roundCubicSegments ( G3DCURVEPOINT *pt );
void g3dcurvepoint_addSegment ( G3DCURVEPOINT   *pt, 
                                 G3DCURVESEGMENT *seg );
void g3dcurvepoint_removeSegment ( G3DCURVEPOINT   *pt, 
                                    G3DCURVESEGMENT *seg );
G3DCURVEPOINT *g3dcurvepoint_new ( float x, float y, float z );
void g3dcubicsegment_free ( G3DCUBICSEGMENT *seg );
void g3dcubicsegment_getPoint ( G3DCUBICSEGMENT *csg, 
                                float            factor, /* range 0 - 1 */
                                G3DVECTOR       *pout );

G3DCURVESEGMENT *g3dcurve_seekSegment ( G3DCURVE *curve, 
                                        G3DCURVEPOINT *p0,
                                        G3DCURVEPOINT *p1 );
G3DCUBICSEGMENT *g3dcubicsegment_new ( G3DCURVEPOINT *pt0,
                                       G3DCURVEPOINT *pt1,
                                       float           hx1,
                                       float           hy1,
                                       float           hz1,
                                       float           hx2,
                                       float           hy2,
                                       float           hz2 );
void g3dquadraticsegment_getPoint ( G3DQUADRATICSEGMENT *qsg, 
                                    float                factor, /* range 0 - 1 */
                                    G3DVECTOR           *pout );
void g3dquadraticsegment_free ( G3DQUADRATICSEGMENT *csg );
void g3dquadraticsegment_init ( G3DQUADRATICSEGMENT *qsg,
                                G3DCURVEPOINT       *pt0,
                                G3DCURVEPOINT       *pt1,
                                float                hx, 
                                float                hy,
                                float                hz );
G3DQUADRATICSEGMENT *g3dquadraticsegment_new ( G3DCURVEPOINT *pt0,
                                               G3DCURVEPOINT *pt1,
                                               float           hx, 
                                               float           hy,
                                               float           hz );
G3DCURVE *g3dcurve_copy ( G3DCURVE *curve, uint32_t engine_flags );
void g3dcurve_free ( G3DOBJECT *obj );
void g3dcurve_addPoint ( G3DCURVE      *curve, 
                         G3DCURVEPOINT *pt );
void g3dcurve_removePoint ( G3DCURVE      *curve, 
                            G3DCURVEPOINT *pt );
void g3dcurve_addSegment ( G3DCURVE        *curve, 
                            G3DCURVESEGMENT *seg );
void g3dcurve_removeSegment ( G3DCURVE        *curve, 
                               G3DCURVESEGMENT *seg );
G3DCURVE *g3dcurve_new ( uint32_t type, 
                         uint32_t engine_flags );
void g3dcurve_unselectPoint ( G3DCURVE *curve, G3DCURVEPOINT *pt );
void g3dcurve_selectPoint ( G3DCURVE *curve, G3DCURVEPOINT *pt );
void g3dcurvepoint_unsetSelected ( G3DCURVEPOINT *pt );
void g3dcurve_unselectAllPoints ( G3DCURVE *curve );
void g3dcurve_addSelectedPoint ( G3DCURVE      *curve, 
                                 G3DCURVEPOINT *pt );
void g3dcurve_drawSegments ( G3DCURVE *curve, uint32_t engine_flags );
void g3dcurve_drawPoints ( G3DCURVE *curve, uint32_t engine_flags );
void g3dcurve_drawHandles ( G3DCURVE *curve, uint32_t engine_flags );
void g3dcurve_draw ( G3DCURVE *curve, uint32_t engine_flags );
void g3dcurvesegment_draw ( G3DCURVESEGMENT *seg,
                             float             from, /* range 0 - 1 */
                             float             to,   /* range 0 - 1 */
                             float             nbSteps,
                             GLUtesselator    *tobj,
                             double          (*coords)[0x03],
                             uint32_t          spline_flags,
                             uint32_t          engine_flags );

void g3dcurve_pickSegments ( G3DCURVE *curve, uint32_t engine_flags );
void g3dcurve_pickPoints ( G3DCURVE *curve, uint32_t engine_flags );
void g3dcurve_pickHandles ( G3DCURVE *curve, uint32_t engine_flags );
void g3dcurve_pick ( G3DCURVE *curve, uint32_t engine_flags );
void g3dcurvesegment_pick ( G3DCURVESEGMENT *seg,
                            float             from,
                            float             to,
                            float             nbSteps,
                            uint32_t          engine_flags );
G3DCURVEPOINT *g3dcurve_getLastSelectedPoint ( G3DCURVE *curve);
void g3dcurve_getSurroundingPoints ( G3DCURVE       *curve, 
                                     G3DCURVEPOINT  *pt, 
                                     G3DCURVEPOINT **prevPt, 
                                     G3DCURVEPOINT **nextPt );
void g3dcurve_insertPointWithinSegment ( G3DCURVE        *curve, 
                                         G3DCURVEPOINT   *pt,
                                         G3DCURVESEGMENT *seg,
                                         LIST           **laddedSegments,
                                         LIST           **lremovedSegments );

#ifdef __cplusplus
}
#endif

#endif
