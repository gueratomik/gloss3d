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
GtkWidget *createUVMapEditorToolBar ( GtkWidget *parent, 
                                      G3DUI     *gui,
                                      char      *name,
                                      gint       x,
                                      gint       y,
                                      gint       width,
                                      gint       height ) {
    GtkUVMapEditor *guv = ( GtkUVMapEditor * ) parent;
    G3DUIUVMAPEDITOR *uvme = &guv->uvme;
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *bar = gtk_toolbar_new ( );
    GtkWidget *grp = NULL;

    gtk_widget_set_name ( bar, name );

    gtk_widget_size_allocate ( bar, &gdkrec );

    gtk_toolbar_set_style(GTK_TOOLBAR(bar), GTK_TOOLBAR_ICONS);

   /********************************/

    addToolBarPushButton   ( bar, uvme, MENU_UNDO   , undo_xpm  , g3duiuvmapeditor_undoCbk );

   /********************************/

    addToolBarPushButton   ( bar, uvme, MENU_REDO   , redo_xpm  , g3duiuvmapeditor_redoCbk );

   /********************************/

    addToolBarToggleButton ( bar, uvme, PICKUVTOOL  , pick_xpm  , g3duiuvmapeditor_setUVMouseTool );
 
    /********************************/

    addToolBarToggleButton ( bar, uvme, MOVEUVTOOL  , move_xpm  , g3duiuvmapeditor_setUVMouseTool );
 
    /********************************/

    addToolBarToggleButton ( bar, uvme, SCALEUVTOOL , scale_xpm , g3duiuvmapeditor_setUVMouseTool );

    /********************************/

    addToolBarToggleButton ( bar, uvme, SELECTAREATOOL, selectarea_xpm, g3duiuvmapeditor_setUVMouseTool );

    /********************************/

    addToolBarToggleButton ( bar, uvme, SELECTRANDOMTOOL, selectrandom_xpm, g3duiuvmapeditor_setUVMouseTool );

    /********************************/

    addToolBarToggleButton ( bar, uvme, PENTOOL, pen_xpm, g3duiuvmapeditor_setUVMouseTool );

    /********************************/

    addToolBarToggleButton ( bar, uvme, BUCKETTOOL, bucket_xpm, g3duiuvmapeditor_setUVMouseTool );

    /********************************/

    addToolBarToggleButton ( bar, uvme, ERASERTOOL, eraser_xpm, g3duiuvmapeditor_setUVMouseTool );



    gtk_toolbar_set_show_arrow ( GTK_TOOLBAR(bar), 0 );

    gtk_fixed_put ( GTK_FIXED(parent), bar, x, y );

    gtk_widget_show ( bar );


    return bar;
}
