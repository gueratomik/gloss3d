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
typedef struct _URMCREATEFACE {
    G3DSCENE *sce;
    G3DMESH *mes;
    G3DFACE *fac;
    LIST    *lnewedg;
} URMCREATEFACE;

/******************************************************************************/
static URMCREATEFACE *urmcreateface_new ( G3DSCENE *sce,
                                          G3DMESH  *mes, 
                                          G3DFACE  *fac ) {
    uint32_t structsize = sizeof ( URMCREATEFACE );
    LIST lnewfac = { .next = NULL, .prev = NULL, .data = fac };

    URMCREATEFACE *cfs = ( URMCREATEFACE * ) calloc ( 0x01, structsize );

    if ( cfs == NULL ) {
        fprintf ( stderr, "urmcreateface_new: memory allocation falied\n" );

        return NULL;
    }

    cfs->sce = sce;
    cfs->mes = mes;
    cfs->fac = fac;

    g3dface_getSharedEdgesFromList   ( &lnewfac, &cfs->lnewedg );


    return cfs;
}

/******************************************************************************/
static void urmcreateface_free ( URMCREATEFACE *cfs ) {
    list_free ( &cfs->lnewedg, NULL );

    free ( cfs );
}

/******************************************************************************/
static void createFace_free ( void    *data, 
                              uint32_t commit ) {
    URMCREATEFACE *cfs = ( URMCREATEFACE * ) data;

    /*** Discard changes ***/
    if ( commit == 0x00 ) {
        list_exec ( cfs->lnewedg, (void(*)(void*)) g3dedge_free );

        g3dface_free ( cfs->fac );
    }

    urmcreateface_free ( cfs );
}

/******************************************************************************/
static void createFace_undo ( G3DURMANAGER *urm, 
                              void         *data, 
                              uint64_t      engine_flags ) {
    URMCREATEFACE *cfs = ( URMCREATEFACE * ) data;
    G3DOBJECT *obj = ( G3DOBJECT * ) cfs->mes;
    G3DMESH *mes = cfs->mes;
    G3DFACE *fac = cfs->fac;
    uint32_t i = 0x00;

    g3dmesh_removeFace ( mes, fac );

    g3dmesh_updateBbox ( mes );

    /*** TODO: understand why those 2 calls are needed. It's becoming ***/
    /*** messy I  dont even know why the faces dont get correctly ***/
    /*** update by the  above call to g3dface_update ***/
    g3dmesh_faceNormal   ( mes );
    g3dmesh_vertexNormal ( mes );

    /*** Rebuild the subdivided mesh ***/
    g3dobject_update_r ( G3DOBJECTCAST(cfs->sce), 0x00, engine_flags );
}

/******************************************************************************/
static void createFace_redo ( G3DURMANAGER *urm, 
                              void         *data, 
                              uint64_t      engine_flags ) {
    URMCREATEFACE *cfs = ( URMCREATEFACE * ) data;
    G3DOBJECT *obj = ( G3DOBJECT * ) cfs->mes;
    G3DMESH *mes = cfs->mes;
    G3DFACE *fac = cfs->fac;
    uint32_t i = 0x00;

    g3dmesh_addFace ( mes, fac );

    g3dmesh_updateBbox ( mes );

    /*** TODO: understand why those 2 calls are needed. It's becoming ***/
    /*** messy I  dont even know why the faces dont get correctly ***/
    /*** update by the  above call to g3dface_update ***/
    g3dmesh_faceNormal   ( mes );
    g3dmesh_vertexNormal ( mes );

    /*** Rebuild the subdivided mesh ***/
    g3dobject_update_r ( G3DOBJECTCAST(cfs->sce), 0x00, engine_flags );
}

/******************************************************************************/
void g3durm_mesh_createFace ( G3DURMANAGER *urm,
                              G3DSCENE     *sce,
                              G3DMESH      *mes, 
                              G3DFACE      *fac, 
                              uint32_t      return_flags ) {
    URMCREATEFACE *cfs;

    cfs = urmcreateface_new ( sce, mes, fac );

    g3durmanager_push ( urm,
                        createFace_undo,
                        createFace_redo,
                        createFace_free,
                        cfs,
                        return_flags );
}
