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
static GTK3G3DUIMAIN *g3tk_g3duimain_new ( GTK3G3DUI *gtk3gui ) {
    uint32_t memSize =  sizeof ( GTK3G3DUIMAIN );
    GTK3G3DUIMAIN *gtk3main = ( GTK3G3DUIMAIN * ) calloc ( 0x01, memSize );

    if ( gtk3main == NULL ) {
        fprint ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3main->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3main;
}

/******************************************************************************/
static void SizeAllocate ( GtkWidget     *widget,
                           GtkAllocation *allocation,
                           gpointer       user_data ) {
    GTK3G3DUIMAIN    *gtk3main    = ( GTK3G3DUIMAIN *    ) user_data;
    GTK3G3DUITOOLBAR *gtk3toolBar = ( GTK3G3DUITOOLBAR * ) gtk3main->core.toolBar;
    GdkRectangle gdkrec;

    g3duimain_sizeAllocate ( &gtk3main->core, 
                              allocation->width, 
                              allocation->height );

    gdkrec.x      = gtk3main->core.tbarrec.x;
    gdkrec.y      = gtk3main->core.tbarrec.y;
    gdkrec.width  = gtk3main->core.tbarrec.width;
    gdkrec.height = gtk3main->core.tbarrec.height;

    gtk_layout_move ( widget, 
                      gtk3toolBar->bar,
                      gdkrec.x,
                      gdkrec.y );

    gtk_widget_size_allocate ( gtk3toolBar->bar, &gdkrec );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, 
                      gpointer   user_data ) {
    GTK3G3DUIMAIN *gtk3main = ( GTK3G3DUIMAIN * ) user_data;

    free ( gtk3main );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, 
                      gpointer   user_data ) {
    GTK3G3DUIMAIN *gtk3main = ( GTK3G3DUIMAIN * ) user_data;
    G3DUI *gui = gtk3main->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    GtkCssProvider *provider = gtk_css_provider_new ();
    static const gchar *myCSS = { "* {                      \n"
                                  "    background-color: rgba(192,192,192,1); \n"
    #ifdef __linux__

                                  "    font-family: Lucida Sans; \n"
                                  "    font-size: 10px; \n"
    #endif
    #ifdef __MINGW32__
                                  "    font-family: Calibri; \n"
                                  "    font-size: 10px; \n"
    #endif
                                  "}                        \n"
    #ifdef UBUNTU16
                                  "GtkEntry {               \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "GtkButton {              \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "GtkSpinButton {          \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     1px; \n"
                                  "    margin-right:   1px; \n"
                                  "    margin-left:    1px; \n"
                                  "    margin-bottom:  1px; \n"
                                  "    padding-top:    1px; \n"
                                  "    padding-right:  1px; \n"
                                  "    padding-left:   1px; \n"
                                  "    padding-bottom: 1px; \n"
                                  "}                        \n"
                                  "GtkQuad {                \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "GtkMenuItem {\n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     1px; \n"
                                  "    margin-right:   1px; \n"
                                  "    margin-left:    1px; \n"
                                  "    margin-bottom:  1px; \n"
                                  "    padding-top:    2px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 2px; \n"
                                  "    color: rgba(0,0,0,1); \n"
                                  "    font-size:      6; \n"
                                  "}                        \n"
                                  "GtkMenuItem#Option_Menu {\n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     1px; \n"
                                  "    margin-right:   1px; \n"
                                  "    margin-left:    1px; \n"
                                  "    margin-bottom:  1px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "    font-size:      6; \n"
                                  "}                        \n"
                                  "GtkComboBoxText#Shading_Menu {   \n"
                                  "    border-width:   0px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "    font-size:      6; \n"
                                  "}                        \n"
                                  "GtkToolbar {             \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
    #else
                                  "entry {               \n"
                                  "    min-height:     0px; \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "scale {               \n"
                                  "    min-height:     0px; \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "tab {               \n"
                                  "    min-height:     0px; \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  8px; \n"
                                  "    padding-left:   8px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "button {              \n"
                                  "    min-height:     0px; \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "spinbutton {          \n"
                                  "    min-height:     0px; \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "quad {                \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "menuitem {\n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     1px; \n"
                                  "    margin-right:   1px; \n"
                                  "    margin-left:    1px; \n"
                                  "    margin-bottom:  1px; \n"
                                  "    padding-top:    2px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 2px; \n"
                                  "    font-size:      6px; \n"
                                  "}                        \n"
                                  "menuitem#Option_Menu {\n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     1px; \n"
                                  "    margin-right:   1px; \n"
                                  "    margin-left:    1px; \n"
                                  "    margin-bottom:  1px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "    font-size:      6px; \n"
                                  "}                        \n"
                                  "comboboxtext#Shading_Menu {   \n"
                                  "    border-width:   0px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "    font-size:      6px; \n"
                                  "}                        \n"
                                  "toolbar {             \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
    #endif
                                  };
    GdkDisplay *display = gdk_display_get_default ( );
    GdkScreen  *screen  = gdk_display_get_default_screen ( display );
    GtkWidget *tab;

    gtk_style_context_add_provider_for_screen ( screen,
                                 GTK_STYLE_PROVIDER (provider),
                                 GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    gtk_css_provider_load_from_data ( provider, myCSS, -1, NULL );

    g_object_unref ( provider );

    g3dui_init ( gui );

    gtk3_g3dui_initDefaultMouseTools ( gtk3gui, 
                                       gtk3_g3dui_getMainViewCamera ( gtk3gui ) );

    gtk3_g3duimain_updateMenuBar ( gtk3main );
}

/******************************************************************************/
void gtk3_g3duimain_updateMenuBar ( GTK3G3DUIMAIN *gtk3main ) {
    updateMenu_r ( gtk3main->core.menuBar, 
                   gtk3main->core.gui );
}

/******************************************************************************/
void gtk3_g3duimain_createMenuBar ( GTK3G3DUIMAIN *gtk3main,
                                    gint           x,
                                    gint           y,
                                    gint           width,
                                    gint           height ) {

    GdkRectangle gdkrec = { x, y, width, height };
    /*GtkWidget *bar = gtk_menu_bar_new ( );*/

    GTK3G3DUIMENU *gtk3menu;

    gtk3menu = gtk3_g3duimenu_parse_r ( g3duimenu_getMainMenuNode ( ),
                                        gtk3main );

    gtk_widget_size_allocate ( gtk3menu->menu, &gdkrec );

    gtk_layout_put ( GTK_LAYOUT(gtk3main->layout), gtk3menu->menu, x, y );

    gtk_widget_show ( gtk3menu->menu );


    gtk3main->core.menuBar = ( G3DUIMENU * ) gtk3menu;
}

/******************************************************************************/
void gtk3_g3duimain_createToolBar ( GTK3G3DUIMAIN *gtk3main,
                                    gint           x,
                                    gint           y,
                                    gint           width,
                                    gint           height ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3main->core.gui;
    GdkRectangle gdkrec = { x, y, width, height };
    /*GtkWidget *bar = gtk_menu_bar_new ( );*/

    GTK3G3DUITOOLBAR *gtk3tb = gtk3_g3duitoolbar_create ( gtk3main->layout,
                                                          gtk3gui,
                                                          "toolbar",
                                                          0,
                                                          0,
                                                          width,
                                                          32 );

    gtk_widget_size_allocate ( gtk3tb->bar, &gdkrec );

    gtk_layout_put ( GTK_LAYOUT(gtk3main->layout), gtk3tb->bar, x, y );

    gtk_widget_show ( gtk3tb->bar );


    gtk3main->core.toolBar = ( G3DUITOOLBAR * ) gtk3tb;
}


/******************************************************************************/
GTK3G3DUIMAIN *gtk3_g3duimain_create ( GtkWidget *parent,
                                       GTK3G3DUI *gtk3gui,
                                       char      *name,
                                       gint       x,
                                       gint       y,
                                       gint       width,
                                       gint       height,
                                       char      *filename ) {
    G3DUI *gui = &gtk3gui->core;
    GTK3G3DUIMAIN *gtk3main = g3tk_g3duimain_new ( gtk3gui );
    GdkRectangle  gdkrec  = { 0x00,  0x00, width, height };
    GtkWidget    *layout  = gtk_layout_new ( NULL, NULL );

    g_object_set_data ( G_OBJECT(layout), "private", (gpointer) gtk3main );

    gtk_widget_set_name ( layout, name );

    gtk_widget_size_allocate ( layout, &gdkrec );

    gtk_container_add ( GTK_CONTAINER(parent), layout );

    gtk_widget_add_events(GTK_WIDGET(layout), GDK_CONFIGURE);

    g_signal_connect ( G_OBJECT (layout), "realize"      , G_CALLBACK (Realize)     , gtk3main );
    g_signal_connect ( G_OBJECT (layout), "destroy"      , G_CALLBACK (Destroy)     , gtk3main );
    g_signal_connect ( G_OBJECT (layout), "size-allocate", G_CALLBACK (SizeAllocate), gtk3main );

    gtk3main->layout = layout;

    /*gtk3_g3duimain_createMenuBar ( gtk3main,
                                   0x00,
                                   0x00,
                                   0x00,
                                   0x00 );*/


    gtk3main->core.toolBar = gtk3_g3duitoolbar_create ( layout,
                                                        gtk3gui,
                                                        "toolbar",
                                                        0,
                                                        0,
                                                        width,
                                                        32 );

    

    gtk_widget_show ( layout );


    /*** File loading must be done AFTER OpenGL init ***/
    if ( filename ) {
        gtk3_interpretUIReturnFlags ( g3dui_openG3DFile ( gui, filename ) );
    } else {
        gui->sce = g3dscene_new  ( 0x00, "Gloss3D scene" );
    }


    return gtk3main;
}
