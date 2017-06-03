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
#include <g3dexport.h>

/******************************************************************************/
G3DEXPORTEXTENSION *g3dexportextension_new ( char      *name,
                                             uint32_t (*blockSize)(void *data),
                                             void     (*writeBlock)(void *data, 
                                                                    FILE *fdst),
                                             void      *data) {
    uint32_t size = sizeof ( G3DEXPORTEXTENSION );
    G3DEXPORTEXTENSION *ext = ( G3DEXPORTEXTENSION * ) calloc ( 0x01, size );

    if ( ext == NULL ) {
        fprintf ( stderr, "g3dexportextension_new: calloc failed\n");

        return NULL;
    }

    ext->name = strdup ( name );
    ext->blockSize = blockSize;
    ext->writeBlock = writeBlock;
    ext->data = data;

    return ext;
}

/******************************************************************************/
void g3dexportextension_free ( G3DEXPORTEXTENSION *ext ) {
    free ( ext );
}

