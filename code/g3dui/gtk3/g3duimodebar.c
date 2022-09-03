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
static GTK3G3DUIMODEBAR *gtk3_g3duimodebar_new ( GTK3G3DUI *gtk3gui ) {
    uint32_t memSize =  sizeof ( GTK3G3DUIMODEBAR );
    GTK3G3DUIMODEBAR *gtk3mb = ( GTK3G3DUIMODEBAR * ) calloc ( 0x01, memSize );

    if ( gtk3mb == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3mb->core.gui = ( G3DUI * ) &gtk3gui->core;


    return gtk3mb;
}

/******************************************************************************/
static void  setUVMode ( GtkWidget *widget,
                         gpointer   user_data ) {
#ifdef TODO
    const char *modename = gtk_widget_get_name ( widget );
    M3DUI *lui = ( M3DUI * ) user_data;

    m3dui_setMode ( lui, modename );

    /*g3dui_redrawGLViews ( gui );*/
#endif
}

/******************************************************************************/
static void setMode ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIMODEBAR *gtk3mb = ( GTK3G3DUIMODEBAR * ) user_data;
    G3DUI *gui = gtk3mb->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    const char *modename = gtk_widget_get_name ( widget );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    gtk3_setHourGlass ( gtk3gui );

    gtk3_interpretUIReturnFlags ( gtk3gui, 
                                  g3duimodebar_setModeCbk ( &gtk3mb->core,
                                                             modename ) );

    gtk3_unsetHourGlass ( gtk3gui );
}

/******************************************************************************/
static GtkWidget *addModeBarButton ( GtkWidget        *bar,
                                     GtkWidget        *grp,
                                     GTK3G3DUIMODEBAR *gtk3mb,
                                     char             *name,
                                     const char      **xpm_data,
                                     void           ( *cbk ) ( GtkWidget *, 
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
        g_signal_connect ( btn, "toggled", G_CALLBACK (cbk), gtk3mb );
    }

    gtk_toolbar_insert ( GTK_TOOLBAR(bar), GTK_TOOL_ITEM(btn), -1 );

    gtk_widget_show ( btn );


    return btn;
}

/******************************************************************************/
GTK3G3DUIMODEBAR *gtk3_g3duimodebar_create ( GtkWidget *parent,
                                             GTK3G3DUI *gtk3gui,
                                             char      *name ) {
    GTK3G3DUIMODEBAR *gtk3mb = gtk3_g3duimodebar_new ( gtk3gui );

    gtk3mb->bar = gtk_toolbar_new ( );

    gtk_widget_set_name ( gtk3mb->bar, name );

    gtk_toolbar_set_style(GTK_TOOLBAR(gtk3mb->bar), GTK_TOOLBAR_ICONS);

    gtk_orientable_set_orientation ( GTK_ORIENTABLE(gtk3mb->bar), GTK_ORIENTATION_VERTICAL );

    gtk3mb->objectMode = addModeBarButton ( gtk3mb->bar, NULL              , gtk3mb, MODE_VIEWOBJECT, objectmode_test_xpm, setMode );
    gtk3mb->vertexMode = addModeBarButton ( gtk3mb->bar, gtk3mb->objectMode, gtk3mb, MODE_VIEWVERTEX, vertexmode_test_xpm, setMode );
    gtk3mb->edgeMode   = addModeBarButton ( gtk3mb->bar, gtk3mb->objectMode, gtk3mb, MODE_VIEWEDGE  , edgemode_test_xpm  , setMode );
    gtk3mb->faceMode   = addModeBarButton ( gtk3mb->bar, gtk3mb->objectMode, gtk3mb, MODE_VIEWFACE  , facemode_test_xpm  , setMode );
    gtk3mb->sculptMode = addModeBarButton ( gtk3mb->bar, gtk3mb->objectMode, gtk3mb, MODE_VIEWSCULPT, sculptmode_xpm     , setMode );
    gtk3mb->skinMode   = addModeBarButton ( gtk3mb->bar, gtk3mb->objectMode, gtk3mb, MODE_VIEWSKIN  , skinmode_test_xpm  , setMode );
    gtk3mb->uvmapMode  = addModeBarButton ( gtk3mb->bar, gtk3mb->objectMode, gtk3mb, MODE_VIEWUVWMAP, uvwmapmode_test_xpm, setMode );
    gtk3mb->axisMode   = addModeBarButton ( gtk3mb->bar, gtk3mb->objectMode, gtk3mb, MODE_VIEWAXIS  , axismode_test_xpm  , setMode );
    gtk3mb->pathMode   = addModeBarButton ( gtk3mb->bar, gtk3mb->objectMode, gtk3mb, MODE_VIEWPATH  , pathmode_xpm       , setMode );

    gtk_toolbar_set_show_arrow ( GTK_TOOLBAR(gtk3mb->bar), TRUE );

    gtk_widget_show ( gtk3mb->bar );


    return gtk3mb;
}
