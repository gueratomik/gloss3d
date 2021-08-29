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
typedef struct _URMADDTAG {
    G3DOBJECT *obj;
    G3DTAG *tag;
} URMADDTAG;

/******************************************************************************/
URMADDTAG *urmaddtag_new ( G3DOBJECT *obj,
                           G3DTAG    *tag ) {
    uint32_t structsize = sizeof ( URMADDTAG );

    URMADDTAG *uat = ( URMADDTAG * ) calloc ( 0x01, structsize );

    if ( uat == NULL ) {
        fprintf ( stderr, "%s: memory allocation falied\n", __func__ );

        return NULL;
    }

    uat->obj = obj;
    uat->tag = tag;

    return uat;
}

/******************************************************************************/
void urmaddtag_free ( URMADDTAG *uat ) {
    free ( uat );
}

/******************************************************************************/
void addTag_free ( void *data, uint32_t commit ) {
    URMADDTAG *uat = ( URMADDTAG * ) data;

    if ( commit == 0x00 ) {
        g3dtag_free ( uat->tag );
    }

    urmaddtag_free ( uat );
}

/******************************************************************************/
void addTag_undo ( G3DURMANAGER *urm, void *data, uint32_t flags ) {
    URMADDTAG *uat = ( URMADDTAG * ) data;

    g3dobject_removeTag ( uat->obj, uat->tag );
}

/******************************************************************************/
void addTag_redo ( G3DURMANAGER *urm, void *data, uint32_t flags ) {
    URMADDTAG *uat = ( URMADDTAG * ) data;

    g3dobject_addTag ( uat->obj, uat->tag );
}

/******************************************************************************/
void g3durm_selection_addTag ( G3DURMANAGER *urm,
                               G3DOBJECT    *obj,
                               G3DTAG       *tag,
                               uint64_t      engine_flags,
                               uint32_t      return_flags ) {
    URMADDTAG *uat = urmaddtag_new ( obj, tag );

    g3dobject_addTag ( obj, tag );

    g3durmanager_push ( urm, 
                        addTag_undo,
                        addTag_redo,
                        addTag_free, 
                        uat,
                        return_flags );
}
