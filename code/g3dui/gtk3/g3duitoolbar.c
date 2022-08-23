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
static GTK3G3DUITOOLBAR *gtk3_g3duitoolbar_new ( GTK3G3DUI *gtk3gui ) {
    uint32_t memSize =  sizeof ( GTK3G3DUITOOLBAR );
    GTK3G3DUITOOLBAR *gtk3tb = ( GTK3G3DUITOOLBAR * ) calloc ( 0x01, memSize );

    if ( gtk3tb == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3tb->core.gui = ( G3DUI * ) &gtk3gui->core;


    return gtk3tb;
}

/******************************************************************************/
GtkWidget *addToolBarRadioButton ( GtkWidget   *bar,
                                   GtkWidget   *grp,
                                   void        *data,
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
        g_signal_connect ( btn, "toggled", G_CALLBACK (cbk), data );
    }

    gtk_toolbar_insert ( GTK_TOOLBAR(bar), GTK_TOOL_ITEM(btn), -1 );

    gtk_widget_show ( btn );


    return btn;
}

/******************************************************************************/
GtkWidget *addToolBarToggleButton ( GtkWidget   *bar,
                                    void        *data,
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
        g_signal_connect ( btn, "toggled", G_CALLBACK (cbk), data );
    }

    gtk_toolbar_insert ( GTK_TOOLBAR(bar), GTK_TOOL_ITEM(btn), -1 );

    gtk_widget_show ( btn );


    return btn;
}

/******************************************************************************/
GtkWidget *addToolBarPushButton ( GtkWidget   *bar,
                                  void        *data,
                                  char        *name,
                                  const char **xpm_data,
                                  void       (*cbk) ( GtkWidget *,
                                                      gpointer ) ) {
    /*GtkIconTheme *icon_theme = gtk_icon_theme_get_default ( );*/
    GtkWidget *btn = (GtkWidget*) gtk_tool_button_new ( NULL, NULL );

    gtk_widget_set_name ( btn, /*name*/"TB" );

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
        g_signal_connect ( btn, "clicked", G_CALLBACK (cbk), data );
    }

    gtk_toolbar_insert ( GTK_TOOLBAR(bar), GTK_TOOL_ITEM(btn), -1 );

    gtk_widget_show ( btn );


    return btn;
}

/******************************************************************************/
GTK3G3DUITOOLBAR *gtk3_g3duitoolbar_create ( GtkWidget *parent,
                                             GTK3G3DUI *gtk3gui,
                                             char      *name,
                                             gint       x,
                                             gint       y,
                                             gint       width,
                                             gint       height ) {
    GTK3G3DUITOOLBAR *gtk3tb = gtk3_g3duitoolbar_new ( gtk3gui );
    GdkRectangle  gdkrec     = { x, y, width, height };
    GtkWidget    *grp        = NULL;

    gtk3tb->bar = gtk_toolbar_new ( );

    gtk_widget_set_name ( gtk3tb->bar, name );

    gtk_widget_size_allocate ( gtk3tb->bar, &gdkrec );

    gtk_toolbar_set_style(GTK_TOOLBAR(gtk3tb->bar), GTK_TOOLBAR_ICONS);

    gtk3tb->newScene   = addToolBarPushButton ( gtk3tb->bar, 
                                                gtk3tb,
                                                MENU_NEWSCENE,
                                                newfile_xpm,
                                                /*g3dui_newscenecbk*/NULL );

    gtk3tb->openFile   = addToolBarPushButton ( gtk3tb->bar,
                                                gtk3tb, MENU_OPENFILE,
                                                openfile_xpm,
                                                /*g3dui_openfilecbk*/NULL );

    gtk3tb->saveFileAs = addToolBarPushButton ( gtk3tb->bar,
                                                gtk3tb,
                                                MENU_SAVEFILEAS,
                                                saveas_xpm,
                                                /*g3dui_saveascbk*/NULL );

    gtk3tb->saveFile   = addToolBarPushButton ( gtk3tb->bar,
                                                gtk3tb,
                                                MENU_SAVEFILE,
                                                save_xpm,
                                                /*g3dui_savefilecbk*/NULL );

    gtk3tb->undo       = addToolBarPushButton ( gtk3tb->bar,
                                                gtk3tb,
                                                MENU_UNDO,
                                                undo_xpm,
                                                /*g3dui_undoCbk*/NULL );

    gtk3tb->redo       = addToolBarPushButton ( gtk3tb->bar,
                                                gtk3tb,
                                                MENU_REDO,
                                                redo_xpm,
                                                /*g3dui_redoCbk*/NULL );

    gtk3tb->delete     = addToolBarPushButton ( gtk3tb->bar,
                                                gtk3tb,
                                                MENU_DELETE,
                                                delete_xpm,
                                                /*g3dui_deleteSelectionCbk*/NULL );

    /********************************/

    gtk3tb->pickTool   = addToolBarToggleButton ( gtk3tb->bar,
                                                  gtk3tb,
                                                  PICKTOOL,
                                                  pick_xpm,
                                                  /*g3dui_setMouseTool*/NULL );

    gtk3tb->moveTool   = addToolBarToggleButton ( gtk3tb->bar,
                                                  gtk3tb,
                                                  MOVETOOL,
                                                  move_xpm,
                                                  /*g3dui_setMouseTool*/NULL    );

    gtk3tb->scaleTool  = addToolBarToggleButton ( gtk3tb->bar,
                                                  gtk3tb,
                                                  SCALETOOL,
                                                  scale_xpm,
                                                  /*g3dui_setMouseTool*/NULL  );



    gtk3tb->rotateTool = addToolBarToggleButton ( gtk3tb->bar,
                                                  gtk3tb,
                                                  ROTATETOOL,
                                                  rotate_xpm,
                                                  /*g3dui_setMouseTool*/NULL );

    /********************************/

    gtk3tb->renderView   = addToolBarPushButton ( gtk3tb->bar,
                                                  gtk3tb,
                                                  MENU_RENDERVIEW,
                                                  renderw_xpm,
                                                  /*g3dui_renderViewCbk*/NULL );

    gtk3tb->renderFinal  = addToolBarPushButton ( gtk3tb->bar,
                                                  gtk3tb,
                                                  MENU_RENDERFINAL,
                                                  render_xpm,
                                                  /*g3dui_runRenderCbk*/NULL );

    gtk3tb->makeEditable = addToolBarPushButton ( gtk3tb->bar,
                                                  gtk3tb,
                                                  MENU_MAKEEDITABLE,
                                                  makeedit_xpm,
                                                  /*g3dui_makeEditableCbk*/NULL );

    /********************************/

    gtk3tb->xAxis = addToolBarToggleButton ( gtk3tb->bar,
                                             gtk3tb,
                                             MENU_XAXIS,
                                             xaxis_xpm,
                                             /*xAxisCbk*/NULL );

    gtk3tb->yAxis = addToolBarToggleButton ( gtk3tb->bar,
                                             gtk3tb,
                                             MENU_YAXIS,
                                             yaxis_xpm,
                                             /*yAxisCbk*/NULL );

    gtk3tb->zAxis = addToolBarToggleButton ( gtk3tb->bar,
                                             gtk3tb,
                                             MENU_ZAXIS,
                                             zaxis_xpm,
                                             /*zAxisCbk*/NULL );

    /********************************/

    gtk_toggle_tool_button_set_active ( GTK_TOGGLE_TOOL_BUTTON(gtk3tb->xAxis), TRUE );
    gtk_toggle_tool_button_set_active ( GTK_TOGGLE_TOOL_BUTTON(gtk3tb->yAxis), TRUE );
    gtk_toggle_tool_button_set_active ( GTK_TOGGLE_TOOL_BUTTON(gtk3tb->zAxis), TRUE );


    gtk_toolbar_set_show_arrow ( GTK_TOOLBAR(gtk3tb->bar), 0 );

    gtk_layout_put ( GTK_LAYOUT(parent), gtk3tb->bar, x, y );

    gtk_widget_show ( gtk3tb->bar );


    return gtk3tb;
}
