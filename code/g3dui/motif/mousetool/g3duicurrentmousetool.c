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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2020         */
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
void updateCurrentMouseTool ( Widget w, G3DUI *gui ) {
    G3DMOUSETOOL *mou = gui->mou;;
    WidgetList children;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    Cardinal nc;
    int i;

    XtVaGetValues ( w, XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    if ( mou ) {
        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];

            if ( strcmp ( XtName ( child ), mou->name ) == 0x00 ) {
                if ( strcmp ( mou->name, PICKTOOL    ) == 0x00 ) updatePickToolEdit    ( child, gui );
                /*if ( strcmp ( mou->name, CUTMESHTOOL ) == 0x00 ) updateCutMeshToolEdit ( child, gui );*/

                XtMapWidget ( child );
            } else {
                XtUnmapWidget   ( child );
            }
        }
    /*** Hide all children widgets if no object is selected ***/
    } else {
        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];

            if ( XtIsRealized ( child ) ) {
                XtUnmapWidget ( child );
            } else {
                /*** start with children made invisible, because no ***/
                /*** object is selected when application starts.  ***/
                XtVaSetValues ( child, XmNmappedWhenManaged, False, NULL );
            }
        }
    }
}

/******************************************************************************/
Widget createCurrentMouseTool ( Widget parent, G3DUI *gui, 
                                               char *name,
                                               Dimension x,
                                               Dimension y,
                                               Dimension width,
                                               Dimension height ) {
    Pixel background, foreground;
    Widget scr, frm;


    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    scr = XmVaCreateManagedScrolledWindow ( parent, name,
                                            XmNx, x,
                                            XmNy, y,
                                            XmNwidth , width,
                                            XmNheight, height,
                                            XmNscrollingPolicy, XmAUTOMATIC,
                                            XmNspacing, 0x00,
                                            XmNforeground, foreground,
                                            XmNbackground, background,
                                            XmNtopShadowColor, background,
                                            XmNbottomShadowColor, background,
                                            XmNshadowThickness, 0x01,
                                            NULL );

    frm = XmVaCreateManagedForm ( scr, name,
                                  XtNx, 0x00,
                                  XtNy, 0x00,
                                  XmNwidth , 0x140,
                                  XmNheight, 0x140,
                                  XmNforeground, foreground,
                                  XmNbackground, background,
                                  XmNborderWidth, 0x00,
                                  NULL );

    /*** This is type dependent: hidden if not of ***/
    /*** selected object type showed otherwise.   ***/
    createPickToolEdit    ( frm, gui, PICKTOOL   , 0, 0, 320,  192 );
    /*createCutMeshToolEdit ( frm, gui, CUTMESHTOOL, 0, 0, 320, 320 );*/

    updateCurrentMouseTool ( frm, gui );

    list_insert ( &gui->lmtools, frm );


    return scr;
}
