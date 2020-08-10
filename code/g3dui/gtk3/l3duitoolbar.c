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

/******************************************************************************/
static void setSquareSelectorCbk ( GtkWidget *widget, 
                                   gpointer   user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;
    G3DUI *gui = ( G3DUI * ) lui->gui;
    L3DMOUSETOOL *uvmou = common_g3dui_getMouseTool ( gui, SELECTTOOL );

    if ( gui->lock ) return;

    if ( uvmou ) {
        l3dselector_setMode ( ( L3DSELECTOR * ) uvmou->obj, SELECTORMODESQUARE );

        l3dui_setUVMouseTool ( widget, user_data );
    }
}

/******************************************************************************/
static void setRandomSelectorCbk ( GtkWidget *widget, 
                                   gpointer   user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;
    G3DUI *gui = ( G3DUI * ) lui->gui;
    L3DMOUSETOOL *uvmou = common_g3dui_getMouseTool ( gui, SELECTTOOL );

    if ( gui->lock ) return;

    if ( uvmou ) {
        l3dselector_setMode ( ( L3DSELECTOR * ) uvmou->obj, SELECTORMODERANDOM );

        l3dui_setUVMouseTool ( widget, user_data );
    }
}

/******************************************************************************/
void l3dui_saveasCbk ( GtkWidget *widget, 
                                  gpointer   user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;
    G3DUI *gui = ( G3DUI * ) lui->gui;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( gui->sce );

    if ( obj ) {
        if ( obj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            /*** try the first texture in case no texture is selected ***/
            if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

            if ( tex ) {
                G3DMATERIAL *mat = tex->mat;
                uint32_t chnID = GETCHANNEL(lui->engine_flags);
                G3DCHANNEL  *chn = g3dmaterial_getChannelByID ( mat, chnID );

                g3dui_saveChannelAlteredImage ( gui,
                                                mat->name,
                                                chn,
                                                0x00,
                                                0x01 );
            }
        }
    }
}

/******************************************************************************/
void l3dui_saveCbk ( GtkWidget *widget, 
                                gpointer   user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;
    G3DUI *gui = ( G3DUI * ) lui->gui;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( gui->sce );

    if ( obj ) {
        if ( obj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            /*** try the first texture in case no texture is selected ***/
            if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

            if ( tex ) {
                G3DMATERIAL *mat = tex->mat;
                uint32_t chnID = GETCHANNEL(lui->engine_flags);
                G3DCHANNEL  *chn = g3dmaterial_getChannelByID ( mat, chnID );

                g3dui_saveChannelAlteredImage ( gui,
                                                mat->name,
                                                chn,
                                                0x00,
                                                0x00 );
            }
        }
    }
}

/******************************************************************************/
GtkWidget *createUVMapEditorToolBar ( GtkWidget *parent, 
                                      G3DUI     *gui,
                                      char      *name,
                                      gint       x,
                                      gint       y,
                                      gint       width,
                                      gint       height ) {
    GtkUVMapEditor *guv = ( GtkUVMapEditor * ) parent;
    L3DUI *lui = &guv->lui;
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *bar = gtk_toolbar_new ( );
    GtkWidget *grp = NULL;

    gtk_widget_set_name ( bar, name );

    gtk_widget_size_allocate ( bar, &gdkrec );

    gtk_toolbar_set_style(GTK_TOOLBAR(bar), GTK_TOOLBAR_ICONS);

    addToolBarPushButton   ( bar, lui, MENU_NEWSCENE  , newfile_xpm, l3dui_createChannelImageCbk );

   /********************************/

    addToolBarPushButton   ( bar, lui, MENU_OPENFILE  , openfile_xpm, l3dui_loadImageByChannelIDCbk );

   /********************************/

    addToolBarPushButton   ( bar, lui, MENU_SAVEFILEAS, saveas_xpm, l3dui_saveasCbk   );

   /********************************/

    addToolBarPushButton   ( bar, lui, MENU_SAVEFILE  , save_xpm  , l3dui_saveCbk );

   /********************************/

    addToolBarPushButton   ( bar, lui, MENU_UNDO     , undo_xpm  , l3dui_undoCbk );

   /********************************/

    addToolBarPushButton   ( bar, lui, MENU_REDO     , redo_xpm  , l3dui_redoCbk );

   /********************************/

    addToolBarToggleButton ( bar, lui, PICKUVTOOL    , pick_xpm  , l3dui_setUVMouseTool );
 
    /********************************/

    addToolBarToggleButton ( bar, lui, MOVEUVTOOL    , move_xpm  , l3dui_setUVMouseTool );
 
    /********************************/

    addToolBarToggleButton ( bar, lui, SCALEUVTOOL   , scale_xpm , l3dui_setUVMouseTool );

    /********************************/

    addToolBarToggleButton ( bar, lui, ROTATEUVTOOL  , rotate_xpm , l3dui_setUVMouseTool );

    /********************************/

    addToolBarToggleButton ( bar, lui, SELECTTOOL, selectarea_xpm, setSquareSelectorCbk );

    /********************************/

    addToolBarToggleButton ( bar, lui, SELECTTOOL, selectrandom_xpm, setRandomSelectorCbk );

    /********************************/

    addToolBarToggleButton ( bar, lui, PENTOOL, pen_xpm, l3dui_setUVMouseTool );

    /********************************/

    addToolBarToggleButton ( bar, lui, BUCKETTOOL, bucket_xpm, l3dui_setUVMouseTool );

    /********************************/

    addToolBarToggleButton ( bar, lui, ERASERTOOL, eraser_xpm, l3dui_setUVMouseTool );



    gtk_toolbar_set_show_arrow ( GTK_TOOLBAR(bar), 0 );

    gtk_fixed_put ( GTK_FIXED(parent), bar, x, y );

    gtk_widget_show ( bar );


    return bar;
}
