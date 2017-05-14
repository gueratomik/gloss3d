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
#include <config.h>
#include <g3dengine/g3dengine.h>

/******************************************************************************/
void g3dpivot_free ( G3DOBJECT *obj ) {


}

/******************************************************************************/
uint32_t g3dpivot_draw ( G3DOBJECT *obj, G3DCAMERA *curcam, 
                                     uint32_t engine_flags ) {
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

    return 0x00;
}

/******************************************************************************/
void g3dpivot_orbit ( G3DPIVOT *piv, int32_t x   , int32_t y, 
                                     int32_t xold, int32_t yold ) {
    G3DOBJECT *yaxis  = ( G3DOBJECT * ) piv,
              *xaxis  = ( G3DOBJECT * ) yaxis->lchildren->data,
              *locam  = ( G3DOBJECT * ) xaxis->lchildren->data;
    double LCX[0x10];

    yaxis->rot.y -= ( xold - x );

    xaxis->rot.x -= ( yold - y );

    g3dobject_updateMatrix_r ( yaxis, 0x00 );

    if ( ((G3DOBJECT*)piv->cam)->parent ) {
        G3DOBJECT *parent = ((G3DOBJECT*)piv->cam)->parent;

        g3dcore_multmatrix ( locam->wmatrix, parent->iwmatrix, LCX );
    } else {
        memcpy ( LCX, locam->wmatrix, sizeof ( LCX ) );
    }

    g3dcore_getMatrixTranslation ( LCX, &((G3DOBJECT*)piv->cam)->pos );
    g3dcore_getMatrixRotation    ( LCX, &((G3DOBJECT*)piv->cam)->rot );
    g3dcore_getMatrixScale       ( LCX, &((G3DOBJECT*)piv->cam)->sca );

    g3dobject_updateMatrix_r ( ( G3DOBJECT * ) piv->cam, 0x00 );

    /*g3dobject_printCoordinates ( ( G3DOBJECT * ) piv->cam );*/
}

/******************************************************************************/
/*** The principle is to build a pivot that has its Zaxis parallel to the   ***/
/*** Camera's world ZAxis, so we get nice rotation ***/
void g3dpivot_init ( G3DPIVOT  *piv, 
                     G3DCAMERA *cam, 
                     G3DVECTOR *pos, 
                     uint32_t   engine_flags ) {
    G3DOBJECT *objpiv   = ( G3DOBJECT * ) piv;
    G3DOBJECT *objcam   = ( G3DOBJECT * ) cam;
    G3DOBJECT *yaxisobj = objpiv;
    G3DOBJECT *xaxisobj = g3dobject_new ( 0x00, "XAxis", 0x00 );
    G3DOBJECT *locam    = g3dobject_new ( 0x00, "Local Camera", 0x00 );
    G3DVECTOR origin    = { 0.0f, 0.0f, 0.0f, 1.0f };
    G3DVECTOR  zaxis    = { 0.0f, 0.0f, 1.0f, 1.0f };
    G3DVECTOR camzaxis; /*** camera world position ***/
    G3DVECTOR to_cam;
    G3DVECTOR sign;
    double RMX[0x10];
    double LCX[0x10];

    g3dobject_init ( objpiv, G3DPIVOTTYPE, 0x00, "YAxis", DRAWBEFORECHILDREN,
                                                          g3dpivot_draw,
                                                          g3dpivot_free,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL,
                                                          NULL );

    piv->cam = cam;

    objpiv->pos.x = pos->x;
    objpiv->pos.y = pos->y;
    objpiv->pos.z = pos->z;

    g3dobject_addChild ( yaxisobj, xaxisobj, engine_flags );
    g3dobject_addChild ( xaxisobj, locam   , engine_flags );

    /*** The rotation matrix helps us to determine camera orientation ***/
    /*** without being altered byt the translation values ***/
    g3dcore_extractRotationMatrix ( objcam->wmatrix, RMX );

    /*** Get Camera's World ZAxis orientation ***/
    g3dvector_matrix ( &zaxis, RMX, &camzaxis );

    /*** We dont want alignment to the Y-Axis. Our YAxisObj must remain  ***/
    /*** parallel to the working XZ plane ***/
    camzaxis.y = 0.0f;

    /*** Get ready for g3dvector_angle() ***/
    g3dvector_normalize ( &camzaxis, NULL );

    /*** Align pivot's ZAxis with camera's World ZAxis ***/
    objpiv->rot.y = g3dvector_angle ( &camzaxis, &zaxis ) * 180 / M_PI;

    /*** Determine which side of the scene we are on, so we get 360deg ***/
    /*** instead of 180deg ***/
    g3dvector_cross ( &camzaxis, &zaxis, &sign );

    if ( sign.y > 0.0f ) objpiv->rot.y = -objpiv->rot.y;

    /*** position the pivot ***/
    g3dobject_updateMatrix_r ( objpiv, 0x00 );

    /*** find the camera's local position within the pivot Space coordinates***/
    g3dcore_multmatrix ( objcam->wmatrix, xaxisobj->iwmatrix, LCX );

    g3dcore_getMatrixTranslation ( LCX, &locam->pos );
    g3dcore_getMatrixRotation    ( LCX, &locam->rot );
    g3dcore_getMatrixScale       ( LCX, &locam->sca );

    g3dobject_updateMatrix_r ( locam, 0x00 );
}

/******************************************************************************/
G3DPIVOT *g3dpivot_new ( G3DCAMERA *cam, 
                         G3DVECTOR *pos, 
                         uint32_t   engine_flags ) {
    G3DPIVOT *piv = ( G3DPIVOT * ) calloc ( 0x01, sizeof ( G3DPIVOT ) );

    if ( piv == NULL ) {
        fprintf ( stderr, "g3dpivot_new: memory allocation failed\n" );

        return NULL;
    }

    g3dpivot_init ( piv, cam, pos, engine_flags );


    return piv;
}

