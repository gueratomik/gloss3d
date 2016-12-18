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
G3DWEIGHTGROUP *common_g3duiweightgrouplist_getWeightGroup ( G3DMESH *mes, 
                                                             float mouse_x,
                                                             float mouse_y ) {
    LIST *ltmpgrp = mes->lgrp;
    float y = 0;

    while ( ltmpgrp ) {
        G3DWEIGHTGROUP *grp = ( G3DWEIGHTGROUP * ) ltmpgrp->data;
        float y1, y2;

        y1 = y;
        y2 = y1 + LISTINDENT;

        if ( ( mouse_y > y1 ) && ( mouse_y < y2 ) ) {
            return grp;
        }

        y = y2;

        ltmpgrp = ltmpgrp->next;
    }

    return NULL;
}

/******************************************************************************/
void common_g3duiweightgrouplist_deleteWeightGroupCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        if ( mes->curgrp ) {
            g3dweightgroup_empty ( mes->curgrp );

            g3dmesh_removeWeightGroup ( mes, mes->curgrp );

            g3dui_redrawAllWeightGroupList ( gui );
        }
    }
}

/******************************************************************************/
void common_g3duiweightgrouplist_addWeightGroupCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        char buf[0x20];

        snprintf ( buf, 0x20, "VertexWeightGroup%02i", mes->nbgrp );

        g3dmesh_addWeightGroup ( mes, g3dweightgroup_new ( mes, buf ) );

        g3dui_redrawAllWeightGroupList ( gui );
    }
}

/******************************************************************************/
void common_g3duiweightgrouplist_nameCbk ( G3DUI *gui, const char *grpname ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3dweightgroup_name ( mes->curgrp, ( char * ) grpname );

        g3dui_redrawAllWeightGroupList ( gui );
    }
}

/******************************************************************************/
void common_g3duiweightgrouplist_deleteSelectedCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    if ( gui->lock ) return;


    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        if ( mes->curgrp ) {
            G3DWEIGHTGROUP *curgrp = mes->curgrp;
/*** TODO - check if we are in buffered mode. Otherwise this is not needed ***/
            LIST *lsub = ( curgrp ) ? g3dvertex_getFacesFromList ( curgrp->lver ) : NULL;


            g3dweightgroup_empty ( curgrp );

            g3dmesh_removeWeightGroup ( mes, curgrp );

            /*** update vertex painting ***/
            g3dmesh_update ( mes, NULL,
                                  NULL,
                                  UPDATEVERTEXNORMAL |
                                  UPDATEFACENORMAL, gui->flags );

            list_free ( &lsub, NULL );

            g3dui_redrawGLViews ( gui );
            g3dui_updateAllCurrentEdit ( gui );
        }
    }
}

/******************************************************************************/
void common_g3duiweightgrouplist_selectCbk ( G3DUI *gui, G3DWEIGHTGROUP *grp ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        G3DWEIGHTGROUP *curgrp = mes->curgrp;
/*** TODO - check if we are in buffered mode. Otherwise this is not needed ***/
        LIST *lsub = ( curgrp ) ? g3dvertex_getFacesFromList ( curgrp->lver ) : NULL;

        /*** deselect previsouly selected weightgroup ***/
        if ( mes->curgrp ) {
            G3DWEIGHTGROUP *curgrp = mes->curgrp;

            g3dmesh_unselectWeightGroup ( mes, curgrp );

            /*** update vertex painting ***/
            g3dmesh_update ( mes, NULL,
                                  NULL,
                                  0x00, gui->flags );

            list_free ( &lsub, NULL );
        }

        g3dmesh_selectWeightGroup ( mes, grp );

        /*** update vertex painting ***/
        g3dmesh_update ( mes, NULL,
                              NULL,
                              0x00, gui->flags );

        list_free ( &lsub, NULL );

        g3dui_redrawGLViews ( gui );
        g3dui_updateAllCurrentEdit ( gui );
    }
}
