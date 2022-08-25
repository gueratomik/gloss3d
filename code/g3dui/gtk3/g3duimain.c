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
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

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
    GTK3G3DUIMENU    *gtk3menuBar = ( GTK3G3DUIMENU    * ) gtk3main->core.menuBar;
    GTK3G3DUIMODEBAR *gtk3modeBar = ( GTK3G3DUIMODEBAR * ) gtk3main->core.modeBar;
    GTK3G3DUIQUAD    *gtk3quad    = ( GTK3G3DUIQUAD    * ) gtk3main->core.quad;

    GdkRectangle gdkrec;

    g3duimain_sizeAllocate ( &gtk3main->core, 
                              allocation->width, 
                              allocation->height );

    if ( gtk3toolBar ) {
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

    if ( gtk3menuBar ) {
        gdkrec.x      = gtk3main->core.menurec.x;
        gdkrec.y      = gtk3main->core.menurec.y;
        gdkrec.width  = gtk3main->core.menurec.width;
        gdkrec.height = gtk3main->core.menurec.height;

        gtk_layout_move ( widget, 
                          gtk3menuBar->menu,
                          gdkrec.x,
                          gdkrec.y );

        gtk_widget_size_allocate ( gtk3menuBar->menu, &gdkrec );
    }

    if ( gtk3modeBar ) {
        gdkrec.x      = gtk3main->core.mbarrec.x;
        gdkrec.y      = gtk3main->core.mbarrec.y;
        gdkrec.width  = gtk3main->core.mbarrec.width;
        gdkrec.height = gtk3main->core.mbarrec.height;

        gtk_layout_move ( widget, 
                          gtk3modeBar->bar,
                          gdkrec.x,
                          gdkrec.y );

        gtk_widget_size_allocate ( gtk3modeBar->bar, &gdkrec );
    }

    if ( gtk3quad ) {
        gdkrec.x      = gtk3main->core.quadrec.x;
        gdkrec.y      = gtk3main->core.quadrec.y;
        gdkrec.width  = gtk3main->core.quadrec.width;
        gdkrec.height = gtk3main->core.quadrec.height;

        gtk_layout_move ( widget, 
                          gtk3quad->layout,
                          gdkrec.x,
                          gdkrec.y );

        gtk_widget_size_allocate ( gtk3quad->layout, &gdkrec );
    }
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
    gchar *myCSS = gtk3_getDefaultCSS ( );
    GdkDisplay *display = gdk_display_get_default ( );
    GdkScreen  *screen  = gdk_display_get_default_screen ( display );
    GtkWidget *tab;

    gtk_style_context_add_provider_for_screen ( screen,
                                 GTK_STYLE_PROVIDER (provider),
                                 GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    gtk_css_provider_load_from_data ( provider, myCSS, -1, NULL );

    g_object_unref ( provider );
/*
    g3dui_init ( gui );

    gtk3_initDefaultMouseTools ( gtk3gui );

    gtk3_g3duimain_updateMenuBar ( gtk3main );
*/
}

/******************************************************************************/
void gtk3_g3duimain_updateMenuBar ( GTK3G3DUIMAIN *gtk3main ) {
    gtk3_g3duimenu_update_r ( gtk3main->core.menuBar );
}

/******************************************************************************/
static void gtk3_g3duimain_createMenuBar ( GTK3G3DUIMAIN *gtk3main ) {
    GTK3G3DUIMENU *gtk3menu;

    gtk3menu = gtk3_g3duimenu_parse_r ( g3duimenu_getMainMenuNode ( ),
                                        gtk3main->core.gui,
                                        gtk3main );

    gtk_layout_put ( GTK_LAYOUT(gtk3main->layout), gtk3menu->menu, 0, 0 );

    gtk_widget_show ( gtk3menu->menu );


    gtk3main->core.menuBar = ( G3DUIMENU * ) gtk3menu;
}

/******************************************************************************/
static void gtk3_g3duimain_createToolBar ( GTK3G3DUIMAIN *gtk3main ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3main->core.gui;

    GTK3G3DUITOOLBAR *gtk3tb = gtk3_g3duitoolbar_create ( gtk3main->layout,
                                                          gtk3gui,
                                                          "toolbar" );

    gtk_layout_put ( GTK_LAYOUT(gtk3main->layout), gtk3tb->bar, 0, 0 );

    gtk_widget_show ( gtk3tb->bar );


    gtk3main->core.toolBar = ( G3DUITOOLBAR * ) gtk3tb;
}

/******************************************************************************/
static void gtk3_g3duimain_createModeBar ( GTK3G3DUIMAIN *gtk3main ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3main->core.gui;

    GTK3G3DUIMODEBAR *gtk3mb = gtk3_g3duimodebar_create ( gtk3main->layout,
                                                          gtk3gui,
                                                          "modebar" );

    gtk_layout_put ( GTK_LAYOUT(gtk3main->layout), gtk3mb->bar, 0, 0 );

    gtk_widget_show ( gtk3mb->bar );


    gtk3main->core.modeBar = ( G3DUIMODEBAR * ) gtk3mb;
}

/******************************************************************************/
static void gtk3_g3duimain_createQuad ( GTK3G3DUIMAIN *gtk3main ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3main->core.gui;
    GTK3G3DUIQUAD *gtk3quad = gtk3_g3duiquad_create ( gtk3main->layout,
                                                      gtk3gui,
                                                      "quad" );

    gtk_layout_put ( GTK_LAYOUT(gtk3main->layout), gtk3quad->layout, 0, 0 );

    gtk_widget_show ( gtk3quad->layout );


    gtk3main->core.quad = ( G3DUIQUAD * ) gtk3quad;
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

    /*gtk_widget_add_events(GTK_WIDGET(layout), GDK_CONFIGURE);*/

    g_signal_connect ( G_OBJECT (layout), "realize"      , G_CALLBACK (Realize)     , gtk3main );
    g_signal_connect ( G_OBJECT (layout), "destroy"      , G_CALLBACK (Destroy)     , gtk3main );
    g_signal_connect ( G_OBJECT (layout), "size-allocate", G_CALLBACK (SizeAllocate), gtk3main );

    gtk3main->layout = layout;

    gtk3_g3duimain_createMenuBar ( gtk3main );
    gtk3_g3duimain_createToolBar ( gtk3main );
    gtk3_g3duimain_createModeBar ( gtk3main );
    gtk3_g3duimain_createQuad    ( gtk3main );


    gtk_widget_show ( layout );


    /*** File loading must be done AFTER OpenGL init ***/
/*
    if ( filename ) {
        gtk3_interpretUIReturnFlags ( g3dui_openG3DFile ( gui, filename ) );
    } else {
        gui->sce = g3dscene_new  ( 0x00, "Gloss3D scene" );
    }
*/

    return gtk3main;
}

