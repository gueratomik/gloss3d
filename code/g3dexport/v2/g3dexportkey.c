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
static uint32_t g3dexportkey_loop ( G3DEXPORTDATA *ged, 
                                    G3DKEY        *key,
                                    uint32_t       flags, 
                                    FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &key->loopFrame, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportkey_transformation ( G3DEXPORTDATA *ged, 
                                              G3DKEY        *key, 
                                              uint32_t       flags, 
                                              FILE          *fdst ) {
    uint32_t usepos = ( key->flags & KEYPOSITION ) ? 0x01 : 0x00;
    uint32_t userot = ( key->flags & KEYROTATION ) ? 0x01 : 0x00;
    uint32_t usesca = ( key->flags & KEYSCALING  ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &key->frame, fdst );

    size += g3dexport_fwritef ( &key->pos.x, fdst );
    size += g3dexport_fwritef ( &key->pos.y, fdst );
    size += g3dexport_fwritef ( &key->pos.z, fdst );
    size += g3dexport_fwritel ( &usepos    , fdst );

    size += g3dexport_fwritef ( &key->rot.x, fdst );
    size += g3dexport_fwritef ( &key->rot.y, fdst );
    size += g3dexport_fwritef ( &key->rot.z, fdst );
    size += g3dexport_fwritel ( &userot    , fdst );

    size += g3dexport_fwritef ( &key->sca.x, fdst );
    size += g3dexport_fwritef ( &key->sca.y, fdst );
    size += g3dexport_fwritef ( &key->sca.z, fdst );
    size += g3dexport_fwritel ( &usesca    , fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportkey_dataMorpherSlotOptionsRate ( G3DEXPORTDATA *ged, 
                                                          G3DKEY        *key, 
                                                          uint32_t       flags, 
                                                          FILE          *fdst ) {
    G3DMORPHER *mpr = ( G3DMORPHER * ) ged->currentObject;
    uint32_t slotID = flags;
    uint32_t size = 0x00;
    float rate = g3dmorpherkey_getMeshPoseRate ( key, slotID );

    size += g3dexport_fwritef ( &rate, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportkey_dataMorpherSlotOptions ( G3DEXPORTDATA *ged, 
                                                      G3DKEY        *key, 
                                                      uint32_t       flags, 
                                                      FILE          *fdst ) {
    G3DMORPHER *mpr = ( G3DMORPHER * ) ged->currentObject;
    uint32_t slotID = flags;
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_KEY_DATA_MORPHER_SLOT_OPTIONS_RATE,
                                   g3dexportkey_dataMorpherSlotOptionsRate,
                                   ged,
                                   key,
                                   slotID,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportkey_dataMorpherSlotID ( G3DEXPORTDATA *ged, 
                                                 G3DKEY        *key,
                                                 uint32_t       flags, 
                                                 FILE          *fdst ) {
    uint32_t slotID = flags;
    uint32_t size = 0x00;

    size += g3dexport_fwritel( &slotID, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportkey_dataMorpherSlotEntry ( G3DEXPORTDATA *ged, 
                                                    G3DKEY        *key, 
                                                    uint32_t       flags, 
                                                    FILE          *fdst ) {
    G3DMORPHER *mpr = ( G3DMORPHER * ) ged->currentObject;
    uint32_t slotID = flags;
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_KEY_DATA_MORPHER_SLOT_ID,
                                   g3dexportkey_dataMorpherSlotID,
                                   ged,
                                   key,
                                   slotID,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_KEY_DATA_MORPHER_SLOT_OPTIONS,
                                   g3dexportkey_dataMorpherSlotOptions,
                                   ged,
                                   key,
                                   slotID,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportkey_dataMorpherSlots ( G3DEXPORTDATA *ged, 
                                                G3DKEY        *key, 
                                                uint32_t       flags, 
                                                FILE          *fdst ) {
    G3DMORPHER *mpr = ( G3DMORPHER * ) ged->currentObject;
    LIST *ltmpmpose = mpr->lmpose;
    uint32_t size = 0x00;

    while ( ltmpmpose ) {
        G3DMORPHERMESHPOSE *mpose = ( G3DMORPHERMESHPOSE * ) ltmpmpose->data;

        if ( g3dmorpherkey_isMeshPoseEnabled ( key, mpose->slotID ) ) {
            size += g3dexport_writeChunk ( SIG_OBJECT_KEY_DATA_MORPHER_SLOT_ENTRY,
                                           g3dexportkey_dataMorpherSlotEntry,
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
static uint32_t g3dexportkey_dataMorpher ( G3DEXPORTDATA *ged, 
                                           G3DKEY        *key, 
                                           uint32_t       flags, 
                                           FILE          *fdst ) {
    uint32_t size = 0x00;

    switch ( ged->currentObject->type ) {
        case G3DMORPHERTYPE :
            size += g3dexport_writeChunk ( SIG_OBJECT_KEY_DATA_MORPHER_SLOTS,
                                           g3dexportkey_dataMorpherSlots,
                                           ged,
                                           key,
                                           0xFFFFFFFF,
                                           fdst );
        break;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportkey_data ( G3DEXPORTDATA *ged, 
                                    G3DKEY        *key, 
                                    uint32_t       flags, 
                                    FILE          *fdst ) {
    uint32_t size = 0x00;

    switch ( ged->currentObject->type ) {
        case G3DMORPHERTYPE :
            size += g3dexport_writeChunk ( SIG_OBJECT_KEY_DATA_MORPHER,
                                           g3dexportkey_dataMorpher,
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
static uint32_t g3dexportkey_flagsTranslation ( G3DEXPORTDATA *ged, 
                                                G3DKEY        *key,
                                                uint32_t       flags, 
                                                FILE          *fdst ) {
    uint32_t enabled = ( key->flags & KEYPOSITION ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexport_fwritel( &enabled, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportkey_flagsRotation ( G3DEXPORTDATA *ged, 
                                             G3DKEY        *key,
                                             uint32_t       flags, 
                                             FILE          *fdst ) {
    uint32_t enabled = ( key->flags & KEYROTATION ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexport_fwritel( &enabled, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportkey_flagsScaling ( G3DEXPORTDATA *ged, 
                                            G3DKEY        *key,
                                            uint32_t       flags, 
                                            FILE          *fdst ) {
    uint32_t enabled = ( key->flags & KEYSCALING ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexport_fwritel( &enabled, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportkey_flagsData ( G3DEXPORTDATA *ged, 
                                         G3DKEY        *key,
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t enabled = ( key->flags & KEYDATA ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexport_fwritel( &enabled, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportkey_flags ( G3DEXPORTDATA *ged, 
                                     G3DKEY        *key, 
                                     uint32_t       flags, 
                                     FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_KEY_FLAGS_TRANSLATION,
                                   g3dexportkey_flagsTranslation,
                                   ged,
                                   key,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_KEY_FLAGS_ROTATION,
                                   g3dexportkey_flagsRotation,
                                   ged,
                                   key,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_KEY_FLAGS_SCALING,
                                   g3dexportkey_flagsScaling,
                                   ged,
                                   key,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_KEY_FLAGS_DATA,
                                   g3dexportkey_flagsData,
                                   ged,
                                   key,
                                   0xFFFFFFFF,
                                   fdst );


    return size;
}

/******************************************************************************/
uint32_t g3dexportkey ( G3DEXPORTDATA *ged, 
                        G3DKEY        *key, 
                        uint32_t       flags, 
                        FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_KEY_FLAGS,
                                   g3dexportkey_flags,
                                   ged,
                                   key,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_KEY_TRANSFORMATION,
                                   g3dexportkey_transformation,
                                   ged,
                                   key,
                                   0xFFFFFFFF,
                                   fdst );

    if ( key->flags & KEYLOOP ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_KEY_LOOP,
                                       g3dexportkey_loop,
                                       ged,
                                       key,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( key->flags & KEYDATA ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_KEY_DATA,
                                       g3dexportkey_data,
                                       ged,
                                       key,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}
