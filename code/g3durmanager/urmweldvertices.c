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
    LIST     *removedVertexList;
    LIST     *removedEdgeList;
    LIST     *removedFaceList;
    LIST     *addedVertexList;
    LIST     *addedEdgeList;
    LIST     *addedFaceList;
} URMWELDVERTICES;

/******************************************************************************/
static URMWELDVERTICES *urmweldvertices_new ( G3DSCENE *sce,
                                              G3DMESH  *mes,
                                              LIST     *removedVertexList,
                                              LIST     *removedEdgeList,
                                              LIST     *removedFaceList,
                                              LIST     *addedVertexList,
                                              LIST     *addedEdgeList,
                                              LIST     *addedFaceList ) {
    uint32_t structsize = sizeof ( URMWELDVERTICES );

    URMWELDVERTICES *wvs = ( URMWELDVERTICES * ) calloc ( 0x01, structsize );

    if ( wvs == NULL ) {
        fprintf ( stderr, "urmweldvertices_new: memory allocation falied\n" );

        return NULL;
    }

    wvs->sce               = sce;
    wvs->mes               = mes;
    wvs->removedVertexList = removedVertexList;
    wvs->removedEdgeList   = removedEdgeList;
    wvs->removedFaceList   = removedFaceList;
    wvs->addedVertexList   = addedVertexList;
    wvs->addedEdgeList     = addedEdgeList;
    wvs->addedFaceList     = addedFaceList;

    return wvs;
}

/******************************************************************************/
static void urmweldvertices_free ( URMWELDVERTICES *wvs ) {
    list_free ( &wvs->removedVertexList, NULL );
    list_free ( &wvs->removedFaceList  , NULL );
    list_free ( &wvs->removedEdgeList  , NULL );

    list_free ( &wvs->addedVertexList, NULL );
    list_free ( &wvs->addedEdgeList  , NULL );
    list_free ( &wvs->addedFaceList  , NULL );

    free ( wvs );
}

/******************************************************************************/
static void weldVertices_free ( void *data, uint32_t commit ) {
    URMWELDVERTICES *wvs = ( URMWELDVERTICES * ) data;

    if ( commit ) {
        list_exec ( wvs->removedVertexList, (void(*)(void*)) g3dvertex_free );
        list_exec ( wvs->removedFaceList  , (void(*)(void*)) g3dface_free   );
        list_exec ( wvs->removedEdgeList  , (void(*)(void*)) g3dedge_free   );
    } else {
        list_exec ( wvs->addedVertexList, (void(*)(void*)) g3dvertex_free );
        list_exec ( wvs->addedFaceList  , (void(*)(void*)) g3dface_free   );
        list_exec ( wvs->addedEdgeList  , (void(*)(void*)) g3dedge_free   );
    }

    urmweldvertices_free ( wvs );
}

/******************************************************************************/
static void weldVertices_undo ( G3DURMANAGER *urm, 
                                void         *data, 
                                uint64_t      engineFlags ) {
    URMWELDVERTICES *wvs = ( URMWELDVERTICES * ) data;
    LIST *ltmpnewver = wvs->addedVertexList;
    LIST *ltmpnewedg = wvs->addedEdgeList;
    LIST *ltmpnewfac = wvs->addedFaceList;
    LIST *ltmpoldver = wvs->removedVertexList;
    LIST *ltmpoldedg = wvs->removedEdgeList;
    LIST *ltmpoldfac = wvs->removedFaceList;
    G3DMESH *mes = wvs->mes;

    g3dmesh_unselectAllVertices ( mes );

    /* delete added faces - Note: unused edges will be removed automatically */
    while( ltmpnewfac ) {
        g3dmesh_removeFace ( mes, G3DFACECAST(ltmpnewfac->data), NULL );
        ltmpnewfac = ltmpnewfac->next;
    }

    /*** delete created vertex ***/
    while( ltmpnewver ) {
        g3dmesh_removeVertex( mes, G3DVERTEXCAST(ltmpnewver->data) );
        ltmpnewver = ltmpnewver->next;
    }

    /*** restore removed vertices ***/
    while( ltmpoldver ) {
        g3dmesh_addVertex    ( mes, G3DVERTEXCAST(ltmpoldver->data) );
        g3dmesh_selectVertex ( mes, G3DVERTEXCAST(ltmpoldver->data) );
        ltmpoldver = ltmpoldver->next;
    }

    /*** restore removed edges for topology linking with faces ***/
    while( ltmpoldedg ) {
        g3dmesh_addEdge ( mes, G3DEDGECAST(ltmpoldedg->data) );
        ltmpoldedg = ltmpoldedg->next;
    }

    /*** restore removed faces ***/
    while( ltmpoldfac ) {
        g3dmesh_addFace( mes, G3DFACECAST(ltmpoldfac->data), NULL );
        ltmpoldfac = ltmpoldfac->next;
    }

    /*** Rebuild the mesh with modifiers ***/
    g3dobject_update_r ( G3DOBJECTCAST(wvs->sce), 0x00, engineFlags );
}

/******************************************************************************/
static void weldVertices_redo ( G3DURMANAGER *urm, 
                                void         *data,
                                uint64_t      engineFlags ) {
    URMWELDVERTICES *wvs = ( URMWELDVERTICES * ) data;
    G3DMESH *mes = wvs->mes;
    LIST *ltmpnewver = wvs->addedVertexList;
    LIST *ltmpnewedg = wvs->addedEdgeList;
    LIST *ltmpnewfac = wvs->addedFaceList;
    LIST *ltmpoldver = wvs->removedVertexList;
    LIST *ltmpoldedg = wvs->removedEdgeList;
    LIST *ltmpoldfac = wvs->removedFaceList;

    g3dmesh_unselectAllVertices ( mes );

    /*delete removed faces - Note: unused edges will be removed automatically*/
    while( ltmpoldfac ) {
        g3dmesh_removeFace ( mes,  G3DFACECAST(ltmpoldfac->data), NULL );
        ltmpoldfac = ltmpoldfac->next;
    }

    /*** Deleted welded vertices ***/
    while( ltmpoldver ) {
        g3dmesh_removeVertex ( mes, G3DVERTEXCAST(ltmpoldver->data) );
        ltmpoldver = ltmpoldver->next;
    }

    /*** add created vertex ***/
    while( ltmpnewver ) {
        g3dmesh_addVertex    ( mes, G3DVERTEXCAST(ltmpnewver->data) );
        g3dmesh_selectVertex ( mes, G3DVERTEXCAST(ltmpnewver->data) );
        ltmpnewver = ltmpnewver->next;
    }

    /*** add added edges for topology linking with faces ***/
    while( ltmpnewedg ) {
        g3dmesh_addEdge ( mes, G3DEDGECAST(ltmpnewedg->data) );
        ltmpnewedg = ltmpnewedg->next;
    }

    /*** restore new faces ***/
    while( ltmpnewfac ) {
        g3dmesh_addFace ( mes, G3DFACECAST(ltmpnewfac->data), NULL );
        ltmpnewfac = ltmpnewfac->next;
    }

    /*** Rebuild the mesh with modifiers ***/
    g3dobject_update_r ( G3DOBJECTCAST(wvs->sce), 0x00, engineFlags );
}

/******************************************************************************/
void g3durm_mesh_weldSelectedVertices ( G3DURMANAGER *urm, 
                                        G3DSCENE     *sce, 
                                        G3DMESH      *mes, 
                                        uint32_t      type,
                                        uint64_t      engineFlags,
                                        uint32_t      returnFlags ) {
    URMWELDVERTICES *wvs;
    LIST *removedVertexList = NULL;
    LIST *removedEdgeList = NULL;
    LIST *removedFaceList = NULL;
    LIST *addedVertexList = NULL;
    LIST *addedEdgeList = NULL;
    LIST *addedFaceList = NULL;
    G3DVERTEX *newver;

    newver = g3dmesh_weldSelectedVertices ( mes,
                                           &removedVertexList,
                                           &removedEdgeList,
                                           &removedFaceList,
                                           &addedEdgeList,
                                           &addedFaceList );

    g3dobject_update_r ( G3DOBJECTCAST(sce), 0x00, engineFlags );

    if ( newver ) {
        list_insert ( &addedVertexList, newver );

        /*** Rebuild the mesh with modifiers ***/
        g3dobject_update_r ( G3DOBJECTCAST(sce), 0x00, engineFlags );

        wvs = urmweldvertices_new ( sce,
                                    mes,
                                    removedVertexList,
                                    removedEdgeList,
                                    removedFaceList,
                                    addedVertexList,
                                    addedEdgeList,
                                    addedFaceList );

        g3durmanager_push ( urm, weldVertices_undo,
                                 weldVertices_redo,
                                 weldVertices_free, wvs, returnFlags );
    } else {
        list_free ( &removedVertexList, NULL );
    }
}

/******************************************************************************/
void g3durm_mesh_weldNeighbourVertices ( G3DURMANAGER *urm,
                                         G3DSCENE     *sce, 
                                         G3DMESH      *mes, 
                                         uint32_t      type,
                                         float         distance,
                                         uint64_t      engineFlags,
                                         uint32_t      returnFlags ) {
    URMWELDVERTICES *wvs;
    LIST *removedVertexList = NULL;
    LIST *removedEdgeList = NULL;
    LIST *removedFaceList = NULL;
    LIST *addedVertexList = NULL;
    LIST *addedEdgeList = NULL;
    LIST *addedFaceList = NULL;

    g3dmesh_weldNeighbourVertices ( mes, 
                                    distance,
                                    &removedVertexList,
                                    &removedEdgeList,
                                    &removedFaceList,
                                    &addedVertexList,
                                    &addedEdgeList,
                                    &addedFaceList );

    /*** Rebuild the mesh with modifiers ***/
    g3dobject_update_r ( G3DOBJECTCAST(sce), 0x00, engineFlags );

    wvs = urmweldvertices_new ( sce, 
                                mes,
                                removedVertexList,
                                removedEdgeList,
                                removedFaceList,
                                addedVertexList,
                                addedVertexList,
                                addedEdgeList );

    g3durmanager_push ( urm, weldVertices_undo,
                             weldVertices_redo,
                             weldVertices_free, wvs, returnFlags );
}

