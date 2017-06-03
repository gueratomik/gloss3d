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

/**
 * @file
 */

/******************************************************************************/
#ifndef _G3DLOOKUP_H_
#define _G3DLOOKUP_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct G3DLOOKUP
 * @brief A structure to store a lookup table.
 */
typedef struct _G3DLOOKUP {
    void *(*table)[0x02];
    uint32_t rank;
    uint32_t size;
} G3DLOOKUP;

/******************************************************************************/
/**
 * Free the memory used by the lookup table.
 * @param a pointer to the G3DLOOKUP structure.
 */
void g3dlookup_freeTable ( G3DLOOKUP *lookup );

/**
 * Add a record to the  lookup table.
 * @param lookup a pointer to the G3DLOOKUP structure.
 * @param src a pointer to a source object.
 * @param dst a pointer to a destination object.
 */
void g3dlookup_add ( G3DLOOKUP *lookup, 
                     void      *src, 
                     void      *dst );

/**
 * Get the destination record to the source record.
 * @param lookup a pointer to the G3DLOOKUP structure.
 * @param src a pointer to a source object.
 */
void *g3dlookup_get ( G3DLOOKUP *lookup, 
                      void      *src );

/**
 * Reset the lookup table (fill with 0). The size of the table stays unchanged.
 * @param lookup a pointer to the G3DLOOKUP structure.
 */
void g3dlookup_reset ( G3DLOOKUP *lookup );

/**
 * Realloc memory for the lookup table.
 * @param lookup a pointer to the G3DLOOKUP structure.
 * @param size the desired number of record couples.
 */
void g3dlookup_realloc ( G3DLOOKUP *lookup, 
                         uint32_t   size );

/**
 * Get the size of the lookup table (number of src/dst couples).
 * @param lookup a pointer to the G3DLOOKUP structure.
 * @return the size of the lookup table (number of src/dst couples).
 */
uint32_t g3dlookup_getSize ( G3DLOOKUP *lookup );

void g3dlookup_init ( G3DLOOKUP *lookup );

#ifdef __cplusplus
}
#endif

#endif
