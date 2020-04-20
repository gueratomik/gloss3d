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
URMSPLITMESH *urmSplitMesh_new ( G3DMESH *mes, 
                                 G3DMESH *splmes, 
                                 LIST    *loldver,
                                 LIST    *loldfac ) {
    uint32_t structsize = sizeof ( URMSPLITMESH );

    URMSPLITMESH *sms = ( URMSPLITMESH * ) calloc ( 0x01, structsize );

    if ( sms == NULL ) {
        fprintf ( stderr, "urmSplitMesh_new: memory allocation falied\n" );

        return NULL;
    }

    sms->mes     = mes;
    sms->splmes  = splmes;
    sms->loldver = loldver;
    sms->loldfac = loldfac;

    return sms;
}

/******************************************************************************/
void urmSplitMesh_free ( URMSPLITMESH *sms ) {
    free ( sms );
}

/******************************************************************************/
void splitMesh_free ( void *data, uint32_t commit ) {
    URMSPLITMESH *sms = ( URMSPLITMESH * ) data;

    if ( commit ) {
        list_exec ( sms->loldver, (void(*)(void*)) g3dvertex_free );
        list_exec ( sms->loldfac, (void(*)(void*)) g3dface_free   );
    } else {
        g3dmesh_free ( sms->splmes );
        list_free ( &sms->loldver, NULL );
        list_free ( &sms->loldfac, NULL );
    }

    urmSplitMesh_free ( sms );
}

/******************************************************************************/
void splitMesh_undo ( G3DURMANAGER *urm, void *data, uint32_t engine_flags ) {
    URMSPLITMESH *sms = ( URMSPLITMESH * ) data;
    G3DMESH *mes = sms->mes;

    g3dmesh_unselectAllFaces ( mes );

    /*** restore deleted vertices ***/
    list_execargdata ( sms->loldver, (void(*)(void*,void*)) g3dmesh_addVertex, mes );

    /*** restore deleted faces ***/
    list_execargdata ( sms->loldfac, (void(*)(void*,void*)) g3dmesh_addFace, mes );

    list_exec ( sms->loldfac, (void(*)(void*)) g3dface_linkVertices );

    /*** delete created mesh ***/
    g3dobject_removeChild ( ((G3DOBJECT*)mes)->parent, sms->splmes, engine_flags );

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
void splitMesh_redo ( G3DURMANAGER *urm, void *data, uint32_t engine_flags ) {
    URMSPLITMESH *sms = ( URMSPLITMESH * ) data;
    G3DMESH *mes = sms->mes;

    g3dmesh_unselectAllVertices ( mes );

    /*** delete old faces ***/
    list_execargdata ( sms->loldfac, (void(*)(void*,void*)) g3dmesh_removeFace, mes );

    /*** Deleted old vertices ***/
    list_execargdata ( sms->loldver, (void(*)(void*,void*)) g3dmesh_removeVertex, mes );

    /*** add created vertex ***/
    g3dobject_addChild ( ((G3DOBJECT*)mes)->parent, sms->splmes, engine_flags );


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
void g3durm_mesh_split ( G3DURMANAGER *urm, 
                         G3DMESH      *mes,
                         uint32_t      splID,
                         uint32_t      keep,
                         uint32_t      engine_flags,
                         uint32_t      return_flags ) {
    URMSPLITMESH *sms;
    LIST *loldfac = NULL;
    LIST *loldver = NULL;
    G3DMESH *splmes;

    splmes = g3dmesh_splitSelectedFaces ( mes, 
                                          splID, 
                                          keep, 
                                         &loldver, 
                                         &loldfac, 
                                          engine_flags );

    g3dobject_addChild ( ((G3DOBJECT*)mes)->parent, splmes, engine_flags );

    /*** Rebuild the mesh with modifiers ***/
    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          UPDATEFACEPOSITION |
                          UPDATEFACENORMAL   |
                          UPDATEVERTEXNORMAL |
                          RESETMODIFIERS, engine_flags );

    g3dmesh_update ( splmes, NULL, /*** update vertices    ***/
                             NULL, /*** update edges       ***/
                             NULL, /*** update faces       ***/
                             UPDATEFACEPOSITION |
                             UPDATEFACENORMAL   |
                             UPDATEVERTEXNORMAL |
                             COMPUTEUVMAPPING   |
                             RESETMODIFIERS,
                             engine_flags );

    sms = urmSplitMesh_new ( mes, splmes, loldver, loldfac );

    g3durmanager_push ( urm, splitMesh_undo,
                             splitMesh_redo,
                             splitMesh_free, sms, return_flags );
}
