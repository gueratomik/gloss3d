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
static uint32_t fitViewActiveCond ( G3DUIMENU *menu,
                                    void      *data ) {
    G3DUIRENDERWINDOW *rwin = ( G3DUIRENDERWINDOW * ) data;

    return ( rwin->displayMode == RENDERWINDOW_FIT_SIZE ) ? MENU_CONDITION_SENSITIVE |
                                                            MENU_CONDITION_ACTIVE : 
                                                            MENU_CONDITION_SENSITIVE;
}

/******************************************************************************/
static uint32_t fullViewActiveCond ( G3DUIMENU *menu,
                                     void      *data ) {
    G3DUIRENDERWINDOW *rwin = ( G3DUIRENDERWINDOW * ) data;

    return ( rwin->displayMode == RENDERWINDOW_FULL_SIZE ) ? MENU_CONDITION_SENSITIVE |
                                                             MENU_CONDITION_ACTIVE : 
                                                             MENU_CONDITION_SENSITIVE;
}

/******************************************************************************/
/*** Note: alos treated in gtk3/g3duimenubar.c/menuItemCallback() ***/
static uint64_t rwViewFitCbk ( G3DUIMENU *menu, void *data ) {
    G3DUIRENDERWINDOW *rwin = ( G3DUIRENDERWINDOW * ) data;

    rwin->displayMode = RENDERWINDOW_FIT_SIZE;

    return RESIZERENDERWINDOW | UPDATERENDERWINDOWMENU;
}

/******************************************************************************/
/*** Note: alos treated in gtk3/g3duimenubar.c/menuItemCallback() ***/
static uint64_t rwViewFullCbk ( G3DUIMENU *menu, void *data ) {
    G3DUIRENDERWINDOW *rwin = ( G3DUIRENDERWINDOW * ) data;

    rwin->displayMode = RENDERWINDOW_FULL_SIZE;

    return RESIZERENDERWINDOW | UPDATERENDERWINDOWMENU;
}

/******************************************************************************/

static G3DUIMENU rw_view_fit   = { NULL,
                                   RWMENU_VIEW_FIT,
                                   MENU_CLASS_MAIN,
                                   G3DUIMENUTYPE_TOGGLEBUTTON,
                                   fitViewActiveCond,
                                   rwViewFitCbk };

static G3DUIMENU rw_view_full = { NULL,
                                  RWMENU_VIEW_FULL,
                                  MENU_CLASS_MAIN,
                                  G3DUIMENUTYPE_TOGGLEBUTTON,
                                  fullViewActiveCond,
                                  rwViewFullCbk };

/******************************************************************************/

static G3DUIMENU *rwviewchildren[] = { &rw_view_fit,
                                       &rw_view_full,
                                        NULL };

static G3DUIMENU rw_view_menu = { NULL,
                                  "_View",
                                  MENU_CLASS_MAIN,
                                  G3DUIMENUTYPE_SUBMENU,
                                  NULL,
                                  NULL,
                                 .nodes = rwviewchildren };

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
                                     MENU_CLASS_MAIN,
                                     G3DUIMENUTYPE_PUSHBUTTON,
                                     NULL,
                                     renderwindowExitCbk };

/*static G3DUIMENU rw_menu_savepng = { NULL,
                                     RWMENU_SAVEPNG,
                                     MENU_CLASS_MAIN,
                                     G3DUIMENUTYPE_PUSHBUTTON,
                                     NULL,
                                     g3duirenderwindow_savePNGCbk };*/

static G3DUIMENU rw_menu_savejpg = { NULL,
                                     RWMENU_SAVEJPG,
                                     MENU_CLASS_MAIN,
                                     G3DUIMENUTYPE_PUSHBUTTON,
                                     NULL,
                                     renderwindowSaveJPGCbk };

/******************************************************************************/
static G3DUIMENU *rwfilechildren[] = { /*&rw_menu_savepng,*/
                                            &rw_menu_savejpg,
                                            &rw_menu_close,
                                             NULL };

static G3DUIMENU rw_file_menu = { NULL,
                                  "_File",
                                  MENU_CLASS_MAIN,
                                  G3DUIMENUTYPE_SUBMENU,
                                  NULL,
                                  NULL,
                                 .nodes = rwfilechildren };

/******************************************************************************/
/******************************************************************************/
static G3DUIMENU *renderwindowchildren[] = { &rw_file_menu,
                                             &rw_view_menu,
                                              NULL };

static G3DUIMENU renderwindowrootnode = { NULL,
                                          "Bar",
                                          NULL,
                                          G3DUIMENUTYPE_MENUBAR,
                                          NULL,
                                          NULL,
                                         .nodes = renderwindowchildren };

/******************************************************************************/
G3DUIMENU *g3duimenu_getRenderWindowMenuNode ( ) {
    return &renderwindowrootnode;
}
