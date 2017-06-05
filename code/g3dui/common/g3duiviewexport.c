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
#include <g3dui.h>

/******************************************************************************/
static uint32_t g3duiviewdefaultcameraposition_blockSize ( ) {
    return sizeof ( float ) * 0x03;
}

/******************************************************************************/
static void g3duiviewdefaultcameraposition_writeBlock ( G3DVECTOR *pos, 
                                                        FILE      *fdst ) {
    chunk_write ( G3DUIVIEWDEFAULTCAMERAPOSITIONSIG, 
                  g3duiviewdefaultcameraposition_blockSize ( ),
                  fdst );

    writef ( &pos->x, sizeof ( float ), 0x01, fdst );
    writef ( &pos->y, sizeof ( float ), 0x01, fdst );
    writef ( &pos->z, sizeof ( float ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t g3duiviewdefaultcamerarotation_blockSize ( ) {
    return sizeof ( float ) * 0x03;
}

/******************************************************************************/
static void g3duiviewdefaultcamerarotation_writeBlock ( G3DVECTOR *rot, 
                                                        FILE      *fdst ) {
    chunk_write ( G3DUIVIEWDEFAULTCAMERAROTATIONSIG, 
                  g3duiviewdefaultcamerarotation_blockSize ( ),
                  fdst );

    writef ( &rot->x, sizeof ( float ), 0x01, fdst );
    writef ( &rot->y, sizeof ( float ), 0x01, fdst );
    writef ( &rot->z, sizeof ( float ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t g3duiviewdefaultcamerascaling_blockSize ( ) {
    return sizeof ( float ) * 0x03;
}

/******************************************************************************/
static void g3duiviewdefaultcamerascaling_writeBlock ( G3DVECTOR *sca, 
                                                       FILE      *fdst ) {
    chunk_write ( G3DUIVIEWDEFAULTCAMERASCALINGSIG, 
                  g3duiviewdefaultcamerascaling_blockSize ( ),
                  fdst );

    writef ( &sca->x, sizeof ( float ), 0x01, fdst );
    writef ( &sca->y, sizeof ( float ), 0x01, fdst );
    writef ( &sca->z, sizeof ( float ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t g3duiviewdefaultcamerafocal_blockSize ( ) {
    return sizeof ( float );
}

/******************************************************************************/
static void g3duiviewdefaultcamerafocal_writeBlock ( float  focal, 
                                                     FILE  *fdst ) {
    chunk_write ( G3DUIVIEWDEFAULTCAMERAFOCALSIG, 
                  g3duiviewdefaultcamerafocal_blockSize ( ),
                  fdst );

    writef ( &focal, sizeof ( float ), 0x01, fdst );
}

/******************************************************************************/
static uint32_t g3duiviewusecamera_blockSize ( ) {
    return sizeof ( uint32_t );
}

/******************************************************************************/
static void g3duiviewusecamera_writeBlock ( G3DCAMERA *cam, 
                                            FILE      *fdst ) {
    chunk_write ( G3DUIVIEWUSECAMERASIG, 
                  g3duiviewusecamera_blockSize ( ),
                  fdst );

    writef ( &((G3DOBJECT*)cam)->id, sizeof ( uint32_t ), 0x01, fdst );
}

/******************************************************************************/
uint32_t common_g3duiview_blockSize ( G3DUIVIEW *view ) {
    uint32_t blocksize = 0x00;

    blocksize += g3duiviewdefaultcameraposition_blockSize ( ) + 0x06;
    blocksize += g3duiviewdefaultcamerarotation_blockSize ( ) + 0x06;
    blocksize += g3duiviewdefaultcamerascaling_blockSize ( ) + 0x06;
    blocksize += g3duiviewdefaultcamerafocal_blockSize ( ) + 0x06;

    if ( view->cam != view->defcam ) {
        blocksize += g3duiviewusecamera_blockSize ( ) + 0x06;
    }

    return blocksize;
}

/******************************************************************************/
void common_g3duiview_writeBlock ( G3DUIVIEW *view, FILE *fdst ) {
    chunk_write ( G3DUIVIEWSIG, 
                  common_g3duiview_blockSize ( view ),
                  fdst );

    g3duiviewdefaultcameraposition_writeBlock ( &((G3DOBJECT*)view->defcam)->pos, fdst );
    g3duiviewdefaultcamerarotation_writeBlock ( &((G3DOBJECT*)view->defcam)->rot, fdst );
    g3duiviewdefaultcamerascaling_writeBlock  ( &((G3DOBJECT*)view->defcam)->sca, fdst );
    g3duiviewdefaultcamerafocal_writeBlock    (  view->defcam->focal, fdst );

    if ( view->cam != view->defcam ) {
        g3duiviewusecamera_writeBlock ( view->cam, fdst );
    }
}
