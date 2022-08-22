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
#include <g3dui.h>

/******************************************************************************/
static uint64_t renderwindowExitCbk ( G3DUIMENU *menu, void *data ) {
/***
    G3DUIRENDERWINDOW *grw = ( G3DUIRENDERWINDOW * ) user_data;

    gtk_widget_destroy ( grw->topLevel );
*/
    return 0x00;
}

/******************************************************************************/
static uint64_t renderwindowSaveJPGCbk ( G3DUIMENU *menu, void *data ) {

    return 0x00;
}

/******************************************************************************/
static G3DUIMENU rw_menu_close   = { NULL,
                                     RWMENU_CLOSE,
                                     G3DUIMENUTYPE_PUSHBUTTON,
                                     NULL,
                                     renderwindowExitCbk };

/*static G3DUIMENU rw_menu_savepng = { NULL,
                                     RWMENU_SAVEPNG,
                                     G3DUIMENUTYPE_PUSHBUTTON,
                                     NULL,
                                     g3duirenderwindow_savePNGCbk };*/

static G3DUIMENU rw_menu_savejpg = { NULL,
                                     RWMENU_SAVEJPG,
                                     G3DUIMENUTYPE_PUSHBUTTON,
                                     NULL,
                                     renderwindowSaveJPGCbk };

/******************************************************************************/
static G3DUIMENU rw_file_menu = { NULL,
                                  "_File",
                                  G3DUIMENUTYPE_SUBMENU,
                                  NULL,
                                  NULL,
                                 .nodes = { /*&rw_menu_savepng,*/
                                            &rw_menu_savejpg,
                                            &rw_menu_close,
                                             NULL } };

/******************************************************************************/
/******************************************************************************/
static G3DUIMENU renderwindowrootnode = { NULL,
                                          "Bar",
                                          G3DUIMENUTYPE_MENUBAR,
                                          NULL,
                                          NULL,
                                         .nodes = { &rw_file_menu,
                                                     NULL } };

/******************************************************************************/
G3DUIMENU *g3duimenu_getRenderWindowMenuNode ( ) {
    return &renderwindowrootnode;
}
