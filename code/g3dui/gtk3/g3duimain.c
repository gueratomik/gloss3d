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
static GTK3G3DUIMAIN *g3tk_g3duimain_new ( ) {
    uint32_t memSize =  sizeof ( GTK3G3DUIMAIN );
    GTK3G3DUIMAIN *gtk3gmn = ( GTK3G3DUIMAIN * ) calloc ( 0x01, memSize );

    if ( gtk3gmn == NULL ) {
        fprint ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3gmn->grp.gui = ( G3DUI * ) gtk3_getUI ( );


    return gtk3gmn;
}

/******************************************************************************/
static void SizeAllocate ( GtkWidget     *widget,
                           GtkAllocation *allocation,
                           gpointer       user_data ) {
    GTK3G3DUIMAIN *gmn = ( GTK3G3DUIMAIN * ) user_data;
    GTK3G3DUI *gtk3gui = gtk3_getUI ( );
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    GdkRectangle gdkrec;

    g3duimain_sizeAllocate ( &gmn->grp, 
                              allocation->width, 
                              allocation->height );

    gdkrec.x      = gmn->grp.tbarrec.x;
    gdkrec.y      = gmn->grp.tbarrec.y;
    gdkrec.width  = gmn->grp.tbarrec.width;
    gdkrec.height = gmn->grp.tbarrec.height;

    gtk_layout_move ( widget, 
                      gmn->toolBar->bar,
                      gdkrec.x,
                      gdkrec.y );

    gtk_widget_size_allocate ( gmn->toolBar->bar, &gdkrec );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMAIN *gtk3gmn = ( GTK3G3DUIMAIN * ) user_data;
    GTK3G3DUI *gtk3gui = gtk3_getUI ( );
    G3DUI *gui = ( G3DUI * ) gtk3gui;

    free ( gtk3gmn );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMAIN *gtk3gmn = ( GTK3G3DUIMAIN * ) user_data;
    GTK3G3DUI *gtk3gui = gtk3_getUI ( );
    G3DUI *gui = ( G3DUI * ) gtk3gui;
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

    gtk3_initDefaultMouseTools ( gtk3_getMainViewCamera ( ) );

    gtk3_updateMenuBar ( );
}

/******************************************************************************/
GtkWidget *gtk3_g3duimain_create ( GtkWidget *parent,
                                   char      *name,
                                   gint       x,
                                   gint       y,
                                   gint       width,
                                   gint       height,
                                   char      *filename ) {
    GTK3G3DUIMAIN *gtk3gmn = g3tk_g3duimain_new ( );
    GdkRectangle  gdkrec  = { 0x00,  0x00, width, height };
    GtkWidget    *layout  = gtk_layout_new ( NULL, NULL );

    g_object_set_data ( G_OBJECT(layout), "private", (gpointer) gtk3gmn );

    gtk_widget_set_name ( layout, name );

    gtk_widget_size_allocate ( layout, &gdkrec );

    gtk_container_add ( GTK_CONTAINER(parent), layout );

    gtk_widget_add_events(GTK_WIDGET(layout), GDK_CONFIGURE);

    g_signal_connect ( G_OBJECT (layout), "realize"      , G_CALLBACK (Realize)     , gtk3gmn );
    g_signal_connect ( G_OBJECT (layout), "destroy"      , G_CALLBACK (Destroy)     , gtk3gmn );
    g_signal_connect ( G_OBJECT (layout), "size-allocate", G_CALLBACK (SizeAllocate), gtk3gmn );

    gtk3gmn->layout = layout;

    gtk3gmn->toolBar = gtk3_g3duitoolbar_create ( layout,
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


    return layout;
}

