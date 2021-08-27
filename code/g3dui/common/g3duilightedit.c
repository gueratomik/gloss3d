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
void common_g3dui_lightSpecularityChangeCbk ( G3DUI    *gui, 
                                              G3DLIGHT *lig,
                                              uint32_t  red,
                                              uint32_t  green,
                                              uint32_t  blue ) {
    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    lig->specularColor.r = red;
    lig->specularColor.g = green;
    lig->specularColor.b = blue;
}

/******************************************************************************/
void common_g3duilightedit_unsetSpotCbk ( G3DUI    *gui,
                                          G3DLIGHT *lig ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;

    g3dlight_unsetSpot ( lig );
}

/******************************************************************************/
void common_g3duilightedit_setSpotCbk ( G3DUI    *gui,
                                        G3DLIGHT *lig,
                                        float     spotLength,
                                        float     spotAngle,
                                        float     spotFadeAngle ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;;

    g3dlight_setSpot ( lig, spotLength, spotAngle, spotFadeAngle );
}

/******************************************************************************/
void common_g3duilightedit_castShadowsCbk ( G3DUI    *gui,
                                            G3DLIGHT *lig ) {
    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( lig->obj.flags & LIGHTCASTSHADOWS ) {
        lig->obj.flags &= (~LIGHTCASTSHADOWS);
    } else {
        lig->obj.flags |= LIGHTCASTSHADOWS;
    }
}

/******************************************************************************/
void common_g3dui_lightDiffuseChangeCbk ( G3DUI    *gui, 
                                          G3DLIGHT *lig,
                                          uint32_t  red,
                                          uint32_t  green,
                                          uint32_t  blue ) {
    /*** prevent a loop when updating widget ***/
    if ( gui->lock ) return;

    lig->diffuseColor.r = red;
    lig->diffuseColor.g = green;
    lig->diffuseColor.b = blue;
}
