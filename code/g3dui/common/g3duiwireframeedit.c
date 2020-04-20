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
void common_g3duiwireframeedit_thicknessCbk ( G3DUI *gui, float thickness ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DWIREFRAMETYPE ) ) {
        G3DWIREFRAME *wir = ( G3DWIREFRAME * ) obj;

        g3dui_setHourGlass ( gui );

        g3dwireframe_setThickness ( wir, thickness, gui->flags );

        g3dui_unsetHourGlass ( gui );
        /*g3dmesh_setSubdivisionLevel ( mes, level, gui->flags );*/
    }

    g3dui_redrawGLViews ( gui );
}
