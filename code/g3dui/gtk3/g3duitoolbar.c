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
#include <xpm/render.xpm>
#include <xpm/newfile.xpm>
#include <xpm/openfile.xpm>
#include <xpm/undo.xpm>
#include <xpm/redo.xpm>
#include <xpm/save.xpm>
#include <xpm/saveas.xpm>
#include <xpm/stoprender.xpm>
#include <xpm/renderw.xpm>
#include <xpm/makeedit.xpm>
#include <xpm/pick.xpm>
#include <xpm/move.xpm>
#include <xpm/scale.xpm>
#include <xpm/rotate.xpm>
#include <xpm/delete.xpm>
#include <xpm/xaxis.xpm>
#include <xpm/yaxis.xpm>
#include <xpm/zaxis.xpm>

/******************************************************************************/
GtkWidget *addToolBarRadioButton ( GtkWidget   *bar,
                                   GtkWidget   *grp,
                                   G3DUI       *gui,
                                   char        *name,
                                   const char **xpm_data,
                                   void      ( *cbk ) ( GtkWidget *, 
                                                        gpointer ) ) {
    /*GtkIconTheme *icon_theme = gtk_icon_theme_get_default ( );*/
    GtkWidget *btn = (GtkWidget*) gtk_radio_tool_button_new_from_widget ( GTK_RADIO_TOOL_BUTTON(grp) );

    gtk_widget_set_name ( btn, name );

    gtk_widget_set_size_request ( btn, TOOLBARBUTTONSIZE, TOOLBARBUTTONSIZE );

    if ( xpm_data ) {
        GdkPixbuf *xpm_img = gdk_pixbuf_new_from_xpm_data ( xpm_data );

        /*** Some trick so that the button fits exactly ***/
        /*xpm_img = gdk_pixbuf_new_subpixbuf ( xpm_img, 0x03, 
                                                      0x03, 
                                                      TOOLBARBUTTONSIZE - 0x06, 
                                                      TOOLBARBUTTONSIZE - 0x06 );*/

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
GtkWidget *addToolBarToggleButton ( GtkWidget   *bar,
                                    G3DUI       *gui,
                                    char        *name,
                                    const char **xpm_data,
                                    void       (*cbk) ( GtkWidget *, 
                                                        gpointer ) ) {
    /*GtkIconTheme *icon_theme = gtk_icon_theme_get_default ( );*/
    GtkWidget *btn = (GtkWidget*) gtk_toggle_tool_button_new ( );

    gtk_widget_set_name ( btn, name );

    gtk_widget_set_tooltip_text ( btn, name );

    gtk_widget_set_size_request ( btn, TOOLBARBUTTONSIZE, TOOLBARBUTTONSIZE );

    if ( xpm_data ) {
        GdkPixbuf *xpm_img = gdk_pixbuf_new_from_xpm_data ( xpm_data );

        /*** Some trick so that the button fits exactly ***/
        /*xpm_img = gdk_pixbuf_new_subpixbuf ( xpm_img, 0x03, 
                                                      0x03, 
                                                      TOOLBARBUTTONSIZE - 0x06, 
                                                      TOOLBARBUTTONSIZE - 0x06 );*/

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
GtkWidget *addToolBarPushButton ( GtkWidget   *bar,
                                  G3DUI       *gui,
                                  char        *name,
                                  const char **xpm_data,
                                  void       (*cbk) ( GtkWidget *,
                                                      gpointer ) ) {
    /*GtkIconTheme *icon_theme = gtk_icon_theme_get_default ( );*/
    GtkWidget *btn = (GtkWidget*) gtk_tool_button_new ( NULL, NULL );

    gtk_widget_set_name ( btn, name );

    gtk_widget_set_tooltip_text ( btn, name );

    gtk_widget_set_size_request ( btn, TOOLBARBUTTONSIZE, TOOLBARBUTTONSIZE );

    if ( xpm_data ) {
        GdkPixbuf *xpm_img = gdk_pixbuf_new_from_xpm_data ( xpm_data );

        /*** Some trick so that the button fits exactly ***/
        /*xpm_img = gdk_pixbuf_new_subpixbuf ( xpm_img, 0x03, 
                                                      0x03, 
                                                      TOOLBARBUTTONSIZE - 0x06, 
                                                      TOOLBARBUTTONSIZE - 0x06 );*/
        if ( xpm_img ) {
            GtkWidget *xpm_wid = gtk_image_new_from_pixbuf ( xpm_img );

            gtk_widget_show ( xpm_wid );

            gtk_tool_button_set_icon_widget ( GTK_TOOL_BUTTON(btn), xpm_wid );
        }
    }

    if ( cbk ) {
        g_signal_connect ( btn, "clicked", G_CALLBACK (cbk), gui );
    }

    gtk_toolbar_insert ( GTK_TOOLBAR(bar), GTK_TOOL_ITEM(btn), -1 );

    gtk_widget_show ( btn );


    return btn;
}

/******************************************************************************/
static void addToolbarAxis ( GtkWidget *bar, G3DUI *gui ) {
    GtkWidget *xbtn, *ybtn, *zbtn;

    xbtn = addToolBarToggleButton ( bar, gui, MENU_XAXIS, xaxis_xpm, /*xAxisCbk*/NULL );
    ybtn = addToolBarToggleButton ( bar, gui, MENU_YAXIS, yaxis_xpm, /*yAxisCbk*/NULL );
    zbtn = addToolBarToggleButton ( bar, gui, MENU_ZAXIS, zaxis_xpm, /*zAxisCbk*/NULL );

    gtk_toggle_tool_button_set_active ( GTK_TOGGLE_TOOL_BUTTON(xbtn), TRUE );
    gtk_toggle_tool_button_set_active ( GTK_TOGGLE_TOOL_BUTTON(ybtn), TRUE );
    gtk_toggle_tool_button_set_active ( GTK_TOGGLE_TOOL_BUTTON(zbtn), TRUE );
}

/******************************************************************************/
GtkWidget *gtk3_g3dui_createToolBar ( GtkWidget *parent, 
                                      G3DUI     *gui,
                                      char      *name,
                                      gint       x,
                                      gint       y,
                                      gint       width,
                                      gint       height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *bar = gtk_toolbar_new ( );
    GtkWidget *grp = NULL;

    gtk_widget_set_name ( bar, name );

    gtk_widget_size_allocate ( bar, &gdkrec );

    gtk_toolbar_set_style(GTK_TOOLBAR(bar), GTK_TOOLBAR_ICONS);

    addToolBarPushButton   ( bar, gui, MENU_NEWSCENE    , newfile_xpm  , /*g3dui_newscenecbk*/NULL );
    addToolBarPushButton   ( bar, gui, MENU_OPENFILE    , openfile_xpm , /*g3dui_openfilecbk*/NULL );
    addToolBarPushButton   ( bar, gui, MENU_SAVEFILEAS  , saveas_xpm   , /*g3dui_saveascbk*/NULL   );
    addToolBarPushButton   ( bar, gui, MENU_SAVEFILE    , save_xpm     , /*g3dui_savefilecbk*/NULL );
    addToolBarPushButton   ( bar, gui, MENU_UNDO        , undo_xpm     , /*g3dui_undoCbk*/NULL     );
    addToolBarPushButton   ( bar, gui, MENU_REDO        , redo_xpm     , /*g3dui_redoCbk*/NULL     );
    addToolBarPushButton   ( bar, gui, MENU_DELETE      , delete_xpm   , /*g3dui_deleteSelectionCbk*/NULL );

    /********************************/

    addToolBarToggleButton ( bar, gui, PICKTOOL         , pick_xpm  , /*g3dui_setMouseTool*/NULL    );
 
    /********************************/

    addToolBarToggleButton ( bar, gui, MOVETOOL         , move_xpm  , /*g3dui_setMouseTool*/NULL    );

    /********************************/

    addToolBarToggleButton ( bar, gui, SCALETOOL        , scale_xpm , /*g3dui_setMouseTool*/NULL  );

    /********************************/

    addToolBarToggleButton ( bar, gui, ROTATETOOL       , rotate_xpm, /*g3dui_setMouseTool*/NULL );

    addToolBarPushButton   ( bar, gui, MENU_RENDERVIEW  , renderw_xpm   , /*g3dui_renderViewCbk*/NULL   );
    addToolBarPushButton   ( bar, gui, MENU_RENDERFINAL , render_xpm    , /*g3dui_runRenderCbk*/NULL    );
    addToolBarPushButton   ( bar, gui, MENU_MAKEEDITABLE, makeedit_xpm  , /*g3dui_makeEditableCbk*/NULL );

    addToolbarAxis ( bar, gui );

    gtk_toolbar_set_show_arrow ( GTK_TOOLBAR(bar), 0 );

    gtk_layout_put ( GTK_LAYOUT(parent), bar, x, y );

    gtk_widget_show ( bar );


    return bar;
}
