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
static URMUNTRIANGULATE *urmuntriangulate_new ( G3DMESH *mes, 
                                                LIST    *loldfac,
                                                LIST    *lnewfac ) {
    uint32_t structsize = sizeof ( URMUNTRIANGULATE );

    URMUNTRIANGULATE *ums = ( URMUNTRIANGULATE * ) calloc ( 0x01, structsize );

    if ( ums == NULL ) {
        fprintf ( stderr, "urmuntriangulate_new: memory allocation falied\n" );

        return NULL;
    }

    ums->mes     = mes;
    ums->loldfac = loldfac;
    ums->lnewfac = lnewfac;

    /*** for freeing unused edges later ***/
    g3dface_getOrphanedEdgesFromList ( loldfac, &ums->loldedg );
    g3dface_getSharedEdgesFromList   ( lnewfac, &ums->lnewedg );

    return ums;
}

/******************************************************************************/
static void urmuntriangulate_free ( URMUNTRIANGULATE *ums ) {
    list_free ( &ums->loldfac, NULL );
    list_free ( &ums->lnewfac, NULL );
    list_free ( &ums->loldedg, NULL );
    list_free ( &ums->lnewedg, NULL );

    free ( ums );
}

/******************************************************************************/
void unTriangulate_free ( void *data, uint32_t commit ) {
    URMUNTRIANGULATE *ums = ( URMUNTRIANGULATE * ) data;

    if ( commit ) {
        list_exec ( ums->loldfac, (void(*)(void*)) g3dface_free );
        list_exec ( ums->loldedg, (void(*)(void*)) g3dedge_free );
    } else {
        list_exec ( ums->lnewfac, (void(*)(void*)) g3dface_free );
        list_exec ( ums->lnewedg, (void(*)(void*)) g3dedge_free );
    }

    urmuntriangulate_free ( ums );
}

/******************************************************************************/
void unTriangulate_undo ( G3DURMANAGER *urm, 
                          void         *data, 
                          uint64_t      engine_flags ) {
    URMUNTRIANGULATE *ums = ( URMUNTRIANGULATE * ) data;
    G3DMESH *mes = ums->mes;

    /*** We have to restore the selection in order to avoid crash ***/
    /*** because other functions are based on the select flag set ***/
    g3dmesh_unselectAllFaces ( mes );

    list_execargdata ( ums->lnewfac, (void(*)(void*,void*)) g3dmesh_removeFace     , ums->mes );

    list_execargdata ( ums->loldfac, (void(*)(void*,void*)) g3dmesh_addSelectedFace, ums->mes );

    g3dmesh_faceNormal   ( mes );
    g3dmesh_vertexNormal ( mes );

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
void unTriangulate_redo ( G3DURMANAGER *urm, 
                          void         *data, 
                          uint64_t      engine_flags ) {
    URMUNTRIANGULATE *ums = ( URMUNTRIANGULATE * ) data;
    G3DMESH *mes = ums->mes;

    /*** We have to restore the selection in order to avoid crash ***/
    /*** because other functions are based on the select flag set ***/
    g3dmesh_unselectAllFaces ( mes );

    list_execargdata ( ums->loldfac, (void(*)(void*,void*)) g3dmesh_removeFace     , ums->mes );

    list_execargdata ( ums->lnewfac, (void(*)(void*,void*)) g3dmesh_addSelectedFace, ums->mes );

    g3dmesh_faceNormal   ( mes );
    g3dmesh_vertexNormal ( mes );

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
void g3durm_mesh_untriangulate ( G3DURMANAGER *urm, 
                                 G3DMESH      *mes,
                                 uint64_t      engine_flags,
                                 uint32_t      return_flags ) {
    LIST *loldfac = NULL,
         *lnewfac = NULL,
         *loldedg = NULL,
         *lnewedg = NULL;
    URMTRIANGULATE *ums;

    g3dmesh_untriangulate ( mes, &loldfac, &lnewfac );

    /*** Rebuild the mesh with modifiers ***/
    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          UPDATEFACEPOSITION |
                          UPDATEFACENORMAL   |
                          UPDATEVERTEXNORMAL |
                          RESETMODIFIERS, engine_flags );

    g3dmesh_faceNormal   ( mes );
    g3dmesh_vertexNormal ( mes );

    ums = urmuntriangulate_new ( mes, loldfac, lnewfac );

    g3durmanager_push ( urm, unTriangulate_undo,
                             unTriangulate_redo,
                             unTriangulate_free, ums, return_flags );
}

/******************************************************************************/
void g3durm_mesh_triangulate ( G3DURMANAGER *urm, 
                               G3DMESH      *mes, 
                               int           clockwise,
                               uint64_t      engine_flags,
                               uint32_t      return_flags ) {
    LIST *loldfac = NULL,
         *lnewfac = NULL,
         *loldedg = NULL,
         *lnewedg = NULL;
    URMTRIANGULATE *ums;

    g3dmesh_triangulate ( mes, &loldfac, &lnewfac, clockwise );

    /*** Rebuild the mesh with modifiers ***/
    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          UPDATEFACEPOSITION |
                          UPDATEFACENORMAL   |
                          UPDATEVERTEXNORMAL |
                          RESETMODIFIERS, engine_flags );

    /*** Triangulate and unTriagulate feature use ***/
    /*** the same functions and data structures. ***/
    ums = urmuntriangulate_new ( mes, loldfac, lnewfac );

    g3durmanager_push ( urm, unTriangulate_undo,
                             unTriangulate_redo,
                             unTriangulate_free, ums, return_flags );
}
