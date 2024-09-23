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
typedef struct _URMREVERTSPLINE{
    G3DSCENE  *sce;
    G3DSPLINE *spline;
} URMREVERTSPLINE;

/******************************************************************************/
static URMREVERTSPLINE *urmRevertSpline_new ( G3DSCENE  *sce,
                                              G3DSPLINE *spline ) {
    uint32_t size = sizeof ( URMREVERTSPLINE );

    URMREVERTSPLINE *rsp = ( URMREVERTSPLINE * ) calloc ( 0x01, size );

    if ( rsp == NULL ) {
        fprintf ( stderr, "urmRevertSpline_new: memory allocation falied\n" );

        return NULL;
    }

    rsp->sce    = sce;
    rsp->spline = spline;


    return rsp; /* hehe */
}

/******************************************************************************/
static void revertSpline_free ( void *data, uint32_t commit ) {
    URMREVERTSPLINE *rsp = ( URMREVERTSPLINE * ) data;

    free ( rsp );
}

/******************************************************************************/
static void revertSpline_undo ( G3DURMANAGER *urm, 
                                void         *data, 
                                uint64_t engine_flags ) {
    URMREVERTSPLINE *rsp = ( URMREVERTSPLINE * ) data;

    g3dcurve_revert ( rsp->spline->curve, engine_flags );

    g3dobject_update_r ( G3DOBJECTCAST(rsp->sce), 0x00, engine_flags );
}

/******************************************************************************/
static void revertSpline_redo ( G3DURMANAGER *urm, 
                                void         *data,
                                uint64_t engine_flags ) {
    URMREVERTSPLINE *rsp = ( URMREVERTSPLINE * ) data;

    g3dcurve_revert ( rsp->spline->curve, engine_flags );

    g3dobject_update_r ( G3DOBJECTCAST(rsp->sce), 0x00, engine_flags );
}

/******************************************************************************/
void g3durm_spline_revert ( G3DURMANAGER *urm,
                            G3DSCENE     *sce,
                            G3DSPLINE    *spline,
                            uint64_t engine_flags,
                            uint32_t      return_flags ) {
    URMREVERTSPLINE *rsp;

    g3dcurve_revert ( spline->curve, engine_flags );

    g3dobject_update_r ( G3DOBJECTCAST(sce), 0x00, engine_flags );

    /* remember it for undoing */
    rsp = urmRevertSpline_new ( sce, spline );

    g3durmanager_push ( urm, revertSpline_undo,
                             revertSpline_redo,
                             revertSpline_free, rsp, return_flags );
}
