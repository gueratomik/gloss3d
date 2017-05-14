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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
URMADDVERTEX *urmaddvertex_new ( G3DMESH *mes, G3DVERTEX *ver ) {
    uint32_t structsize = sizeof ( URMADDVERTEX );

    URMADDVERTEX *avs = ( URMADDVERTEX * ) calloc ( 0x01, structsize );

    if ( avs == NULL ) {
        fprintf ( stderr, "urmaddvertex_new: memory allocation falied\n" );

        return NULL;
    }

    avs->mes = mes;
    avs->ver = ver;


    return avs;
}

/******************************************************************************/
void urmaddvertex_free ( URMADDVERTEX *avs ) {

    free ( avs );
}

/******************************************************************************/
void addVertex_free ( void *data, uint32_t commit ) {
    URMADDVERTEX *avs = ( URMADDVERTEX * ) data;

    /*** Discard changes ***/
    if ( commit == 0x00 ) {
        g3dvertex_free ( avs->ver );
    }

    urmaddvertex_free ( avs );
}

/******************************************************************************/
void addVertex_undo ( G3DURMANAGER *urm, void *data, uint32_t engine_flags ) {
    URMADDVERTEX *avs = ( URMADDVERTEX * ) data;
    G3DOBJECT *obj = ( G3DOBJECT * ) avs->mes;
    G3DMESH *mes = avs->mes;
    G3DVERTEX *ver = avs->ver;
    uint32_t i = 0x00;

    g3dmesh_removeVertex ( mes, ver );

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
void addVertex_redo ( G3DURMANAGER *urm, void *data, uint32_t engine_flags ) {
    URMADDVERTEX *avs = ( URMADDVERTEX * ) data;
    G3DOBJECT *obj = ( G3DOBJECT * ) avs->mes;
    G3DMESH *mes = avs->mes;
    G3DVERTEX *ver = avs->ver;
    uint32_t i = 0x00;

    g3dmesh_addVertex ( mes, ver );

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
void g3durm_mesh_addVertex ( G3DURMANAGER *urm,
                             G3DMESH *mes, 
                             G3DVERTEX *ver, uint32_t return_flags ) {
    URMADDVERTEX *avs;

    avs = urmaddvertex_new ( mes, ver );

    g3durmanager_push ( urm, addVertex_undo,
                             addVertex_redo,
                             addVertex_free, avs, return_flags );
}
