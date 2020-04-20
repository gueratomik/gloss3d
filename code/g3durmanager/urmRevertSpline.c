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
void revertSpline_free ( void *data, uint32_t commit ) {
    /* Nothing to do */
}

/******************************************************************************/
void revertSpline_undo ( G3DURMANAGER *urm, 
                         void         *data, 
                         uint32_t      engine_flags ) {
    G3DSPLINE *spline = ( G3DSPLINE * ) data;

    g3dcurve_revert ( spline->curve, engine_flags );
}

/******************************************************************************/
void revertSpline_redo ( G3DURMANAGER *urm, 
                         void         *data,
                         uint32_t      engine_flags ) {
    G3DSPLINE *spline = ( G3DSPLINE * ) data;

    g3dcurve_revert ( spline->curve, engine_flags );
}

/******************************************************************************/
void g3durm_spline_revert ( G3DURMANAGER *urm, 
                            G3DSPLINE    *spline,
                            uint32_t      engine_flags,
                            uint32_t      return_flags ) {
    g3dcurve_revert ( spline->curve, engine_flags );

    g3dspline_update ( spline, NULL, RESETMODIFIERS, engine_flags );

    g3durmanager_push ( urm, revertSpline_undo,
                             revertSpline_redo,
                             revertSpline_free, spline, return_flags );
}
