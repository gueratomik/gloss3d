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
static uint64_t setMaterialCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    g3dui_setMaterial ( menu->gui );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t removeMaterialCbk ( G3DUIMENU *menu, 
                                    void      *data ) {
    if ( menu->gui->lselmat ) {
        /*** TODO: multiple removal ***/
        G3DMATERIAL *mat = ( G3DMATERIAL * ) menu->gui->lselmat->data;

        g3durm_scene_removeMaterial ( menu->gui->urm, 
                                      menu->gui->sce, 
                                      mat, 
                                      0x00,
                                      UPDATEMATERIALLIST | 
                                      REDRAWMATERIALLIST |
                                      UPDATECURRENTMATERIAL );
    }

    return UPDATEMATERIALLIST   |
           REDRAWMATERIALLIST   | 
           UPDATECURRENTMATERIAL;
}

/******************************************************************************/
static uint64_t addMaterialCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    G3DMATERIAL *mat = g3dmaterial_new ( "Material" );

    g3durm_scene_addMaterial ( menu->gui->urm,
                               menu->gui->sce, 
                               mat,
                               menu->gui->engine_flags,
                               UPDATEMATERIALLIST | 
                               REDRAWMATERIALLIST |
                               UPDATECURRENTMATERIAL );

    /*** unselect all materials first ***/
    list_free ( &menu->gui->lselmat, g3dmaterial_unsetSelected );

    list_insert ( &menu->gui->lselmat, mat );

    g3dmaterial_setSelected ( mat );

    return UPDATEMATERIALLIST   |
           REDRAWMATERIALLIST   | 
           UPDATECURRENTMATERIAL;
}

/******************************************************************************/
static G3DUIMENU matfile_menu_add    = { NULL,
                                         "Add Material",
                                         MENU_CLASS_MAIN,
                                         G3DUIMENUTYPE_PUSHBUTTON,
                                         NULL,
                                         addMaterialCbk };

static G3DUIMENU matfile_menu_remove = { NULL,
                                         "Remove Material",
                                         MENU_CLASS_MAIN,
                                         G3DUIMENUTYPE_PUSHBUTTON,
                                         NULL,
                                         removeMaterialCbk };

static G3DUIMENU matfile_menu_set    = { NULL,
                                         "Set Material",
                                         MENU_CLASS_MAIN,
                                         G3DUIMENUTYPE_PUSHBUTTON,
                                         NULL,
                                         setMaterialCbk };

/******************************************************************************/
static G3DUIMENU *matfilechildren[] = { &matfile_menu_add,
                                            &matfile_menu_remove,
                                            &matfile_menu_set,
                                             NULL };

static G3DUIMENU matfile_menu = { NULL,
                                  "File",
                                  MENU_CLASS_MAIN,
                                  G3DUIMENUTYPE_SUBMENU,
                                  NULL,
                                  NULL,
                                 .nodes = matfilechildren };

/******************************************************************************/
/******************************************************************************/
static G3DUIMENU *matrootchildren[] = { &matfile_menu,
                                            NULL };

static G3DUIMENU matrootnode = { NULL,
                                 "Bar",
                                 MENU_CLASS_MAIN,
                                 G3DUIMENUTYPE_MENUBAR,
                                 NULL,
                                 NULL,
                                .nodes = matrootchildren };

/******************************************************************************/
G3DUIMENU *g3duimenu_getMaterialBoardMenuNode ( ) {
    return &matrootnode;
}
