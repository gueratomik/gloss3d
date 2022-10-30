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
static void xAxisCbk ( GtkWidget *widget, gpointer user_data ) {
    int status = gtk_toggle_tool_button_get_active ( GTK_TOGGLE_TOOL_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( status ) gui->engine_flags |=   XAXIS;
    else          gui->engine_flags &= (~XAXIS);
}

/******************************************************************************/
static void yAxisCbk ( GtkWidget *widget, gpointer user_data ) {
    int status = gtk_toggle_tool_button_get_active ( GTK_TOGGLE_TOOL_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( status ) gui->engine_flags |=   YAXIS;
    else          gui->engine_flags &= (~YAXIS);
}

/******************************************************************************/
static void zAxisCbk ( GtkWidget *widget, gpointer user_data ) {
    int status = gtk_toggle_tool_button_get_active ( GTK_TOGGLE_TOOL_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( status ) gui->engine_flags |=   ZAXIS;
    else          gui->engine_flags &= (~ZAXIS);
}

/******************************************************************************/
void g3dui_saveascbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Save file as ...",
                                           GTK_WINDOW(ggt->top),
                        /*** from ristretto-0.3.5/src/main_window.c ***/
                                           GTK_FILE_CHOOSER_ACTION_SAVE,
                                           "_Cancel", 
                                           GTK_RESPONSE_CANCEL,
                                           "_Open", 
                                           GTK_RESPONSE_OK,
                                           NULL );

    gtk_file_chooser_set_do_overwrite_confirmation ( GTK_FILE_CHOOSER(dialog), TRUE );

    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_OK ) {
        GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
        char           *filename = gtk_file_chooser_get_filename ( chooser );
        static char     filenameext[0x1000] = { 0x00 };
        static char     windowname[0x1000] = { 0x00 };

        if ( strstr ( filename, ".g3d" ) == NULL ) {
            snprintf ( filenameext, 0x1000, "%s.g3d", filename );
        } else {
            snprintf ( filenameext, 0x1000, "%s", filename );
        }

        common_g3dui_setFileName ( gui, filenameext );

        common_g3dui_saveG3DFile ( gui );

        snprintf ( windowname, 
                   0x1000, 
                  "%s %s (%s)", 
                   G3DUIAPPNAME,
                   VERSION,
                   basename ( filenameext ) );

        gtk_window_set_title ( gtk_widget_get_parent ( ggt->top ), windowname );

        g_free    ( ( gpointer ) filename );
    }

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
void g3dui_savefilecbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    GtkWidget *dialog;
    gint       res;

    if ( gui->filename ) {
        printf ( "saving as:%s\n", gui->filename );

        common_g3dui_saveG3DFile ( gui );
    } else {
        g3dui_saveascbk ( widget, user_data );
    }
}

/******************************************************************************/
void g3dui_newscenecbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_message_dialog_new ( NULL,
                                      GTK_DIALOG_MODAL,
                                      GTK_MESSAGE_QUESTION,
                                      GTK_BUTTONS_YES_NO,
                                      "Close scene and create a new one ?" );


    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_YES ) {
        common_g3dui_closeScene ( gui );

        g3dui_clearMaterials ( gui );

        gui->sce = g3dscene_new ( 0x00, "SCENE" );
    }

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
/*** WTF: https://developer.gnome.org/gtk3/stable/GtkFileChooserDialog.html ***/
void g3dui_openfilecbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Open File",
                                           GTK_WINDOW(ggt->top),
                        /*** from ristretto-0.3.5/src/main_window.c ***/
                                           GTK_FILE_CHOOSER_ACTION_OPEN,
                                           "_Cancel", 
                                           GTK_RESPONSE_CANCEL,
                                           "_Open", 
                                           GTK_RESPONSE_OK,
                                           NULL );

    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_OK ) {
        GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
        char           *filename = gtk_file_chooser_get_filename ( chooser );
        static char     windowname[0x1000] = { 0x00 };

        if ( filename ) {
            common_g3dui_closeScene ( gui );

            common_g3dui_openG3DFile ( gui, filename );

            snprintf ( windowname, 
                       0x1000, 
                      "%s %s (%s)", 
                       G3DUIAPPNAME,
                       VERSION,
                       basename ( filename ) );

            gtk_window_set_title ( gtk_widget_get_parent ( ggt->top ), windowname );

            g_free    ( filename );
        }
    }

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
void g3dui_mergeSceneCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Merge File",
                                           GTK_WINDOW(ggt->top),
                        /*** from ristretto-0.3.5/src/main_window.c ***/
                                           GTK_FILE_CHOOSER_ACTION_OPEN,
                                           "_Cancel", 
                                           GTK_RESPONSE_CANCEL,
                                           "_Open", 
                                           GTK_RESPONSE_OK,
                                           NULL );

    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_OK ) {
        GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
        char           *filename = gtk_file_chooser_get_filename ( chooser );

        if ( filename ) {
            common_g3dui_mergeG3DFile ( gui, filename );

            g_free    ( filename );
        }
    }

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
void g3dui_undoCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_undoCbk ( gui );
}

/******************************************************************************/
void g3dui_redoCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_redoCbk ( gui );
}


/******************************************************************************/
void g3dui_makeEditableCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_makeEditableCbk ( gui );
}

/******************************************************************************/
void g3dui_deleteSelectionCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_deleteSelectionCbk ( gui );
}

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

    xbtn = addToolBarToggleButton ( bar, gui, MENU_XAXIS, xaxis_xpm, xAxisCbk );
    ybtn = addToolBarToggleButton ( bar, gui, MENU_YAXIS, yaxis_xpm, yAxisCbk );
    zbtn = addToolBarToggleButton ( bar, gui, MENU_ZAXIS, zaxis_xpm, zAxisCbk );

    gtk_toggle_tool_button_set_active ( GTK_TOGGLE_TOOL_BUTTON(xbtn), TRUE );
    gtk_toggle_tool_button_set_active ( GTK_TOGGLE_TOOL_BUTTON(ybtn), TRUE );
    gtk_toggle_tool_button_set_active ( GTK_TOGGLE_TOOL_BUTTON(zbtn), TRUE );
}

/******************************************************************************/
GtkWidget *createToolBar ( GtkWidget *parent, G3DUI *gui,
                                              char *name,
                                              gint x,
                                              gint y,
                                              gint width,
                                              gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *bar = gtk_toolbar_new ( );
    GtkWidget *grp = NULL;

    gtk_widget_set_name ( bar, name );

    gtk_widget_size_allocate ( bar, &gdkrec );

    gtk_toolbar_set_style(GTK_TOOLBAR(bar), GTK_TOOLBAR_ICONS);

    addToolBarPushButton   ( bar, gui, MENU_NEWSCENE    , newfile_xpm  , g3dui_newscenecbk );
    addToolBarPushButton   ( bar, gui, MENU_OPENFILE    , openfile_xpm , g3dui_openfilecbk );
    addToolBarPushButton   ( bar, gui, MENU_SAVEFILEAS  , saveas_xpm   , g3dui_saveascbk   );
    addToolBarPushButton   ( bar, gui, MENU_SAVEFILE    , save_xpm     , g3dui_savefilecbk );
    addToolBarPushButton   ( bar, gui, MENU_UNDO        , undo_xpm     , g3dui_undoCbk     );
    addToolBarPushButton   ( bar, gui, MENU_REDO        , redo_xpm     , g3dui_redoCbk     );
    addToolBarPushButton   ( bar, gui, MENU_DELETE      , delete_xpm   , g3dui_deleteSelectionCbk );

    /********************************/

    addToolBarToggleButton ( bar, gui, PICKTOOL         , pick_xpm  , g3dui_setMouseTool    );
 
    /********************************/

    addToolBarToggleButton ( bar, gui, MOVETOOL         , move_xpm  , g3dui_setMouseTool    );

    /********************************/

    addToolBarToggleButton ( bar, gui, SCALETOOL        , scale_xpm , g3dui_setMouseTool  );

    /********************************/

    addToolBarToggleButton ( bar, gui, ROTATETOOL       , rotate_xpm, g3dui_setMouseTool );

    addToolBarPushButton   ( bar, gui, MENU_RENDERVIEW  , renderw_xpm   , g3dui_renderViewCbk   );
    addToolBarPushButton   ( bar, gui, MENU_RENDERFINAL , render_xpm    , g3dui_runRenderCbk    );
    addToolBarPushButton   ( bar, gui, MENU_MAKEEDITABLE, makeedit_xpm  , g3dui_makeEditableCbk );

    addToolbarAxis ( bar, gui );

    gtk_toolbar_set_show_arrow ( GTK_TOOLBAR(bar), TRUE );

    gtk_fixed_put ( GTK_FIXED(parent), bar, x, y );

    gtk_widget_show ( bar );


    return bar;
}
