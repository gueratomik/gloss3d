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
uint32_t g3dvector3f_reflect ( G3DVECTOR3F *vec,
                               G3DVECTOR3F *nor,
                               G3DVECTOR3F *vout ) {
    float dot = g3dvector3f_scalar ( vec, nor );

    if ( dot > 0.0f ) {
        return 0x00;
    } else {
        float dotby2 = dot * 2.0f;

        memset ( vout, 0x00, sizeof ( G3DVECTOR3F ) );

        /*** reflection formula ***/
        vout->x = vec->x - ( dotby2 * nor->x );
        vout->y = vec->y - ( dotby2 * nor->y );
        vout->z = vec->z - ( dotby2 * nor->z );
    }

    return 0x01;
}

/******************************************************************************/
int g3dvector3f_sameside ( G3DVECTOR3F *v0,
                           G3DVECTOR3F *v1,
                           G3DVECTOR3F *v2,
                           G3DVECTOR3F *vp ) {
    G3DVECTOR3F v0v1 = { v1->x - v0->x,
                         v1->y - v0->y,
                         v1->z - v0->z },
                v0v2 = { v2->x - v0->x,
                         v2->y - v0->y,
                         v2->z - v0->z },
                v0vp = { vp->x - v0->x,
                         vp->y - v0->y,
                         vp->z - v0->z },
                vo, vf;

    g3dvector3f_normalize ( &v0v1 );
    g3dvector3f_normalize ( &v0v2 );
    g3dvector3f_normalize ( &v0vp );

    g3dvector3f_cross  ( &v0v1, &v0vp, &vo );
    g3dvector3f_cross  ( &v0v1, &v0v2, &vf );

    if ( g3dvector3f_scalar ( &vo, &vf ) >= 0.0f ) {

        return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
#define _VECTOR3X_MATRIX_DEF(prefix,type,ieee754) \
void prefix##_matrixf ( type    *vec,             \
                        ieee754 *matrix,          \
                        type    *vout ) {         \
    type tmp;                                     \
                                                  \
    tmp.x = ( ( vec->x * matrix[0x00] ) +         \
              ( vec->y * matrix[0x04] ) +         \
              ( vec->z * matrix[0x08] ) +         \
              (          matrix[0x0C] ) );        \
                                                  \
    tmp.y = ( ( vec->x * matrix[0x01] ) +         \
              ( vec->y * matrix[0x05] ) +         \
              ( vec->z * matrix[0x09] ) +         \
              (          matrix[0x0D] ) );        \
                                                  \
    tmp.z = ( ( vec->x * matrix[0x02] ) +         \
              ( vec->y * matrix[0x06] ) +         \
              ( vec->z * matrix[0x0A] ) +         \
              (          matrix[0x0E] ) );        \
                                                  \
    memcpy( vout, &tmp, sizeof ( tmp ) );         \
}

_VECTOR3X_MATRIX_DEF( g3dvector3f, G3DVECTOR3F, float  )
_VECTOR3X_MATRIX_DEF( g3dvector3d, G3DVECTOR3D, double )

#define _VECTOR4X_MATRIX_DEF(prefix,type,ieee754) \
void prefix##_matrixf ( type    *vec,             \
                        ieee754 *matrix,          \
                        type    *vout ) {         \
    type tmp;                                     \
                                                  \
    tmp.x = ( ( vec->x * matrix[0x00] ) +         \
              ( vec->y * matrix[0x04] ) +         \
              ( vec->z * matrix[0x08] ) +         \
              ( vec->w * matrix[0x0C] ) );        \
                                                  \
    tmp.y = ( ( vec->x * matrix[0x01] ) +         \
              ( vec->y * matrix[0x05] ) +         \
              ( vec->z * matrix[0x09] ) +         \
              ( vec->w * matrix[0x0D] ) );        \
                                                  \
    tmp.z = ( ( vec->x * matrix[0x02] ) +         \
              ( vec->y * matrix[0x06] ) +         \
              ( vec->z * matrix[0x0A] ) +         \
              ( vec->w * matrix[0x0E] ) );        \
                                                  \
    tmp.w = ( ( vec->x * matrix[0x03] ) +         \
              ( vec->y * matrix[0x07] ) +         \
              ( vec->z * matrix[0x0B] ) +         \
              ( vec->w * matrix[0x0F] ) );        \
                                                  \
    memcpy( vout, &tmp, sizeof ( tmp ) );         \
}

_VECTOR4X_MATRIX_DEF( g3dvector4f, G3DVECTOR4F, float  )
_VECTOR4X_MATRIX_DEF( g3dvector4d, G3DVECTOR4D, double )

/******************************************************************************/
#define _VECTOR3X_PRINT_DEF(prefix,type)             \
void prefix##_print( type *vec ) {                   \
    printf ( "%f %f %f\n", vec->x, vec->y, vec->z ); \
}

_VECTOR3X_PRINT_DEF( g3dvector3f, G3DVECTOR3F );
_VECTOR3X_PRINT_DEF( g3dvector3d, G3DVECTOR3D );

#define _VECTOR4X_PRINT_DEF(prefix,type)                        \
void prefix##_print( type *vec ) {                              \
    printf ( "%f %f %f %f\n", vec->x, vec->y, vec->z, vec->w ); \
}

_VECTOR4X_PRINT_DEF( g3dvector4f, G3DVECTOR4F );
_VECTOR4X_PRINT_DEF( g3dvector4d, G3DVECTOR4D );

/******************************************************************************/
#define _VECTOR3X_INVERT_DEF(prefix,type,ieee754) \
ieee754 prefix##_invert( type *vec ) {            \
    vec->x = - vec->x;                            \
    vec->y = - vec->y;                            \
    vec->z = - vec->z;                            \
}

_VECTOR3X_INVERT_DEF( g3dvector3f, G3DVECTOR3F, float  )
_VECTOR3X_INVERT_DEF( g3dvector3d, G3DVECTOR3D, double )

#define _VECTOR4X_INVERT_DEF(prefix,type,ieee754) \
ieee754 prefix##_invert( type *vec ) {            \
    vec->x = - vec->x;                            \
    vec->y = - vec->y;                            \
    vec->z = - vec->z;                            \
    vec->w = - vec->w;                            \
}

_VECTOR4X_INVERT_DEF( g3dvector4f, G3DVECTOR4F, float  )
_VECTOR4X_INVERT_DEF( g3dvector4d, G3DVECTOR4D, double )

/******************************************************************************/
#define _VECTOR3X_SCALAR_DEF(prefix,type,ieee754) \
ieee754 prefix##_scalar( type *v0,                \
                         type *v1 ) {             \
    return ( ( v0->x * v1->x ) +                  \
             ( v0->y * v1->y ) +                  \
             ( v0->z * v1->z ) );                 \
}

_VECTOR3X_SCALAR_DEF( g3dvector3f, G3DVECTOR3F, float  )
_VECTOR3X_SCALAR_DEF( g3dvector3d, G3DVECTOR3D, double )

#define _VECTOR4X_SCALAR_DEF(prefix,type,ieee754) \
ieee754 prefix##_scalar( type *v0,                \
                         type *v1 ) {             \
    return ( ( v0->x * v1->x ) +                  \
             ( v0->y * v1->y ) +                  \
             ( v0->z * v1->z ) +                  \
             ( v0->w * v1->w ) );                 \
}

_VECTOR4X_SCALAR_DEF( g3dvector4f, G3DVECTOR4F, float  )
_VECTOR4X_SCALAR_DEF( g3dvector4d, G3DVECTOR4D, double )

/******************************************************************************/
#define _VECTORXX_AVERAGE_DEF(prefix,type) \
void prefix##_average( type *vone,         \
                       type *vtwo,         \
                       type *vout ) {      \
    type tmp;                              \
                                           \
    tmp.x = ( vone->x + vtwo->x ) / 2.0f;  \
    tmp.y = ( vone->y + vtwo->y ) / 2.0f;  \
    tmp.z = ( vone->z + vtwo->z ) / 2.0f;  \
                                           \
    memcpy( vout, &tmp, sizeof ( tmp ) );  \
}

_VECTORXX_AVERAGE_DEF( g3dvector3f, G3DVECTOR3F )
_VECTORXX_AVERAGE_DEF( g3dvector3d, G3DVECTOR3D )
_VECTORXX_AVERAGE_DEF( g3dvector4f, G3DVECTOR4F )
_VECTORXX_AVERAGE_DEF( g3dvector4d, G3DVECTOR4D )

/******************************************************************************/
#define _VECTOR3X_CROSS_DEF(prefix,type)                   \
void prefix##_cross( type *vone,                           \
                     type *vtwo,                           \
                     type *vout ) {                        \
    type tmp;                                              \
                                                           \
    tmp.x = ( vone->y * vtwo->z ) - ( vone->z * vtwo->y ); \
    tmp.y = ( vone->z * vtwo->x ) - ( vone->x * vtwo->z ); \
    tmp.z = ( vone->x * vtwo->y ) - ( vone->y * vtwo->x ); \
                                                           \
    memcpy( vout, &tmp, sizeof ( tmp ) );                  \
}

_VECTOR3X_CROSS_DEF( g3dvector3f, G3DVECTOR3F )
_VECTOR3X_CROSS_DEF( g3dvector3d, G3DVECTOR3D )

/******************************************************************************/
#define _VECTOR3X_INIT_DEF(prefix,type,ieee754) \
void prefix##_init( type *vec,                  \
                    ieee754 x,                  \
                    ieee754 y,                  \
                    ieee754 z ) {               \
    vec->x = x;                                 \
    vec->y = y;                                 \
    vec->z = z;                                 \
}

_VECTOR3X_INIT_DEF( g3dvector3f, G3DVECTOR3F, float  )
_VECTOR3X_INIT_DEF( g3dvector3d, G3DVECTOR3D, double )

#define _VECTOR4X_INIT_DEF(prefix,type,ieee754) \
void prefix##_init( type *vec,                  \
                    ieee754 x,                  \
                    ieee754 y,                  \
                    ieee754 z,                  \
                    ieee754 w ) {               \
    vec->x = x;                                 \
    vec->y = y;                                 \
    vec->z = z;                                 \
    vec->w = w;                                 \
}

_VECTOR4X_INIT_DEF( g3dvector4f, G3DVECTOR4F, float  )
_VECTOR4X_INIT_DEF( g3dvector4d, G3DVECTOR4D, double )

/******************************************************************************/
#define _VECTOR3X_LENGTH_DEF(prefix,type,ieee754) \
ieee754 prefix##_length( type *vec ) {            \
    ieee754 len = ( vec->x * vec->x ) +           \
                 ( vec->y * vec->y ) +            \
                 ( vec->z * vec->z );             \
                                                  \
    return sqrt ( len );                          \
}

_VECTOR3X_LENGTH_DEF( g3dvector3f, G3DVECTOR3F, float  )
_VECTOR3X_LENGTH_DEF( g3dvector3d, G3DVECTOR3D, double )

#define _VECTOR4X_LENGTH_DEF(prefix,type,ieee754) \
ieee754 prefix##_length( type *vec ) {            \
    ieee754 len = ( vec->x * vec->x ) +           \
                 ( vec->y * vec->y ) +            \
                 ( vec->z * vec->z ) +            \
                 ( vec->w * vec->w );             \
                                                  \
    return sqrt ( len );                          \
}

_VECTOR4X_LENGTH_DEF( g3dvector4f, G3DVECTOR4F, float  )
_VECTOR4X_LENGTH_DEF( g3dvector4d, G3DVECTOR4D, double )

/******************************************************************************/
#define _VECTOR3X_NORMALIZE_DEF(prefix,type,ieee754) \
ieee754 prefix##_normalize( type *vec ) {            \
    ieee754 len = prefix##_length ( vec );           \
                                                     \
    if ( len ) {                                     \
        vec->x = vec->x / len;                       \
        vec->y = vec->y / len;                       \
        vec->z = vec->z / len;                       \
    }                                                \
                                                     \
    return len;                                      \
}

_VECTOR3X_NORMALIZE_DEF( g3dvector3f, G3DVECTOR3F, float  )
_VECTOR3X_NORMALIZE_DEF( g3dvector3d, G3DVECTOR3D, double )

#define _VECTOR4X_NORMALIZE_DEF(prefix,type,ieee754) \
ieee754 prefix##_normalize( type *vec ) {            \
    ieee754 len = prefix##_length ( vec );           \
                                                     \
    if ( len ) {                                     \
        vec->x = vec->x / len;                       \
        vec->y = vec->y / len;                       \
        vec->z = vec->z / len;                       \
        vec->w = vec->w / len;                       \
    }                                                \
                                                     \
    return len;                                      \
}

_VECTOR4X_NORMALIZE_DEF( g3dvector4f, G3DVECTOR4F, float  )
_VECTOR4X_NORMALIZE_DEF( g3dvector4d, G3DVECTOR4D, double )

/******************************************************************************/
#define _VECTORXX_ANGLE_DEF(prefix,type,ieee754) \
ieee754 prefix##_angle( type *v0,                \
                        type *v1 ) {             \
    ieee754 dot = prefix##_scalar ( v0, v1 );    \
                                                 \
    /** handle rounding errors */                \
    if( dot < -1.0f ) dot = -1.0f;               \
    if( dot >  1.0f ) dot =  1.0f;               \
                                                 \
    return  acos ( dot );                        \
}

_VECTORXX_ANGLE_DEF( g3dvector3f, G3DVECTOR3F, float  )
_VECTORXX_ANGLE_DEF( g3dvector3d, G3DVECTOR3D, double )
_VECTORXX_ANGLE_DEF( g3dvector4f, G3DVECTOR4F, float  )
_VECTORXX_ANGLE_DEF( g3dvector4d, G3DVECTOR4D, double )

/******************************************************************************/
#define _VECTORXX_FREE_DEF(prefix,type)  \
void prefix##_free( type *vec ) {   \
    free ( vec );                   \
}

_VECTORXX_FREE_DEF( g3dvector3f, G3DVECTOR3F )
_VECTORXX_FREE_DEF( g3dvector3d, G3DVECTOR3D )
_VECTORXX_FREE_DEF( g3dvector4f, G3DVECTOR4F )
_VECTORXX_FREE_DEF( g3dvector4d, G3DVECTOR4D )

/******************************************************************************/
#define _VECTOR3X_NEW_DEF(prefix,type,ieee754)               \
type *prefix##_new( ieee754 x,                               \
                    ieee754 y,                               \
                    ieee754 z ) {                            \
    type *vec = ( type * ) calloc ( 0x01, sizeof ( type ) ); \
                                                             \
    if ( vec == NULL ) {                                     \
        return NULL;                                         \
    }                                                        \
                                                             \
    prefix##_init ( vec, x, y, z );                          \
                                                             \
                                                             \
    return vec;                                              \
}

_VECTOR3X_NEW_DEF( g3dvector3f, G3DVECTOR3F, float  )
_VECTOR3X_NEW_DEF( g3dvector3d, G3DVECTOR3D, double )

#define _VECTOR4X_NEW_DEF(prefix,type,ieee754)               \
type *prefix##_new( ieee754 x,                               \
                    ieee754 y,                               \
                    ieee754 z,                               \
                    ieee754 w ) {                            \
    type *vec = ( type * ) calloc ( 0x01, sizeof ( type ) ); \
                                                             \
    if ( vec == NULL ) {                                     \
        return NULL;                                         \
    }                                                        \
                                                             \
    prefix##_init ( vec, x, y, z, w );                       \
                                                             \
                                                             \
    return vec;                                              \
}

_VECTOR4X_NEW_DEF( g3dvector4f, G3DVECTOR4F, float  )
_VECTOR4X_NEW_DEF( g3dvector4d, G3DVECTOR4D, double )
