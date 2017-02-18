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
static void lengthCbk ( Widget w, XtPointer client, XtPointer call ) {
    float length = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duicylinderedit_lengthCbk ( gui, length );
}

/******************************************************************************/
static void radiusCbk ( Widget w, XtPointer client, XtPointer call ) {
    float radius = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duicylinderedit_radiusCbk ( gui, radius );
}

/******************************************************************************/
static void xcapCbk ( Widget w, XtPointer client, XtPointer call ) {
    int cap = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );

    if ( cap ) {
        common_g3duicylinderedit_capCbk ( gui, G3DUIXAXIS, cap );
    } else {
        updateCylinderEdit ( parent, gui );
    }
}

/******************************************************************************/
static void ycapCbk ( Widget w, XtPointer client, XtPointer call ) {
    int cap = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );

    if ( cap ) {
        common_g3duicylinderedit_capCbk ( gui, G3DUIYAXIS, cap );
    } else {
        updateCylinderEdit ( parent, gui );
    }
}

/******************************************************************************/
static void sliceCbk ( Widget w, XtPointer client, XtPointer call ) {
    int slice = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );

    if ( slice >= 0x03 ) {
        common_g3duicylinderedit_sliceCbk ( gui, slice );
    } else {
        updateCylinderEdit ( parent, gui );
    }
}

/******************************************************************************/
void updateCylinderEdit ( Widget w, G3DUI *gui ) {
    WidgetList children;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DCYLINDERTYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        CYLINDERDATASTRUCT *cds = ( CYLINDERDATASTRUCT * ) pri->data;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );

            if ( XtClass ( child ) == xmSpinButtonWidgetClass ) {
                if ( strcmp ( name, EDITCYLINDERXCAPS  ) == 0x00 ) {
                    XmSpinButtonSetValue ( child, cds->capx );
                }

                if ( strcmp ( name, EDITCYLINDERYCAPS  ) == 0x00 ) {
                    XmSpinButtonSetValue ( child, cds->capy );
                }

                if ( strcmp ( name, EDITCYLINDERSLICES ) == 0x00 ) {
                    XmSpinButtonSetValue ( child, cds->slice );
                }

                if ( strcmp ( name, EDITCYLINDERRADIUS ) == 0x00 ) {
                    XmSpinButtonSetValue ( child, cds->radius );
                }

                if ( strcmp ( name, EDITCYLINDERLENGTH ) == 0x00 ) {
                    XmSpinButtonSetValue ( child, cds->length );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
Widget createCylinderEdit ( Widget parent, G3DUI *gui, char *name,
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
                                   NULL );

    createIntegerText ( frm, gui, EDITCYLINDERXCAPS , 0x00, 0x00,
                                                      0x60, 0x20, xcapCbk   );

    /*createIntegerText ( frm, EDITCYLINDERYCAPS , 0, 20, 32, capycbk );*/
    createIntegerText ( frm, gui, EDITCYLINDERSLICES, 0x00, 0x14, 
                                                      0x60, 0x20, sliceCbk  );

    createFloatText   ( frm, gui, EDITCYLINDERLENGTH, 0x00, 0x28, 
                                                      0x60, 0x60, lengthCbk );

    createFloatText   ( frm, gui, EDITCYLINDERRADIUS, 0x00, 0x3C, 
                                                      0x60, 0x60, radiusCbk );


    XtManageChild ( frm );


    return frm;
}
