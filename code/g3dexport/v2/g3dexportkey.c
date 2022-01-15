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
#include <g3dexportv2.h>

/******************************************************************************/
static uint32_t g3dexportv2key_loop ( G3DEXPORTV2DATA *ged, 
                                    G3DKEY        *key,
                                    uint32_t       flags, 
                                    FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_fwritef ( &key->loopFrame, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2key_transformation ( G3DEXPORTV2DATA *ged, 
                                              G3DKEY        *key, 
                                              uint32_t       flags, 
                                              FILE          *fdst ) {
    uint32_t usepos = ( key->flags & KEYPOSITION ) ? 0x01 : 0x00;
    uint32_t userot = ( key->flags & KEYROTATION ) ? 0x01 : 0x00;
    uint32_t usesca = ( key->flags & KEYSCALING  ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexportv2_fwritef ( &key->frame, fdst );

    size += g3dexportv2_fwritef ( &key->pos.x, fdst );
    size += g3dexportv2_fwritef ( &key->pos.y, fdst );
    size += g3dexportv2_fwritef ( &key->pos.z, fdst );
    size += g3dexportv2_fwritel ( &usepos    , fdst );

    size += g3dexportv2_fwritef ( &key->rot.x, fdst );
    size += g3dexportv2_fwritef ( &key->rot.y, fdst );
    size += g3dexportv2_fwritef ( &key->rot.z, fdst );
    size += g3dexportv2_fwritel ( &userot    , fdst );

    size += g3dexportv2_fwritef ( &key->sca.x, fdst );
    size += g3dexportv2_fwritef ( &key->sca.y, fdst );
    size += g3dexportv2_fwritef ( &key->sca.z, fdst );
    size += g3dexportv2_fwritel ( &usesca    , fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2key_dataMorpherSlotOptionsRate ( G3DEXPORTV2DATA *ged, 
                                                          G3DKEY        *key, 
                                                          uint32_t       flags, 
                                                          FILE          *fdst ) {
    G3DMORPHER *mpr = ( G3DMORPHER * ) ged->currentObject;
    uint32_t slotID = flags;
    uint32_t size = 0x00;
    float rate = g3dmorpherkey_getMeshPoseRate ( key, slotID );

    size += g3dexportv2_fwritef ( &rate, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2key_dataMorpherSlotOptions ( G3DEXPORTV2DATA *ged, 
                                                      G3DKEY        *key, 
                                                      uint32_t       flags, 
                                                      FILE          *fdst ) {
    G3DMORPHER *mpr = ( G3DMORPHER * ) ged->currentObject;
    uint32_t slotID = flags;
    uint32_t size = 0x00;

    size += g3dexportv2_writeChunk ( SIG_OBJECT_KEY_DATA_MORPHER_SLOT_OPTIONS_RATE,
                   EXPORTV2_CALLBACK(g3dexportv2key_dataMorpherSlotOptionsRate),
                                     ged,
                                     key,
                                     slotID,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2key_dataMorpherSlotID ( G3DEXPORTV2DATA *ged, 
                                                 G3DKEY        *key,
                                                 uint32_t       flags, 
                                                 FILE          *fdst ) {
    uint32_t slotID = flags;
    uint32_t size = 0x00;

    size += g3dexportv2_fwritel( &slotID, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2key_dataMorpherSlotEntry ( G3DEXPORTV2DATA *ged, 
                                                    G3DKEY        *key, 
                                                    uint32_t       flags, 
                                                    FILE          *fdst ) {
    G3DMORPHER *mpr = ( G3DMORPHER * ) ged->currentObject;
    uint32_t slotID = flags;
    uint32_t size = 0x00;

    size += g3dexportv2_writeChunk ( SIG_OBJECT_KEY_DATA_MORPHER_SLOT_ID,
                    EXPORTV2_CALLBACK(g3dexportv2key_dataMorpherSlotID),
                                     ged,
                                     key,
                                     slotID,
                                     fdst );

    size += g3dexportv2_writeChunk ( SIG_OBJECT_KEY_DATA_MORPHER_SLOT_OPTIONS,
                    EXPORTV2_CALLBACK(g3dexportv2key_dataMorpherSlotOptions),
                                     ged,
                                     key,
                                     slotID,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2key_dataMorpherSlots ( G3DEXPORTV2DATA *ged, 
                                                G3DKEY        *key, 
                                                uint32_t       flags, 
                                                FILE          *fdst ) {
    G3DMORPHER *mpr = ( G3DMORPHER * ) ged->currentObject;
    LIST *ltmpmpose = mpr->lmpose;
    uint32_t size = 0x00;

    while ( ltmpmpose ) {
        G3DMORPHERMESHPOSE *mpose = ( G3DMORPHERMESHPOSE * ) ltmpmpose->data;

        if ( g3dmorpherkey_isMeshPoseEnabled ( key, mpose->slotID ) ) {
            size += g3dexportv2_writeChunk ( SIG_OBJECT_KEY_DATA_MORPHER_SLOT_ENTRY,
                           EXPORTV2_CALLBACK(g3dexportv2key_dataMorpherSlotEntry),
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
static uint32_t g3dexportv2key_dataMorpher ( G3DEXPORTV2DATA *ged, 
                                           G3DKEY        *key, 
                                           uint32_t       flags, 
                                           FILE          *fdst ) {
    uint32_t size = 0x00;

    switch ( ged->currentObject->type ) {
        case G3DMORPHERTYPE :
            size += g3dexportv2_writeChunk ( SIG_OBJECT_KEY_DATA_MORPHER_SLOTS,
                           EXPORTV2_CALLBACK(g3dexportv2key_dataMorpherSlots),
                                             ged,
                                             key,
                                             0xFFFFFFFF,
                                             fdst );
        break;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2key_dataLight ( G3DEXPORTV2DATA *ged, 
                                         G3DKEY        *key, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    return g3dexportv2light ( ged, key->data.ptr, flags, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv2key_data ( G3DEXPORTV2DATA *ged, 
                                    G3DKEY        *key, 
                                    uint32_t       flags, 
                                    FILE          *fdst ) {
    uint32_t size = 0x00;

    switch ( ged->currentObject->type ) {
        case G3DMORPHERTYPE :
            size += g3dexportv2_writeChunk ( SIG_OBJECT_KEY_DATA_MORPHER,
                           EXPORTV2_CALLBACK(g3dexportv2key_dataMorpher),
                                             ged,
                                             key,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case G3DLIGHTTYPE :
            size += g3dexportv2_writeChunk ( SIG_OBJECT_KEY_DATA_LIGHT,
                           EXPORTV2_CALLBACK(g3dexportv2key_dataLight),
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
static uint32_t g3dexportv2key_flagsTranslation ( G3DEXPORTV2DATA *ged, 
                                                G3DKEY        *key,
                                                uint32_t       flags, 
                                                FILE          *fdst ) {
    uint32_t enabled = ( key->flags & KEYPOSITION ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexportv2_fwritel( &enabled, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2key_flagsRotation ( G3DEXPORTV2DATA *ged, 
                                             G3DKEY        *key,
                                             uint32_t       flags, 
                                             FILE          *fdst ) {
    uint32_t enabled = ( key->flags & KEYROTATION ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexportv2_fwritel( &enabled, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2key_flagsScaling ( G3DEXPORTV2DATA *ged, 
                                            G3DKEY        *key,
                                            uint32_t       flags, 
                                            FILE          *fdst ) {
    uint32_t enabled = ( key->flags & KEYSCALING ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexportv2_fwritel( &enabled, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2key_flagsData ( G3DEXPORTV2DATA *ged, 
                                         G3DKEY        *key,
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t enabled = ( key->flags & KEYDATA ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexportv2_fwritel( &enabled, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2key_flags ( G3DEXPORTV2DATA *ged, 
                                     G3DKEY        *key, 
                                     uint32_t       flags, 
                                     FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_writeChunk ( SIG_OBJECT_KEY_FLAGS_TRANSLATION,
                   EXPORTV2_CALLBACK(g3dexportv2key_flagsTranslation),
                                     ged,
                                     key,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv2_writeChunk ( SIG_OBJECT_KEY_FLAGS_ROTATION,
                   EXPORTV2_CALLBACK(g3dexportv2key_flagsRotation),
                                     ged,
                                     key,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv2_writeChunk ( SIG_OBJECT_KEY_FLAGS_SCALING,
                   EXPORTV2_CALLBACK(g3dexportv2key_flagsScaling),
                                     ged,
                                     key,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv2_writeChunk ( SIG_OBJECT_KEY_FLAGS_DATA,
                   EXPORTV2_CALLBACK(g3dexportv2key_flagsData),
                                     ged,
                                     key,
                                     0xFFFFFFFF,
                                     fdst );


    return size;
}

/******************************************************************************/
uint32_t g3dexportv2key ( G3DEXPORTV2DATA *ged, 
                        G3DKEY        *key, 
                        uint32_t       flags, 
                        FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_writeChunk ( SIG_OBJECT_KEY_FLAGS,
                   EXPORTV2_CALLBACK(g3dexportv2key_flags),
                                     ged,
                                     key,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv2_writeChunk ( SIG_OBJECT_KEY_TRANSFORMATION,
                   EXPORTV2_CALLBACK(g3dexportv2key_transformation),
                                     ged,
                                     key,
                                     0xFFFFFFFF,
                                     fdst );

    if ( key->flags & KEYLOOP ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_KEY_LOOP,
                       EXPORTV2_CALLBACK(g3dexportv2key_loop),
                                         ged,
                                         key,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( key->flags & KEYDATA ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_KEY_DATA,
                       EXPORTV2_CALLBACK(g3dexportv2key_data),
                                         ged,
                                         key,
                                         0xFFFFFFFF,
                                         fdst );
    }

    return size;
}
