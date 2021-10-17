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
URMOBJECTPOSE *urmObjectPose_new ( G3DOBJECT *obj, 
                                   float      frame,
                                   G3DKEY    *key,
                                   G3DKEY    *overwrittenKey ) {
    uint32_t size = sizeof ( URMOBJECTPOSE );

    URMOBJECTPOSE *op = ( URMOBJECTPOSE * ) calloc ( 0x01, size );

    if ( op == NULL ) {
        fprintf ( stderr, "urmObjectPose_new: memory allocation falied\n" );

        return NULL;
    }

    op->obj   = obj;
    op->frame = frame;
    op->key   = key;
    op->overwrittenKey = overwrittenKey;

    memcpy ( &op->keypos, &obj->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &op->keyrot, &obj->rot, sizeof ( G3DVECTOR ) );
    memcpy ( &op->keysca, &obj->sca, sizeof ( G3DVECTOR ) );

    return op;
}

/******************************************************************************/
void urmObjectPose_free ( URMOBJECTPOSE *op ) {
    free ( op );
}

/******************************************************************************/
void objectPose_free ( void *data, uint32_t commit ) {
    LIST *ltmpop = ( LIST * ) data;

    while ( ltmpop ) {
        URMOBJECTPOSE *op = ( URMOBJECTPOSE * ) ltmpop->data;

        /*** Discard changes ***/
        if ( commit == 0x00 ) {
            g3dkey_free ( op->key );
        } else {
            if ( op->overwrittenKey ) {
                g3dkey_free ( op->overwrittenKey );
            }
        }

        ltmpop = ltmpop->next;
    }

    list_free ( &ltmpop, urmObjectPose_free );
}

/******************************************************************************/
void objectPose_undo ( G3DURMANAGER *urm, 
                       void         *data,
                       uint64_t engine_flags ) {
    LIST *ltmpop = ( LIST * ) data;

    while ( ltmpop ) {
        URMOBJECTPOSE *op = ( URMOBJECTPOSE * ) ltmpop->data;

        g3dobject_removeKey ( op->obj, 
                              op->key, 0x01 );

        if ( op->overwrittenKey ) {
            g3dobject_addKey ( op->obj, op->overwrittenKey );
        }

        ltmpop = ltmpop->next;
    }
}

/******************************************************************************/
void objectPose_redo ( G3DURMANAGER *urm, 
                       void         *data,
                       uint64_t engine_flags ) {
    LIST *ltmpop = ( LIST * ) data;

    while ( ltmpop ) {
        URMOBJECTPOSE *op = ( URMOBJECTPOSE * ) ltmpop->data;

        op->key = g3dobject_pose ( op->obj, 
                                   op->frame, 
                                  &op->keypos, 
                                  &op->keyrot, 
                                  &op->keysca,
                                  &op->overwrittenKey,
                                   op->key->flags );

        ltmpop = ltmpop->next;
    }
}

/******************************************************************************/
void g3durm_object_pose ( G3DURMANAGER *urm,
                          LIST         *lobj,
                          float         frame,
                          uint32_t      key_flags,
                          uint64_t engine_flags,
                          uint32_t      return_flags ) {
    LIST *ltmpobj = lobj;
    LIST *lop = NULL;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;
        G3DKEY *key, *overwrittenKey = NULL;
        URMOBJECTPOSE *op;

        /* perform the operation */
        key = g3dobject_pose ( obj, frame, &obj->pos, 
                                           &obj->rot, 
                                           &obj->sca, &overwrittenKey, key_flags );

        /* remember it for undoing */
        op = urmObjectPose_new ( obj, frame, key, overwrittenKey );

        list_insert ( &lop, op );

        ltmpobj = ltmpobj->next;
    }

    g3durmanager_push ( urm, objectPose_undo,
                             objectPose_redo,
                             objectPose_free, lop, return_flags );
}
