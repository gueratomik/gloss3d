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
static GTK3G3DUIOBJECTBOARD *gtk3_g3duiobjectboard_new ( GTK3G3DUI *gtk3gui ) {
    uint32_t memSize =  sizeof ( GTK3G3DUIOBJECTBOARD );
    GTK3G3DUIOBJECTBOARD *gtk3objbrd = ( GTK3G3DUIOBJECTBOARD * ) calloc ( 0x01, memSize );

    if ( gtk3objbrd == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;

    }

    gtk3objbrd->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3objbrd;
}

/******************************************************************************/
static void SizeAllocate ( GtkWidget     *widget,
                           GtkAllocation *allocation,
                           gpointer       user_data ) {
    GTK3G3DUIOBJECTBOARD *gtk3objboard = ( GTK3G3DUIOBJECTBOARD * ) user_data;
    GTK3G3DUIMENU *gtk3menu = ( GTK3G3DUIMENU * ) gtk3objboard->core.menuBar;
    GdkRectangle gdkrec;
    GTK3G3DUIOBJECTLIST *objlist = ( GTK3G3DUIOBJECTLIST * ) gtk3objboard->core.objlist;
    GTK3G3DUIOBJECTEDIT *objedit = ( GTK3G3DUIOBJECTEDIT * ) gtk3objboard->core.objedit;

    g3duiobjectboard_resize ( &gtk3objboard->core,
                               allocation->width,
                               allocation->height );

    /*** Menu ***/

    g3duirectangle_toGdkRectangle ( &gtk3objboard->core.menurec, &gdkrec );

gtk_layout_move ( gtk3objboard->layout, gtk3menu->menu, gdkrec.x, gdkrec.y );
gtk_widget_set_size_request ( gtk3menu->menu, gdkrec.width, gdkrec.height  );
    /*gtk_widget_size_allocate ( gtk3menu->menu, &gdkrec );*/


    /*** Object List ***/

    /*g3duirectangle_toGdkRectangle ( &gtk3objboard->core.listrec, &gdkrec );

    gtk_widget_size_allocate ( objlist->layout, &gdkrec );*/

    /*** Object Edit ***/

    g3duirectangle_toGdkRectangle ( &gtk3objboard->core.editrec, &gdkrec );

/*gtk_layout_move ( gtk3objboard->layout, objedit->scrolled, gdkrec.x, gdkrec.y );
gtk_widget_set_size_request ( objedit->scrolled, gdkrec.width, gdkrec.height  );*/
    /*gtk_widget_size_allocate ( objedit->scrolled, &gdkrec );*/
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIOBJECTBOARD *gtk3objboard = ( GTK3G3DUIOBJECTBOARD * ) user_data;

    free ( gtk3objboard );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, 
                      gpointer   user_data ) {
    GTK3G3DUIOBJECTBOARD *gtk3objboard = ( GTK3G3DUIOBJECTBOARD * ) user_data;


}

/******************************************************************************/
void gtk3_g3duiobjectboard_updateMenuBar ( GTK3G3DUIOBJECTBOARD *gtk3objbrd ) {
    gtk3_g3duimenu_update_r ( ( GTK3G3DUIMENU * ) gtk3objbrd->core.menuBar );
}

/******************************************************************************/
static void createObjectEdit ( GTK3G3DUIOBJECTBOARD *gtk3objboard ) {
    GTK3G3DUIOBJECTEDIT *gtk3objedit;

    gtk3objedit = gtk3_g3duiobjectedit_create ( gtk3objboard->layout,
                                ( GTK3G3DUI * ) gtk3objboard->core.gui,
                                                "Current object" );

    gtk_layout_put ( GTK_LAYOUT(gtk3objboard->layout), gtk3objedit->scrolled, 0, 0 );

    gtk3objboard->core.objedit = ( G3DUIOBJECTEDIT * ) gtk3objedit;
}

/******************************************************************************/
static void createMenuBar ( GTK3G3DUIOBJECTBOARD *gtk3objboard ) {
    GTK3G3DUIMENU *gtk3menu;

    gtk3menu = gtk3_g3duimenu_parse_r ( g3duimenu_getObjectBoardMenuNode ( ),
                        ( GTK3G3DUI * ) gtk3objboard->core.gui,
                                        gtk3objboard );

    gtk3_g3duimenu_update_r ( gtk3menu );

    gtk_layout_put ( GTK_LAYOUT(gtk3objboard->layout), gtk3menu->menu, 0, 0 );

    gtk_widget_show ( gtk3menu->menu );


    gtk3objboard->core.menuBar = ( G3DUIMENU * ) gtk3menu;
}

/******************************************************************************/
GTK3G3DUIOBJECTBOARD *gtk3_g3duiobjectboard_create ( GtkWidget *parent,
                                                     GTK3G3DUI *gtk3gui,
                                                     char      *name ) {
    GTK3G3DUIOBJECTBOARD *gtk3objboard = gtk3_g3duiobjectboard_new ( gtk3gui );
    GtkWidget    *layout = gtk_layout_new ( NULL, NULL );

    gtk_widget_set_name ( layout, name );

    /*gtk_widget_add_events(GTK_WIDGET(layout), GDK_CONFIGURE);*/

    g_signal_connect ( G_OBJECT (layout), "realize"      , G_CALLBACK (Realize)     , gtk3objboard );
    g_signal_connect ( G_OBJECT (layout), "destroy"      , G_CALLBACK (Destroy)     , gtk3objboard );
    g_signal_connect ( G_OBJECT (layout), "size-allocate", G_CALLBACK (SizeAllocate), gtk3objboard );

    gtk_widget_show ( layout );

    gtk3objboard->layout = layout;

    /*gtk3_g3duiobjectboard_createObjectList  ( gtk3objbrd );*/
    createMenuBar    ( gtk3objboard );
    createObjectEdit ( gtk3objboard );


    return gtk3objboard;
}
















