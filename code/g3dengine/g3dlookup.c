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
#include <g3dengine/g3dengine.h>

/******************************************************************************/
void g3dlookup_freeTable ( G3DLOOKUP *lookup ) {
    if ( lookup->table ) {
        free ( lookup->table );
    }
}

/******************************************************************************/
void g3dlookup_add ( G3DLOOKUP *lookup, void *src, void *dst ) {
    lookup->table[lookup->rank][0x00] = src;
    lookup->table[lookup->rank][0x01] = dst;

    lookup->rank++;
}

/******************************************************************************/
void *g3dlookup_get ( G3DLOOKUP *lookup, void *src ) {
    uint32_t i;

    for ( i = 0x00; i < lookup->rank; i++ ) {
        if ( lookup->table[i][0x00] == src ) return lookup->table[i][0x01];
    }

    return src;
}

/******************************************************************************/
void g3dlookup_reset ( G3DLOOKUP *lookup ) {
    memset ( lookup->table, 0x00, lookup->size * sizeof ( void * ) * 0x02 );

    lookup->rank = 0x00;
}

/******************************************************************************/
void g3dlookup_init ( G3DLOOKUP *lookup ) {
    lookup->table = NULL;
    lookup->rank = 0x00;
}

/******************************************************************************/
void g3dlookup_realloc ( G3DLOOKUP *lookup, uint32_t size ) {
    lookup->table = realloc ( lookup->table, size * sizeof ( void * ) * 0x02 );
    lookup->size = size;
}

/******************************************************************************/
uint32_t g3dlookup_getSize ( G3DLOOKUP *lookup ) {
    return lookup->size;
}
