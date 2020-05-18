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
void g3dimportextension ( G3DIMPORTDATA *gid, uint32_t chunkEnd, FILE *fsrc ) {
    uint32_t chunkSignature, chunkSize;

    g3dimportdata_incrementIndentLevel ( gid );

    g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        G3DIMPORTEXTENSION *ext = g3dimportextension_getFromList ( chunkSignature, gid->lext );

        if ( ext ) {
            printf ( "Extension found - Signature: %08X\n", chunkSignature );

            ext->read ( gid, chunkEnd, fsrc, ext->data );
        } else {
            fseek ( fsrc, chunkSize, SEEK_CUR );
        }

        /** hand the file back to the parent function ***/
        if ( ftell ( fsrc ) == chunkEnd ) break;

        g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
        g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );

    g3dimportdata_decrementIndentLevel ( gid );
}

/******************************************************************************/
G3DIMPORTEXTENSION *g3dimportextension_new ( uint32_t signature,
                                             void (*read)( G3DIMPORTDATA *gid,
                                                           uint32_t       chunkEnd,
                                                           FILE          *fsrc,
                                                           void          *data ),
                                             void *data) {
    uint32_t size = sizeof ( G3DIMPORTEXTENSION );
    G3DIMPORTEXTENSION *ext = ( G3DIMPORTEXTENSION * ) calloc ( 0x01, size );

    if ( ext == NULL ) {
        fprintf ( stderr, "g3dimportextension_new: calloc failed\n");

        return NULL;
    }

    ext->signature = signature;
    ext->read      = read;
    ext->data      = data;

    return ext;
}

/******************************************************************************/
void g3dimportextension_free ( G3DIMPORTEXTENSION *ext ) {
    free ( ext );
}

/******************************************************************************/
G3DIMPORTEXTENSION *g3dimportextension_getFromList ( uint32_t signature,
                                                     LIST    *lext ) {
    LIST *ltmpext = lext;

    while ( ltmpext ) {
        G3DIMPORTEXTENSION *ext = ( G3DIMPORTEXTENSION * ) ltmpext->data;

        if ( ext->signature == signature ) return ext;

        ltmpext = ltmpext->next;
    }

    return NULL;
}
