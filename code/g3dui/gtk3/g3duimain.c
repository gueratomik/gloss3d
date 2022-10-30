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


    gtk3main->core.menuBar = ( G3DUIMENU * ) gtk3menu;
}

/******************************************************************************/
static void gtk3_g3duimain_createToolBar ( GTK3G3DUIMAIN *gtk3main ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3main->core.gui;

    GTK3G3DUITOOLBAR *gtk3tb = gtk3_g3duitoolbar_create ( gtk3main->layout,
                                                          gtk3gui,
                                                          "toolbar" );

    gtk_layout_put ( GTK_LAYOUT(gtk3main->layout), gtk3tb->bar, 0, 0 );


    gtk3main->core.toolBar = ( G3DUITOOLBAR * ) gtk3tb;
}

/******************************************************************************/
static void gtk3_g3duimain_createModeBar ( GTK3G3DUIMAIN *gtk3main ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3main->core.gui;

    GTK3G3DUIMODEBAR *gtk3mb = gtk3_g3duimodebar_create ( gtk3main->layout,
                                                          gtk3gui,
                                                          "modebar" );

    gtk_layout_put ( GTK_LAYOUT(gtk3main->layout), gtk3mb->bar, 0, 0 );


    gtk3main->core.modeBar = ( G3DUIMODEBAR * ) gtk3mb;
}

/******************************************************************************/
static void gtk3_g3duimain_createQuad ( GTK3G3DUIMAIN *gtk3main ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3main->core.gui;
    GTK3G3DUIQUAD *gtk3quad = gtk3_g3duiquad_create ( gtk3main->layout,
                                                      gtk3gui,
                                                      "quad" );

    gtk_layout_put ( GTK_LAYOUT(gtk3main->layout), gtk3quad->layout, 0, 0 );

    gtk3main->core.quad = ( G3DUIQUAD * ) gtk3quad;
}

/******************************************************************************/
static void gtk3_g3duimain_createBoard ( GTK3G3DUIMAIN *gtk3main ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3main->core.gui;
    GTK3G3DUIBOARD *gtk3board = gtk3_g3duiboard_create ( gtk3main->layout,
                                                         gtk3gui,
                                                         "quad" );

    gtk_layout_put ( GTK_LAYOUT(gtk3main->layout), gtk3board->layout, 0, 0 );

    gtk3main->core.board = ( GTK3G3DUIBOARD * ) gtk3board;
}

/******************************************************************************/
static void gtk3_g3duimain_createTimeboard ( GTK3G3DUIMAIN *gtk3main ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3main->core.gui;
    GTK3G3DUITIMEBOARD *gtk3timeboard = gtk3_g3duitimeboard_create ( gtk3main->layout,
                                                                     gtk3gui,
                                                                     "timeboard" );

    gtk_layout_put ( GTK_LAYOUT(gtk3main->layout), gtk3timeboard->layout, 0, 0 );

    gtk3main->core.timeboard = ( GTK3G3DUITIMEBOARD * ) gtk3timeboard;
}

/******************************************************************************/
void gtk3_g3duimain_resize ( GTK3G3DUIMAIN *gtk3main,
                             uint32_t       width,
                             uint32_t       height ) {
    GTK3G3DUIQUAD      *gtk3quad      = ( GTK3G3DUIQUAD      * ) gtk3main->core.quad;
    GTK3G3DUIMENU      *gtk3menuBar   = ( GTK3G3DUIMENU      * ) gtk3main->core.menuBar;
    GTK3G3DUITOOLBAR   *gtk3toolBar   = ( GTK3G3DUITOOLBAR   * ) gtk3main->core.toolBar;
    GTK3G3DUIMODEBAR   *gtk3modeBar   = ( GTK3G3DUIMODEBAR   * ) gtk3main->core.modeBar;
    GTK3G3DUIBOARD     *gtk3board     = ( GTK3G3DUIBOARD     * ) gtk3main->core.board;
    GTK3G3DUITIMEBOARD *gtk3timeboard = ( GTK3G3DUITIMEBOARD * ) gtk3main->core.timeboard;

    GdkRectangle gdkrec;

    g3duimain_resize ( &gtk3main->core, 
                        width, 
                        height );

    if ( gtk3toolBar ) {
        g3duirectangle_toGdkRectangle ( &gtk3main->core.tbarrec, &gdkrec );

        gtk_layout_move ( gtk3main->layout,
                         gtk3toolBar->bar, 
                         gdkrec.x,
                         gdkrec.y );

        gtk_widget_set_size_request ( gtk3toolBar->bar,
                                      gdkrec.width,
                                      gdkrec.height );
    }

    if ( gtk3menuBar ) {
        g3duirectangle_toGdkRectangle ( &gtk3main->core.menurec, &gdkrec );

        gtk_layout_move ( gtk3main->layout,
                         gtk3menuBar->menu, 
                         gdkrec.x,
                         gdkrec.y );

        gtk_widget_set_size_request ( gtk3menuBar->menu,
                                      gdkrec.width,
                                      gdkrec.height );
    }

    if ( gtk3modeBar ) {
        g3duirectangle_toGdkRectangle ( &gtk3main->core.mbarrec, &gdkrec );

        gtk_layout_move ( gtk3main->layout,
                         gtk3modeBar->bar, 
                         gdkrec.x,
                         gdkrec.y );

        gtk_widget_set_size_request ( gtk3modeBar->bar,
                                      gdkrec.width,
                                      gdkrec.height );
    }

    if ( gtk3quad ) {
        g3duirectangle_toGdkRectangle ( &gtk3main->core.quadrec, &gdkrec );

        gtk3_g3duiquad_resize ( gtk3quad, gdkrec.width, gdkrec.height );

        gtk_layout_move ( gtk3main->layout,
                         gtk3quad->layout, 
                         gdkrec.x,
                         gdkrec.y );

        gtk_widget_set_size_request ( gtk3quad->layout,
                                      gdkrec.width,
                                      gdkrec.height );
    }

    if ( gtk3board ) {
        g3duirectangle_toGdkRectangle ( &gtk3main->core.mbrdrec, &gdkrec );

        gtk3_g3duiboard_resize ( gtk3board,
                                 gdkrec.width,
                                 gdkrec.height );

        gtk_layout_move ( gtk3main->layout,
                          gtk3board->layout, 
                          gdkrec.x,
                          gdkrec.y );

        gtk_widget_set_size_request ( gtk3board->layout,
                                      gdkrec.width,
                                      gdkrec.height );
    }

    if ( gtk3timeboard ) {
        g3duirectangle_toGdkRectangle ( &gtk3main->core.timerec, &gdkrec );

        gtk3_g3duitimeboard_resize ( gtk3timeboard,
                                     gdkrec.width,
                                     gdkrec.height );

        gtk_layout_move ( gtk3main->layout,
                          gtk3timeboard->layout,
                          gdkrec.x,
                          gdkrec.y );

        gtk_widget_set_size_request ( gtk3timeboard->layout,
                                      gdkrec.width,
                                      gdkrec.height );
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
}

/******************************************************************************/
static void Map ( GtkWidget *widget, 
                  gpointer   user_data ) {
    GTK3G3DUIMAIN *gtk3main = ( GTK3G3DUIMAIN * ) user_data;
    G3DUI *gui = gtk3main->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;

    /*** things that must be called after OpenGL initializing, this is why ***/
    /*** we use the Map signa. It comes after all widgets were realized ***/
    if ( gui->inited == 0x00 ) {
        g3dui_init ( gui, ( G3DUIMAIN * ) gtk3main );

        gui->toframe = q3dfilter_gotoframe_new ( gtk3gui );

        /**** signal handling for dumping pixels or playing animation ***/
        /*** because these are threaded actions and OpenGL does not ***/
        /*** work in threaded processes ****/
        g_signal_new( "action", G_TYPE_OBJECT, 
                                   G_SIGNAL_RUN_FIRST,
                                   0,
                                   NULL, NULL,
                                   g_cclosure_marshal_VOID__POINTER,
                                   G_TYPE_NONE, 1, G_TYPE_POINTER );

        g_signal_connect ( G_OBJECT(gtk3gui->topWin), 
                           "action",
                           G_CALLBACK ( gtk3_g3duicom_handleAction ), gui );

        gtk3_initDefaultMouseTools ( gtk3gui );

        gtk3_interpretUIReturnFlags ( gtk3gui, UPDATEANDREDRAWALL );
    }
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
    GtkWidget    *layout  = ui_gtk_layout_new ( CLASS_MAIN, NULL, NULL );

    gtk_widget_set_name ( layout, name );

    gtk_widget_add_events(GTK_WIDGET(layout), GDK_CONFIGURE);

    g_signal_connect ( G_OBJECT (layout), "realize"      , G_CALLBACK (Realize)     , gtk3main );
    g_signal_connect ( G_OBJECT (layout), "destroy"      , G_CALLBACK (Destroy)     , gtk3main );
    g_signal_connect ( G_OBJECT (layout), "map"          , G_CALLBACK (Map         ), gtk3main );

    gtk3main->layout = layout;

    gtk3_g3duimain_createMenuBar   ( gtk3main );
    gtk3_g3duimain_createToolBar   ( gtk3main );
    gtk3_g3duimain_createModeBar   ( gtk3main );
    gtk3_g3duimain_createQuad      ( gtk3main );
    gtk3_g3duimain_createBoard     ( gtk3main );
    gtk3_g3duimain_createTimeboard ( gtk3main );

    gtk_widget_show ( layout );

    /*gui->filename = filename;*/

    return gtk3main;
}

