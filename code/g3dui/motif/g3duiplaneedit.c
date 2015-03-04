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
static void divuCbk ( Widget w, XtPointer client, XtPointer call ) {
    char  *value  = XmTextGetString ( w );
    int    div    = strtol ( value, NULL, 10 );
    Widget parent = XtParent ( w );
    G3DUI *gui    = ( G3DUI * ) client;

    if ( div ) {
        common_g3duiplaneedit_divCbk ( gui, G3DUIUAXIS, div );
    } else {
        updatePlaneEdit ( parent, gui );
    }

    XtFree ( value );
}

/******************************************************************************/
static void divvCbk ( Widget w, XtPointer client, XtPointer call ) {
    char  *value  = XmTextGetString ( w );
    int    div    = strtol ( value, NULL, 10 );
    Widget parent = XtParent ( w );
    G3DUI *gui    = ( G3DUI * ) client;

    if ( div ) {
        common_g3duiplaneedit_divCbk ( gui, G3DUIVAXIS, div );
    } else {
        updatePlaneEdit ( parent, gui );
    }

    XtFree ( value );
}

/******************************************************************************/
static void raduCbk ( Widget w, XtPointer client, XtPointer call ) {
    char *value = XmTextGetString ( w );
    float radius = strtof ( value, NULL );
    Widget parent = XtParent ( w );
    G3DUI *gui    = ( G3DUI * ) client;

    if ( radius >= 0.0f ) {
        common_g3duiplaneedit_radiusCbk ( gui, G3DUIUAXIS, radius );
    } else {
        updatePlaneEdit ( parent, gui );
    }

    XtFree ( value );
}

/******************************************************************************/
static void radvCbk ( Widget w, XtPointer client, XtPointer call ) {
    char *value = XmTextGetString ( w );
    float radius = strtof ( value, NULL );
    Widget parent = XtParent ( w );
    G3DUI *gui    = ( G3DUI * ) client;

    if ( radius >= 0.0f ) {
        common_g3duiplaneedit_radiusCbk ( gui, G3DUIVAXIS, radius );
    } else {
        updatePlaneEdit ( parent, gui );
    }

    XtFree ( value );
}

/******************************************************************************/
static void orientationCbk ( Widget w, XtPointer client, XtPointer call ) {
    XmComboBoxCallbackStruct *cbs = ( XmComboBoxCallbackStruct * ) call;
    Widget parent = XtParent ( w );
    G3DUI *gui    = ( G3DUI * ) client;
    char *oristr  = NULL;

    XmStringGetLtoR ( cbs->item_or_text, XmFONTLIST_DEFAULT_TAG, &oristr );

    if ( oristr ) {
        common_g3duiplaneedit_orientationcbk ( gui, oristr );
    } else {
        updatePlaneEdit ( parent, gui );
    }

    if ( oristr ) XtFree ( oristr );
}

/******************************************************************************/
void updatePlaneEdit ( Widget w, G3DUI *gui ) {
    WidgetList children;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DPLANETYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        PLANEDATASTRUCT *pds = ( PLANEDATASTRUCT * ) pri->data;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );

            if ( XtClass ( child ) == xmTextWidgetClass ) {
                if ( strcmp ( name, EDITPLANEDIVU   ) == 0x00 ) {
                    char buf[0x10];
                    snprintf ( buf, 0x10, "%d", pds->nbu );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITPLANEDIVV ) == 0x00 ) {
                    char buf[0x10];
                    snprintf ( buf, 0x10, "%d", pds->nbv );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITPLANERADU ) == 0x00 ) {
                    char buf[0x10];
                    snprintf ( buf, 0x10, "%f", pds->radu );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITPLANERADV ) == 0x00 ) {
                    char buf[0x10];
                    snprintf ( buf, 0x10, "%f", pds->radv );

                    XmTextSetString ( child, buf );
                }
            }

            if ( XtClass ( child ) == xmComboBoxWidgetClass ) {
                if ( strcmp ( name, EDITPLANEORIENTATION   ) == 0x00 ) {
                    XtVaSetValues ( child, XmNselectedPosition,
                                           pds->orientation, NULL );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
Widget createPlaneEdit ( Widget parent, G3DUI *gui, char *name,
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

    createIntegerText ( frm, gui, EDITPLANEDIVU, 0x00, 0x00,
                                                 0x60, 0x20, divuCbk );

    createIntegerText ( frm, gui, EDITPLANEDIVV, 0x00, 0x14, 
                                                 0x60, 0x20, divvCbk );

    createFloatText   ( frm, gui, EDITPLANERADU, 0x00, 0x28, 
                                                 0x60, 0x60, raduCbk );

    createFloatText   ( frm, gui, EDITPLANERADV, 0x00, 0x3C, 
                                                 0x60, 0x60, radvCbk );

    createOrientationSelection ( frm, gui, EDITPLANEORIENTATION, 0x00, 0x50, 
                                                                 0x60, 0x60, orientationCbk );

    XtManageChild ( frm );


    return frm;
}
