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
uint64_t g3duicoordinatesedit_pos ( G3DUICOORDINATESEDIT *coordedit, 
                                    G3DUIAXIS             axis, 
                                    uint32_t              absolute,
                                    float                 val ) {
    G3DUI *gui = coordedit->gui;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        if ( gui->engine_flags & VIEWOBJECT ) {
            URMTRANSFORMOBJECT *uto = g3durm_object_transform ( urm,
                                                                sce,
                                                                sce->lsel,
                                                                REDRAWVIEW );

            urmtransform_saveState ( uto, UTOSAVESTATEBEFORE );

            if ( axis == G3DUIXAXIS ) obj->pos.x = val;
            if ( axis == G3DUIYAXIS ) obj->pos.y = val;
            if ( axis == G3DUIZAXIS ) obj->pos.z = val;

            g3dobject_updateMatrix_r ( obj, gui->engine_flags );

            urmtransform_saveState ( uto, UTOSAVESTATEAFTER );
        }

        if ( gui->engine_flags & VIEWVERTEX ) {
            if ( obj->type == G3DMESHTYPE ) {
                G3DMESH *mes = ( G3DMESH * ) obj;
                uint32_t axis_flags = 0x00;
                G3DVECTOR3F *oldpos = NULL;
                G3DVECTOR3F *newpos = NULL;
                G3DVECTOR3F avg;

                LIST *ltmpver = mes->selectedVertexList;

                g3dvertex_getAveragePositionFromList ( mes->selectedVertexList, &avg );

                g3dvertex_copyPositionFromList ( mes->selectedVertexList, &oldpos );


                while( ltmpver ) {
                    G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                    G3DVECTOR3F to = { .x = ver->pos.x,
                                       .y = ver->pos.y,
                                       .z = ver->pos.z };

                    if ( axis == G3DUIXAXIS ) { to.x = val; axis_flags |= XAXIS; }
                    if ( axis == G3DUIYAXIS ) { to.y = val; axis_flags |= YAXIS; }
                    if ( axis == G3DUIZAXIS ) { to.z = val; axis_flags |= ZAXIS; }


                    /* setting position via function call will */
                    /* invalidate owner object */
                    g3dvertex_setPosition( ver, to.x, to.y, to.z );

                    ltmpver = ltmpver->next;
                }
/*
                g3dmesh_moveVerticesTo ( mes, mes->selectedVertexList, &avg, &to, absolute, axis_flags, gui->engine_flags );
*/
                g3dvertex_copyPositionFromList ( mes->selectedVertexList, &newpos );

                g3durm_mesh_moveVertexList ( gui->urm, mes, mes->selectedVertexList, NULL, NULL, NULL,
                                             oldpos, 
                                             newpos, 
                                             REDRAWVIEW );
            }
        }
    }

    g3dscene_updatePivot ( sce, gui->engine_flags );


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duicoordinatesedit_rot ( G3DUICOORDINATESEDIT *coordedit, 
                                       G3DUIAXIS             axis, 
                                       float                 val ) {
    G3DUI *gui = coordedit->gui;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        if ( gui->engine_flags & VIEWOBJECT ) {
            URMTRANSFORMOBJECT *uto = g3durm_object_transform ( urm,
                                                                sce,
                                                                sce->lsel,
                                                                REDRAWVIEW );

            urmtransform_saveState ( uto, UTOSAVESTATEBEFORE );

            if ( axis == G3DUIXAXIS ) obj->rot.x = val;
            if ( axis == G3DUIYAXIS ) obj->rot.y = val;
            if ( axis == G3DUIZAXIS ) obj->rot.z = val;

            g3dobject_updateMatrix_r ( obj, gui->engine_flags );

            urmtransform_saveState ( uto, UTOSAVESTATEAFTER );
        }
    }

    g3dscene_updatePivot ( sce, gui->engine_flags );


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duicoordinatesedit_sca ( G3DUICOORDINATESEDIT *coordedit, 
                                       G3DUIAXIS             axis, 
                                       float                 val ) {
    G3DUI *gui = coordedit->gui;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        if ( gui->engine_flags & VIEWOBJECT ) {
            URMTRANSFORMOBJECT *uto = g3durm_object_transform ( urm,
                                                                sce,
                                                                sce->lsel,
                                                                REDRAWVIEW );

            if ( axis == G3DUIXAXIS ) obj->sca.x = val;
            if ( axis == G3DUIYAXIS ) obj->sca.y = val;
            if ( axis == G3DUIZAXIS ) obj->sca.z = val;

            g3dobject_updateMatrix_r ( obj, gui->engine_flags );

            urmtransform_saveState ( uto, UTOSAVESTATEAFTER );
        }
    }

    g3dscene_updatePivot ( sce, gui->engine_flags );


    return REDRAWVIEW;
}

