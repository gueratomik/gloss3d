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
G3DWEIGHTGROUP *g3duiweightgrouplist_getWeightGroup ( G3DUIWEIGHTGROUPLIST *wgrplist,
                                                      G3DMESH              *mes, 
                                                      float                mouse_x,
                                                      float                mouse_y ) {
    G3DUI *gui = wgrplist->gui;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        LIST *ltmpgrp = mes->weightgroupList;
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
    }

    return NULL;
}

/******************************************************************************/
uint64_t g3duiweightgrouplist_deleteWeightGroup ( G3DUIWEIGHTGROUPLIST *wgrplist ) {
    G3DUI *gui = wgrplist->gui;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        if ( mes->curgrp ) {
            g3dmesh_removeWeightGroup ( mes, mes->curgrp );
        }
    }

    return UPDATECURRENTOBJECT;
}

/******************************************************************************/
uint64_t g3duiweightgrouplist_addWeightGroup ( G3DUIWEIGHTGROUPLIST *wgrplist ) {
    G3DUI *gui = wgrplist->gui;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        char buf[0x20];

        snprintf ( buf, 0x20, "VertexWeightGroup%02i", mes->weightgroupCount );

        g3dmesh_addWeightGroup ( mes, g3dweightgroup_new ( mes, buf ) );
    }

    return UPDATECURRENTOBJECT;
}

/******************************************************************************/
uint64_t g3duiweightgrouplist_name ( G3DUIWEIGHTGROUPLIST *wgrplist,
                                        const char           *grpname ) {
    G3DUI *gui = wgrplist->gui;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        g3dweightgroup_name ( mes->curgrp, ( char * ) grpname );
    }

    return UPDATECURRENTOBJECT;
}

/******************************************************************************/
uint64_t g3duiweightgrouplist_deleteSelected ( G3DUIWEIGHTGROUPLIST *wgrplist ) {
    G3DUI *gui = wgrplist->gui;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        if ( mes->curgrp ) {
            G3DWEIGHTGROUP *curgrp = mes->curgrp;
/*** TODO - check if we are in buffered mode. Otherwise this is not needed ***/
            /*LIST *lsub = ( curgrp ) ? g3dvertex_getFacesFromList ( curgrp->lver ) : NULL;*/


            g3dmesh_removeWeightGroup ( mes, curgrp );
/*
            mes->obj.invalidationFlags |= ( UPDATEVERTEXNORMAL |
                                       UPDATEFACENORMAL |
                                       RESETMODIFIERS );
*/
            /*** update vertex painting ***/
            g3dmesh_update ( mes, 0x00, gui->engine_flags );

            /*list_free ( &lsub, NULL );*/
        }
    }

    return REDRAWVIEW | UPDATECURRENTOBJECT;
}

/******************************************************************************/
uint64_t g3duiweightgrouplist_select ( G3DUIWEIGHTGROUPLIST *wgrplist,
                                          G3DWEIGHTGROUP       *grp ) {
    G3DUI *gui = wgrplist->gui;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        G3DWEIGHTGROUP *curgrp = mes->curgrp;
        LIST *lver;

        /*** deselect previsouly selected weightgroup ***/
        if ( mes->curgrp ) {
            G3DWEIGHTGROUP *curgrp = mes->curgrp;

            lver = g3dmesh_getVerticesFromWeightgroup ( mes, curgrp );

            g3dmesh_unselectWeightGroup ( mes, curgrp );
/*
            mes->lupdver = lver;
            mes->obj.invalidationFlags |= ( UPDATEMODIFIERS );
*/
            /*** update vertex painting ***/
            g3dmesh_update ( mes, 0x00, gui->engine_flags );

            list_free ( &lver, NULL );
        }

        lver = g3dmesh_getVerticesFromWeightgroup ( mes, grp );

        g3dmesh_selectWeightGroup ( mes, grp );
/*
        mes->lupdver = lver;
        mes->obj.invalidationFlags |= ( RESETMODIFIERS );
*/
        /*** update vertex painting ***/
        g3dmesh_update ( mes, 0x00, gui->engine_flags );

        list_free ( &lver, NULL );
    }

    return UPDATECURRENTOBJECT | REDRAWVIEW;
}
