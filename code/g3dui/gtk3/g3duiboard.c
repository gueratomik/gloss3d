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
#include <xpm/translate_view.xpm>
#include <xpm/rotate_view.xpm>
#include <xpm/maximize_view.xpm>
#include <xpm/zoom_view.xpm>

/******************************************************************************/
static GTK3G3DUIBOARD *gtk3_g3duiboard_new ( GTK3G3DUI *gtk3gui ) {
    uint32_t memSize =  sizeof ( GTK3G3DUIBOARD );
    GTK3G3DUIBOARD *gtk3board = ( GTK3G3DUIBOARD * ) calloc ( 0x01, memSize );

    if ( gtk3board == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;

    }

    gtk3board->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3board;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, 
                      gpointer   user_data ) {
    GTK3G3DUIBOARD *gtk3board = ( GTK3G3DUIBOARD * ) user_data;

    free ( gtk3board );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, 
                      gpointer   user_data ) {
    GTK3G3DUIBOARD *gtk3brd = ( GTK3G3DUIBOARD * ) user_data;
}

/******************************************************************************/
static void gtk3_g3duiboard_createToolBook ( GTK3G3DUIBOARD *gtk3board ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3board->core.gui;
    GtkNotebook  *toolbook = ui_gtk_notebook_new ( CLASS_MAIN );
    GtkWidget *moulab = ui_gtk_label_new ( CLASS_MAIN, "Mouse Tool" );
    GtkWidget *coordslab = ui_gtk_label_new ( CLASS_MAIN, "Coordinates" );
    GTK3G3DUIMOUSETOOLEDIT *gtk3mtledit;
    GTK3G3DUICOORDINATESEDIT *gtk3coordsedit;

    gtk_layout_put ( GTK_LAYOUT(gtk3board->layout), toolbook, 0, 0 );

    gtk3board->toolbook = toolbook;

    /*************** Coordinates *****************/
    gtk3coordsedit = gtk3_g3duicoordinatesedit_create ( toolbook, 
                                                        gtk3gui,
                                                        "Coordinates" );

    gtk_notebook_append_page ( GTK_NOTEBOOK(toolbook), gtk3coordsedit->fixed, coordslab );

    gtk3board->core.coordsedit = gtk3coordsedit;


    /**************** Mouse tool **************/
    gtk3mtledit = gtk3_g3duimousetooledit_create ( toolbook, 
                                                   gtk3gui,
                                                   "Mouse tools" );

    gtk_notebook_append_page ( GTK_NOTEBOOK(toolbook), gtk3mtledit->scrolled, moulab );

    gtk3board->core.mtledit = gtk3mtledit;



    gtk_widget_show ( toolbook );
}

/******************************************************************************/
static void gtk3_g3duiboard_createBoards ( GTK3G3DUIBOARD *gtk3board ) {
    GtkWidget *notebook = ui_gtk_notebook_new ( CLASS_MAIN );
    GtkWidget *objlab = ui_gtk_label_new ( CLASS_MAIN, "Objects" );
    GtkWidget *matlab = ui_gtk_label_new ( CLASS_MAIN, "Materials" );
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3board->core.gui;
    GTK3G3DUIMATERIALBOARD *gtk3matboard;
    GTK3G3DUIOBJECTBOARD *gtk3objboard;

    gtk_layout_put ( GTK_LAYOUT(gtk3board->layout), notebook, 0, 0 );

    gtk3board->notebook = notebook;

    /**************** Object board **************/
    gtk3objboard = gtk3_g3duiobjectboard_create ( notebook,
                                                  gtk3gui,
                                                  "Objects" );

    gtk_notebook_append_page ( GTK_NOTEBOOK(notebook), gtk3objboard->layout, objlab );

    gtk3board->core.objboard = ( G3DUIOBJECTBOARD * ) gtk3objboard;

    /**************** Material board **************/

    gtk3matboard = gtk3_g3duimaterialboard_create ( notebook,
                                                    gtk3gui,
                                                    "Material" );

    gtk_notebook_append_page ( GTK_NOTEBOOK(notebook), gtk3matboard->layout, matlab );

    gtk3board->core.matboard = ( G3DUIMATERIALBOARD * ) gtk3matboard;


    gtk_widget_show ( notebook );
}

/******************************************************************************/
void gtk3_g3duiboard_resize ( GTK3G3DUIBOARD *gtk3board,
                              uint32_t        width,
                              uint32_t        height ) {
    GdkRectangle gdkrec;

    g3duiboard_resize ( &gtk3board->core,
                         width,
                         height );

    /*** Boards ***/

    g3duirectangle_toGdkRectangle ( &gtk3board->core.boardrec, &gdkrec );

    gtk_layout_move ( gtk3board->layout,
                      gtk3board->notebook,
                      gdkrec.x,
                      gdkrec.y );

    gtk_widget_set_size_request ( gtk3board->notebook,
                                  gdkrec.width,
                                  gdkrec.height );

    gtk3_g3duiobjectboard_resize ( gtk3board->core.objboard,
                                   gdkrec.width,
                                   gdkrec.height );

    gtk3_g3duimaterialboard_resize ( gtk3board->core.matboard,
                                     gdkrec.width,
                                     gdkrec.height );

    /*******************************/

    g3duirectangle_toGdkRectangle ( &gtk3board->core.toolrec, &gdkrec );

    gtk_layout_move ( gtk3board->layout,
                      gtk3board->toolbook,
                      gdkrec.x,
                      gdkrec.y );

    gtk_widget_set_size_request ( gtk3board->toolbook,
                                  gdkrec.width,
                                  gdkrec.height );
}


/******************************************************************************/
GTK3G3DUIBOARD *gtk3_g3duiboard_create ( GtkWidget *parent,
                                         GTK3G3DUI *gtk3gui,
                                         char      *name ) {
    GTK3G3DUIBOARD *gtk3board = gtk3_g3duiboard_new ( gtk3gui );
    GtkWidget    *layout = ui_gtk_layout_new ( CLASS_MAIN, NULL, NULL );

    g_signal_connect ( G_OBJECT (layout), "realize"      , G_CALLBACK (Realize)     , gtk3board );
    g_signal_connect ( G_OBJECT (layout), "destroy"      , G_CALLBACK (Destroy)     , gtk3board );

    gtk_widget_show ( layout );

    gtk3board->layout = layout;

    gtk3_g3duiboard_createBoards ( gtk3board );
    gtk3_g3duiboard_createToolBook  ( gtk3board );


    return gtk3board;
}




