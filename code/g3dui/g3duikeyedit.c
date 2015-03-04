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
#include <Xm/Text.h>
#include <Xm/PrimitiveP.h>

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
static void keycbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj ) {
        char *value = XmTextGetString ( w );
        float fval = strtof ( value, NULL );
        LIST *ltmpkey = obj->lselkey;
        char *wname = XtName ( w );

        while ( ltmpkey ) {
            G3DKEY *key = ltmpkey->data;

            if ( strcmp ( wname, EDITKEYXPOSITION ) == 0x00 ) key->pos.x = fval;
            if ( strcmp ( wname, EDITKEYYPOSITION ) == 0x00 ) key->pos.y = fval;
            if ( strcmp ( wname, EDITKEYZPOSITION ) == 0x00 ) key->pos.z = fval;

            if ( strcmp ( wname, EDITKEYXROTATION ) == 0x00 ) key->rot.x = fval;
            if ( strcmp ( wname, EDITKEYYROTATION ) == 0x00 ) key->rot.y = fval;
            if ( strcmp ( wname, EDITKEYZROTATION ) == 0x00 ) key->rot.z = fval;

            if ( strcmp ( wname, EDITKEYXSCALING  ) == 0x00 ) key->sca.x = fval;
            if ( strcmp ( wname, EDITKEYYSCALING  ) == 0x00 ) key->sca.y = fval;
            if ( strcmp ( wname, EDITKEYZSCALING  ) == 0x00 ) key->sca.z = fval;

            ltmpkey = ltmpkey->next;
        }

        redrawGLViews ( gui );
        XtFree ( value );
    }
}

/******************************************************************************/
static void loopcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpobj = sce->lsel;
    XmToggleButtonCallbackStruct *tcs = ( XmToggleButtonCallbackStruct * ) call;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        if ( tcs->set ) {
            g3dkey_unsetLoopFromList ( obj->lselkey );
        } else {
            g3dkey_setLoopFromList ( obj->lselkey );
        }

        ltmpobj = ltmpobj->next;
    }
}

/******************************************************************************/
static void loopframecbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj ) {
        char *value = XmTextGetString ( w );
        int32_t val = strtol ( value, NULL, 10 );

        g3dkey_setLoopFrameFromList ( obj->lselkey, (float) val );

        XtFree ( value );
    }
}

/******************************************************************************/
void updateKeyEdit ( Widget w ) {
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

    if ( obj ) {
        G3DVECTOR keypos, keyrot, keysca;
        uint32_t keyloop = 0x00;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        if ( obj->lselkey == NULL ) {
            for ( i = 0x00; i < nc; i++ ) {
                Widget child = children[i];

                if ( XtClass ( child ) == xmTextWidgetClass ) {
                    XmTextSetEditable ( child, False );
                }
            }
        } else {
            float loopFrame;
            uint32_t loopFlag;

            loopFlag = g3dkey_getLoopFrameFromList ( obj->lselkey, &loopFrame );

            g3dkey_getTransformationFromList ( obj->lselkey, &keypos,
                                                             &keyrot,
                                                             &keysca );


            for ( i = 0x00; i < nc; i++ ) {
                Widget child = children[i];
                char *name = XtName ( child );
                char buf[0x10];

                if ( XtClass ( child ) == xmToggleButtonWidgetClass ) {
                    if ( strcmp ( name, EDITKEYLOOP ) == 0x00 ) {
                        if ( loopFlag ) {
                            XmToggleButtonSetState ( child, True, False );
                        } else {
                            XmToggleButtonSetState ( child, False, False );
                        }
                    }
                }

                if ( XtClass ( child ) == xmTextWidgetClass ) {
                    if ( XmTextGetEditable ( child ) == False ) {
                        XmTextSetEditable ( child, True );
                    }

                    if ( strcmp ( name, EDITKEYLOOPFRAME ) == 0x00 ) {
                        snprintf ( buf, 0x10, "%d", ( int32_t ) loopFrame );

                        XmTextSetString ( child, buf );
                    }

                    /****************** Position *****************/
                    if ( strcmp ( name, EDITKEYXPOSITION ) == 0x00 ) {
                        snprintf ( buf, 0x10, "%f", keypos.x );

                        XmTextSetString ( child, buf );
                    }

                    if ( strcmp ( name, EDITKEYYPOSITION ) == 0x00 ) {
                        snprintf ( buf, 0x10, "%f", keypos.y );

                        XmTextSetString ( child, buf );
                    }

                    if ( strcmp ( name, EDITKEYZPOSITION ) == 0x00 ) {
                        snprintf ( buf, 0x10, "%f", keypos.z );

                        XmTextSetString ( child, buf );
                    }

                    /****************** Rotation *****************/
                    if ( strcmp ( name, EDITKEYXROTATION ) == 0x00 ) {
                        snprintf ( buf, 0x10, "%f", keyrot.x );

                        XmTextSetString ( child, buf );
                    }

                    if ( strcmp ( name, EDITKEYYROTATION ) == 0x00 ) {
                        snprintf ( buf, 0x10, "%f", keyrot.y );

                        XmTextSetString ( child, buf );
                    }

                    if ( strcmp ( name, EDITKEYZROTATION ) == 0x00 ) {
                        snprintf ( buf, 0x10, "%f", keyrot.z );

                        XmTextSetString ( child, buf );
                    }

                    /****************** Scaling  *****************/
                    if ( strcmp ( name, EDITKEYXSCALING  ) == 0x00 ) {
                        snprintf ( buf, 0x10, "%f", keysca.x );

                        XmTextSetString ( child, buf );
                    }

                    if ( strcmp ( name, EDITKEYYSCALING  ) == 0x00 ) {
                        snprintf ( buf, 0x10, "%f", keysca.y );

                        XmTextSetString ( child, buf );
                    }

                    if ( strcmp ( name, EDITKEYZSCALING  ) == 0x00 ) {
                        snprintf ( buf, 0x10, "%f", keysca.z );

                        XmTextSetString ( child, buf );
                    }
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void destroycbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    list_remove ( &gui->lkeyedit, w );
}

/******************************************************************************/
Widget createKeyEdit ( Widget parent, G3DUI *gui, char *name,
                                      Dimension x    , Dimension y,
                                      Dimension width, Dimension height ) {
    Widget posx, posy, posz, rotx, roty, rotz, scax, scay, scaz;
    Pixel background, foreground;
    Widget frm;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    frm = XmVaCreateManagedForm ( parent, name,
                                  XmNx, x,
                                  XmNy, y,
                                  XmNwidth , width,
                                  XmNheight, height,
                                  XmNforeground, foreground,
                                  XmNbackground, background,
                                  XmNuserData, gui,
                                  NULL );

    /******** Translation values text fields *****************/

    createSimpleLabel ( frm, "Position", 0x00, 0x00, 0x60 );

    createFloatText ( frm, EDITKEYXPOSITION, 0x00, 0x14, 0x00, 0x60, keycbk );
    createFloatText ( frm, EDITKEYYPOSITION, 0x00, 0x28, 0x00, 0x60, keycbk );
    createFloatText ( frm, EDITKEYZPOSITION, 0x00, 0x3C, 0x00, 0x60, keycbk );

    /********** Rotation values text fields ****************/

    createSimpleLabel ( frm, "Rotation", 0x60, 0x00, 0x60 );

    createFloatText ( frm, EDITKEYXROTATION, 0x60, 0x14, 0x00, 0x60, keycbk );
    createFloatText ( frm, EDITKEYYROTATION, 0x60, 0x28, 0x00, 0x60, keycbk );
    createFloatText ( frm, EDITKEYZROTATION, 0x60, 0x3C, 0x00, 0x60, keycbk );

    /********** Scaling values text fields ****************/

    createSimpleLabel ( frm, "Scaling" , 0xC0, 0x00, 0x60 );

    createFloatText ( frm, EDITKEYXSCALING , 0xC0, 0x14, 0x00, 0x60, keycbk );
    createFloatText ( frm, EDITKEYYSCALING , 0xC0, 0x28, 0x00, 0x60, keycbk );
    createFloatText ( frm, EDITKEYZSCALING , 0xC0, 0x3C, 0x00, 0x60, keycbk );

    createToggleLabel ( frm, EDITKEYLOOP, 0x00, 0x50, 0x80, 0x20, loopcbk );
    createIntegerText   ( frm, EDITKEYLOOPFRAME, 0x84, 0x54,
                                                 0x00, 0x20, loopframecbk );

    updateKeyEdit ( frm );

    list_insert ( &gui->lkeyedit, frm );

    /*** We have to remove this widget form the keyedit list when closed ***/
    XtAddCallback ( frm, XmNdestroyCallback, destroycbk, gui );


    return frm;
}
