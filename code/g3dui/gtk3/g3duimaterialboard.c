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
static GTK3G3DUIMATERIALBOARD *gtk3_g3duimaterialboard_new ( GTK3G3DUI *gtk3gui ) {
    uint32_t memSize =  sizeof ( GTK3G3DUIMATERIALBOARD );
    GTK3G3DUIMATERIALBOARD *gtk3objbrd = ( GTK3G3DUIMATERIALBOARD * ) calloc ( 0x01, memSize );

    if ( gtk3objbrd == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;

    }

    gtk3objbrd->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3objbrd;
}

/******************************************************************************/
void gtk3_g3duimaterialboard_resize ( GTK3G3DUIMATERIALBOARD *gtk3matboard,
                                      uint32_t                width,
                                      uint32_t                height ) {
    GTK3G3DUIMENU *gtk3menu = ( GTK3G3DUIMENU * ) gtk3matboard->core.menuBar;
    GdkRectangle gdkrec;

    GTK3G3DUIMATERIALLIST *gtk3matlist = ( GTK3G3DUIMATERIALLIST * ) gtk3matboard->core.matlist;
    GTK3G3DUIMATERIALEDIT *gtk3matedit = ( GTK3G3DUIMATERIALEDIT * ) gtk3matboard->core.matedit;

    g3duimaterialboard_resize ( &gtk3matboard->core,
                                 width,
                                 height );

    /*** Menu ***/

    g3duirectangle_toGdkRectangle ( &gtk3matboard->core.menurec, &gdkrec );

    gtk_layout_move ( gtk3matboard->layout, gtk3menu->menu, gdkrec.x, gdkrec.y );
    gtk_widget_set_size_request ( gtk3menu->menu, gdkrec.width, gdkrec.height  );

    /*gtk_widget_size_allocate ( gtk3menu->menu, &gdkrec );*/


    /*** Material List ***/
    if ( gtk3matlist ) {
        g3duirectangle_toGdkRectangle ( &gtk3matboard->core.listrec, &gdkrec );

        gtk_layout_move ( gtk3matboard->layout,
                          gtk3matlist->scrolled,
                          gdkrec.x,
                          gdkrec.y );

        gtk_widget_set_size_request ( gtk3matlist->scrolled,
                                      gdkrec.width,
                                      gdkrec.height );

    }

    /*** Material Edit ***/

    if ( gtk3matedit ) {
        g3duirectangle_toGdkRectangle ( &gtk3matboard->core.editrec, &gdkrec );

        gtk_layout_move ( gtk3matboard->layout,
                          gtk3matedit->fixed,
                          gdkrec.x,
                          gdkrec.y );

        gtk3_g3duimaterialedit_resize ( gtk3matedit,
                                        gdkrec.width,
                                        gdkrec.height );

    }
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIMATERIALBOARD *gtk3matboard = ( GTK3G3DUIMATERIALBOARD * ) user_data;

    free ( gtk3matboard );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, 
                      gpointer   user_data ) {
    GTK3G3DUIMATERIALBOARD *gtk3matboard = ( GTK3G3DUIMATERIALBOARD * ) user_data;


}

/******************************************************************************/
void gtk3_g3duimaterialboard_updateMenuBar ( GTK3G3DUIMATERIALBOARD *gtk3matboard ) {
    gtk3_g3duimenu_update_r ( ( GTK3G3DUIMENU * ) gtk3matboard->core.menuBar );
}

/******************************************************************************/
static void createMaterialList ( GTK3G3DUIMATERIALBOARD *gtk3matboard ) {
    GTK3G3DUIMATERIALLIST *gtk3matlist;

    gtk3matlist = gtk3_g3duimateriallist_create ( gtk3matboard->layout,
                                  ( GTK3G3DUI * ) gtk3matboard->core.gui,
                                                  "Material list" );

    gtk_layout_put ( GTK_LAYOUT(gtk3matboard->layout), gtk3matlist->scrolled, 0, 0 );

    gtk3matboard->core.matlist = ( G3DUIMATERIALLIST * ) gtk3matlist;
}

/******************************************************************************/
static void createMaterialEdit ( GTK3G3DUIMATERIALBOARD *gtk3matboard ) {
    GTK3G3DUIMATERIALEDIT *gtk3matedit;

    gtk3matedit = gtk3_g3duimaterialedit_create ( gtk3matboard->layout,
                                  ( GTK3G3DUI * ) gtk3matboard->core.gui,
                                                  "Material edit" );

    gtk_layout_put ( GTK_LAYOUT(gtk3matboard->layout), gtk3matedit->fixed, 0, 0 );

    gtk3matboard->core.matedit = ( GTK3G3DUIMATERIALEDIT * ) gtk3matedit;
}

/******************************************************************************/
/*
static void createObjectEdit ( GTK3G3DUIMATERIALBOARD *gtk3matboard ) {
    GTK3G3DUIMATERIALEDIT *gtk3objedit;

    gtk3objedit = gtk3_g3duiobjectedit_create ( gtk3matboard->layout,
                                ( GTK3G3DUI * ) gtk3matboard->core.gui,
                                                "Current object" );

    gtk_layout_put ( GTK_LAYOUT(gtk3matboard->layout), gtk3objedit->scrolled, 0, 0 );

    gtk3matboard->core.objedit = ( G3DUIMATERIALEDIT * ) gtk3objedit;
}
*/
/******************************************************************************/
static void createMenuBar ( GTK3G3DUIMATERIALBOARD *gtk3matboard ) {
    GTK3G3DUIMENU *gtk3menu;

    gtk3menu = gtk3_g3duimenu_parse_r ( g3duimenu_getMaterialBoardMenuNode ( ),
                        ( GTK3G3DUI * ) gtk3matboard->core.gui,
                                        gtk3matboard );

    gtk3_g3duimenu_update_r ( gtk3menu );

    gtk_layout_put ( GTK_LAYOUT(gtk3matboard->layout), gtk3menu->menu, 0, 0 );

    gtk_widget_show ( gtk3menu->menu );


    gtk3matboard->core.menuBar = ( G3DUIMENU * ) gtk3menu;
}

/******************************************************************************/
GTK3G3DUIMATERIALBOARD *gtk3_g3duimaterialboard_create ( GtkWidget *parent,
                                                         GTK3G3DUI *gtk3gui,
                                                         char      *name ) {
    GTK3G3DUIMATERIALBOARD *gtk3matboard = gtk3_g3duimaterialboard_new ( gtk3gui );
    GtkWidget    *layout = ui_gtk_layout_new ( CLASS_MAIN, NULL, NULL );

    gtk_widget_set_name ( layout, name );

    /*gtk_widget_add_events(GTK_WIDGET(layout), GDK_CONFIGURE);*/

    g_signal_connect ( G_OBJECT (layout), "realize", G_CALLBACK (Realize), gtk3matboard );
    g_signal_connect ( G_OBJECT (layout), "destroy", G_CALLBACK (Destroy), gtk3matboard );

    gtk_widget_show ( layout );

    gtk3matboard->layout = layout;

    createMaterialList ( gtk3matboard );
    createMenuBar      ( gtk3matboard );
    createMaterialEdit ( gtk3matboard );


    return gtk3matboard;
}
















