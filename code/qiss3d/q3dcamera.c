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
static uint32_t q3dcamera_intersect ( Q3DCAMERA *qcam,
                                      Q3DRAY    *qray, 
                                      float      frame,
                                      uint64_t   query_flags,
                                      uint64_t   render_flags ) {

    return 0x00;
}

/******************************************************************************/
static void q3dcamera_free ( Q3DCAMERA *qcam ) {

}

/******************************************************************************/
static void q3dcamera_initViewport ( Q3DCAMERA *qcam,
                                     uint32_t   width, 
                                     uint32_t   height ) {
    G3DCAMERA *cam = ( G3DCAMERA * ) q3dobject_getObject ( ( Q3DOBJECT * ) qcam );
    G3DOBJECT *objcam = ( G3DOBJECT * ) cam;
    double a = ( height ) ? ( double ) width / height : 0.0f;
    double f = 1.0f / tan ( ( double ) cam->focal / 2.0f * M_PI / 180.0f );
    double zfPzn = cam->zfar  + cam->znear;
    double znMzf = cam->znear - cam->zfar;
    double zfMzn = cam->zfar - cam->znear;

    /*** image render size. Differs from the OpenGL view ***/
    qcam->VPX[0x00] = 0x00;
    qcam->VPX[0x01] = 0x00;
    qcam->VPX[0x02] = width;
    qcam->VPX[0x03] = height;

    if ( ((G3DOBJECT*)cam)->flags & CAMERAORTHOGRAPHIC ) {
        /*** This part simulates the same code as in g3dcamera.c ***/
        /*** for the call to glOrtho ***/
        float dist;

        double orthoLeft   = - ( ( double ) width  * cam->ortho.z ) + cam->ortho.x,
               orthoRight  =   ( ( double ) width  * cam->ortho.z ) + cam->ortho.x,
               orthoBottom = - ( ( double ) height * cam->ortho.z ) + cam->ortho.y,
               orthoTop    =   ( ( double ) height * cam->ortho.z ) + cam->ortho.y;
        double rightPlusLeft  = ( orthoRight + orthoLeft   ),
               rightMinusLeft = ( orthoRight - orthoLeft   ),
               topPlusBottom  = ( orthoTop   + orthoBottom ),
               topMinusBottom = ( orthoTop   - orthoBottom );

    /* https://www.khronos.org/registry/OpenGL-Refpages/gl2.1/xhtml/glOrtho.xml */
        qcam->PJX[0x00] =   ( double ) 2.0f / rightMinusLeft;
        qcam->PJX[0x04] =  0.0f;
        qcam->PJX[0x08] =  0.0f;
        qcam->PJX[0x0C] = - ( double ) rightPlusLeft / rightMinusLeft;
        qcam->PJX[0x01] =  0.0f;
        qcam->PJX[0x05] =  2.0f / topMinusBottom;
        qcam->PJX[0x09] =  0.0f;
        qcam->PJX[0x0D] = - ( double ) topPlusBottom / topMinusBottom;
        qcam->PJX[0x02] =  0.0f;
        qcam->PJX[0x06] =  0.0f;
        qcam->PJX[0x0A] = - ( double ) 2.0f / zfMzn;
        qcam->PJX[0x0E] = - ( zfPzn / zfMzn );
        qcam->PJX[0x03] =  0.0f;
        qcam->PJX[0x07] =  0.0f;
        qcam->PJX[0x0B] =  0.0f;
        qcam->PJX[0x0F] =  1.0f;
    } else {
        /** we cannot use gluPerspective() because we are in a thread. **/
        /* https://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml */
        qcam->PJX[0x00] =  a ? f / a : 0.0f;
        qcam->PJX[0x04] =  0.0f;
        qcam->PJX[0x08] =  0.0f;
        qcam->PJX[0x0C] =  0.0f;
        qcam->PJX[0x01] =  0.0f;
        qcam->PJX[0x05] =  f;
        qcam->PJX[0x09] =  0.0f;
        qcam->PJX[0x0D] =  0.0f;
        qcam->PJX[0x02] =  0.0f;
        qcam->PJX[0x06] =  0.0f;
        qcam->PJX[0x0A] = zfPzn / znMzf;
        qcam->PJX[0x0E] = ( 2.0f * cam->zfar * cam->znear ) / znMzf;
        qcam->PJX[0x03] =  0.0f;
        qcam->PJX[0x07] =  0.0f;
        qcam->PJX[0x0B] = -1.0f;
        qcam->PJX[0x0F] =  0.0f;
    }
}

/******************************************************************************/
void q3dcamera_init ( Q3DCAMERA *qcam, 
                      G3DCAMERA *cam,
                      uint32_t   id,
                      uint64_t   object_flags,
                      uint32_t    width,
                      uint32_t   height ) {
    static Q3DVECTOR3F origin = { 0.0f, 0.0f, 0.0f };

    q3dobject_init ( ( Q3DOBJECT * ) qcam,
                     ( G3DOBJECT * ) cam,
                     id,
                     object_flags,
    Q3DFREE_CALLBACK(q3dcamera_free),
Q3DINTERSECT_CALLBACK(q3dcamera_intersect) );

    q3dvector3f_matrixf ( &origin, ((G3DOBJECT*)cam)->worldMatrix, &qcam->wpos );

    q3dcamera_initViewport ( qcam, width, height );
}

/******************************************************************************/
Q3DCAMERA *q3dcamera_new ( G3DCAMERA *cam,
                           uint32_t  id,
                           uint64_t  object_flags,
                           uint32_t  width,
                           uint32_t  height ) {
    Q3DCAMERA *qcam = ( Q3DCAMERA * ) calloc ( 0x01, sizeof ( Q3DCAMERA ) );

    if ( qcam == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    q3dcamera_init ( qcam, cam, id, object_flags, width, height );


    return qcam;
}
