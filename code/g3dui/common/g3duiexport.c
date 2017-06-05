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
uint32_t g3duisettings_blockSize ( G3DUI *gui ) {
    uint32_t blocksize = 0x00;
    LIST *ltmpview = gui->lview;

    while ( ltmpview ) {
        G3DUIVIEW *view = ( G3DUIVIEW * ) ltmpview->data;

        blocksize += ( common_g3duiview_blockSize ( view ) + 0x06 );

        ltmpview = ltmpview->next;
    }

    return blocksize;
}

/******************************************************************************/
void g3duisettings_writeBlock ( G3DUI *gui, 
                                FILE  *fdst ) {
    LIST *ltmpview = gui->lview;

    chunk_write ( G3DUISETTINGSSIG, 
                  g3duisettings_blockSize ( gui ),
                  fdst );

    while ( ltmpview ) {
        G3DUIVIEW *view = ( G3DUIVIEW * ) ltmpview->data;

        common_g3duiview_writeBlock ( view, fdst );

        ltmpview = ltmpview->next;
    }
}


/******************************************************************************/
void g3duisettings_readBlock ( G3DUI    *gui, 
                               G3DSCENE *sce,
                               FILE     *fsrc ) {
    LIST *ltmpview = gui->lview;
    G3DUIVIEW *view = NULL;
    uint16_t chunksig;
    uint32_t chunklen;

    readf ( &chunksig, sizeof ( uint16_t ), 0x01, fsrc );
    readf ( &chunklen, sizeof ( uint32_t ), 0x01, fsrc );

    do {
        switch ( chunksig ) {
            case G3DUISETTINGSSIG :
            break;

            case G3DUIVIEWSIG :
                if ( ltmpview ) {
                    view = ( G3DUIVIEW * ) ltmpview->data;

                    ltmpview = ltmpview->next;
                }
            break;

            case G3DUIVIEWDEFAULTCAMERAPOSITIONSIG :
                if ( view ) {
                    readf ( &((G3DOBJECT*)view->defcam)->pos.x, sizeof ( float ), 1, fsrc );
                    readf ( &((G3DOBJECT*)view->defcam)->pos.y, sizeof ( float ), 1, fsrc );
                    readf ( &((G3DOBJECT*)view->defcam)->pos.z, sizeof ( float ), 1, fsrc );
                }
            break;

            case G3DUIVIEWDEFAULTCAMERAROTATIONSIG :
                if ( view ) {
                    readf ( &((G3DOBJECT*)view->defcam)->rot.x, sizeof ( float ), 1, fsrc );
                    readf ( &((G3DOBJECT*)view->defcam)->rot.y, sizeof ( float ), 1, fsrc );
                    readf ( &((G3DOBJECT*)view->defcam)->rot.z, sizeof ( float ), 1, fsrc );
                }
            break;

            case G3DUIVIEWDEFAULTCAMERASCALINGSIG :
                if ( view ) {
                    readf ( &((G3DOBJECT*)view->defcam)->sca.x, sizeof ( float ), 1, fsrc );
                    readf ( &((G3DOBJECT*)view->defcam)->sca.y, sizeof ( float ), 1, fsrc );
                    readf ( &((G3DOBJECT*)view->defcam)->sca.z, sizeof ( float ), 1, fsrc );
                }
            break;

            case G3DUIVIEWDEFAULTCAMERAFOCALSIG :
                if ( view ) {
                    readf ( &view->defcam->focal, sizeof ( float ), 1, fsrc );
                }
            break;

            case G3DUIVIEWUSECAMERASIG : {
                if ( view ) {
                    LIST *lcam = g3dobject_getChildrenByType ( sce, 
                                                               G3DCAMERATYPE );
                    LIST *ltmpcam = lcam;
                    uint32_t useCameraID;

                    readf ( &useCameraID, sizeof ( uint32_t ), 1, fsrc );

                    while ( ltmpcam ) {
                        G3DCAMERA *cam = ( G3DCAMERA * ) ltmpcam->data;

                        if ( ((G3DOBJECT*)cam)->id == useCameraID ) {
                            view->cam = cam;
                        }

                        ltmpcam = ltmpcam->next;
                    }
                }
            } break;

            case 0xFFFF : { /* EXTENSIONENDSIG */
                ltmpview = gui->lview;

                while ( ltmpview ) {
                    G3DUIVIEW *view = ( G3DUIVIEW * ) ltmpview->data;

                    g3dobject_updateMatrix_r ( view->defcam, gui->flags );

                    ltmpview = ltmpview->next;
                }

                return;
            } break;

            default :
                fseek ( fsrc, chunklen, SEEK_CUR );
            break;
        }

        readf ( &chunksig, sizeof ( uint16_t ), 0x01, fsrc );
        readf ( &chunklen, sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );
}
