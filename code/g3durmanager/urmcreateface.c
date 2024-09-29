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
    LIST    *addedEdgeList;
} URMCREATEFACE;

/******************************************************************************/
static URMCREATEFACE *urmcreateface_new ( G3DSCENE *sce,
                                          G3DMESH  *mes, 
                                          G3DFACE  *fac,
                                          LIST     *addedEdgeList ) {
    uint32_t structsize = sizeof ( URMCREATEFACE );
    LIST lnewfac = { .next = NULL, .prev = NULL, .data = fac };

    URMCREATEFACE *cfs = ( URMCREATEFACE * ) calloc ( 0x01, structsize );

    if ( cfs == NULL ) {
        fprintf ( stderr, "urmcreateface_new: memory allocation falied\n" );

        return NULL;
    }

    cfs->sce           = sce;
    cfs->mes           = mes;
    cfs->fac           = fac;
    cfs->addedEdgeList = addedEdgeList;

    return cfs;
}

/******************************************************************************/
static void urmcreateface_free ( URMCREATEFACE *cfs ) {
    list_free ( &cfs->addedEdgeList, NULL );

    free ( cfs );
}

/******************************************************************************/
static void createFace_free ( void    *data, 
                              uint32_t commit ) {
    URMCREATEFACE *cfs = ( URMCREATEFACE * ) data;

    if ( commit ) {

    } else { /*** Discard changes ***/
        list_exec ( cfs->addedEdgeList, (void(*)(void*)) g3dedge_free );

        g3dface_free ( cfs->fac );
    }

    urmcreateface_free ( cfs );
}

/******************************************************************************/
static void createFace_undo ( G3DURMANAGER *urm, 
                              void         *data, 
                              uint64_t      engine_flags ) {
    URMCREATEFACE *cfs = ( URMCREATEFACE * ) data;
    G3DMESH *mes = cfs->mes;
    G3DFACE *fac = cfs->fac;
    uint32_t i = 0x00;

    /* Note: will remove unused edges automatically */
    g3dmesh_removeFace ( mes, fac, NULL );

    /*** Rebuild the subdivided mesh ***/
    g3dobject_update_r ( G3DOBJECTCAST(cfs->sce), 0x00, engine_flags );
}

/******************************************************************************/
static void createFace_redo ( G3DURMANAGER *urm, 
                              void         *data, 
                              uint64_t      engine_flags ) {
    URMCREATEFACE *cfs = ( URMCREATEFACE * ) data;
    G3DMESH *mes = cfs->mes;
    G3DFACE *fac = cfs->fac;
    uint32_t i = 0x00;
    LIST *ltmpnewedg = cfs->addedEdgeList;

    /* add added egdes for linking with faces */
    while ( ltmpnewedg ) {
        g3dmesh_addEdge ( cfs->mes, ( G3DEDGE * ) ltmpnewedg->data );
        ltmpnewedg = ltmpnewedg->next;
    }

    /* Note: it wil not add any edge, as we added them just before */
    g3dmesh_addFace ( mes, fac, NULL );

    /*** Rebuild the subdivided mesh ***/
    g3dobject_update_r ( G3DOBJECTCAST(cfs->sce), 0x00, engine_flags );
}

/******************************************************************************/
G3DFACE *g3durm_mesh_createFace ( G3DURMANAGER *urm,
                                  G3DSCENE     *sce,
                                  G3DMESH      *mes, 
                                  G3DVERTEX    *v0,
                                  G3DVERTEX    *v1,
                                  G3DVERTEX    *v2,
                                  G3DVERTEX    *v3,
                                  uint64_t      engine_flags,
                                  uint32_t      return_flags ) {

    /*** if ver[0x03] != NULL, then we did not create any ***/ 
    /*** triangle. Then, create a QUAD ***/
    G3DFACE *fac = ( v3 == v1 ) ? g3dtriangle_new ( v0, v1, v2 )
                                : g3dquad_new ( v0, v1, v3, v2 );
    URMCREATEFACE *cfs;
    LIST *addedEdgeList = NULL;

    g3dmesh_addFace ( mes, fac, &addedEdgeList );

    if ( g3dface_checkOrientation ( fac ) ) {
        g3dface_invertNormal ( fac );
    }

    /*** regenerate subdivision buffer ***/
    g3dobject_update_r ( G3DOBJECTCAST(sce), 0x00, engine_flags );

    cfs = urmcreateface_new ( sce, mes, fac, addedEdgeList );

    g3durmanager_push ( urm,
                        createFace_undo,
                        createFace_redo,
                        createFace_free,
                        cfs,
                        return_flags );

    return fac;
}
