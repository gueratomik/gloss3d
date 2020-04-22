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
void g3dimportobject ( G3DOBJECT *obj, FILE *fsrc ) {
    char objectName[0x100] = { 0 };
    uint32_t chunksig, chunklen;

    readf ( &chunksig, sizeof ( uint32_t ), 0x01, fsrc );
    readf ( &chunklen, sizeof ( uint32_t ), 0x01, fsrc );

    do {
        fprintf( stderr, "SIG:%X LEN:%d\n", chunksig, chunklen );

        switch ( chunksig ) {
            case SIG_OBJECT_IDENTITY : {
                fprintf ( stderr, "SIG_OBJECT_IDENTITY found\n" );
            } break;

            /*case SIG_OBJECT_IDENTITY_NAME : {
                fprintf ( stderr, "SIG_OBJECT_IDENTITY_NAME found\n" );
            } break;

            case SIG_OBJECT_IDENTITY_TYPE : {
                fprintf ( stderr, "SIG_OBJECT_IDENTITY_TYPE found\n" );
            } break;

            case SIG_OBJECT_IDENTITY_PARENT : {
                fprintf ( stderr, "SIG_OBJECT_IDENTITY_PARENT found\n" );
            } break;

            case SIG_OBJECT_IDENTITY_ACTIVE : {
                fprintf ( stderr, "SIG_OBJECT_IDENTITY_ACTIVE found\n" );
            } break;

            case SIG_OBJECT_TRANSFORMATION : {
                fprintf ( stderr, "SIG_OBJECT_TRANSFORMATION found\n" );
            } break;

            case SIG_OBJECT_KEYS : {
                fprintf ( stderr, "SIG_OBJECT_KEYS found\n" );
            } break;

            case SIG_OBJECT_KEYS_KEY : {
                fprintf ( stderr, "SIG_OBJECT_KEYS_KEY found\n" );
            } break;

            case SIG_OBJECT_KEYS_KEY_TRANSFORMATION : {
                fprintf ( stderr, "SIG_OBJECT_KEYS_KEY_TRANSFORMATION found\n" );
            } break;

            case SIG_OBJECT_KEYS_KEY_LOOP : {
                fprintf ( stderr, "SIG_OBJECT_KEYS_KEY_LOOP found\n" );
            } break;*/

            default :
                fseek ( fsrc, chunklen, SEEK_CUR );
            break;
        }

        readf ( &chunksig, sizeof ( uint32_t ), 0x01, fsrc );
        readf ( &chunklen, sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );
}
