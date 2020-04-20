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
URMDELETESPLINEPOINTS *urmDeleteSplinePoints_new ( G3DSPLINE *spline, 
                                                   LIST      *lremovedPoints,
                                                   LIST      *lremovedSegments ) {
    uint32_t size = sizeof ( URMDELETESPLINEPOINTS );

    URMDELETESPLINEPOINTS *dsp = ( URMDELETESPLINEPOINTS * ) calloc ( 0x01, 
                                                                      size );

    if ( dsp == NULL ) {
        fprintf ( stderr, "urmDeleteSplinePoints_new: memory allocation failed\n" );

        return NULL;
    }

    dsp->spline           = spline;
    dsp->lremovedPoints   = lremovedPoints;
    dsp->lremovedSegments = lremovedSegments;

    return dsp;
}

/******************************************************************************/
void urmDeleteSplinePoints_free ( URMDELETESPLINEPOINTS *dsp ) {
    free ( dsp );
}

/******************************************************************************/
void deleteSplinePoints_free ( void    *data, 
                               uint32_t commit ) {
    URMDELETESPLINEPOINTS *dsp = ( URMDELETESPLINEPOINTS * ) data;

    /*** Discard changes ***/
    if ( commit == 0x00 ) {
        list_free ( &dsp->lremovedSegments, NULL );
        list_free ( &dsp->lremovedPoints  , NULL );
    } else {
        list_free ( &dsp->lremovedSegments, g3dcubicsegment_free );
        list_free ( &dsp->lremovedPoints  , g3dcurvepoint_free  );
    }

    urmDeleteSplinePoints_free ( dsp );
}

/******************************************************************************/
void deleteSplinePoints_undo ( G3DURMANAGER *urm, 
                               void         *data, 
                               uint32_t      engine_flags ) {
    URMDELETESPLINEPOINTS *dsp = ( URMDELETESPLINEPOINTS * ) data;
    G3DSPLINE *spline = ( G3DSPLINE * ) dsp->spline;
    LIST *ltmpRemovedPoints   = dsp->lremovedPoints;
    LIST *ltmpRemovedSegments = dsp->lremovedSegments;

    /* restore deleted points */
    while ( ltmpRemovedPoints ) {
        G3DSPLINEPOINT *pt = ( G3DSPLINEPOINT * ) ltmpRemovedPoints->data;

        g3dcurve_addPoint ( spline->curve, pt );

        ltmpRemovedPoints = ltmpRemovedPoints->next;
    }

    /* restore deleted segments */
    while ( ltmpRemovedSegments ) {
        G3DSPLINESEGMENT *seg = ( G3DSPLINESEGMENT * ) ltmpRemovedSegments->data;

        g3dcurve_addSegment ( spline->curve, seg );

        ltmpRemovedSegments = ltmpRemovedSegments->next;
    }

    /*** Rebuild the spline modifiers ***/
    g3dspline_update ( spline, NULL, RESETMODIFIERS, engine_flags );
}

/******************************************************************************/
void deleteSplinePoints_redo ( G3DURMANAGER *urm, 
                               void         *data, 
                               uint32_t      engine_flags ) {
    URMDELETESPLINEPOINTS *dsp = ( URMDELETESPLINEPOINTS * ) data;
    G3DSPLINE *spline = ( G3DSPLINE * ) dsp->spline;
    LIST *ltmpRemovedPoints   = dsp->lremovedPoints;
    LIST *ltmpRemovedSegments = dsp->lremovedSegments;

    /* remove deleted points */
    while ( ltmpRemovedPoints ) {
        G3DSPLINEPOINT *pt = ( G3DSPLINEPOINT * ) ltmpRemovedPoints->data;

        g3dcurve_removePoint ( spline->curve, pt );

        ltmpRemovedPoints = ltmpRemovedPoints->next;
    }

    /* remove deleted segments */
    while ( ltmpRemovedSegments ) {
        G3DSPLINESEGMENT *seg = ( G3DSPLINESEGMENT * ) ltmpRemovedSegments->data;

        g3dcurve_removeSegment ( spline->curve, seg );

        ltmpRemovedSegments = ltmpRemovedSegments->next;
    }

    /*** Rebuild the spline modifiers ***/
    g3dspline_update ( spline, NULL, RESETMODIFIERS, engine_flags );
}

/******************************************************************************/
void g3durm_spline_deletePoints ( G3DURMANAGER *urm,
                                  G3DSPLINE    *spline, 
                                  LIST         *lremovedPoints,
                                  uint32_t      engine_flags,
                                  uint32_t      return_flags ) {
    URMDELETESPLINEPOINTS *dsp;
    LIST *lremovedSegments = NULL;

    g3dcurve_deletePoints ( spline->curve, 
                             lremovedPoints, 
                            &lremovedSegments, 
                             engine_flags );

    /*** Rebuild the spline modifiers ***/
    g3dspline_update ( spline, NULL, RESETMODIFIERS, engine_flags );

    dsp = urmDeleteSplinePoints_new ( spline, 
                                      lremovedPoints, 
                                      lremovedSegments );

    g3durmanager_push ( urm, deleteSplinePoints_undo,
                             deleteSplinePoints_redo,
                             deleteSplinePoints_free, dsp, return_flags );
}
