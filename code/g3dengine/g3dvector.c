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

void g3dvector_print ( G3DVECTOR *vec ) {
    printf ( "%f %f %f\n", vec->x, vec->y, vec->z );
}

void g3ddoublevector_print ( G3DDOUBLEVECTOR *vec ) {
    printf ( "%f %f %f\n", vec->x, vec->y, vec->z );
}

/******************************************************************************/
uint32_t g3dvector_reflect ( G3DVECTOR *vec, G3DVECTOR *nor, G3DVECTOR *vout ) {
    float dot = g3dvector_scalar ( vec, nor );

    if ( dot > 0.0f ) {
        return 0x00;
    } else {
        float dotby2 = dot * 2.0f;

        memset ( vout, 0x00, sizeof ( G3DVECTOR ) );

        /*** reflection formula ***/
        vout->x = vec->x - ( dotby2 * nor->x );
        vout->y = vec->y - ( dotby2 * nor->y );
        vout->z = vec->z - ( dotby2 * nor->z );
    }

    return 0x01;
}

/******************************************************************************/
void g3ddoublevector_matrix ( G3DDOUBLEVECTOR *vec, double *matrix,
                                                    G3DDOUBLEVECTOR *vout ) {
    vout->x = ( ( vec->x * matrix[0x00] ) + 
                ( vec->y * matrix[0x04] ) + 
                ( vec->z * matrix[0x08] ) + 
                (          matrix[0x0C] ) );

    vout->y = ( ( vec->x * matrix[0x01] ) + 
                ( vec->y * matrix[0x05] ) + 
                ( vec->z * matrix[0x09] ) + 
                (          matrix[0x0D] ) );

    vout->z = ( ( vec->x * matrix[0x02] ) + 
                ( vec->y * matrix[0x06] ) + 
                ( vec->z * matrix[0x0A] ) + 
                (          matrix[0x0E] ) );
}

/******************************************************************************/
double g3ddoublevector_length ( G3DDOUBLEVECTOR *vec ) {
    double len = ( vec->x * vec->x ) +
                 ( vec->y * vec->y ) +
                 ( vec->z * vec->z );

    return sqrt ( len );
}

/******************************************************************************/
void g3ddoublevector_normalize ( G3DDOUBLEVECTOR *vec, float *len ) {
    float LEN = g3ddoublevector_length ( vec );

    if ( len ) (*len) = LEN;

    if ( LEN ) {
        vec->x = vec->x / LEN;
        vec->y = vec->y / LEN;
        vec->z = vec->z / LEN;
        vec->w = 1.0f;
    }
}

/******************************************************************************/
void g3ddoublevector_invert ( G3DDOUBLEVECTOR *vec ) {
    vec->x = - vec->x;
    vec->y = - vec->y;
    vec->z = - vec->z;
}

/******************************************************************************/
double g3ddoublevector_scalar ( G3DDOUBLEVECTOR *v0, G3DDOUBLEVECTOR *v1 ) {
    return ( ( v0->x * v1->x ) + ( v0->y * v1->y ) + ( v0->z * v1->z ) );
}

/******************************************************************************/
void g3ddoublevector_cross ( G3DDOUBLEVECTOR *vone, G3DDOUBLEVECTOR *vtwo, 
                                                    G3DDOUBLEVECTOR *vout ) {
    vout->x = ( vone->y * vtwo->z ) - ( vone->z * vtwo->y );
    vout->y = ( vone->z * vtwo->x ) - ( vone->x * vtwo->z );
    vout->z = ( vone->x * vtwo->y ) - ( vone->y * vtwo->x );
    vout->w = 1.0f;
}

/******************************************************************************/
float g3dvector_scalar ( G3DVECTOR *v0, G3DVECTOR *v1 ) {
    return ( ( v0->x * v1->x ) + ( v0->y * v1->y ) + ( v0->z * v1->z ) );
}

/******************************************************************************/
/** Multiply vector "vec" by a 4x4 "matrix" and returns the result in "vout" **/
/******************************************************************************/


/******************************************************************************/
float g3dtinyvector_length ( G3DTINYVECTOR *vec ) {
    float len = sqrt ( ( vec->x * vec->x ) +
                       ( vec->y * vec->y ) +
                       ( vec->z * vec->z ) );
    return len;
}

/******************************************************************************/
void g3dtinyvector_normalize ( G3DTINYVECTOR *vec, float *len ) {
    float LEN = g3dtinyvector_length ( vec );

    if ( len ) (*len) = LEN;

    if ( LEN /*( len > NORMMAX ) || ( len < NORMMIN )*/ ) {
        vec->x = vec->x / LEN;
        vec->y = vec->y / LEN;
        vec->z = vec->z / LEN;
    }
}

/******************************************************************************/
void g3dtinyvector_matrix ( G3DTINYVECTOR *vec, double *matrix,
                                                G3DTINYVECTOR *vout ) {
    vout->x = ( ( vec->x * matrix[0x00] ) + 
                ( vec->y * matrix[0x04] ) + 
                ( vec->z * matrix[0x08] ) + 
                (          matrix[0x0C] ) );

    vout->y = ( ( vec->x * matrix[0x01] ) + 
                ( vec->y * matrix[0x05] ) + 
                ( vec->z * matrix[0x09] ) + 
                (          matrix[0x0D] ) );

    vout->z = ( ( vec->x * matrix[0x02] ) + 
                ( vec->y * matrix[0x06] ) + 
                ( vec->z * matrix[0x0A] ) + 
                (          matrix[0x0E] ) );
}

/******************************************************************************/
void g3dvector_matrix3 ( G3DVECTOR *vec, double *matrix, G3DVECTOR *vout) {
    vout->x = ( ( vec->x * matrix[0x00] ) + 
                ( vec->y * matrix[0x03] ) + 
                ( vec->z * matrix[0x06] ) );

    vout->y = ( ( vec->x * matrix[0x01] ) + 
                ( vec->y * matrix[0x04] ) + 
                ( vec->z * matrix[0x07] ) );

    vout->z = ( ( vec->x * matrix[0x02] ) + 
                ( vec->y * matrix[0x05] ) + 
                ( vec->z * matrix[0x08] ) );

    vout->w = 1.0f;
}

/******************************************************************************/
void g3dvector_matrix ( G3DVECTOR *vec, double *matrix, G3DVECTOR *vout) {
    vout->x = ( ( vec->x * matrix[0x00] ) + 
                ( vec->y * matrix[0x04] ) + 
                ( vec->z * matrix[0x08] ) + 
                ( /*vec->w * */matrix[0x0C] ) );

    vout->y = ( ( vec->x * matrix[0x01] ) + 
                ( vec->y * matrix[0x05] ) + 
                ( vec->z * matrix[0x09] ) + 
                ( /*vec->w * */matrix[0x0D] ) );

    vout->z = ( ( vec->x * matrix[0x02] ) + 
                ( vec->y * matrix[0x06] ) + 
                ( vec->z * matrix[0x0A] ) + 
                ( /*vec->w * */matrix[0x0E] ) );

    vout->w = ( ( vec->x * matrix[0x03] ) + 
                ( vec->y * matrix[0x07] ) + 
                ( vec->z * matrix[0x0B] ) + 
                ( /*vec->w * */matrix[0x0F] ) );
}

/******************************************************************************/
int g3dvector_sameside ( G3DVECTOR *v0, G3DVECTOR *v1,
                         G3DVECTOR *v2, G3DVECTOR *vp ) {
    G3DVECTOR v0v1 = { v1->x - v0->x,
                       v1->y - v0->y,
                       v1->z - v0->z },
              v0v2 = { v2->x - v0->x,
                       v2->y - v0->y,
                       v2->z - v0->z },
              v0vp = { vp->x - v0->x,
                       vp->y - v0->y,
                       vp->z - v0->z },
              vo, vf;

    g3dvector_normalize ( &v0v1, NULL );
    g3dvector_normalize ( &v0v2, NULL );
    g3dvector_normalize ( &v0vp, NULL );

    g3dvector_cross  ( &v0v1, &v0vp, &vo );
    g3dvector_cross  ( &v0v1, &v0v2, &vf );

    if ( g3dvector_scalar ( &vo, &vf ) >= 0.0f ) {

        return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
void g3dvector_average ( G3DVECTOR *vone, G3DVECTOR *vtwo, G3DVECTOR *vout ) {
    vout->x = ( vone->x + vtwo->x ) / 2.0f;
    vout->y = ( vone->y + vtwo->y ) / 2.0f;
    vout->z = ( vone->z + vtwo->z ) / 2.0f;
    vout->w = ( vone->w + vtwo->w ) / 2.0f;
}

/******************************************************************************/
void g3dvector_cross ( G3DVECTOR *vone, G3DVECTOR *vtwo, G3DVECTOR *vout ) {
    vout->x = ( vone->y * vtwo->z ) - ( vone->z * vtwo->y );
    vout->y = ( vone->z * vtwo->x ) - ( vone->x * vtwo->z );
    vout->z = ( vone->x * vtwo->y ) - ( vone->y * vtwo->x );
    vout->w = 1.0f;
}

/******************************************************************************/
void g3dvector_init ( G3DVECTOR *vec, float x, float y, float z, float w ) {
    vec->x = x;
    vec->y = y;
    vec->z = z;
    vec->w = w;
}

/******************************************************************************/
float g3dvector_length ( G3DVECTOR *vec ) {
    float len = ( vec->x * vec->x ) +
                ( vec->y * vec->y ) +
                ( vec->z * vec->z );

    /*if ( ( len < NORMMAX ) && ( len > NORMMIN ) ) return 1.0f;*/

/** commented out: not accurate enough for spline point rounding. ***/
/** must be made optional ***/
/*** http://bits.stephan-brumme.com/squareRoot.html ***/

  /*unsigned int i = *(unsigned int*) &len;

  // adjust bias
  i  += 127 << 23;
  // approximation of square root
  i >>= 1;

  return *(float*) &i;*/
/******************************************************/

    return sqrt ( len );
}

/******************************************************************************/
/*********** http://en.wikipedia.org/wiki/Fast_inverse_square_root ************/
/*float fastinversesqrt( float number ) {
    long i;
    float x2, y;
    const float threehalfs = 1.5F;
 
    x2 = number * 0.5F;
    y  = number;
    i  = * ( long * ) &y;                       // evil floating point bit level hacking
    i  = 0x5f3759df - ( i >> 1 );               // what the fuck?
    y  = * ( float * ) &i;
    y  = y * ( threehalfs - ( x2 * y * y ) );   // 1st iteration

    return y;
}*/

/******************************************************************************/
/*void g3dvector_normalize ( G3DVECTOR *vec ) {
    float factor = fastinversesqrt ((vec->x*vec->x)+
                                    (vec->y*vec->y)+
                                    (vec->z*vec->z));

    vec->x = vec->x * factor;
    vec->y = vec->y * factor;
    vec->z = vec->z * factor;
    vec->w = 1.0f;
}*/

/******************************************************************************/
void g3dvector_normalize ( G3DVECTOR *vec, float *len ) {
    float LEN = g3dvector_length ( vec );

    if ( len ) (*len) = LEN;

    if ( LEN ) {
        vec->x = vec->x / LEN;
        vec->y = vec->y / LEN;
        vec->z = vec->z / LEN;
        vec->w = 1.0f;
    }
}

/******************************************************************************/
float g3dvector_angle ( G3DVECTOR *v0, G3DVECTOR *v1 ) {
    float s, n;

    s = g3dvector_scalar ( v0, v1 );

    n = g3dvector_length ( v0 ) * g3dvector_length ( v1 );

    if( s == n ) {
        return 0.0f;
    }

    if( n == 0.0f ) {
        return 1.57079633f;
    }

    return  ( float ) acos ( s / n );
}

/******************************************************************************/
void g3dvector_free ( void *data ) {
    G3DVECTOR *vec =  ( G3DVECTOR * ) data;

    free ( vec );
}

/******************************************************************************/
G3DVECTOR *g3dvector_new ( float x, float y, float z, float w ) {
    G3DVECTOR *vec = ( G3DVECTOR * ) calloc ( 0x01, sizeof ( G3DVECTOR ) );

    if ( vec == NULL ) {
        return NULL;
    }

    g3dvector_init ( vec, x, y, z, w );


    return vec;
}

/******************************************************************************/
void g3dvector_createTranslationMatrix ( G3DVECTOR *vec, double *MVX ) {
    MVX[0x00] = 1.0f;
    MVX[0x01] = 0.0f;
    MVX[0x02] = 0.0f;
    MVX[0x03] = 0.0f;

    MVX[0x04] = 0.0f;
    MVX[0x05] = 1.0f;
    MVX[0x06] = 0.0f;
    MVX[0x07] = 0.0f;

    MVX[0x08] = 0.0f;
    MVX[0x09] = 0.0f;
    MVX[0x0A] = 1.0f;
    MVX[0x0B] = 0.0f;

    MVX[0x0C] = vec->x;
    MVX[0x0D] = vec->y;
    MVX[0x0E] = vec->z;
    MVX[0x0F] = 1.0f;
}
