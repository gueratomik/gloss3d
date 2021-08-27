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
uint32_t g3dimportv3_freadd ( double *d, FILE *stream ) {
    return g3dimportv3_fread ( d, sizeof ( double ), 0x01, stream );
}

/******************************************************************************/
uint32_t g3dimportv3_freadf ( float *f, FILE *stream ) {
    return g3dimportv3_fread ( f, sizeof ( float ), 0x01, stream );
}

/******************************************************************************/
uint32_t g3dimportv3_freadl ( uint32_t *l, FILE *stream ) {
    return g3dimportv3_fread ( l, sizeof ( uint32_t ), 0x01, stream );
}

/******************************************************************************/
uint32_t g3dimportv3_freadll ( uint64_t *ll, FILE *stream ) {
    return g3dimportv3_fread ( ll, sizeof ( uint64_t ), 0x01, stream );
}

/******************************************************************************/
/*** Write to file or, if stream is NULL, return the size to write. ***/
/*** this is useful to compute the chunk's future size. All file writing op ***/
/*** MUST call this function. ***/
uint32_t g3dimportv3_fread ( void   *ptr,
                           size_t  size,
                           size_t  count,
                           FILE   *stream ) {
    return fread ( ptr, size, count, stream );
}

/******************************************************************************/
void g3dimportv3data_incrementIndentLevel ( G3DIMPORTV3DATA *gid ) {
    gid->indentLevel += 4;
}

/******************************************************************************/
void g3dimportv3data_decrementIndentLevel ( G3DIMPORTV3DATA *gid ) {
    gid->indentLevel -= 4;
}
