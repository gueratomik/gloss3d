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

/*******************************************************************************
Special Thanks to : 
http://content.gpwiki.org/index.php/OpenGL:Tutorials:Using_Quaternions_to_repres
ent_rotation
*******************************************************************************/

void g3dquaternion_print ( G3DQUATERNION *qua ) {
    printf ( "pitch:%f yaw:%f roll:%f w:%f\n", qua->pitch, 
                                               qua->yaw, 
                                               qua->roll, 
                                               qua->w );
}

/******************************************************************************/
/*https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles*/
void g3dquaternion_toEuler ( G3DQUATERNION *qua, G3DVECTOR *rot ) {
    /* roll (Z-axis rotation) */
    double sinr_cosp = 2.0f *        ( ( qua->w     * qua->roll  ) + 
                                       ( qua->pitch * qua->yaw   ) );
    double cosr_cosp = 1.0f - 2.0f * ( ( qua->roll  * qua->roll  ) +
                                       ( qua->pitch * qua->pitch ) );

    rot->z = atan2 ( sinr_cosp, cosr_cosp );

    /* pitch (X-axis rotation) */
    double sinp = 2 * ( ( qua->w * qua->pitch ) - ( qua->yaw * qua->roll ) );

    if ( fabs ( sinp ) >= 1 ) {
        /* use 90 degrees if out of range*/
        rot->x = copysign ( M_PI / 2.0f, sinp ); 
    } else {
        rot->x = asin ( sinp );
    }

    /* yaw (Y-axis rotation) */
    double siny_cosp = 2.0f *        ( ( qua->w     * qua->yaw   ) + 
                                       ( qua->roll  * qua->pitch ) );
    double cosy_cosp = 1.0f - 2.0f * ( ( qua->pitch * qua->pitch ) + 
                                       ( qua->yaw   * qua->yaw   ) );

    rot->y = atan2 ( siny_cosp, cosy_cosp );
}

/******************************************************************************/
void g3dquaternion_toEulerInDegrees ( G3DQUATERNION *qua, G3DVECTOR *rot ) {
    g3dquaternion_toEuler ( qua, rot );

    rot->x = rot->x * 180.0f / M_PI;
    rot->y = rot->y * 180.0f / M_PI;
    rot->z = rot->z * 180.0f / M_PI;
}

/******************************************************************************/
/* https://en.wikipedia.org/wiki/Slerp#Quaternion_Slerp */
void g3dquaternion_slerp ( G3DQUATERNION *q0, 
                           G3DQUATERNION *q1, 
                           double          t,
                           G3DQUATERNION *qout ) {
    /* Only unit quaternions are valid rotations.
       Normalize to avoid undefined behavior. */
    g3dquaternion_normalize ( q0 );
    g3dquaternion_normalize ( q1 );

    /* Compute the cosine of the angle between the two vectors. */
    double dot = g3dquaternion_scalar ( q0, q1 );

    /* If the dot product is negative, slerp won't take */
    /* the shorter path. Note that v1 and -v1 are equivalent when */
    /* the negation is applied to all four components. Fix by */
    /* reversing one quaternion. */
    if ( dot < 0.0f ) {
        q1->w     = - q1->w;
        q1->pitch = - q1->pitch;
        q1->yaw   = - q1->yaw;
        q1->roll  = - q1->roll;

        dot = -dot;
    }

    const double DOT_THRESHOLD = 0.9995f;

    if ( dot > DOT_THRESHOLD ) {
        /* If the inputs are too close for comfort, linearly interpolate  */
        /* and normalize the result.  */

        qout->w     = q0->w     + ( t * ( q1->w     - q0->w     ) );
        qout->pitch = q0->pitch + ( t * ( q1->pitch - q0->pitch ) );
        qout->yaw   = q0->yaw   + ( t * ( q1->yaw   - q0->yaw   ) );
        qout->roll  = q0->roll  + ( t * ( q1->roll  - q0->roll  ) );
        
        g3dquaternion_normalize ( qout );

        return;
    }

    /* Since dot is in range [0, DOT_THRESHOLD], acos is safe */
    double theta_0     = acos(dot);    /* theta_0 = angle between input vectors */
    double theta       = theta_0*t;    /* theta = angle between v0 and result */
    double sin_theta   = sin(theta);   /* compute this value only once */
    double sin_theta_0 = sin(theta_0); /* compute this value only once */

    double s0 = cos(theta) - dot * sin_theta / sin_theta_0;   /* == sin(theta_0 - theta) / sin(theta_0) */
    double s1 = sin_theta / sin_theta_0;

    qout->w     = ( s0 * q0->w     ) + ( s1 * q1->w     );
    qout->pitch = ( s0 * q0->pitch ) + ( s1 * q1->pitch );
    qout->yaw   = ( s0 * q0->yaw   ) + ( s1 * q1->yaw   );
    qout->roll  = ( s0 * q0->roll  ) + ( s1 * q1->roll  );
}

/*
void Quaternion::FromEuler(float pitch, float yaw, float roll)
{
	// Basically we create 3 Quaternions, one for pitch, one for yaw, one for roll
	// and multiply those together.
	// the calculation below does the same, just shorter
 
	float p = pitch * PIOVER180 / 2.0;
	float y = yaw * PIOVER180 / 2.0;
	float r = roll * PIOVER180 / 2.0;
 
	float sinp = sin(p);
	float siny = sin(y);
	float sinr = sin(r);
	float cosp = cos(p);
	float cosy = cos(y);
	float cosr = cos(r);
 
	this->x = sinr * cosp * cosy - cosr * sinp * siny;
	this->y = cosr * sinp * cosy + sinr * cosp * siny;
	this->z = cosr * cosp * siny - sinr * sinp * cosy;
	this->w = cosr * cosp * cosy + sinr * sinp * siny;
 
	normalise();
}
*/

/******************************************************************************/
#define PIOVER180 0.01745329252
/*void g3dquaternion_set ( G3DQUATERNION *qua, float xrot,
                                             float yrot,
                                             float zrot ) {
    float p = xrot * PIOVER180 / 2.0f;
    float y = yrot * PIOVER180 / 2.0f;
    float r = zrot * PIOVER180 / 2.0f;

    float sinp = sin ( p );
    float siny = sin ( y );
    float sinr = sin ( r );
    float cosp = cos ( p );
    float cosy = cos ( y );
    float cosr = cos ( r );

    qua->x = ( sinr * cosp * cosy ) - ( cosr * sinp * siny );
    qua->y = ( cosr * sinp * cosy ) + ( sinr * cosp * siny );
    qua->z = ( cosr * cosp * siny ) - ( sinr * sinp * cosy );
    qua->w = ( cosr * cosp * cosy ) + ( sinr * sinp * siny );

    g3dquaternion_normalize ( qua );
}*/

/******************************************************************************/
void g3dquaternion_convert ( G3DQUATERNION *qua, double *matrix ) {
    double TMPX[0x10];

    float roll2      = qua->roll  * qua->roll;
    float pitch2     = qua->pitch * qua->pitch;
    float yaw2       = qua->yaw   * qua->yaw;
    float rollPitch  = qua->roll  * qua->pitch;
    float yawRoll    = qua->yaw   * qua->roll;
    float pitchYaw   = qua->pitch * qua->yaw;
    float angleRoll  = qua->w * qua->roll;
    float anglePitch = qua->w * qua->pitch;
    float angleYaw   = qua->w * qua->yaw;
  
    matrix[0x00] = 1.0f - 2.0f * ( pitch2 + yaw2 );
    matrix[0x01] = 2.0f * ( rollPitch - angleYaw );
    matrix[0x02] = 2.0f * ( yawRoll + anglePitch );
    matrix[0x03] = 0.0f;
    matrix[0x04] = 2.0f * ( rollPitch + angleYaw );
    matrix[0x05] = 1.0f - 2.0f * ( roll2 + yaw2 );
    matrix[0x06] = 2.0f * ( pitchYaw - angleRoll );
    matrix[0x07] = 0.0f;
    matrix[0x08] = 2.0f * ( yawRoll - anglePitch );
    matrix[0x09] = 2.0f * ( pitchYaw + angleRoll );
    matrix[0x0A] = 1.0f - 2.0f * ( roll2 + pitch2 );
    matrix[0x0B] = 0.0f;
    matrix[0x0C] = 0.0f;
    matrix[0x0D] = 0.0f;
    matrix[0x0E] = 0.0f;
    matrix[0x0F] = 1.0f;

    /*g3dcore_transposeMatrix ( matrix, TMPX );
    memcpy ( matrix, TMPX, sizeof ( TMPX ) );*/
}

/******************************************************************************/
/*void g3dquaternion_init ( G3DQUATERNION *qua, float x, float y,
                                              float z, float w ) {
    float sinh = sin ( w / 2.0f );
    float cosh = cos ( w / 2.0f );

    qua->x = sinh * x;
    qua->y = sinh * y;
    qua->z = sinh * z;
    qua->w = cosh;
}*/

/******************************************************************************/
/*void g3dquaternion_inverse ( G3DQUATERNION *qua, G3DQUATERNION *qout ) {
    qout->x = - qua->x;
    qout->y = - qua->y;
    qout->z = - qua->z;
    qout->w =   qua->w;
}*/

/******************************************************************************/
/*void g3dquaternion_multiplyVector ( G3DQUATERNION *qua, G3DVECTOR *vec,
                                    G3DQUATERNION *qout ) {
    g3dvector_normalize ( vec );
    G3DQUATERNION ter, inv, tmp;

    g3dquaternion_inverse ( qua, &inv );

    ter.x = vec->x;
    ter.y = vec->y;
    ter.z = vec->z;
    ter.w = 0.0f;
 
    g3dquaternion_multiply ( &ter, &inv, &tmp );
    g3dquaternion_multiply (  qua, &tmp, qout );
}*/

/******************************************************************************/
/*void g3dquaternion_multiply ( G3DQUATERNION *qua0, G3DQUATERNION *qua1,
                              G3DQUATERNION *qout ) {

    qout->x = ( qua0->w * qua1->x ) + ( qua0->x * qua1->w ) +
              ( qua0->y * qua1->z ) - ( qua0->z * qua1->y );

    qout->y = ( qua0->w * qua1->y ) + ( qua0->y * qua1->w ) +
              ( qua0->z * qua1->x ) - ( qua0->x * qua1->z );

    qout->z = ( qua0->w * qua1->z ) + ( qua0->z * qua1->w ) +
              ( qua0->x * qua1->y ) - ( qua0->y * qua1->x );

    qout->w = ( qua0->w * qua1->w ) + ( qua0->x * qua1->x ) +
              ( qua0->y * qua1->y ) - ( qua0->z * qua1->z );

    g3dquaternion_normalize ( qout );
}*/

/******************************************************************************/
float g3dquaternion_scalar ( G3DQUATERNION *q0, G3DQUATERNION *q1 ) {
    return ( ( q0->pitch * q1->pitch ) + ( q0->yaw * q1->yaw ) +
             ( q0->roll  * q1->roll  ) + ( q0->w   * q1->w   ) );
}

/******************************************************************************/
float g3dquaternion_length ( G3DQUATERNION *qua ) {
    return sqrt ( ( qua->pitch * qua->pitch ) + ( qua->yaw * qua->yaw ) +
                  ( qua->roll  * qua->roll  ) + ( qua->w   * qua->w   ) );
}

/******************************************************************************/
void g3dquaternion_normalize ( G3DQUATERNION *qua ) {
    float len = g3dquaternion_length ( qua );

    if ( len ) {
        qua->pitch = ( qua->pitch / len );
        qua->yaw   = ( qua->yaw   / len );
        qua->roll  = ( qua->roll  / len );
        qua->w     = ( qua->w     / len );
    }
}
