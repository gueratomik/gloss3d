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
static void projcbk ( Widget w, XtPointer client, XtPointer call ) {
    XmComboBoxCallbackStruct *cbs = ( XmComboBoxCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    char *str;

    XmStringGetLtoR ( cbs->item_or_text, XmFONTLIST_DEFAULT_TAG, &str );

    if ( obj && ( obj->type == G3DUVMAPTYPE ) ) {
        G3DUVMAP *map = ( G3DUVMAP * ) obj;

        if ( strcmp ( str, CYLINDRICALPROJECTION ) == 0x00 ) {
            map->projection = UVMAPCYLINDRICAL;
        }

        if ( strcmp ( str, SPHERICALPROJECTION ) == 0x00 ) {
            map->projection = UVMAPSPHERICAL;
        }
        
        if ( strcmp ( str, FLATPROJECTION ) == 0x00 ) {
            map->projection = UVMAPFLAT;
        }

        g3dobject_updateMatrix_r ( ( G3DOBJECT * ) map, gui->flags );
        /*g3duvmap_applyProjection ( map );*/

        g3dui_redrawGLViews ( gui );
    }

    XtFree ( str );
}

/******************************************************************************/
void updateUVMapEdit ( Widget w ) {
    WidgetList children;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    G3DUI *gui;

    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    sce = gui->sce;
    obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type == G3DSYMMETRYTYPE ) ) {
        G3DSYMMETRY *sym = ( G3DSYMMETRY * ) obj;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );

            if ( XtClass ( child ) == xmComboBoxWidgetClass ) {
                if ( strcmp ( name, EDITUVMAPPROJECTION   ) == 0x00 ) {
                    /*XtVaSetValues ( child, XmNselectedPosition,
                                           sym->orientation, NULL );*/
                }
            }
        }
    }
}

/******************************************************************************/
static void uvmapfixedcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type == G3DUVMAPTYPE ) ) {
        G3DUVMAP *map = ( G3DUVMAP * ) obj;

        if ( obj->flags & UVMAPFIXED ) {
            g3duvmap_unfix ( map );
        } else {
            g3duvmap_fix   ( map );
        }
    }
}

/******************************************************************************/
Widget createUVMapEdit ( Widget parent  , G3DUI *gui, char *name,
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

    createUVMappingSelection ( frm, EDITUVMAPPROJECTION, 0x00, 0x30,
                                                         0x60, 0x60, projcbk );

    createToggleLabel ( frm, EDITUVMAPFIXED, 0x00, 0x20,
                                                   0x60, 0x10, uvmapfixedcbk );


    XtManageChild ( frm );

    return frm;
}