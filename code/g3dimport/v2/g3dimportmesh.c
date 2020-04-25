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
void g3dimportmesh ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;

    g3dimportdata_incrementIndentLevel ( gid );

    g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_MESH_GEOMETRY : {

            } break;

            case SIG_OBJECT_MESH_GEOMETRY_VERTICES : {
                G3DMESH *mes = ( G3DMESH * ) gid->currentObject;
                uint32_t nbver;
                uint32_t i;

                g3dimport_freadl ( &nbver, fsrc );

                gid->currentVertexArray = ( G3DVERTEX ** ) realloc ( gid->currentVertexArray, nbver * sizeof ( G3DVERTEX * ) );

                for ( i = 0x00; i < nbver; i++ ) {
                    float x, y, z, w;

                    g3dimport_freadf ( &x, fsrc );
                    g3dimport_freadf ( &y, fsrc );
                    g3dimport_freadf ( &z, fsrc );
                    g3dimport_freadf ( &w, fsrc );

                    gid->currentVertexArray[i] = g3dvertex_new ( x, y, z );

                    g3dmesh_addVertex ( mes, gid->currentVertexArray[i] );
                }

                g3dmesh_updateBbox ( mes );
            } break;

            case SIG_OBJECT_MESH_GEOMETRY_POLYGONS : {
                G3DMESH *mes = ( G3DMESH * ) gid->currentObject;
                uint32_t nbfac;
                uint32_t i;

                g3dimport_freadl ( &nbfac, fsrc );

                gid->currentFaceArray = ( G3DFACE ** ) realloc ( gid->currentFaceArray, nbfac * sizeof ( G3DFACE * ) );

                for ( i = 0x00; i < nbfac; i++ ) {
                    uint32_t v0ID, v1ID, v2ID, v3ID;
                    G3DFACE *fac;

                    g3dimport_freadl ( &v0ID, fsrc );
                    g3dimport_freadl ( &v1ID, fsrc );
                    g3dimport_freadl ( &v2ID, fsrc );
                    g3dimport_freadl ( &v3ID, fsrc );

                    if ( v2ID == v3ID ) {
                        fac = g3dtriangle_new ( gid->currentVertexArray[v0ID],
                                                gid->currentVertexArray[v1ID], 
                                                gid->currentVertexArray[v2ID] );
                    } else {
                        fac = g3dquad_new ( gid->currentVertexArray[v0ID],
                                            gid->currentVertexArray[v1ID], 
                                            gid->currentVertexArray[v2ID], 
                                            gid->currentVertexArray[v3ID] );
                    }

                    gid->currentFaceArray[i] = fac;

                    g3dmesh_addFace ( mes, fac );
                }

                g3dmesh_update ( mes, NULL,
                                      NULL,
                                      NULL,
                                      UPDATEVERTEXNORMAL  |
                                      UPDATEFACEPOSITION  |
                                      UPDATEFACENORMAL, gid->engineFlags );
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
