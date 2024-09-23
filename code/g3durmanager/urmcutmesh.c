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
static URMCUTMESH *urmcutmesh_new ( G3DSCENE *sce,
                                    G3DMESH  *mes,
                                    LIST     *loldfac,
                                    LIST     *lnewver,
                                    LIST     *lnewfac ) {
    uint32_t structsize = sizeof ( URMCUTMESH );

    URMCUTMESH *cms = ( URMCUTMESH * ) calloc ( 0x01, structsize );

    if ( cms == NULL ) {
        fprintf ( stderr, "urmcutmesh_new: memory allocation falied\n" );

        return NULL;
    }

    cms->sce     = sce;
    cms->mes     = mes;
    cms->loldfac = loldfac;
    cms->lnewver = lnewver;
    cms->lnewfac = lnewfac;

    /*** for freeing unused edges later ***/
    g3dface_getOrphanedEdgesFromList ( loldfac, &cms->loldedg );
    g3dface_getSharedEdgesFromList   ( lnewfac, &cms->lnewedg );

    return cms;
}

/******************************************************************************/
static void urmcutmesh_free ( URMCUTMESH *cms ) {
    list_free ( &cms->loldfac, NULL );
    list_free ( &cms->loldedg, NULL );
    list_free ( &cms->lnewver, NULL );
    list_free ( &cms->lnewfac, NULL );
    list_free ( &cms->lnewedg, NULL );

    free ( cms );
}

/******************************************************************************/
static void cutMesh_free ( void    *data, 
                           uint32_t commit ) {
    URMCUTMESH *cms = ( URMCUTMESH * ) data;

    if ( commit ) {
        list_exec ( cms->loldfac, (void(*)(void*)) g3dface_free );
        list_exec ( cms->loldedg, (void(*)(void*)) g3dedge_free );
    } else {
        list_exec ( cms->lnewver, (void(*)(void*)) g3dvertex_free );
        list_exec ( cms->lnewfac, (void(*)(void*)) g3dface_free   );
        list_exec ( cms->lnewedg, (void(*)(void*)) g3dedge_free   );
    }

    urmcutmesh_free ( cms );
}

/******************************************************************************/
static void cutMesh_undo ( G3DURMANAGER *urm, 
                           void         *data, 
                           uint64_t      engine_flags ) {
    URMCUTMESH *cms = ( URMCUTMESH * ) data;
    G3DOBJECT *obj = ( G3DOBJECT * ) cms->mes;
    G3DMESH *mes = cms->mes;

    /*** We have to restore the selection in order to avoid crash ***/
    /*** because other functions are based on the select flag set ***/
    g3dmesh_unselectAllFaces ( cms->mes );

    list_execargdata ( cms->lnewfac, (void(*)(void*,void*)) g3dmesh_removeFace  , cms->mes );
    list_execargdata ( cms->lnewver, (void(*)(void*,void*)) g3dmesh_removeVertex, cms->mes );

    list_execargdata ( cms->loldfac, (void(*)(void*,void*)) g3dmesh_addFace, cms->mes );

    /*** Rebuild the cut mesh ***/
    g3dobject_update_r ( G3DOBJECTCAST(cms->sce), 0x00, engine_flags );
}

/******************************************************************************/
static void cutMesh_redo ( G3DURMANAGER *urm, 
                           void         *data, 
                           uint64_t      engine_flags ) {
    URMCUTMESH *cms = ( URMCUTMESH * ) data;
    G3DOBJECT *obj = ( G3DOBJECT * ) cms->mes;
    G3DMESH *mes = cms->mes;

    /*** We have to restore the selection in order to avoid crash ***/
    /*** because other functions are based on the select flag set ***/
    g3dmesh_unselectAllFaces ( cms->mes );

    list_execargdata ( cms->loldfac, (void(*)(void*,void*)) g3dmesh_removeFace, cms->mes );

    list_execargdata ( cms->lnewver, (void(*)(void*,void*)) g3dmesh_addVertex, cms->mes );
    list_execargdata ( cms->lnewfac, (void(*)(void*,void*)) g3dmesh_addFace  , cms->mes );

    /*** Rebuild the cut mesh ***/
    g3dobject_update_r ( G3DOBJECTCAST(cms->sce), 0x00, engine_flags );
}

/******************************************************************************/
void g3durm_mesh_cut ( G3DURMANAGER *urm,
                       G3DSCENE     *sce,
                       G3DMESH      *mes,
                       G3DFACE      *knife,
                       int           restricted,
                       uint64_t      engine_flags,
                       uint32_t      return_flags ) {
    LIST *loldfac = NULL,
         *lnewver = NULL, 
         *lnewfac = NULL;
    URMCUTMESH *cms;

    g3dmesh_cut ( mes, 
                  knife, 
                 &loldfac,
                 &lnewver,
                 &lnewfac, 
                  restricted, 
                  engine_flags );

    g3dobject_update_r ( G3DOBJECTCAST(sce), 0x00, engine_flags );

    cms = urmcutmesh_new ( sce, mes, loldfac, lnewver, lnewfac );

    g3durmanager_push ( urm,
                        cutMesh_undo,
                        cutMesh_redo,
                        cutMesh_free,
                        cms,
                        return_flags );
}
