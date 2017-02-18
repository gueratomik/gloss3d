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
void common_g3duiuvmap_projectionCbk ( G3DUI *gui, const char *projection ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type == G3DUVMAPTYPE ) ) {
        G3DUVMAP *map = ( G3DUVMAP * ) obj;

        /*** 0 ***/
        if ( strcmp ( projection, FLATPROJECTION        ) == 0x00 ) {
            map->projection = UVMAPFLAT;
        }

        /*** 1 ***/
        if ( strcmp ( projection, SPHERICALPROJECTION   ) == 0x00 ) {
            map->projection = UVMAPSPHERICAL;
        }

        /*** 2 ***/
        if ( strcmp ( projection, CYLINDRICALPROJECTION ) == 0x00 ) {
            map->projection = UVMAPCYLINDRICAL;
        }

        g3dobject_updateMatrix_r ( ( G3DOBJECT * ) map, gui->flags );
        /*g3duvmap_applyProjection ( map );*/

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3duiuvmap_lockUVMapCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type == G3DUVMAPTYPE ) ) {
        G3DUVMAP *map = ( G3DUVMAP * ) obj;

        if ( obj->flags & UVMAPFIXED ) {
            g3duvmap_unfix ( map );
        } else {
            g3duvmap_fix   ( map );
        }
    }
}
