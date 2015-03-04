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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2014         */
/*                                                                            */
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
#include <Xm/Text.h>
#include <Xm/ComboBox.h>

/******************************************************************************/
#include <X11/xpm.h>

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/GLwMDrawA.h>

/******************************************************************************/
#include <list.h>
#include <g3d.h>
#include <r3d.h>
#include <r3dnet.h>
#include <g3durmanager.h>
#include <g3dmouse.h>
#include <g3dui.h>

/******************************************************************************/
static void divucbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    Widget parent = XtParent ( w );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DPLANETYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        PLANEDATASTRUCT *pds = ( PLANEDATASTRUCT * ) pri->data;
        char *value = XmTextGetString ( w );
        int div = strtol ( value, NULL, 10 );

        g3dui_setHourGlass ( gui );

        if ( div ) {
            g3dplane_build ( pri, pds->orientation, 
                                  div, 
                                  pds->nbv, 
                                  pds->radu, 
                                  pds->radv );
        } else {
            updatePlaneEdit ( parent );
        }

        g3dui_unsetHourGlass ( gui );

        redrawGLViews ( gui );
        XtFree ( value );
    }
}

/******************************************************************************/
static void divvcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    Widget parent = XtParent ( w );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DPLANETYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        PLANEDATASTRUCT *pds = ( PLANEDATASTRUCT * ) pri->data;
        char *value = XmTextGetString ( w );
        int div = strtol ( value, NULL, 10 );

        g3dui_setHourGlass ( gui );

        if ( div ) {
            g3dplane_build ( pri, pds->orientation, 
                                  pds->nbu, 
                                  div, 
                                  pds->radu, 
                                  pds->radv );
        } else {
            updatePlaneEdit ( parent );
        }

        g3dui_unsetHourGlass ( gui );

        redrawGLViews ( gui );
        XtFree ( value );
    }
}

/******************************************************************************/
static void raducbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DPLANETYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        PLANEDATASTRUCT *pds = ( PLANEDATASTRUCT * ) pri->data;
        char *value = XmTextGetString ( w );
        float radius = strtof ( value, NULL );

        g3dui_setHourGlass ( gui );

        g3dplane_build ( pri, pds->orientation, 
                              pds->nbu, 
                              pds->nbv, 
                              radius, 
                              pds->radv );

        g3dui_unsetHourGlass ( gui );

        redrawGLViews ( gui );
        XtFree ( value );
    }
}

/******************************************************************************/
static void radvcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DPLANETYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        PLANEDATASTRUCT *pds = ( PLANEDATASTRUCT * ) pri->data;
        char *value = XmTextGetString ( w );
        float radius = strtof ( value, NULL );

        g3dui_setHourGlass ( gui );

        g3dplane_build ( pri, pds->orientation, 
                              pds->nbu, 
                              pds->nbv, 
                              pds->radu, 
                              radius );

        g3dui_unsetHourGlass ( gui );

        redrawGLViews ( gui );
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

    if ( obj && ( obj->type == G3DPLANETYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        PLANEDATASTRUCT *pds = ( PLANEDATASTRUCT * ) pri->data;
        uint32_t orientation;

        if ( strcmp ( str, ZXSTR ) == 0x00 ) {
            orientation = PLANEZX;
        }

        if ( strcmp ( str, XYSTR ) == 0x00 ) {
            orientation = PLANEXY;
        }

        if ( strcmp ( str, YZSTR ) == 0x00 ) {
            orientation = PLANEYZ;
        }

        g3dplane_build ( pri, orientation, 
                              pds->nbu, 
                              pds->nbv, 
                              pds->radu, 
                              pds->radv );
        
        redrawGLViews ( gui );
    }

    XtFree ( str );
}

/******************************************************************************/
void updatePlaneEdit ( Widget w ) {
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
                                   XmNuserData, gui,
                                   NULL );

    createIntegerText ( frm, EDITPLANEDIVU, 0x00, 0x00, 0x60, 0x20, divucbk );
    createIntegerText ( frm, EDITPLANEDIVV, 0x00, 0x14, 0x60, 0x20, divvcbk );
    createFloatText   ( frm, EDITPLANERADU, 0x00, 0x28, 0x60, 0x60, raducbk );
    createFloatText   ( frm, EDITPLANERADV, 0x00, 0x3C, 0x60, 0x60, radvcbk );
    createOrientationSelection ( frm, EDITPLANEORIENTATION, 0x00, 0x50, 0x60, 0x60, orientationcbk );

    XtManageChild ( frm );


    return frm;
}
