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
#include <g3durmanager.h>

/******************************************************************************/
typedef struct _URMADDSPLINESEGMENT {
    G3DSCENE *sce;
    G3DSPLINE        *spline;
    G3DSPLINESEGMENT *seg;
} URMADDSPLINESEGMENT;

/******************************************************************************/
static URMADDSPLINESEGMENT *urmAddSplineSegment_new ( G3DSCENE         *sce,
                                                      G3DSPLINE        *spline,
                                                      G3DSPLINESEGMENT *seg ) {
    uint32_t size = sizeof ( URMADDSPLINESEGMENT );

    URMADDSPLINESEGMENT *ass = ( URMADDSPLINESEGMENT * ) calloc ( 0x01, size );

    if ( ass == NULL ) {
        fprintf ( stderr, "urmAddSplinePoint_new: memory allocation falied\n" );

        return NULL;
    }

    ass->sce    = sce;
    ass->spline = spline;
    ass->seg    = seg;


    return ass; /* hehe */
}

/******************************************************************************/
static void urmAddSplineSegment_free ( URMADDSPLINESEGMENT *ass ) {
    free ( ass ); /* hehe */
}

/******************************************************************************/
static void addSplineSegment_free ( void *data, uint32_t commit ) {
    URMADDSPLINESEGMENT *ass = ( URMADDSPLINESEGMENT * ) data;

    /*** Discard changes ***/
    if ( commit == 0x00 ) {
        g3dcubicsegment_free ( ( G3DCUBICSEGMENT * ) ass->seg );
    }

    urmAddSplineSegment_free ( ass );
}

/******************************************************************************/
static void addSplineSegment_undo ( G3DURMANAGER *urm, 
                                    void         *data,
                                    uint64_t engine_flags ) {
    URMADDSPLINESEGMENT *ass = ( URMADDSPLINESEGMENT * ) data;

    g3dcurve_removeSegment ( ass->spline->curve, ass->seg );

    g3dobject_update_r ( G3DOBJECTCAST(ass->sce), 0x00, engine_flags );
}

/******************************************************************************/
static void addSplineSegment_redo ( G3DURMANAGER *urm, 
                                    void         *data,
                                    uint64_t engine_flags ) {
    URMADDSPLINESEGMENT *ass = ( URMADDSPLINESEGMENT * ) data;

    g3dcurve_addSegment ( ass->spline->curve, ass->seg );

    g3dobject_update_r ( G3DOBJECTCAST(ass->sce), 0x00, engine_flags );
}

/******************************************************************************/
void g3durm_spline_addSegment ( G3DURMANAGER     *urm,
                                G3DSCENE         *sce,
                                G3DSPLINE        *spline,
                                G3DSPLINESEGMENT *seg,
                                uint64_t engine_flags,
                                uint32_t          return_flags ) {
    URMADDSPLINESEGMENT *ass; /* hehe */

    g3dcurve_addSegment ( spline->curve, seg );

    g3dcurvepoint_roundCubicSegments ( seg->pt[0x00] );
    g3dcurvepoint_roundCubicSegments ( seg->pt[0x01] );

    g3dobject_update_r ( G3DOBJECTCAST(sce), 0x00, engine_flags );

    /* remember it for undoing */
    ass = urmAddSplineSegment_new ( sce, spline, seg );

    g3durmanager_push ( urm, addSplineSegment_undo,
                             addSplineSegment_redo,
                             addSplineSegment_free, ass, return_flags );
}
