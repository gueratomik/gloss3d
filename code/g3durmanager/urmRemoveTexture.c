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
static URMREMOVETEXTURE *urmremovetexture_new ( G3DSCENE   *sce,
                                                G3DMESH    *mes,
                                                G3DTEXTURE *tex,
                                                uint64_t engine_flags ) {
    uint32_t structsize = sizeof ( URMREMOVETEXTURE );

    URMREMOVETEXTURE *urt = ( URMREMOVETEXTURE * ) calloc ( 0x01, structsize );

    if ( urt == NULL ) {
        fprintf ( stderr, "%s: memory allocation falied\n", __func__ );

        return NULL;
    }

    urt->sce = sce;
    urt->obj = tex->obj; /*** could also be the "mes" parameter ***/
    urt->tex = tex;

    urt->lfacgrp = list_copy ( tex->lfacgrp );

    return urt;
}

/******************************************************************************/
void urmremovetexture_free ( URMREMOVETEXTURE *urt ) {
    list_free ( &urt->lfacgrp, NULL );

    free ( urt );
}

/******************************************************************************/
static void removeTexture_free ( void    *data,
                                 uint32_t commit ) {
    URMREMOVETEXTURE *urt = ( URMREMOVETEXTURE * ) data;

    if ( commit ) {
        g3dtexture_free ( urt->tex );
    }

    urmremovetexture_free ( urt );
}

/******************************************************************************/
static void removeTexture_undo ( G3DURMANAGER *urm,
                                 void         *data,
                                 uint64_t      engine_flags ) {
    URMREMOVETEXTURE *urt = ( URMREMOVETEXTURE * ) data;

    if ( urt->obj->type & MESH ) {
        G3DMESH *mes = ( G3DMESH * ) urt->obj;
        LIST *ltmpfacgrp = urt->lfacgrp;

        g3dmesh_addTexture ( mes, urt->tex );

        while ( ltmpfacgrp ) {
            G3DFACEGROUP *facgrp = ( G3DFACEGROUP * ) ltmpfacgrp->data;

            g3dtexture_restrictFacegroup ( urt->tex, facgrp );

            ltmpfacgrp = ltmpfacgrp->next;
        }

        /*** Rebuild the mesh with modifiers (e.g for displacement) ***/
        g3dobject_update_r ( G3DOBJECTCAST(urt->sce), 0x00, engine_flags );
    }
}

/******************************************************************************/
static void removeTexture_redo ( G3DURMANAGER *urm,
                                 void         *data,
                                 uint64_t      engine_flags ) {
    URMREMOVETEXTURE *urt = ( URMREMOVETEXTURE * ) data;

    if ( urt->obj->type & MESH ) {
        G3DMESH *mes = ( G3DMESH * ) urt->obj;

        g3dmesh_removeTexture ( mes, urt->tex );

        /*** Rebuild the mesh with modifiers (e.g for displacement) ***/
        g3dobject_update_r ( G3DOBJECTCAST(urt->sce), 0x00, engine_flags );
    }
}

/******************************************************************************/
void g3durm_mesh_removeTexture ( G3DURMANAGER *urm,
                                 G3DSCENE     *sce,
                                 G3DMESH      *mes,
                                 G3DTEXTURE   *tex, 
                                 uint64_t      engine_flags,
                                 uint32_t      return_flags ) {
    LIST *loldselobj = NULL;
    URMREMOVETEXTURE *urt;

    /*** save state ***/
    urt = urmremovetexture_new ( sce, mes, tex, engine_flags );

    g3durmanager_push ( urm, 
                        removeTexture_undo,
                        removeTexture_redo,
                        removeTexture_free, 
                        urt,
                        return_flags );
}
