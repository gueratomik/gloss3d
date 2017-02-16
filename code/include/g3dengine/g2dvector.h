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
#ifndef _G2DVECTOR_H_
#define _G2DVECTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

typedef struct _G3DVECTOR G3DVECTOR;

/**
 * @struct G2DVECTOR
 * @brief A structure to store 2D coordinates (as signed integer).
 */
typedef struct _G2DVECTOR {
    int32_t x; /*!< x coordinate */
    int32_t y; /*!< y coordinate */
} G2DVECTOR, G2DPOINT;

/******************************************************************************/
/**
 * Return the scalar product of two 2D vectors.
 * @param v0 a pointer to a vector.
 * @param v1 a pointer to a vector.
 * @return the 2D scalar product.
 */
int32_t g2dvector_scalar ( G2DVECTOR *v0, G2DVECTOR *v1 );

/**
 * Return the cross product of two 2D vectors.
 * In 2D, the cross product is a 3D vector. This functions returns the
 * the value of the Z member and also stores the whole result in vout.
 * @param v0 a pointer to a vector.
 * @param v1 a pointer to a vector.
 * @param vout a pointer to a vector (result).
 * @return the value of the Z member.
 */
int32_t g2dvector_cross ( G2DVECTOR *v0, G2DVECTOR *v1, G3DVECTOR *vout );

/**
 * Compute the mean value of two 2D vectors.
 * @param v0 a pointer to a vector.
 * @param v1 a pointer to a vector.
 * @param vout a pointer to a vector (result).
 */
void g2dvector_average ( G2DVECTOR *v0, G2DVECTOR *v1, G2DVECTOR *vout );

/**
 * Init a 2D vector.
 * @param vec a pointer to a vector.
 * @param x the X coordinate.
 * @param y the Y coordinate.
 * @param vout a pointer to a vector (result).
 */
void g2dvector_init ( G2DVECTOR *vec, int32_t x, int32_t y );

/**
 * Get the length of a 2D vector.
 * @param vec a pointer to a vector.
 * @param the length of the 2D vector.
 */
uint32_t g2dvector_length ( G2DVECTOR *vec );

/**
 * Normalize a 2D vector.
 * @param vec a pointer to a vector.
 */
void g2dvector_normalize ( G2DVECTOR *vec );

/**
 * Free a 2D vector previously allocated.
 * @param vec a pointer to a vector.
 */
void g2dvector_free ( G2DVECTOR *vec );

/**
 * Allocate a 2D vector
 * @param x the X coordinate.
 * @param y the Y coordinate.
 * @return a pointer to the allocated vector.
 */
G2DVECTOR *g2dvector_new ( int32_t x, int32_t y );

#ifdef __cplusplus
}
#endif

#endif
