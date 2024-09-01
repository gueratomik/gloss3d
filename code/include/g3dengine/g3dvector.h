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


/******************************************************************************/

/**
 * @struct G3DVECTOR4F
 * @brief A structure to store 3D coordinates as 4 float, with a w member.
 */

typedef struct _G3DVECTOR4F {
    float x;
    float y;
    float z;
    float w;
} G3DVECTOR4F;

/******************************************************************************/

/**
 * @struct G3DVECTOR4F
 * @brief A structure to store 3D coordinates as 4 double, with a w member.
 */

typedef struct _G3DVECTOR4D {
    double x;
    double y;
    double z;
    double w;
} G3DVECTOR4D;

/******************************************************************************/

/**
 * @struct G3DVECTOR3F
 * @brief A structure to store 3D coordinates as 3 float.
 */

typedef struct _G3DVECTOR3F {
    float x;
    float y;
    float z;
} G3DVECTOR3F;

/******************************************************************************/

/**
 * @struct G3DVECTOR3F
 * @brief A structure to store 3D coordinates as 3 double.
 */

typedef struct _G3DVECTOR3D {
    double x;
    double y;
    double z;
} G3DVECTOR3D;

/******************************************************************************/
typedef struct _G3DQUATERNION {
    double yaw;
    double pitch;
    double roll;
    double w;
} G3DQUATERNION;

/******************************************************************************/

/**
 * Reflect a vector on a flat surface (the "nor" parameter is the
 * normal vector of that surface).
 * @param vec a pointer to a vector.
 * @param nor a pointer to the normal vector of the reflecting surface.
 * @param vout a pointer to a vector (result).
 */
uint32_t g3dvector3f_reflect ( G3DVECTOR3F *vec,
                               G3DVECTOR3F *nor,
                               G3DVECTOR3F *vout );

/******************************************************************************/


/**
 */
int g3dvector3f_sameside ( G3DVECTOR3F *,
                           G3DVECTOR3F *,
                           G3DVECTOR3F *,
                           G3DVECTOR3F * );

/******************************************************************************/

/** \def _VECTOR3X_CROSS_DEC
 * Return the cross product of two vectors.
 * @param v0 a pointer to a vector.
 * @param v1 a pointer to a vector.
 * @param vout a pointer to a vector (result).
 */

#define _VECTOR3X_CROSS_DEC(prefix,type) \
void prefix##_cross( type *vone,         \
                     type *vtwo,         \
                     type *vout );

_VECTOR3X_CROSS_DEC( g3dvector3f, G3DVECTOR3F )
_VECTOR3X_CROSS_DEC( g3dvector3d, G3DVECTOR3D )

/******************************************************************************/

/** \def _VECTOR3X_INIT_DEC
 * Init a 3D vector.
 * @param vec a pointer to a vector.
 * @param x the X coordinate.
 * @param y the Y coordinate.
 * @param y the Z coordinate.
 * @param vout a pointer to a vector (result).
 */

#define _VECTOR3X_INIT_DEC(prefix,type,iee754) \
void prefix##_init( type *vec,                 \
                    iee754 x,                  \
                    iee754 y,                  \
                    iee754 z );

_VECTOR3X_INIT_DEC( g3dvector3f, G3DVECTOR3F, float  )
_VECTOR3X_INIT_DEC( g3dvector3d, G3DVECTOR3D, double )

/** \def _VECTOR4X_INIT_DEC
 * Init a 3D vector.
 * @param vec a pointer to a vector.
 * @param x the X coordinate.
 * @param y the Y coordinate.
 * @param y the Z coordinate.
 * @param y the W coordinate.
 * @param vout a pointer to a vector (result).
 */

#define _VECTOR4X_INIT_DEC(prefix,type,iee754) \
void prefix##_init( type *vec,                 \
                    iee754 x,                  \
                    iee754 y,                  \
                    iee754 z,                  \
                    iee754 w );

_VECTOR4X_INIT_DEC( g3dvector4f, G3DVECTOR4F, float  )
_VECTOR4X_INIT_DEC( g3dvector4d, G3DVECTOR4D, double )

/******************************************************************************/

/** \def _VECTORXX_AVERAGE_DEC
 * Compute the mean value of two vectors.
 * @param v0 a pointer to a vector.
 * @param v1 a pointer to a vector.
 * @param vout a pointer to a vector (result).
 */

#define _VECTORXX_AVERAGE_DEC(prefix,type) \
void prefix##_average( type *vone,         \
                       type *vtwo,         \
                       type *vout );

_VECTORXX_AVERAGE_DEC( g3dvector3f, G3DVECTOR3F )
_VECTORXX_AVERAGE_DEC( g3dvector3d, G3DVECTOR3D )
_VECTORXX_AVERAGE_DEC( g3dvector4f, G3DVECTOR4F )
_VECTORXX_AVERAGE_DEC( g3dvector4d, G3DVECTOR4D )

/******************************************************************************/

/** \def _VECTOR3X_NORMALIZE_DEC
 * Normalize a vector.
 * @param vec a pointer to a vector.
 * @param len the returned length of the vector. Can be NULL.
 */

#define _VECTOR3X_NORMALIZE_DEC(prefix,type,ieee754)  \
ieee754 prefix##_normalize( type *vec );

_VECTOR3X_NORMALIZE_DEC( g3dvector3f, G3DVECTOR3F, float  )
_VECTOR3X_NORMALIZE_DEC( g3dvector3d, G3DVECTOR3D, double )

/** \def _VECTOR4X_NORMALIZE_DEC
 * Normalize a vector.
 * @param vec a pointer to a vector.
 * @param len the returned length of the vector. Can be NULL.
 */

#define _VECTOR4X_NORMALIZE_DEC(prefix,type,ieee754)  \
ieee754 prefix##_normalize( type *vec );

_VECTOR4X_NORMALIZE_DEC( g3dvector4f, G3DVECTOR4F, float  )
_VECTOR4X_NORMALIZE_DEC( g3dvector4d, G3DVECTOR4D, double )

/******************************************************************************/

/** \def _VECTOR3X_SCALAR_DEC
 * Return the scalar product of two 2D vectors.
 * @param v0 a pointer to a vector.
 * @param v1 a pointer to a vector.
 * @return the scalar product.
 */

#define _VECTOR3X_SCALAR_DEC(prefix,type,ieee754) \
ieee754 prefix##_scalar( type *v0,                \
                         type *v1 );

_VECTOR3X_SCALAR_DEC( g3dvector3f, G3DVECTOR3F, float  )
_VECTOR3X_SCALAR_DEC( g3dvector3d, G3DVECTOR3D, double )

/** \def _VECTOR4X_SCALAR_DEC
 * Return the scalar product of two 2D vectors.
 * @param v0 a pointer to a vector.
 * @param v1 a pointer to a vector.
 * @return the scalar product.
 */

#define _VECTOR4X_SCALAR_DEC(prefix,type,ieee754) \
ieee754 prefix##_scalar( type *v0,                \
                         type *v1 );

_VECTOR4X_SCALAR_DEC( g3dvector4f, G3DVECTOR4F, float  )
_VECTOR4X_SCALAR_DEC( g3dvector4d, G3DVECTOR4D, double )

/******************************************************************************/
#define _VECTOR3X_INVERT_DEC(prefix,type,ieee754) \
ieee754 prefix##_invert( type *v0 );

_VECTOR3X_INVERT_DEC( g3dvector3f, G3DVECTOR3F, float  )
_VECTOR3X_INVERT_DEC( g3dvector3d, G3DVECTOR3D, double )

#define _VECTOR4X_INVERT_DEC(prefix,type,ieee754) \
ieee754 prefix##_invert( type *v0 );

_VECTOR4X_INVERT_DEC( g3dvector4f, G3DVECTOR4F, float  )
_VECTOR4X_INVERT_DEC( g3dvector4d, G3DVECTOR4D, double )

/******************************************************************************/

/** \def _VECTOR3X_NEW_DEC
 * Allocate a 3D vector
 * @param x the X coordinate.
 * @param y the Y coordinate.
 * @param z the Z coordinate.
 * @param w the W coordinate.
 * @return a pointer to the allocated vector.
 */

#define _VECTOR3X_NEW_DEC(prefix,type,iee754) \
type *prefix##_new( iee754 x,                 \
                    iee754 y,                 \
                    iee754 z );

_VECTOR3X_NEW_DEC( g3dvector3f, G3DVECTOR3F, float  )
_VECTOR3X_NEW_DEC( g3dvector3d, G3DVECTOR3D, double )

/** \def _VECTOR4X_NEW_DEC
 * Allocate a 3D vector
 * @param x the X coordinate.
 * @param y the Y coordinate.
 * @param z the Z coordinate.
 * @param w the W coordinate.
 * @return a pointer to the allocated vector.
 */

#define _VECTOR4X_NEW_DEC(prefix,type,iee754) \
type *prefix##_new( iee754 x,                 \
                    iee754 y,                 \
                    iee754 z,                 \
                    iee754 w );

_VECTOR4X_NEW_DEC( g3dvector4f, G3DVECTOR4F, float  )
_VECTOR4X_NEW_DEC( g3dvector4d, G3DVECTOR4D, double )

/******************************************************************************/

/** \def _VECTORXX_FREE_DEC
 * Free a 3D vector previously allocated.
 * @param vec a pointer to a vector.
 */

#define _VECTORXX_FREE_DEC(prefix, type)  \
void prefix##_free( type *vec );

_VECTORXX_FREE_DEC( g3dvector3f, G3DVECTOR3F )
_VECTORXX_FREE_DEC( g3dvector3d, G3DVECTOR3D )
_VECTORXX_FREE_DEC( g3dvector4f, G3DVECTOR4F )
_VECTORXX_FREE_DEC( g3dvector4d, G3DVECTOR4D )

/******************************************************************************/

/** \def _VECTOR3X_MATRIX_DEC
 * Multiply a vector by a matrix and store the result in vout.
 * @param vec a pointer to a vector.
 * @param matrix a pointer to a 4x4 matrix.
 * @param vout a pointer to a vector (result).
 */

#define _VECTOR3X_MATRIX_DEC(prefix,type,ieee754) \
void prefix##_matrixf ( type    *vec,             \
                        ieee754 *matrix,          \
                        type    *vout );

_VECTOR3X_MATRIX_DEC( g3dvector3f, G3DVECTOR3F, float  )
_VECTOR3X_MATRIX_DEC( g3dvector3d, G3DVECTOR3D, double )

/** \def _VECTOR4X_MATRIX_DEC
 * Multiply a vector by a matrix and store the result in vout.
 * @param vec a pointer to a vector.
 * @param matrix a pointer to a 4x4 matrix.
 * @param vout a pointer to a vector (result).
 */

#define _VECTOR4X_MATRIX_DEC(prefix,type,ieee754) \
void prefix##_matrixf ( type    *vec,             \
                        ieee754 *matrix,          \
                        type    *vout );

_VECTOR4X_MATRIX_DEC( g3dvector4f, G3DVECTOR4F, float  )
_VECTOR4X_MATRIX_DEC( g3dvector4d, G3DVECTOR4D, double )

/******************************************************************************/

/** \def _VECTOR3X_LENGTH_DEC
 * Get the length of a vector.
 * @param vec a pointer to a vector.
 * @return the length of the vector.
 */

#define _VECTOR3X_LENGTH_DEC(prefix,type,ieee754) \
ieee754 prefix##_length( type *vec );

_VECTOR3X_LENGTH_DEC( g3dvector3f, G3DVECTOR3F, float  )
_VECTOR3X_LENGTH_DEC( g3dvector3d, G3DVECTOR3D, double )

/** \def _VECTOR4X_LENGTH_DEC
 * Get the length of a vector.
 * @param vec a pointer to a vector.
 * @return the length of the vector.
 */

#define _VECTOR4X_LENGTH_DEC(prefix,type,ieee754) \
ieee754 prefix##_length( type *vec );

_VECTOR4X_LENGTH_DEC( g3dvector4f, G3DVECTOR4F, float  )
_VECTOR4X_LENGTH_DEC( g3dvector4d, G3DVECTOR4D, double )

/******************************************************************************/

/** \def _VECTORXX_ANGLE_DEC
 * Get the angle between formed by vectors (in radians).
 * @param v0 a pointer to a vector.
 * @param v1 a pointer to a vector.
 */

#define _VECTORXX_ANGLE_DEC(prefix,type,ieee754) \
ieee754 prefix##_angle( type *v0,                \
                        type *v1 );

_VECTORXX_ANGLE_DEC( g3dvector3f, G3DVECTOR3F, float  )
_VECTORXX_ANGLE_DEC( g3dvector3d, G3DVECTOR3D, double )
_VECTORXX_ANGLE_DEC( g3dvector4f, G3DVECTOR4F, float  )
_VECTORXX_ANGLE_DEC( g3dvector4d, G3DVECTOR4D, double )

/******************************************************************************/

#ifdef __cplusplus
}
#endif

#endif
