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
#include <g3dimportv2.h>

/******************************************************************************/
void g3dimportv2key ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;
    G3DKEY    *key;
    uint32_t   poseID;
    uint32_t   enable_translation = 0x00;
    uint32_t   enable_rotation = 0x00;
    uint32_t   enable_scaling = 0x00;
    uint32_t   enable_data = 0x00;

    g3dimportv2data_incrementIndentLevel ( gid );

    g3dimportv2_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv2_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_KEY_ENTRY : {
            } break;

            case SIG_OBJECT_KEY_FLAGS : {
            } break;

            case SIG_OBJECT_KEY_FLAGS_TRANSLATION : {
                g3dimportv2_freadl ( &enable_translation, fsrc );
            } break;

            case SIG_OBJECT_KEY_FLAGS_ROTATION : {
                g3dimportv2_freadl ( &enable_rotation, fsrc );
            } break;

            case SIG_OBJECT_KEY_FLAGS_SCALING : {
                g3dimportv2_freadl ( &enable_scaling, fsrc );
            } break;

            case SIG_OBJECT_KEY_FLAGS_DATA : {
                g3dimportv2_freadl ( &enable_data, fsrc );
            } break;

            case SIG_OBJECT_KEY_DATA : {
            } break;

            case SIG_OBJECT_KEY_DATA_LIGHT : {
                G3DOBJECT *backup = gid->currentObject;

                gid->currentObject = key->data.ptr;

                if ( chunkSize ) {
                    g3dimportv2light ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }

                gid->currentObject = backup;
            } break;

            case SIG_OBJECT_KEY_DATA_MORPHER : {
            } break;

            case SIG_OBJECT_KEY_DATA_MORPHER_SLOTS : {
            } break;

            case SIG_OBJECT_KEY_DATA_MORPHER_SLOT_ENTRY : {
            } break;

            case SIG_OBJECT_KEY_DATA_MORPHER_SLOT_ID : {
                g3dimportv2_freadl ( &poseID, fsrc );

                g3dmorpherkey_enableMeshPose ( key, poseID );
            } break;

            case SIG_OBJECT_KEY_DATA_MORPHER_SLOT_OPTIONS : {
            } break;

            case SIG_OBJECT_KEY_DATA_MORPHER_SLOT_OPTIONS_RATE : {
                float rate;

                g3dimportv2_freadf ( &rate, fsrc );

                g3dmorpherkey_setMeshPoseRate ( key, poseID, rate );
            } break;

            case SIG_OBJECT_KEY_TRANSFORMATION : {
                uint32_t usepos, userot, usesca, key_flags = 0x00;
                G3DKEY *unusedKeyArg = NULL;
                G3DVECTOR pos, rot, sca;
                float frame;

                g3dimportv2_freadf ( &frame, fsrc );

                g3dimportv2_freadf ( &pos.x , fsrc );
                g3dimportv2_freadf ( &pos.y , fsrc );
                g3dimportv2_freadf ( &pos.z , fsrc );
                g3dimportv2_freadl ( &usepos, fsrc );

                g3dimportv2_freadf ( &rot.x , fsrc );
                g3dimportv2_freadf ( &rot.y , fsrc );
                g3dimportv2_freadf ( &rot.z , fsrc );
                g3dimportv2_freadl ( &userot, fsrc );

                g3dimportv2_freadf ( &sca.x , fsrc );
                g3dimportv2_freadf ( &sca.y , fsrc );
                g3dimportv2_freadf ( &sca.z , fsrc );
                g3dimportv2_freadl ( &usesca, fsrc );

                if ( enable_translation ) key_flags |= KEYPOSITION;
                if ( enable_rotation    ) key_flags |= KEYROTATION;
                if ( enable_scaling     ) key_flags |= KEYSCALING;
                if ( enable_data        ) key_flags |= KEYDATA;

                if ( gid->currentObject ) {
                    key = g3dobject_pose ( gid->currentObject,
                                           frame, 
                                          &pos,
                                          &rot,
                                          &sca,
                                          &unusedKeyArg,
                                           key_flags );
                }
            } break;

            case SIG_OBJECT_KEY_LOOP : {
                float loopFrame;

                g3dimportv2_freadf ( &loopFrame, fsrc );

                g3dkey_setLoop ( key );
                g3dkey_setLoopFrame ( key, loopFrame );
            } break;

            default : {
                fseek ( fsrc, chunkSize, SEEK_CUR );
            } break;
        }

        /** hand the file back to the parent function ***/
        if ( ftell ( fsrc ) == chunkEnd ) break;

        g3dimportv2_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
        g3dimportv2_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );

    g3dimportv2data_decrementIndentLevel ( gid );
}
