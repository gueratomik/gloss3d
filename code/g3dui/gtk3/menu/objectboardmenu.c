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
/**************************** Tags MENU Callbacks *****************************/

static uint64_t addVibratorTagCbk ( G3DUIMENU *menu, 
                                    void      *data ) {
    g3dui_addVibratorTag ( menu->gui );
}

/******************************************************************************/
static uint64_t addTrackerTagCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_addTrackerTag ( menu->gui );
}


/******************************************************************************/
static uint64_t removeSelectedTagCbk ( G3DUIMENU *menu, 
                                       void      *data ) {
    g3dui_removeSelectedTag ( menu->gui );
}

/******************************************************************************/
/******************************** Tags MENU ***********************************/

static G3DUIMENU tags_menu_addVibrator     = { NULL,
                                               MENU_ADDVIBRATORTAG,
                                               MENU_CLASS_MAIN,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectMode_objectSelected,
                                               addVibratorTagCbk };

static G3DUIMENU tags_menu_addTrackerTag   = { NULL,
                                               MENU_ADDTRACKERTAG,
                                               MENU_CLASS_MAIN,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectMode_objectSelected,
                                               addTrackerTagCbk };

static G3DUIMENU tags_menu_removeSelTag    = { NULL,
                                               MENU_REMOVESELTAG,
                                               MENU_CLASS_MAIN,
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
                               MENU_CLASS_MAIN,
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = tagschildren };

/******************************************************************************/
/******************************************************************************/

static uint64_t makeupCbk ( G3DUIMENU *menu, 
                            void      *data ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) menu->gui;
    GTK3M3DUI *gtk3mui = ( GTK3M3DUI * ) gtk3gui->core.mui;

    if ( gtk3mui == NULL ) {
        gtk3mui = gtk3_m3dui_create ( gtk3gui );

        gtk3gui->core.mui = ( M3DUI * ) gtk3mui;

        gtk3_m3dui_display ( gtk3mui );
    } else {
        gtk_widget_show ( gtk3mui->topWin );
    }
}
/******************************************************************************/
/******************************************************************************/
static G3DUIMENU uvw_menu_makeup = { NULL,
                                     MENU_MAKEUP,
                                     MENU_CLASS_MAIN,
                                     G3DUIMENUTYPE_PUSHBUTTON,
                                     object_objectSelectedAndUVFixed,
                                     makeupCbk };

/******************************************************************************/
static G3DUIMENU *uvwchildren[] = { &uvw_menu_makeup,
                                     NULL };

/******************************************************************************/
static G3DUIMENU uvw_menu = { NULL,
                              "Texturing",
                              MENU_CLASS_MAIN,
                              G3DUIMENUTYPE_SUBMENU,
                              NULL,
                              NULL,
                             .nodes = uvwchildren };

/******************************************************************************/
/******************************************************************************/
static G3DUIMENU *objrootchildren[] = { &uvw_menu,
                                        &tags_menu,
                                         NULL };

static G3DUIMENU objrootnode = { NULL,
                                 "Bar",
                                 MENU_CLASS_MAIN,
                                 G3DUIMENUTYPE_MENUBAR,
                                 NULL,
                                 NULL,
                                .nodes = objrootchildren };

/******************************************************************************/
G3DUIMENU *g3duimenu_getObjectBoardMenuNode ( ) {
    return &objrootnode;
}
