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
#include <g3dui_motif.h>

/******************************************************************************/
static void xsliceCbk ( Widget w, XtPointer client, XtPointer call ) {
    int slice = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );

    if ( slice ) {
        common_g3duiffdedit_sliceCbk ( gui, G3DUIXAXIS, slice );
    } else {
        updateFFDEdit ( parent, gui );
    }
}

/******************************************************************************/
static void ysliceCbk ( Widget w, XtPointer client, XtPointer call ) {
    int slice = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );

    if ( slice ) {
        common_g3duiffdedit_sliceCbk ( gui, G3DUIYAXIS, slice );
    } else {
        updateFFDEdit ( parent, gui );
    }
}

/******************************************************************************/
static void zsliceCbk ( Widget w, XtPointer client, XtPointer call ) {
    int slice = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );

    if ( slice ) {
        common_g3duiffdedit_sliceCbk ( gui, G3DUIZAXIS, slice );
    } else {
        updateFFDEdit ( parent, gui );
    }
}

/******************************************************************************/
static void xradiusCbk ( Widget w, XtPointer client, XtPointer call ) {
    float radius = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );

    if ( radius >= 0.0f ) {
        common_g3duiffdedit_radiusCbk ( gui, G3DUIXAXIS, radius );
    } else {
        updateFFDEdit ( parent, gui );
    }
}

/******************************************************************************/
static void yradiusCbk ( Widget w, XtPointer client, XtPointer call ) {
    float radius = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );

    if ( radius >= 0.0f ) {
        common_g3duiffdedit_radiusCbk ( gui, G3DUIYAXIS, radius );
    } else {
        updateFFDEdit ( parent, gui );
    }
}

/******************************************************************************/
static void zradiusCbk ( Widget w, XtPointer client, XtPointer call ) {
    float radius = XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );

    if ( radius >= 0.0f ) {
        common_g3duiffdedit_radiusCbk ( gui, G3DUIZAXIS, radius );
    } else {
        updateFFDEdit ( parent, gui );
    }
}

/******************************************************************************/
void updateFFDEdit ( Widget w, G3DUI *gui ) {
    WidgetList children;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DFFDTYPE ) ) {
        G3DFFD *ffd = ( G3DFFD * ) obj;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );

            if ( XtClass ( child ) == xmSpinButtonWidgetClass ) {
                if ( strcmp ( name, EDITFFDXSLICES   ) == 0x00 ) {
                    XmSpinButtonSetValue ( child, ffd->nbx );
                }

                if ( strcmp ( name, EDITFFDYSLICES ) == 0x00 ) {
                    XmSpinButtonSetValue ( child, ffd->nby );
                }

                if ( strcmp ( name, EDITFFDZSLICES ) == 0x00 ) {
                    XmSpinButtonSetValue ( child, ffd->nbz );
                }

                if ( strcmp ( name, EDITFFDXRADIUS ) == 0x00 ) {
                    XmSpinButtonSetValue ( child, ffd->locmax.x );
                }

                if ( strcmp ( name, EDITFFDYRADIUS ) == 0x00 ) {
                    XmSpinButtonSetValue ( child, ffd->locmax.y );
                }

                if ( strcmp ( name, EDITFFDZRADIUS ) == 0x00 ) {
                    XmSpinButtonSetValue ( child, ffd->locmax.z );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
Widget createFFDEdit ( Widget parent, G3DUI *gui, char *name,
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

    createIntegerText ( frm, gui, EDITFFDXSLICES, 0,   0, 96, 32, xsliceCbk  );
    createIntegerText ( frm, gui, EDITFFDYSLICES, 0,  24, 96, 32, ysliceCbk  );
    createIntegerText ( frm, gui, EDITFFDZSLICES, 0,  48, 96, 32, zsliceCbk  );
    createFloatText   ( frm, gui, EDITFFDXRADIUS, 0,  72, 96, 96, xradiusCbk );
    createFloatText   ( frm, gui, EDITFFDYRADIUS, 0,  96, 96, 96, yradiusCbk );
    createFloatText   ( frm, gui, EDITFFDZRADIUS, 0, 120, 96, 96, zradiusCbk );

    XtManageChild ( frm );


    return frm;
}
