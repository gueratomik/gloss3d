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
#include <g3dui.h>

/******************************************************************************/
void common_g3duicoordinatesedit_posCbk ( G3DUI *gui, G3DUIAXIS axis, 
                                                      uint32_t absolute,
                                                      float val ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    /*** prevent a loop ***/
    if ( gui->lock ) return;

    if ( obj ) {
        g3dui_setHourGlass ( gui );

        if ( gui->engine_flags & VIEWOBJECT ) {
            URMTRANSFORMOBJECT *uto = g3durm_object_transform ( urm,
                                                                sce->lsel,
                                                                UTOSAVETRANSLATION,
                                                                REDRAWVIEW );

            urmtransform_saveState ( uto, UTOSAVESTATEBEFORE );

            if ( axis == G3DUIXAXIS ) obj->pos.x = val;
            if ( axis == G3DUIYAXIS ) obj->pos.y = val;
            if ( axis == G3DUIZAXIS ) obj->pos.z = val;

            g3dobject_updateMatrix_r ( obj, gui->engine_flags );

            urmtransform_saveState ( uto, UTOSAVESTATEAFTER );
        }

        if ( gui->engine_flags & VIEWVERTEX ) {
            if ( obj->type == G3DMESHTYPE ) {
                G3DMESH *mes = ( G3DMESH * ) obj;
                uint32_t axis_flags = 0x00;
                G3DVECTOR *oldpos = NULL;
                G3DVECTOR *newpos = NULL;
                G3DVECTOR avg;
                G3DVECTOR to;

                g3dvertex_getAveragePositionFromList ( mes->lselver, &avg );

                if ( axis == G3DUIXAXIS ) { to.x = val; axis_flags |= XAXIS; }
                if ( axis == G3DUIYAXIS ) { to.y = val; axis_flags |= YAXIS; }
                if ( axis == G3DUIZAXIS ) { to.z = val; axis_flags |= ZAXIS; }

                g3dvertex_copyPositionFromList ( mes->lselver, &oldpos );

                g3dmesh_moveVerticesTo ( mes, mes->lselver, &avg, &to, absolute, axis_flags, gui->engine_flags );

                g3dvertex_copyPositionFromList ( mes->lselver, &newpos );

                g3durm_mesh_moveVertexList ( gui->urm, mes, mes->lselver, NULL, NULL, NULL,
                                             oldpos, 
                                             newpos, 
                                             REDRAWVIEW );
            }
        }

        g3dui_unsetHourGlass ( gui );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3duicoordinatesedit_rotCbk ( G3DUI *gui, G3DUIAXIS axis, 
                                                      float val ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    /*** prevent a loop ***/
    if ( gui->lock ) return;

    if ( obj ) {
        g3dui_setHourGlass ( gui );

        if ( gui->engine_flags & VIEWOBJECT ) {
            URMTRANSFORMOBJECT *uto = g3durm_object_transform ( urm,
                                                                sce->lsel,
                                                                UTOSAVEROTATION,
                                                                REDRAWVIEW );

            urmtransform_saveState ( uto, UTOSAVESTATEBEFORE );

            if ( axis == G3DUIXAXIS ) obj->rot.x = val;
            if ( axis == G3DUIYAXIS ) obj->rot.y = val;
            if ( axis == G3DUIZAXIS ) obj->rot.z = val;

            g3dobject_updateMatrix_r ( obj, gui->engine_flags );

            urmtransform_saveState ( uto, UTOSAVESTATEAFTER );
        }

        g3dui_unsetHourGlass ( gui );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3duicoordinatesedit_scaCbk ( G3DUI *gui, G3DUIAXIS axis, 
                                                      float val ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    /*** prevent a loop ***/
    if ( gui->lock ) return;

    if ( obj ) {
        g3dui_setHourGlass ( gui );

        if ( gui->engine_flags & VIEWOBJECT ) {
            URMTRANSFORMOBJECT *uto = g3durm_object_transform ( urm,
                                                                sce->lsel,
                                                                UTOSAVESCALING,
                                                                REDRAWVIEW );

            if ( axis == G3DUIXAXIS ) obj->sca.x = val;
            if ( axis == G3DUIYAXIS ) obj->sca.y = val;
            if ( axis == G3DUIZAXIS ) obj->sca.z = val;

            g3dobject_updateMatrix_r ( obj, gui->engine_flags );

            urmtransform_saveState ( uto, UTOSAVESTATEAFTER );
        }

        g3dui_unsetHourGlass ( gui );

        g3dui_redrawGLViews ( gui );
    }
}
