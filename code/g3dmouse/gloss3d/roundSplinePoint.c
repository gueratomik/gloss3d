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
#include <g3dmouse.h>

/******************************************************************************/
/* each function must return FALSE for redrawing the OGL Widget it belongs to */
/* only or TRUE to redraw all OGL Widgets                                     */
/******************************************************************************/

static uint32_t roundSplinePoint_init  ( G3DMOUSETOOL *mou,
                                         G3DSCENE     *sce, 
                                         G3DURMANAGER *urm, 
                                         uint64_t engine_flags );

/******************************************************************************/
G3DMOUSETOOLROUNDSPLINEPOINT *g3dmousetoolroundsplinepoint_new ( ) {
    void *memarea = calloc ( 0x01, sizeof ( G3DMOUSETOOLROUNDSPLINEPOINT ) );
    G3DMOUSETOOLROUNDSPLINEPOINT *sp = ( G3DMOUSETOOLROUNDSPLINEPOINT * ) memarea;

    if ( sp == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( sp,
                        ROUNDSPLINEPOINTTOOL,
                        's',
                        NULL,
                        roundSplinePoint_init,
                        NULL,
                        NULL,
                        MOUSETOOLNOCURRENT );

    return sp;
}

/******************************************************************************/
static uint32_t roundSplinePoint_init  ( G3DMOUSETOOL *mou,
                                         G3DSCENE     *sce,
                                         G3DURMANAGER *urm,
                                         uint64_t engine_flags ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( ( obj ) && ( obj->type == G3DSPLINETYPE ) ) {
        G3DSPLINE *spline = ( G3DSPLINE * ) obj;

        g3durm_spline_roundSelectedPoints ( urm, 
                                            sce,
                                            spline, 
                                            engine_flags, 
                                            REDRAWVIEW );

        return REDRAWVIEW;
    }

    return 0x00;
}
