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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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
/*void g3dquaternion_convert ( G3DQUATERNION *qua, double *matrix ) {
    float x2 = qua->x * qua->x;
    float y2 = qua->y * qua->y;
    float z2 = qua->z * qua->z;
    float xy = qua->x * qua->y;
    float xz = qua->x * qua->z;
    float yz = qua->y * qua->z;
    float wx = qua->w * qua->x;
    float wy = qua->w * qua->y;
    float wz = qua->w * qua->z;
  
    matrix[0x00] = 1.0f - 2.0f * ( y2 + z2 );
    matrix[0x01] = 2.0f * ( xy - wz );
    matrix[0x02] = 2.0f * ( xz + wy );
    matrix[0x03] = 0.0f;
    matrix[0x04] = 2.0f * ( xy + wz );
    matrix[0x05] = 1.0f - 2.0f * ( x2 + z2 );
    matrix[0x06] = 2.0f * ( yz - wx );
    matrix[0x07] = 0.0f;
    matrix[0x08] = 2.0f * ( xz - wy );
    matrix[0x09] = 2.0f * ( yz + wx );
    matrix[0x0A] = 1.0f - 2.0f * ( x2 + y2 );
    matrix[0x0B] = 0.0f;
    matrix[0x0C] = 0.0f;
    matrix[0x0D] = 0.0f;
    matrix[0x0E] = 0.0f;
    matrix[0x0F] = 1.0f;
}*/

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
/*float g3dquaternion_length ( G3DQUATERNION *qua ) {
    return sqrt ( ( qua->x * qua->x ) + ( qua->y * qua->y ) +
                  ( qua->z * qua->z ) + ( qua->w * qua->w ) );
}*/

/******************************************************************************/
/*void g3dquaternion_normalize ( G3DQUATERNION *qua ) {
    float len = g3dquaternion_length ( qua );

    if ( len ) {
        qua->x = ( qua->x / len );
        qua->y = ( qua->y / len );
        qua->z = ( qua->z / len );
        qua->w = ( qua->w / len );
    }
}*/
