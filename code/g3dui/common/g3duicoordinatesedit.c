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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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
                                                      float val ) {
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    /*** prevent a loop ***/
    if ( gui->lock ) return;

    if ( obj ) {
        g3dui_setHourGlass ( gui );

        if ( gui->flags & VIEWOBJECT ) {
            G3DVECTOR oldpos, oldrot, oldsca;

            memcpy ( &oldpos, &obj->pos, sizeof ( G3DVECTOR ) );
            memcpy ( &oldrot, &obj->rot, sizeof ( G3DVECTOR ) );
            memcpy ( &oldsca, &obj->sca, sizeof ( G3DVECTOR ) );

            if ( axis == G3DUIXAXIS ) obj->pos.x = val;
            if ( axis == G3DUIYAXIS ) obj->pos.y = val;
            if ( axis == G3DUIZAXIS ) obj->pos.z = val;

            g3dobject_updateMatrix_r ( obj, gui->flags );

            g3durm_object_move ( urm, obj, &oldpos,  
                                           &oldrot, 
                                           &oldsca, REDRAWVIEW );
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

        if ( gui->flags & VIEWOBJECT ) {
            G3DVECTOR oldpos, oldrot, oldsca;

            memcpy ( &oldpos, &obj->pos, sizeof ( G3DVECTOR ) );
            memcpy ( &oldrot, &obj->rot, sizeof ( G3DVECTOR ) );
            memcpy ( &oldsca, &obj->sca, sizeof ( G3DVECTOR ) );

            if ( axis == G3DUIXAXIS ) obj->rot.x = val;
            if ( axis == G3DUIYAXIS ) obj->rot.y = val;
            if ( axis == G3DUIZAXIS ) obj->rot.z = val;

            g3dobject_updateMatrix_r ( obj, gui->flags );

            g3durm_object_move ( urm, obj, &oldpos,  
                                           &oldrot, 
                                           &oldsca, REDRAWVIEW );
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

        if ( gui->flags & VIEWOBJECT ) {
            G3DVECTOR oldpos, oldrot, oldsca;

            memcpy ( &oldpos, &obj->pos, sizeof ( G3DVECTOR ) );
            memcpy ( &oldrot, &obj->rot, sizeof ( G3DVECTOR ) );
            memcpy ( &oldsca, &obj->sca, sizeof ( G3DVECTOR ) );

            if ( axis == G3DUIXAXIS ) obj->sca.x = val;
            if ( axis == G3DUIYAXIS ) obj->sca.y = val;
            if ( axis == G3DUIZAXIS ) obj->sca.z = val;

            g3dobject_updateMatrix_r ( obj, gui->flags );

            g3durm_object_move ( urm, obj, &oldpos,  
                                           &oldrot, 
                                           &oldsca, REDRAWVIEW );
        }

        g3dui_unsetHourGlass ( gui );

        g3dui_redrawGLViews ( gui );
    }
}
