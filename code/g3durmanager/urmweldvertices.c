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
#include <g3durmanager.h>

/******************************************************************************/
URMWELDVERTICES *urmweldvertices_new ( G3DMESH  *mes, LIST *loldver,
                                                      LIST *loldfac,
                                                      LIST *lnewfac,
                                                      G3DVERTEX *newver ) {
    uint32_t structsize = sizeof ( URMWELDVERTICES );

    URMWELDVERTICES *wvs = ( URMWELDVERTICES * ) calloc ( 0x01, structsize );

    if ( wvs == NULL ) {
        fprintf ( stderr, "urmweldvertices_new: memory allocation falied\n" );

        return NULL;
    }

    wvs->mes     = mes;
    wvs->loldver = loldver;
    wvs->loldfac = loldfac;
    wvs->lnewfac = lnewfac;
    wvs->newver  = newver;


    return wvs;
}

/******************************************************************************/
void urmweldvertices_free ( URMWELDVERTICES *wvs ) {
    list_free ( &wvs->loldver, NULL );
    list_free ( &wvs->loldfac, NULL );
    list_free ( &wvs->lnewfac, NULL );

    free ( wvs );
}

/******************************************************************************/
void weldVertices_free ( void *data, uint32_t commit ) {
    URMWELDVERTICES *wvs = ( URMWELDVERTICES * ) data;

    if ( commit ) {
        list_exec ( wvs->loldver, (void(*)(void*)) g3dvertex_free );
        list_exec ( wvs->loldfac, (void(*)(void*)) g3dface_free   );
    } else {
        g3dvertex_free ( wvs->newver );
        list_exec ( wvs->lnewfac, (void(*)(void*)) g3dface_free   );
    }

    urmweldvertices_free ( wvs );
}

/******************************************************************************/
void weldVertices_undo ( G3DURMANAGER *urm, void *data, uint32_t engine_flags ) {
    URMWELDVERTICES *wvs = ( URMWELDVERTICES * ) data;
    G3DMESH *mes = wvs->mes;

    g3dmesh_unselectAllVertices ( mes );

    /*** delete added faces ***/
    list_execargdata ( wvs->lnewfac, (void(*)(void*,void*)) g3dmesh_removeFace, mes );

    /*** delete created vertex ***/
    g3dmesh_removeVertex ( mes, wvs->newver );

    /*** restore deleted vertices ***/
    list_execargdata ( wvs->loldver, (void(*)(void*,void*)) g3dmesh_addSelectedVertex, mes );

    /*** restore deleted faces ***/
    list_execargdata ( wvs->loldfac, (void(*)(void*,void*)) g3dmesh_addFace, mes );

    list_exec ( wvs->loldfac, (void(*)(void*)) g3dface_linkVertices );

    /*** Rebuild the mesh with modifiers ***/
    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          UPDATEFACEPOSITION |
                          UPDATEFACENORMAL   |
                          UPDATEVERTEXNORMAL |
                          RESETMODIFIERS, engine_flags );
}

/******************************************************************************/
void weldVertices_redo ( G3DURMANAGER *urm, void *data, uint32_t engine_flags ) {
    URMWELDVERTICES *wvs = ( URMWELDVERTICES * ) data;
    G3DMESH *mes = wvs->mes;

    g3dmesh_unselectAllVertices ( mes );

    /*** delete old faces ***/
    list_execargdata ( wvs->loldfac, (void(*)(void*,void*)) g3dmesh_removeFace, mes );

    /*** Deleted welded vertices ***/
    list_execargdata ( wvs->loldver, (void(*)(void*,void*)) g3dmesh_removeVertex, mes );

    /*** add created vertex ***/
    g3dmesh_addSelectedVertex ( mes, wvs->newver );

    /*** restore new faces ***/
    list_execargdata ( wvs->lnewfac, (void(*)(void*,void*)) g3dmesh_addFace, mes );

    list_exec ( wvs->lnewfac, (void(*)(void*)) g3dface_linkVertices );

    /*** Rebuild the mesh with modifiers ***/
    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          UPDATEFACEPOSITION |
                          UPDATEFACENORMAL   |
                          UPDATEVERTEXNORMAL |
                          RESETMODIFIERS, engine_flags );
}

/******************************************************************************/
void g3durm_mesh_weldSelectedVertices ( G3DURMANAGER *urm, 
                                        G3DMESH *mes, uint32_t type,
                                                      uint32_t engine_flags,
                                                      uint32_t return_flags ) {
    URMWELDVERTICES *wvs;
    LIST *loldver = list_copy ( mes->lselver );
    LIST *loldfac = NULL,
         *lnewfac = NULL;
    G3DVERTEX *newver;

    newver = g3dmesh_weldSelectedVertices ( mes, type, &loldfac, &lnewfac );

    /*** Rebuild the mesh with modifiers ***/
    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          UPDATEFACEPOSITION |
                          UPDATEFACENORMAL   |
                          UPDATEVERTEXNORMAL |
                          RESETMODIFIERS, engine_flags );

    wvs = urmweldvertices_new ( mes, loldver, loldfac, lnewfac, newver );

    g3durmanager_push ( urm, weldVertices_undo,
                             weldVertices_redo,
                             weldVertices_free, wvs, return_flags );
}
