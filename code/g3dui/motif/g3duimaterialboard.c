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
void setMaterialCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_setMaterialCbk ( gui );
}

/******************************************************************************/
void removeMaterialCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    LIST *ltmpmatlist = gui->lmatlist;

    if ( gui->selmat ) {
        while ( ltmpmatlist ) {
            Widget matlst = ( Widget ) ltmpmatlist->data;

            g3duimateriallist_removeMaterial ( matlst, gui );

            gui->selmat = NULL;


            ltmpmatlist = ltmpmatlist->next;
        }
    }

    g3dui_redrawMaterialList ( gui );
    g3dui_updateMaterialEdit ( gui );
    g3dui_redrawGLViews      ( gui );
}

/******************************************************************************/
void addMaterialCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DMATERIAL *mat = g3dmaterial_new ( "Material" );
    G3DUI *gui = ( G3DUI * ) client;
    LIST *ltmpmatlist = gui->lmatlist;

    g3dscene_addMaterial ( gui->sce, mat );

    while ( ltmpmatlist ) {
        Widget matlst = ( Widget ) ltmpmatlist->data;

        g3duimateriallist_addPreview ( matlst, gui, mat );

        ltmpmatlist = ltmpmatlist->next;
    }

    gui->selmat = mat;

    g3dui_redrawMaterialList ( gui );
    g3dui_updateMaterialEdit ( gui );
}

/******************************************************************************/
static Widget createMaterialFileMenu ( Widget parent, G3DUI *gui, 
                                                      char *name,
                                                      Dimension width ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget item, menu;

    menu = XmCreatePulldownMenu ( parent, "FileMenuPullDown", NULL, 0x00 );

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground,
                            NULL );

    XtVaSetValues ( menu, XmNhighlightThickness, 0x00,
                          XmNshadowThickness   , 0x01, 
                          NULL );

    item = XmVaCreateCascadeButton ( parent, "File",
                                     XmNwidth, width,
                                     XmNrecomputeSize, False,
                                     XmNfontList, gmt->fontlist,
                                     XmNhighlightThickness, 0x01,
                                     XmNshadowThickness, 0x01,
                                     XmNbackground, background,
                                     XmNforeground, foreground,
                                     XmNsubMenuId, menu,
                                     NULL );

    g3dui_addMenuButton ( menu, gui, "Add Material"   , 96, addMaterialCbk    );
    g3dui_addMenuButton ( menu, gui, "Delete Material", 96, removeMaterialCbk );
    g3dui_addMenuButton ( menu, gui, "Set Material"   , 96, setMaterialCbk    );

    XtManageChild ( item );


    return menu;
}

/******************************************************************************/
static Widget createMaterialMenuBar ( Widget parent, G3DUI *gui, 
                                                     char *name,
                                                     Dimension x,
                                                     Dimension y,
                                                     Dimension width,
                                                     Dimension height ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget bar;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    bar = XmVaCreateSimpleMenuBar ( parent, MENUBARNAME,
                                            XmNfontList, gmt->fontlist,
                                            XmNheight, height,
                                            XmNwidth, width,
                                            XmNhighlightThickness, 0x01,
                                            XmNshadowThickness, 0x01,
                                            XmNbackground, background,
                                            XmNforeground, foreground,
                                            NULL );

    createMaterialFileMenu ( bar, gui, "FileMenu", 60 );


    XtManageChild ( bar );


    return bar;
}

/******************************************************************************/
Widget createMaterialBoard ( Widget parent, G3DUI *gui, 
                                            char *name,
                                            Dimension x,
                                            Dimension y,
                                            Dimension width,
                                            Dimension height ) {
    Pixel white = XWhitePixel ( XtDisplay ( parent ), 0x00 );
    Pixel background, foreground;
    Widget curedit, objlist, tab, frm, menu;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

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

    menu = createMaterialMenuBar ( frm, gui, "MENU", 0, 0, width, 32 );

    XtVaSetValues ( menu, XmNleftAttachment , XmATTACH_FORM, NULL );
    XtVaSetValues ( menu, XmNrightAttachment, XmATTACH_FORM, NULL );

    createMaterialList ( frm, gui, "Materials"    , 0,  32, 320, 320 );
    createMaterialEdit ( frm, gui, "Material Edit", 0, 352, 320, 200 );

    XtManageChild ( frm );


    return frm;
}

