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
URMADDTEXTURE *urmaddtexture_new ( G3DMESH     *mes,
                                   G3DTEXTURE  *tex,
                                   uint64_t engine_flags ) {
    uint32_t structsize = sizeof ( URMADDTEXTURE );

    URMADDTEXTURE *uat = ( URMADDTEXTURE * ) calloc ( 0x01, structsize );

    if ( uat == NULL ) {
        fprintf ( stderr, "%s: memory allocation falied\n", __func__ );

        return NULL;
    }

    uat->obj = mes;
    uat->tex = tex;

    return uat;
}

/******************************************************************************/
void urmaddtexture_free ( URMADDTEXTURE *uat ) {
    free ( uat );
}

/******************************************************************************/
void addTexture_free ( void *data, uint32_t commit ) {
    URMADDTEXTURE *uat = ( URMADDTEXTURE * ) data;

    if ( commit ) {

    }

    urmaddtexture_free ( uat );
}

/******************************************************************************/
void addTexture_undo ( G3DURMANAGER *urm, void *data, uint32_t flags ) {
    URMADDTEXTURE *uat = ( URMADDTEXTURE * ) data;

    if ( uat->obj->type & MESH ) {
        G3DMESH *mes = ( G3DMESH * ) uat->obj;

        g3dmesh_removeTexture ( mes, uat->tex );

        /*** Rebuild the mesh with modifiers (e.g for displacement) ***/
        g3dmesh_update ( mes, 
                         NULL,
                         NULL,
                         NULL,
                         RESETMODIFIERS, flags );
    }
}

/******************************************************************************/
void addTexture_redo ( G3DURMANAGER *urm, void *data, uint32_t flags ) {
    URMADDTEXTURE *uat = ( URMADDTEXTURE * ) data;

    if ( uat->obj->type & MESH ) {
        G3DMESH *mes = ( G3DMESH * ) uat->obj;

        g3dmesh_addTexture ( mes, uat->tex );

        /*** Rebuild the mesh with modifiers (e.g for displacement) ***/
        g3dmesh_update ( mes, 
                         NULL,
                         NULL,
                         NULL,
                         RESETMODIFIERS, flags );
    }
}

/******************************************************************************/
void g3durm_mesh_addTexture ( G3DURMANAGER *urm,
                              G3DMESH      *mes,
                              G3DTEXTURE   *tex,
                              uint64_t engine_flags,
                              uint32_t      return_flags ) {
    URMADDTEXTURE *uat;

    /*** save state ***/
    uat = urmaddtexture_new ( mes, tex, engine_flags );

    g3dmesh_addTexture ( mes, tex );

    /*** Rebuild the mesh with modifiers (e.g for displacement) ***/
    g3dmesh_update ( mes, 
                     NULL,
                     NULL,
                     NULL,
                     RESETMODIFIERS, engine_flags );

    g3durmanager_push ( urm, 
                        addTexture_undo,
                        addTexture_redo,
                        addTexture_free, 
                        uat,
                        return_flags );
}
