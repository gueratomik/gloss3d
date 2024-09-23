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
typedef struct _URMCUTSPLINE {
    G3DSCENE *sce;
    G3DSPLINE *spline;
    LIST      *laddedPoints;
    LIST      *laddedSegments;
    LIST      *lremovedSegments;
} URMCUTSPLINE;

/******************************************************************************/
static URMCUTSPLINE *urmCutSpline_new ( G3DSCENE  *sce,
                                        G3DSPLINE *spline, 
                                        LIST      *laddedPoints,
                                        LIST      *laddedSegments,
                                        LIST      *lremovedSegments ) {
    uint32_t size = sizeof ( URMCUTSPLINE );
    URMCUTSPLINE *csp = ( URMCUTSPLINE * ) calloc ( 0x01, size );

    if ( csp == NULL ) {
        fprintf ( stderr, "urmCutSpline_new: memory allocation failed\n" );

        return NULL;
    }

    csp->sce              = sce;
    csp->spline           = spline;
    csp->laddedPoints     = laddedPoints;
    csp->laddedSegments   = laddedSegments;
    csp->lremovedSegments = lremovedSegments;

    return csp;
}

/******************************************************************************/
static void urmCutSpline_free ( URMCUTSPLINE *csp ) {
    free ( csp );
}

/******************************************************************************/
static void cutSpline_free ( void    *data, 
                             uint32_t commit ) {
    URMCUTSPLINE *csp = ( URMCUTSPLINE * ) data;

    /*** Discard changes ***/
    if ( commit == 0x00 ) {
        list_free ( &csp->laddedSegments  , LIST_FUNCDATA(g3dcubicsegment_free) );
        list_free ( &csp->laddedPoints    , LIST_FUNCDATA(g3dcurvepoint_free)  );
        list_free ( &csp->lremovedSegments, NULL  );
    } else {
        list_free ( &csp->laddedSegments  , NULL  );
        list_free ( &csp->laddedPoints    , NULL  );
        list_free ( &csp->lremovedSegments, LIST_FUNCDATA(g3dcubicsegment_free) );
    }

    urmCutSpline_free ( csp );
}

/******************************************************************************/
static void cutSpline_undo ( G3DURMANAGER *urm, 
                             void         *data, 
                             uint64_t      engine_flags ) {
    URMCUTSPLINE *csp = ( URMCUTSPLINE * ) data;
    G3DSPLINE *spline = ( G3DSPLINE * ) csp->spline;

    /* remove deleted segments */
    list_execargdata ( csp->laddedSegments  , LIST_FUNCARGDATA(g3dcurve_removeSegment), spline->curve );
    /* remove added points */
    list_execargdata ( csp->laddedPoints    , LIST_FUNCARGDATA(g3dcurve_removePoint)  , spline->curve );
    /* restore deleted segments */
    list_execargdata ( csp->lremovedSegments, LIST_FUNCARGDATA(g3dcurve_addSegment)   , spline->curve );

    /*** Rebuild the spline modifiers ***/
    g3dobject_update_r ( G3DOBJECTCAST(csp->sce), 0x00, engine_flags );
}

/******************************************************************************/
static void cutSpline_redo ( G3DURMANAGER *urm, 
                             void         *data, 
                             uint64_t      engine_flags ) {
    URMCUTSPLINE *csp = ( URMCUTSPLINE * ) data;
    G3DSPLINE *spline = ( G3DSPLINE * ) csp->spline;

    /* remove deleted segments */
    list_execargdata ( csp->lremovedSegments, LIST_FUNCARGDATA(g3dcurve_removeSegment), spline->curve );
    /* re-add new points */
    list_execargdata ( csp->laddedPoints    , LIST_FUNCARGDATA(g3dcurve_addPoint)     , spline->curve );
    /* re-add new segments */
    list_execargdata ( csp->laddedSegments  , LIST_FUNCARGDATA(g3dcurve_addSegment)   , spline->curve );

    g3dobject_update_r ( G3DOBJECTCAST(csp->sce), 0x00, engine_flags );
}

/******************************************************************************/
void g3durm_spline_cut ( G3DURMANAGER *urm,
                         G3DSCENE     *sce, 
                         G3DSPLINE    *spline, 
                         G3DFACE      *knife,
                         uint64_t      engine_flags,
                         uint32_t      return_flags ) {
    URMCUTSPLINE *csp;
    LIST *laddedPoints = NULL;
    LIST *laddedSegments = NULL;
    LIST *lremovedSegments = NULL;

    g3dcurve_cut ( spline->curve, 
                    knife,
                   &laddedPoints,
                   &laddedSegments,
                   &lremovedSegments, 
                    engine_flags );

    g3dobject_update_r ( G3DOBJECTCAST(csp->sce), 0x00, engine_flags );

    csp = urmCutSpline_new ( sce,
                             spline,
                             laddedPoints,
                             laddedSegments,
                             lremovedSegments );

    g3durmanager_push ( urm,
                        cutSpline_undo,
                        cutSpline_redo,
                        cutSpline_free, csp, return_flags );
}
