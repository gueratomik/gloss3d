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
static void posCbk ( Widget w, XtPointer client, XtPointer call ) {
    char *value = XmTextGetString ( w );
    float val = strtof ( value, NULL );
    const char *name = XtName ( w );
    G3DUI *gui = ( G3DUI * ) client;

    if ( strcmp ( name, EDITXPOSITION ) == 0x00 ) {
        common_g3duicoordinatesedit_posCbk ( gui, G3DUIXAXIS, val );
    }

    if ( strcmp ( name, EDITYPOSITION ) == 0x00 ) {
        common_g3duicoordinatesedit_posCbk ( gui, G3DUIYAXIS, val );
    }

    if ( strcmp ( name, EDITZPOSITION ) == 0x00 ) {
        common_g3duicoordinatesedit_posCbk ( gui, G3DUIZAXIS, val );
    }
}

/******************************************************************************/
static void rotCbk ( Widget w, XtPointer client, XtPointer call ) {
    char *value = XmTextGetString ( w );
    float val = strtof ( value, NULL );
    const char *name = XtName ( w );
    G3DUI *gui = ( G3DUI * ) client;

    if ( strcmp ( name, EDITXROTATION ) == 0x00 ) {
        common_g3duicoordinatesedit_rotCbk ( gui, G3DUIXAXIS, val );
    }

    if ( strcmp ( name, EDITYROTATION ) == 0x00 ) {
        common_g3duicoordinatesedit_rotCbk ( gui, G3DUIYAXIS, val );
    }

    if ( strcmp ( name, EDITZROTATION ) == 0x00 ) {
        common_g3duicoordinatesedit_rotCbk ( gui, G3DUIZAXIS, val );
    }
}

/******************************************************************************/
static void scaCbk ( Widget w, XtPointer client, XtPointer call ) {
    char *value = XmTextGetString ( w );
    float val = strtof ( value, NULL );
    const char *name = XtName ( w );
    G3DUI *gui = ( G3DUI * ) client;

    if ( strcmp ( name, EDITXSCALING ) == 0x00 ) {
        common_g3duicoordinatesedit_scaCbk ( gui, G3DUIXAXIS, val );
    }

    if ( strcmp ( name, EDITYSCALING ) == 0x00 ) {
        common_g3duicoordinatesedit_scaCbk ( gui, G3DUIYAXIS, val );
    }

    if ( strcmp ( name, EDITZSCALING ) == 0x00 ) {
        common_g3duicoordinatesedit_scaCbk ( gui, G3DUIZAXIS, val );
    }
}

/******************************************************************************/
void updateCoordinatesEdit ( Widget w, G3DUI *gui ) {
    WidgetList children;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj ) {
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );
            char buf[0x10];

            if ( XtClass ( child ) == xmTextWidgetClass ) {
                if ( strcmp ( name, EDITXPOSITION   ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%.3f", obj->pos.x );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITYPOSITION   ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%.3f", obj->pos.y );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITZPOSITION   ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%.3f", obj->pos.z );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITXROTATION   ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%.3f", obj->rot.x );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITYROTATION   ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%.3f", obj->rot.y );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITZROTATION   ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%.3f", obj->rot.z );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITXSCALING   ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%.3f", obj->sca.x );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITYSCALING   ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%.3f", obj->sca.y );

                    XmTextSetString ( child, buf );
                }

                if ( strcmp ( name, EDITZSCALING   ) == 0x00 ) {
                    snprintf ( buf, 0x10, "%.3f", obj->sca.z );

                    XmTextSetString ( child, buf );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void Destroy ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    list_remove ( &gui->lcoordedit, w );
}

/******************************************************************************/
Widget createCoordinatesEdit ( Widget parent, G3DUI *gui, char *name,
                               Dimension x    , Dimension y,
                               Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget frm, rwc, coords;

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

    rwc = XmVaCreateRowColumn ( frm, name,
                                XmNx, 0,
                                XmNy, 0,
                                XmNwidth , width,
                                XmNheight, height,
                                XmNnumColumns, 0x03,
                                /*XmNorientation, XmHORIZONTAL,*/
                                XmNpacking, XmPACK_COLUMN,
                                XmNspacing, 0x01,
                                XmNforeground, foreground,
                                XmNbackground, background,
                                XmNuserData, gui,
                                NULL );

    createSimpleLabel ( rwc, gui, "Position"   ,   0,  0, 0, 96 );
    createFloatText   ( rwc, gui, EDITXPOSITION,   0, 24, 0, 96, posCbk );
    createFloatText   ( rwc, gui, EDITYPOSITION,   0, 48, 0, 96, posCbk );
    createFloatText   ( rwc, gui, EDITZPOSITION,   0, 72, 0, 96, posCbk );

    createSimpleLabel ( rwc, gui, "Rotation"   ,  96,  0, 0, 96 );
    createFloatText   ( rwc, gui, EDITXROTATION,  96, 24, 0, 96, rotCbk );
    createFloatText   ( rwc, gui, EDITYROTATION,  96, 48, 0, 96, rotCbk );
    createFloatText   ( rwc, gui, EDITZROTATION,  96, 72, 0, 96, rotCbk );

    createSimpleLabel ( rwc, gui, "Scaling"    , 192,  0, 0, 96 );
    createFloatText   ( rwc, gui, EDITXSCALING , 192, 24, 0, 96, scaCbk );
    createFloatText   ( rwc, gui, EDITYSCALING , 192, 48, 0, 96, scaCbk );
    createFloatText   ( rwc, gui, EDITZSCALING , 192, 72, 0, 96, scaCbk );

    /*coords = createObjectCoordinates ( frm, gui, "COORDINATES",
                                       x, y, width, height );*/

    XtAddCallback ( rwc, XmNdestroyCallback, Destroy, gui );

    XtManageChild ( rwc );

    updateCoordinatesEdit ( rwc, gui );

    list_insert ( &gui->lcoordedit, rwc );


    return frm;
}
