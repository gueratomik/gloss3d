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
void g3dimportv2extension ( G3DIMPORTV2DATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;

    g3dimportv2data_incrementIndentLevel ( gid );

    g3dimportv2_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv2_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        G3DIMPORTV2EXTENSION *ext = g3dimportv2extension_getFromList ( chunkSignature, gid->lext );

        if ( ext ) {
            printf ( "Extension found - Signature: %08X\n", chunkSignature );

            ext->read ( gid, chunkEnd, fsrc, ext->data );
        } else {
            fseek ( fsrc, chunkSize, SEEK_CUR );
        }

        /** hand the file back to the parent function ***/
        if ( ftell ( fsrc ) == chunkEnd ) break;

        g3dimportv2_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
        g3dimportv2_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );

    g3dimportv2data_decrementIndentLevel ( gid );
}

/******************************************************************************/
G3DIMPORTV2EXTENSION *g3dimportv2extension_new ( uint32_t signature,
                                             void (*read)( G3DIMPORTV2DATA *gid,
                                                           uint32_t       chunkEnd,
                                                           FILE          *fsrc,
                                                           void          *data ),
                                             void *data) {
    uint32_t size = sizeof ( G3DIMPORTV2EXTENSION );
    G3DIMPORTV2EXTENSION *ext = ( G3DIMPORTV2EXTENSION * ) calloc ( 0x01, size );

    if ( ext == NULL ) {
        fprintf ( stderr, "g3dimportv2extension_new: calloc failed\n");

        return NULL;
    }

    ext->signature = signature;
    ext->read      = read;
    ext->data      = data;

    return ext;
}

/******************************************************************************/
void g3dimportv2extension_free ( G3DIMPORTV2EXTENSION *ext ) {
    free ( ext );
}

/******************************************************************************/
G3DIMPORTV2EXTENSION *g3dimportv2extension_getFromList ( uint32_t signature,
                                                     LIST    *lext ) {
    LIST *ltmpext = lext;

    while ( ltmpext ) {
        G3DIMPORTV2EXTENSION *ext = ( G3DIMPORTV2EXTENSION * ) ltmpext->data;

        if ( ext->signature == signature ) return ext;

        ltmpext = ltmpext->next;
    }

    return NULL;
}
