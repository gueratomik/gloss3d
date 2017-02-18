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
static void usePosCbk ( Widget w, XtPointer client, XtPointer call ) {
    uint32_t rpos = XmToggleButtonGetState ( w );
    G3DUI *gui = ( G3DUI * ) client;

    if ( rpos ) common_g3duikeyedit_setFlagCbk   ( gui, KEYPOSITION );
    else        common_g3duikeyedit_unsetFlagCbk ( gui, KEYPOSITION );
}

/******************************************************************************/
static void useRotCbk ( Widget w, XtPointer client, XtPointer call ) {
    uint32_t rrot = XmToggleButtonGetState ( w );
    G3DUI *gui = ( G3DUI * ) client;

    if ( rrot ) common_g3duikeyedit_setFlagCbk   ( gui, KEYROTATION );
    else        common_g3duikeyedit_unsetFlagCbk ( gui, KEYROTATION );
}

/******************************************************************************/
static void useScaCbk ( Widget w, XtPointer client, XtPointer call ) {
    uint32_t rsca = XmToggleButtonGetState ( w );
    G3DUI *gui = ( G3DUI * ) client;

    if ( rsca ) common_g3duikeyedit_setFlagCbk   ( gui, KEYSCALING );
    else        common_g3duikeyedit_unsetFlagCbk ( gui, KEYSCALING );
}

/******************************************************************************/
static void loopFrameCbk ( Widget w, XtPointer client, XtPointer call ) {
    float frm = ( float ) XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duikeyedit_loopFrameCbk ( gui, frm );
}

/******************************************************************************/
static void loopCbk ( Widget w, XtPointer client, XtPointer call ) {
    uint32_t loop = XmToggleButtonGetState ( w );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duikeyedit_loopCbk ( gui, loop );
}

/******************************************************************************/
static void keyCbk ( Widget w, XtPointer client, XtPointer call ) {
    const char *field_name = XtName ( w );
    float val = ( float ) XmSpinButtonGetValue ( w );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duikeyedit_keyCbk ( gui, field_name, val );
}

/******************************************************************************/
void updateKeyEdit ( Widget w, G3DUI *gui ) {
    WidgetList children;
    G3DSCENE  *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

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

        if ( obj && obj->lselkey ) {
            G3DVECTOR keypos, keyrot, keysca;
            uint32_t keyloop = 0x00;
            float loopFrame;
            uint32_t loopFlag;
            uint32_t usePos = g3dkey_getUsePositionFromList ( obj->lselkey );
            uint32_t useRot = g3dkey_getUseRotationFromList ( obj->lselkey );
            uint32_t useSca = g3dkey_getUseScalingFromList  ( obj->lselkey );

            loopFlag = g3dkey_getLoopFrameFromList ( obj->lselkey, &loopFrame );

            g3dkey_getTransformationFromList ( obj->lselkey, &keypos,
                                                             &keyrot,
                                                             &keysca );

            for ( i = 0x00; i < nc; i++ ) {
                Widget child = children[i];
                char *name = XtName ( child );

                if ( XtClass ( child ) == xmToggleButtonWidgetClass ) {
                    if ( strcmp ( name, EDITKEYLOOP ) == 0x00 ) {
                        if ( loopFlag ) {
                            XmToggleButtonSetState ( child, True, False );
                        } else {
                            XmToggleButtonSetState ( child, False, False );
                        }
                    }

                    if ( strcmp ( name, EDITKEYPOSITION ) == 0x00 ) {
                        XmToggleButtonSetState ( child, usePos, False );
                    }

                    if ( strcmp ( name, EDITKEYROTATION ) == 0x00 ) {
                        XmToggleButtonSetState ( child, useRot, False );
                    }

                    if ( strcmp ( name, EDITKEYSCALING ) == 0x00 ) {
                        XmToggleButtonSetState ( child, useSca, False );
                    }
                }

                if ( XtClass ( child ) == xmSpinButtonWidgetClass ) {
                    /*if ( XmTextGetEditable ( child ) == False ) {
                        XmTextSetEditable ( child, True );
                    }*/

                    if ( strcmp ( name, EDITKEYLOOPFRAME ) == 0x00 ) {
                        XmSpinButtonSetValue ( child, loopFrame );
                    }

                    /****************** Position *****************/
                    if ( strcmp ( name, EDITKEYXPOSITION ) == 0x00 ) {
                        XmSpinButtonSetValue ( child, keypos.x );
                    }

                    if ( strcmp ( name, EDITKEYYPOSITION ) == 0x00 ) {
                        XmSpinButtonSetValue ( child, keypos.y );
                    }

                    if ( strcmp ( name, EDITKEYZPOSITION ) == 0x00 ) {
                        XmSpinButtonSetValue ( child, keypos.z );
                    }

                    /****************** Rotation *****************/
                    if ( strcmp ( name, EDITKEYXROTATION ) == 0x00 ) {
                        XmSpinButtonSetValue ( child, keyrot.x );
                    }

                    if ( strcmp ( name, EDITKEYYROTATION ) == 0x00 ) {
                        XmSpinButtonSetValue ( child, keyrot.y );
                    }

                    if ( strcmp ( name, EDITKEYZROTATION ) == 0x00 ) {
                        XmSpinButtonSetValue ( child, keyrot.z );
                    }

                    /****************** Scaling  *****************/
                    if ( strcmp ( name, EDITKEYXSCALING  ) == 0x00 ) {
                        XmSpinButtonSetValue ( child, keysca.x );
                    }

                    if ( strcmp ( name, EDITKEYYSCALING  ) == 0x00 ) {
                        XmSpinButtonSetValue ( child, keysca.y );
                    }

                    if ( strcmp ( name, EDITKEYZSCALING  ) == 0x00 ) {
                        XmSpinButtonSetValue ( child, keysca.z );
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

    /*XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );*/

    frm = XmVaCreateManagedForm ( parent, name,
                                  XmNx, x,
                                  XmNy, y,
                                  XmNwidth , width,
                                  XmNheight, height,
                                  /*XmNforeground, foreground,
                                  XmNbackground, background,*/
                                  NULL );

    /*** Callbacks will return prematurely if gui->lock == 0x01 ***/
    gui->lock = 0x01;

    /******** Translation values text fields *****************/

    createToggleLabel ( frm, gui, EDITKEYPOSITION ,   8, 0, 96, 20, usePosCbk );

    createFloatText   ( frm, gui, EDITKEYXPOSITION,   8, 24, 0, 96, keyCbk );
    createFloatText   ( frm, gui, EDITKEYYPOSITION,   8, 48, 0, 96, keyCbk );
    createFloatText   ( frm, gui, EDITKEYZPOSITION,   8, 72, 0, 96, keyCbk );

    /********** Rotation values text fields ****************/

    createToggleLabel ( frm, gui, EDITKEYROTATION , 160, 0, 96, 20, useRotCbk );

    createFloatText   ( frm, gui, EDITKEYXROTATION, 160, 24, 0, 96, keyCbk );
    createFloatText   ( frm, gui, EDITKEYYROTATION, 160, 48, 0, 96, keyCbk );
    createFloatText   ( frm, gui, EDITKEYZROTATION, 160, 72, 0, 96, keyCbk );

    /********** Scaling values text fields ****************/

    createToggleLabel ( frm, gui, EDITKEYSCALING  , 312, 0, 96, 20, useScaCbk );

    createFloatText   ( frm, gui, EDITKEYXSCALING , 312, 24, 0, 96, keyCbk );
    createFloatText   ( frm, gui, EDITKEYYSCALING , 312, 48, 0, 96, keyCbk );
    createFloatText   ( frm, gui, EDITKEYZSCALING , 312, 72, 0, 96, keyCbk );

    /*** Loop frame ***/

    createToggleLabel ( frm, gui, EDITKEYLOOP     ,   8, 104,
                                                    128,
                                                     20, loopCbk );

    createIntegerText ( frm, gui, EDITKEYLOOPFRAME, 160, 104,
                                                      0,
                                                     32, loopFrameCbk );

    updateKeyEdit ( frm, gui );

    gui->lock = 0x00;

    list_insert ( &gui->lkeyedit, frm );

    /*** We have to remove this widget form the keyedit list when closed ***/
    XtAddCallback ( frm, XmNdestroyCallback, destroycbk, gui );


    return frm;
}
