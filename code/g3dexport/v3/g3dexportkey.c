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
#include <g3dexportv3.h>

/******************************************************************************/
static uint32_t g3dexportv3key_loop ( G3DEXPORTV3DATA *ged, 
                                    G3DKEY        *key,
                                    uint32_t       flags, 
                                    FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &key->loopFrame, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3key_transformation ( G3DEXPORTV3DATA *ged, 
                                              G3DKEY        *key, 
                                              uint32_t       flags, 
                                              FILE          *fdst ) {
    uint32_t usepos = ( key->flags & KEYPOSITION ) ? 0x01 : 0x00;
    uint32_t userot = ( key->flags & KEYROTATION ) ? 0x01 : 0x00;
    uint32_t usesca = ( key->flags & KEYSCALING  ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &key->frame, fdst );

    size += g3dexportv3_fwritef ( &key->pos.x, fdst );
    size += g3dexportv3_fwritef ( &key->pos.y, fdst );
    size += g3dexportv3_fwritef ( &key->pos.z, fdst );
    size += g3dexportv3_fwritel ( &usepos    , fdst );

    size += g3dexportv3_fwritef ( &key->rot.x, fdst );
    size += g3dexportv3_fwritef ( &key->rot.y, fdst );
    size += g3dexportv3_fwritef ( &key->rot.z, fdst );
    size += g3dexportv3_fwritel ( &userot    , fdst );

    size += g3dexportv3_fwritef ( &key->sca.x, fdst );
    size += g3dexportv3_fwritef ( &key->sca.y, fdst );
    size += g3dexportv3_fwritef ( &key->sca.z, fdst );
    size += g3dexportv3_fwritel ( &usesca    , fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3key_dataMorpherSlotOptionsRate ( G3DEXPORTV3DATA *ged, 
                                                          G3DKEY        *key, 
                                                          uint32_t       flags, 
                                                          FILE          *fdst ) {
    G3DMORPHER *mpr = ( G3DMORPHER * ) ged->currentObject;
    uint32_t slotID = flags;
    uint32_t size = 0x00;
    float rate = g3dmorpherkey_getMeshPoseRate ( key, slotID );

    size += g3dexportv3_fwritef ( &rate, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3key_dataMorpherSlotOptions ( G3DEXPORTV3DATA *ged, 
                                                      G3DKEY        *key, 
                                                      uint32_t       flags, 
                                                      FILE          *fdst ) {
    G3DMORPHER *mpr = ( G3DMORPHER * ) ged->currentObject;
    uint32_t slotID = flags;
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_KEY_DATA_MORPHER_SLOT_OPTIONS_RATE,
                   EXPORTV3_CALLBACK(g3dexportv3key_dataMorpherSlotOptionsRate),
                                     ged,
                                     key,
                                     slotID,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3key_dataMorpherSlotID ( G3DEXPORTV3DATA *ged, 
                                                 G3DKEY        *key,
                                                 uint32_t       flags, 
                                                 FILE          *fdst ) {
    uint32_t slotID = flags;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel( &slotID, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3key_dataMorpherSlotEntry ( G3DEXPORTV3DATA *ged, 
                                                    G3DKEY        *key, 
                                                    uint32_t       flags, 
                                                    FILE          *fdst ) {
    G3DMORPHER *mpr = ( G3DMORPHER * ) ged->currentObject;
    uint32_t slotID = flags;
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_KEY_DATA_MORPHER_SLOT_ID,
                   EXPORTV3_CALLBACK(g3dexportv3key_dataMorpherSlotID),
                                     ged,
                                     key,
                                     slotID,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_KEY_DATA_MORPHER_SLOT_OPTIONS,
                   EXPORTV3_CALLBACK(g3dexportv3key_dataMorpherSlotOptions),
                                     ged,
                                     key,
                                     slotID,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3key_dataMorpherSlots ( G3DEXPORTV3DATA *ged, 
                                                G3DKEY        *key, 
                                                uint32_t       flags, 
                                                FILE          *fdst ) {
    G3DMORPHER *mpr = ( G3DMORPHER * ) ged->currentObject;
    LIST *ltmpmpose = mpr->lmpose;
    uint32_t size = 0x00;

    while ( ltmpmpose ) {
        G3DMORPHERMESHPOSE *mpose = ( G3DMORPHERMESHPOSE * ) ltmpmpose->data;

        if ( g3dmorpherkey_isMeshPoseEnabled ( key, mpose->slotID ) ) {
            size += g3dexportv3_writeChunk ( SIG_OBJECT_KEY_DATA_MORPHER_SLOT_ENTRY,
                           EXPORTV3_CALLBACK(g3dexportv3key_dataMorpherSlotEntry),
                                             ged,
                                             key,
                                             mpose->slotID,
                                             fdst );
        }

        ltmpmpose = ltmpmpose->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3key_dataMorpher ( G3DEXPORTV3DATA *ged, 
                                           G3DKEY        *key, 
                                           uint32_t       flags, 
                                           FILE          *fdst ) {
    uint32_t size = 0x00;

    switch ( ged->currentObject->type ) {
        case G3DMORPHERTYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_KEY_DATA_MORPHER_SLOTS,
                           EXPORTV3_CALLBACK(g3dexportv3key_dataMorpherSlots),
                                             ged,
                                             key,
                                             0xFFFFFFFF,
                                             fdst );
        break;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3key_dataPEmitter ( G3DEXPORTV3DATA *ged, 
                                              G3DKEY        *key, 
                                              uint32_t       flags, 
                                              FILE          *fdst ) {
    return g3dexportv3particleemitter ( ged, key->data.ptr, flags, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv3key_dataLight ( G3DEXPORTV3DATA *ged, 
                                         G3DKEY        *key, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    return g3dexportv3light ( ged, key->data.ptr, flags, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv3key_dataCamera ( G3DEXPORTV3DATA *ged, 
                                            G3DKEY          *key, 
                                            uint32_t         flags, 
                                            FILE            *fdst ) {
    return g3dexportv3camera ( ged, key->data.ptr, flags, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv3key_data ( G3DEXPORTV3DATA *ged, 
                                    G3DKEY        *key, 
                                    uint32_t       flags, 
                                    FILE          *fdst ) {
    uint32_t size = 0x00;

    switch ( ged->currentObject->type ) {
        case G3DMORPHERTYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_KEY_DATA_MORPHER,
                           EXPORTV3_CALLBACK(g3dexportv3key_dataMorpher),
                                             ged,
                                             key,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DLIGHTTYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_KEY_DATA_LIGHT,
                           EXPORTV3_CALLBACK(g3dexportv3key_dataLight),
                                             ged,
                                             key,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DCAMERATYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_KEY_DATA_CAMERA,
                           EXPORTV3_CALLBACK(g3dexportv3key_dataCamera),
                                             ged,
                                             key,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DPARTICLEEMITTERTYPE :
            size += g3dexportv3_writeChunk ( SIG_OBJECT_KEY_DATA_PARTICLEEMITTER,
                           EXPORTV3_CALLBACK(g3dexportv3key_dataPEmitter),
                                             ged,
                                             key,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        default :
        break;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3key_flagsTranslation ( G3DEXPORTV3DATA *ged, 
                                                G3DKEY        *key,
                                                uint32_t       flags, 
                                                FILE          *fdst ) {
    uint32_t enabled = ( key->flags & KEYPOSITION ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel( &enabled, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3key_flagsRotation ( G3DEXPORTV3DATA *ged, 
                                             G3DKEY        *key,
                                             uint32_t       flags, 
                                             FILE          *fdst ) {
    uint32_t enabled = ( key->flags & KEYROTATION ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel( &enabled, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3key_flagsScaling ( G3DEXPORTV3DATA *ged, 
                                            G3DKEY        *key,
                                            uint32_t       flags, 
                                            FILE          *fdst ) {
    uint32_t enabled = ( key->flags & KEYSCALING ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel( &enabled, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3key_flagsData ( G3DEXPORTV3DATA *ged, 
                                         G3DKEY        *key,
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t enabled = ( key->flags & KEYDATA ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel( &enabled, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3key_flags ( G3DEXPORTV3DATA *ged, 
                                     G3DKEY        *key, 
                                     uint32_t       flags, 
                                     FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_KEY_FLAGS_TRANSLATION,
                   EXPORTV3_CALLBACK(g3dexportv3key_flagsTranslation),
                                     ged,
                                     key,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_KEY_FLAGS_ROTATION,
                   EXPORTV3_CALLBACK(g3dexportv3key_flagsRotation),
                                     ged,
                                     key,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_KEY_FLAGS_SCALING,
                   EXPORTV3_CALLBACK(g3dexportv3key_flagsScaling),
                                     ged,
                                     key,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_KEY_FLAGS_DATA,
                   EXPORTV3_CALLBACK(g3dexportv3key_flagsData),
                                     ged,
                                     key,
                                     0xFFFFFFFF,
                                     fdst );


    return size;
}

/******************************************************************************/
uint32_t g3dexportv3key ( G3DEXPORTV3DATA *ged, 
                        G3DKEY        *key, 
                        uint32_t       flags, 
                        FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_KEY_FLAGS,
                   EXPORTV3_CALLBACK(g3dexportv3key_flags),
                                     ged,
                                     key,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_KEY_TRANSFORMATION,
                   EXPORTV3_CALLBACK(g3dexportv3key_transformation),
                                     ged,
                                     key,
                                     0xFFFFFFFF,
                                     fdst );

    if ( key->flags & KEYLOOP ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_KEY_LOOP,
                       EXPORTV3_CALLBACK(g3dexportv3key_loop),
                                         ged,
                                         key,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( key->flags & KEYDATA ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_KEY_DATA,
                       EXPORTV3_CALLBACK(g3dexportv3key_data),
                                         ged,
                                         key,
                                         0xFFFFFFFF,
                                         fdst );
    }

    return size;
}
