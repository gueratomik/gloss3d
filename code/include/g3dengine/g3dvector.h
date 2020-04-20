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
#ifndef _G3DVECTOR_H_
#define _G3DVECTOR_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct G3DVECTOR
 * @brief A structure to store 3D coordinates as 4 floats, with a w member.
 */
/******************************************************************************/
typedef struct _G3DVECTOR {
    float x;
    float y;
    float z;
    float w;
} G3DVECTOR, G3DPOINT, G3DQUATERNION;

/******************************************************************************/
/**
 * Init a 3D vector.
 * @param vec a pointer to a vector.
 * @param x the X coordinate.
 * @param y the Y coordinate.
 * @param y the Z coordinate.
 * @param y the W coordinate.
 * @param vout a pointer to a vector (result).
 */
void g3dvector_init ( G3DVECTOR *vec, float x, float y, float z, float w );

/**
 * Reflect a vector on a flat surface (the "nor" parameter is the
 * normal vector of that surface).
 * @param vec a pointer to a vector.
 * @param nor a pointer to the normal vector of the reflecting surface.
 * @param vout a pointer to a vector (result).
 */
uint32_t g3dvector_reflect ( G3DVECTOR *vec, G3DVECTOR *nor, G3DVECTOR *vout );

/**
 * Compute the mean value of two vectors.
 * @param v0 a pointer to a vector.
 * @param v1 a pointer to a vector.
 * @param vout a pointer to a vector (result).
 */
void g3dvector_average ( G3DVECTOR *v0, G3DVECTOR *v1, G3DVECTOR *vout );

/**
 * Return the cross product of two vectors.
 * @param v0 a pointer to a vector.
 * @param v1 a pointer to a vector.
 * @param vout a pointer to a vector (result).
 */
void g3dvector_cross ( G3DVECTOR *, G3DVECTOR *, G3DVECTOR * );

/**
 * Normalize a vector.
 * @param vec a pointer to a vector.
 * @param len the returned length of the vector. Can be NULL.
 */
void g3dvector_normalize ( G3DVECTOR *vec, float *len );

/**
 * Return the scalar product of two 2D vectors.
 * @param v0 a pointer to a vector.
 * @param v1 a pointer to a vector.
 * @return the scalar product.
 */
float g3dvector_scalar ( G3DVECTOR *v0, G3DVECTOR *v1 );

/**
 */
int g3dvector_sameside ( G3DVECTOR *, G3DVECTOR *, G3DVECTOR *, G3DVECTOR * );

/**
 * Allocate a 3D vector
 * @param x the X coordinate.
 * @param y the Y coordinate.
 * @param z the Z coordinate.
 * @param w the W coordinate.
 * @return a pointer to the allocated vector.
 */
G3DVECTOR *g3dvector_new ( float x, float y, float z, float w );

/**
 * Free a 3D vector previously allocated.
 * @param vec a pointer to a vector.
 */
void g3dvector_free ( void * );

/**
 * Multiply a vector by a matrix and store the result in vout.
 * @param vec a pointer to a vector.
 * @param matrix a pointer to a 4x4 matrix.
 * @param vout a pointer to a vector (result).
 */
void g3dvector_matrix ( G3DVECTOR *vec, double *matrix, G3DVECTOR *vout );

/**
 * Get the length of a vector.
 * @param vec a pointer to a vector.
 * @return the length of the vector.
 */
float g3dvector_length ( G3DVECTOR *vec );

/**
 * Get the angle between formed by vectors (in radians).
 * @param v0 a pointer to a vector.
 * @param v1 a pointer to a vector.
 */
float g3dvector_angle ( G3DVECTOR *v0, G3DVECTOR *v1 );


void g3dvector_createTranslationMatrix ( G3DVECTOR *vec, double *MVX );

#ifdef __cplusplus
}
#endif

#endif
