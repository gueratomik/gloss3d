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
typedef struct _URMCUTMESH {
    G3DSCENE *sce;
    G3DMESH  *mes;
    LIST     *removedEdgeList;
    LIST     *removedFaceList;
    LIST     *addedVertexList;
    LIST     *addedEdgeList;
    LIST     *addedFaceList;
} URMCUTMESH;

/******************************************************************************/
static URMCUTMESH *urmcutmesh_new ( G3DSCENE *sce,
                                    G3DMESH  *mes,
                                    LIST     *removedEdgeList,
                                    LIST     *removedFaceList,
                                    LIST     *addedVertexList,
                                    LIST     *addedEdgeList,
                                    LIST     *addedFaceList ) {
    uint32_t structsize = sizeof ( URMCUTMESH );

    URMCUTMESH *cms = ( URMCUTMESH * ) calloc ( 0x01, structsize );

    if ( cms == NULL ) {
        fprintf ( stderr, "urmcutmesh_new: memory allocation falied\n" );

        return NULL;
    }

    cms->sce             = sce;
    cms->mes             = mes;
    cms->removedEdgeList = removedEdgeList;
    cms->removedFaceList = removedFaceList;
    cms->addedVertexList = addedVertexList;
    cms->addedEdgeList   = addedEdgeList;
    cms->addedFaceList   = addedFaceList;

    return cms;
}

/******************************************************************************/
static void urmcutmesh_free ( URMCUTMESH *cms ) {
    list_free ( &cms->removedFaceList, NULL );
    list_free ( &cms->removedEdgeList, NULL );
    list_free ( &cms->addedVertexList, NULL );
    list_free ( &cms->addedFaceList  , NULL );
    list_free ( &cms->addedEdgeList  , NULL );

    free ( cms );
}

/******************************************************************************/
static void cutMesh_free ( void    *data, 
                           uint32_t commit ) {
    URMCUTMESH *cms = ( URMCUTMESH * ) data;

    if ( commit ) {
        list_exec ( cms->removedEdgeList, (void(*)(void*)) g3dedge_free   );
        list_exec ( cms->removedFaceList, (void(*)(void*)) g3dface_free   );
    } else {
        list_exec ( cms->addedVertexList, (void(*)(void*)) g3dvertex_free );
        list_exec ( cms->addedEdgeList  , (void(*)(void*)) g3dedge_free   );
        list_exec ( cms->addedFaceList  , (void(*)(void*)) g3dface_free   );
    }

    urmcutmesh_free ( cms );
}

/******************************************************************************/
static void cutMesh_undo ( G3DURMANAGER *urm, 
                           void         *data, 
                           uint64_t      engineFlags ) {
    URMCUTMESH *cms = ( URMCUTMESH * ) data;
    LIST *ltmpoldedg = cms->removedEdgeList;
    LIST *ltmpoldfac = cms->removedFaceList;
    LIST *ltmpnewver = cms->addedVertexList;
    LIST *ltmpnewedg = cms->addedEdgeList;
    LIST *ltmpnewfac = cms->addedFaceList;

    /*** We have to restore the selection in order to avoid crash ***/
    /*** because other functions are based on the select flag set ***/
    g3dmesh_unselectAllFaces ( cms->mes );

    /* Remove new faces - Note: will remove unused edges automatically */
    while( ltmpnewfac ) {
        g3dmesh_removeFace ( cms->mes, ( G3DFACE * ) ltmpnewfac->data, NULL );
        ltmpnewfac = ltmpnewfac->next;
    }

    /* Remove new vertices */
    while( ltmpnewver ) {
        g3dmesh_removeVertex ( cms->mes, ( G3DVERTEX * ) ltmpnewver->data );
        ltmpnewver = ltmpnewver->next;
    }

    /* re-add old edges for topology linking */
    while( ltmpoldedg ) {
        g3dmesh_addEdge ( cms->mes, ( G3DEDGE * ) ltmpoldedg->data );
        ltmpoldedg = ltmpoldedg->next;
    }

    /* re-add old faces */
    while( ltmpoldfac ) {
        g3dmesh_addFace ( cms->mes, ( G3DFACE * ) ltmpoldfac->data, NULL );
        ltmpoldfac = ltmpoldfac->next;
    }

    /*** Rebuild the cut mesh ***/
    g3dobject_update_r ( G3DOBJECTCAST(cms->sce), 0x00, engineFlags );
}

/******************************************************************************/
static void cutMesh_redo ( G3DURMANAGER *urm, 
                           void         *data, 
                           uint64_t      engineFlags ) {
    URMCUTMESH *cms = ( URMCUTMESH * ) data;
    G3DMESH *mes = cms->mes;
    LIST *ltmpoldedg = cms->removedEdgeList;
    LIST *ltmpoldfac = cms->removedFaceList;
    LIST *ltmpnewver = cms->addedVertexList;
    LIST *ltmpnewedg = cms->addedEdgeList;
    LIST *ltmpnewfac = cms->addedFaceList;

    /*** We have to restore the selection in order to avoid crash ***/
    /*** because other functions are based on the select flag set ***/
    g3dmesh_unselectAllFaces ( cms->mes );

    /* remove removed faces - Note: will automatically remove unused edges */
    while( ltmpoldfac ) {
        g3dmesh_removeFace ( cms->mes, ( G3DFACE * ) ltmpoldfac->data, NULL );
        ltmpoldfac = ltmpoldfac->next;
    }

    /* add added vertices */
    while( ltmpnewver ) {
        g3dmesh_addVertex ( cms->mes, ( G3DVERTEX * ) ltmpnewver->data );
        ltmpnewver = ltmpnewver->next;
    }

    /* add added edges for topology linking */
    while( ltmpnewedg ) {
        g3dmesh_addEdge ( cms->mes, ( G3DEDGE * ) ltmpnewedg->data );
        ltmpnewedg = ltmpnewedg->next;
    }

    /* add added faces */
    while( ltmpnewfac ) {
        g3dmesh_addFace ( cms->mes, ( G3DFACE * ) ltmpnewfac->data, NULL );
        ltmpnewfac = ltmpnewfac->next;
    }

    /*** Rebuild the cut mesh ***/
    g3dobject_update_r ( G3DOBJECTCAST(cms->sce), 0x00, engineFlags );
}

/******************************************************************************/
void g3durm_mesh_cut ( G3DURMANAGER *urm,
                       G3DSCENE     *sce,
                       G3DMESH      *mes,
                       G3DFACE      *knife,
                       int           restricted,
                       uint64_t      engineFlags,
                       uint32_t      returnFlags ) {
    LIST *removedEdgeList   = NULL,
         *removedFaceList   = NULL,
         *addedVertexList = NULL,
         *addedEdgeList   = NULL,
         *addedFaceList   = NULL;
    URMCUTMESH *cms;

    g3dmesh_cut ( mes, 
                  knife, 
                 &removedEdgeList,
                 &removedFaceList,
                 &addedVertexList,
                 &addedEdgeList,
                 &addedFaceList,
                  restricted, 
                  engineFlags );

    g3dobject_update_r ( G3DOBJECTCAST(sce), 0x00, engineFlags );

    cms = urmcutmesh_new ( sce,
                           mes,
                           removedEdgeList,
                           removedFaceList,
                           addedVertexList,
                           addedEdgeList,
                           addedFaceList );

    g3durmanager_push ( urm,
                        cutMesh_undo,
                        cutMesh_redo,
                        cutMesh_free,
                        cms,
                        returnFlags );
}
