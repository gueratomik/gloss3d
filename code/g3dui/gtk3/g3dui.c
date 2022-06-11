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
typedef struct _G3DUIMAIN {
    G3DUIWIDGETGROUP grp;

    GtkWidget       *main;
    GtkWidget       *toolBar;
    GtkWidget       *quad;
    GtkWidget       *timeBoard;
    GtkWidget       *board;
} G3DUIMAIN;

/******************************************************************************/
static G3DUIMAIN *g3duimain_new ( G3DUI *gui ) {
    G3DUIMAIN *mui = calloc ( 0x01, sizeof ( G3DUIMAIN ) );

    if ( mui == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );
    }

    mui->grp.gui = gui;

    return mui; 
}

/******************************************************************************/
static void SizeAllocate ( GtkWidget     *widget,
                           GtkAllocation *allocation,
                           gpointer       user_data ) {
    G3DUIMAIN *mui = ( G3DUIMAIN * ) user_data;
    static int x = 0, y = 0;


    gtk_layout_move ( widget, mui->toolBar, x++, y++ );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    G3DUIMAIN *mui = ( G3DUIMAIN * ) user_data;

    free ( mui );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUIMAIN *mui = ( G3DUIMAIN * ) user_data;

    /*updateInstanceEdit ( widget, NULL );*/
}

/******************************************************************************/
GtkWidget *gtk3_g3dui_createMain ( GtkWidget *parent,
                                   G3DUI     *gui,
                                   char      *name,
                                   gint       x,
                                   gint       y,
                                   gint       width,
                                   gint       height ) {
    GdkRectangle gdkrec = { 0x00, 0x00, width, height };
    GtkWidget *frm = gtk_layout_new ( NULL, NULL );
    G3DUIMAIN *mui = g3duimain_new ( gui );

    g_object_set_data ( G_OBJECT(frm), "private", (gpointer) mui );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &gdkrec );

    /*gtk_fixed_put ( GTK_FIXED(parent), frm, x, y );*/

    gtk_container_add ( GTK_CONTAINER(parent), frm );

    gtk_widget_add_events(GTK_WIDGET(frm), GDK_CONFIGURE);

    g_signal_connect ( G_OBJECT (frm), "realize"      , G_CALLBACK (Realize)     , mui );
    g_signal_connect ( G_OBJECT (frm), "destroy"      , G_CALLBACK (Destroy)     , mui );
    g_signal_connect ( G_OBJECT (frm), "size-allocate", G_CALLBACK (SizeAllocate), mui );

    mui->main = frm;

    mui->toolBar = g3dui_createToolBar ( frm, 
                                         gui,
                                         "toolbar",
                                         0,
                                         0,
                                         width,
                                         32 );


    gtk_widget_show ( frm );

    return frm;
}

