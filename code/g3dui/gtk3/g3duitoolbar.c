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
static GTK3G3DUITOOLBAR *gtk3_g3duitoolbar_new ( ) {
    uint32_t memSize =  sizeof ( GTK3G3DUITOOLBAR );
    GTK3G3DUITOOLBAR *gtk3gtb = ( GTK3G3DUITOOLBAR * ) calloc ( 0x01, memSize );

    if ( gtk3gtb == NULL ) {
        fprint ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3gtb->grp.gui = ( G3DUI * ) gtk3_getUI ( );


    return gtk3gtb;
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
        g_signal_connect ( btn, "clicked", G_CALLBACK (cbk), data );
    }

    gtk_toolbar_insert ( GTK_TOOLBAR(bar), GTK_TOOL_ITEM(btn), -1 );

    gtk_widget_show ( btn );


    return btn;
}

/******************************************************************************/
GTK3G3DUITOOLBAR *gtk3_g3duitoolbar_create ( GtkWidget *parent, 
                                             char      *name,
                                             gint       x,
                                             gint       y,
                                             gint       width,
                                             gint       height ) {
    GTK3G3DUITOOLBAR *gtk3gtb = gtk3_g3duitoolbar_new ( );
    GdkRectangle  gdkrec  = { x, y, width, height };
    GtkWidget    *grp     = NULL;

    gtk3gtb->bar = gtk_toolbar_new ( );

    gtk_widget_set_name ( gtk3gtb->bar, name );

    gtk_widget_size_allocate ( gtk3gtb->bar, &gdkrec );

    gtk_toolbar_set_style(GTK_TOOLBAR(gtk3gtb->bar), GTK_TOOLBAR_ICONS);

    gtk3gtb->newScene   = addToolBarPushButton ( gtk3gtb->bar, 
                                                 gtk3gtb,
                                                 MENU_NEWSCENE,
                                                 newfile_xpm,
                                                 /*g3dui_newscenecbk*/NULL );

    gtk3gtb->openFile   = addToolBarPushButton ( gtk3gtb->bar,
                                                 gtk3gtb, MENU_OPENFILE,
                                                 openfile_xpm,
                                                 /*g3dui_openfilecbk*/NULL );

    gtk3gtb->saveFileAs = addToolBarPushButton ( gtk3gtb->bar,
                                                 gtk3gtb,
                                                 MENU_SAVEFILEAS,
                                                 saveas_xpm,
                                                 /*g3dui_saveascbk*/NULL );

    gtk3gtb->saveFile   = addToolBarPushButton ( gtk3gtb->bar,
                                                 gtk3gtb,
                                                 MENU_SAVEFILE,
                                                 save_xpm,
                                                 /*g3dui_savefilecbk*/NULL );

    gtk3gtb->undo       = addToolBarPushButton ( gtk3gtb->bar,
                                                 gtk3gtb,
                                                 MENU_UNDO,
                                                 undo_xpm,
                                                 /*g3dui_undoCbk*/NULL );

    gtk3gtb->redo       = addToolBarPushButton ( gtk3gtb->bar,
                                                 gtk3gtb,
                                                 MENU_REDO,
                                                 redo_xpm,
                                                 /*g3dui_redoCbk*/NULL );

    gtk3gtb->delete     = addToolBarPushButton ( gtk3gtb->bar,
                                                 gtk3gtb,
                                                 MENU_DELETE,
                                                 delete_xpm,
                                                 /*g3dui_deleteSelectionCbk*/NULL );

    /********************************/

    gtk3gtb->pickTool   = addToolBarToggleButton ( gtk3gtb->bar,
                                                   gtk3gtb,
                                                   PICKTOOL,
                                                   pick_xpm,
                                                   /*g3dui_setMouseTool*/NULL );

    gtk3gtb->moveTool   = addToolBarToggleButton ( gtk3gtb->bar,
                                                   gtk3gtb,
                                                   MOVETOOL,
                                                   move_xpm,
                                                   /*g3dui_setMouseTool*/NULL    );

    gtk3gtb->scaleTool  = addToolBarToggleButton ( gtk3gtb->bar,
                                                   gtk3gtb,
                                                   SCALETOOL,
                                                   scale_xpm,
                                                   /*g3dui_setMouseTool*/NULL  );



    gtk3gtb->rotateTool = addToolBarToggleButton ( gtk3gtb->bar,
                                                   gtk3gtb,
                                                   ROTATETOOL,
                                                   rotate_xpm,
                                                   /*g3dui_setMouseTool*/NULL );

    /********************************/

    gtk3gtb->renderView   = addToolBarPushButton ( gtk3gtb->bar,
                                                   gtk3gtb,
                                                   MENU_RENDERVIEW,
                                                   renderw_xpm,
                                                   /*g3dui_renderViewCbk*/NULL );

    gtk3gtb->renderFinal  = addToolBarPushButton ( gtk3gtb->bar,
                                                   gtk3gtb,
                                                   MENU_RENDERFINAL,
                                                   render_xpm,
                                                   /*g3dui_runRenderCbk*/NULL );

    gtk3gtb->makeEditable = addToolBarPushButton ( gtk3gtb->bar,
                                                   gtk3gtb,
                                                   MENU_MAKEEDITABLE,
                                                   makeedit_xpm,
                                                   /*g3dui_makeEditableCbk*/NULL );

    /********************************/

    gtk3gtb->xAxis = addToolBarToggleButton ( gtk3gtb->bar,
                                              gtk3gtb,
                                              MENU_XAXIS,
                                              xaxis_xpm,
                                              /*xAxisCbk*/NULL );

    gtk3gtb->yAxis = addToolBarToggleButton ( gtk3gtb->bar,
                                              gtk3gtb,
                                              MENU_YAXIS,
                                              yaxis_xpm,
                                              /*yAxisCbk*/NULL );

    gtk3gtb->zAxis = addToolBarToggleButton ( gtk3gtb->bar,
                                              gtk3gtb,
                                              MENU_ZAXIS,
                                              zaxis_xpm,
                                              /*zAxisCbk*/NULL );

    /********************************/

    gtk_toggle_tool_button_set_active ( GTK_TOGGLE_TOOL_BUTTON(gtk3gtb->xAxis), TRUE );
    gtk_toggle_tool_button_set_active ( GTK_TOGGLE_TOOL_BUTTON(gtk3gtb->yAxis), TRUE );
    gtk_toggle_tool_button_set_active ( GTK_TOGGLE_TOOL_BUTTON(gtk3gtb->zAxis), TRUE );


    gtk_toolbar_set_show_arrow ( GTK_TOOLBAR(gtk3gtb->bar), 0 );

    gtk_layout_put ( GTK_LAYOUT(parent), gtk3gtb->bar, x, y );

    gtk_widget_show ( gtk3gtb->bar );


    return gtk3gtb;
}
