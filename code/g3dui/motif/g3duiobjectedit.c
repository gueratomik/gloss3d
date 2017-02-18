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
static void nameCbk ( Widget w, XtPointer client, XtPointer call ) {
    char *object_name = XmTextGetString ( w );
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duiobjectedit_nameCbk ( gui, object_name );

    XtFree ( object_name );
}

/******************************************************************************/
void updateObjectEdit ( Widget w, G3DUI *gui ) {
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
            if ( XtClass ( child ) == xmTextWidgetClass ) {
                if ( strcmp ( name, EDITOBJECTNAME ) == 0x00 ) {
                    char buf[0x100];

                    snprintf ( buf, 0x100, "%s", obj->name );

                    XmTextSetString ( child, buf );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
Widget createObjectEdit ( Widget parent, G3DUI *gui, char *name,
                          Dimension x    , Dimension y,
                          Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget frm, lab, txt;
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

    createCharText ( frm, gui, EDITOBJECTNAME, 0, 0, 96, 96, nameCbk );


    XtManageChild ( frm );


    return frm;
}
