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
void g3dimportv2mesh ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;

    g3dimportv2data_incrementIndentLevel ( gid );

    g3dimportv2_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv2_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_MESH_GEOMETRY : {
            } break;

            case SIG_OBJECT_MESH_FACEGROUPS : {
            } break;

            case SIG_OBJECT_MESH_FACEGROUP_ENTRY : {
            } break;

            case SIG_OBJECT_MESH_FACEGROUP_NAME : {
                G3DMESH *mes = ( G3DMESH * ) gid->currentObject;

                if ( chunkSize ) {
                    char *name = calloc ( 0x01, chunkSize );

                    g3dimportv2_fread ( name, chunkSize, 0x01, fsrc );

                    gid->currentFacegroup = g3dfacegroup_new ( name, NULL );

                    g3dmesh_addFacegroup ( mes, gid->currentFacegroup );

                    gid->currentFacegroup->id = gid->currentFacegroupID++;

                    free ( name );
                }
            } break;

            case SIG_OBJECT_MESH_FACEGROUP_FACES : {
                uint32_t nbFaces;
                uint32_t i;

                g3dimportv2_freadl ( &nbFaces, fsrc );

                for ( i = 0x00; i < nbFaces; i++ ) {
                    uint32_t facID;

                    g3dimportv2_freadl ( &facID , fsrc );

                    g3dface_addFacegroup ( gid->currentFaceArray[facID],
                                           gid->currentFacegroup );
                }
            } break;

            case SIG_OBJECT_MESH_WEIGHTGROUPS : {
            } break;

            case SIG_OBJECT_MESH_WEIGHTGROUP_ENTRY : {
            } break;

            case SIG_OBJECT_MESH_WEIGHTGROUP_NAME : {
                G3DMESH *mes = ( G3DMESH * ) gid->currentObject;

                if ( chunkSize ) {
                    char *name = calloc ( 0x01, chunkSize );

                    g3dimportv2_fread ( name, chunkSize, 0x01, fsrc );

                    gid->currentWeightgroup = g3dweightgroup_new ( mes, name );

                    g3dmesh_addWeightGroup ( mes, gid->currentWeightgroup );

                    gid->currentWeightgroup->id = gid->currentWeightgroupID++;

                    free ( name );
                }
            } break;

            case SIG_OBJECT_MESH_WEIGHTGROUP_WEIGHTS : {
                uint32_t nbWeights;
                uint32_t i;

                g3dimportv2_freadl ( &nbWeights, fsrc );

                for ( i = 0x00; i < nbWeights; i++ ) {
                    uint32_t verID;
                    float weight;

                    g3dimportv2_freadl ( &verID , fsrc );
                    g3dimportv2_freadf ( &weight, fsrc );

                    g3dvertex_addWeight ( gid->currentVertexArray[verID],
                                          g3dweight_new ( weight,
                                                          gid->currentWeightgroup ) );
                }
            } break;

            case SIG_OBJECT_MESH_GEOMETRY_VERTICES : {
                G3DMESH *mes = ( G3DMESH * ) gid->currentObject;
                uint32_t nbver;
                uint32_t i;

                g3dimportv2_freadl ( &nbver, fsrc );

                gid->currentVertexArray = ( G3DVERTEX ** ) realloc ( gid->currentVertexArray, nbver * sizeof ( G3DVERTEX * ) );

                for ( i = 0x00; i < nbver; i++ ) {
                    uint32_t verFlags;
                    float x, y, z;

                    g3dimportv2_freadf ( &x       , fsrc );
                    g3dimportv2_freadf ( &y       , fsrc );
                    g3dimportv2_freadf ( &z       , fsrc );
                    g3dimportv2_freadl ( &verFlags, fsrc );

                    gid->currentVertexArray[i] = g3dvertex_new ( x, y, z );

                    gid->currentVertexArray[i]->id    = i;
                    gid->currentVertexArray[i]->flags = verFlags;

                    g3dmesh_addVertex ( mes, gid->currentVertexArray[i] );
                }

                g3dmesh_updateBbox ( mes );
            } break;

            case SIG_OBJECT_MESH_GEOMETRY_POLYGONS_WITH_EDGES :
            /*** kept for legacy ***/
            case SIG_OBJECT_MESH_GEOMETRY_POLYGONS : {
                G3DMESH *mes = ( G3DMESH * ) gid->currentObject;
                uint32_t nbfac;
                uint32_t i;

                g3dimportv2_freadl ( &nbfac, fsrc );

                gid->currentFaceArray = ( G3DFACE ** ) realloc ( gid->currentFaceArray, nbfac * sizeof ( G3DFACE * ) );

                for ( i = 0x00; i < nbfac; i++ ) {
                    uint32_t v0ID, v1ID, v2ID, v3ID;
                    G3DFACE *fac;

                    g3dimportv2_freadl ( &v0ID, fsrc );
                    g3dimportv2_freadl ( &v1ID, fsrc );
                    g3dimportv2_freadl ( &v2ID, fsrc );
                    g3dimportv2_freadl ( &v3ID, fsrc );

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

                    if ( chunkSignature == SIG_OBJECT_MESH_GEOMETRY_POLYGONS_WITH_EDGES ) {
                        uint32_t e0ID, e1ID, e2ID, e3ID;


                        g3dimportv2_freadl ( &e0ID, fsrc );
                        g3dimportv2_freadl ( &e1ID, fsrc );
                        g3dimportv2_freadl ( &e2ID, fsrc );
                        g3dimportv2_freadl ( &e3ID, fsrc );

                        /*** kept for legac but edegs are no longer stored ***/
                        /*** int the file. Too complicated ***/
                        g3dmesh_addFace ( mes, fac );
                    } else {
                        g3dmesh_addFace ( mes, fac );
                    }
                }
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
