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
R3DCAMERA *r3dcamera_new ( G3DCAMERA *cam ) {
    R3DCAMERA *rcam   = ( R3DCAMERA * ) calloc ( 0x01, sizeof ( R3DCAMERA ) );
    G3DOBJECT *objcam = ( G3DOBJECT * ) cam;
    R3DVECTOR  zero   = { 0.0f, 0.0f, 0.0f };

    if ( rcam == NULL ) {
        fprintf ( stderr, "r3dcamera_new: memory allocation failed\n" );
    }

    ((R3DOBJECT*)rcam)->obj = ( G3DOBJECT * ) cam;

    g3dtinyvector_matrix ( &zero, objcam->wmatrix, &rcam->pos );


    return rcam;
}

/******************************************************************************/
void r3dcamera_free ( R3DCAMERA *rcam ) {
    free ( rcam );
}
