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
uint64_t g3duipicktooledit_paintRadiusCbk ( G3DUIPICKTOOLEDIT *ptedit,
                                            float              radius ) {
    G3DUI *gui = ptedit->gui;
    G3DMOUSETOOL *mou = gui->mou;
    G3DMOUSETOOLPICK *pt = ( G3DMOUSETOOLPICK * ) mou;

    if ( pt ) {
        pt->radius = radius;
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duipicktooledit_setWeightCbk ( G3DUIPICKTOOLEDIT *ptedit,
                                          float              weight ) {
    G3DUI *gui = ptedit->gui;
    G3DMOUSETOOL *mou = gui->mou;
    G3DMOUSETOOLPICK *pt = ( G3DMOUSETOOLPICK * ) mou;

    if ( pt ) {
        pt->weight = weight;
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duipicktooledit_onlyVisibleCbk ( G3DUIPICKTOOLEDIT *ptedit, 
                                            int                visible_only ) {
    G3DUI *gui = ptedit->gui;
    G3DMOUSETOOL *mou = gui->mou;
    G3DMOUSETOOLPICK *pt = ( G3DMOUSETOOLPICK * ) mou;

    if ( pt ) {
        pt->only_visible = visible_only;
    }

    return 0x00;
}
