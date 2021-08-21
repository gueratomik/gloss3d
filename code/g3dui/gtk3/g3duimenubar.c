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
#include <g3dui_gtk3.h>

#define G3DUIMENUTYPE_PUSHBUTTON   0x00
#define G3DUIMENUTYPE_TOGGLEBUTTON 0x01
#define G3DUIMENUTYPE_SEPARATOR    0x02
#define G3DUIMENUTYPE_SUBMENU      0x03
#define G3DUIMENUTYPE_MENUBAR      0x04

/******************************************************************************/
static uint32_t objectModeOnly ( G3DUI *gui ) {
    return ( gui->engine_flags & VIEWOBJECT ) ? 0x01 : 0x00;
}

/******************************************************************************/
static uint32_t vertexModeOnly ( G3DUI *gui ) {
    return ( gui->engine_flags & VIEWVERTEX ) ? 0x01 : 0x00;
}

/******************************************************************************/
static uint32_t faceModeOnly ( G3DUI *gui ) {
    return ( gui->engine_flags & VIEWFACE ) ? 0x01 : 0x00;
}

/******************************************************************************/
static uint32_t edgeModeOnly ( G3DUI *gui ) {
    return ( gui->engine_flags & VIEWEDGE ) ? 0x01 : 0x00;
}

/******************************************************************************/
static uint32_t skinModeOnly ( G3DUI *gui ) {
    return ( gui->engine_flags & VIEWSKIN ) ? 0x01 : 0x00;
}

/******************************************************************************/
static uint32_t objectMode_skinSelected ( G3DUI *gui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    return  ( ( obj                            ) && 
              ( obj->type & SKIN               ) &&
              ( gui->engine_flags & VIEWOBJECT ) ) ? 0x01 : 0x00;
}

/******************************************************************************/
static uint32_t objectMode_objectSelected ( G3DUI *gui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    return  ( ( obj                            ) && 
              ( gui->engine_flags & VIEWOBJECT ) ) ? 0x01 : 0x00;
}

#ifdef unused
/******************************************************************************/
static void addUVMapCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duimenubar_addUVMapCbk ( gui );
}

/******************************************************************************/
static void fitUVMapCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duimenubar_fitUVMapCbk ( gui );
}

/******************************************************************************/
static void alignUVMapCbk ( GtkWidget *widget, gpointer user_data ) {
    const gchar *option = gtk_widget_get_name  ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duimenubar_alignUVMapCbk ( gui, option );
}

/******************************************************************************/
GtkWidget *createAlignUVMapMenu ( GtkWidget *parent, G3DUI *gui,
                                                            char *name,
                                                            gint width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( name );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( parent ), item );

    g3dui_addMenuButton ( menu, gui, MENU_ALIGNUVMAPXY, width, G_CALLBACK(alignUVMapCbk));
    g3dui_addMenuButton ( menu, gui, MENU_ALIGNUVMAPYZ, width, G_CALLBACK(alignUVMapCbk));
    g3dui_addMenuButton ( menu, gui, MENU_ALIGNUVMAPZX, width, G_CALLBACK(alignUVMapCbk));


    gtk_widget_show ( item );

    gtk_widget_show ( menu );


    return menu;
}

/******************************************************************************/
GtkWidget *createUVMappingMenu ( GtkWidget *bar, G3DUI *gui,
                                                 char *name,
                                                 gint width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( "_UVMapping" );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_halign ( item, GTK_ALIGN_CENTER );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( bar ), item );

    g3dui_addMenuButton  ( menu, gui, MENU_ADDUVMAP  , width, G_CALLBACK(addUVMapCbk));
    g3dui_addMenuButton  ( menu, gui, MENU_FITUVMAP  , width, G_CALLBACK(fitUVMapCbk));
    createAlignUVMapMenu ( menu, gui, MENU_ALIGNUVMAP, width );

/*    g3dui_addMenuButton ( menu, gui, MENU_ADDUVWMAP    , width, g3dui_addLightCbk    , NULL );
*/
    gtk_widget_show     ( item );

    gtk_widget_show     ( menu );



    return menu;
}

/******************************************************************************/
static void renderSettingsCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );

    /*** For some reason, GtkSpinButtons calls its callbacks ***/
    /*** when being realized. With this trick I bypass that. ***/
    /*** Callbacks will return prematurely if gui->lock == 0x01 ***/
    gui->lock = 0x01;

    createRenderEdit ( dial, gui, "RENDEREDIT", 0, 0, 480, 340 );

    gtk_widget_show ( dial );

    gui->lock = 0x00;
}

/******************************************************************************/
GtkWidget *createRenderMenu ( GtkWidget *bar, G3DUI *gui,
                                                 char *name,
                                                 gint width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( "_Render" );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_halign ( item, GTK_ALIGN_CENTER );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( bar ), item );

    g3dui_addMenuButton ( menu, gui, MENU_RENDERVIEW    , width, G_CALLBACK(g3dui_renderViewCbk) );
    g3dui_addMenuButton ( menu, gui, MENU_RENDERSETTINGS, width, G_CALLBACK(renderSettingsCbk) );
    /*g3dui_addMenuButton ( menu, gui, MENU_RUNRENDER, width, renderFinalCbk    );*/

/*    g3dui_addMenuButton ( menu, gui, MENU_ADDUVWMAP    , width, g3dui_addLightCbk );
*/
    gtk_widget_show     ( item );

    gtk_widget_show     ( menu );



    return menu;
}

/******************************************************************************/
void g3dui_aboutCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    gchar *authors[] = { "Gary GABRIEL", NULL };

    gtk_show_about_dialog ( NULL,
                           "authors", authors,
                           "program-name", "Gloss3D",
                           "license", GLOSS3DLICENSE,
                           "comments", "Graphics by a Lightweight Open-Source Software",
                           "version", VERSION,
                           "copyright", "2012-2020 The Gloss3D Team",
                           "website", "http://www.gloss3d.net",
                           "title", ("About Gloss3D"),
                           NULL );
}

/******************************************************************************/
GtkWidget *createHelpMenu ( GtkWidget *bar, G3DUI *gui,
                                            char *name,
                                            gint width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( "_Help" );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_halign ( item, GTK_ALIGN_CENTER );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( bar ), item );

    g3dui_addMenuButton ( menu, gui, MENU_ABOUT, width, G_CALLBACK(g3dui_aboutCbk) );

    gtk_widget_show     ( item );

    gtk_widget_show     ( menu );



    return menu;
}

/******************************************************************************/
void g3dui_mergeMeshCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_mergeMeshCbk ( gui );
}

/******************************************************************************/
void g3dui_splitMeshCbk ( GtkWidget *widget, gpointer user_data ) {
    const gchar *option = gtk_widget_get_name  ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_splitMeshCbk ( gui, option );
}

/******************************************************************************/
void g3dui_mirrorWeightGroupCbk ( GtkWidget *widget, gpointer user_data ) {
    const gchar *option = gtk_widget_get_name  ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_mirrorWeightGroupCbk ( gui, option );
}

/******************************************************************************/
GtkWidget *createSplitMeshMenu ( GtkWidget *parent, G3DUI *gui,
                                                    char *name,
                                                    gint width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( name );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( parent ), item );

    g3dui_addMenuButton ( menu, gui, MENU_SPLITANDKEEP  , width, G_CALLBACK(g3dui_splitMeshCbk) );
    g3dui_addMenuButton ( menu, gui, MENU_SPLITANDREMOVE, width, G_CALLBACK(g3dui_splitMeshCbk) );

    gtk_widget_show ( item );

    gtk_widget_show ( menu );


    return menu;
}

/******************************************************************************/
GtkWidget *createMirrorWeightGroupMenu ( GtkWidget *parent, G3DUI *gui,
                                                            char *name,
                                                            gint width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( name );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( parent ), item );

    g3dui_addMenuButton ( menu, gui, MENU_MIRRORXY, width, G_CALLBACK(g3dui_mirrorWeightGroupCbk) );
    g3dui_addMenuButton ( menu, gui, MENU_MIRRORYZ, width, G_CALLBACK(g3dui_mirrorWeightGroupCbk) );
    g3dui_addMenuButton ( menu, gui, MENU_MIRRORZX, width, G_CALLBACK(g3dui_mirrorWeightGroupCbk) );


    gtk_widget_show ( item );

    gtk_widget_show ( menu );


    return menu;
}

/******************************************************************************/
GtkWidget *createFunctionsMenu ( GtkWidget *bar, G3DUI *gui,
                                                 char *name,
                                                 gint width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( "_Functions" );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_halign ( item, GTK_ALIGN_CENTER );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( bar ), item );

    createMirrorWeightGroupMenu ( menu, gui, MENU_MIRRORWEIGHTGROUP, width );
    createSplitMeshMenu         ( menu, gui, MENU_SPLITMESH        , width );
    g3dui_addMenuButton         ( menu, gui, MENU_MERGEMESH        , width, g3dui_mergeMeshCbk );
    g3dui_addMenuButton         ( menu, gui, MENU_MAKEEDITABLE     , width, g3dui_makeEditableCbk );

    gtk_widget_show     ( item );

    gtk_widget_show     ( menu );



    return menu;
}

/******************************************************************************/
void g3dui_resetBoneTreeCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_resetBoneTreeCbk ( gui );
}

/******************************************************************************/
void g3dui_resetBoneCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_resetBoneCbk ( gui );
}

/******************************************************************************/
void g3dui_fixBoneTreeCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_fixBoneTreeCbk ( gui );
}

/******************************************************************************/
void g3dui_fixBoneCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_fixBoneCbk ( gui );
}

/******************************************************************************/
void g3dui_addSymmetryCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addSymmetryCbk ( gui );
}

/******************************************************************************/
void g3dui_addWireframeCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addWireframeCbk ( gui );
}

/******************************************************************************/
void g3dui_addSubdividerCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addSubdividerCbk ( gui );
}

/******************************************************************************/
void g3dui_addSplineRevolverCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addSplineRevolverCbk ( gui );
}

/******************************************************************************/
void g3dui_addFFDBoxCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addFFDBoxCbk ( gui );
}

/******************************************************************************/
void g3dui_addMorpherCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addMorpherCbk ( gui );
}

/******************************************************************************/
void g3dui_addSkinCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addSkinCbk ( gui );
}

/******************************************************************************/
void g3dui_addBoneCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addBoneCbk ( gui );
}

/******************************************************************************/
GtkWidget *createResetBoneMenu ( GtkWidget *parent, G3DUI *gui,
                                                    char *name,
                                                    gint width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( name );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( parent ), item );

    g3dui_addMenuButton ( menu, gui, MENU_RESETBONETREE, width, G_CALLBACK(g3dui_resetBoneTreeCbk) );
    g3dui_addMenuButton ( menu, gui, MENU_RESETBONEONLY, width, G_CALLBACK(g3dui_resetBoneCbk)     );

    gtk_widget_show ( item );

    gtk_widget_show ( menu );


    return menu;
}

/******************************************************************************/
GtkWidget *createFixBoneMenu ( GtkWidget *parent, G3DUI *gui,
                                                  char *name,
                                                  gint width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( name );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( parent ), item );

    g3dui_addMenuButton ( menu, gui, MENU_FIXBONETREE, width, G_CALLBACK(g3dui_fixBoneTreeCbk) );
    g3dui_addMenuButton ( menu, gui, MENU_FIXBONEONLY, width, G_CALLBACK(g3dui_fixBoneCbk)     );

    gtk_widget_show ( item );

    gtk_widget_show ( menu );


    return menu;
}

/******************************************************************************/
GtkWidget *createMultipliersMenu ( GtkWidget *bar, G3DUI *gui,
                                                   char *name,
                                                   gint width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( "_Multipliers" );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_halign ( item, GTK_ALIGN_CENTER );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( bar ), item );

    g3dui_addMenuButton       ( menu, gui, MENU_ADDSYMMETRY    , width, G_CALLBACK(g3dui_addSymmetryCbk)     );

/*    g3dui_addMenuButton ( menu, gui, MENU_ADDUVWMAP    , width, g3dui_addLightCbk    , NULL );
*/
    gtk_widget_show     ( item );

    gtk_widget_show     ( menu );



    return menu;
}

/******************************************************************************/
GtkWidget *createModifiersMenu ( GtkWidget *bar, G3DUI *gui,
                                                 char *name,
                                                 gint width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( "_Modifiers" );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_halign ( item, GTK_ALIGN_CENTER );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( bar ), item );

    g3dui_addMenuButton ( menu, gui, MENU_ADDWIREFRAME , width, G_CALLBACK(g3dui_addWireframeCbk) );
    g3dui_addMenuButton ( menu, gui, MENU_ADDSUBDIVIDER, width, G_CALLBACK(g3dui_addSubdividerCbk) );
    g3dui_addMenuButton ( menu, gui, MENU_ADDFFDBOX    , width, G_CALLBACK(g3dui_addFFDBoxCbk) );
    g3dui_addMenuButton ( menu, gui, MENU_ADDMORPHER   , width, G_CALLBACK(g3dui_addMorpherCbk) );
    g3dui_addMenuButton ( menu, gui, MENU_ADDSKIN      , width, G_CALLBACK(g3dui_addSkinCbk) );

    g3dui_addMenuSeparator ( menu );

    g3dui_addMenuButton ( menu, gui, MENU_ADDBONE  , width, G_CALLBACK(g3dui_addBoneCbk)      );
    createFixBoneMenu   ( menu, gui, MENU_FIXBONE  , width );
    createResetBoneMenu ( menu, gui, MENU_RESETBONE, width );

    g3dui_addMenuSeparator ( menu );
    g3dui_addMenuButton ( menu, gui, MENU_ADDSPLINEREVOLVER , width, G_CALLBACK(g3dui_addSplineRevolverCbk));


/*    g3dui_addMenuButton ( menu, gui, MENU_ADDUVWMAP    , width, g3dui_addLightCbk    , NULL );
*/
    gtk_widget_show     ( item );

    gtk_widget_show     ( menu );



    return menu;
}

/******************************************************************************/
void g3dui_addPlaneCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addPlaneCbk ( gui );
}

/******************************************************************************/
void g3dui_addCubeCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addCubeCbk ( gui );
}

/******************************************************************************/
void g3dui_addSphereCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addSphereCbk ( gui );
}

/******************************************************************************/
void g3dui_addTorusCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addTorusCbk ( gui );
}

/******************************************************************************/
void g3dui_addCylinderCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addCylinderCbk ( gui );
}

/******************************************************************************/
void g3dui_addTubeCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addTubeCbk ( gui );
}

/******************************************************************************/
void g3dui_addLightCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addLightCbk ( gui );
}

/******************************************************************************/
void g3dui_addCameraCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addCameraCbk ( gui, g3dui_getCurrentViewCamera ( gui ) );
}

/******************************************************************************/
void g3dui_addEmptyMeshCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addEmptyMeshCbk ( gui );
}

/******************************************************************************/
void g3dui_addSplineCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addSplineCbk ( gui );
}

/******************************************************************************/
void g3dui_addTextCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addTextCbk ( gui );
}

/******************************************************************************/
void g3dui_addNullCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addNullCbk ( gui );
}

/******************************************************************************/
GtkWidget *createObjectsMenu ( GtkWidget *bar, G3DUI *gui,
                                               char *name,
                                               gint width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( "_Objects" );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_halign ( item, GTK_ALIGN_CENTER );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( bar ), item );

    g3dui_addMenuButton    ( menu, gui, MENU_ADDPLANE    , width, G_CALLBACK(g3dui_addPlaneCbk)    );
    g3dui_addMenuButton    ( menu, gui, MENU_ADDCUBE     , width, G_CALLBACK(g3dui_addCubeCbk)     );
    g3dui_addMenuButton    ( menu, gui, MENU_ADDSPHERE   , width, G_CALLBACK(g3dui_addSphereCbk)   );
    g3dui_addMenuButton    ( menu, gui, MENU_ADDTORUS    , width, G_CALLBACK(g3dui_addTorusCbk)    );
    g3dui_addMenuButton    ( menu, gui, MENU_ADDCYLINDER , width, G_CALLBACK(g3dui_addCylinderCbk) );
    g3dui_addMenuButton    ( menu, gui, MENU_ADDTUBE     , width, G_CALLBACK(g3dui_addTubeCbk)     );
    g3dui_addMenuButton    ( menu, gui, MENU_ADDTEXT     , width, G_CALLBACK(g3dui_addTextCbk)     );
    g3dui_addMenuButton    ( menu, gui, MENU_ADDNULL     , width, G_CALLBACK(g3dui_addNullCbk)     );

    g3dui_addMenuSeparator ( menu );

    g3dui_addMenuButton    ( menu, gui, MENU_ADDLIGHT    , width, G_CALLBACK(g3dui_addLightCbk)     );

    g3dui_addMenuSeparator ( menu );

    g3dui_addMenuButton    ( menu, gui, MENU_ADDCAMERA   , width, G_CALLBACK(g3dui_addCameraCbk)    );

    g3dui_addMenuSeparator ( menu );

    g3dui_addMenuButton    ( menu, gui, MENU_ADDEMPTYMESH, width, G_CALLBACK(g3dui_addEmptyMeshCbk) );

    g3dui_addMenuSeparator ( menu );

    g3dui_addMenuButton    ( menu, gui, MENU_ADDSPLINE   , width, G_CALLBACK(g3dui_addSplineCbk) );


    gtk_widget_show ( item );
    gtk_widget_show ( menu );



    return menu;
}

#endif

/******************************************************************************/
void g3dui_invertNormalCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_invertNormalCbk ( gui );
}

/******************************************************************************/
void g3dui_alignNormalsCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_alignNormalsCbk ( gui );
}

/******************************************************************************/
void g3dui_triangulateCbk ( GtkWidget *widget, gpointer user_data ) {
    const gchar *option = gtk_widget_get_name  ( widget );
    /*const gchar *option = gtk_menu_item_get_label  ( GTK_MENU_ITEM(widget) );*/
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_triangulateCbk ( gui, option );
}

/******************************************************************************/
void g3dui_untriangulateCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_untriangulateCbk ( gui );
}

/******************************************************************************/
void g3dui_weldVerticesCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_weldVerticesCbk ( gui );
}

/******************************************************************************/
void g3dui_deleteLoneVerticesCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_deleteLoneVerticesCbk ( gui );
}

/******************************************************************************/
void g3dui_selectAllCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_selectAllCbk ( gui );
}

/******************************************************************************/
void g3dui_invertSelectionCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_invertSelectionCbk ( gui );
}

/******************************************************************************/
void g3dui_getObjectStatsCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    char buffer[0x200] = { 0 };
    GtkWidget *dialog;

    common_g3dui_getObjectStatsCbk ( gui, buffer, 0x200 );

    if ( strlen ( buffer ) ) {
        dialog = gtk_message_dialog_new ( NULL,
                                          GTK_DIALOG_MODAL,
                                          GTK_MESSAGE_INFO,
                                          GTK_BUTTONS_NONE,
                                          buffer,
                                          NULL );

        gtk_dialog_run ( GTK_DIALOG ( dialog ) );
        gtk_widget_destroy ( dialog );
    }
}

/******************************************************************************/
void g3dui_selectTreeCbk ( GtkWidget *widget, gpointer user_data ) {
    /*const gchar *option = gtk_menu_item_get_label  ( widget );*/
    const gchar *option = gtk_widget_get_name  ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_selectTreeCbk ( gui, option );
}

/******************************************************************************/
gboolean g3dui_exitEventCbk ( GtkWidget *widget, 
                              GdkEvent  *event, 
                              gpointer   user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    GtkWidget *dialog;
    gint       res;

    if ( g3dui_saveAlteredImages ( gui ) ) {
        return TRUE;
    }

    dialog = gtk_message_dialog_new ( NULL,
                                      GTK_DIALOG_MODAL,
                                      GTK_MESSAGE_QUESTION,
                                      GTK_BUTTONS_YES_NO,
                                      "Leave Gloss3D ?" );


    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_YES ) {
        common_g3dui_exitokcbk ( gui );

        return FALSE;
    }

    gtk_widget_destroy ( dialog );

    return TRUE;
}

/******************************************************************************/
void g3dui_exitcbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_message_dialog_new ( NULL,
                                      GTK_DIALOG_MODAL,
                                      GTK_MESSAGE_QUESTION,
                                      GTK_BUTTONS_YES_NO,
                                      "Leave Gloss3D ?" );


    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_YES ) {
        common_g3dui_exitokcbk ( gui );
    }

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
void g3dui_exportfilecbk ( GtkWidget *widget, gpointer user_data ) {
    const gchar *filedesc = gtk_widget_get_name  ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Export file ...",
                                           GTK_WINDOW(ggt->top),
                        /*** from ristretto-0.3.5/src/main_window.c ***/
                                           GTK_FILE_CHOOSER_ACTION_SAVE,
                                           "_Cancel", 
                                           GTK_RESPONSE_CANCEL,
                                           "_Open", 
                                           GTK_RESPONSE_OK,
                                           NULL );

    gtk_file_chooser_set_do_overwrite_confirmation ( GTK_FILE_CHOOSER(dialog),
                                                     TRUE );

    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_OK ) {
        GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
        const char *filename = gtk_file_chooser_get_filename ( chooser );

        common_g3dui_exportfileokcbk ( gui, filedesc, filename );

        g_free    ( ( gpointer ) filename );
    }

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
void g3dui_importfilecbk ( GtkWidget *widget, gpointer user_data ) {
    const gchar *filedesc = gtk_widget_get_name  ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Import file ...",
                                           GTK_WINDOW(ggt->top),
                        /*** from ristretto-0.3.5/src/main_window.c ***/
                                           GTK_FILE_CHOOSER_ACTION_OPEN,
                                           "_Cancel", 
                                           GTK_RESPONSE_CANCEL,
                                           "_Open", 
                                           GTK_RESPONSE_OK,
                                           NULL );


    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_OK ) {
        GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
        const char *filename = gtk_file_chooser_get_filename ( chooser );

        common_g3dui_importfileokcbk ( gui, filedesc, filename );

        g_free    ( ( gpointer ) filename );
    }

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/

static G3DUIMENU menu_separator = { "SEPARATOR",
                                    G3DUIMENUTYPE_SEPARATOR,
                                    NULL,
                                    NULL };
/******************************************************************************/
/****************************** Edit MENU *************************************/

static G3DUIMENU edit_menu_selectTree_all = { MENU_OPTION_ALLTYPES,
                                              G3DUIMENUTYPE_PUSHBUTTON,
                                              NULL,
                                              g3dui_selectTreeCbk };

static G3DUIMENU edit_menu_selectTree_same = { MENU_OPTION_SAMETYPE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               NULL,
                                               g3dui_selectTreeCbk };

/******************************************************************************/

static G3DUIMENU edit_menu_triangulate_cw = { MENU_OPTION_CLOCKWISE,
                                              G3DUIMENUTYPE_PUSHBUTTON,
                                              NULL,
                                              g3dui_triangulateCbk };

static G3DUIMENU edit_menu_triangulate_ccw = { MENU_OPTION_ANTICLOCKWISE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               NULL,
                                               g3dui_triangulateCbk };

/******************************************************************************/

static G3DUIMENU edit_menu_undo = { MENU_UNDO,
                                    G3DUIMENUTYPE_PUSHBUTTON,
                                    NULL,
                                    g3dui_undoCbk };

static G3DUIMENU edit_menu_redo = { MENU_REDO,
                                    G3DUIMENUTYPE_PUSHBUTTON,
                                    NULL,
                                    g3dui_undoCbk };

static G3DUIMENU edit_menu_invertNormals = { MENU_INVERTNORMALS,
                                             G3DUIMENUTYPE_PUSHBUTTON,
                                             faceModeOnly,
                                             g3dui_invertNormalCbk };

static G3DUIMENU edit_menu_alignNormals = { MENU_ALIGNNORMALS,
                                            G3DUIMENUTYPE_PUSHBUTTON,
                                            faceModeOnly,
                                            g3dui_alignNormalsCbk };

static G3DUIMENU edit_menu_triangulate = { MENU_TRIANGULATE,
                                           G3DUIMENUTYPE_SUBMENU,
                                           faceModeOnly,
                                           NULL,
                                          .nodes = { &edit_menu_triangulate_cw,
                                                     &edit_menu_triangulate_ccw,
                                                      NULL } };

static G3DUIMENU edit_menu_untriangulate = { MENU_UNTRIANGULATE,
                                             G3DUIMENUTYPE_PUSHBUTTON,
                                             faceModeOnly,
                                             g3dui_untriangulateCbk };

static G3DUIMENU edit_menu_weldVertices = { MENU_WELDVERTICES,
                                            G3DUIMENUTYPE_PUSHBUTTON,
                                            vertexModeOnly,
                                            g3dui_weldVerticesCbk };

static G3DUIMENU edit_menu_deleteLoneVertices = { MENU_DELETELONEVERTICES,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  vertexModeOnly,
                                                  g3dui_deleteLoneVerticesCbk };

static G3DUIMENU edit_menu_selectAll = { MENU_SELECTALL,
                                         G3DUIMENUTYPE_PUSHBUTTON,
                                         NULL,
                                         g3dui_selectAllCbk };

static G3DUIMENU edit_menu_selectTree = { MENU_SELECTTREE,
                                          G3DUIMENUTYPE_SUBMENU,
                                          objectModeOnly,
                                          NULL,
                                          .nodes = { &edit_menu_selectTree_all,
                                                     &edit_menu_selectTree_same,
                                                      NULL } };

static G3DUIMENU edit_menu_invertSelection = { MENU_INVERTSELECTION,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               NULL,
                                               g3dui_invertSelectionCbk };

static G3DUIMENU edit_menu_getObjectStats = { MENU_GETOBJECTSTATS,
                                              G3DUIMENUTYPE_PUSHBUTTON,
                                              objectMode_objectSelected,
                                              g3dui_getObjectStatsCbk };

/******************************************************************************/

static G3DUIMENU edit_menu = { "Edit",
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = { &edit_menu_undo,
                                         &edit_menu_redo,
                                         &edit_menu_invertNormals,
                                         &edit_menu_alignNormals,
                                         &edit_menu_triangulate,
                                         &edit_menu_untriangulate,
                                         &edit_menu_weldVertices,
                                         &edit_menu_deleteLoneVertices,
                                         &edit_menu_selectAll,
                                         &edit_menu_selectTree,
                                         &edit_menu_invertSelection,
                                         &edit_menu_getObjectStats,
                                          NULL } };

/******************************************************************************/
/****************************** File MENU *************************************/

static G3DUIMENU file_menu_export_obj = { FILEDESC_OBJ,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          g3dui_exportfilecbk };

static G3DUIMENU file_menu_export_pov = { FILEDESC_POV,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          g3dui_exportfilecbk };

static G3DUIMENU file_menu_export_sta = { FILEDESC_STA,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          g3dui_exportfilecbk };

static G3DUIMENU file_menu_export_v1  = { FILEDESC_V1,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          g3dui_exportfilecbk };

/******************************************************************************/

static G3DUIMENU file_menu_import_3ds = { FILEDESC_3DS,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          g3dui_importfilecbk };

static G3DUIMENU file_menu_import_obj = { FILEDESC_OBJ,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          g3dui_importfilecbk };
#ifdef HAVE_EXPAT_H
static G3DUIMENU file_menu_import_dae = { FILEDESC_DAE,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          g3dui_importfilecbk };
#endif
#ifdef HAVE_C4D_H
static G3DUIMENU file_menu_import_c4d = { FILEDESC_C4D,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          g3dui_importfilecbk };
#endif

/******************************************************************************/

static G3DUIMENU file_menu_new    = { MENU_NEWSCENE,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      g3dui_newscenecbk };

static G3DUIMENU file_menu_open   = { MENU_OPENFILE,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      g3dui_openfilecbk };

static G3DUIMENU file_menu_merge  = { MENU_MERGESCENE,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      g3dui_mergeSceneCbk };

static G3DUIMENU file_menu_import = { MENU_IMPORTFILE,
                                     G3DUIMENUTYPE_SUBMENU,
                                     NULL,
                                     NULL,
                                     .nodes = { &file_menu_import_3ds,
                                                &file_menu_import_obj,
#ifdef HAVE_EXPAT_H
                                                &file_menu_import_dae,
#endif
#ifdef HAVE_C4D_H
                                                &file_menu_import_c4d,
#endif
                                                 NULL                 } };

static G3DUIMENU file_menu_save   = { MENU_SAVEFILE,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      g3dui_savefilecbk };

static G3DUIMENU file_menu_saveas = { MENU_SAVEFILEAS,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      g3dui_saveascbk };

static G3DUIMENU file_menu_export = { MENU_EXPORTSCENE,
                                      G3DUIMENUTYPE_SUBMENU,
                                      NULL,
                                      g3dui_exportfilecbk,
                                     .nodes = { &file_menu_export_obj,
                                                &file_menu_export_pov,
                                                &file_menu_export_sta,
                                                &file_menu_export_v1 ,
                                                 NULL                 } };

static G3DUIMENU file_menu_exit   = { MENU_EXIT,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      g3dui_exitcbk };

/******************************************************************************/

static G3DUIMENU file_menu = { "File",
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = { &file_menu_new,
                                         &file_menu_open,
                                         &file_menu_merge,
                                         &file_menu_import,
                                         &menu_separator,
                                         &file_menu_save,
                                         &file_menu_saveas,
                                         &file_menu_export,
                                         &menu_separator,
                                         &file_menu_exit,
                                          NULL } };

/******************************************************************************/

static G3DUIMENU rootnode = { "Bar",
                              G3DUIMENUTYPE_MENUBAR,
                              NULL,
                              NULL,
                             .nodes = { &file_menu,
                                        &edit_menu,
                                         NULL } };

/******************************************************************************/
static void parseMenu_r ( G3DUIMENU *node, 
                          G3DUIMENU *parent, 
                          G3DUI     *gui ) {
    switch ( node->type ) {
        case G3DUIMENUTYPE_SEPARATOR :
            node->item = gtk_separator_menu_item_new ( );
        break;

        case G3DUIMENUTYPE_TOGGLEBUTTON :
            node->item = gtk_check_menu_item_new_with_mnemonic ( node->name );

            if ( node->callback ) {
                g_signal_connect ( G_OBJECT ( node->item ), 
                                   "toggled", 
                                   node->callback, 
                                   gui );
            }
        break;

        case G3DUIMENUTYPE_PUSHBUTTON :
            node->item = gtk_menu_item_new_with_mnemonic ( node->name );

            if ( node->callback ) {
                g_signal_connect ( G_OBJECT ( node->item ), 
                                   "activate", 
                                   node->callback, 
                                   gui );
            }
        break;

        case G3DUIMENUTYPE_MENUBAR : {
            uint32_t i = 0x00;

            node->menu = gtk_menu_bar_new ( );

            while ( node->nodes[i] != NULL ) {
                parseMenu_r ( node->nodes[i], node, gui );

                i++;
            }

            gtk_widget_show ( node->menu );
        } break;

        case G3DUIMENUTYPE_SUBMENU : {
            uint32_t i = 0x00;

            node->menu = gtk_menu_new ( );
            node->item = gtk_menu_item_new_with_mnemonic ( node->name );

            gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( node->item ), node->menu );

            while ( node->nodes[i] != NULL ) {
                parseMenu_r ( node->nodes[i], node, gui );

                i++;
            }

            gtk_widget_show ( node->menu );

            gtk_widget_set_size_request ( node->menu, 0x60, 24 );
        } break;

        default :
        break;
    }

    /*if ( node->item ) {
    int height = gtk_widget_get_allocated_height ( node->item );

    gtk_widget_set_size_request ( node->item, 0x60, height );
    }*/

    if ( parent ) {
        gtk_menu_shell_append ( GTK_MENU_SHELL ( parent->menu ), 
                                node->item );
    }

    gtk_widget_set_name ( node->item, node->name );
    gtk_widget_show     ( node->item );
}

/******************************************************************************/
static void updateMenu_r ( G3DUIMENU *node, 
                           G3DUI     *gui ) {
    switch ( node->type ) {
        case G3DUIMENUTYPE_SEPARATOR :
        break;

        case G3DUIMENUTYPE_TOGGLEBUTTON :

        break;

        case G3DUIMENUTYPE_PUSHBUTTON :
        break;

        case G3DUIMENUTYPE_MENUBAR :
        case G3DUIMENUTYPE_SUBMENU : {
            uint32_t i = 0x00;

            while ( node->nodes[i] != NULL ) {
                updateMenu_r ( node->nodes[i], gui );

                i++;
            }
        } break;

        default :
        break;
    }

    if ( node->item ) {
        if ( node->condition ) {
            if ( node->condition ( gui ) == 0x00 ) {
                gtk_widget_set_sensitive ( node->item, FALSE );
            } else {
                gtk_widget_set_sensitive ( node->item, TRUE  );
            }
        } else {
            gtk_widget_set_sensitive ( node->item, TRUE  );
        }
    }
}

/******************************************************************************/
void g3dui_updateMenuBar ( G3DUI *gui ) {
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;

    updateMenu_r ( ggt->menuBar, gui );
}

/******************************************************************************/
void createMenuBar ( G3DUI *gui,
                     char  *name,
                     gint   x,
                     gint   y,
                     gint   width,
                     gint   height ) {
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;

    GdkRectangle gdkrec = { x, y, width, height };
    /*GtkWidget *bar = gtk_menu_bar_new ( );*/

    parseMenu_r ( &rootnode, NULL, gui );

    gtk_widget_size_allocate ( rootnode.menu, &gdkrec );
    gtk_fixed_put ( GTK_FIXED(ggt->main), rootnode.menu, x, y );

    /*createFileMenu        ( bar, gui, "FileMenu"       , 60 );*/


    /*createEditMenu        ( bar, gui, "EditMenu"       , 60 );
    createObjectsMenu     ( bar, gui, "ObjectsMenu"    , 90 );
    createModifiersMenu   ( bar, gui, "ModifiersMenu"  , 90 );
    createMultipliersMenu ( bar, gui, "MultipliersMenu", 90 );
    createFunctionsMenu   ( bar, gui, "FunctionsMenu"  , 90 );
    createUVMappingMenu   ( bar, gui, "UVMappingMenu"  , 90 );
    createRenderMenu      ( bar, gui, "RenderMenu"     , 90 );
    createHelpMenu        ( bar, gui, "HelpMenu"       , 90 );*/

    gtk_widget_show ( rootnode.menu );


    ggt->menuBar = &rootnode;
}
