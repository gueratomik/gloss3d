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
static void xsliceCbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget parent = XtParent ( w );
    int slice = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;

    if ( slice ) {
        common_g3duicubeedit_sliceCbk ( gui, G3DUIXAXIS, slice );
    } else {
        updateCubeEdit ( parent, gui );
    }
}

/******************************************************************************/
static void ysliceCbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget parent = XtParent ( w );
    int slice = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;

    if ( slice ) {
        common_g3duicubeedit_sliceCbk ( gui, G3DUIYAXIS, slice );
    } else {
        updateCubeEdit ( parent, gui );
    }
}

/******************************************************************************/
static void zsliceCbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget parent = XtParent ( w );
    int slice = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;

    if ( slice ) {
        common_g3duicubeedit_sliceCbk ( gui, G3DUIZAXIS, slice );
    } else {
        updateCubeEdit ( parent, gui );
    }
}

/******************************************************************************/
static void radiusCbk ( Widget w, XtPointer client, XtPointer call ) {
    float radius = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duicubeedit_radiusCbk ( gui, radius );
}

/******************************************************************************/
void updateCubeEdit ( Widget w, G3DUI *gui ) {
    WidgetList children;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DCUBETYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        CUBEDATASTRUCT *cds = ( CUBEDATASTRUCT * ) pri->data;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );
            char buf[0x10];

            if ( XtClass ( child ) == xmSpinButtonWidgetClass ) {
                if ( strcmp ( name, EDITCUBEXSLICES   ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%d", cds->nbx );

                    XtVaSetValues ( child, XmNvalue, buf, NULL );
                }

                if ( strcmp ( name, EDITCUBEYSLICES ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%d", cds->nby );

                    XtVaSetValues ( child, XmNvalue, buf, NULL );
                }

                if ( strcmp ( name, EDITCUBEZSLICES ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%d", cds->nbz );

                    XtVaSetValues ( child, XmNvalue, buf, NULL );
                }

                if ( strcmp ( name, EDITCUBERADIUS ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%f", cds->radius );

                    XtVaSetValues ( child, XmNvalue, buf, NULL );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
Widget createCubeEdit ( Widget parent, G3DUI *gui, char *name,
                        Dimension x    , Dimension y,
                        Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget frm, cap, sli, rad;
    Pixel white = XWhitePixel ( XtDisplay ( parent ), 0x00 );

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

    createIntegerText ( frm, gui, EDITCUBEXSLICES, 0x00, 0x00, 
                                                   0x60, 0x20, xsliceCbk );

    createIntegerText ( frm, gui, EDITCUBEYSLICES, 0x00, 0x14, 
                                                   0x60, 0x20, ysliceCbk );

    createIntegerText ( frm, gui, EDITCUBEZSLICES, 0x00, 0x28, 
                                                   0x60, 0x20, zsliceCbk );

    createFloatText   ( frm, gui, EDITCUBERADIUS , 0x00, 0x3C, 
                                                   0x60, 0x60, radiusCbk );

    XtManageChild ( frm );


    return frm;
}
