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
#include <r3d.h>

/******************************************************************************/
R3DCAMERA *r3dcamera_new ( G3DCAMERA *cam, uint32_t width, uint32_t height ) {
    R3DCAMERA *rcam   = ( R3DCAMERA * ) calloc ( 0x01, sizeof ( R3DCAMERA ) );
    R3DVECTOR  zero   = { 0.0f, 0.0f, 0.0f };
    G3DOBJECT *objcam = ( G3DOBJECT * ) cam;
    double a = ( height ) ? ( double ) width / height : 0.0f;
    double f = 1.0f / tan ( ( double ) cam->focal / 2.0f * M_PI / 180.0f );
    double zfPzn = cam->zfar  + cam->znear;
    double znMzf = cam->znear - cam->zfar;

    if ( rcam == NULL ) {
        fprintf ( stderr, "r3dcamera_new: memory allocation failed\n" );
    }

    ((R3DOBJECT*)rcam)->obj = cam;

    /*** image render size. Differs from the OpenGL view ***/
    rcam->VPX[0x00] = 0x00;
    rcam->VPX[0x01] = 0x00;
    rcam->VPX[0x02] = width;
    rcam->VPX[0x03] = height;

    /** we cannot use gluPerspective() because we are in a thread. **/
    /* https://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml */
    rcam->PJX[0x00] =  a ? f / a : 0.0f;
    rcam->PJX[0x04] =  0.0f;
    rcam->PJX[0x08] =  0.0f;
    rcam->PJX[0x0C] =  0.0f;
    rcam->PJX[0x01] =  0.0f;
    rcam->PJX[0x05] =  f;
    rcam->PJX[0x09] =  0.0f;
    rcam->PJX[0x0D] =  0.0f;
    rcam->PJX[0x02] =  0.0f;
    rcam->PJX[0x06] =  0.0f;
    rcam->PJX[0x0A] = zfPzn / znMzf;
    rcam->PJX[0x0E] = ( 2.0f * cam->zfar * cam->znear ) / znMzf;
    rcam->PJX[0x03] =  0.0f;
    rcam->PJX[0x07] =  0.0f;
    rcam->PJX[0x0B] = -1.0f;
    rcam->PJX[0x0F] =  0.0f;

    memcpy ( rcam->MVX, ((G3DOBJECT*)cam)->iwmatrix, sizeof ( double ) * 0x10 );

    g3dtinyvector_matrix ( &zero, objcam->wmatrix, &rcam->pos );


    return rcam;
}

/******************************************************************************/
void r3dcamera_free ( R3DCAMERA *rcam ) {
    free ( rcam );
}
