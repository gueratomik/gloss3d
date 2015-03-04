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
static void xslicbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DFFDTYPE ) ) {
        G3DFFD *ffd = ( G3DFFD * ) obj;
        char *value = XmTextGetString ( w );
        int nbx = strtol ( value, NULL, 10 );

        g3dffd_shape ( ffd, nbx, ffd->nby, ffd->nbz, ffd->locmax.x,
                                                     ffd->locmax.y,
                                                     ffd->locmax.z );

        redrawGLViews ( gui );
        XtFree ( value );
    }
}

/******************************************************************************/
static void yslicbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DFFDTYPE ) ) {
        G3DFFD *ffd = ( G3DFFD * ) obj;
        char *value = XmTextGetString ( w );
        int nby = strtol ( value, NULL, 10 );

        g3dffd_shape ( ffd, ffd->nbx, nby, ffd->nbz, ffd->locmax.x,
                                                     ffd->locmax.y,
                                                     ffd->locmax.z );

        redrawGLViews ( gui );
        XtFree ( value );
    }
}

/******************************************************************************/
static void zslicbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DFFDTYPE ) ) {
        G3DFFD *ffd = ( G3DFFD * ) obj;
        char *value = XmTextGetString ( w );
        int nbz = strtol ( value, NULL, 10 );

        g3dffd_shape ( ffd, ffd->nbx, ffd->nby, nbz, ffd->locmax.x,
                                                     ffd->locmax.y,
                                                     ffd->locmax.z );

        redrawGLViews ( gui );
        XtFree ( value );
    }
}

/******************************************************************************/
static void xradcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DFFDTYPE ) ) {
        G3DFFD *ffd = ( G3DFFD * ) obj;
        char *value = XmTextGetString ( w );
        float xradius = strtof ( value, NULL );

        g3dffd_shape ( ffd, ffd->nbx, ffd->nby, ffd->nbz, xradius,
                                                          ffd->locmax.y,
                                                          ffd->locmax.z );

        redrawGLViews ( gui );
        XtFree ( value );
    }
}

/******************************************************************************/
static void yradcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DFFDTYPE ) ) {
        G3DFFD *ffd = ( G3DFFD * ) obj;
        char *value = XmTextGetString ( w );
        float yradius = strtof ( value, NULL );

        g3dffd_shape ( ffd, ffd->nbx, ffd->nby, ffd->nbz, ffd->locmax.x,
                                                          yradius,
                                                          ffd->locmax.z );

        redrawGLViews ( gui );
        XtFree ( value );
    }
}

/******************************************************************************/
static void zradcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DFFDTYPE ) ) {
        G3DFFD *ffd = ( G3DFFD * ) obj;
        char *value = XmTextGetString ( w );
        float zradius = strtof ( value, NULL );

        g3dffd_shape ( ffd, ffd->nbx, ffd->nby, ffd->nbz, ffd->locmax.x,
                                                          ffd->locmax.y,
                                                          zradius );

        redrawGLViews ( gui );
        XtFree ( value );
    }
}


/******************************************************************************/
void updateFFDEdit ( Widget w ) {
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

            if ( XtClass ( child ) == xmTextWidgetClass ) {
                if ( strcmp ( name, EDITFFDXSLICES   ) == 0x00 ) {
                    char buf[0x10];
                    snprintf ( buf, 0x10, "%d", ffd->nbx );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITFFDYSLICES ) == 0x00 ) {
                    char buf[0x10];
                    snprintf ( buf, 0x10, "%d", ffd->nby );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITFFDZSLICES ) == 0x00 ) {
                    char buf[0x10];
                    snprintf ( buf, 0x10, "%d", ffd->nbz );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITFFDXRADIUS ) == 0x00 ) {
                    char buf[0x10];
                    snprintf ( buf, 0x10, "%f", ffd->locmax.x );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITFFDYRADIUS ) == 0x00 ) {
                    char buf[0x10];
                    snprintf ( buf, 0x10, "%f", ffd->locmax.y );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITFFDZRADIUS ) == 0x00 ) {
                    char buf[0x10];
                    snprintf ( buf, 0x10, "%f", ffd->locmax.z );

                    XmTextSetString ( child, buf );
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
                                   XmNuserData, gui,
                                   NULL );

    createIntegerText ( frm, EDITFFDXSLICES, 0x00, 0x00, 0x60, 0x20, xslicbk );
    createIntegerText ( frm, EDITFFDYSLICES, 0x00, 0x14, 0x60, 0x20, yslicbk );
    createIntegerText ( frm, EDITFFDZSLICES, 0x00, 0x28, 0x60, 0x20, zslicbk );
    createFloatText   ( frm, EDITFFDXRADIUS, 0x00, 0x3C, 0x60, 0x60, xradcbk );
    createFloatText   ( frm, EDITFFDYRADIUS, 0x00, 0x50, 0x60, 0x60, yradcbk );
    createFloatText   ( frm, EDITFFDZRADIUS, 0x00, 0x64, 0x60, 0x60, zradcbk );

    XtManageChild ( frm );


    return frm;
}
