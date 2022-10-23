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
#define FOR_EACH_SELECTED_MESH                             \
    G3DUI *gui = mesedit->gui;                             \
    G3DSCENE *sce = gui->sce;                              \
    LIST *ltmpselobj = sce->lsel;                          \
                                                           \
    while ( ltmpselobj ) {                                 \
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data; \
                                                           \
        if ( sel->type == G3DMESHTYPE ) {                  \
            G3DMESH *mes = ( G3DMESH * ) sel;              \


#define END_FOR                                            \
        }                                                  \
                                                           \
        ltmpselobj = ltmpselobj->next;                     \
    }


/******************************************************************************/
uint64_t g3duimeshedit_useIsoLines ( G3DUIMESHEDIT *mesedit ) {
FOR_EACH_SELECTED_MESH
    if ( mes->obj.flags & MESHUSEISOLINES ) {
        mes->obj.flags &= (~MESHUSEISOLINES);
    } else {
        mes->obj.flags |= MESHUSEISOLINES;
    }

    /*** rebuild using adaptive subdivision (or not) ***/
    g3dmesh_update ( mes, mesedit->gui->engine_flags );
END_FOR

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duimeshedit_gouraud ( G3DUIMESHEDIT *mesedit,
                                 float          scalarLimit ) {
FOR_EACH_SELECTED_MESH
    mes->gouraudScalarLimit = cos ( scalarLimit );
END_FOR

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duimeshedit_toggleShading ( G3DUIMESHEDIT *mesedit ) {
FOR_EACH_SELECTED_MESH
    if ( mes->obj.flags & OBJECTNOSHADING ) {
        mes->obj.flags &= (~OBJECTNOSHADING);
    } else {
        mes->obj.flags |= OBJECTNOSHADING;
    }
END_FOR

    return REDRAWVIEW;
}
