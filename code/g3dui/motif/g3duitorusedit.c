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
static void slicecbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    Widget parent = XtParent ( w );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DTORUSTYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;
        char *value = XmTextGetString ( w );
        int slice = strtol ( value, NULL, 10 );

        g3dui_setHourGlass ( gui );

        if ( slice >= 0x03 ) {
            g3dtorus_build ( pri, tds->orientation, 
                                  slice,
                                  tds->cap,
                                  tds->extrad, 
                                  tds->intrad );
        } else {
            updateTorusEdit ( parent );
        }

        g3dui_unsetHourGlass ( gui );

        g3dui_redrawGLViews ( gui );
        XtFree ( value );
    }
}

/******************************************************************************/
static void capcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    Widget parent = XtParent ( w );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DTORUSTYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;
        char *value = XmTextGetString ( w );
        int cap = strtol ( value, NULL, 10 );

        g3dui_setHourGlass ( gui );

        if ( cap >= 0x03 ) {
            g3dtorus_build ( pri, tds->orientation, 
                                  tds->slice,
                                  cap,
                                  tds->extrad, 
                                  tds->intrad );
        } else {
            updateTorusEdit ( parent );
        }

        g3dui_unsetHourGlass ( gui );

        g3dui_redrawGLViews ( gui );
        XtFree ( value );
    }
}

/******************************************************************************/
static void extradcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DTORUSTYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;
        char *value = XmTextGetString ( w );
        float extrad = strtof ( value, NULL );

        g3dui_setHourGlass ( gui );

        g3dtorus_build ( pri, tds->orientation, 
                              tds->slice,
                              tds->cap,
                              extrad, 
                              tds->intrad );

        g3dui_unsetHourGlass ( gui );

        g3dui_redrawGLViews ( gui );
        XtFree ( value );
    }
}

/******************************************************************************/
static void intradcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DTORUSTYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;
        char *value = XmTextGetString ( w );
        float intrad = strtof ( value, NULL );

        g3dui_setHourGlass ( gui );

        g3dtorus_build ( pri, tds->orientation, 
                              tds->slice,
                              tds->cap,
                              tds->extrad, 
                              intrad );

        g3dui_unsetHourGlass ( gui );

        g3dui_redrawGLViews ( gui );
        XtFree ( value );
    }
}

/******************************************************************************/
static void orientationcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmComboBoxCallbackStruct *cbs = ( XmComboBoxCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    char *str;

    XmStringGetLtoR ( cbs->item_or_text, XmFONTLIST_DEFAULT_TAG, &str );

    if ( obj && ( obj->type == G3DTORUSTYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        TORUSDATASTRUCT *tds = ( TORUSDATASTRUCT * ) pri->data;
        uint32_t orientation;

        if ( strcmp ( str, ZXSTR ) == 0x00 ) {
            orientation = TORUSZX;
        }

        if ( strcmp ( str, XYSTR ) == 0x00 ) {
            orientation = TORUSXY;
        }

        if ( strcmp ( str, YZSTR ) == 0x00 ) {
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

    XtFree ( str );
}

/******************************************************************************/
void updateTorusEdit ( Widget w ) {
    WidgetList children;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    G3DUI *gui;

    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    sce = gui->sce;
    obj = g3dscene_getSelectedObject ( sce );

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

    createIntegerText ( frm, EDITTORUSSLICE , 0x00, 0x00, 0x60, 0x20, slicecbk  );
    createIntegerText ( frm, EDITTORUSCAP   , 0x00, 0x14, 0x60, 0x20, capcbk    );
    createFloatText   ( frm, EDITTORUSEXTRAD, 0x00, 0x28, 0x60, 0x60, extradcbk );
    createFloatText   ( frm, EDITTORUSINTRAD, 0x00, 0x3C, 0x60, 0x60, intradcbk );
    createOrientationSelection ( frm, EDITTORUSORIENTATION, 0x00, 0x50, 0x60, 0x60, orientationcbk );

    XtManageChild ( frm );


    return frm;
}
