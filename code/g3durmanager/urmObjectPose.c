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
URMOBJECTPOSE *urmObjectPose_new ( G3DOBJECT *obj, 
                                   float      frame,
                                   G3DKEY    *key ) {
    uint32_t size = sizeof ( URMOBJECTPOSE );

    URMOBJECTPOSE *op = ( URMOBJECTPOSE * ) calloc ( 0x01, size );

    if ( op == NULL ) {
        fprintf ( stderr, "urmObjectPose_new: memory allocation falied\n" );

        return NULL;
    }

    op->obj   = obj;
    op->frame = frame;
    op->key   = key;

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
    URMOBJECTPOSE *op = ( URMOBJECTPOSE * ) data;

    /*** Discard changes ***/
    if ( commit == 0x00 ) {
        g3dkey_free ( op->key );
    }

    urmObjectPose_free ( op );
}

/******************************************************************************/
void objectPose_undo ( G3DURMANAGER *urm, 
                       void         *data,
                       uint32_t      engine_flags ) {
    URMOBJECTPOSE *op = ( URMOBJECTPOSE * ) data;

    g3dobject_removeKey ( op->obj, 
                          op->key );
}

/******************************************************************************/
void objectPose_redo ( G3DURMANAGER *urm, 
                       void         *data,
                       uint32_t      engine_flags ) {
    URMOBJECTPOSE *op = ( URMOBJECTPOSE * ) data;
    op->key = g3dobject_pose ( op->obj, 
                               op->frame, 
                              &op->keypos, 
                              &op->keyrot, 
                              &op->keysca, 
                               op->key->flags );
}

/******************************************************************************/
void g3durm_object_pose ( G3DURMANAGER     *urm,
                          G3DOBJECT        *obj,
                          float             frame,
                          G3DVECTOR        *pos,
                          G3DVECTOR        *rot,
                          G3DVECTOR        *sca,
                          uint32_t          key_flags,
                          uint32_t          engine_flags,
                          uint32_t          return_flags ) {
    URMOBJECTPOSE *op;
    G3DKEY *key;

    /* perform the operation */
    key = g3dobject_pose ( obj, frame, pos, rot, sca, key_flags );

    /* remember it for undoing */
    op = urmObjectPose_new ( obj, frame, key );

    g3durmanager_push ( urm, objectPose_undo,
                             objectPose_redo,
                             objectPose_free, op, return_flags );
}
