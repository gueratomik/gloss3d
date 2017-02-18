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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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
void common_g3dui_setMaterialCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
    G3DMATERIAL *mat = gui->selmat;

    if ( obj && mat ) {
        if ( ( obj->type == G3DMESHTYPE ) ||
           ( ( obj->type &  G3DPRIMITIVETYPE ) == G3DPRIMITIVETYPE ) ) {
            G3DMESH  *mes = ( G3DMESH * ) obj;
            G3DUVMAP *map = g3dmesh_getLastUVMap ( mes );

            /*** A textured mesh should ALWAYS have a UVMAP.     ***/
            /*** Likely design issue - to FIX when I have time ***/
            if ( map == NULL ) {
                map = g3duvmap_new ( "UVMap", 0x00, UVMAPFLAT );

                g3dmesh_addUVMap ( mes, map, gui->flags );
            }

            g3dmesh_addMaterial ( ( G3DMESH * ) mes, mat, NULL, map );
        } else {
            if ( obj->type == G3DUVMAPTYPE ){
                G3DMESH  *mes = ( G3DMESH * ) obj->parent;
                G3DUVMAP *map = ( G3DUVMAP* ) obj;

                g3dmesh_addMaterial ( ( G3DMESH * ) mes, mat, NULL, map );
            } else {
                fprintf ( stderr, "No material selected\n" );
            }
        }
    }

    g3dui_redrawGLViews ( gui );
}

