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
void common_g3duitextedit_setTextCbk ( G3DUI *gui, char *text ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DTEXTTYPE ) ) {
        G3DTEXT *txt = ( G3DTEXT * ) obj;
        g3dui_setHourGlass ( gui );

        g3dtext_setText ( txt, text, gui->flags );

        g3dui_unsetHourGlass ( gui );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3duitextedit_thicknessCbk ( G3DUI *gui, float thickness ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DTEXTTYPE ) ) {
        G3DTEXT *txt = ( G3DTEXT * ) obj;
        g3dui_setHourGlass ( gui );

        g3dtext_setThickness ( txt, thickness, gui->flags );

        g3dui_unsetHourGlass ( gui );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3duitextedit_roundnessCbk ( G3DUI *gui, uint32_t roundness ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DTEXTTYPE ) ) {
        G3DTEXT *txt = ( G3DTEXT * ) obj;

        g3dui_setHourGlass ( gui );

        g3dtext_setRoundness ( txt, roundness, gui->flags );

        g3dui_unsetHourGlass ( gui );

        g3dui_redrawGLViews ( gui );
    }
}
