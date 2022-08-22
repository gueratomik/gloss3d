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
    g3dui_setMaterialCbk ( menu->gui );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t removeMaterialCbk ( G3DUIMENU *menu, 
                                    void      *data ) {
#ifdef TODO
    G3DUI *gui = ( G3DUI * ) user_data;
    LIST *ltmpmatlist = gui->lmatlist;

    if ( gui->selmat ) {
        while ( ltmpmatlist ) {
            GtkWidget *matlst = ( GtkWidget * ) ltmpmatlist->data;

            g3duimateriallist_removeMaterial ( matlst, gui->sce, 
                                                       gui->urm,
                                                       gui->selmat );

            gui->selmat = NULL;


            ltmpmatlist = ltmpmatlist->next;
        }
    }
#endif

    return REDRAWVIEW | REDRAWMATERIALLIST | REDRAWCURRENTMATERIAL;
}

/******************************************************************************/
static uint64_t addMaterialCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
#ifdef TODO
    G3DMATERIAL *mat = g3dmaterial_new ( "Material" );
    G3DUI *gui = ( G3DUI * ) user_data;
    LIST *ltmpmatlist = gui->lmatlist;

    g3durm_scene_addMaterial ( gui->urm,
                               gui->sce, 
                               mat,
                               gui->engine_flags,
                               REBUILDMATERIALLIST );

    while ( ltmpmatlist ) {
        GtkWidget *matlst = ( GtkWidget * ) ltmpmatlist->data;

        g3duimateriallist_addPreview ( matlst, mat );

        ltmpmatlist = ltmpmatlist->next;
    }

    gui->selmat = mat;
#endif


    return REDRAWMATERIALLIST | REDRAWCURRENTMATERIAL;
}

/******************************************************************************/
static G3DUIMENU matfile_menu_add    = { NULL,
                                         "Add Material",
                                         G3DUIMENUTYPE_PUSHBUTTON,
                                         NULL,
                                         addMaterialCbk };

static G3DUIMENU matfile_menu_remove = { NULL,
                                         "Remove Material",
                                         G3DUIMENUTYPE_PUSHBUTTON,
                                         NULL,
                                         removeMaterialCbk };

static G3DUIMENU matfile_menu_set    = { NULL,
                                         "Set Material",
                                         G3DUIMENUTYPE_PUSHBUTTON,
                                         NULL,
                                         setMaterialCbk };

/******************************************************************************/
static G3DUIMENU matfile_menu = { NULL,
                                  "File",
                                  G3DUIMENUTYPE_SUBMENU,
                                  NULL,
                                  NULL,
                                 .nodes = { &matfile_menu_add,
                                            &matfile_menu_remove,
                                            &matfile_menu_set,
                                             NULL } };

/******************************************************************************/
/******************************************************************************/
static G3DUIMENU matrootnode = { NULL,
                                 "Bar",
                                 G3DUIMENUTYPE_MENUBAR,
                                 NULL,
                                 NULL,
                                .nodes = { &matfile_menu,
                                            NULL } };

/******************************************************************************/
G3DUIMENU *g3duimenu_getMaterialBoardMenuNode ( ) {
    return &matrootnode;
}
