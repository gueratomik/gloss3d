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

/******************************************************************************/
GtkWidget *g3dui_addMenuSeparator ( GtkWidget *menu ) {
    GtkWidget *item = gtk_separator_menu_item_new ( );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( menu ), item );


    gtk_widget_show ( item );


    return item;
} 

/******************************************************************************/
GtkWidget *g3dui_addMenuToggle ( GtkWidget *menu, G3DUI *gui,
                                                  const gchar *name,
                                                  gint width,
                                                  GCallback callback ) {
    GtkWidget *item = gtk_check_menu_item_new_with_mnemonic ( name );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( menu ), item );

    gtk_widget_set_name ( item, name );

    if ( callback ) {
        g_signal_connect ( G_OBJECT ( item ), "toggled", callback, gui );
    }

    gtk_widget_show ( item );


    return item;
}

/******************************************************************************/
GtkWidget *g3dui_addMenuButton ( GtkWidget *menu, G3DUI *gui,
                                                  const gchar *name,
                                                  gint width,
                                                  GCallback callback ) {
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( name );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( menu ), item );

    gtk_widget_set_name ( item, name );

    if ( callback ) {
        g_signal_connect ( G_OBJECT ( item ), "activate", callback, gui );
    }

    gtk_widget_show ( item );


    return item;
}

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
GtkWidget *createSelectTreeMenu ( GtkWidget *parent, G3DUI *gui,
                                                     char *name,
                                                     gint width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( name );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( parent ), item );

    g3dui_addMenuButton ( menu, gui, MENU_OPTION_ALLTYPES, width, G_CALLBACK(g3dui_selectTreeCbk) );
    g3dui_addMenuButton ( menu, gui, MENU_OPTION_SAMETYPE, width, G_CALLBACK(g3dui_selectTreeCbk) );

    gtk_widget_show ( item );
    gtk_widget_show ( menu );



    return menu;
}

/******************************************************************************/
GtkWidget *createTriangulateMenu ( GtkWidget *parent, G3DUI *gui,
                                                      char *name,
                                                      gint width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( name );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( parent ), item );

    g3dui_addMenuButton ( menu, gui, MENU_OPTION_CLOCKWISE    , width, G_CALLBACK(g3dui_triangulateCbk) );
    g3dui_addMenuButton ( menu, gui, MENU_OPTION_ANTICLOCKWISE, width, G_CALLBACK(g3dui_triangulateCbk) );

    gtk_widget_show ( item );
    gtk_widget_show ( menu );



    return menu;
}

/******************************************************************************/
GtkWidget *createEditMenu ( GtkWidget *bar, G3DUI *gui,
                                            char *name,
                                            gint width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( "_Edit" );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_halign ( item, GTK_ALIGN_CENTER );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( bar ), item );

    g3dui_addMenuButton   ( menu, gui, MENU_UNDO              , width, G_CALLBACK(g3dui_undoCbk)               );
    g3dui_addMenuButton   ( menu, gui, MENU_REDO              , width, G_CALLBACK(g3dui_redoCbk)               );
    g3dui_addMenuButton   ( menu, gui, MENU_INVERTNORMALS     , width, G_CALLBACK(g3dui_invertNormalCbk)       );
    g3dui_addMenuButton   ( menu, gui, MENU_ALIGNNORMALS      , width, G_CALLBACK(g3dui_alignNormalsCbk)       );
    createTriangulateMenu ( menu, gui, MENU_TRIANGULATE       , width );
    g3dui_addMenuButton   ( menu, gui, MENU_UNTRIANGULATE     , width, G_CALLBACK(g3dui_untriangulateCbk)      );
    g3dui_addMenuButton   ( menu, gui, MENU_WELDVERTICES      , width, G_CALLBACK(g3dui_weldVerticesCbk)       );
    g3dui_addMenuButton   ( menu, gui, MENU_DELETELONEVERTICES, width, G_CALLBACK(g3dui_deleteLoneVerticesCbk) );
    g3dui_addMenuButton   ( menu, gui, MENU_SELECTALL         , width, G_CALLBACK(g3dui_selectAllCbk)          );
    createSelectTreeMenu  ( menu, gui, MENU_SELECTTREE        , width );
    g3dui_addMenuButton   ( menu, gui, MENU_INVERTSELECTION   , width, G_CALLBACK(g3dui_invertSelectionCbk)    );
    g3dui_addMenuButton   ( menu, gui, MENU_GETOBJECTSTATS    , width, G_CALLBACK(g3dui_getObjectStatsCbk)     );

    gtk_widget_show ( item );
    gtk_widget_show ( menu );



    return menu;
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
                                      "Leave Gloss3D :-( ?" );


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
GtkWidget *createImportMenu ( GtkWidget *parent, G3DUI *gui,
                                                 char *name,
                                                 gint width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( name );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( parent ), item );

    g3dui_addMenuButton ( menu, gui, FILEDESC_3DS, width, G_CALLBACK(g3dui_importfilecbk) );
    g3dui_addMenuButton ( menu, gui, FILEDESC_OBJ, width, G_CALLBACK(g3dui_importfilecbk) );
#ifdef HAVE_EXPAT_H
    g3dui_addMenuButton ( menu, gui, FILEDESC_DAE, width, G_CALLBACK(g3dui_importfilecbk) );
#endif
#ifdef HAVE_C4D_H
    g3dui_addMenuButton ( menu, gui, FILEDESC_C4D, width, G_CALLBACK(g3dui_importfilecbk) );
#endif
    g3dui_addMenuButton ( menu, gui, FILEDESC_V1, width, G_CALLBACK(g3dui_importfilecbk) );

    gtk_widget_show ( item );
    gtk_widget_show ( menu );



    return menu;
}

/******************************************************************************/
GtkWidget *createExportMenu ( GtkWidget *parent, G3DUI *gui,
                                                 char *name,
                                                 gint width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( name );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( parent ), item );

    g3dui_addMenuButton ( menu, gui, FILEDESC_OBJ, width, G_CALLBACK(g3dui_exportfilecbk) );
    g3dui_addMenuButton ( menu, gui, FILEDESC_POV, width, G_CALLBACK(g3dui_exportfilecbk) );
    g3dui_addMenuButton ( menu, gui, FILEDESC_STA, width, G_CALLBACK(g3dui_exportfilecbk) );
    g3dui_addMenuButton ( menu, gui, FILEDESC_V1, width, G_CALLBACK(g3dui_exportfilecbk) );

    gtk_widget_show ( item );
    gtk_widget_show ( menu );



    return menu;
}

/******************************************************************************/
GtkWidget *createFileMenu ( GtkWidget *bar, G3DUI *gui,
                                            char *name,
                                            gint width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( "_File" );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_halign ( item, GTK_ALIGN_CENTER );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( bar ), item );

    g3dui_addMenuButton    ( menu, gui, MENU_NEWSCENE   , width, G_CALLBACK(g3dui_newscenecbk) );
    g3dui_addMenuButton    ( menu, gui, MENU_OPENFILE   , width, G_CALLBACK(g3dui_openfilecbk) );
    g3dui_addMenuButton    ( menu, gui, MENU_MERGESCENE , width, G_CALLBACK(g3dui_mergeSceneCbk) );
    createImportMenu       ( menu, gui, MENU_IMPORTFILE , width );

    g3dui_addMenuSeparator ( menu );

    g3dui_addMenuButton    ( menu, gui, MENU_SAVEFILE   , width, G_CALLBACK(g3dui_savefilecbk) );
    g3dui_addMenuButton    ( menu, gui, MENU_SAVEFILEAS , width, G_CALLBACK(g3dui_saveascbk)   );
    createExportMenu       ( menu, gui, MENU_EXPORTSCENE, width );

    g3dui_addMenuSeparator ( menu );

    g3dui_addMenuButton    ( menu, gui, MENU_EXIT       , width, G_CALLBACK(g3dui_exitcbk)     );


    gtk_widget_show ( item );
    gtk_widget_show ( menu );



    return menu;
}

/******************************************************************************/
GtkWidget *createMenuBar ( GtkWidget *parent, G3DUI *gui,
                                              char *name,
                                              gint x,
                                              gint y,
                                              gint width,
                                              gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *bar = gtk_menu_bar_new ( );

    gtk_widget_set_name ( bar, name );

    gtk_widget_size_allocate ( bar, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), bar, x, y );

    createFileMenu        ( bar, gui, "FileMenu"       , 60 );
    createEditMenu        ( bar, gui, "EditMenu"       , 60 );
    createObjectsMenu     ( bar, gui, "ObjectsMenu"    , 90 );
    createModifiersMenu   ( bar, gui, "ModifiersMenu"  , 90 );
    createMultipliersMenu ( bar, gui, "MultipliersMenu", 90 );
    createFunctionsMenu   ( bar, gui, "FunctionsMenu"  , 90 );
    createUVMappingMenu   ( bar, gui, "UVMappingMenu"  , 90 );
    createRenderMenu      ( bar, gui, "RenderMenu"     , 90 );
    createHelpMenu        ( bar, gui, "HelpMenu"       , 90 );

    gtk_widget_show ( bar );


    return bar;
}
