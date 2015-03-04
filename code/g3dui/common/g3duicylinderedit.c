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
void common_g3duicylinderedit_lengthCbk ( G3DUI *gui, float length ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DCYLINDERTYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        CYLINDERDATASTRUCT *cds = ( CYLINDERDATASTRUCT * ) pri->data;

        g3dui_setHourGlass ( gui );

        g3dcylinder_build ( pri, cds->slice, cds->capx,
                                             cds->capy, cds->radius, length );

        g3dui_unsetHourGlass ( gui );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3duicylinderedit_radiusCbk ( G3DUI *gui, float radius ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DCYLINDERTYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        CYLINDERDATASTRUCT *cds = ( CYLINDERDATASTRUCT * ) pri->data;

        g3dui_setHourGlass ( gui );

        g3dcylinder_build ( pri, cds->slice, cds->capx,
                                             cds->capy, radius, cds->length );

        g3dui_unsetHourGlass ( gui );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3duicylinderedit_capCbk ( G3DUI *gui, G3DUIAXIS axis, int cap ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DCYLINDERTYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        CYLINDERDATASTRUCT *cds = ( CYLINDERDATASTRUCT * ) pri->data;

        g3dui_setHourGlass ( gui );

        if ( axis == G3DUIXAXIS ) g3dcylinder_build ( pri, cds->slice, 
                                                           cap,
                                                           cds->capy,
                                                           cds->radius,
                                                           cds->length );

        if ( axis == G3DUIYAXIS ) g3dcylinder_build ( pri, cds->slice, 
                                                           cds->capy,
                                                           cap,
                                                           cds->radius,
                                                           cds->length );

        g3dui_unsetHourGlass ( gui );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3duicylinderedit_sliceCbk ( G3DUI *gui, int slice ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DCYLINDERTYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        CYLINDERDATASTRUCT *cds = ( CYLINDERDATASTRUCT * ) pri->data;

        g3dui_setHourGlass ( gui );

        g3dcylinder_build ( pri, slice, cds->capx, 
                                        cds->capy, 
                                        cds->radius,
                                        cds->length );

        g3dui_unsetHourGlass ( gui );

        g3dui_redrawGLViews ( gui );
    }
}
