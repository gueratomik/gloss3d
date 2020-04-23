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
void g3dimport_identityType ( G3DIMPORTDATA *gid, 
                              uint32_t       chunkEnd, 
                              FILE          *fsrc ) {
    uint32_t chunkSignature, chunkSize;
    uint32_t   type;        /***  object type ***/
    uint32_t   active;      /**** is object active or not ***/
    G3DKEY    *key;

    g3dimportdata_incrementIndentLevel ( gid );

    g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_SCENE : {

            } break;

            case SIG_OBJECT_MESH : {
                gid->currentObject = g3dmesh_new ( gid->currentObjectID++ ,
                                                   gid->currentObjectName,
                                                   gid->engineFlags );

                g3dimportmesh ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            case SIG_OBJECT_PRIMITIVE : {
                g3dimportprimitive ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            /*** Unknown object type. Create a null object ***/
            default : {
                gid->currentObject = g3dobject_new ( gid->currentObjectID++, 
                                                     "NULL",
                                                     gid->engineFlags );

                fseek ( fsrc, chunkSize, SEEK_CUR );
            } break;
        }

        /** hand the file back to the parent function ***/
        if ( ftell ( fsrc ) == chunkEnd ) break;

        g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
        g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );

    g3dimportdata_decrementIndentLevel ( gid );
}

/******************************************************************************/
void g3dimportobject ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;
    uint32_t   type;        /***  object type ***/
    uint32_t   active;      /**** is object active or not ***/
    G3DKEY    *key;

    memset ( gid->currentObjectName, 0x00, sizeof ( gid->currentObjectName ) );

    g3dimportdata_incrementIndentLevel ( gid );

    g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_IDENTITY : {
            } break;

            case SIG_OBJECT_IDENTITY_NAME : {
                /*** Potential buffer overflow here ***/
                g3dimport_fread ( gid->currentObjectName, chunkSize, 0x01, fsrc );
            } break;

            case SIG_OBJECT_IDENTITY_TYPE : {
                g3dimport_identityType ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            case SIG_OBJECT_IDENTITY_PARENT : {
                G3DOBJECT *parentObject = NULL;
                uint32_t   parentID;

                g3dimport_fread ( &parentID, sizeof ( uint32_t ), 0x01, fsrc );

                /*** We check the pointer because object might be NULL if  ***/
                /*** coming from an unsupported primitive from a plugin e.g **/
                if ( gid->currentObject ) {
                    parentObject = g3dobject_getChildByID ( gid->currentScene, parentID );

                    if ( parentObject ) {
                        /*** here we append otherwise the list is going to be ***/
                        /*** reversed compared to the saved version ***/
                        g3dobject_appendChild ( parentObject, 
                                                gid->currentObject, 
                                                gid->engineFlags );
                    }
                }
            } break;

            case SIG_OBJECT_IDENTITY_ACTIVE : {
                g3dimport_fread ( &active, sizeof ( uint32_t ), 0x01, fsrc );
            } break;

            case SIG_OBJECT_TRANSFORMATION : {
                G3DVECTOR pos, rot, sca;

                g3dimport_fread ( &pos.x, sizeof ( float ), 0x01, fsrc );
                g3dimport_fread ( &pos.y, sizeof ( float ), 0x01, fsrc );
                g3dimport_fread ( &pos.z, sizeof ( float ), 0x01, fsrc );
                g3dimport_fread ( &pos.w, sizeof ( float ), 0x01, fsrc );

                g3dimport_fread ( &rot.x, sizeof ( float ), 0x01, fsrc );
                g3dimport_fread ( &rot.y, sizeof ( float ), 0x01, fsrc );
                g3dimport_fread ( &rot.z, sizeof ( float ), 0x01, fsrc );
                g3dimport_fread ( &rot.w, sizeof ( float ), 0x01, fsrc );

                g3dimport_fread ( &sca.x, sizeof ( float ), 0x01, fsrc );
                g3dimport_fread ( &sca.y, sizeof ( float ), 0x01, fsrc );
                g3dimport_fread ( &sca.z, sizeof ( float ), 0x01, fsrc );
                g3dimport_fread ( &sca.w, sizeof ( float ), 0x01, fsrc );

                if ( gid->currentObject ) {
                    memcpy ( &gid->currentObject->pos, &pos, sizeof ( G3DVECTOR ) );
                    memcpy ( &gid->currentObject->rot, &rot, sizeof ( G3DVECTOR ) );
                    memcpy ( &gid->currentObject->sca, &sca, sizeof ( G3DVECTOR ) );

                    g3dobject_updateMatrix_r ( gid->currentObject, 0x00 );
                }
            } break;

            case SIG_OBJECT_KEYS : {
            } break;

            case SIG_OBJECT_KEY_ENTRY : {
            } break;

            case SIG_OBJECT_KEY_TRANSFORMATION : {
                uint32_t usepos, userot, usesca, key_flags = 0x00;
                G3DKEY *unusedKeyArg = NULL;
                G3DVECTOR pos, rot, sca;
                float frame;

                g3dimport_fread ( &frame, sizeof ( float ), 0x01, fsrc );

                g3dimport_fread ( &pos.x , sizeof ( float    ), 0x01, fsrc );
                g3dimport_fread ( &pos.y , sizeof ( float    ), 0x01, fsrc );
                g3dimport_fread ( &pos.z , sizeof ( float    ), 0x01, fsrc );
                g3dimport_fread ( &usepos, sizeof ( uint32_t ), 0x01, fsrc );

                g3dimport_fread ( &rot.x , sizeof ( float    ), 0x01, fsrc );
                g3dimport_fread ( &rot.y , sizeof ( float    ), 0x01, fsrc );
                g3dimport_fread ( &rot.z , sizeof ( float    ), 0x01, fsrc );
                g3dimport_fread ( &userot, sizeof ( uint32_t ), 0x01, fsrc );

                g3dimport_fread ( &sca.x , sizeof ( float    ), 0x01, fsrc );
                g3dimport_fread ( &sca.y , sizeof ( float    ), 0x01, fsrc );
                g3dimport_fread ( &sca.z , sizeof ( float    ), 0x01, fsrc );
                g3dimport_fread ( &usesca, sizeof ( uint32_t ), 0x01, fsrc );

                if ( usepos ) key_flags |= KEYPOSITION;
                if ( userot ) key_flags |= KEYROTATION;
                if ( usesca ) key_flags |= KEYSCALING;

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

                g3dimport_fread ( &loopFrame, sizeof ( float ), 0x01, fsrc );

                g3dkey_setLoop ( key );
                g3dkey_setLoopFrame ( key, loopFrame );
            } break;

            default : {
                fseek ( fsrc, chunkSize, SEEK_CUR );
            } break;
        }

        /** hand the file back to the parent function ***/
        if ( ftell ( fsrc ) == chunkEnd ) break;

        g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
        g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );

    g3dimportdata_decrementIndentLevel ( gid );
}
