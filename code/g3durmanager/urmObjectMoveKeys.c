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
typedef struct _URMOBJECTMOVEKEYS {
    G3DOBJECT *obj;
    float     *newFrame;
    float     *oldFrame;
    LIST      *lselkey;
    uint32_t   nbkey;
    LIST      *lremovedKeys;
    LIST      *laddedPosSegments;
    LIST      *laddedRotSegments;
    LIST      *laddedScaSegments;
    LIST      *lremovedPosSegments;
    LIST      *lremovedRotSegments;
    LIST      *lremovedScaSegments;
} URMOBJECTMOVEKEYS;

/******************************************************************************/
URMOBJECTMOVEKEYS *urmObjectMoveKeys_new ( G3DOBJECT *obj ) {
    uint32_t size = sizeof ( URMOBJECTMOVEKEYS );
    URMOBJECTMOVEKEYS *op = ( URMOBJECTMOVEKEYS * ) calloc ( 0x01, size );
    uint32_t i = 0x00;

    if ( op == NULL ) {
        fprintf ( stderr, "urmObjectMoveKeys_new: memory allocation falied\n" );

        return NULL;
    }

    op->obj  = obj;
    op->lselkey = list_copy ( obj->lselkey );
    op->nbkey = list_count ( op->lselkey );

    return op;
}

/******************************************************************************/
static void urmObjectMoveKeys_saveNewFrame ( URMOBJECTMOVEKEYS *op ) {
    LIST *ltmpkey = op->lselkey;
    uint32_t i = 0x00;

    op->newFrame = ( float * ) calloc ( op->nbkey, sizeof ( float ) );

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        op->newFrame[i++] = key->frame;

        ltmpkey = ltmpkey->next;
    }
}

/******************************************************************************/
static void urmObjectMoveKeys_saveOldFrame ( URMOBJECTMOVEKEYS *op ) {
    LIST *ltmpkey = op->lselkey;
    uint32_t i = 0x00;

    op->oldFrame = ( float * ) calloc ( op->nbkey, sizeof ( float ) );

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        op->oldFrame[i++] = key->frame;

        ltmpkey = ltmpkey->next;
    }
}

/******************************************************************************/
static void urmObjectMoveKeys_restoreNewFrame ( URMOBJECTMOVEKEYS *op ) {
    LIST *ltmpkey = op->lselkey;
    uint32_t i = 0x00;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        key->frame = op->newFrame[i++];

        ltmpkey = ltmpkey->next;
    }
}

/******************************************************************************/
static void urmObjectMoveKeys_restoreOldFrame ( URMOBJECTMOVEKEYS *op ) {
    LIST *ltmpkey = op->lselkey;
    uint32_t i = 0x00;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        key->frame = op->oldFrame[i++];

        ltmpkey = ltmpkey->next;
    }
}

/******************************************************************************/
void urmObjectMoveKeys_free ( URMOBJECTMOVEKEYS *op ) {
    list_free ( &op->lselkey, NULL );

    if (  op->newFrame ) free ( op->newFrame );
    if (  op->oldFrame ) free ( op->oldFrame );

    free ( op );
}

/******************************************************************************/
void objectMoveKeys_free ( void *data, uint32_t commit ) {
    LIST *lop = ( LIST * ) data;
    LIST *ltmpop = lop;

    while ( ltmpop ) {
        URMOBJECTMOVEKEYS *op = ( URMOBJECTMOVEKEYS * ) ltmpop->data;

        /*** Discard changes ***/
        if ( commit == 0x00 ) {
            list_free ( &op->lremovedKeys, NULL );

            list_free ( &op->lremovedPosSegments, NULL );
            list_free ( &op->lremovedRotSegments, NULL );
            list_free ( &op->lremovedScaSegments, NULL );

            list_free ( &op->laddedPosSegments, LIST_FUNCDATA(g3dcubicsegment_free) );
            list_free ( &op->laddedRotSegments, LIST_FUNCDATA(g3dcubicsegment_free) );
            list_free ( &op->laddedScaSegments, LIST_FUNCDATA(g3dcubicsegment_free) );
        } else {
            list_free ( &op->lremovedKeys, LIST_FUNCDATA(g3dkey_free) );

            list_free ( &op->lremovedPosSegments, LIST_FUNCDATA(g3dcubicsegment_free) );
            list_free ( &op->lremovedRotSegments, LIST_FUNCDATA(g3dcubicsegment_free) );
            list_free ( &op->lremovedScaSegments, LIST_FUNCDATA(g3dcubicsegment_free) );

            list_free ( &op->laddedPosSegments, NULL );
            list_free ( &op->laddedRotSegments, NULL );
            list_free ( &op->laddedScaSegments, NULL );
        }

        ltmpop = ltmpop->next;
    }

    list_free ( &lop, LIST_FUNCDATA(urmObjectMoveKeys_free) );
}

/******************************************************************************/
void objectMoveKeys_undo ( G3DURMANAGER *urm, 
                             void         *data,
                             uint64_t engine_flags ) {
    LIST *ltmpop = ( LIST * ) data;

    while ( ltmpop ) {
        URMOBJECTMOVEKEYS *op = ( URMOBJECTMOVEKEYS * ) ltmpop->data;

        urmObjectMoveKeys_restoreOldFrame ( op );

        /*** Note: here we cannot use g3dobject_addKey because  ***/
        /*** it also performs segment creation and stuff ***/
        list_execargdata ( op->lremovedKeys, LIST_FUNCARGDATA(list_insert), &op->obj->lkey );

        op->obj->nbkey += list_count ( op->lremovedKeys );

        list_execargdata ( op->lremovedPosSegments, LIST_FUNCARGDATA(g3dcurve_addSegment), op->obj->curve[0x00] );
        list_execargdata ( op->lremovedRotSegments, LIST_FUNCARGDATA(g3dcurve_addSegment), op->obj->curve[0x01] );
        list_execargdata ( op->lremovedScaSegments, LIST_FUNCARGDATA(g3dcurve_addSegment), op->obj->curve[0x02] );

        list_execargdata ( op->laddedPosSegments, LIST_FUNCARGDATA(g3dcurve_removeSegment), op->obj->curve[0x00] );
        list_execargdata ( op->laddedRotSegments, LIST_FUNCARGDATA(g3dcurve_removeSegment), op->obj->curve[0x01] );
        list_execargdata ( op->laddedScaSegments, LIST_FUNCARGDATA(g3dcurve_removeSegment), op->obj->curve[0x02] );

        ltmpop = ltmpop->next;
    }
}

/******************************************************************************/
void objectMoveKeys_redo ( G3DURMANAGER *urm, 
                             void         *data,
                             uint64_t engine_flags ) {
    LIST *ltmpop = ( LIST * ) data;

    while ( ltmpop ) {
        URMOBJECTMOVEKEYS *op = ( URMOBJECTMOVEKEYS * ) ltmpop->data;

        urmObjectMoveKeys_restoreNewFrame ( op );

        /*** Note: here we cannot use g3dobject_addKey because  ***/
        /*** it also performs segment creation and stuff ***/
        list_execargdata ( op->lremovedKeys, LIST_FUNCARGDATA(list_remove), &op->obj->lkey );

        op->obj->nbkey -= list_count ( op->lremovedKeys );

        list_execargdata ( op->lremovedPosSegments, LIST_FUNCARGDATA(g3dcurve_removeSegment), op->obj->curve[0x00] );
        list_execargdata ( op->lremovedRotSegments, LIST_FUNCARGDATA(g3dcurve_removeSegment), op->obj->curve[0x01] );
        list_execargdata ( op->lremovedScaSegments, LIST_FUNCARGDATA(g3dcurve_removeSegment), op->obj->curve[0x02] );

        list_execargdata ( op->laddedPosSegments, LIST_FUNCARGDATA(g3dcurve_addSegment), op->obj->curve[0x00] );
        list_execargdata ( op->laddedRotSegments, LIST_FUNCARGDATA(g3dcurve_addSegment), op->obj->curve[0x01] );
        list_execargdata ( op->laddedScaSegments, LIST_FUNCARGDATA(g3dcurve_addSegment), op->obj->curve[0x02] );

        ltmpop = ltmpop->next;
    }
}

/******************************************************************************/
void g3durm_objectList_driftSelectedKeys ( G3DURMANAGER *urm,
                                           LIST         *lobj,
                                           float         drift,
                                           uint64_t      engine_flags,
                                           uint32_t      return_flags ) {
    LIST *ltmpobj = lobj;
    LIST *lop = NULL;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;
        URMOBJECTMOVEKEYS *op = urmObjectMoveKeys_new ( obj );

        urmObjectMoveKeys_saveOldFrame ( op );

        g3dobject_driftSelectedKeys ( obj,
                                      drift,
                                     &op->lremovedKeys,
                                     &op->lremovedPosSegments,
                                     &op->lremovedRotSegments,
                                     &op->lremovedScaSegments,
                                     &op->laddedPosSegments,
                                     &op->laddedRotSegments,
                                     &op->laddedScaSegments );

        urmObjectMoveKeys_saveNewFrame ( op );

        list_insert ( &lop, op );

        ltmpobj = ltmpobj->next;
    }

    g3durmanager_push ( urm, objectMoveKeys_undo,
                             objectMoveKeys_redo,
                             objectMoveKeys_free, lop, return_flags );
}

/******************************************************************************/
void g3durm_objectList_scaleSelectedKeys ( G3DURMANAGER *urm,
                                           LIST         *lobj,
                                           float         factor, 
                                           float         reference,
                                           uint64_t      engine_flags,
                                           uint32_t      return_flags ) {
    LIST *ltmpobj = lobj;
    LIST *lop = NULL;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;
        URMOBJECTMOVEKEYS *op = urmObjectMoveKeys_new ( obj );

        urmObjectMoveKeys_saveOldFrame ( op );

        g3dobject_scaleSelectedKeys ( obj,
                                      factor, 
                                      reference,
                                     &op->lremovedKeys,
                                     &op->lremovedPosSegments,
                                     &op->lremovedRotSegments,
                                     &op->lremovedScaSegments,
                                     &op->laddedPosSegments,
                                     &op->laddedRotSegments,
                                     &op->laddedScaSegments );

        urmObjectMoveKeys_saveNewFrame ( op );

        list_insert ( &lop, op );

        ltmpobj = ltmpobj->next;
    }

    g3durmanager_push ( urm, objectMoveKeys_undo,
                             objectMoveKeys_redo,
                             objectMoveKeys_free, lop, return_flags );
}
