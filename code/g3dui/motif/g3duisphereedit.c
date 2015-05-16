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
#include <g3dui_motif.h>

/******************************************************************************/
static void radiusCbk ( Widget w, XtPointer client, XtPointer call ) {
    float radius = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duisphereedit_radiusCbk ( gui, radius );
}

/******************************************************************************/
static void capCbk ( Widget w, XtPointer client, XtPointer call ) {
    int cap = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );

    if ( cap >= 0x02 ) {
        common_g3duisphereedit_capCbk ( gui, cap );
    } else {
        updateSphereEdit ( parent, gui );
    }
}

/******************************************************************************/
static void sliceCbk ( Widget w, XtPointer client, XtPointer call ) {
    int slice = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );

    if ( slice >= 0x03 ) {
        common_g3duisphereedit_sliceCbk ( gui, slice );
    } else {
        updateSphereEdit ( parent, gui );
    }
}

/******************************************************************************/
void updateSphereEdit ( Widget w, G3DUI *gui ) {
    WidgetList children;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DSPHERETYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        SPHEREDATASTRUCT *sds = ( SPHEREDATASTRUCT * ) pri->data;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );

            if ( XtClass ( child ) == xmSpinButtonWidgetClass ) {
                if ( strcmp ( name, EDITSPHERECAPS   ) == 0x00 ) {
                    XmSpinButtonSetValue ( child, sds->cap );
                }

                if ( strcmp ( name, EDITSPHERESLICES ) == 0x00 ) {
                    XmSpinButtonSetValue ( child, sds->slice );
                }

                if ( strcmp ( name, EDITSPHERERADIUS ) == 0x00 ) {
                    XmSpinButtonSetValue ( child, sds->radius );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
Widget createSphereEdit ( Widget parent, G3DUI *gui, char *name,
                          Dimension x    , Dimension y,
                          Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget frm;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    frm = XmVaCreateForm ( parent, name,
                                   XtNx, x,
                                   XtNy, y,
                                   XtNwidth , width,
                                   XtNheight, height,
                                   XmNforeground, foreground,
                                   XmNbackground, background,
                                   XmNborderWidth, 0x00,
                                   XmNuserData, gui,
                                   NULL );

    createIntegerText ( frm, gui, EDITSPHERECAPS  , 0x00, 0x00,
                                                    0x60, 0x20, capCbk    );

    createIntegerText ( frm, gui, EDITSPHERESLICES, 0x00, 0x14, 
                                                    0x60, 0x20, sliceCbk  );

    createFloatText   ( frm, gui, EDITSPHERERADIUS, 0x00, 0x28,
                                                    0x60, 0x60, radiusCbk );

    XtManageChild ( frm );


    return frm;
}
