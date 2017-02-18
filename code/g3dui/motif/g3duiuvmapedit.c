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
static void projectionCbk ( Widget w, XtPointer client, XtPointer call ) {
    XmComboBoxCallbackStruct *cbs = ( XmComboBoxCallbackStruct * ) call;
    G3DUI *gui = ( G3DUI * ) client;
    char *str;

    XmStringGetLtoR ( cbs->item_or_text, XmFONTLIST_DEFAULT_TAG, &str );

    common_g3duiuvmap_projectionCbk ( gui, str );

    XtFree ( str );
}

/******************************************************************************/
static void lockUVMapCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3duiuvmap_lockUVMapCbk ( gui );
}

/******************************************************************************/
void createProjectionSelection ( Widget parent, G3DUI *gui,
                                                char *name,
                                                Dimension x,
                                                Dimension y,
                                                Dimension labwidth,
                                                Dimension txtwidth,
                                                void (*cbk)( Widget, 
                                                             XtPointer,
                                                             XtPointer ) ) {
    Pixel white = XWhitePixel ( XtDisplay ( parent ), 0x00 );
    Pixel background, foreground;
    uint32_t strsize = sizeof ( XmString );
    XmStringTable dmlist = ( XmStringTable ) XtMalloc ( 0x03 * strsize );
    Widget lab;
    Widget sel;
    G3DUIMOTIF *gmt;

    XtVaGetValues ( parent, XmNbackground, &background, 
                            XmNforeground, &foreground, 
                            NULL );

    gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    dmlist[0x00] = XmStringCreate ( FLATPROJECTION       , XmFONTLIST_DEFAULT_TAG );
    dmlist[0x01] = XmStringCreate ( SPHERICALPROJECTION  , XmFONTLIST_DEFAULT_TAG );
    dmlist[0x02] = XmStringCreate ( CYLINDRICALPROJECTION, XmFONTLIST_DEFAULT_TAG );

    lab = XmVaCreateManagedLabel ( parent, name,
                                   XmNx, x,
                                   XmNy, y,
                                   XmNwidth , labwidth,
                                   XmNheight, 0x12,
                                   XmNfontList, gmt->fontlist,
                                   XmNforeground, foreground,
                                   XmNbackground, background,
                                   NULL );

    sel = XmVaCreateManagedComboBox ( parent, name, 
                                       XmNx, x + labwidth,
                                       XmNy, y,
                                       XmNwidth , txtwidth,
                                       XmNhighlightThickness, 0x00,
                                       XmNshadowThickness, 0x01,
                                       XmNmarginHeight, 0x00,
                                       XmNmarginWidth, 0x00,
                                       XmNitemCount,	0x03,
                                       XmNitems, dmlist,
                                       XmNvisibleItemCount, 0x03,
                                       XmNfontList, gmt->fontlist,
                                       XmNarrowSize, 0x0C,
                                       XmNcomboBoxType, XmDROP_DOWN_LIST,
                                       XmNrenderTable, gmt->renTable,
                                       XmNtraversalOn, False,
                                       XtNbackground, background,
                                       XtNforeground, foreground,
                                       NULL );

    /*** OpenMotif prior to 2.3.4 crashes if this is set before managing ***/
    XtVaSetValues ( sel, XmNheight, 0x12, NULL );

    XmComboBoxSelectItem ( sel, dmlist[0x00] );

    XmStringFree ( dmlist[0x00] );
    XmStringFree ( dmlist[0x01] );
    XmStringFree ( dmlist[0x02] );

    XtFree ( ( char * ) dmlist );

    if ( cbk ) {
        XtAddCallback ( sel, XmNselectionCallback, cbk, gui );
    }
}

/******************************************************************************/
void updateUVMapEdit ( Widget w, G3DUI *gui ) {
    WidgetList children;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DUVMAPTYPE ) ) {
        G3DUVMAP *map = ( G3DUVMAP * ) obj;
        Cardinal nc;
        int i;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );

            if ( XtClass ( child ) == xmToggleButtonWidgetClass ) {
                if ( strcmp ( name, EDITUVMAPFIXED ) == 0x00 ) {
                    if ( ((G3DOBJECT*)map)->flags & UVMAPFIXED ) {
                        XtVaSetValues ( child, XmNset, True, NULL );
                    } else {
                        XtVaSetValues ( child, XmNset, False, NULL );
                    }
                }
            }

            if ( XtClass ( child ) == xmComboBoxWidgetClass ) {
                if ( strcmp ( name, EDITUVMAPPROJECTION   ) == 0x00 ) {
                    /*XtVaSetValues ( child, XmNselectedPosition,
                                           sym->orientation, NULL );*/
                }
            }
        }
    }

    gui->lock = 0x00;
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

    createToggleLabel         ( frm, gui, EDITUVMAPFIXED,
                                0,  0, 200, 20, lockUVMapCbk );

    createProjectionSelection ( frm, gui, EDITUVMAPPROJECTION, 
                                0, 24, 96, 96, projectionCbk );

    XtManageChild ( frm );

    return frm;
}
