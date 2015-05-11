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
static void sliceCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );
    char *value = XmTextGetString ( w );
    int slice = strtol ( value, NULL, 10 );

    if ( slice >= 0x03 ) {
        common_g3duitorusedit_sliceCbk ( gui, slice );
    } else {
        updateTorusEdit ( parent, gui );
    }

    XtFree ( value );
}

/******************************************************************************/
static void capCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );
    char *value = XmTextGetString ( w );
    int cap = strtol ( value, NULL, 10 );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( cap >= 0x03 ) {
        common_g3duitorusedit_sliceCbk ( gui, cap );
    } else {
        updateTorusEdit ( parent, gui );
    }

    XtFree ( value );
}

/******************************************************************************/
static void extRadiusCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    char *value = XmTextGetString ( w );
    float rad = strtof ( value, NULL );

    common_g3duitorusedit_extRadiusCbk ( gui, rad );

    XtFree ( value );
}

/******************************************************************************/
static void intRadiusCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    char *value = XmTextGetString ( w );
    float rad = strtof ( value, NULL );

    common_g3duitorusedit_intRadiusCbk ( gui, rad );

    XtFree ( value );
}

/******************************************************************************/
static void orientationCbk ( Widget w, XtPointer client, XtPointer call ) {
    XmComboBoxCallbackStruct *cbs = ( XmComboBoxCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    char *str;

    XmStringGetLtoR ( cbs->item_or_text, XmFONTLIST_DEFAULT_TAG, &str );

    common_g3duitorusedit_orientationCbk ( gui, str );

    XtFree ( str );
}

/******************************************************************************/
void updateTorusEdit ( Widget w, G3DUI *gui ) {
    WidgetList children;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DTORUSTYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );

            if ( XtClass ( child ) == xmTextWidgetClass ) {
                if ( strcmp ( name, EDITTORUSSLICE   ) == 0x00 ) {
                    char buf[0x10];
                    snprintf ( buf, 0x10, "%d", tds->slice );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITTORUSCAP ) == 0x00 ) {
                    char buf[0x10];
                    snprintf ( buf, 0x10, "%d", tds->cap );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITTORUSEXTRAD ) == 0x00 ) {
                    char buf[0x10];
                    snprintf ( buf, 0x10, "%f", tds->extrad );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITTORUSINTRAD ) == 0x00 ) {
                    char buf[0x10];
                    snprintf ( buf, 0x10, "%f", tds->intrad );

                    XmTextSetString ( child, buf );
                }
            }

            if ( XtClass ( child ) == xmComboBoxWidgetClass ) {
                if ( strcmp ( name, EDITTORUSORIENTATION   ) == 0x00 ) {
                    XtVaSetValues ( child, XmNselectedPosition,
                                           tds->orientation, NULL );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
Widget createTorusEdit ( Widget parent, G3DUI *gui, char *name,
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
                                   XmNuserData, gui,
                                   NULL );

    createIntegerText ( frm, gui, EDITTORUSSLICE ,  0, 0,
                                                   96, 32, sliceCbk  );

    createIntegerText ( frm, gui, EDITTORUSCAP   ,  0, 20, 
                                                   96, 32, capCbk    );

    createFloatText   ( frm, gui, EDITTORUSEXTRAD,  0, 40,
                                                   96, 96, extRadiusCbk );

    createFloatText   ( frm, gui, EDITTORUSINTRAD,  0, 60, 
                                                   96, 96, intRadiusCbk );

    createOrientationSelection ( frm, gui, EDITTORUSORIENTATION,  0, 80,
                                                                 96, 96, orientationCbk );

    updateTorusEdit ( frm, gui );

    XtManageChild ( frm );


    return frm;
}
