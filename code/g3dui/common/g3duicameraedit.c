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
#include <g3dui.h>

/******************************************************************************/
void common_g3duicameraedit_dofRadiusCbk ( G3DUI *gui, uint32_t radius ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DCAMERATYPE ) ) {
        G3DCAMERA *cam = ( G3DCAMERA * ) obj;

        cam->dof.radius = radius;

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3duicameraedit_dofNearBlurCbk ( G3DUI *gui, float nearBlur ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DCAMERATYPE ) ) {
        G3DCAMERA *cam = ( G3DCAMERA * ) obj;

        cam->dof.nearBlur = nearBlur;

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3duicameraedit_dofNoBlurCbk ( G3DUI *gui, float noBlur ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DCAMERATYPE ) ) {
        G3DCAMERA *cam = ( G3DCAMERA * ) obj;

        cam->dof.noBlur = noBlur;

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3duicameraedit_dofFarBlurCbk ( G3DUI *gui, float farBlur ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DCAMERATYPE ) ) {
        G3DCAMERA *cam = ( G3DCAMERA * ) obj;

        cam->dof.farBlur = farBlur;

        g3dui_redrawGLViews ( gui );
    }
}


/******************************************************************************/
void common_g3duicameraedit_dofEnableCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DCAMERATYPE ) ) {
        G3DCAMERA *cam = ( G3DCAMERA * ) obj;

        if ( obj->flags & CAMERADOF ) {
            obj->flags &= (~CAMERADOF);
        } else {
            obj->flags |= CAMERADOF;
        }

        g3dui_redrawGLViews ( gui );
    }
}
