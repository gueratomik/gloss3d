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
#include <qiss3d/q3d.h>

/******************************************************************************/
double q3dvector3f_scalar ( Q3DVECTOR3F *v0, 
                            Q3DVECTOR3F *v1 ) {
    return ( ( v0->x * v1->x ) + 
             ( v0->y * v1->y ) + 
             ( v0->z * v1->z ) );
}

/******************************************************************************/
void q3dvector3f_cross ( Q3DVECTOR3F *vone, 
                         Q3DVECTOR3F *vtwo, 
                         Q3DVECTOR3F *vout ) {
    vout->x = ( vone->y * vtwo->z ) - ( vone->z * vtwo->y );
    vout->y = ( vone->z * vtwo->x ) - ( vone->x * vtwo->z );
    vout->z = ( vone->x * vtwo->y ) - ( vone->y * vtwo->x );
}

/******************************************************************************/
void q3dvector3f_normalize ( Q3DVECTOR3F *vec, 
                             float       *len ) {
    float LEN = q3dvector3f_length ( vec );

    if ( len ) (*len) = LEN;

    if ( LEN /*( len > NORMMAX ) || ( len < NORMMIN )*/ ) {
        vec->x = vec->x / LEN;
        vec->y = vec->y / LEN;
        vec->z = vec->z / LEN;
    }
}

/******************************************************************************/
float q3dvector3f_length ( Q3DVECTOR3F *vec ) {
    float len = sqrt ( ( vec->x * vec->x ) +
                       ( vec->y * vec->y ) +
                       ( vec->z * vec->z ) );
    return len;
}

/******************************************************************************/
void q3dvector3f_matrix ( Q3DVECTOR3F *vec, 
                          double      *matrix,
                          Q3DVECTOR3F *vout ) {
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

    /*vout->w = ( ( vec->x * matrix[0x03] ) + 
                ( vec->y * matrix[0x07] ) + 
                ( vec->z * matrix[0x0B] ) + 
                (          matrix[0x0F] ) );*/
}
