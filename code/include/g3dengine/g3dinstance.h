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
#ifndef _G3DINSTANCE_H_
#define _G3DINSTANCE_H_

#ifdef __cplusplus
extern "C" {
#endif


/**
 * @struct G3DINSTANCE
 * @brief .
 */
typedef struct _G3DINSTANCE {
    G3DOBJECT  obj;
    G3DOBJECT *ref;
    double     smatrix[0x10];
    uint32_t   orientation;
    G3DSCENE  *sce;
} G3DINSTANCE;

/******************************************************************************/
G3DINSTANCE *g3dinstance_new ( uint32_t  id, 
                               char     *name,
                               G3DSCENE *sce );

void g3dinstance_setMirrored    ( G3DINSTANCE *ins );
void g3dinstance_unsetMirrored  ( G3DINSTANCE *ins );
void g3dinstance_setOrientation ( G3DINSTANCE *ins, 
                                  uint32_t     orientation );
void g3dinstance_setReference   ( G3DINSTANCE *ins, 
                                  G3DOBJECT   *ref );

#ifdef __cplusplus
}
#endif

#endif
