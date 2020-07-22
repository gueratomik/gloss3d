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
void common_g3dui_undoCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    uint32_t return_value;

    return_value = g3durmanager_undo ( urm, gui->engine_flags );

    common_g3dui_interpretMouseToolReturnFlags ( gui, return_value );

    /*if ( return_value & REDRAWVIEW ) {
        g3dui_redrawGLViews ( gui );
    }

    if ( return_value & REDRAWLIST ) {
        g3dui_redrawObjectList ( gui );
    }

    if ( return_value & REDRAWCURRENTOBJECT ) {
        g3dui_updateAllCurrentEdit ( gui );
    }*/
}

/******************************************************************************/
void common_g3dui_redoCbk ( G3DUI *gui ) {
    G3DURMANAGER *urm = gui->urm;
    uint32_t return_value;

    return_value = g3durmanager_redo ( urm, gui->engine_flags );

    common_g3dui_interpretMouseToolReturnFlags ( gui, return_value );

    /*if ( return_value & REDRAWVIEW ) {
        g3dui_redrawGLViews ( gui );
    }

    if ( return_value & REDRAWLIST ) {
        g3dui_redrawObjectList ( gui );
    }

    if ( return_value & REDRAWCURRENTOBJECT ) {
        g3dui_updateAllCurrentEdit ( gui );
    }*/
}

/******************************************************************************/
void common_g3dui_makeEditableCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    uint32_t oid = g3dscene_getNextObjectID ( sce );

    if ( obj && ( obj->type & PRIMITIVE ) ) {
        g3durm_primitive_convert ( gui->urm, 
                                   gui->sce,
                                   gui->engine_flags, ( G3DPRIMITIVE * ) obj,
                                               ( G3DOBJECT    * ) obj->parent,
                                               ( REDRAWCURRENTOBJECT | 
                                                 REDRAWVIEW          | 
                                                 REDRAWLIST ) );

        g3dui_redrawObjectList     ( gui );
        g3dui_updateAllCurrentEdit ( gui );
        g3dui_redrawGLViews        ( gui );
    }

    if ( obj && ( ( obj->type & MODIFIER         ) ||
                  ( obj->type & TEXT             ) ||
                  ( obj->type == G3DSYMMETRYTYPE ) ) ) {
        G3DOBJECT *commitedObj = g3dobject_commit ( obj, 
                                                    oid, 
                                                    obj->name, 
                                                    gui->engine_flags );

        if ( commitedObj ) {
            g3durm_object_addChild ( gui->urm, 
                                     gui->sce, 
                                     gui->engine_flags, 
                                     ( REDRAWVIEW |
                                       REDRAWLIST | REDRAWCURRENTOBJECT ),
                                     ( G3DOBJECT * ) NULL,
                                     ( G3DOBJECT * ) sce,
                                     ( G3DOBJECT * ) commitedObj );

            g3dscene_unselectAllObjects ( sce, gui->engine_flags );
            g3dscene_selectObject ( sce,
                    ( G3DOBJECT * ) commitedObj, 
                                    gui->engine_flags );

            g3dui_redrawGLViews ( gui );
            g3dui_updateCoords ( gui );
            g3dui_redrawObjectList ( gui );
            g3dui_updateAllCurrentEdit ( gui );
        }
    }
}

/******************************************************************************/
void common_g3dui_pasteSelectionCbk ( G3DUI *gui ) {
    G3DUICLIPBOARD *cli = gui->cli;
    G3DURMANAGER *urm = gui->urm;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *src = g3dscene_getSelectedObject ( sce );
    G3DOBJECT *dst = ( G3DOBJECT * ) sce;

    printf("pasting\n");

    /*** add the copied object to the   ***/
    /*** selected object's parent objet ***/
    if ( src ) dst = src->parent;

    g3dui_setHourGlass   ( gui );
    g3duiclipboard_paste ( cli, urm, sce, dst, gui->engine_flags );

    g3dui_redrawObjectList     ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawGLViews        ( gui );

    g3dui_unsetHourGlass ( gui );
}

/******************************************************************************/
void common_g3dui_copySelectionCbk ( G3DUI *gui ) {
    G3DUICLIPBOARD *cli = gui->cli;
    G3DSCENE *sce = gui->sce;

    printf("copying %d object(s)\n", list_count ( sce->lsel ) );

    g3dui_setHourGlass ( gui );
    g3duiclipboard_copyObject ( cli, 
                                sce,
                                sce->lsel, 0x01, gui->engine_flags );
    g3dui_unsetHourGlass ( gui );
}

/******************************************************************************/
void common_g3dui_deleteSelectionCbk ( G3DUI *gui ) {
    G3DSCENE  *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
    G3DURMANAGER *urm = gui->urm;
    G3DMESH   *mes = NULL;

    if ( gui->engine_flags & VIEWOBJECT ) {
        g3durm_scene_deleteSelectedObjects ( urm, 
                                             sce, 
                                             gui->engine_flags, 
                                             REDRAWVIEW | REDRAWLIST );

        g3dui_updateAllCurrentEdit ( gui );
        g3dui_updateCoords         ( gui );
        g3dui_redrawObjectList     ( gui );
    } else {
        if ( obj && ( obj->type == G3DSPLINETYPE ) ) {
            G3DSPLINE *spline = ( G3DSPLINE * ) obj;
            if ( gui->engine_flags & VIEWVERTEX ) {
                LIST *lremovedPoints   = g3dcurve_getSelectedPoints ( spline->curve );

                g3durm_spline_deletePoints ( urm,
                                             spline, 
                                             lremovedPoints, 
                                             gui->engine_flags,
                                             REDRAWVIEW );

                g3dmesh_unselectAllVertices ( spline );
            }
        }

        if ( obj && ( obj->type == G3DMESHTYPE ) ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            g3durm_mesh_deleteGeometry ( urm,
                                         mes,
                                         gui->engine_flags,
                                         REDRAWVIEW );
        }
    }

    g3dui_redrawObjectList     ( gui );
    g3dui_updateAllCurrentEdit ( gui );
    g3dui_redrawGLViews        ( gui );
}
