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
G3DSCENE *g3dscene_importv2 ( const char *filename,
                              uint32_t    flags ) {
    G3DSCENE *sce = g3dscene_new ( 0x00, "Scene" );
    uint32_t chunksig, chunklen;
    FILE *fsrc = NULL;

    if ( ( fsrc = fopen ( filename, "rb" ) ) == NULL ) {
        fprintf ( stderr, "Could not open %s !\n", filename );

        return 0x00;
    }

    readf ( &chunksig, sizeof ( uint32_t ), 0x01, fsrc );
    readf ( &chunklen, sizeof ( uint32_t ), 0x01, fsrc );

    do {
        fprintf( stderr, "SIG:%X LEN:%d\n", chunksig, chunklen );

        switch ( chunksig ) {

            case SIG_OBJECT : {
                fprintf ( stderr, "Object found\n" );

                g3dimportobject ( sce, fsrc );
            } break;


            default :
                fseek ( fsrc, chunklen, SEEK_CUR );
            break;
        }

        readf ( &chunksig, sizeof ( uint32_t ), 0x01, fsrc );
        readf ( &chunklen, sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );

    fclose ( fsrc );

    return sce;
}
