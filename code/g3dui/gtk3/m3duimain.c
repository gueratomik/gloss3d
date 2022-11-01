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
static GTK3M3DUIMAIN *g3tk_m3duimain_new ( GTK3M3DUI *gtk3mui ) {
    uint32_t memSize =  sizeof ( GTK3M3DUIMAIN );
    GTK3M3DUIMAIN *gtk3main = ( GTK3M3DUIMAIN * ) calloc ( 0x01, memSize );

    if ( gtk3main == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3main->core.mui = ( M3DUI * ) gtk3mui;


    return gtk3main;
}

/******************************************************************************/
void gtk3_m3duimain_updateMenuBar ( GTK3M3DUIMAIN *gtk3main ) {
    gtk3_g3duimenu_update_r ( gtk3main->core.menuBar );
}

/******************************************************************************/
static void gtk3_m3duimain_createMenuBar ( GTK3M3DUIMAIN *gtk3main ) {
    GTK3G3DUIMENU *gtk3menu;

    gtk3menu = gtk3_g3duimenu_parse_r ( g3duimenu_getUVEditorMenuNode ( ),
                                        gtk3main->core.mui->gui,
                                        gtk3main );

    gtk_layout_put ( GTK_LAYOUT(gtk3main->layout), gtk3menu->menu, 0, 0 );


    gtk3main->core.menuBar = ( G3DUIMENU * ) gtk3menu;
}

/******************************************************************************/
static void gtk3_m3duimain_createToolBar ( GTK3M3DUIMAIN *gtk3main ) {
    GTK3M3DUI *gtk3mui = ( GTK3M3DUI * ) gtk3main->core.mui;

    GTK3M3DUITOOLBAR *gtk3tb = gtk3_m3duitoolbar_create ( gtk3main->layout,
                                                          gtk3mui,
                                                          "toolbar" );

    gtk_layout_put ( GTK_LAYOUT(gtk3main->layout), gtk3tb->bar, 0, 0 );


    gtk3main->core.toolBar = ( G3DUITOOLBAR * ) gtk3tb;
}

/******************************************************************************/
static void gtk3_m3duimain_createModeBar ( GTK3M3DUIMAIN *gtk3main ) {
    GTK3M3DUI *gtk3mui = ( GTK3M3DUI * ) gtk3main->core.mui;

    GTK3M3DUIMODEBAR *gtk3mb = gtk3_m3duimodebar_create ( gtk3main->layout,
                                                          gtk3mui,
                                                          "modebar" );

    gtk_layout_put ( GTK_LAYOUT(gtk3main->layout), gtk3mb->bar, 0, 0 );


    gtk3main->core.modeBar = ( G3DUIMODEBAR * ) gtk3mb;
}

/******************************************************************************/
static void gtk3_m3duimain_createView ( GTK3M3DUIMAIN *gtk3main ) {
    GTK3M3DUI *gtk3mui = ( GTK3M3DUI * ) gtk3main->core.mui;
    GTK3M3DUIVIEW *gtk3view = gtk3_m3duiview_create ( gtk3main->layout,
                                                      gtk3mui,
                                                      "view" );

    gtk_layout_put ( GTK_LAYOUT(gtk3main->layout), gtk3view->layout, 0, 0 );

    gtk3main->core.view = ( M3DUIVIEW * ) gtk3view;
}

/******************************************************************************/
static void gtk3_m3duimain_createBoard ( GTK3M3DUIMAIN *gtk3main ) {
    GTK3M3DUI *gtk3mui = ( GTK3M3DUI * ) gtk3main->core.mui;
    GTK3M3DUIBOARD *gtk3board = gtk3_m3duiboard_create ( gtk3main->layout,
                                                         gtk3mui,
                                                         "Board" );

    gtk_layout_put ( GTK_LAYOUT(gtk3main->layout), gtk3board->layout, 0, 0 );

    gtk3main->core.board = ( GTK3M3DUIBOARD * ) gtk3board;
}

/******************************************************************************/

void gtk3_m3duimain_resize ( GTK3M3DUIMAIN *gtk3main,
                             uint32_t       width,
                             uint32_t       height ) {
    GTK3M3DUIVIEW      *gtk3view      = ( GTK3M3DUIVIEW      * ) gtk3main->core.view;
    GTK3G3DUIMENU      *gtk3menuBar   = ( GTK3G3DUIMENU      * ) gtk3main->core.menuBar;
    GTK3M3DUITOOLBAR   *gtk3toolBar   = ( GTK3M3DUITOOLBAR   * ) gtk3main->core.toolBar;
    GTK3M3DUIMODEBAR   *gtk3modeBar   = ( GTK3M3DUIMODEBAR   * ) gtk3main->core.modeBar;
    GTK3M3DUIBOARD     *gtk3board     = ( GTK3M3DUIBOARD     * ) gtk3main->core.board;

    GdkRectangle gdkrec;

    m3duimain_resize ( &gtk3main->core, 
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


    if ( gtk3view ) {
        g3duirectangle_toGdkRectangle ( &gtk3main->core.viewrec, &gdkrec );

        gtk3_m3duiview_resize ( &gtk3view->core,
                                 gdkrec.width,
                                 gdkrec.height );

        gtk_layout_move ( gtk3main->layout,
                          gtk3view->layout, 
                          gdkrec.x,
                          gdkrec.y );

        gtk_widget_set_size_request ( gtk3view->layout,
                                      gdkrec.width,
                                      gdkrec.height );
    }

    if ( gtk3board ) {
        g3duirectangle_toGdkRectangle ( &gtk3main->core.mbrdrec, &gdkrec );

        gtk3_m3duiboard_resize ( &gtk3board->core,
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
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, 
                      gpointer   user_data ) {
    GTK3M3DUIMAIN *gtk3main = ( GTK3M3DUIMAIN * ) user_data;
    M3DUI *mui = gtk3main->core.mui;
    GTK3M3DUI *gtk3mui = ( GTK3M3DUI * ) mui;
    
    mui->main = NULL;

    free ( gtk3main );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, 
                      gpointer   user_data ) {
    GTK3M3DUIMAIN *gtk3main = ( GTK3M3DUIMAIN * ) user_data;
    M3DUI *mui = gtk3main->core.mui;
    GTK3M3DUI *gtk3mui = ( GTK3M3DUI * ) mui;

}

/******************************************************************************/
GTK3M3DUIMAIN *gtk3_m3duimain_create ( GtkWidget *parent,
                                       GTK3M3DUI *gtk3mui,
                                       char      *name ) {
    GTK3M3DUIMAIN *gtk3main = g3tk_m3duimain_new ( gtk3mui );
    GtkWidget    *layout  = ui_gtk_layout_new ( CLASS_MAIN, NULL, NULL );

    gtk_widget_set_name ( layout, name );

    gtk_widget_add_events(GTK_WIDGET(layout), GDK_CONFIGURE);

    g_signal_connect ( G_OBJECT (layout), "realize", G_CALLBACK (Realize), gtk3main );
    g_signal_connect ( G_OBJECT (layout), "destroy", G_CALLBACK (Destroy), gtk3main );

    gtk3main->layout = layout;

    gtk3_m3duimain_createMenuBar   ( gtk3main );
    gtk3_m3duimain_createToolBar   ( gtk3main );
    gtk3_m3duimain_createModeBar   ( gtk3main );
    gtk3_m3duimain_createView      ( gtk3main );
    gtk3_m3duimain_createBoard     ( gtk3main );

    gtk_widget_show ( layout );


    return gtk3main;
}

