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
int32_t g2dvector_scalar ( G2DVECTOR *v0, G2DVECTOR *v1 ) {
    return ( ( v0->x * v1->x ) + ( v0->y * v1->y ) );
}

/******************************************************************************/
int32_t g2dvector_cross ( G2DVECTOR *vone, G2DVECTOR *vtwo, G3DVECTOR *vout ) {
    int32_t z = ( vone->x * vtwo->y ) - ( vone->y * vtwo->x );

    if ( vout ) {
        vout->x = 0.0f;
        vout->y = 0.0f;
        vout->z = ( float ) z;
        vout->w = 1.0f;
    }

    return z;
}

/******************************************************************************/
void g2dvector_average ( G2DVECTOR *vone, G2DVECTOR *vtwo, G2DVECTOR *vout ) {
    vout->x = ( vone->x + vtwo->x ) >> 0x01;
    vout->y = ( vone->y + vtwo->y ) >> 0x01;
}

/******************************************************************************/
void g2dvector_init ( G2DVECTOR *vec, int32_t x, int32_t y ) {
    vec->x = x;
    vec->y = y;
}

/******************************************************************************/
uint32_t g2dvector_length ( G2DVECTOR *vec ) {
    float len = sqrt ( ( vec->x * vec->x ) +
                       ( vec->y * vec->y ) );
    return ( uint32_t ) len;
}

/******************************************************************************/
void g2dvector_normalize ( G2DVECTOR *vec ) {
    float len = g2dvector_length ( vec );

    if ( len ) {
        vec->x = vec->x / len;
        vec->y = vec->y / len;
    }
}

/******************************************************************************/
void g2dvector_free ( G2DVECTOR *data ) {
    G2DVECTOR *vec =  ( G2DVECTOR * ) data;

    free ( vec );
}

/******************************************************************************/
G2DVECTOR *g2dvector_new ( int32_t x, int32_t y ) {
    G2DVECTOR *vec = ( G2DVECTOR * ) calloc ( 0x01, sizeof ( G2DVECTOR ) );

    if ( vec == NULL ) {
        return NULL;
    }

    g2dvector_init ( vec, x, y );


    return vec;
}

