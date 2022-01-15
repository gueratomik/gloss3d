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
void g3dimportv3uvmap ( G3DIMPORTV3DATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t uvmapProjection;
    uint32_t chunkSignature, chunkSize;
    

    g3dimportv3data_incrementIndentLevel ( gid );

    g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_OBJECT_UVMAP_ENTRY : {
            } break;

            case SIG_OBJECT_UVMAP_PROJECTION : {
                g3dimportv3_freadl ( &uvmapProjection, fsrc );
            } break;

            case SIG_OBJECT_UVMAP_NAME : {
                if ( chunkSize ) {
                    char *name = ( char * ) calloc ( 0x01, chunkSize );

                    g3dimportv3_fread ( name, chunkSize, 0x01, fsrc );

                    gid->currentUVMap = g3duvmap_new ( name, uvmapProjection );

                    ((G3DOBJECT*)gid->currentUVMap)->id = gid->currentUVMapID++;

                    if ( gid->currentObject->type & MESH ) {
                        g3dmesh_addUVMap ( ( G3DMESH * ) gid->currentObject,
                                                         gid->currentUVMap,
                                                         NULL,
                                                         gid->engineFlags );
                    }

                    free ( name );
                }
            } break;

            case SIG_OBJECT_UVMAP_FLAGS : {
                g3dimportv3_freadl ( &((G3DOBJECT*)gid->currentUVMap)->flags, fsrc );
            } break;

            case SIG_OBJECT_UVMAP_RADIUS : {
                g3dimportv3_freadf ( &gid->currentUVMap->pln.xradius, fsrc );
                g3dimportv3_freadf ( &gid->currentUVMap->pln.yradius, fsrc );
            } break;

            case SIG_OBJECT_UVMAP_TRANSFORMATION : {
                G3DOBJECT *obj = ( G3DOBJECT * ) gid->currentUVMap;
                G3DMESH *mes = ( G3DMESH * ) gid->currentObject;

                g3dimportv3_freadf ( &obj->pos.x, fsrc );
                g3dimportv3_freadf ( &obj->pos.y, fsrc );
                g3dimportv3_freadf ( &obj->pos.z, fsrc );
                g3dimportv3_freadf ( &obj->pos.w, fsrc );

                g3dimportv3_freadf ( &obj->rot.x, fsrc );
                g3dimportv3_freadf ( &obj->rot.y, fsrc );
                g3dimportv3_freadf ( &obj->rot.z, fsrc );
                g3dimportv3_freadf ( &obj->rot.w, fsrc );

                g3dimportv3_freadf ( &obj->sca.x, fsrc );
                g3dimportv3_freadf ( &obj->sca.y, fsrc );
                g3dimportv3_freadf ( &obj->sca.z, fsrc );
                g3dimportv3_freadf ( &obj->sca.w, fsrc );

                g3dobject_updateMatrix_r ( obj, gid->engineFlags );

                if ( ( obj->flags & UVMAPFIXED ) == 0x00 ) {
                    g3duvmap_applyProjection ( gid->currentUVMap, mes );
                }
            } break;

            case SIG_OBJECT_UVMAP_UVSETS : {
                uint32_t nbUVSets;
                uint32_t i;

                g3dimportv3_freadl ( &nbUVSets, fsrc );

                for ( i = 0x00; i < nbUVSets; i++ ) {
                    G3DUVSET *uvs = NULL;
                    uint32_t flags;
                    uint32_t facID;
                    G3DUVSET uvset;

                    memset ( &uvset, 0x00, sizeof ( G3DUVSET ) );

                    g3dimportv3_freadl ( &flags   , fsrc );
                    g3dimportv3_freadl ( &facID   , fsrc );

                    g3dimportv3_freadf ( &uvset.veruv[0x00].u, fsrc );
                    g3dimportv3_freadf ( &uvset.veruv[0x00].v, fsrc );
                    g3dimportv3_freadf ( &uvset.veruv[0x01].u, fsrc );
                    g3dimportv3_freadf ( &uvset.veruv[0x01].v, fsrc );
                    g3dimportv3_freadf ( &uvset.veruv[0x02].u, fsrc );
                    g3dimportv3_freadf ( &uvset.veruv[0x02].v, fsrc );
                    g3dimportv3_freadf ( &uvset.veruv[0x03].u, fsrc );
                    g3dimportv3_freadf ( &uvset.veruv[0x03].v, fsrc );

                    uvs = g3dface_getUVSet ( gid->currentFaceArray[facID], 
                                             gid->currentUVMap );

                    if ( uvs ) {
                        memcpy ( uvs->veruv, 
                                &uvset.veruv, sizeof ( G3DUV ) * 0x04 );
                    }
                }
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
