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
typedef struct _URMREMOVETAG {
    G3DSCENE  *sce;
    G3DOBJECT *obj;
    G3DTAG    *tag;
} URMREMOVETAG;

/******************************************************************************/
static URMREMOVETAG *urmremovetag_new ( G3DSCENE  *sce,
                                        G3DOBJECT *obj,
                                        G3DTAG    *tag ) {
    uint32_t structsize = sizeof ( URMREMOVETAG );

    URMREMOVETAG *urt = ( URMREMOVETAG * ) calloc ( 0x01, structsize );

    if ( urt == NULL ) {
        fprintf ( stderr, "%s: memory allocation falied\n", __func__ );

        return NULL;
    }

    urt->sce = sce;
    urt->obj = obj;
    urt->tag = tag;

    return urt;
}

/******************************************************************************/
static void urmremovetag_free ( URMREMOVETAG *urt ) {
    free ( urt );
}

/******************************************************************************/
static void removeTag_free ( void *data, uint32_t commit ) {
    URMREMOVETAG *urt = ( URMREMOVETAG * ) data;

    if ( commit ) {
        g3dtag_free ( urt->tag );
    }

    urmremovetag_free ( urt );
}

/******************************************************************************/
static void removeTag_undo ( G3DURMANAGER *urm, void *data, uint64_t engine_flags ) {
    URMREMOVETAG *urt = ( URMREMOVETAG * ) data;

    g3dobject_addTag ( urt->obj, urt->tag );

    g3dobject_update_r ( G3DOBJECTCAST(urt->sce), 0x00, engine_flags );
}

/******************************************************************************/
static void removeTag_redo ( G3DURMANAGER *urm, void *data, uint64_t engine_flags ) {
    URMREMOVETAG *urt = ( URMREMOVETAG * ) data;

    g3dobject_removeTag ( urt->obj, urt->tag );

    g3dobject_update_r ( G3DOBJECTCAST(urt->sce), 0x00, engine_flags );
}

/******************************************************************************/
void g3durm_selection_removeTag ( G3DURMANAGER *urm,
                                  G3DSCENE     *sce,
                                  G3DOBJECT    *obj,
                                  G3DTAG       *tag,
                                  uint64_t      engine_flags,
                                  uint32_t      return_flags ) {
    URMREMOVETAG *urt = urmremovetag_new ( sce, obj, tag );

    g3dobject_removeTag ( obj, tag );

    g3dobject_update_r ( G3DOBJECTCAST(sce), 0x00, engine_flags );

    g3durmanager_push ( urm, 
                        removeTag_undo,
                        removeTag_redo,
                        removeTag_free, 
                        urt,
                        return_flags );
}
