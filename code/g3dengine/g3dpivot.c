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
static void _default_free ( G3DOBJECT *obj ) {


}

/******************************************************************************/
static uint32_t _default_draw ( G3DOBJECT *obj, 
                                G3DCAMERA *curcam, 
                                G3DENGINE *engine, 
                                uint64_t   engine_flags ) {
#ifdef need_refactor
    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable   ( GL_LIGHTING );
    glColor3ub  ( 0xFF, 0x00, 0xFF );
    glBegin ( GL_LINES );
    glVertex3f ( 0.0f, 0.0, 0.0f );
    glVertex3f ( 1.0f, 0.0, 0.0f );

    glVertex3f ( 0.0f, 0.0, 0.0f );
    glVertex3f ( 0.0f, 1.0, 0.0f );

    glVertex3f ( 0.0f, 0.0, 0.0f );
    glVertex3f ( 0.0f, 0.0, 1.0f );

    glEnd ( );
    glPopAttrib ( );
#endif

    return 0x00;
}

/******************************************************************************/
void g3dpivot_orbit ( G3DPIVOT *piv, float diffx, float diffy ) {
    G3DOBJECT *yaxis  = ( G3DOBJECT * ) piv,
              *xaxis  = ( G3DOBJECT * ) yaxis->childList->data,
              *locam  = ( G3DOBJECT * ) xaxis->childList->data;
    float LCX[0x10];

    yaxis->rot.y -= ( diffx );

    xaxis->rot.x -= ( diffy );

    g3dobject_updateMatrix_r ( yaxis, 0x00 );

    if ( ((G3DOBJECT*)piv->cam)->parent ) {
        G3DOBJECT *parent = ((G3DOBJECT*)piv->cam)->parent;

        g3dcore_multMatrixf ( parent->inverseWorldMatrix,
                              locam->worldMatrix,
                              LCX ); /* revised */
    } else {
        memcpy ( LCX, locam->worldMatrix, sizeof ( LCX ) );
    }

    g3dcore_getMatrixTranslationf ( LCX, &((G3DOBJECT*)piv->cam)->pos );
    g3dcore_getMatrixRotationf    ( LCX, &((G3DOBJECT*)piv->cam)->rot );
    g3dcore_getMatrixScalef       ( LCX, &((G3DOBJECT*)piv->cam)->sca );

    g3dobject_updateMatrix_r ( ( G3DOBJECT * ) piv->cam, 0x00 );

    /*g3dobject_printCoordinates ( ( G3DOBJECT * ) piv->cam );*/
}

/******************************************************************************/
/*** The principle is to build a pivot that has its Zaxis parallel to the   ***/
/*** Camera's world ZAxis, so we get nice rotation ***/
void g3dpivot_init ( G3DPIVOT  *piv, 
                     G3DCAMERA *cam, 
                     G3DCURSOR *csr, 
                     uint64_t engine_flags ) {
    G3DOBJECT *objpiv   = ( G3DOBJECT * ) piv;
    G3DOBJECT *objcam   = ( G3DOBJECT * ) cam;
    G3DOBJECT *yaxisobj = objpiv;
    G3DOBJECT *xaxisobj = g3dobject_new ( 0x00, "XAxis", 0x00 );
    G3DOBJECT *locam    = g3dobject_new ( 0x00, "Local Camera", 0x00 );
    G3DVECTOR3F origin    = { 0.0f, 0.0f, 0.0f };
    G3DVECTOR3F  zaxis    = { 0.0f, 0.0f, 1.0f };
    G3DVECTOR3F camzaxis; /*** camera world position ***/
    G3DVECTOR3F to_cam;
    G3DVECTOR3F sign;
    float RMX[0x10];
    float LCX[0x10];
    G3DVECTOR3F localpos = { csr->pivot.x, 
                             csr->pivot.y, 
                             csr->pivot.z };

    g3dobject_init ( objpiv,
                     G3DPIVOTTYPE,
                     0x00,
                     "YAxis",
                     0x00,
       DRAW_CALLBACK(_default_draw),
       FREE_CALLBACK(_default_free),
                     NULL,
                     NULL,
                     NULL,
                     NULL,
                     NULL,
                     NULL,
                     NULL,
                     NULL,
                     NULL,
                     NULL,
                     NULL,
                     NULL );

    piv->cam = cam;

    g3dvector3f_matrixf ( &localpos, csr->matrix, &objpiv->pos );

    g3dobject_addChild ( yaxisobj, xaxisobj, engine_flags );
    g3dobject_addChild ( xaxisobj, locam   , engine_flags );

    /*** The rotation matrix helps us to determine camera orientation ***/
    /*** without being altered byt the translation values ***/
    g3dcore_extractRotationMatrixf ( objcam->worldMatrix, RMX );

    /*** Get Camera's World ZAxis orientation ***/
    g3dvector3f_matrixf ( &zaxis, RMX, &camzaxis );

    /*** We dont want alignment to the Y-Axis. Our YAxisObj must remain  ***/
    /*** parallel to the working XZ plane ***/
    camzaxis.y = 0.0f;

    /*** Get ready for g3dvector_angle() ***/
    g3dvector3f_normalize ( &camzaxis );

    /*** Align pivot's ZAxis with camera's World ZAxis ***/
    objpiv->rot.y = g3dvector3f_angle ( &camzaxis, &zaxis ) * 180 / M_PI;

    /*** Determine which side of the scene we are on, so we get 360deg ***/
    /*** instead of 180deg ***/
    g3dvector3f_cross ( &camzaxis, &zaxis, &sign );

    if ( sign.y > 0.0f ) objpiv->rot.y = -objpiv->rot.y;

    /*** position the pivot ***/
    g3dobject_updateMatrix_r ( objpiv, 0x00 );

    /*** find the camera's local position within the pivot Space coordinates***/
    g3dcore_multMatrixf ( xaxisobj->inverseWorldMatrix,
                          objcam->worldMatrix,
                          LCX ); /* revised */

    g3dcore_getMatrixTranslationf ( LCX, &locam->pos );
    g3dcore_getMatrixRotationf    ( LCX, &locam->rot );
    g3dcore_getMatrixScalef       ( LCX, &locam->sca );

    g3dobject_updateMatrix_r ( locam, 0x00 );
}

/******************************************************************************/
G3DPIVOT *g3dpivot_new ( G3DCAMERA *cam, 
                         G3DCURSOR *csr, 
                         uint64_t   engine_flags ) {
    G3DPIVOT *piv = ( G3DPIVOT * ) calloc ( 0x01, sizeof ( G3DPIVOT ) );

    if ( piv == NULL ) {
        fprintf ( stderr, "g3dpivot_new: memory allocation failed\n" );

        return NULL;
    }

    g3dpivot_init ( piv, cam, csr, engine_flags );


    return piv;
}

