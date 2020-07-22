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
URMROUNDSPLINEPOINT *urmRoundSplinePoint_new ( G3DSPLINE  *spline,
                                               LIST       *lselectedPoints,
                                               LIST       *lsegments,
                                               G3DVECTOR (*pos)[0x02] ) {
    uint32_t size = sizeof ( URMROUNDSPLINEPOINT );

    URMROUNDSPLINEPOINT *rsp = ( URMROUNDSPLINEPOINT * ) calloc ( 0x01, size );

    if ( rsp == NULL ) {
        fprintf ( stderr, "urmRoundSplinePoint_new: memory allocation falied\n" );

        return NULL;
    }

    rsp->spline = spline;
    rsp->lselectedPoints = lselectedPoints;
    rsp->lsegments = lsegments;
    rsp->pos = pos;

    return rsp;
}

/******************************************************************************/
void urmRoundSplinePoint_free ( URMROUNDSPLINEPOINT *rsp ) {
    free ( rsp );
}

/******************************************************************************/
void roundSplinePoint_free ( void *data, uint32_t commit ) {
    URMROUNDSPLINEPOINT *rsp = ( URMROUNDSPLINEPOINT * ) data;

    list_free ( &rsp->lselectedPoints, NULL );
    list_free ( &rsp->lsegments, NULL );
    free ( rsp->pos );

    urmRoundSplinePoint_free ( rsp );
}

/******************************************************************************/
void roundSplinePoint_undo ( G3DURMANAGER *urm, 
                             void         *data,
                             uint64_t engine_flags ) {
    URMROUNDSPLINEPOINT *rsp = ( URMROUNDSPLINEPOINT * ) data;

    g3dcubicsegment_setHandlePositionFromList ( rsp->lsegments, rsp->pos );

    /*** Rebuild the subdivided mesh ***/
    g3dmesh_update ( rsp->spline, 
                     NULL,
                     NULL,
                     NULL,
                     UPDATEMODIFIERS, engine_flags );
}

/******************************************************************************/
void roundSplinePoint_redo ( G3DURMANAGER *urm, 
                             void         *data,
                             uint64_t engine_flags ) {
    URMROUNDSPLINEPOINT *rsp = ( URMROUNDSPLINEPOINT * ) data;
    LIST *lbackupSelectedPoints = ((G3DMESH*)rsp->spline)->lselver;

    ((G3DMESH*)rsp->spline)->lselver = rsp->lselectedPoints;

    g3dcurve_roundSelectedPoints ( rsp->spline->curve );

    ((G3DMESH*)rsp->spline)->lselver = lbackupSelectedPoints;

    /*** Rebuild the subdivided mesh ***/
    g3dmesh_update ( rsp->spline, 
                     NULL,
                     NULL,
                     NULL,
                     UPDATEMODIFIERS, engine_flags );
}

/******************************************************************************/
void g3durm_spline_roundSelectedPoints ( G3DURMANAGER     *urm,
                                         G3DSPLINE        *spline,
                                         uint64_t engine_flags,
                                         uint32_t          return_flags ) {
    LIST *lsegments = g3dcurve_getSegmentsFromSelectedPoints ( spline->curve );
    uint32_t nbseg = list_count ( lsegments );

    if ( nbseg ) {
        G3DVECTOR (*pos)[0x02] = calloc ( nbseg, sizeof ( G3DVECTOR ) * 0x02 );
        LIST *lselectedPoints = g3dcurve_getSelectedPoints ( spline->curve );
        URMROUNDSPLINEPOINT *rsp;

        /* backup handles position */
        g3dcubicsegment_getHandlePositionFromList ( lsegments, pos );

        /* then perform the operation */
        g3dcurve_roundSelectedPoints  ( spline->curve );



        /*** Rebuild the spline modifiers ***/
        g3dmesh_update ( spline, NULL,
                                 NULL,
                                 NULL,
                                 UPDATEMODIFIERS, engine_flags );

        /* remember it for undoing */
        rsp = urmRoundSplinePoint_new ( spline, 
                                        lselectedPoints, 
                                        lsegments, 
                                        pos );

        g3durmanager_push ( urm, roundSplinePoint_undo,
                                 roundSplinePoint_redo,
                                 roundSplinePoint_free, rsp, return_flags );
    }
}
