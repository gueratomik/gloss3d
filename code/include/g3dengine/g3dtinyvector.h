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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2013         */
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
#ifndef _G3DTINYVECTOR_H_
#define _G3DTINYVECTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct G3DTINYVECTOR
 * @brief A structure to store 3D coordinates as 3 floats, without the w member.
 */
/******************************************************************************/
typedef struct _G3DTINYVECTOR {
    float x;
    float y;
    float z;
} G3DTINYVECTOR;

/******************************************************************************/
/**
 * Multiply a vector by a matrix and store the result in vout.
 * @param vec a pointer to a vector.
 * @param matrix a pointer to a 4x4 matrix.
 * @param vout a pointer to a vector (result).
 */
void g3dtinyvector_matrix ( G3DTINYVECTOR *vec, 
                            double        *matrix, 
                            G3DTINYVECTOR *vout );

/**
 * Normalize a vector.
 * @param vec a pointer to a vector.
 * @param len the returned length of the vector. Can be NULL.
 */
void g3dtinyvector_normalize ( G3DTINYVECTOR *vec, float *len );

/**
 * Get the length of a vector.
 * @param vec a pointer to a vector.
 * @return the length of the vector.
 */
float g3dtinyvector_length ( G3DTINYVECTOR *vec );


#ifdef __cplusplus
}
#endif

#endif
