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
#include <g3dexportv2.h>

/******************************************************************************/
uint32_t g3dexport_fwrited ( double *d, FILE *stream ) {
    return g3dexport_fwrite ( d, sizeof ( double ), 0x01, stream );
}

/******************************************************************************/
uint32_t g3dexport_fwritef ( float *f, FILE *stream ) {
    return g3dexport_fwrite ( f, sizeof ( float ), 0x01, stream );
}

/******************************************************************************/
uint32_t g3dexport_fwritel ( uint32_t *l, FILE *stream ) {
    return g3dexport_fwrite ( l, sizeof ( uint32_t ), 0x01, stream );
}

/******************************************************************************/
uint32_t g3dexport_fwritell ( uint64_t *ll, FILE *stream ) {
    return g3dexport_fwrite ( ll, sizeof ( uint64_t ), 0x01, stream );
}

/******************************************************************************/
/*** Write to file or, if stream is NULL, return the size to write. ***/
/*** this is useful to compute the chunk's future size. All file writing op ***/
/*** MUST call this function. ***/
uint32_t g3dexport_fwrite ( void   *ptr,
                            size_t  size,
                            size_t  count,
                            FILE   *stream ) {
    uint32_t written = 0x00;

    if ( stream ) {
        uint32_t realCount = fwrite ( ptr, size, count, stream );

        written = size * realCount;
    } else {
        written = ( size * count );
    }

    return written;
}

/******************************************************************************/
uint32_t g3dexport_writeChunk ( uint32_t   chunkSignature,
                                uint32_t (*writeChunk) ( G3DEXPORTDATA *ged,
                                                         void          *data,
                                                         uint32_t       flags,
                                                         FILE          *fdst ),
                                G3DEXPORTDATA *ged,
                                void          *data,
                                uint32_t       flags,
                                FILE          *fdst ) {
    /*** size. Side note : chunk size header is 8 bytes ***/
    uint32_t dummyChunkSize = 0x00;
    uint32_t chunkSize;
    uint32_t chunkSizeOffset;
    uint32_t chunkEndPos;
    uint32_t size = 0x00;

    size += g3dexport_fwrite ( &chunkSignature, sizeof ( uint32_t  ), 0x01, fdst );

    size += g3dexport_fwrite ( &dummyChunkSize, sizeof ( uint32_t  ), 0x01, fdst );

    /*** write our data ***/
    chunkSize = ( writeChunk ) ? writeChunk ( ged, data, flags, fdst ) : 0x00;

    chunkEndPos  = ftell ( fdst ); /*** memorize current position ***/
    chunkSizeOffset = ( chunkSize + sizeof ( uint32_t ) );

    fseek ( fdst, ( int32_t ) - chunkSizeOffset, SEEK_CUR );

    g3dexport_fwrite ( &chunkSize, sizeof ( uint32_t  ), 0x01, fdst );

    fseek ( fdst, chunkEndPos, SEEK_SET ); /*** continue ***/


    return ( size + chunkSize );
}
