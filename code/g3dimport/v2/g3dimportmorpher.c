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
void g3dimportv2morpher ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    G3DMORPHER *mpr = ( G3DMORPHER * ) gid->currentObject;
    uint32_t chunkSignature, chunkSize;
    uint32_t totalVertexCount = 0x00;
    G3DMORPHERMESHPOSE *mpose;
    char mposename[0x100] = { 0 };

    if ( chunkEnd == ftell ( fsrc ) ) return;

    g3dimportv2data_incrementIndentLevel ( gid );

    g3dimportv2_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv2_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_MORPHER_MESHPOSES : {
            } break;

            case SIG_OBJECT_MORPHER_VERTEX_COUNT : {
                g3dimportv2_freadl ( &totalVertexCount, fsrc );
            } break;

            case SIG_OBJECT_MORPHER_MESHPOSE_ENTRY : {
            } break;

            case SIG_OBJECT_MORPHER_MESHPOSE_NAME : {
                memset ( mposename, 0x00, sizeof ( mposename ) );

                g3dimportv2_fread ( mposename, chunkSize, 0x01, fsrc );
            } break;

            case SIG_OBJECT_MORPHER_MESHPOSE_SLOT_ID : {
                uint32_t slotID = 0x00;

                g3dimportv2_freadl ( &slotID, fsrc );

                mpose = g3dmorphermeshpose_new ( totalVertexCount, mposename );

                /*** Note: directly using g3dmorpher_addMeshPose() instead ***/
                /*** of g3dmorpher_createMeshPose prevents us from having ***/
                /*** multiple realloc calls when adding the vertices ***/
                g3dmorpher_addMeshPose ( mpr, 
                                         mpose,
                                         slotID );
            } break;

            case SIG_OBJECT_MORPHER_MESHPOSE_GEOMETRY : {
                G3DMESH *mes = ( G3DMESH * ) ((G3DOBJECT*)mpr)->parent;
                /*** Note: this is not efficient and should be ***/
                /*** buffered somewhere ***/
                G3DVERTEX **ver = ( G3DVERTEX ** ) list_to_reversed_array ( mes->vertexList );
                uint32_t nbver;
                uint32_t i;

                g3dimportv2_freadl ( &nbver, fsrc );

                for ( i = 0x00; i < nbver; i++ ) {
                    G3DMORPHERVERTEXPOSE *vpose;
                    uint32_t verID;
                    G3DVECTOR3F vpos;

                    g3dimportv2_freadl ( &verID, fsrc );
                    g3dimportv2_freadf ( &vpos.x, fsrc );
                    g3dimportv2_freadf ( &vpos.y, fsrc );
                    g3dimportv2_freadf ( &vpos.z, fsrc );

                    g3dmorpher_addVertexPose ( mpr,
                                               ver[verID],
                                               mpose,
                                              &vpos );
                }

                /*** Note: this is not efficient and should be ***/
                /*** buffered somewhere ***/
                if ( ver ) free ( ver );
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
