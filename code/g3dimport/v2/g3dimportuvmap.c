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
void g3dimportuvmap ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t uvmapProjection;
    uint32_t chunkSignature, chunkSize;
    

    g3dimportdata_incrementIndentLevel ( gid );

    g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_UVMAP_ENTRY : {
            } break;

            case SIG_OBJECT_UVMAP_PROJECTION : {
                g3dimport_freadl ( &uvmapProjection, fsrc );
            } break;

            case SIG_OBJECT_UVMAP_NAME : {
                if ( chunkSize ) {
                    char *name = ( char * ) calloc ( 0x01, chunkSize );

                    g3dimport_fread ( name, chunkSize, 0x01, fsrc );

                    gid->currentUVMap = g3duvmap_new ( name, uvmapProjection );

                    if ( gid->currentObject->type & MESH ) {
                        g3dmesh_addUVMap ( ( G3DMESH * ) gid->currentObject,
                                                         gid->currentUVMap, 
                                                         gid->engineFlags );
                    }

                    free ( name );
                }
            } break;

            case SIG_OBJECT_UVMAP_FLAGS : {
                g3dimport_freadl ( &((G3DOBJECT*)gid->currentUVMap)->flags, fsrc );
            } break;

            case SIG_OBJECT_UVMAP_RADIUS : {
                g3dimport_freadf ( &gid->currentUVMap->pln.xradius, fsrc );
                g3dimport_freadf ( &gid->currentUVMap->pln.yradius, fsrc );
            } break;

            case SIG_OBJECT_UVMAP_TRANSFORMATION : {
                G3DOBJECT *obj = ( G3DOBJECT * ) gid->currentUVMap;

                g3dimport_freadf ( &obj->pos.x, fsrc );
                g3dimport_freadf ( &obj->pos.y, fsrc );
                g3dimport_freadf ( &obj->pos.z, fsrc );
                g3dimport_freadf ( &obj->pos.w, fsrc );

                g3dimport_freadf ( &obj->rot.x, fsrc );
                g3dimport_freadf ( &obj->rot.y, fsrc );
                g3dimport_freadf ( &obj->rot.z, fsrc );
                g3dimport_freadf ( &obj->rot.w, fsrc );

                g3dimport_freadf ( &obj->sca.x, fsrc );
                g3dimport_freadf ( &obj->sca.y, fsrc );
                g3dimport_freadf ( &obj->sca.z, fsrc );
                g3dimport_freadf ( &obj->sca.w, fsrc );

                g3dobject_updateMatrix_r ( obj, gid->engineFlags );

                if ( ( obj->flags & UVMAPFIXED ) == 0x00 ) {
                    g3duvmap_applyProjection ( gid->currentUVMap,
                                               gid->currentObject );
                }
            } break;

            case SIG_OBJECT_UVMAP_UVSETS : {
                uint32_t nbUVSets;
                uint32_t i;

                g3dimport_freadl ( &nbUVSets, fsrc );

                for ( i = 0x00; i < nbUVSets; i++ ) {
                    G3DUVSET *uvs = NULL;
                    uint32_t flags;
                    uint32_t facID;
                    G3DUVSET uvset;

                    g3dimport_freadl ( &flags   , fsrc );
                    g3dimport_freadl ( &facID   , fsrc );

                    g3dimport_freadf ( &uvset.veruv[0x00].u, fsrc );
                    g3dimport_freadf ( &uvset.veruv[0x00].v, fsrc );
                    g3dimport_freadf ( &uvset.veruv[0x01].u, fsrc );
                    g3dimport_freadf ( &uvset.veruv[0x01].v, fsrc );
                    g3dimport_freadf ( &uvset.veruv[0x02].u, fsrc );
                    g3dimport_freadf ( &uvset.veruv[0x02].v, fsrc );
                    g3dimport_freadf ( &uvset.veruv[0x03].u, fsrc );
                    g3dimport_freadf ( &uvset.veruv[0x03].v, fsrc );

                    uvs = g3dface_getUVSet ( gid->currentFaceArray[facID], 
                                             gid->currentUVMap );

                    if ( uvs ) {
                        memcpy ( uvs, &uvset, sizeof ( G3DUVSET ) );
                    }
                }
            } break;

            case SIG_OBJECT_UVMAP_MATERIALS : {
            } break;

            case SIG_OBJECT_UVMAP_MATERIAL_ENTRY : {
                G3DMESH *mes = ( G3DMESH * ) gid->currentObject;
                G3DMATERIAL *mat;
                uint32_t matID;

                g3dimport_freadl ( &matID, fsrc );

                mat = g3dscene_getMaterialByID ( gid->currentScene, matID );

                if ( mat ) {
                    g3dmesh_addMaterial ( gid->currentObject, mat, 
                                          gid->currentUVMap );
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
