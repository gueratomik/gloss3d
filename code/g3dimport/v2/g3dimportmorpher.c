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
void g3dimportmorpher ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    G3DMORPHER *mpr = ( G3DMORPHER * ) gid->currentObject;
    uint32_t chunkSignature, chunkSize;
    uint32_t totalVertexCount = 0x00;
    G3DMORPHERMESHPOSE *mpose;

    g3dimportdata_incrementIndentLevel ( gid );

    g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_MORPHER_MESHPOSES : {
            } break;

            case SIG_OBJECT_MORPHER_MESHPOSE_ENTRY : {
            } break;

            case SIG_OBJECT_MORPHER_VERTEX_COUNT : {

                g3dimport_freadl ( &totalVertexCount, fsrc );
            } break;

            case SIG_OBJECT_MORPHER_MESHPOSE_NAME : {

                char name[0x100] = { 0 };

                g3dimport_fread ( name, chunkSize, 0x01, fsrc );

                mpose = g3dmorphermeshpose_new ( totalVertexCount, name );

                /*** Note: directly using g3dmorpher_addMeshPose() instead ***/
                /*** of g3dmorpher_createMeshPose prevents us from having ***/
                /*** multiple realloc calls when adding the vertices ***/
                g3dmorpher_addMeshPose ( mpr, 
                                         mpose,
                                         g3dmorpher_getAvailableSlot ( mpr ) );
            } break;

            case SIG_OBJECT_MORPHER_MESHPOSE_GEOMETRY : {
                uint32_t nbver;
                uint32_t i;

                g3dimport_freadl ( &nbver, fsrc );

                for ( i = 0x00; i < nbver; i++ ) {
                    G3DMORPHERVERTEXPOSE *vpose;
                    uint32_t verID;
                    G3DVECTOR vpos;

                    g3dimport_freadl ( &verID, fsrc );
                    g3dimport_freadl ( &vpos.x, fsrc );
                    g3dimport_freadl ( &vpos.y, fsrc );
                    g3dimport_freadl ( &vpos.z, fsrc );

                    g3dmorpher_addVertexPose ( mpr,
                                               gid->currentVertexArray[verID],
                                               mpose,
                                              &vpos );
                }
            } break;

            case SIG_OBJECT_MORPHER_RESETPOSITIONS : {
                uint32_t nbver;
                uint32_t i;

                g3dimport_freadl ( &nbver, fsrc );
                for ( i = 0x00; i < nbver; i++ ) {
                    uint32_t verID;
                    G3DVECTOR vpos;

                    g3dimport_freadl ( &verID, fsrc );
                    g3dimport_freadl ( &vpos.x, fsrc );
                    g3dimport_freadl ( &vpos.y, fsrc );
                    g3dimport_freadl ( &vpos.z, fsrc );

                    g3dmorpher_setVertexResetPosition ( mpr,
                                                        gid->currentVertexArray[verID],
                                                       &vpos );
                }
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
