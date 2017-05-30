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
URMADDSPLINEPOINT *urmAddSplinePoint_new ( G3DSPLINE        *spline, 
                                           G3DSPLINEPOINT   *pt,
                                           G3DSPLINESEGMENT *seg ) {
    uint32_t size = sizeof ( URMADDSPLINEPOINT );

    URMADDSPLINEPOINT *asp = ( URMADDSPLINEPOINT * ) calloc ( 0x01, size );

    if ( asp == NULL ) {
        fprintf ( stderr, "urmAddSplinePoint_new: memory allocation falied\n" );

        return NULL;
    }

    asp->spline = spline;
    asp->pt     = pt;
    asp->seg    = seg;


    return asp;
}

/******************************************************************************/
void urmAddSplinePoint_free ( URMADDSPLINEPOINT *asp ) {
    free ( asp );
}

/******************************************************************************/
void addSplinePoint_free ( void *data, uint32_t commit ) {
    URMADDSPLINEPOINT *asp = ( URMADDSPLINEPOINT * ) data;

    /*** Discard changes ***/
    if ( commit == 0x00 ) {
        g3dsplinepoint_free ( asp->pt );

        if ( asp->seg ) g3dcubicsegment_free ( asp->seg );
    }

    urmAddSplinePoint_free ( asp );
}

/******************************************************************************/
void addSplinePoint_undo ( G3DURMANAGER *urm, 
                           void         *data,
                           uint32_t      engine_flags ) {
    URMADDSPLINEPOINT *asp = ( URMADDSPLINEPOINT * ) data;

    if ( asp->seg ) g3dspline_removeSegment ( asp->spline, asp->seg );

    g3dmesh_removeVertex ( asp->spline, asp->pt );

    /*** Rebuild the subdivided mesh ***/
    g3dmesh_update ( asp->spline, 
                     NULL,
                     NULL,
                     NULL,
                     RESETMODIFIERS, engine_flags );
}

/******************************************************************************/
void addSplinePoint_redo ( G3DURMANAGER *urm, 
                           void         *data,
                           uint32_t      engine_flags ) {
    URMADDSPLINEPOINT *asp = ( URMADDSPLINEPOINT * ) data;

    g3dmesh_addVertex ( asp->spline, asp->pt );

    if ( asp->seg ) g3dspline_addSegment ( asp->spline, asp->seg );

    /*** Rebuild the subdivided mesh ***/
    g3dmesh_update ( asp->spline, 
                     NULL,
                     NULL,
                     NULL,
                     RESETMODIFIERS, engine_flags );
}

/******************************************************************************/
void g3durm_spline_addPoint ( G3DURMANAGER     *urm,
                              G3DSPLINE        *spline, 
                              G3DSPLINEPOINT   *pt,
                              G3DSPLINESEGMENT *seg,
                              uint32_t          engine_flags,
                              uint32_t          return_flags ) {
    URMADDSPLINEPOINT *asp;

    /* perform the operation */
    g3dmesh_addSelectedVertex ( spline, pt );

    if ( seg ) {
        G3DSPLINEPOINT *previousPoint = ( seg->pt[0] == pt ) ? seg->pt[1] :
                                                               seg->pt[0];

        g3dspline_addSegment ( spline, seg );

        g3dsplinepoint_roundCubicSegments ( previousPoint );
    }

    /* remember it for undoing */
    asp = urmAddSplinePoint_new ( spline, pt, seg );

    g3durmanager_push ( urm, addSplinePoint_undo,
                             addSplinePoint_redo,
                             addSplinePoint_free, asp, return_flags );
}
