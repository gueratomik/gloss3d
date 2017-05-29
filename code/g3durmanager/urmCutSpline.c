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
#include <config.h>
#include <g3durmanager.h>

/******************************************************************************/
URMCUTSPLINE *urmCutSpline_new ( G3DSPLINE *spline, 
                                 LIST      *laddedPoints,
                                 LIST      *laddedSegments,
                                 LIST      *lremovedSegments ) {
    uint32_t size = sizeof ( URMCUTSPLINE );
    URMCUTSPLINE *csp = ( URMCUTSPLINE * ) calloc ( 0x01, size );

    if ( csp == NULL ) {
        fprintf ( stderr, "urmCutSpline_new: memory allocation failed\n" );

        return NULL;
    }

    csp->spline           = spline;
    csp->laddedPoints     = laddedPoints;
    csp->laddedSegments   = laddedSegments;
    csp->lremovedSegments = lremovedSegments;

    return csp;
}

/******************************************************************************/
void urmCutSpline_free ( URMCUTSPLINE *csp ) {
    free ( csp );
}

/******************************************************************************/
void cutSpline_free ( void    *data, 
                      uint32_t commit ) {
    URMCUTSPLINE *csp = ( URMCUTSPLINE * ) data;

    /*** Discard changes ***/
    if ( commit == 0x00 ) {
        list_free ( &csp->laddedSegments  , g3dcubicsegment_free );
        list_free ( &csp->laddedPoints    , g3dsplinepoint_free  );
        list_free ( &csp->lremovedSegments, NULL  );
    } else {
        list_free ( &csp->laddedSegments  , NULL  );
        list_free ( &csp->laddedPoints    , NULL  );
        list_free ( &csp->lremovedSegments, g3dcubicsegment_free );
    }

    urmCutSpline_free ( csp );
}

/******************************************************************************/
void cutSpline_undo ( G3DURMANAGER *urm, 
                      void         *data, 
                      uint32_t      engine_flags ) {
    URMCUTSPLINE *csp = ( URMCUTSPLINE * ) data;
    G3DSPLINE *spline = ( G3DSPLINE * ) csp->spline;

    /* remove deleted segments */
    list_execargdata ( csp->laddedSegments  , g3dspline_removeSegment, spline );
    /* remove added points */
    list_execargdata ( csp->laddedPoints    , g3dmesh_removeVertex   , spline );
    /* restore deleted segments */
    list_execargdata ( csp->lremovedSegments, g3dspline_addSegment   , spline );

    /*** Rebuild the spline modifiers ***/
    g3dmesh_update ( spline, NULL,
                             NULL,
                             NULL,
                             RESETMODIFIERS, engine_flags );
}

/******************************************************************************/
void cutSpline_redo ( G3DURMANAGER *urm, 
                      void         *data, 
                      uint32_t      engine_flags ) {
    URMCUTSPLINE *csp = ( URMCUTSPLINE * ) data;
    G3DSPLINE *spline = ( G3DSPLINE * ) csp->spline;

    /* remove deleted segments */
    list_execargdata ( csp->lremovedSegments, g3dspline_removeSegment, spline );
    /* re-add new points */
    list_execargdata ( csp->laddedPoints    , g3dmesh_addVertex      , spline );
    /* re-add new segments */
    list_execargdata ( csp->laddedSegments  , g3dspline_addSegment   , spline );

    /*** Rebuild the spline modifiers ***/
    g3dmesh_update ( spline, NULL,
                             NULL,
                             NULL,
                             RESETMODIFIERS, engine_flags );
}

/******************************************************************************/
void g3durm_spline_cut ( G3DURMANAGER *urm,
                         G3DSPLINE    *spline, 
                         G3DFACE      *knife,
                         uint32_t      engine_flags,
                         uint32_t      return_flags ) {
    URMCUTSPLINE *csp;
    LIST *laddedPoints = NULL;
    LIST *laddedSegments = NULL;
    LIST *lremovedSegments = NULL;

    g3dspline_cut ( spline, 
                    knife,
                   &laddedPoints,
                   &laddedSegments,
                   &lremovedSegments, 
                    engine_flags );

    csp = urmCutSpline_new ( spline,
                             laddedPoints,
                             laddedSegments,
                             lremovedSegments );

    g3durmanager_push ( urm, cutSpline_undo,
                             cutSpline_redo,
                             cutSpline_free, csp, return_flags );
}
