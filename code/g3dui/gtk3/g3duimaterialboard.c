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
static void setMaterialCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_setMaterialCbk ( gui );
}

/******************************************************************************/
static void removeMaterialCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    LIST *ltmpmatlist = gui->lmatlist;

    if ( gui->selmat ) {
        while ( ltmpmatlist ) {
            GtkWidget *matlst = ( GtkWidget * ) ltmpmatlist->data;

            g3duimateriallist_removeMaterial ( matlst, gui->sce, 
                                                       gui->urm,
                                                       gui->selmat );

            gui->selmat = NULL;


            ltmpmatlist = ltmpmatlist->next;
        }
    }

    g3dui_redrawMaterialList ( gui );
    g3dui_updateMaterialEdit ( gui );
    g3dui_redrawGLViews      ( gui );
}

/******************************************************************************/
static void addMaterialCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DMATERIAL *mat = g3dmaterial_new ( "Material" );
    G3DUI *gui = ( G3DUI * ) user_data;
    LIST *ltmpmatlist = gui->lmatlist;

    g3durm_scene_addMaterial ( gui->urm,
                               gui->sce, 
                               mat,
                               gui->engine_flags,
                               REBUILDMATERIALLIST );

    while ( ltmpmatlist ) {
        GtkWidget *matlst = ( GtkWidget * ) ltmpmatlist->data;

        g3duimateriallist_addPreview ( matlst, mat );

        ltmpmatlist = ltmpmatlist->next;
    }

    gui->selmat = mat;

    g3dui_redrawMaterialList ( gui );
    g3dui_updateMaterialEdit ( gui );
}

/******************************************************************************/
static GtkWidget *createMaterialFileMenu ( GtkWidget *bar, G3DUI *gui,
                                                           char *name,
                                                           gint width ) {
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( "_File" );
    int height = gtk_widget_get_allocated_height ( item );

    gtk_widget_set_halign ( item, GTK_ALIGN_CENTER );

    gtk_widget_set_size_request ( item, width, height );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( bar ), item );

    /************************************************/
#ifdef unused
    g3dui_addMenuButton ( menu, gui, "Add Material"   , 96,
                          G_CALLBACK(addMaterialCbk) );

    g3dui_addMenuButton ( menu, gui, "Delete Material", 96,
                          G_CALLBACK(removeMaterialCbk) );

    g3dui_addMenuButton ( menu, gui, "Set Material"   , 96,
                          G_CALLBACK(setMaterialCbk) );
#endif

    /*addMenuButton ( wfile, gui, "Small Icons"    , smallpreviewscbk  );
    addMenuButton ( wfile, gui, "Normal Icons"   , normalpreviewscbk );
    addMenuButton ( wfile, gui, "Big Icons"      , bigpreviewscbk    );*/

    gtk_widget_show ( item );
    gtk_widget_show ( menu );

    return menu;
}

/******************************************************************************/
static GtkWidget *createMaterialMenuBar ( GtkWidget *parent, G3DUI *gui,
                                                             char *name,
                                                             gint x,
                                                             gint y,
                                                             gint width,
                                                             gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *bar = gtk_menu_bar_new ( );

    gtk_widget_set_name ( bar, name );

    /*gtk_widget_size_allocate ( bar, &gdkrec );*/
    gtk_widget_set_size_request ( bar, width, height );

    gtk_fixed_put ( GTK_FIXED(parent), bar, x, y );

    createMaterialFileMenu ( bar, gui, "FileMenu", 60 );

    gtk_widget_show ( bar );


    return bar;
}

/******************************************************************************/
GtkWidget *createMaterialBoard ( GtkWidget *parent, G3DUI *gui,
                                                    char *name,
                                                    gint x,
                                                    gint y,
                                                    gint width,
                                                    gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *label = gtk_label_new ( name );
    GtkWidget *curedit, *objlist, *tab, *frm, *menu;
    uint32_t menuheight;

    frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &gdkrec );

    menu = createMaterialMenuBar ( frm, gui, "MENU", 0, 0, width, 32 );

    /*menuheight = gtk_widget_get_allocated_height ( menu );*/

    createMaterialList     ( frm, gui, "Materials"    , 0,  32, 320, 320 );
    createMaterialEdit     ( frm, gui, "Material Edit", 0, 352, 304, 168 );

    gtk_notebook_append_page ( GTK_NOTEBOOK(parent), frm, label );


    gtk_widget_show_all ( frm );


    return frm;
}
