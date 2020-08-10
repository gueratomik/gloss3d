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
GtkWidget *createUVHelpMenu ( GtkWidget *bar, G3DUI *gui,
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
void l3dui_resizeChannelImageCbk ( GtkWidget *widget, 
                                              gpointer   user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;
    G3DCHANNEL *chn = common_l3dui_getWorkingChannel ( lui );

    if ( chn ) {
        GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );

        createChannelImage ( dial, 
                             lui, 
                             chn,
                             0x01,
                             "Channel Image", 0, 0, 200, 96 );

        gtk_widget_show ( dial );
    }
}

/******************************************************************************/
void l3dui_fgfillCbk ( GtkWidget *widget, gpointer user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;
    L3DSYSINFO *sysinfo = l3dsysinfo_get ( );

    common_l3dui_fillWithColor ( lui, sysinfo->fgcolor );
}

/******************************************************************************/
void l3dui_bgfillCbk ( GtkWidget *widget, gpointer user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;
    L3DSYSINFO *sysinfo = l3dsysinfo_get ( );

    common_l3dui_fillWithColor ( lui, sysinfo->bgcolor );
}

/******************************************************************************/
GtkWidget *createUVImageMenu ( GtkWidget        *bar,
                               L3DUI *lui,
                               char             *name,
                               gint              width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( "_Image" );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_halign ( item, GTK_ALIGN_CENTER );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( bar ), item );

    g3dui_addMenuButton ( menu, 
                          lui, 
                          MENU_RESIZEIMAGE, 
                          width, 
                          G_CALLBACK ( l3dui_resizeChannelImageCbk ) );

    g3dui_addMenuButton ( menu, 
                          lui, 
                          MENU_FGFILL, 
                          width, 
                          G_CALLBACK ( l3dui_fgfillCbk ) );

    g3dui_addMenuButton ( menu, 
                          lui, 
                          MENU_BGFILL, 
                          width, 
                          G_CALLBACK ( l3dui_bgfillCbk ) );

    gtk_widget_show ( item );
    gtk_widget_show ( menu );



    return menu;
}

/******************************************************************************/
void l3dui_fac2uvsetCbk ( GtkWidget *widget, gpointer user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;

    common_l3dui_fac2uvsetCbk ( lui );

    g3dui_redrawUVMapEditors ( lui->gui );
}

/******************************************************************************/
void l3dui_uvset2facCbk ( GtkWidget *widget, gpointer user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;

    common_l3dui_uvset2facCbk ( lui );

    g3dui_redrawGLViews ( lui->gui );
}

/******************************************************************************/
void l3dui_ver2uvCbk ( GtkWidget *widget, gpointer user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;

    common_l3dui_ver2uvCbk ( lui );

    g3dui_redrawUVMapEditors ( lui->gui );
}

/******************************************************************************/
void l3dui_uv2verCbk ( GtkWidget *widget, gpointer user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;

    common_l3dui_uv2verCbk ( lui );

    g3dui_redrawGLViews ( lui->gui );
}

/******************************************************************************/
GtkWidget *createUVEditMenu ( GtkWidget        *bar,
                              L3DUI *lui,
                              char             *name,
                              gint              width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( "_Mapping" );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_halign ( item, GTK_ALIGN_CENTER );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( bar ), item );

    g3dui_addMenuButton   ( menu, lui, MENU_UNDO         , width, G_CALLBACK(l3dui_undoCbk)   );
    g3dui_addMenuButton   ( menu, lui, MENU_REDO         , width, G_CALLBACK(l3dui_redoCbk)   );
    g3dui_addMenuButton   ( menu, lui, MENU_VERTEXFROMUV , width, G_CALLBACK(l3dui_uv2verCbk) );
    g3dui_addMenuButton   ( menu, lui, MENU_UVFROMVERTEX , width, G_CALLBACK(l3dui_ver2uvCbk) );
    g3dui_addMenuButton   ( menu, lui, MENU_FACEFROMUVSET, width, G_CALLBACK(l3dui_uvset2facCbk) );
    g3dui_addMenuButton   ( menu, lui, MENU_UVSETFROMFACE, width, G_CALLBACK(l3dui_fac2uvsetCbk) );

/*    createTriangulateMenu ( menu, gui, MENU_TRIANGULATE       , width );
    g3dui_addMenuButton   ( menu, gui, MENU_UNTRIANGULATE     , width, G_CALLBACK(g3dui_untriangulateCbk)      );
    g3dui_addMenuButton   ( menu, gui, MENU_WELDVERTICES      , width, G_CALLBACK(g3dui_weldVerticesCbk)       );
    g3dui_addMenuButton   ( menu, gui, MENU_DELETELONEVERTICES, width, G_CALLBACK(g3dui_deleteLoneVerticesCbk) );
    g3dui_addMenuButton   ( menu, gui, MENU_SELECTALL         , width, G_CALLBACK(g3dui_selectAllCbk)          );
    createSelectTreeMenu  ( menu, gui, MENU_SELECTTREE        , width );
    g3dui_addMenuButton   ( menu, gui, MENU_INVERTSELECTION   , width, G_CALLBACK(g3dui_invertSelectionCbk)    );
    g3dui_addMenuButton   ( menu, gui, MENU_GETOBJECTSTATS    , width, G_CALLBACK(g3dui_getObjectStatsCbk)     );
*/
    gtk_widget_show ( item );
    gtk_widget_show ( menu );



    return menu;
}

/******************************************************************************/
void l3dui_loadImageByChannelIDCbk ( GtkWidget *widget, 
                                                gpointer   user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;
    G3DUI *gui = ( G3DUI * ) lui->gui;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( gui->sce );

    if ( obj ) {
        if ( obj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            /*** try the first texture in case no texture is selected ***/
            if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

            if ( tex ) {
                G3DMATERIAL *mat = tex->mat;
                uint32_t chnID = GETCHANNEL(lui->engine_flags);
                G3DCHANNEL  *chn = g3dmaterial_getChannelByID ( mat, chnID );

                g3dui_loadImageForChannel ( gui, chn );

                /*** resize selection mask and zbuffer ***/
                common_l3dui_resizeBuffers ( lui );
            }
        }
    }
}

/******************************************************************************/
void l3dui_createChannelImageCbk ( GtkWidget *widget, 
                                              gpointer   user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;
    G3DCHANNEL *chn = common_l3dui_getWorkingChannel ( lui );

    if ( chn ) {
        GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );

        createChannelImage ( dial, 
                             lui, 
                             chn,
                             0x00,
                             "Channel Image", 0, 0, 200, 96 );

        gtk_widget_show ( dial );
    }
}

/******************************************************************************/
GtkWidget *createUVFileMenu ( GtkWidget        *bar,
                              L3DUI *lui,
                              char             *name,
                              gint              width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( "_File" );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_halign ( item, GTK_ALIGN_CENTER );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( bar ), item );

    g3dui_addMenuButton ( menu, lui, MENU_CREATECHANNELIMAGE, width, G_CALLBACK(l3dui_createChannelImageCbk) );
    g3dui_addMenuButton ( menu, lui, MENU_OPENCHANNELIMAGE  , width, G_CALLBACK(l3dui_loadImageByChannelIDCbk) );
    /*g3dui_addMenuButton ( menu, lui, MENU_SAVECHANNELIMAGE  , width, G_CALLBACK(l3dui_saveChannelImageCbk) );
    g3dui_addMenuButton ( menu, lui, MENU_SAVECHANNELIMAGEAS, width, G_CALLBACK(l3dui_saveChannelImageAsCbk) );
    g3dui_addMenuButton ( menu, lui, MENU_LEAVELIPS3D       , width, G_CALLBACK(l3dui_exitLIPS3DCbk) );*/

    gtk_widget_show ( item );
    gtk_widget_show ( menu );



    return menu;
}

/******************************************************************************/
/*GtkWidget *createUVFileMenu ( GtkWidget        *bar,
                              L3DUI *lui,
                              char             *name,
                              gint              width ) {
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
}*/

/******************************************************************************/
GtkWidget *createUVMenuBar ( GtkWidget        *parent,  
                             L3DUI *lui,
                             char             *name,
                             gint              x,
                             gint              y,
                             gint              width,
                             gint              height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *bar = gtk_menu_bar_new ( );

    gtk_widget_set_name ( bar, name );

    gtk_widget_size_allocate ( bar, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), bar, x, y );

    createUVFileMenu  ( bar, lui, "FileMenu"  , 60 );
    createUVEditMenu  ( bar, lui, "EditMenu"  , 60 );
    createUVImageMenu ( bar, lui, "ImageMenu" , 60 );

    /*createUVHelpMenu ( bar, gui, "HelpMenu" , 90 );*/

    gtk_widget_show ( bar );


    return bar;
}
