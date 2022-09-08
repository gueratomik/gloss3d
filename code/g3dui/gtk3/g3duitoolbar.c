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
static void saveFileCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITOOLBAR *gtk3toolbar = ( GTK3G3DUITOOLBAR * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3toolbar->core.gui;

    gtk3_saveFile ( gtk3gui);
}

/******************************************************************************/
static void saveAsCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITOOLBAR *gtk3toolbar = ( GTK3G3DUITOOLBAR * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3toolbar->core.gui;

    gtk3_saveAs ( gtk3gui );
}

/******************************************************************************/
static void openFileCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITOOLBAR *gtk3toolbar = ( GTK3G3DUITOOLBAR * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3toolbar->core.gui;

    gtk3_openFile ( gtk3gui );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWALL );
}

/******************************************************************************/
static void setMouseToolCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITOOLBAR *gtk3toolbar = ( GTK3G3DUITOOLBAR * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3toolbar->core.gui;

    if ( gtk3gui->core.lock ) return;

    if ( widget != gtk3gui->currentMouseToolButton ) {
        if ( gtk3_setMouseTool ( gtk3gui, gtk_widget_get_name ( widget ) ) ) {
            /*** prevent callback to loop ***/
            gtk3gui->core.lock = 0x01;

            if ( GTK_IS_TOGGLE_TOOL_BUTTON ( gtk3gui->currentMouseToolButton ) ) {
                GtkToggleToolButton *ttb = GTK_TOGGLE_TOOL_BUTTON(gtk3gui->currentMouseToolButton);

                gtk_toggle_tool_button_set_active ( ttb, FALSE );
            }

            gtk3gui->currentMouseToolButton = widget;

            gtk3gui->core.lock = 0x00;
        }
    }
}

/******************************************************************************/
static void newSceneCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITOOLBAR *gtk3toolbar = ( GTK3G3DUITOOLBAR * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3toolbar->core.gui;

    gtk3_newScene ( gtk3gui );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWALL );
}

/******************************************************************************/
static void undoCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITOOLBAR *gtk3toolbar = ( GTK3G3DUITOOLBAR * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3toolbar->core.gui;
    uint64_t ret = g3dui_undo ( gtk3toolbar->core.gui );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );
}

/******************************************************************************/
static void redoCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITOOLBAR *gtk3toolbar = ( GTK3G3DUITOOLBAR * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3toolbar->core.gui;
    uint64_t ret = g3dui_redo ( &gtk3gui->core );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );
}

/******************************************************************************/
static void makeEditableCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITOOLBAR *gtk3toolbar = ( GTK3G3DUITOOLBAR * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3toolbar->core.gui;
    uint64_t ret = g3dui_makeEditable ( &gtk3gui->core );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );
}

/******************************************************************************/
void deleteSelectionCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITOOLBAR *gtk3toolbar = ( GTK3G3DUITOOLBAR * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3toolbar->core.gui;
    uint64_t ret = g3dui_deleteSelection ( gtk3toolbar->core.gui );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );
}

/******************************************************************************/
static void renderViewCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITOOLBAR *gtk3toolbar = ( GTK3G3DUITOOLBAR * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3toolbar->core.gui;

    gtk3_renderView ( gtk3gui );
}

/******************************************************************************/
static void runRenderCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITOOLBAR *gtk3toolbar = ( GTK3G3DUITOOLBAR * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3toolbar->core.gui;

    gtk3_runRenderCbk ( gtk3gui );
}

/******************************************************************************/
static void xAxisCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITOOLBAR *gtk3toolbar = ( GTK3G3DUITOOLBAR * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3toolbar->core.gui;
    int status = gtk_toggle_tool_button_get_active ( GTK_TOGGLE_TOOL_BUTTON(widget) );

    if ( gtk3gui->core.lock ) return;

    if ( status ) gtk3gui->core.engine_flags |=   XAXIS;
    else          gtk3gui->core.engine_flags &= (~XAXIS);
}

/******************************************************************************/
static void yAxisCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITOOLBAR *gtk3toolbar = ( GTK3G3DUITOOLBAR * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3toolbar->core.gui;
    int status = gtk_toggle_tool_button_get_active ( GTK_TOGGLE_TOOL_BUTTON(widget) );

    if ( gtk3gui->core.lock ) return;

    if ( status ) gtk3gui->core.engine_flags |=   YAXIS;
    else          gtk3gui->core.engine_flags &= (~YAXIS);
}

/******************************************************************************/
static void zAxisCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITOOLBAR *gtk3toolbar = ( GTK3G3DUITOOLBAR * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3toolbar->core.gui;
    int status = gtk_toggle_tool_button_get_active ( GTK_TOGGLE_TOOL_BUTTON(widget) );

    if ( gtk3gui->core.lock ) return;

    if ( status ) gtk3gui->core.engine_flags |=   ZAXIS;
    else          gtk3gui->core.engine_flags &= (~ZAXIS);
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
                                             char      *name ) {
    GTK3G3DUITOOLBAR *gtk3tb = gtk3_g3duitoolbar_new ( gtk3gui );

    gtk3tb->bar = ui_gtk_toolbar_new ( CLASS_MAIN );

    gtk_widget_set_name ( gtk3tb->bar, name );

    gtk_toolbar_set_style(GTK_TOOLBAR(gtk3tb->bar), GTK_TOOLBAR_ICONS);

    gtk3tb->newScene   = addToolBarPushButton ( gtk3tb->bar, 
                                                gtk3tb,
                                                MENU_NEWSCENE,
                                                newfile_xpm,
                                                newSceneCbk  );

    gtk3tb->openFile   = addToolBarPushButton ( gtk3tb->bar,
                                                gtk3tb, MENU_OPENFILE,
                                                openfile_xpm,
                                                openFileCbk );

    gtk3tb->saveFileAs = addToolBarPushButton ( gtk3tb->bar,
                                                gtk3tb,
                                                MENU_SAVEFILEAS,
                                                saveas_xpm,
                                                saveAsCbk );

    gtk3tb->saveFile   = addToolBarPushButton ( gtk3tb->bar,
                                                gtk3tb,
                                                MENU_SAVEFILE,
                                                save_xpm,
                                                saveFileCbk );

    gtk3tb->undo       = addToolBarPushButton ( gtk3tb->bar,
                                                gtk3tb,
                                                MENU_UNDO,
                                                undo_xpm,
                                                undoCbk );

    gtk3tb->redo       = addToolBarPushButton ( gtk3tb->bar,
                                                gtk3tb,
                                                MENU_REDO,
                                                redo_xpm,
                                                redoCbk );

    gtk3tb->delete     = addToolBarPushButton ( gtk3tb->bar,
                                                gtk3tb,
                                                MENU_DELETE,
                                                delete_xpm,
                                                deleteSelectionCbk );

    /********************************/

    gtk3tb->pickTool   = addToolBarToggleButton ( gtk3tb->bar,
                                                  gtk3tb,
                                                  PICKTOOL,
                                                  pick_xpm,
                                                  setMouseToolCbk );

    gtk3tb->moveTool   = addToolBarToggleButton ( gtk3tb->bar,
                                                  gtk3tb,
                                                  MOVETOOL,
                                                  move_xpm,
                                                  setMouseToolCbk );

    gtk3tb->scaleTool  = addToolBarToggleButton ( gtk3tb->bar,
                                                  gtk3tb,
                                                  SCALETOOL,
                                                  scale_xpm,
                                                  setMouseToolCbk );

    gtk3tb->rotateTool = addToolBarToggleButton ( gtk3tb->bar,
                                                  gtk3tb,
                                                  ROTATETOOL,
                                                  rotate_xpm,
                                                  setMouseToolCbk );

    /********************************/

    gtk3tb->renderView   = addToolBarPushButton ( gtk3tb->bar,
                                                  gtk3tb,
                                                  MENU_RENDERVIEW,
                                                  renderw_xpm,
                                                  renderViewCbk );

    gtk3tb->renderFinal  = addToolBarPushButton ( gtk3tb->bar,
                                                  gtk3tb,
                                                  MENU_RENDERFINAL,
                                                  render_xpm,
                                                  runRenderCbk );

    gtk3tb->makeEditable = addToolBarPushButton ( gtk3tb->bar,
                                                  gtk3tb,
                                                  MENU_MAKEEDITABLE,
                                                  makeedit_xpm,
                                                  makeEditableCbk );

    /********************************/

    gtk3tb->xAxis = addToolBarToggleButton ( gtk3tb->bar,
                                             gtk3tb,
                                             MENU_XAXIS,
                                             xaxis_xpm,
                                             xAxisCbk );

    gtk3tb->yAxis = addToolBarToggleButton ( gtk3tb->bar,
                                             gtk3tb,
                                             MENU_YAXIS,
                                             yaxis_xpm,
                                             yAxisCbk );

    gtk3tb->zAxis = addToolBarToggleButton ( gtk3tb->bar,
                                             gtk3tb,
                                             MENU_ZAXIS,
                                             zaxis_xpm,
                                             zAxisCbk );

    /********************************/

    gtk_toggle_tool_button_set_active ( GTK_TOGGLE_TOOL_BUTTON(gtk3tb->xAxis), TRUE );
    gtk_toggle_tool_button_set_active ( GTK_TOGGLE_TOOL_BUTTON(gtk3tb->yAxis), TRUE );
    gtk_toggle_tool_button_set_active ( GTK_TOGGLE_TOOL_BUTTON(gtk3tb->zAxis), TRUE );


    gtk_toolbar_set_show_arrow ( GTK_TOOLBAR(gtk3tb->bar), TRUE );

    gtk_widget_show ( gtk3tb->bar );


    return gtk3tb;
}
