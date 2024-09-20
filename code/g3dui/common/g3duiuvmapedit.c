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
uint64_t g3duiuvmapedit_projection ( G3DUIUVMAPEDIT *uvedit,
                                     const char     *projection ) {
    G3DUI *gui = uvedit->gui;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = uvedit->obj;

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );

        if ( ( ((G3DOBJECT*)uvmap)->flags & UVMAPFIXED ) == 0x00 ) {
            /*** 0 ***/
            if ( strcmp ( projection, FLATPROJECTION        ) == 0x00 ) {
                uvmap->projection = UVMAPFLAT;
            }

            /*** 1 ***/
            if ( strcmp ( projection, SPHERICALPROJECTION   ) == 0x00 ) {
                uvmap->projection = UVMAPSPHERICAL;
            }

            /*** 2 ***/
            if ( strcmp ( projection, CYLINDRICALPROJECTION ) == 0x00 ) {
                uvmap->projection = UVMAPCYLINDRICAL;
            }

            /*** 2 ***/
            if ( strcmp ( projection, BACKGROUNDPROJECTION ) == 0x00 ) {
                uvmap->projection = UVMAPBACKGROUND;
            }

            g3duvmap_applyProjection ( uvmap, mes );
/*
            mes->obj.invalidationFlags |= RESETMODIFIERS;
*/
            /*** might need to recompute displacement ***/
            g3dmesh_update ( mes, 0x00, gui->engine_flags );
        }
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duiuvmapedit_lockUVMap ( G3DUIUVMAPEDIT *uvedit ) {
    G3DUI *gui = uvedit->gui;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = uvedit->obj;

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );

        if ( ((G3DOBJECT*)uvmap)->flags & UVMAPFIXED ) {
            g3duvmap_unfix ( uvmap );
        } else {
            g3duvmap_fix   ( uvmap );
        }
    }


    return 0x00;
}
