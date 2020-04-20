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
URMEXTRUDEMESH *urmextrudemesh_new ( G3DMESH *mes, LIST *loriver,
                                                   LIST *loldfac,
                                                   LIST *lnewver,
                                                   LIST *lnewfac,
                                                   G3DVECTOR *oldpos,
                                                   G3DVECTOR *newpos ) {
    uint32_t structsize = sizeof ( URMEXTRUDEMESH );

    URMEXTRUDEMESH *ems = ( URMEXTRUDEMESH * ) calloc ( 0x01, structsize );

    if ( ems == NULL ) {
        fprintf ( stderr, "urmextrudemesh_new: memory allocation falied\n" );

        return NULL;
    }

    ems->mes     = mes;
    ems->loriver = loriver;
    ems->loldfac = loldfac;
    ems->lnewver = lnewver;
    ems->lnewfac = lnewfac;
    ems->oldpos  = oldpos;
    ems->newpos  = newpos;


    return ems;
}

/******************************************************************************/
void urmextrudemesh_free ( URMEXTRUDEMESH *ems ) {
    list_free ( &ems->loriver, NULL );
    list_free ( &ems->loldfac, NULL );
    list_free ( &ems->lnewver, NULL );
    list_free ( &ems->lnewfac, NULL );

    free ( ems->oldpos );
    free ( ems->newpos );


    free ( ems );
}

/******************************************************************************/
void extrudeMesh_free ( void *data, uint32_t commit ) {
    URMEXTRUDEMESH *ems = ( URMEXTRUDEMESH * ) data;

    /*** Discard changes ***/
    if ( commit ) {
        list_exec ( ems->loldfac, (void(*)(void*)) g3dface_free );
    } else {
        list_exec ( ems->lnewfac, (void(*)(void*)) g3dface_free   );
        list_exec ( ems->lnewver, (void(*)(void*)) g3dvertex_free );
    }

    urmextrudemesh_free ( ems );
}

/******************************************************************************/
void extrudeMesh_undo ( G3DURMANAGER *urm, void *data, uint32_t engine_flags ) {
    URMEXTRUDEMESH *ems = ( URMEXTRUDEMESH * ) data;
    G3DMESH *mes = ems->mes;

    list_execargdata ( ems->lnewfac, (void(*)(void*,void*)) g3dmesh_removeFace  , mes );
    list_execargdata ( ems->lnewver, (void(*)(void*,void*)) g3dmesh_removeVertex, mes );

    list_execargdata ( ems->loldfac, (void(*)(void*,void*)) g3dmesh_addFace, mes );

    /*** this is need because the first call to g3dmesh_removeFace unlinked ***/
    /*** the face list for each vertex belonging to the face ***/
    list_exec ( ems->loldfac, (void(*)(void*)) g3dface_linkVertices );

    /*** reset vertex position ***/
    g3dvertex_setPositionFromList ( ems->loriver, ems->oldpos );

    g3dmesh_updateBbox ( mes );

    /*** Rebuild the subdivided mesh ***/
    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          UPDATEFACEPOSITION |
                          UPDATEFACENORMAL   |
                          UPDATEVERTEXNORMAL |
                          RESETMODIFIERS, engine_flags );
}

/******************************************************************************/
void extrudeMesh_redo ( G3DURMANAGER *urm, void *data, uint32_t engine_flags ) {
    URMEXTRUDEMESH *ems = ( URMEXTRUDEMESH * ) data;
    G3DMESH *mes = ems->mes;

    list_execargdata ( ems->loldfac, (void(*)(void*,void*)) g3dmesh_removeFace, mes );

    list_execargdata ( ems->lnewver, (void(*)(void*,void*)) g3dmesh_addVertex, mes );
    list_execargdata ( ems->lnewfac, (void(*)(void*,void*)) g3dmesh_addFace  , mes );

    /*** this is need because the first call to g3dmesh_removeFace unlinked ***/
    /*** the face list for each vertex belonging to the face ***/
    list_exec ( ems->lnewfac, (void(*)(void*)) g3dface_linkVertices );

    /*** reset vertex position ***/
    g3dvertex_setPositionFromList ( ems->loriver, ems->newpos );

    g3dmesh_updateBbox ( mes );

    /*** Rebuild the subdivided mesh ***/
    g3dmesh_update ( mes, NULL,
                          NULL,
                          NULL,
                          UPDATEFACEPOSITION |
                          UPDATEFACENORMAL   |
                          UPDATEVERTEXNORMAL |
                          RESETMODIFIERS, engine_flags );
}

/******************************************************************************/
void g3durm_mesh_extrude ( G3DURMANAGER *urm, G3DMESH *mes,
                                              LIST *loriver,
                                              LIST *loldfac,
                                              LIST *lnewver,
                                              LIST *lnewfac,
                                              G3DVECTOR *oldpos,
                                              G3DVECTOR *newpos,
                                              uint32_t return_flags ) {
    URMEXTRUDEMESH *ems;

    ems = urmextrudemesh_new ( mes, loriver,
                                    loldfac,
                                    lnewver,
                                    lnewfac,
                                    oldpos,
                                    newpos );

    g3durmanager_push ( urm, extrudeMesh_undo,
                             extrudeMesh_redo,
                             extrudeMesh_free, ems, return_flags );
}
