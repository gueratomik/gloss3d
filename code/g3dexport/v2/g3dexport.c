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
/*** Write to file or, if stream is NULL, return the size to write. ***/
/*** this is useful to compute the chunk's future size. All file writing op ***/
/*** MUST call this function. ***/
uint32_t g3dexport_fwrite ( void   *ptr,
                            size_t  size,
                            size_t  count,
                            FILE   *stream ) {
    if ( stream == NULL ) {
        return ( size * count );
    }

    return fwrite ( ptr, size, count, stream );
}

/******************************************************************************/
uint32_t g3dexport_writeChunk ( uint32_t   chunkSignature,
                                uint32_t (*writeChunk) ( void    *data,
                                                         uint32_t flags,
                                                         FILE    *fdst ),
                                void      *data,
                                uint32_t  flags,
                                FILE      *fdst ) {
    /*** First call the func with a NULL argument to get the nested chunks ***/
    /*** size. Side note : chunk size header is 8 bytes ***/
    uint32_t chunkSize = writeChunk ( data, flags, NULL );
    uint32_t size = 0x00;

    size += g3dexport_fwrite ( &chunkSignature, sizeof ( uint32_t  ), 0x01, fdst );
    size += g3dexport_fwrite ( &chunkSize     , sizeof ( uint32_t  ), 0x01, fdst );

    /*** Actually write on the disk ***/
    if ( fdst ) {
        writeChunk ( data, flags, fdst );
    }

    return ( size + chunkSize );
}
