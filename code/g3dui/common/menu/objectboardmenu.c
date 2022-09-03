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
/**************************** Tags MENU Callbacks *****************************/

static uint64_t addVibratorTagCbk ( G3DUIMENU *menu, 
                                    void      *data ) {
    g3dui_addVibratorTagCbk ( menu->gui );
}

/******************************************************************************/
static uint64_t addTrackerTagCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_addTrackerTagCbk ( menu->gui );
}


/******************************************************************************/
static uint64_t removeSelectedTagCbk ( G3DUIMENU *menu, 
                                       void      *data ) {
    g3dui_removeSelectedTagCbk ( menu->gui );
}

/******************************************************************************/
/******************************** Tags MENU ***********************************/

static G3DUIMENU tags_menu_addVibrator     = { NULL,
                                               MENU_ADDVIBRATORTAG,
                                               MENU_CLASS_VIEW,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectMode_objectSelected,
                                               addVibratorTagCbk };

static G3DUIMENU tags_menu_addTrackerTag   = { NULL,
                                               MENU_ADDTRACKERTAG,
                                               MENU_CLASS_VIEW,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectMode_objectSelected,
                                               addTrackerTagCbk };

static G3DUIMENU tags_menu_removeSelTag    = { NULL,
                                               MENU_REMOVESELTAG,
                                               MENU_CLASS_VIEW,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectMode_objectSelected,
                                               removeSelectedTagCbk };

/******************************************************************************/
static G3DUIMENU *tagschildren[] = { /*&tags_menu_addVibrator,*/
                                         &tags_menu_addTrackerTag,
                                         &tags_menu_removeSelTag,
                                          NULL };

static G3DUIMENU tags_menu = { NULL,
                               "Tags",
                               MENU_CLASS_VIEW,
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = tagschildren };

/******************************************************************************/
/******************************************************************************/
static G3DUIMENU *objrootchildren[] = { &tags_menu,
                                         NULL };

static G3DUIMENU objrootnode = { NULL,
                                 "Bar",
                                 NULL,
                                 G3DUIMENUTYPE_MENUBAR,
                                 NULL,
                                 NULL,
                                .nodes = objrootchildren };

/******************************************************************************/
G3DUIMENU *g3duimenu_getObjectBoardMenuNode ( ) {
    return &objrootnode;
}
