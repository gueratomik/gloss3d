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
#include <g3dengine/g3dengine.h>

/******************************************************************************/
void g3dkey_recordPosCurvePoint ( G3DKEY *key, G3DVECTOR *pos ) {
    memcpy ( &key->posCurvePoint.pos, pos, sizeof ( G3DVECTOR ) );
}

/******************************************************************************/
void g3dkey_recordRotCurvePoint ( G3DKEY *key, G3DVECTOR *rot ) {
    memcpy ( &key->rotCurvePoint.pos, rot, sizeof ( G3DVECTOR ) );
}

/******************************************************************************/
void g3dkey_recordScaCurvePoint ( G3DKEY *key, G3DVECTOR *sca ) {
    memcpy ( &key->scaCurvePoint.pos, sca, sizeof ( G3DVECTOR ) );
}

/******************************************************************************/
void g3dkey_recordAllCurvePoint ( G3DKEY    *key, 
                                  G3DVECTOR *pos, 
                                  G3DVECTOR *rot, 
                                  G3DVECTOR *sca ) {
    if ( pos ) g3dkey_recordPosCurvePoint ( key, pos );
    if ( rot ) g3dkey_recordRotCurvePoint ( key, rot );
    if ( sca ) g3dkey_recordScaCurvePoint ( key, sca );
}

/******************************************************************************/
void g3dkey_setLoopFrame ( G3DKEY *key, float loopFrame ) {
    key->loopFrame = loopFrame;
}

/******************************************************************************/
void g3dkey_setLoop ( G3DKEY *key ) {
    key->flags |= KEYLOOP;
}

/******************************************************************************/
void g3dkey_unsetLoop ( G3DKEY *key ) {
    key->flags &= (~KEYLOOP);
}

/******************************************************************************/
void g3dkey_setSelected ( G3DKEY *key ) {
    key->flags |= KEYSELECTED;
}

/******************************************************************************/
void g3dkey_unsetSelected ( G3DKEY *key ) {
    key->flags &= (~KEYSELECTED);
}

/******************************************************************************/
void g3dkey_unsetLoopFrameFromList ( LIST *lkey ) {
    LIST *ltmpkey = lkey;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        g3dkey_unsetLoop ( key );

        ltmpkey = ltmpkey->next;
    }
}

/******************************************************************************/
void g3dkey_unsetLoopFromList ( LIST *lkey ) {
    LIST *ltmpkey = lkey;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        g3dkey_unsetLoop ( key );

        ltmpkey = ltmpkey->next;
    }
}

/******************************************************************************/
void g3dkey_setLoopFromList ( LIST *lkey ) {
    LIST *ltmpkey = lkey;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        g3dkey_setLoop ( key );

        ltmpkey = ltmpkey->next;
    }
}

/******************************************************************************/
void g3dkey_setLoopFrameFromList ( LIST *lkey, float loopFrame ) {
    LIST *ltmpkey = lkey;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        g3dkey_setLoopFrame ( key, loopFrame );

        ltmpkey = ltmpkey->next;
    }
}

/******************************************************************************/
void g3dkey_setFlagFromList ( LIST *lkey, uint32_t flag ) {
    LIST *ltmpkey = lkey;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        key->flags |= flag;

        ltmpkey = ltmpkey->next;
    }
}

/******************************************************************************/
void g3dkey_unsetFlagFromList ( LIST *lkey, uint32_t flag ) {
    LIST *ltmpkey = lkey;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        key->flags &= (~flag);

        ltmpkey = ltmpkey->next;
    }
}

/******************************************************************************/
void g3dkey_setUseScalingFromList ( LIST *lkey ) {
    g3dkey_setFlagFromList ( lkey, KEYSCALING );
}

/******************************************************************************/
void g3dkey_setUseRotationFromList ( LIST *lkey ) {
    g3dkey_setFlagFromList ( lkey, KEYROTATION );
}

/******************************************************************************/
void g3dkey_setUsePositionFromList ( LIST *lkey ) {
    g3dkey_setFlagFromList ( lkey, KEYPOSITION );
}

/******************************************************************************/
uint32_t g3dkey_getFlagFromList ( LIST *lkey, uint32_t flag ) {
    LIST *ltmpkey = lkey;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        if ( key->flags & flag ) return 0x01;

        ltmpkey = ltmpkey->next;
    }

    return 0x00;
}

/******************************************************************************/
uint32_t g3dkey_getUseScalingFromList ( LIST *lkey ) {
    return g3dkey_getFlagFromList ( lkey, KEYSCALING );
}

/******************************************************************************/
uint32_t g3dkey_getUseRotationFromList ( LIST *lkey ) {
    return g3dkey_getFlagFromList ( lkey, KEYROTATION );
}

/******************************************************************************/
uint32_t g3dkey_getUsePositionFromList ( LIST *lkey ) {
    return g3dkey_getFlagFromList ( lkey, KEYPOSITION );
}

/******************************************************************************/
uint32_t g3dkey_getLoopFrameFromList ( LIST *lkey, float *loopFrame ) {
    uint32_t nbkey = 0x00;
    LIST *ltmpkey = lkey;
    uint32_t loopFlag = 0x00;

    (*loopFrame) = 0.0f;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        (*loopFrame) += key->loopFrame;

        loopFlag |= ( key->flags & KEYLOOP );

        nbkey++;

        ltmpkey = ltmpkey->next;
    }

    if ( nbkey ) {
        (*loopFrame) /= nbkey;
    }

    return loopFlag;
}

/******************************************************************************/
void g3dkey_setTransformationFromList ( LIST *lkey, G3DVECTOR *pos,
                                                    G3DVECTOR *rot,
                                                    G3DVECTOR *sca ) {
    LIST *ltmpkey = lkey;

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        memcpy ( &key->pos, pos, sizeof ( G3DVECTOR ) );
        memcpy ( &key->rot, pos, sizeof ( G3DVECTOR ) );
        memcpy ( &key->sca, pos, sizeof ( G3DVECTOR ) );


        ltmpkey = ltmpkey->next;
    }
}

/******************************************************************************/
void g3dkey_getTransformationFromList ( LIST *lkey, G3DVECTOR *pos,
                                                    G3DVECTOR *rot,
                                                    G3DVECTOR *sca ) {
    uint32_t nbkey = 0x00;
    LIST *ltmpkey = lkey;

    g3dvector_init ( pos, 0.0f, 0.0f, 0.0f, 1.0f );
    g3dvector_init ( rot, 0.0f, 0.0f, 0.0f, 1.0f );
    g3dvector_init ( sca, 0.0f, 0.0f, 0.0f, 1.0f );

    while ( ltmpkey ) {
        G3DKEY *key = ( G3DKEY * ) ltmpkey->data;

        pos->x += key->pos.x;
        pos->y += key->pos.y;
        pos->z += key->pos.z;

        rot->x += key->rot.x;
        rot->y += key->rot.y;
        rot->z += key->rot.z;

        sca->x += key->sca.x;
        sca->y += key->sca.y;
        sca->z += key->sca.z;

        nbkey++;


        ltmpkey = ltmpkey->next;
    }

    if ( nbkey ) {
        pos->x /= nbkey;
        pos->y /= nbkey;
        pos->z /= nbkey;

        rot->x /= nbkey;
        rot->y /= nbkey;
        rot->z /= nbkey;

        sca->x /= nbkey;
        sca->y /= nbkey;
        sca->z /= nbkey;
    }
}

/******************************************************************************/
void g3dkey_free ( G3DKEY *key ) {
    free ( key );
}

/******************************************************************************/
void g3dkey_init ( G3DKEY *key, float frame, 
                                G3DVECTOR *pos,
                                G3DVECTOR *rot,
                                G3DVECTOR *sca, uint32_t key_flags ) {
    key->frame = frame;
    key->flags = key_flags;

    memcpy ( &key->pos, pos, sizeof ( G3DVECTOR ) );
    memcpy ( &key->rot, rot, sizeof ( G3DVECTOR ) );
    memcpy ( &key->sca, sca, sizeof ( G3DVECTOR ) );

    g3dkey_recordAllCurvePoint ( key, pos, rot, sca );
}

/******************************************************************************/
G3DKEY *g3dkey_new ( float frame, G3DVECTOR *pos,
                                  G3DVECTOR *rot,
                                  G3DVECTOR *sca, uint32_t key_flags ) {
    G3DKEY *key = ( G3DKEY * ) calloc ( 0x01, sizeof ( G3DKEY ) );

    if ( key == NULL ) {
        fprintf ( stderr, "g3dkey_new: memory allocation failed\n" );

        return NULL;
    }

    g3dkey_init ( key, frame, pos, rot, sca, key_flags );

    return key;
}
