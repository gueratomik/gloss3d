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
#include <g3dimportv3.h>

/******************************************************************************/
void g3dimportv3_identityType ( G3DIMPORTV3DATA *gid, 
                              uint32_t       chunkEnd, 
                              FILE          *fsrc ) {
    uint32_t chunkSignature, chunkSize;
    uint32_t   type;        /***  object type ***/
    uint32_t   active;      /**** is object active or not ***/
    G3DKEY    *key;

    g3dimportv3data_incrementIndentLevel ( gid );

    g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_NULL : {
            } break;

            case SIG_OBJECT_SCENE : {
                if ( chunkSize ) {
                    g3dimportv3scene ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            case SIG_OBJECT_MESH : {
                if ( chunkSize ) {
                    g3dimportv3mesh ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            case SIG_OBJECT_PRIMITIVE : {
                if ( chunkSize ) {
                    g3dimportv3primitive ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            case SIG_OBJECT_CAMERA : {
                if ( chunkSize ) {
                    g3dimportv3camera ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            case SIG_OBJECT_LIGHT : {
                if ( chunkSize ) {
                    g3dimportv3light ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            case SIG_OBJECT_BONE : {
                if ( chunkSize ) {
                    g3dimportv3bone ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            case SIG_OBJECT_SYMMETRY : {
                if ( chunkSize ) {
                    g3dimportv3symmetry ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            case SIG_OBJECT_SUBDIVIDER : {
                if ( chunkSize ) {
                    g3dimportv3subdivider ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            case SIG_OBJECT_SKIN : {
                if ( chunkSize ) {
                    g3dimportv3skin ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            case SIG_OBJECT_SPLINEREVOLVER : {
                if ( chunkSize ) {
                    g3dimportv3splinerevolver ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            case SIG_OBJECT_SPLINE : {
                if ( chunkSize ) {
                    g3dimportv3spline ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            case SIG_OBJECT_WIREFRAME : {
                if ( chunkSize ) {
                    g3dimportv3wireframe ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            case SIG_OBJECT_TEXT : {
                if ( chunkSize ) {
                    g3dimportv3text ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            case SIG_OBJECT_FFD : {
                if ( chunkSize ) {
                    g3dimportv3ffd ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            case SIG_OBJECT_MORPHER : {
                if ( chunkSize ) {
                    g3dimportv3morpher ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            case SIG_OBJECT_INSTANCE : {
                if ( chunkSize ) {
                    g3dimportv3instance ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            case SIG_OBJECT_PARTICLEEMITTER : {
                if ( chunkSize ) {
                    g3dimportv3particleemitter ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            /*** Unknown object type ***/
            default : {
                fseek ( fsrc, chunkSize, SEEK_CUR );
            } break;
        }

        /** hand the file back to the parent function ***/
        if ( ftell ( fsrc ) == chunkEnd ) break;

        g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
        g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );

    g3dimportv3data_decrementIndentLevel ( gid );
}

/******************************************************************************/
void g3dimportv3object ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;
    uint32_t   type;        /***  object type ***/
    uint32_t   active;      /**** is object active or not ***/
    G3DKEY    *key;

    g3dimportv3data_incrementIndentLevel ( gid );

    g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_IDENTITY : {
            } break;

            case SIG_OBJECT_IDENTITY_ID : {
                uint32_t objID;

                g3dimportv3_fread ( &objID, sizeof ( uint32_t ), 0x01, fsrc );

                gid->currentObject = gid->declaredObjects[objID];
            } break;

            case SIG_OBJECT_IDENTITY_TYPE : {
                g3dimportv3_identityType ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            case SIG_OBJECT_IDENTITY_PARENT : {
                G3DOBJECT *parentObject = NULL;
                uint32_t   parentID;

                g3dimportv3_fread ( &parentID, sizeof ( uint32_t ), 0x01, fsrc );

                gid->parentObject = gid->declaredObjects[parentID];

                g3dobject_addChild ( gid->parentObject, 
                                     gid->currentObject, 
                                     gid->engineFlags );
            } break;

            case SIG_OBJECT_NOSHADING : {
                uint32_t noshading;

                g3dimportv3_freadl ( &noshading, fsrc );

                if ( noshading ) {
                    gid->currentObject->flags |= OBJECTNOSHADING;
                }
            } break;

            case SIG_OBJECT_ACTIVE : {
                g3dimportv3_freadl ( &active, fsrc );

                if ( active ) {
                    g3dobject_activate ( gid->currentObject, gid->engineFlags );
                } else {
                    g3dobject_deactivate ( gid->currentObject, gid->engineFlags );
                }
            } break;

            case SIG_OBJECT_TRANSFORMATION : {
                G3DVECTOR pos, rot, sca;

                g3dimportv3_freadf ( &pos.x, fsrc );
                g3dimportv3_freadf ( &pos.y, fsrc );
                g3dimportv3_freadf ( &pos.z, fsrc );
                g3dimportv3_freadf ( &pos.w, fsrc );

                g3dimportv3_freadf ( &rot.x, fsrc );
                g3dimportv3_freadf ( &rot.y, fsrc );
                g3dimportv3_freadf ( &rot.z, fsrc );
                g3dimportv3_freadf ( &rot.w, fsrc );

                g3dimportv3_freadf ( &sca.x, fsrc );
                g3dimportv3_freadf ( &sca.y, fsrc );
                g3dimportv3_freadf ( &sca.z, fsrc );
                g3dimportv3_freadf ( &sca.w, fsrc );

                if ( gid->currentObject ) {
                    memcpy ( &gid->currentObject->pos, &pos, sizeof ( G3DVECTOR ) );
                    memcpy ( &gid->currentObject->rot, &rot, sizeof ( G3DVECTOR ) );
                    memcpy ( &gid->currentObject->sca, &sca, sizeof ( G3DVECTOR ) );

                    g3dobject_updateMatrix_r ( gid->currentObject, 0x00 );
                }
            } break;

            case SIG_OBJECT_UVMAPS : {
                g3dimportv3uvmap ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            case SIG_OBJECT_TEXTURES : {
            } break;

            case SIG_OBJECT_TEXTURE_ENTRY : {
            } break;

            case SIG_OBJECT_TEXTURE_MATERIAL : {
                G3DMESH *mes = ( G3DMESH * ) gid->currentObject;
                G3DMATERIAL *mat = NULL;
                uint32_t matID;

                g3dimportv3_freadl ( &matID, fsrc );

                mat = g3dscene_getMaterialByID ( gid->currentScene, matID );

                gid->currentTexture = g3dtexture_new ( ( G3DOBJECT * ) mes, 
                                                       mat, 
                                                       NULL );

                g3dmesh_appendTexture ( mes, gid->currentTexture );
            } break;

            case SIG_OBJECT_TEXTURE_UVMAP : {
                G3DUVMAP *uvmap = NULL;
                uint32_t uvmapID;

                g3dimportv3_freadl ( &uvmapID, fsrc );

                uvmap = g3dmesh_getUVMapByID ( ( G3DMESH * ) gid->currentObject, uvmapID );

                gid->currentTexture->map = uvmap;
            } break;

            case SIG_OBJECT_TEXTURE_RESTRICT : {
                uint32_t nbfacgrp;
                int i;

                g3dtexture_restrict ( gid->currentTexture );

                g3dimportv3_freadl ( &nbfacgrp, fsrc );

                for ( i = 0x00; i < nbfacgrp; i++ ) {
                    G3DFACEGROUP *facgrp;
                    uint32_t facgrpID;

                    g3dimportv3_freadl ( &facgrpID, fsrc );

                    facgrp = g3dmesh_getFacegroupByID ( ( G3DMESH * ) gid->currentObject, facgrpID );

                    g3dtexture_restrictFacegroup ( gid->currentTexture, facgrp );
                }
            } break;

            case SIG_OBJECT_KEYS : {
                if ( chunkSize ) {
                    g3dimportv3key ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            case SIG_OBJECT_PATHS : {
                if ( chunkSize ) {
                    g3dimportv3path ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            case SIG_OBJECT_TAGS : {
                if ( chunkSize ) {
                    g3dimportv3tag ( gid, ftell ( fsrc ) + chunkSize, fsrc );
                }
            } break;

            case SIG_OBJECT_KEY_TRANSFORMATION : {
                uint32_t usepos, userot, usesca, key_flags = 0x00;
                G3DKEY *unusedKeyArg = NULL;
                G3DVECTOR pos, rot, sca;
                float frame;

                g3dimportv3_freadf ( &frame, fsrc );

                g3dimportv3_freadf ( &pos.x , fsrc );
                g3dimportv3_freadf ( &pos.y , fsrc );
                g3dimportv3_freadf ( &pos.z , fsrc );
                g3dimportv3_freadl ( &usepos, fsrc );

                g3dimportv3_freadf ( &rot.x , fsrc );
                g3dimportv3_freadf ( &rot.y , fsrc );
                g3dimportv3_freadf ( &rot.z , fsrc );
                g3dimportv3_freadl ( &userot, fsrc );

                g3dimportv3_freadf ( &sca.x , fsrc );
                g3dimportv3_freadf ( &sca.y , fsrc );
                g3dimportv3_freadf ( &sca.z , fsrc );
                g3dimportv3_freadl ( &usesca, fsrc );

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
                                           key_flags,
                                           NULL,
                                           NULL,
                                           NULL,
                                           NULL,
                                           NULL,
                                           NULL );
                }
            } break;

            case SIG_OBJECT_KEY_LOOP : {
                float loopFrame;

                g3dimportv3_freadf ( &loopFrame, fsrc );

                g3dkey_setLoop ( key );
                g3dkey_setLoopFrame ( key, loopFrame );
            } break;

            default : {
                fseek ( fsrc, chunkSize, SEEK_CUR );
            } break;
        }

        /** hand the file back to the parent function ***/
        if ( ftell ( fsrc ) == chunkEnd ) break;

        g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
        g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );

    g3dimportv3data_decrementIndentLevel ( gid );
}
