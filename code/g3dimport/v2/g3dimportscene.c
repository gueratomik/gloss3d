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
void g3dimportscene ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;

    g3dimportdata_incrementIndentLevel ( gid );

    g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_MATERIALS : {

            } break;

            case SIG_MATERIAL_ENTRY : {
                g3dimportmaterial ( gid, ftell ( fsrc ) + chunkSize, fsrc );
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

/******************************************************************************/
void g3dimportroot ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;

    g3dimportdata_incrementIndentLevel ( gid );

    g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_EXTENSIONS : {
            } break;

            case SIG_EXTENSION_ENTRY : {
                g3dimportextension ( gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            case SIG_OBJECTS : {
            } break;

            case SIG_OBJECT_ENTRY : {
                gid->currentWeightgroupID = 0x00;
                gid->currentFacegroupID = 0x00;
                gid->currentUVMapID = 0x00;

                g3dimportobject ( gid, ftell ( fsrc ) + chunkSize, fsrc );
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


/******************************************************************************/
G3DSCENE *g3dscene_importv2 ( const char *filename,
                              G3DSCENE   *mergedScene,
                              LIST      *lextension,
                              uint32_t    flags ) {
    uint32_t chunkSignature, chunkSize;
    G3DIMPORTDATA gid;
    FILE *fsrc = NULL;

    if ( ( fsrc = fopen ( filename, "rb" ) ) == NULL ) {
        fprintf ( stderr, "Could not open %s !\n", filename );

        return 0x00;
    }

    memset ( &gid, 0x00, sizeof ( G3DIMPORTDATA ) );

    gid.lext = lextension;
    gid.currentScene = ( mergedScene ) ? mergedScene : 
                                         g3dscene_new ( gid.currentObjectID++, 
                                                        "Scene" );
    gid.engineFlags  = VIEWOBJECT;

    g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid.indentLevel);

        switch ( chunkSignature ) {
            case SIG_ROOT : {
                g3dimportroot ( &gid, ftell ( fsrc ) + chunkSize, fsrc );
            } break;

            default : {
                fseek ( fsrc, chunkSize, SEEK_CUR );

                /*if ( ftell ( fsrc ) == chunkEnd ) {*/
                    /** hand the file back to the parent function ***/
                    /*return sce;
                }*/
            } break;
        }

        g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
        g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );

    fclose ( fsrc );

    /* apply modifiers, compute normals */
    g3dobject_updateMeshes_r ( gid.currentScene, flags );

    /*** Disable default light if there are some ligths ***/
    g3dscene_checkLights ( gid.currentScene );

    return gid.currentScene;
}
