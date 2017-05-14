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
void common_g3duisculpttooledit_setRadiusCbk ( G3DUI *gui, int radius ) {
    G3DMOUSETOOL *mou = common_g3dui_getMouseTool ( gui, SCULPTTOOL );

    if ( gui->lock ) return;

    if ( mou ) {
        G3DSCULPTTOOL *st = mou->data;

        st->radius = radius;
    }
}

/******************************************************************************/
void common_g3duisculpttooledit_setPressureCbk ( G3DUI *gui, float pressure ) {
    G3DMOUSETOOL *mou = common_g3dui_getMouseTool ( gui, SCULPTTOOL );

    if ( gui->lock ) return;

    if ( mou ) {
        G3DSCULPTTOOL *st = mou->data;

        st->pressure = pressure;
    }
}

/******************************************************************************/
void common_g3duisculpttooledit_onlyVisibleCbk ( G3DUI *gui, int visible_only ) {
    G3DMOUSETOOL *mou = common_g3dui_getMouseTool ( gui, SCULPTTOOL );

    if ( gui->lock ) return;

    if ( mou ) {
        G3DSCULPTTOOL *st = mou->data;

        st->only_visible = visible_only;
    }
}
