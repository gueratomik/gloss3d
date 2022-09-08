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
static uint64_t deleteKeysCbk ( G3DUIMENU *menu, 
                                void      *data ) {
    G3DUITIMELINE tim = { .gui = menu->gui };

    g3duitimeline_deleteSelectedKeys ( &tim );


    return REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t scaleKeysDialogCbk ( G3DUIMENU *menu, 
                                     void      *data ) {
/***
    G3DUITIMELINE *tim = ( G3DUITIMELINE * ) user_data;
    GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );

    createScaleKeysDialog ( dial,
                            tim->grp.gui,
                            "Scale Keys",
                            0,
                            0,
                            150,
                            96 );

    gtk_widget_show ( dial );
*/
    return 0x00;
}

/******************************************************************************/
static uint64_t scaleKeysCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    G3DUITIMELINE *tim = ( G3DUITIMELINE * ) data;

    tim->tool = TIME_SCALE_TOOL;


    return 0x00;
}

/******************************************************************************/
static uint64_t selectKeysCbk ( G3DUIMENU *menu, 
                                void      *data ) {
    G3DUITIMELINE *tim = ( G3DUITIMELINE * ) data;

    tim->tool = TIME_MOVE_TOOL;


    return 0x00;
}

/******************************************************************************/
static G3DUIMENU time_menu_delete = { NULL,
                                      TIMEMENU_DELETE,
                                      MENU_CLASS_MAIN,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      deleteKeysCbk };

static G3DUIMENU time_menu_scale = { NULL,
                                     TIMEMENU_SCALE,
                                     MENU_CLASS_MAIN,
                                     G3DUIMENUTYPE_PUSHBUTTON,
                                     NULL,
                                     scaleKeysDialogCbk };

static G3DUIMENU time_menu_select = { NULL,
                                      TIMEMENU_SELECT,
                                      MENU_CLASS_MAIN,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      selectKeysCbk };

/******************************************************************************/
static G3DUIMENU *timechildren[] = { /*&time_menu_select,*/
                                         &time_menu_scale,
                                         &time_menu_delete,
                                          NULL };

static G3DUIMENU time_menu = { NULL,
                               "_Options",
                               MENU_CLASS_MAIN,
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = timechildren };

/******************************************************************************/
G3DUIMENU *g3duimenu_getTimelineMenuNode ( ) {
    return &time_menu;
}
