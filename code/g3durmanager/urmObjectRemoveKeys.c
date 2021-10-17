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
URMOBJECTREMOVEKEYS *urmObjectRemoveKeys_new ( G3DOBJECT *obj ) {
    uint32_t size = sizeof ( URMOBJECTREMOVEKEYS );

    URMOBJECTREMOVEKEYS *ork = ( URMOBJECTREMOVEKEYS * ) calloc ( 0x01, size );

    if ( ork == NULL ) {
        fprintf ( stderr, "urmObjectRemoveKeys_new: memory allocation falied\n" );

        return NULL;
    }

    ork->obj  = obj;
    ork->lkey = list_copy ( obj->lselkey );

    return ork;
}

/******************************************************************************/
void urmObjectRemoveKeys_free ( URMOBJECTREMOVEKEYS *ork ) {
    free ( ork );
}

/******************************************************************************/
void objectRemoveKeys_free ( void *data, uint32_t commit ) {
    LIST *lork = ( LIST * ) data;
    LIST *ltmpork = lork;

    while ( ltmpork ) {
        URMOBJECTREMOVEKEYS *ork = ( URMOBJECTREMOVEKEYS * ) ltmpork->data;

        /*** Discard changes ***/
        if ( commit == 0x00 ) {
            list_free ( &ork->lkey, NULL );
        } else {
            list_free ( &ork->lkey, g3dkey_free );
        }

        ltmpork = ltmpork->next;
    }

    list_free ( &lork, urmObjectRemoveKeys_free );
}

/******************************************************************************/
void objectRemoveKeys_undo ( G3DURMANAGER *urm, 
                             void         *data,
                             uint64_t engine_flags ) {
    LIST *ltmpork = ( LIST * ) data;

    while ( ltmpork ) {
        URMOBJECTREMOVEKEYS *ork = ( URMOBJECTREMOVEKEYS * ) ltmpork->data;
        LIST *ltmpkey = ork->lkey;

        /*obj->lselkey = list_copy ( ork->lkey );*/

        /* perform the operation */
        while ( ltmpkey ) {
            G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

            g3dobject_addKey ( ork->obj, key );

            ltmpkey = ltmpkey->next;
        }

        ltmpork = ltmpork->next;
    }
}

/******************************************************************************/
void objectRemoveKeys_redo ( G3DURMANAGER *urm, 
                             void         *data,
                             uint64_t engine_flags ) {
    LIST *ltmpork = ( LIST * ) data;

    while ( ltmpork ) {
        URMOBJECTREMOVEKEYS *ork = ( URMOBJECTREMOVEKEYS * ) ltmpork->data;
        LIST *ltmpkey = ork->lkey;

        /*g3dobject_unselectAllKeys ( obj );*/

        /* perform the operation */
        while ( ltmpkey ) {
            G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

            g3dobject_removeKey ( ork->obj, key, 0x00 );

            ltmpkey = ltmpkey->next;
        }

        g3dobject_stitchTransformations ( ork->obj );

        ltmpork = ltmpork->next;
    }
}

/******************************************************************************/
void g3durm_objectList_removeSelectedKeys ( G3DURMANAGER *urm,
                                            LIST         *lobj,
                                            uint64_t engine_flags,
                                            uint32_t      return_flags ) {
    LIST *ltmpobj = lobj;
    LIST *lork = NULL;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;
        URMOBJECTREMOVEKEYS *ork = urmObjectRemoveKeys_new ( obj );
        LIST *ltmpkey = ork->lkey;

        g3dobject_unselectAllKeys ( obj );

        /* perform the operation */
        while ( ltmpkey ) {
            G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

            g3dobject_removeKey ( obj, key, 0x00 );

            ltmpkey = ltmpkey->next;
        }

        list_insert ( &lork, ork );

        g3dobject_stitchTransformations ( obj );

        ltmpobj = ltmpobj->next;
    }

    g3durmanager_push ( urm, objectRemoveKeys_undo,
                             objectRemoveKeys_redo,
                             objectRemoveKeys_free, lork, return_flags );
}
