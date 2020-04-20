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
#include <g3dimport.h>

/******************************************************************************/
G3DIMPORTEXTENSION *g3dimportextension_new ( char      *name,
                                             void     (*readBlock)(void     *data,
                                                                   G3DSCENE *sce, 
                                                                   FILE     *fsrc),
                                             void      *data) {
    uint32_t size = sizeof ( G3DIMPORTEXTENSION );
    G3DIMPORTEXTENSION *ext = ( G3DIMPORTEXTENSION * ) calloc ( 0x01, size );

    if ( ext == NULL ) {
        fprintf ( stderr, "g3dimportextension_new: calloc failed\n");

        return NULL;
    }

    ext->name = strdup ( name );
    ext->readBlock = readBlock;
    ext->data = data;

    return ext;
}

/******************************************************************************/
void g3dimportextension_free ( G3DIMPORTEXTENSION *ext ) {
    free ( ext );
}

/******************************************************************************/
G3DIMPORTEXTENSION *g3dimportextension_getFromList ( LIST *lext,
                                                     char *name ) {
    LIST *ltmpext = lext;

    while ( ltmpext ) {
        G3DIMPORTEXTENSION *ext = ( G3DIMPORTEXTENSION * ) ltmpext->data;

        if ( strcmp ( ext->name, name ) == 0x00 ) {
            return ext;
        }

        ltmpext = ltmpext->next;
    }

    return NULL;
}
