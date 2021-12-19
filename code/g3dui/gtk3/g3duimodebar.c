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
#include <xpm/objectmode.xpm>
#include <xpm/objectmode_test.xpm>
#include <xpm/facemode.xpm>
#include <xpm/facemode_test.xpm>
#include <xpm/vertexmode.xpm>
#include <xpm/vertexmode_test.xpm>
#include <xpm/edgemode.xpm>
#include <xpm/edgemode_test.xpm>
#include <xpm/skinmode.xpm>
#include <xpm/skinmode_test.xpm>
#include <xpm/uvwmapmode.xpm>
#include <xpm/uvwmapmode_test.xpm>
#include <xpm/sculptmode.xpm>
#include <xpm/axismode.xpm>
#include <xpm/axismode_test.xpm>
#include <xpm/pathmode.xpm>

/******************************************************************************/
void l3dui_setUVMode ( GtkWidget *widget, gpointer user_data ) {
    const char *modename = gtk_widget_get_name ( widget );
    L3DUI *lui = ( L3DUI * ) user_data;

    common_l3dui_setMode ( lui, modename );

    /*g3dui_redrawGLViews ( gui );*/
}

/******************************************************************************/
void g3dui_setMode ( GtkWidget *widget, gpointer user_data ) {
    const char *modename = gtk_widget_get_name ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_setMode ( gui, modename );

    g3dui_redrawGLViews ( gui );

    g3dui_updateMenuBar ( gui );
}

/******************************************************************************/
static GtkWidget *addModeBarButton ( GtkWidget *bar,
                                     GtkWidget *grp,
                                     G3DUI *gui,
                                     char *name,
                                     const char **xpm_data,
                                     void ( *cbk ) ( GtkWidget *, 
                                                     gpointer ) ) {
    /*GtkIconTheme *icon_theme = gtk_icon_theme_get_default ( );*/
    GtkWidget *btn = (GtkWidget*) gtk_radio_tool_button_new_from_widget ( GTK_RADIO_TOOL_BUTTON(grp) );

    gtk_widget_set_name ( btn, name );

    gtk_widget_set_tooltip_text ( btn, name );

    gtk_widget_set_size_request ( btn, MODEBARBUTTONSIZE, MODEBARBUTTONSIZE );

    if ( xpm_data ) {
        GdkPixbuf *xpm_img = gdk_pixbuf_new_from_xpm_data ( xpm_data );

        /*** Some trick so that the button fits exactly ***/
        xpm_img = gdk_pixbuf_new_subpixbuf ( xpm_img, 0x03, 
                                                      0x03, 
                                                      MODEBARBUTTONSIZE - 0x06, 
                                                      MODEBARBUTTONSIZE - 0x06 );

        if ( xpm_img ) {
            GtkWidget *xpm_wid = gtk_image_new_from_pixbuf ( xpm_img );



            gtk_widget_show ( xpm_wid );

            gtk_tool_button_set_icon_widget ( GTK_TOOL_BUTTON(btn), xpm_wid );
        }
    }

    if ( cbk ) {
        g_signal_connect ( btn, "toggled", G_CALLBACK (cbk), gui );
    }

    gtk_toolbar_insert ( GTK_TOOLBAR(bar), GTK_TOOL_ITEM(btn), -1 );

    gtk_widget_show ( btn );


    return btn;
}

/******************************************************************************/
GtkWidget *createModeBar ( GtkWidget *parent, G3DUI *gui,
                                              char *name,
                                              gint x,
                                              gint y,
                                              gint width,
                                              gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *bar = gtk_toolbar_new ( );
    GtkWidget *grp;

    gtk_widget_set_name ( bar, name );

    gtk_widget_size_allocate ( bar, &gdkrec );

    gtk_toolbar_set_style(GTK_TOOLBAR(bar), GTK_TOOLBAR_ICONS);

    gtk_orientable_set_orientation ( GTK_ORIENTABLE(bar), GTK_ORIENTATION_VERTICAL );

    grp = addModeBarButton ( bar, NULL, gui, MODE_VIEWOBJECT, objectmode_test_xpm, g3dui_setMode );
          addModeBarButton ( bar, grp , gui, MODE_VIEWVERTEX, vertexmode_test_xpm, g3dui_setMode );
          addModeBarButton ( bar, grp , gui, MODE_VIEWEDGE  , edgemode_test_xpm  , g3dui_setMode );
          addModeBarButton ( bar, grp , gui, MODE_VIEWFACE  , facemode_test_xpm  , g3dui_setMode );
          addModeBarButton ( bar, grp , gui, MODE_VIEWSKIN  , skinmode_test_xpm  , g3dui_setMode );
          addModeBarButton ( bar, grp , gui, MODE_VIEWUVWMAP, uvwmapmode_test_xpm, g3dui_setMode );
          addModeBarButton ( bar, grp , gui, MODE_VIEWAXIS  , axismode_test_xpm  , g3dui_setMode );
          addModeBarButton ( bar, grp , gui, MODE_VIEWPATH  , pathmode_xpm  , g3dui_setMode );
          /*addModeBarButton ( bar, grp , gui, MODE_VIEWSCULPT, sculptmode_xpm, g3dui_setMode );*/

    gtk_toolbar_set_show_arrow ( GTK_TOOLBAR(bar), 0 );

    gtk_fixed_put ( GTK_FIXED(parent), bar, x, y );

    gtk_widget_show ( bar );



    return bar;
}

/******************************************************************************/
GtkWidget *createUVMapEditorModeBar ( GtkWidget *parent, 
                                      G3DUI     *gui,
                                      char      *name,
                                      gint       x,
                                      gint       y,
                                      gint       width,
                                      gint       height ) {
    GtkUVMapEditor *guv = ( GtkUVMapEditor * ) parent;
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *bar = gtk_toolbar_new ( );
    GtkWidget *grp = NULL;

    gtk_widget_set_name ( bar, name );

    gtk_widget_size_allocate ( bar, &gdkrec );

    gtk_toolbar_set_style(GTK_TOOLBAR(bar), GTK_TOOLBAR_ICONS);

    gtk_orientable_set_orientation ( GTK_ORIENTABLE(bar), GTK_ORIENTATION_VERTICAL );

    grp = addModeBarButton ( bar, grp , &guv->lui, MODE_VIEWVERTEX, vertexmode_xpm, l3dui_setUVMode );
          addModeBarButton ( bar, grp , &guv->lui, MODE_VIEWFACE  , facemode_xpm  , l3dui_setUVMode );

    gtk_toolbar_set_show_arrow ( GTK_TOOLBAR(bar), 0 );

    gtk_fixed_put ( GTK_FIXED(parent), bar, x, y );

    gtk_widget_show ( bar );



    return bar;
}
