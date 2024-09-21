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
typedef struct _URMREMOVEUVMAP {
    G3DSCENE *sce;
    G3DMESH  *mes;
    G3DUVMAP *uvmap;
    LIST     *lolduvset;
    LIST     *loldtex;
} URMREMOVEUVMAP;

/******************************************************************************/
static URMREMOVEUVMAP *urmremoveuvmap_new ( G3DSCENE *sce,
                                            G3DMESH  *mes,
                                            G3DUVMAP *uvmap,
                                            LIST     *lolduvset,
                                            LIST     *loldtex,
                                            uint64_t engine_flags ) {
    uint32_t structsize = sizeof ( URMREMOVEUVMAP );

    URMREMOVEUVMAP *uru = ( URMREMOVEUVMAP * ) calloc ( 0x01, structsize );

    if ( uru == NULL ) {
        fprintf ( stderr, "%s: memory allocation falied\n", __func__ );

        return NULL;
    }

    uru->sce       = sce;
    uru->mes       = mes;
    uru->uvmap     = uvmap;
    uru->lolduvset = lolduvset;
    uru->loldtex   = loldtex;

    return uru;
}

/******************************************************************************/
static void urmremoveuvmap_free ( URMREMOVEUVMAP *uru ) {
    list_free ( &uru->loldtex, NULL );

    free ( uru );
}

/******************************************************************************/
static void removeUVMap_free ( void *data, uint32_t commit ) {
    URMREMOVEUVMAP *uru = ( URMREMOVEUVMAP * ) data;

    if ( commit ) {
        g3dobject_free ( G3DOBJECTCAST(uru->uvmap) );

        list_free ( &uru->lolduvset, LIST_FUNCDATA(g3duvset_free) );
    } else {
        list_free ( &uru->lolduvset, NULL );
    }

    urmremoveuvmap_free ( uru );
}

/******************************************************************************/
static void removeUVMap_undo ( G3DURMANAGER *urm,
                               void         *data,
                               uint64_t      engine_flags ) {
    URMREMOVEUVMAP *uru = ( URMREMOVEUVMAP * ) data;
    LIST *ltmpolduvset = uru->lolduvset;
    LIST *ltmpoldtex = uru->loldtex;

    /*** this is put BEFORE g3dmesh_addUVMap because g3dmesh_update is called */
    /*** in g3dmesh_addUVMap and e.g displacement would need the uvsets to be */
    /*** attached to the faces. Maybe g3dmesh_update should be taken out of **/
    /*** of g3dmesh_addUVMap ***/
    while ( ltmpolduvset ) {
        G3DUVSET *uvset = ( G3DUVSET * ) ltmpolduvset->data;

        g3dface_addUVSet ( uvset->fac, uvset );

        ltmpolduvset = ltmpolduvset->next;
    }

    while ( ltmpoldtex ) {
        G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmpoldtex->data;

        tex->map = uru->uvmap;

        ltmpoldtex = ltmpoldtex->next;
    }

    g3dmesh_addUVMap ( uru->mes, uru->uvmap, NULL, engine_flags );

    g3dobject_update( G3DOBJECTCAST(uru->sce), 0x00, engine_flags );
}

/******************************************************************************/
static void removeUVMap_redo ( G3DURMANAGER *urm, 
                               void         *data,
                               uint64_t engine_flags ) {
    URMREMOVEUVMAP *uru = ( URMREMOVEUVMAP * ) data;

    g3dmesh_removeUVMap ( uru->mes, 
                          uru->uvmap, 
                          NULL, /*** no need to backp uvsets this time **/
                          NULL, /** no need to backup textures this time **/
                          engine_flags );

    g3dobject_update( G3DOBJECTCAST(uru->sce), 0x00, engine_flags );
}

/******************************************************************************/
void g3durm_mesh_removeUVMap ( G3DURMANAGER *urm,
                               G3DSCENE     *sce,
                               G3DMESH      *mes,
                               G3DUVMAP     *uvmap, 
                               uint64_t      engine_flags,
                               uint32_t      return_flags ) {
    LIST *lolduvset = NULL;
    LIST *loldtex = NULL;
    URMREMOVEUVMAP *uru;

    g3dmesh_removeUVMap ( mes, uvmap, &lolduvset, &loldtex, engine_flags );

    g3dobject_update( G3DOBJECTCAST(sce), 0x00, engine_flags );

    /*** save state ***/
    uru = urmremoveuvmap_new ( sce,
                               mes,
                               uvmap,
                               lolduvset,
                               loldtex,
                               engine_flags );


    g3durmanager_push ( urm, 
                        removeUVMap_undo,
                        removeUVMap_redo,
                        removeUVMap_free, 
                        uru,
                        return_flags );
}
