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
            case SIG_OBJECT_NULL : {
                gid->currentObject = g3dobject_new ( gid->currentObjectID++, 
                                                     "NULL",
                                                     0x00 );

                g3dobject_addChild ( gid->parentObject, 
                                     gid->currentObject, 
                                     gid->engineFlags );
            } break;

            case SIG_OBJECT_SCENE : {
                gid->currentObject = gid->currentScene;

                g3dimportscene ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            case SIG_OBJECT_MESH : {
                gid->currentObject = g3dmesh_new ( gid->currentObjectID++ ,
                                                   gid->currentObjectName,
                                                   gid->engineFlags );

                g3dobject_addChild ( gid->parentObject, 
                                     gid->currentObject, 
                                     gid->engineFlags );

                g3dimportmesh ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            case SIG_OBJECT_PRIMITIVE : {
                g3dimportprimitive ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            case SIG_OBJECT_CAMERA : {
                gid->currentObject = g3dcamera_new ( gid->currentObjectID++ ,
                                                     gid->currentObjectName,
                                                     0.0f, 
                                                     0.0f, 
                                                     0.1f, 
                                                     1000.0f );

                g3dobject_addChild ( gid->parentObject, 
                                     gid->currentObject, 
                                     gid->engineFlags );

                g3dimportcamera ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            case SIG_OBJECT_LIGHT : {
                gid->currentObject = g3dlight_new ( gid->currentObjectID++ ,
                                                    gid->currentObjectName );

                g3dobject_addChild ( gid->parentObject, 
                                     gid->currentObject, 
                                     gid->engineFlags );

                g3dimportlight ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            case SIG_OBJECT_BONE : {
                gid->currentObject = g3dbone_new ( gid->currentObjectID++ ,
                                                   gid->currentObjectName,
                                                   0.0f );

                g3dobject_addChild ( gid->parentObject, 
                                     gid->currentObject, 
                                     gid->engineFlags );

                g3dimportbone ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            case SIG_OBJECT_SYMMETRY : {
                gid->currentObject = g3dsymmetry_new ( gid->currentObjectID++ ,
                                                       gid->currentObjectName );

                g3dobject_addChild ( gid->parentObject, 
                                     gid->currentObject, 
                                     gid->engineFlags );

                g3dimportsymmetry ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            case SIG_OBJECT_SUBDIVIDER : {
                gid->currentObject = g3dsubdivider_new ( gid->currentObjectID++ ,
                                                         gid->currentObjectName,
                                                         gid->engineFlags );

                g3dobject_addChild ( gid->parentObject, 
                                     gid->currentObject, 
                                     gid->engineFlags );

                g3dimportsubdivider ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            case SIG_OBJECT_SPLINEREVOLVER : {
                gid->currentObject = g3dsplinerevolver_new ( gid->currentObjectID++ ,
                                                             gid->currentObjectName );

                g3dobject_addChild ( gid->parentObject, 
                                     gid->currentObject, 
                                     gid->engineFlags );

                g3dimportsplinerevolver ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            case SIG_OBJECT_SPLINE : {
                gid->currentObject = g3dspline_new ( gid->currentObjectID++ ,
                                                     gid->currentObjectName,
                                                     CUBIC,
                                                     gid->engineFlags );

                g3dobject_addChild ( gid->parentObject, 
                                     gid->currentObject, 
                                     gid->engineFlags );

                g3dimportspline ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            case SIG_OBJECT_WIREFRAME : {
                gid->currentObject = g3dwireframe_new ( gid->currentObjectID++ ,
                                                        gid->currentObjectName );

                g3dobject_addChild ( gid->parentObject, 
                                     gid->currentObject, 
                                     gid->engineFlags );

                g3dimportwireframe ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            case SIG_OBJECT_TEXT : {
                gid->currentObject = g3dtext_new ( gid->currentObjectID++ ,
                                                   gid->currentObjectName,
                                                   gid->engineFlags );

                g3dobject_addChild ( gid->parentObject, 
                                     gid->currentObject, 
                                     gid->engineFlags );

                g3dimporttext ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            case SIG_OBJECT_FFD : {
                gid->currentObject = g3dffd_new ( gid->currentObjectID++ ,
                                                  gid->currentObjectName );

                g3dobject_addChild ( gid->parentObject, 
                                     gid->currentObject, 
                                     gid->engineFlags );

                g3dimportffd ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            /*** Unknown object type. Create a null object ***/
            default : {
                gid->currentObject = g3dobject_new ( gid->currentObjectID++, 
                                                     "NULL",
                                                     gid->engineFlags );

                g3dobject_addChild ( gid->parentObject, 
                                     gid->currentObject, 
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

                printf ( "Object Name: %s\n", gid->currentObjectName );
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

                gid->parentObject = g3dobject_getChildByID ( gid->currentScene, 
                                                             parentID );
            } break;

            case SIG_OBJECT_ACTIVE : {
                g3dimport_freadl ( &active, fsrc );

                if ( active ) {
                    g3dobject_activate ( gid->currentObject, gid->engineFlags );
                } else {
                    g3dobject_deactivate ( gid->currentObject, gid->engineFlags );
                }
            } break;

            case SIG_OBJECT_TRANSFORMATION : {
                G3DVECTOR pos, rot, sca;

                g3dimport_freadf ( &pos.x, fsrc );
                g3dimport_freadf ( &pos.y, fsrc );
                g3dimport_freadf ( &pos.z, fsrc );
                g3dimport_freadf ( &pos.w, fsrc );

                g3dimport_freadf ( &rot.x, fsrc );
                g3dimport_freadf ( &rot.y, fsrc );
                g3dimport_freadf ( &rot.z, fsrc );
                g3dimport_freadf ( &rot.w, fsrc );

                g3dimport_freadf ( &sca.x, fsrc );
                g3dimport_freadf ( &sca.y, fsrc );
                g3dimport_freadf ( &sca.z, fsrc );
                g3dimport_freadf ( &sca.w, fsrc );

                if ( gid->currentObject ) {
                    memcpy ( &gid->currentObject->pos, &pos, sizeof ( G3DVECTOR ) );
                    memcpy ( &gid->currentObject->rot, &rot, sizeof ( G3DVECTOR ) );
                    memcpy ( &gid->currentObject->sca, &sca, sizeof ( G3DVECTOR ) );

                    g3dobject_updateMatrix_r ( gid->currentObject, 0x00 );
                }
            } break;

            case SIG_OBJECT_UVMAPS : {
                g3dimportuvmap ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            case SIG_OBJECT_TEXTURES : {
            } break;

            case SIG_OBJECT_TEXTURE_ENTRY : {
            } break;

            case SIG_OBJECT_TEXTURE_MATERIAL : {
                G3DMESH *mes = ( G3DMESH * ) gid->currentObject;
                G3DMATERIAL *mat = NULL;
                uint32_t matID;

                g3dimport_freadl ( &matID, fsrc );

                mat = g3dscene_getMaterialByID ( gid->currentScene, matID );

                gid->currentTexture = g3dtexture_new ( mes, mat, NULL );

                g3dmesh_addTexture ( mes, gid->currentTexture );
            } break;

            case SIG_OBJECT_TEXTURE_UVMAP : {
                G3DUVMAP *uvmap = NULL;
                uint32_t uvmapID;

                g3dimport_freadl ( &uvmapID, fsrc );

                uvmap = g3dmesh_getUVMapByID ( gid->currentObject, uvmapID );

                gid->currentTexture->map = uvmap;
            } break;

            case SIG_OBJECT_TEXTURE_RESTRICT : {
                uint32_t nbfacgrp;
                int i;

                g3dtexture_restrict ( gid->currentTexture );

                g3dimport_freadl ( &nbfacgrp, fsrc );

                for ( i = 0x00; i < nbfacgrp; i++ ) {
                    G3DFACEGROUP *facgrp;
                    uint32_t facgrpID;

                    g3dimport_freadl ( &facgrpID, fsrc );

                    facgrp = g3dmesh_getFacegroupByID ( gid->currentObject, facgrpID );

                    g3dtexture_restrictFacegroup ( gid->currentTexture, facgrp );
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

                g3dimport_freadf ( &frame, fsrc );

                g3dimport_freadf ( &pos.x , fsrc );
                g3dimport_freadf ( &pos.y , fsrc );
                g3dimport_freadf ( &pos.z , fsrc );
                g3dimport_freadl ( &usepos, fsrc );

                g3dimport_freadf ( &rot.x , fsrc );
                g3dimport_freadf ( &rot.y , fsrc );
                g3dimport_freadf ( &rot.z , fsrc );
                g3dimport_freadl ( &userot, fsrc );

                g3dimport_freadf ( &sca.x , fsrc );
                g3dimport_freadf ( &sca.y , fsrc );
                g3dimport_freadf ( &sca.z , fsrc );
                g3dimport_freadl ( &usesca, fsrc );

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

                g3dimport_freadf ( &loopFrame, fsrc );

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
