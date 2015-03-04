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
static void radcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DSPHERETYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        SPHEREDATASTRUCT *sds = ( SPHEREDATASTRUCT * ) pri->data;
        char *value = XmTextGetString ( w );
        float radius = strtof ( value, NULL );

        g3dui_setHourGlass ( gui );

        g3dsphere_build ( pri, sds->slice, sds->cap, radius );

        g3dui_unsetHourGlass ( gui );

        redrawGLViews ( gui );
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

    if ( obj && ( obj->type == G3DSPHERETYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        SPHEREDATASTRUCT *sds = ( SPHEREDATASTRUCT * ) pri->data;
        char *value = XmTextGetString ( w );
        int cap = strtol ( value, NULL, 10 );

        g3dui_setHourGlass ( gui );

        if ( cap >= 0x02 ) {
            g3dsphere_build ( pri, sds->slice, cap, sds->radius );
        } else {
            updateSphereEdit ( parent );
        }

        g3dui_unsetHourGlass ( gui );

        redrawGLViews ( gui );
        XtFree ( value );
    }
}

/******************************************************************************/
static void slicbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    Widget parent = XtParent ( w );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DSPHERETYPE ) ) {
        G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) obj;
        SPHEREDATASTRUCT *sds = ( SPHEREDATASTRUCT * ) pri->data;
        char *value = XmTextGetString ( w );
        int slice = strtol ( value, NULL, 10 );

        g3dui_setHourGlass ( gui );

        if ( slice >= 0x03 ) {
            g3dsphere_build ( pri, slice, sds->cap, sds->radius );
        } else {
            updateSphereEdit ( parent );
        }

        g3dui_unsetHourGlass ( gui );

        redrawGLViews ( gui );
        XtFree ( value );
    }
}

/******************************************************************************/
void updateSphereEdit ( Widget w ) {
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

            if ( XtClass ( child ) == xmTextWidgetClass ) {
                if ( strcmp ( name, EDITSPHERECAPS   ) == 0x00 ) {
                    char buf[0x10];
                    snprintf ( buf, 0x10, "%d", sds->cap );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITSPHERESLICES ) == 0x00 ) {
                    char buf[0x10];
                    snprintf ( buf, 0x10, "%d", sds->slice );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITSPHERERADIUS ) == 0x00 ) {
                    char buf[0x10];
                    snprintf ( buf, 0x10, "%f", sds->radius );

                    XmTextSetString ( child, buf );
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

    createIntegerText ( frm, EDITSPHERECAPS  , 0x00, 0x00, 0x60, 0x20, capcbk );
    createIntegerText ( frm, EDITSPHERESLICES, 0x00, 0x14, 0x60, 0x20, slicbk );
    createFloatText   ( frm, EDITSPHERERADIUS, 0x00, 0x28, 0x60, 0x60, radcbk );

    XtManageChild ( frm );


    return frm;
}
