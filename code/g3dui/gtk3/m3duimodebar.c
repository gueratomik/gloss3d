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
#include <xpm/facemode_test2.xpm>
#include <xpm/vertexmode_test2.xpm>

/******************************************************************************/
static GTK3M3DUIMODEBAR *gtk3_m3duimodebar_new ( GTK3M3DUI *gtk3mui ) {
    uint32_t memSize =  sizeof ( GTK3M3DUIMODEBAR );
    GTK3M3DUIMODEBAR *gtk3mb = ( GTK3M3DUIMODEBAR * ) calloc ( 0x01, memSize );

    if ( gtk3mb == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3mb->core.mui = ( M3DUI * ) gtk3mui;


    return gtk3mb;
}

/******************************************************************************/
static void setMode ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3M3DUIMODEBAR *gtk3mb = ( GTK3M3DUIMODEBAR * ) user_data;
    M3DUI *mui = gtk3mb->core.mui;
    G3DUI *gui = mui->gui;
    const char *modename = gtk_widget_get_name ( widget );
    uint64_t ret;

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    ret = m3duimodebar_setMode ( &gtk3mb->core, modename );

    gtk3_interpretUIReturnFlags ( ( GTK3G3DUI * ) gui, ret );
}

/******************************************************************************/
static GtkWidget *addModeBarButton ( GtkWidget        *bar,
                                     GtkWidget        *grp,
                                     GTK3M3DUIMODEBAR *gtk3mb,
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
GTK3M3DUIMODEBAR *gtk3_m3duimodebar_create ( GtkWidget *parent,
                                             GTK3M3DUI *gtk3gui,
                                             char      *name ) {
    GTK3M3DUIMODEBAR *gtk3mb = gtk3_m3duimodebar_new ( gtk3gui );

    gtk3mb->bar = ui_gtk_toolbar_new ( CLASS_MAIN );

    gtk_widget_set_name ( gtk3mb->bar, name );

    gtk_toolbar_set_style(GTK_TOOLBAR(gtk3mb->bar), GTK_TOOLBAR_ICONS);

    gtk_orientable_set_orientation ( GTK_ORIENTABLE(gtk3mb->bar), GTK_ORIENTATION_VERTICAL );

    gtk3mb->vertexMode = addModeBarButton ( gtk3mb->bar, NULL              , gtk3mb, MODE_VIEWVERTEX, vertexmode_test_xpm, setMode );
    gtk3mb->faceMode   = addModeBarButton ( gtk3mb->bar, gtk3mb->vertexMode, gtk3mb, MODE_VIEWFACE  , facemode_test_xpm  , setMode );

    gtk_toolbar_set_show_arrow ( GTK_TOOLBAR(gtk3mb->bar), TRUE );

    gtk_widget_show ( gtk3mb->bar );


    return gtk3mb;
}
