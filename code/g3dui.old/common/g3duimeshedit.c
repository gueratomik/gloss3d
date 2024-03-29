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
void common_g3duimeshedit_useIsoLinesCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = ( sce ) ? g3dscene_getSelectedObject ( sce ) : NULL;

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( obj ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3dui_setHourGlass ( gui );

        if ( obj->flags & MESHUSEISOLINES ) {
            obj->flags &= (~MESHUSEISOLINES);
        } else {
            obj->flags |= MESHUSEISOLINES;
        }

        /*** rebuild using adaptive subdivision (or not) ***/
        g3dmesh_update ( mes, gui->engine_flags );

        g3dui_unsetHourGlass ( gui );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3duimeshedit_subdivRenderCbk ( G3DUI *gui, int level ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = ( sce ) ? g3dscene_getSelectedObject ( sce ) : NULL;

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3dui_setHourGlass ( gui );

        /*mes->subdiv_render = level;*/


        g3dui_unsetHourGlass ( gui );
    }

    g3dui_redrawGLViews ( gui );
}

/******************************************************************************/
void common_g3duimeshedit_gouraudCbk ( G3DUI *gui, float scalarLimit ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = ( sce ) ? g3dscene_getSelectedObject ( sce ) : NULL;

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    g3dui_setHourGlass ( gui );

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        mes->gouraudScalarLimit = cos ( scalarLimit );
    }

    g3dui_unsetHourGlass ( gui );

    g3dui_redrawGLViews ( gui );
}

/******************************************************************************/
void common_g3duimeshedit_toggleShadingCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = ( sce ) ? g3dscene_getSelectedObject ( sce ) : NULL;

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    g3dui_setHourGlass ( gui );

    if ( obj ) {
        if ( obj->flags & OBJECTNOSHADING ) {
            obj->flags &= (~OBJECTNOSHADING);
        } else {
            obj->flags |= OBJECTNOSHADING;
        }
    }

    g3dui_unsetHourGlass ( gui );

    g3dui_redrawGLViews ( gui );
}


/******************************************************************************/
void common_g3duimeshedit_subdivLevelCbk ( G3DUI *gui, int level ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = ( sce ) ? g3dscene_getSelectedObject ( sce ) : NULL;

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    g3dui_setHourGlass ( gui );

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        /*g3dmesh_setSubdivisionLevel ( mes, level, gui->engine_flags );*/
    }

    g3dui_unsetHourGlass ( gui );

    g3dui_redrawGLViews ( gui );
}
