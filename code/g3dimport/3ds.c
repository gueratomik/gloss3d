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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
#include <g3dimport.h>

/*****************************************************************************/
G3DSCENE *g3dscene_import3ds ( const char *filename, uint32_t flags ) {
    uint16_t chunkid;
    uint32_t chunklen;
    G3DVERTEX **ver = NULL;
    G3DSCENE *sce = NULL;
    G3DMESH *mes = NULL;
    char str[0x100];
    FILE *fsrc;

    if ( ( fsrc = fopen ( filename, "rb" ) ) == NULL ) {
        fprintf ( stderr, "g3dscene_import3ds: Cannot open %s\n", filename );

        return NULL;
    }

    readf ( &chunkid , sizeof ( chunkid  ), 0x01, fsrc );
    readf ( &chunklen, sizeof ( chunklen ), 0x01, fsrc );

    if ( chunkid != 0x4D4D ) {
        fprintf ( stderr, "g3dscene_import3ds: not a 3DS file !\n" );

        fclose ( fsrc );

        return NULL;
    }

    while ( feof ( fsrc ) == 0x00 ) {
        switch ( chunkid ) {
            case 0x4D4D :
                sce = g3dscene_new ( 0x00, "Scene" );
            break;

            case 0x3D3D :
            break;

            case 0x4000 : {
                int i = 0x00;

                do {
                    readf ( &str[i], sizeof ( char ), 0x01, fsrc );
                } while ( str[i++] != 0x00 );
            } break;

            case 0x4100 :
                mes = g3dmesh_new ( g3dscene_getNextObjectID ( sce ), "Mesh", flags );
                g3dobject_addChild ( ( G3DOBJECT * ) sce, ( G3DOBJECT * ) mes, flags );
            break;

            case 0x4110 : {
                G3DOBJECT *obj = ( G3DOBJECT * ) mes;
                uint16_t nb;
                float x, y, z;
                int i;

                readf ( &nb, sizeof ( nb ), 0x01, fsrc );
                ver = ( G3DVERTEX ** ) calloc ( nb, sizeof ( G3DVERTEX * ) );

                for ( i = 0x00; i < nb; i++ ) {
                    readf ( &x, sizeof ( float ), 0x01, fsrc );
                    readf ( &z, sizeof ( float ), 0x01, fsrc );
/*** Gloss3D's up vector is Y, 3ds Up vector is Z, we swap ***/
                    readf ( &y, sizeof ( float ), 0x01, fsrc );

                    x = ( x / 100.0f );
                    y = ( y / 100.0f );
                    z = ( z / 100.0f );

                    /*if ( i == 0x00 ) {
                        obj->bbox.xmin = obj->bbox.xmax = x;
                        obj->bbox.ymin = obj->bbox.ymax = y;
                        obj->bbox.zmin = obj->bbox.zmax = z;
                    } else {
                        if ( x > obj->bbox.xmax ) obj->bbox.xmax = x;
                        if ( x < obj->bbox.xmin ) obj->bbox.xmin = x;
                        if ( y > obj->bbox.ymax ) obj->bbox.ymax = y;
                        if ( y < obj->bbox.ymin ) obj->bbox.ymin = y;
                        if ( z > obj->bbox.zmax ) obj->bbox.zmax = z;
                        if ( z < obj->bbox.zmin ) obj->bbox.zmin = z;
                    }*/

                    ver[i] = g3dvertex_new ( x, y, z );
                    g3dmesh_addVertex ( mes, ver[i] );

                    g3dmesh_updateBbox ( mes );
                }
            } break;

            case 0x4120 : {
                uint16_t nb, face[0x04];
                int i;

                readf ( &nb, sizeof ( nb ), 0x01, fsrc );

                for ( i = 0x00; i < nb; i++ ) {
                    G3DVERTEX *v0, *v1, *v2;
                    G3DFACE *fac;

                    /*** read vertices IDs ***/
                    readf ( &face, sizeof ( face ), 0x01, fsrc );

                    /*** retrieve vertices pointers ***/
                    v0 = ver[face[0x00]];
                    v1 = ver[face[0x01]];
                    v2 = ver[face[0x02]];

                    /*** Create face ***/
                    fac = g3dtriangle_new ( v0, v1, v2 );

                    g3dmesh_addFace ( mes, fac );
                }

                g3dmesh_vertexNormal ( mes );
            } break;

            default :
                fseek ( fsrc, chunklen - 0x06, SEEK_CUR );
            break;
        }

        readf ( &chunkid , sizeof ( chunkid  ), 0x01, fsrc );
        readf ( &chunklen, sizeof ( chunklen ), 0x01, fsrc );
    }

    fclose ( fsrc );

    g3dscene_updateMeshes ( sce, flags );


    return sce;
}
