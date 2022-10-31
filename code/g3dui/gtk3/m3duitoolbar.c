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
#include <xpm/selectarea.xpm>
#include <xpm/selectrandom.xpm>
#include <xpm/pen.xpm>
#include <xpm/bucket.xpm>
#include <xpm/eraser.xpm>
#include <xpm/unselectarea.xpm>
#include <xpm/saveimageas.xpm>
#include <xpm/saveimage.xpm>

/******************************************************************************/
static GTK3M3DUITOOLBAR *gtk3_g3duitoolbar_new ( GTK3M3DUI *gtk3mui ) {
    uint32_t memSize =  sizeof ( GTK3M3DUITOOLBAR );
    GTK3M3DUITOOLBAR *gtk3tb = ( GTK3M3DUITOOLBAR * ) calloc ( 0x01, memSize );

    if ( gtk3tb == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3tb->core.mui = ( M3DUI * ) gtk3mui;


    return gtk3tb;
}

/******************************************************************************/
static void loadImageByChannelIDCbk ( GtkWidget *widget, 
                                      gpointer   user_data ) {
    GTK3M3DUITOOLBAR *gtk3tb = ( GTK3M3DUITOOLBAR * ) user_data;
    M3DUI *mui = ( M3DUI * ) gtk3tb->core.mui;
    G3DUI *gui = ( G3DUI * ) mui->gui;
    uint64_t ret = gtk3_m3dui_loadImageByChannelIDCbk ( ( GTK3M3DUI * ) mui );

    gtk3_interpretUIReturnFlags ( ( GTK3G3DUI * ) gui, ret );
}

/******************************************************************************/
static void createChannelImageCbk ( GtkWidget *widget, 
                                    gpointer   user_data ) {
    GTK3M3DUITOOLBAR *gtk3tb = ( GTK3M3DUITOOLBAR * ) user_data;
    M3DUI *mui = ( M3DUI * ) gtk3tb->core.mui;
    G3DUI *gui = ( G3DUI * ) mui->gui;
    G3DCHANNEL *chn = m3dui_getWorkingChannel ( mui );

    if ( chn ) {
        GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );
#ifdef TODO
        createChannelImage ( dial, 
                             lui, 
                             chn,
                             0x00,
                             "Channel Image", 0, 0, 200, 96 );
#endif
        gtk_widget_show ( dial );
    }
}

/******************************************************************************/
static void saveFileAsCbk ( GtkWidget *widget, 
                            gpointer   user_data ) {
    GTK3M3DUITOOLBAR *gtk3tb = ( GTK3M3DUITOOLBAR * ) user_data;
    M3DUI *mui = ( M3DUI * ) gtk3tb->core.mui;
    G3DUI *gui = ( G3DUI * ) mui->gui;

    gtk3_saveAs ( ( GTK3G3DUI * ) gui );
}

/******************************************************************************/
static void saveFileCbk ( GtkWidget *widget, 
                          gpointer   user_data ) {
    GTK3M3DUITOOLBAR *gtk3tb = ( GTK3M3DUITOOLBAR * ) user_data;
    M3DUI *mui = ( M3DUI * ) gtk3tb->core.mui;
    G3DUI *gui = ( G3DUI * ) mui->gui;

    gtk3_saveFile ( ( GTK3G3DUI * ) gui );
}

/******************************************************************************/
static void undoCbk ( GtkWidget *widget, 
                      gpointer   user_data ) {
    GTK3M3DUITOOLBAR *gtk3tb = ( GTK3M3DUITOOLBAR * ) user_data;
    M3DUI *mui = ( M3DUI * ) gtk3tb->core.mui;
    G3DUI *gui = ( G3DUI * ) mui->gui;
    uint64_t ret;

    ret = m3dui_undo ( mui );

    gtk3_interpretUIReturnFlags ( ( GTK3G3DUI * ) gui, ret );
}

/******************************************************************************/
static void redoCbk ( GtkWidget *widget, 
                      gpointer   user_data ) {
    GTK3M3DUITOOLBAR *gtk3tb = ( GTK3M3DUITOOLBAR * ) user_data;
    M3DUI *mui = ( M3DUI * ) gtk3tb->core.mui;
    G3DUI *gui = ( G3DUI * ) mui->gui;
    uint64_t ret;

    ret = m3dui_undo ( mui );

    gtk3_interpretUIReturnFlags ( ( GTK3G3DUI * ) gui, ret );
}

/******************************************************************************/
static void setMouseToolCbk ( GtkWidget *widget, 
                              gpointer   user_data ) {
    GTK3M3DUITOOLBAR *gtk3tb = ( GTK3M3DUITOOLBAR * ) user_data;
    M3DUI *mui = ( M3DUI * ) gtk3tb->core.mui;
    char *toolName = gtk_widget_get_name ( widget );

    if ( mui->gui->lock ) return;

    /*** this func will be called on all disarmed radio buttons ***/
    /*** that's why we have to lock it ***/
    mui->gui->lock = 0x01;

    gtk3_m3dui_setMouseTool ( ( GTK3M3DUI * ) mui, widget, toolName );

    mui->gui->lock = 0x00;
}

/******************************************************************************/
static void setSquareSelectorCbk ( GtkWidget *widget, 
                                   gpointer   user_data ) {
    GTK3M3DUITOOLBAR *gtk3tb = ( GTK3M3DUITOOLBAR * ) user_data;
    M3DUI *mui = ( M3DUI * ) gtk3tb->core.mui;
    G3DUI *gui = ( G3DUI * ) mui->gui;
    G3DUIMOUSETOOL *mou = g3dui_getMouseTool ( gui, SELECTTOOL );

    if ( mou ) {
        M3DMOUSETOOL *tool = mou->tool;

        if ( tool ) {
            m3dselector_setMode ( ( M3DSELECTOR * ) tool->obj, SELECTORMODESQUARE );
        }
    }

    gtk3_m3dui_setMouseTool ( ( GTK3M3DUI * ) mui, widget, SELECTTOOL );
}

/******************************************************************************/
static void setRandomSelectorCbk ( GtkWidget *widget, 
                                   gpointer   user_data ) {
    GTK3M3DUITOOLBAR *gtk3tb = ( GTK3M3DUITOOLBAR * ) user_data;
    M3DUI *mui = ( M3DUI * ) gtk3tb->core.mui;
    G3DUI *gui = ( G3DUI * ) mui->gui;
    G3DUIMOUSETOOL *mou = g3dui_getMouseTool ( gui, SELECTTOOL );

    if ( mou ) {
        M3DMOUSETOOL *tool = mou->tool;

        if ( tool ) {
            m3dselector_setMode ( ( M3DSELECTOR * ) tool->obj, SELECTORMODERANDOM );
        }
    }

    gtk3_m3dui_setMouseTool ( ( GTK3M3DUI * ) mui, widget, SELECTTOOL );
}

/******************************************************************************/
static void saveImageAsCbk ( GtkWidget *widget, 
                             gpointer   user_data ) {
    GTK3M3DUITOOLBAR *gtk3tb = ( GTK3M3DUITOOLBAR * ) user_data;
    M3DUI *mui = ( M3DUI * ) gtk3tb->core.mui;
    G3DUI *gui = ( G3DUI * ) mui->gui;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( gui->sce );

    if ( obj ) {
        if ( obj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            /*** try the first texture in case no texture is selected ***/
            if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

            if ( tex ) {
                G3DMATERIAL *mat = tex->mat;
                uint32_t chnID = GETCHANNEL(mui->engine_flags);
                G3DCHANNEL  *chn = g3dmaterial_getChannelByID ( mat, chnID );

                gtk3_g3dui_saveChannelAlteredImage ( gui,
                                                     mat->name,
                                                     chn,
                                                     0x00,
                                                     0x01 );
            }
        }
    }
}

/******************************************************************************/
static void saveImageCbk ( GtkWidget *widget, 
                           gpointer   user_data ) {
    GTK3M3DUITOOLBAR *gtk3tb = ( GTK3M3DUITOOLBAR * ) user_data;
    M3DUI *mui = ( M3DUI * ) gtk3tb->core.mui;

    gtk3_m3dui_saveimage ( ( GTK3M3DUI * ) mui );
}

/******************************************************************************/
static void unselectAreaCbk ( GtkWidget *widget, 
                              gpointer   user_data ) {
    GTK3M3DUITOOLBAR *gtk3tb = ( GTK3M3DUITOOLBAR * ) user_data;
    M3DUI *mui = ( M3DUI * ) gtk3tb->core.mui;
    G3DUI *gui = ( G3DUI * ) mui->gui;

    /** m3dui_resizeBuffers() calls tool->reset() ***/
    m3dui_resizeBuffers ( mui );

    gtk3_interpretUIReturnFlags ( ( GTK3G3DUI * ) gui, REDRAWUVMAPEDITOR ); 
}

/******************************************************************************/
static GtkWidget *addToolBarToggleButton ( GtkWidget   *bar,
                                           void        *data,
                                           char        *name,
                                           const char **xpm_data,
                                           void       (*cbk) ( GtkWidget *, 
                                                               gpointer ) ) {
    /*GtkIconTheme *icon_theme = gtk_icon_theme_get_default ( );*/
    GtkWidget *btn = (GtkWidget*) ui_gtk_toggle_tool_button_new ( CLASS_MAIN );

    gtk_widget_set_name ( btn, name );

    gtk_widget_set_tooltip_text ( btn, name );

    gtk_widget_set_size_request ( btn, TOOLBARBUTTONSIZE, TOOLBARBUTTONSIZE );

    if ( xpm_data ) {
        GdkPixbuf *xpm_img = gdk_pixbuf_new_from_xpm_data ( xpm_data );

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
static GtkWidget *addToolBarPushButton ( GtkWidget   *bar,
                                         void        *data,
                                         char        *name,
                                         const char **xpm_data,
                                         void       (*cbk) ( GtkWidget *,
                                                             gpointer ) ) {
    /*GtkIconTheme *icon_theme = gtk_icon_theme_get_default ( );*/
    GtkWidget *btn = (GtkWidget*) ui_gtk_tool_button_new ( CLASS_MAIN, NULL, NULL );

    gtk_widget_set_tooltip_text ( btn, name );

    gtk_widget_set_name ( btn, name );

    gtk_widget_set_size_request ( btn, TOOLBARBUTTONSIZE, TOOLBARBUTTONSIZE );

    if ( xpm_data ) {
        GdkPixbuf *xpm_img = gdk_pixbuf_new_from_xpm_data ( xpm_data );

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
GTK3M3DUITOOLBAR *gtk3_m3duitoolbar_create ( GtkWidget *parent,
                                             GTK3M3DUI *gtk3gui,
                                             char      *name ) {
    GTK3M3DUITOOLBAR *gtk3tb = gtk3_g3duitoolbar_new ( gtk3gui );

    gtk3tb->bar = ui_gtk_toolbar_new ( CLASS_MAIN );

    gtk_widget_set_name ( gtk3tb->bar, name );

    gtk_toolbar_set_style(GTK_TOOLBAR(gtk3tb->bar), GTK_TOOLBAR_ICONS);


    gtk3tb->createImageButton    = addToolBarPushButton   ( gtk3tb->bar,
                                                            gtk3tb,
                                                            MENU_CREATEIMAGE,
                                                            newfile_xpm,
                                                            createChannelImageCbk );

    gtk3tb->openFileButton       = addToolBarPushButton   ( gtk3tb->bar,
                                                            gtk3tb,
                                                            MENU_LOADIMAGE,
                                                            openfile_xpm,
                                                            loadImageByChannelIDCbk );

    gtk3tb->saveAsButton         = addToolBarPushButton   ( gtk3tb->bar,
                                                            gtk3tb,
                                                            MENU_SAVEFILEAS,
                                                            saveas_xpm,
                                                            saveFileAsCbk );

    gtk3tb->saveButton           = addToolBarPushButton   ( gtk3tb->bar,
                                                            gtk3tb,
                                                            MENU_SAVEFILE,
                                                            save_xpm,
                                                            saveFileCbk );

    gtk3tb->saveImageAsButton    = addToolBarPushButton   ( gtk3tb->bar,
                                                            gtk3tb,
                                                            MENU_SAVEIMAGEAS,
                                                            saveimageas_xpm,
                                                            saveImageAsCbk );

    gtk3tb->saveImageButton      = addToolBarPushButton   ( gtk3tb->bar,
                                                            gtk3tb,
                                                            MENU_SAVEIMAGE,
                                                            saveimage_xpm,
                                                            saveImageCbk );

    gtk3tb->undoButton           = addToolBarPushButton   ( gtk3tb->bar,
                                                            gtk3tb,
                                                            MENU_UNDO,
                                                            undo_xpm,
                                                            undoCbk );

    gtk3tb->redoButton           = addToolBarPushButton   ( gtk3tb->bar,
                                                            gtk3tb,
                                                            MENU_REDO,
                                                            redo_xpm,
                                                            redoCbk );

    gtk3tb->pickUVToggle         = addToolBarToggleButton ( gtk3tb->bar,
                                                            gtk3tb,
                                                            PICKUVTOOL,
                                                            pick_xpm,
                                                            setMouseToolCbk );

    gtk3tb->moveUVToggle         = addToolBarToggleButton ( gtk3tb->bar,
                                                            gtk3tb,
                                                            MOVEUVTOOL,
                                                            move_xpm,
                                                            setMouseToolCbk );

    gtk3tb->scaleUVToggle        = addToolBarToggleButton ( gtk3tb->bar,
                                                            gtk3tb,
                                                            SCALEUVTOOL,
                                                            scale_xpm,
                                                            setMouseToolCbk );

    gtk3tb->rotateUVToggle       = addToolBarToggleButton ( gtk3tb->bar,
                                                            gtk3tb,
                                                            ROTATEUVTOOL,
                                                            rotate_xpm,
                                                            setMouseToolCbk );

    gtk3tb->selectSquareToggle   = addToolBarToggleButton ( gtk3tb->bar,
                                                            gtk3tb,
                                                            SELECTTOOL,
                                                            selectarea_xpm,
                                                            setSquareSelectorCbk );

    gtk3tb->selectRandomToggle   = addToolBarToggleButton ( gtk3tb->bar,
                                                            gtk3tb,
                                                            SELECTTOOL,
                                                            selectrandom_xpm,
                                                            setRandomSelectorCbk );

    gtk3tb->unselectButton       = addToolBarPushButton   ( gtk3tb->bar,
                                                            gtk3tb,
                                                            MENU_UNSELECTAREA,
                                                            unselectarea_xpm,
                                                            unselectAreaCbk );

    gtk3tb->penToggle            = addToolBarToggleButton ( gtk3tb->bar,
                                                            gtk3tb,
                                                            PENTOOL,
                                                            pen_xpm,
                                                            setMouseToolCbk );

    gtk3tb->bucketToggle         = addToolBarToggleButton ( gtk3tb->bar,
                                                            gtk3tb,
                                                            BUCKETTOOL,
                                                            bucket_xpm,
                                                            setMouseToolCbk );

    gtk3tb->eraserToggle         = addToolBarToggleButton ( gtk3tb->bar,
                                                            gtk3tb,
                                                            ERASERTOOL,
                                                            eraser_xpm,
                                                            setMouseToolCbk );

    gtk_toolbar_set_show_arrow ( GTK_TOOLBAR(gtk3tb->bar), TRUE );

    gtk_widget_show ( gtk3tb->bar );


    return gtk3tb;
}
