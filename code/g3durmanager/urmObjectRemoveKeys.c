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

            list_free ( &ork->lremovedPosSegments, NULL );
            list_free ( &ork->lremovedRotSegments, NULL );
            list_free ( &ork->lremovedScaSegments, NULL );

            list_free ( &ork->laddedPosSegments, g3dcubicsegment_free );
            list_free ( &ork->laddedRotSegments, g3dcubicsegment_free );
            list_free ( &ork->laddedScaSegments, g3dcubicsegment_free );
        } else {
            list_free ( &ork->lkey, g3dkey_free );

            list_free ( &ork->lremovedPosSegments, g3dcubicsegment_free );
            list_free ( &ork->lremovedRotSegments, g3dcubicsegment_free );
            list_free ( &ork->lremovedScaSegments, g3dcubicsegment_free );

            list_free ( &ork->laddedPosSegments, NULL );
            list_free ( &ork->laddedRotSegments, NULL );
            list_free ( &ork->laddedScaSegments, NULL );
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

        /*** Note: here we cannot use g3dobject_addKey because  ***/
        /*** it also performs segment creation and stuff ***/
        list_execargdata ( ork->lkey, list_insert, &ork->obj->lkey );

        ork->obj->nbkey += list_count ( ork->lkey );

        list_execargdata ( ork->lremovedPosSegments, g3dcurve_addSegment, ork->obj->curve[0x00] );
        list_execargdata ( ork->lremovedRotSegments, g3dcurve_addSegment, ork->obj->curve[0x01] );
        list_execargdata ( ork->lremovedScaSegments, g3dcurve_addSegment, ork->obj->curve[0x02] );

        list_execargdata ( ork->laddedPosSegments, g3dcurve_removeSegment, ork->obj->curve[0x00] );
        list_execargdata ( ork->laddedRotSegments, g3dcurve_removeSegment, ork->obj->curve[0x01] );
        list_execargdata ( ork->laddedScaSegments, g3dcurve_removeSegment, ork->obj->curve[0x02] );

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

        /*** Note: here we cannot use g3dobject_addKey because  ***/
        /*** it also performs segment creation and stuff ***/
        list_execargdata ( ork->lkey, list_remove, &ork->obj->lkey );

        ork->obj->nbkey -= list_count ( ork->lkey );

        list_execargdata ( ork->lremovedPosSegments, g3dcurve_removeSegment, ork->obj->curve[0x00] );
        list_execargdata ( ork->lremovedRotSegments, g3dcurve_removeSegment, ork->obj->curve[0x01] );
        list_execargdata ( ork->lremovedScaSegments, g3dcurve_removeSegment, ork->obj->curve[0x02] );

        list_execargdata ( ork->laddedPosSegments, g3dcurve_addSegment, ork->obj->curve[0x00] );
        list_execargdata ( ork->laddedRotSegments, g3dcurve_addSegment, ork->obj->curve[0x01] );
        list_execargdata ( ork->laddedScaSegments, g3dcurve_addSegment, ork->obj->curve[0x02] );

        ltmpork = ltmpork->next;
    }
}

/******************************************************************************/
void g3durm_objectList_removeSelectedKeys ( G3DURMANAGER *urm,
                                            LIST         *lobj,
                                            uint64_t      engine_flags,
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

            g3dobject_removeKey ( obj, 
                                  key, 
                                  0x00,
                                 &ork->lremovedPosSegments,
                                 &ork->lremovedRotSegments,
                                 &ork->lremovedScaSegments,
                                  NULL,
                                  NULL,
                                  NULL );

            ltmpkey = ltmpkey->next;
        }

        list_insert ( &lork, ork );

        g3dobject_stitchTransformations ( obj,
                                         &ork->laddedPosSegments,
                                         &ork->laddedRotSegments,
                                         &ork->laddedScaSegments );

        ltmpobj = ltmpobj->next;
    }

    g3durmanager_push ( urm, objectRemoveKeys_undo,
                             objectRemoveKeys_redo,
                             objectRemoveKeys_free, lork, return_flags );
}
