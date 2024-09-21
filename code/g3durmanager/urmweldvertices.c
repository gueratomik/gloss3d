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
#include <g3durmanager.h>

/******************************************************************************/
typedef struct _URMWELDVERTICES {
    G3DSCENE *sce;
    G3DMESH  *mes;
    LIST     *loldver;
    LIST     *loldfac;
    LIST     *loldedg;
    LIST     *lnewfac;
    LIST     *lnewver;
    LIST     *lnewedg;
} URMWELDVERTICES;

/******************************************************************************/
static URMWELDVERTICES *urmweldvertices_new ( G3DSCENE *sce,
                                              G3DMESH  *mes, 
                                              LIST     *loldver,
                                              LIST     *lnewver,
                                              LIST     *loldfac,
                                              LIST     *lnewfac ) {
    uint32_t structsize = sizeof ( URMWELDVERTICES );

    URMWELDVERTICES *wvs = ( URMWELDVERTICES * ) calloc ( 0x01, structsize );

    if ( wvs == NULL ) {
        fprintf ( stderr, "urmweldvertices_new: memory allocation falied\n" );

        return NULL;
    }

    wvs->sce     = sce;
    wvs->mes     = mes;
    wvs->loldver = loldver;
    wvs->loldfac = loldfac;
    wvs->lnewfac = lnewfac;
    wvs->lnewver = lnewver;

    /*** for freeing unused edges later ***/
    g3dface_getOrphanedEdgesFromList ( loldfac, &wvs->loldedg );
    g3dface_getSharedEdgesFromList   ( lnewfac, &wvs->lnewedg );

    return wvs;
}

/******************************************************************************/
static void urmweldvertices_free ( URMWELDVERTICES *wvs ) {
    list_free ( &wvs->loldver, NULL );
    list_free ( &wvs->loldfac, NULL );
    list_free ( &wvs->loldedg, NULL );

    list_free ( &wvs->lnewfac, NULL );
    list_free ( &wvs->lnewver, NULL );
    list_free ( &wvs->lnewedg, NULL );

    free ( wvs );
}

/******************************************************************************/
static void weldVertices_free ( void *data, uint32_t commit ) {
    URMWELDVERTICES *wvs = ( URMWELDVERTICES * ) data;

    if ( commit ) {
        list_exec ( wvs->loldver, (void(*)(void*)) g3dvertex_free );
        list_exec ( wvs->loldfac, (void(*)(void*)) g3dface_free   );
        list_exec ( wvs->loldedg, (void(*)(void*)) g3dedge_free   );
    } else {
        list_exec ( wvs->lnewver, (void(*)(void*)) g3dvertex_free );
        list_exec ( wvs->lnewfac, (void(*)(void*)) g3dface_free   );
        list_exec ( wvs->lnewedg, (void(*)(void*)) g3dedge_free   );
    }

    urmweldvertices_free ( wvs );
}

/******************************************************************************/
static void weldVertices_undo ( G3DURMANAGER *urm, 
                                void         *data, 
                                uint64_t      engine_flags ) {
    URMWELDVERTICES *wvs = ( URMWELDVERTICES * ) data;
    G3DMESH *mes = wvs->mes;

    g3dmesh_unselectAllVertices ( mes );

    /*** delete added faces ***/
    list_execargdata ( wvs->lnewfac, (void(*)(void*,void*)) g3dmesh_removeFace, mes );

    /*** delete created vertex ***/
    list_execargdata ( wvs->lnewver, (void(*)(void*,void*)) g3dmesh_removeVertex, mes );

    /*** restore deleted vertices ***/
    list_execargdata ( wvs->loldver, (void(*)(void*,void*)) g3dmesh_addSelectedVertex, mes );

    /*** restore deleted faces ***/
    list_execargdata ( wvs->loldfac, (void(*)(void*,void*)) g3dmesh_addFace, mes );

    /*** Rebuild the mesh with modifiers ***/
    g3dobject_update ( G3DOBJECTCAST(wvs->sce), 0x00, engine_flags );
}

/******************************************************************************/
static void weldVertices_redo ( G3DURMANAGER *urm, 
                                void         *data,
                                uint64_t      engine_flags ) {
    URMWELDVERTICES *wvs = ( URMWELDVERTICES * ) data;
    G3DMESH *mes = wvs->mes;

    g3dmesh_unselectAllVertices ( mes );

    /*** delete old faces ***/
    list_execargdata ( wvs->loldfac, (void(*)(void*,void*)) g3dmesh_removeFace, mes );

    /*** Deleted welded vertices ***/
    list_execargdata ( wvs->loldver, (void(*)(void*,void*)) g3dmesh_removeVertex, mes );

    /*** add created vertex ***/
    list_execargdata ( wvs->lnewver, (void(*)(void*,void*)) g3dmesh_addSelectedVertex, mes);

    /*** restore new faces ***/
    list_execargdata ( wvs->lnewfac, (void(*)(void*,void*)) g3dmesh_addFace, mes );

    /*** Rebuild the mesh with modifiers ***/
    g3dobject_update ( G3DOBJECTCAST(wvs->sce), 0x00, engine_flags );
}

/******************************************************************************/
void g3durm_mesh_weldSelectedVertices ( G3DURMANAGER *urm, 
                                        G3DSCENE     *sce, 
                                        G3DMESH      *mes, 
                                        uint32_t      type,
                                        uint64_t      engine_flags,
                                        uint32_t      return_flags ) {
    URMWELDVERTICES *wvs;
    LIST *loldver = list_copy ( mes->selectedVertexList );
    LIST *loldfac = NULL,
         *lnewfac = NULL;
    G3DVERTEX *newver;
    LIST *lnewver = NULL;

    newver = g3dmesh_weldSelectedVertices ( mes, type, &loldfac, &lnewfac );

    if ( newver ) {
        list_insert ( &lnewver, newver );

        /*** Rebuild the mesh with modifiers ***/
        g3dobject_update ( G3DOBJECTCAST(sce), 0x00, engine_flags );

        wvs = urmweldvertices_new ( sce,
                                    mes,
                                    loldver,
                                    lnewver,
                                    loldfac,
                                    lnewfac );

        g3durmanager_push ( urm, weldVertices_undo,
                                 weldVertices_redo,
                                 weldVertices_free, wvs, return_flags );
    } else {
        list_free ( &loldver, NULL );
    }
}

/******************************************************************************/
void g3durm_mesh_weldNeighbourVertices ( G3DURMANAGER *urm,
                                         G3DSCENE     *sce, 
                                         G3DMESH      *mes, 
                                         uint32_t      type,
                                         float         distance,
                                         uint64_t      engine_flags,
                                         uint32_t      return_flags ) {
    URMWELDVERTICES *wvs;
    LIST *loldver = NULL;
    LIST *loldfac = NULL,
         *lnewfac = NULL;
    LIST *lnewver = NULL;

    g3dmesh_weldNeighbourVertices ( mes, 
                                    distance,
                                    &loldver, 
                                    &lnewver, 
                                    &loldfac, 
                                    &lnewfac );

    /*** Rebuild the mesh with modifiers ***/
    g3dobject_update ( G3DOBJECTCAST(sce), 0x00, engine_flags );

    wvs = urmweldvertices_new ( sce, 
                                mes,
                                loldver,
                                lnewver,
                                loldfac,
                                lnewfac );

    g3durmanager_push ( urm, weldVertices_undo,
                             weldVertices_redo,
                             weldVertices_free, wvs, return_flags );
}
