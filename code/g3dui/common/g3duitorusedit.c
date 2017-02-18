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
void common_g3duitorusedit_sliceCbk ( G3DUI *gui, int slice ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DTORUSTYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;

        g3dui_setHourGlass ( gui );

        g3dtorus_build ( pri, tds->orientation, 
                              slice,
                              tds->cap,
                              tds->extrad, 
                              tds->intrad );

        g3dui_unsetHourGlass ( gui ); 
        g3dui_redrawGLViews  ( gui );
    }
}

/******************************************************************************/
void common_g3duitorusedit_capCbk ( G3DUI *gui, int cap ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DTORUSTYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;

        g3dui_setHourGlass ( gui );

        g3dtorus_build ( pri, tds->orientation, 
                              tds->slice,
                              cap,
                              tds->extrad, 
                              tds->intrad );

        g3dui_unsetHourGlass ( gui );
        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3duitorusedit_extRadiusCbk ( G3DUI *gui, float extrad ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DTORUSTYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;

        g3dui_setHourGlass ( gui );

        g3dtorus_build ( pri, tds->orientation, 
                              tds->slice,
                              tds->cap,
                              extrad, 
                              tds->intrad );

        g3dui_unsetHourGlass ( gui );
        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3duitorusedit_intRadiusCbk ( G3DUI *gui, float intrad ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DTORUSTYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;

        g3dui_setHourGlass ( gui );

        g3dtorus_build ( pri, tds->orientation, 
                              tds->slice,
                              tds->cap,
                              tds->extrad, 
                              intrad );

        g3dui_unsetHourGlass ( gui );
        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3duitorusedit_orientationCbk ( G3DUI *gui, const char *oristr ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DTORUSTYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;
        uint32_t orientation;

        if ( strcmp ( oristr, ZXSTR ) == 0x00 ) {
            orientation = TORUSZX;
        }

        if ( strcmp ( oristr, XYSTR ) == 0x00 ) {
            orientation = TORUSXY;
        }

        if ( strcmp ( oristr, YZSTR ) == 0x00 ) {
            orientation = TORUSYZ;
        }

        g3dui_setHourGlass ( gui );

        g3dtorus_build ( pri, orientation,
                              tds->slice,
                              tds->cap,
                              tds->extrad, 
                              tds->intrad );

        g3dui_unsetHourGlass ( gui );        
        g3dui_redrawGLViews ( gui );
    }
}

