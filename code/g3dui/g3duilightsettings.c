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
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>

/******************************************************************************/
#include <math.h>

/******************************************************************************/
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

/******************************************************************************/
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

/******************************************************************************/
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

/******************************************************************************/
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>
#include <Xm/Manager.h>
#include <Xm/ManagerP.h>
#include <Xm/PrimitiveP.h>
#include <Xm/Scale.h>
#include <Xm/Label.h>
#include <Xm/Text.h>
#include <Xm/VaSimpleP.h>

/******************************************************************************/
#include <list.h>

/******************************************************************************/
#include <TabbedBook.h>
#include <ColorEdit.h>

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/GLwMDrawA.h>

/******************************************************************************/
#include <g3d.h>
#include <r3d.h>
#include <r3dnet.h>
#include <g3durmanager.h>

/******************************************************************************/
#include <g3dmouse.h>

/******************************************************************************/
#include <g3dui.h>

/******************************************************************************/
#include <MaterialEdit.h>

/******************************************************************************/
static void specularitychange ( uint32_t pixel, void *data ) {
    G3DUI *gui = ( G3DUI * ) data;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type == G3DLIGHTTYPE ) ) {
        G3DLIGHT *lig = ( G3DLIGHT * ) obj;

        uint32_t R = ( pixel & 0x00FF0000 ) >> 0x10,
                 G = ( pixel & 0x0000FF00 ) >> 0x08,
                 B = ( pixel & 0x000000FF );

        lig->speccol.r = R;
        lig->speccol.g = G;
        lig->speccol.b = B;

        redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void colorchange ( uint32_t pixel, void *data ) {
    G3DUI *gui = ( G3DUI * ) data;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj && ( obj->type == G3DLIGHTTYPE ) ) {
        G3DLIGHT *lig = ( G3DLIGHT * ) obj;

        uint32_t R = ( pixel & 0x00FF0000 ) >> 0x10,
                 G = ( pixel & 0x0000FF00 ) >> 0x08,
                 B = ( pixel & 0x000000FF );

        lig->diffcol.r = R;
        lig->diffcol.g = G;
        lig->diffcol.b = B;

        redrawGLViews ( gui );
    }
}

/******************************************************************************/
static void updateSpecularityPanel ( Widget w, G3DLIGHT *lig ) {
    Display *dis = XtDisplay ( w );
    Window win = XDefaultRootWindow ( dis );
    XWindowAttributes attr;
    WidgetList children;
    Cardinal nc;
    uint32_t i;
    G3DUI *gui;

    XGetWindowAttributes ( dis, win, &attr );

    XtVaGetValues ( w, XmNuserData, &gui,
                       XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( ( strcmp ( EDITLIGHTSPECULARITY, XtName ( child ) ) == 0x00 ) ) {
            uint32_t R = ( lig ) ? ( lig->speccol.r ) : 0x00,
                     G = ( lig ) ? ( lig->speccol.g ) : 0x00,
                     B = ( lig ) ? ( lig->speccol.b ) : 0x00;
            uint32_t pixel = 0x00,
                     color = ( R << 0x10 ) | ( G << 0x08 ) | B;

            switch ( attr.depth ) {
                case 0x10 :
                    R = ( R >> 0x03 );
                    G = ( G >> 0x02 );
                    B = ( B >> 0x03 );

                    pixel = color;
                break;

                case 0x18 :
                case 0x20 :
                    pixel = ( R << 0x10 ) | ( G << 0x08 ) | B;
                break;

                default :
                break;
            }

            XmColorEditLabelSetColor ( child, color );

            XtVaSetValues ( child, XmNbackground, pixel, NULL );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void updateColorPanel ( Widget w, G3DLIGHT *lig ) {
    Display *dis = XtDisplay ( w );
    Window win = XDefaultRootWindow ( dis );
    XWindowAttributes attr;
    WidgetList children;
    Cardinal nc;
    uint32_t i;
    G3DUI *gui;

    XGetWindowAttributes ( dis, win, &attr );

    XtVaGetValues ( w, XmNuserData, &gui,
                       XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( ( strcmp ( EDITLIGHTCOLOR      , XtName ( child ) ) == 0x00 ) ) {
            uint32_t R = ( lig ) ? ( lig->diffcol.r ) : 0x00,
                     G = ( lig ) ? ( lig->diffcol.g ) : 0x00,
                     B = ( lig ) ? ( lig->diffcol.b ) : 0x00;
            uint32_t pixel = 0x00,
                     color = ( R << 0x10 ) | ( G << 0x08 ) | B;

            switch ( attr.depth ) {
                case 0x10 :
                    R = ( R >> 0x03 );
                    G = ( G >> 0x02 );
                    B = ( B >> 0x03 );

                    pixel = color;
                break;

                case 0x18 :
                case 0x20 :
                    pixel = ( R << 0x10 ) | ( G << 0x08 ) | B;
                break;

                default :
                break;
            }

            XmColorEditLabelSetColor ( child, color );

            XtVaSetValues ( child, XmNbackground, pixel, NULL );
        }

        /*** Crashes if I don't use the if statement below. Don't know why ***/
        if ( XtClass ( child ) == xmTextWidgetClass ) {
            if ( strcmp ( EDITLIGHTINTENSITY, XtName ( child ) ) == 0x00 ) {
                char buf[0x10];
                snprintf ( buf, 0x10, "%f", lig->intensity );

                XmTextSetString ( child, buf );
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void radcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DLIGHTTYPE ) ) {
        G3DLIGHT *lig = ( G3DLIGHT * ) obj;
        char *value = XmTextGetString ( w );
        float intensity = strtof ( value, NULL );

        lig->intensity = intensity;

        redrawGLViews ( gui );

        XtFree ( value );
    }
}

/******************************************************************************/
static Widget createColorPanel ( G3DUI *gui, Widget parent,
                                 Dimension x    , Dimension y,
                                 Dimension width, Dimension height ) {
    Widget lab, col, pan;
    
    pan = XmVaCreateManagedForm ( parent, "Color",
                                  XmNx, x,
                                  XmNy, y,
                                  XmNwidth, width,
                                  XmNheight, height,
                                  XmNuserData, gui,
                                  NULL );

    lab = XmVaCreateManagedLabel ( pan, "Solid Color",
                                        XmNx, 0x00,
                                        XmNy, 0x04,
                                        XmNwidth, 0x60,
                                        XmNheight, 0x12,
                                        XmNfontList, gui->fontlist,
                                        NULL );

    col = XmCreateColorEditLabel ( pan, EDITLIGHTCOLOR,
                                        0x60, 0x04,
                                        0x20, 0x12,
                                        0x00, colorchange, gui );

    createFloatText ( pan, EDITLIGHTINTENSITY, 0x00, 0x30, 0x60, 0x60, radcbk );


    XtManageChild ( col );


    return pan;
}

/******************************************************************************/
static Widget createSpecularityPanel ( G3DUI *gui, Widget parent,
                                       Dimension x    , Dimension y,
                                       Dimension width, Dimension height ) {
    Widget lab, col, pan;
    
    pan = XmVaCreateManagedForm ( parent, "Specularity",
                                  XmNx, x,
                                  XmNy, y,
                                  XmNwidth, width,
                                  XmNheight, height,
                                  XmNuserData, gui,
                                  NULL );

    lab = XmVaCreateManagedLabel ( pan, "Solid Color",
                                        XmNx, 0x00,
                                        XmNy, 0x04,
                                        XmNwidth, 0x60,
                                        XmNheight, 0x12,
                                        XmNfontList, gui->fontlist,
                                        NULL );

    col = XmCreateColorEditLabel ( pan, EDITLIGHTSPECULARITY,
                                        0x60, 0x04,
                                        0x20, 0x12,
                                        0x00, specularitychange, gui );

    /*createFloatText ( pan, EDITLIGHTINTENSITY, 0x00, 0x30, 0x60, 0x60, radcbk );*/


    XtManageChild ( col );


    return pan;
}

/******************************************************************************/
void updateLightEdit ( Widget w, G3DLIGHT *lig ) {
    WidgetList children;
    Cardinal nc;
    uint32_t i;

    XtVaGetValues ( w, XmNchildren, &children, XmNnumChildren, &nc, NULL );

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( strcmp ( "Color", XtName ( child ) ) == 0x00 ) {
            updateColorPanel ( child, lig );
        }

        if ( strcmp ( "Specularity", XtName ( child ) ) == 0x00 ) {
            updateColorPanel ( child, lig );
        }
    }
}

/******************************************************************************/
Widget createLightEdit ( Widget parent, G3DUI *gui, char *name,
                         Dimension x    , Dimension y,
                         Dimension width, Dimension height ) {
    Widget lst;

    lst =  XmVaCreateManagedTabbedBook ( parent, name,
                                         XmNx, x,
                                         XmNy, y,
                                         XmNwidth, width,
                                         XmNheight, height,
                                         XmNfontList, gui->fontlist,
                                         NULL );

    createSpecularityPanel ( gui, lst, 0x00, 0x00, 0x100, 0x100 );
    createColorPanel       ( gui, lst, 0x00, 0x00, 0x100, 0x100 );


    return lst;
}
