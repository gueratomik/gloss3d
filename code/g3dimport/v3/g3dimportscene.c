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
void g3dimportv3scene ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;

    g3dimportv3data_incrementIndentLevel ( gid );

    g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_SCENE_FPS : {
                g3dimportv3_freadl ( &gid->currentScene->fps, fsrc );
            } break;

            case SIG_MATERIALS : {

            } break;

            case SIG_MATERIAL_ENTRY : {
                g3dimportv3material ( gid, ftell ( fsrc ) + chunkSize, fsrc );
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

/******************************************************************************/
void g3dimportv3objects_declare ( G3DIMPORTV3DATA *gid,
                                  uint32_t         chunkEnd, 
                                  FILE            *fsrc ) {
    uint32_t chunkSignature, chunkSize;

    g3dimportv3data_incrementIndentLevel ( gid );

    g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_DECLARE_ENTRY : {
                memset ( gid->currentObjectName, 
                         0x00, 
                         sizeof ( gid->currentObjectName ) );
            } break;

            case SIG_OBJECT_DECLARE_SCENE  :
            case SIG_OBJECT_DECLARE_MESH :
            case SIG_OBJECT_DECLARE_NULL :
            case SIG_OBJECT_DECLARE_SPHERE :
            case SIG_OBJECT_DECLARE_TORUS :
            case SIG_OBJECT_DECLARE_TUBE :
            case SIG_OBJECT_DECLARE_CUBE :
            case SIG_OBJECT_DECLARE_PLANE :
            case SIG_OBJECT_DECLARE_CYLINDER :
            case SIG_OBJECT_DECLARE_CAMERA :
            case SIG_OBJECT_DECLARE_SUBDIVIDER :
            case SIG_OBJECT_DECLARE_SPLINE :
            case SIG_OBJECT_DECLARE_SPLINEREVOLVER :
            case SIG_OBJECT_DECLARE_SKIN :
            case SIG_OBJECT_DECLARE_FFD :
            case SIG_OBJECT_DECLARE_WIREFRAME :
            case SIG_OBJECT_DECLARE_SYMMETRY :
            case SIG_OBJECT_DECLARE_TEXT :
            case SIG_OBJECT_DECLARE_BONE :
            case SIG_OBJECT_DECLARE_LIGHT :
            case SIG_OBJECT_DECLARE_MORPHER :
            case SIG_OBJECT_DECLARE_INSTANCE :
            case SIG_OBJECT_DECLARE_PARTICLEEMITTER : {
                G3DOBJECT *obj = NULL;

                /*** Potential buffer overflow here ***/
                g3dimportv3_fread ( gid->currentObjectName, chunkSize, 0x01, fsrc );

                printf ( "Object Name: %s\n", gid->currentObjectName );

                /*** again ***/
                switch ( chunkSignature ) {
                    case SIG_OBJECT_DECLARE_SCENE : {
                        obj = ( G3DOBJECT * ) gid->currentScene;
                    } break;

                    case SIG_OBJECT_DECLARE_MESH : {
                        obj = ( G3DOBJECT * ) g3dmesh_new ( gid->currentObjectID++ ,
                                                            gid->currentObjectName,
                                                            gid->engineFlags );
                    } break;

                    case SIG_OBJECT_DECLARE_NULL : {
                        obj = ( G3DOBJECT * ) g3dobject_new ( gid->currentObjectID++, 
                                                              gid->currentObjectName,
                                                              0x00 );
                    } break;

                    case SIG_OBJECT_DECLARE_SPHERE : {
                        obj = ( G3DOBJECT * ) g3dsphere_new ( gid->currentObjectID++, 
                                                              gid->currentObjectName );
                    } break;

                    case SIG_OBJECT_DECLARE_TORUS : {
                        obj = ( G3DOBJECT * ) g3dtorus_new ( gid->currentObjectID++, 
                                                             gid->currentObjectName );
                    } break;

                    case SIG_OBJECT_DECLARE_TUBE : {
                        obj = ( G3DOBJECT * ) g3dtube_new ( gid->currentObjectID++, 
                                                            gid->currentObjectName );
                    } break;

                    case SIG_OBJECT_DECLARE_CUBE : {
                        obj = ( G3DOBJECT * ) g3dcube_new ( gid->currentObjectID++, 
                                                            gid->currentObjectName );
                    } break;

                    case SIG_OBJECT_DECLARE_PLANE : {
                        obj = ( G3DOBJECT * ) g3dplane_new ( gid->currentObjectID++, 
                                                             gid->currentObjectName );
                    } break;

                    case SIG_OBJECT_DECLARE_CYLINDER : {
                        obj = ( G3DOBJECT * ) g3dcylinder_new ( gid->currentObjectID++, 
                                                                gid->currentObjectName );
                    } break;

                    case SIG_OBJECT_DECLARE_CAMERA : {
                        obj = ( G3DOBJECT * ) g3dcamera_new ( gid->currentObjectID++ ,
                                                              gid->currentObjectName,
                                                              0.0f, 
                                                              0.0f, 
                                                              0.1f, 
                                                              1000.0f );
                    } break;

                    case SIG_OBJECT_DECLARE_SUBDIVIDER : {
                        obj = ( G3DOBJECT * ) g3dsubdivider_new ( gid->currentObjectID++ ,
                                                                  gid->currentObjectName,
                                                                  gid->engineFlags );
                    } break;

                    case SIG_OBJECT_DECLARE_SPLINE : {
                        obj = ( G3DOBJECT * ) g3dspline_new ( gid->currentObjectID++ ,
                                                              gid->currentObjectName,
                                                              CUBIC,
                                                              gid->engineFlags );
                    } break;

                    case SIG_OBJECT_DECLARE_SPLINEREVOLVER : {
                        obj = ( G3DOBJECT * ) g3dsplinerevolver_new ( gid->currentObjectID++ ,
                                                                      gid->currentObjectName );
                    } break;

                    case SIG_OBJECT_DECLARE_SKIN : {
                        obj = ( G3DOBJECT * ) g3dskin_new ( gid->currentObjectID++ ,
                                                            gid->currentObjectName,
                                                            gid->engineFlags );
                    } break;

                    case SIG_OBJECT_DECLARE_FFD : {
                        obj = ( G3DOBJECT * ) g3dffd_new ( gid->currentObjectID++ ,
                                                           gid->currentObjectName );
                    } break;

                    case SIG_OBJECT_DECLARE_WIREFRAME : {
                        obj = ( G3DOBJECT * ) g3dwireframe_new ( gid->currentObjectID++ ,
                                                                 gid->currentObjectName );
                    } break;

                    case SIG_OBJECT_DECLARE_SYMMETRY : {
                        obj = ( G3DOBJECT * ) g3dsymmetry_new ( gid->currentObjectID++ ,
                                                                gid->currentObjectName );
                    } break;

                    case SIG_OBJECT_DECLARE_TEXT : {
                        obj = ( G3DOBJECT * ) g3dtext_new ( gid->currentObjectID++ ,
                                                            gid->currentObjectName,
                                                            gid->engineFlags );
                    } break;

                    case SIG_OBJECT_DECLARE_BONE : {
                        obj = ( G3DOBJECT * ) g3dbone_new ( gid->currentObjectID++ ,
                                                            gid->currentObjectName,
                                                            0.0f );
                    } break;

                    case SIG_OBJECT_DECLARE_LIGHT : {
                        obj = ( G3DOBJECT * ) g3dlight_new ( gid->currentObjectID++ ,
                                                             gid->currentObjectName );
                    } break;

                    case SIG_OBJECT_DECLARE_MORPHER : {
                        obj = ( G3DOBJECT * ) g3dmorpher_new ( gid->currentObjectID++ ,
                                                               gid->currentObjectName,
                                                               gid->engineFlags );
                    } break;

                    case SIG_OBJECT_DECLARE_INSTANCE : {
                        obj = ( G3DOBJECT * ) g3dinstance_new ( gid->currentObjectID++ ,
                                                                gid->currentObjectName,
                                                                gid->currentScene );
                    } break;

                    case SIG_OBJECT_DECLARE_PARTICLEEMITTER : {
                        obj = ( G3DOBJECT * ) g3dparticleemitter_new ( gid->currentObjectID++ ,
                                                                       gid->currentObjectName,
                                                                       gid->currentScene );
                    } break;


                    default :
                        obj = g3dobject_new ( gid->currentObjectID++, 
                                              "NULL",
                                              gid->engineFlags );
                    break;
                }

                list_insert ( &gid->ldec, obj );
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

/******************************************************************************/
void g3dimportv3objects_define ( G3DIMPORTV3DATA *gid,
                                 uint32_t         chunkEnd, 
                                 FILE            *fsrc ) {
    uint32_t chunkSignature, chunkSize;
    uint32_t nbobj = list_count ( gid->ldec );

    if ( nbobj ) {
        LIST *ltmpdec = gid->ldec;

        gid->declaredObjects = ( G3DOBJECT ** ) calloc ( nbobj, sizeof ( G3DOBJECT * ) );

        while ( ltmpdec ) {
            G3DOBJECT *obj = ( G3DOBJECT * ) ltmpdec->data;

            gid->declaredObjects[obj->id] = obj;

            ltmpdec = ltmpdec->next;
        }
    }

    g3dimportv3data_incrementIndentLevel ( gid );

    g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_ENTRY : {
                gid->currentWeightgroupID = 0x00;
                gid->currentFacegroupID = 0x00;
                gid->currentUVMapID = 0x00;

                g3dimportv3object ( gid, ftell ( fsrc ) + chunkSize, fsrc );
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

/******************************************************************************/
void g3dimportv3root ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;

    g3dimportv3data_incrementIndentLevel ( gid );

    g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_EXTENSIONS : {
            } break;

            case SIG_EXTENSION_ENTRY : {
                g3dimportv3extension ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            case SIG_OBJECTS : {
            } break;

            case SIG_OBJECTS_DECLARE : {
                g3dimportv3objects_declare ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            case SIG_OBJECTS_DEFINE : {
                g3dimportv3objects_define ( gid, ftell ( fsrc ) + chunkSize, fsrc );
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


/******************************************************************************/
G3DSCENE *g3dscene_importv3 ( const char *filename,
                              G3DSCENE   *mergedScene,
                              LIST      *lextension,
                              uint32_t    flags ) {
    uint32_t chunkSignature, chunkSize;
    G3DIMPORTV3DATA gid;
    FILE *fsrc = NULL;

    if ( ( fsrc = fopen ( filename, "rb" ) ) == NULL ) {
        fprintf ( stderr, "Could not open %s !\n", filename );

        return 0x00;
    }

    memset ( &gid, 0x00, sizeof ( G3DIMPORTV3DATA ) );

    gid.lext = lextension;
    gid.currentScene = ( mergedScene ) ? mergedScene : 
                                         g3dscene_new ( gid.currentObjectID, 
                                                        "Scene" );
    gid.currentObjectID++; /*** scene ID is 0 ***/

    gid.engineFlags  = VIEWOBJECT;

    g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid.indentLevel);

        switch ( chunkSignature ) {
            case SIG_ROOT : {
                g3dimportv3root ( &gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            default : {
                fseek ( fsrc, chunkSize, SEEK_CUR );

                /*if ( ftell ( fsrc ) == chunkEnd ) {*/
                    /** hand the file back to the parent function ***/
                    /*return sce;
                }*/
            } break;
        }

        g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
        g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );

    fclose ( fsrc );

    if ( gid.currentVertexArray ) free ( gid.currentVertexArray );
    if ( gid.currentEdgeArray   ) free ( gid.currentEdgeArray   );
    if ( gid.currentFaceArray   ) free ( gid.currentFaceArray   );
    if ( gid.currentPointArray  ) free ( gid.currentPointArray  );
 
    /* apply modifiers, compute normals */
    g3dobject_updateMeshes_r ( ( G3DOBJECT * ) gid.currentScene, flags );

    g3dobject_anim_r ( ( G3DOBJECT * ) gid.currentScene, 0, flags );

    g3dobject_update_r ( ( G3DOBJECT * ) gid.currentScene, flags );

    return gid.currentScene;
}
