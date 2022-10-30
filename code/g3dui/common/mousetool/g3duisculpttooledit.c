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
uint64_t g3duisculpttooledit_setCircularCbk ( G3DUISCULPTTOOLEDIT *scedit ) {
    G3DUI *gui = scedit->gui;
    G3DMOUSETOOL *tool = gui->curmou->tool;
    G3DMOUSETOOLSCULPT *sc = ( G3DMOUSETOOLSCULPT * ) tool;

    if ( sc ) {
        sc->circular = 0x01;
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duisculpttooledit_unsetCircularCbk ( G3DUISCULPTTOOLEDIT *scedit ) {
    G3DUI *gui = scedit->gui;
    G3DMOUSETOOL *tool = gui->curmou->tool;
    G3DMOUSETOOLSCULPT *sc = ( G3DMOUSETOOLSCULPT * ) tool;

    if ( sc ) {
        sc->circular = 0x00;
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duisculpttooledit_setRadiusCbk ( G3DUISCULPTTOOLEDIT *scedit,
                                            int                  radius ) {
    G3DUI *gui = scedit->gui;
    G3DMOUSETOOL *tool = gui->curmou->tool;
    G3DMOUSETOOLSCULPT *sc = ( G3DMOUSETOOLSCULPT * ) tool;

    if ( sc ) {
        sc->radius = radius;
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duisculpttooledit_setPressureCbk ( G3DUISCULPTTOOLEDIT *scedit,
                                              float                pressure ) {
    G3DUI *gui = scedit->gui;
    G3DMOUSETOOL *tool = gui->curmou->tool;
    G3DMOUSETOOLSCULPT *sc = ( G3DMOUSETOOLSCULPT * ) tool;


    if ( sc ) {
        sc->pressure = pressure;
    }

    return 0x00;
}

/******************************************************************************/
uint64_t g3duisculpttooledit_onlyVisibleCbk ( G3DUISCULPTTOOLEDIT *scedit,
                                              int                  visible ) {
    G3DUI *gui = scedit->gui;
    G3DMOUSETOOL *tool = gui->curmou->tool;
    G3DMOUSETOOLSCULPT *sc = ( G3DMOUSETOOLSCULPT * ) tool;

    if ( sc ) {
        sc->only_visible = visible;
    }

    return 0x00;
}
