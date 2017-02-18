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
void updateTextureEdit ( Widget w ) {
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

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        Cardinal nc;
        int i, j;

        XtVaGetValues ( w, XmNchildren, &children,
                           XmNnumChildren, &nc,
                           NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];
            char *name = XtName ( child );

            if ( XtClass ( child ) == xmComboBoxWidgetClass ) {
                if ( strcmp ( name, EDITTEXTUREMAPPING ) == 0x00 ) {
                    LIST *lmap = g3dobject_getChildrenByType ( obj, G3DUVMAPTYPE );
                    LIST *ltmpmap  = lmap;
                    uint32_t nbmap = list_count ( lmap );
                    uint32_t mapid = 0x00;
                    XmStringTable dmlist;

                    dmlist = ( XmStringTable ) XtMalloc ( nbmap * sizeof ( XmString * ) );

                    while ( ltmpmap ) {
                        G3DUVMAP *map = ( G3DUVMAP * ) ltmpmap->data;
                        char *mapname = ((G3DOBJECT*)map)->name;

                        dmlist[mapid++] = XmStringCreate ( mapname, XmFONTLIST_DEFAULT_TAG );

                        ltmpmap = ltmpmap->next;
                    }

                    XtVaSetValues( child, XmNitems, dmlist, 
                                          XmNitemCount, nbmap,
                                          XmNvisibleItemCount, nbmap, NULL );

                    for ( j = 0x00; j < nbmap; j++ ) {
                        XmStringFree ( dmlist[j] );
                    }

                    XtFree ( ( char * ) dmlist );

                    list_free ( &lmap, NULL );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void destroycbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    list_remove ( &gui->ltexedit, w );
}

/******************************************************************************/
static void setUVMapCbk ( Widget w, XtPointer client, XtPointer call ) {
    XmComboBoxCallbackStruct *cbs = ( XmComboBoxCallbackStruct * ) call;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    G3DUI *gui;

    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    sce = gui->sce;
    obj = g3dscene_getSelectedObject ( sce );

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes  = ( G3DMESH * ) obj;
            G3DUVMAP *map = g3dmesh_getUVMapByID ( mes, cbs->item_position );
            LIST *ltmpseltex = mes->lseltex;

            while ( ltmpseltex ) {
                G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmpseltex->data;

                tex->map = map;
 
                ltmpseltex = ltmpseltex->next;
            }
        }
    }
}

/******************************************************************************/
Widget createTextureEdit ( Widget parent, G3DUI *gui, char *name,
                                          Dimension x    , Dimension y,
                                          Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget frm, cbb;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

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

    createSimpleLabel ( frm, EDITTEXTUREMAPPING, 0x10, 0x10, 0x80 );

    cbb = XmVaCreateComboBox ( frm, EDITTEXTUREMAPPING, 
                                    XmNx, 0x90,
                                    XmNy, 0x10,
                                    XmNwidth, 0x80,
                                    XmNhighlightThickness, 0x00,
                                    XmNshadowThickness, 0x01,
                                    XmNmarginHeight, 0x00,
                                    XmNmarginWidth, 0x00,
                                    XmNitemCount, 0x00,
                                    XmNvisibleItemCount, 0x01,
                                    XmNfontList, gmt->fontlist,
                                    XmNtraversalOn, False,
                                    XmNarrowSize, 0x0C,
                                    XmNcomboBoxType, XmDROP_DOWN_LIST,
                                    XmNrenderTable, gmt->renTable,
                                    XmNbackground, background,
                                    XmNforeground, foreground,
                                    XmNuserData, gui,
                                    NULL );

    XtManageChild ( cbb );

    /*** OpenMotif prior to 2.3.4 crashes if this is set before managing ***/
    XtVaSetValues ( cbb, XmNheight, 0x10, NULL );

    XtAddCallback ( cbb, XmNselectionCallback, setUVMapCbk, NULL );

    updateTextureEdit ( frm );

    list_insert ( &gui->ltexedit, frm );

    /*** We have to remove this widget form the keyedit list when closed ***/
    XtAddCallback ( frm, XmNdestroyCallback, destroycbk, gui );


    return frm;
}
