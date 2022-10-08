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
void common_g3dui_setMaterialCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    LIST *ltmpobj = sce->lsel;
    G3DMATERIAL *mat = gui->selmat;

    if ( mat ) {
        g3durm_selection_addTexture ( gui->urm,
                                      sce->lsel, 
                                      mat,
                                      gui->engine_flags,
                                      REDRAWVIEW | REDRAWOBJECTLIST );
    }

    common_g3duimenubar_fitUVMapCbk ( gui );

    g3dui_redrawGLViews ( gui );
}

