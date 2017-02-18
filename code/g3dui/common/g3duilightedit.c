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
void common_g3dui_lightSpecularityChangeCbk ( G3DUI *gui, uint32_t red,
                                                          uint32_t green,
                                                          uint32_t blue ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DLIGHTTYPE ) ) {
        G3DLIGHT *lig = ( G3DLIGHT * ) obj;

        lig->speccol.r = red;
        lig->speccol.g = green;
        lig->speccol.b = blue;

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3duilightedit_castShadowsCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DLIGHTTYPE ) ) {
        G3DLIGHT *lig = ( G3DLIGHT * ) obj;

        if ( obj->flags & LIGHTCASTSHADOWS ) {
            obj->flags &= (~LIGHTCASTSHADOWS);
        } else {
            obj->flags |= LIGHTCASTSHADOWS;
        }
    }
}

/******************************************************************************/
void common_g3dui_lightDiffuseChangeCbk ( G3DUI *gui, uint32_t red,
                                                      uint32_t green,
                                                      uint32_t blue ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DLIGHTTYPE ) ) {
        G3DLIGHT *lig = ( G3DLIGHT * ) obj;

        lig->diffcol.r = red;
        lig->diffcol.g = green;
        lig->diffcol.b = blue;

        g3dui_redrawGLViews ( gui );
    }
}
