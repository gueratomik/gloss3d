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
#include <g3dui.h>

/******************************************************************************/
static uint32_t g3duiview_cameraPosition ( G3DEXPORTDATA *ged,
                                           G3DVECTOR     *pos,
                                           uint32_t       flags, 
                                           FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &pos->x, fdst );
    size += g3dexport_fwritef ( &pos->y, fdst );
    size += g3dexport_fwritef ( &pos->z, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3duiview_cameraRotation ( G3DEXPORTDATA *ged,
                                           G3DVECTOR     *rot,
                                           uint32_t       flags, 
                                           FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &rot->x, fdst );
    size += g3dexport_fwritef ( &rot->y, fdst );
    size += g3dexport_fwritef ( &rot->z, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3duiview_cameraScaling ( G3DEXPORTDATA *ged,
                                          G3DVECTOR     *sca,
                                          uint32_t       flags, 
                                          FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &sca->x, fdst );
    size += g3dexport_fwritef ( &sca->y, fdst );
    size += g3dexport_fwritef ( &sca->z, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3duiview_cameraFocal ( G3DEXPORTDATA *ged,
                                        G3DCAMERA     *cam,
                                        uint32_t       flags, 
                                        FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &cam->focal, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3duiview_useCamera ( G3DEXPORTDATA *ged,
                                      G3DCAMERA     *cam,
                                      uint32_t       flags, 
                                      FILE          *fdst ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) cam;
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &obj->id, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3duiview_flags ( G3DEXPORTDATA *ged,
                                  G3DUIVIEW     *view,
                                  uint32_t       flags, 
                                  FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &view->engine_flags, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3duiview_camera ( G3DEXPORTDATA *ged,
                                   G3DCAMERA     *cam,
                                   uint32_t       flags, 
                                   FILE          *fdst ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) cam;
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_G3DUI_VIEW_CAMERA_POSITION,
                                   g3duiview_cameraPosition,
                                   ged,
                                  &obj->pos,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_G3DUI_VIEW_CAMERA_ROTATION,
                                   g3duiview_cameraRotation,
                                   ged,
                                  &obj->rot,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_G3DUI_VIEW_CAMERA_SCALING,
                                   g3duiview_cameraScaling,
                                   ged,
                                  &obj->sca,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_G3DUI_VIEW_CAMERA_FOCAL,
                                   g3duiview_cameraFocal,
                                   ged,
                                   cam,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3duiview_entry ( G3DEXPORTDATA *ged,
                                   G3DUIVIEW     *view,
                                   uint32_t       flags, 
                                   FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_G3DUI_VIEW_FLAGS,
                                   g3duiview_flags,
                                   ged,
                                   view,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_G3DUI_VIEW_CAMERA,
                                   g3duiview_camera,
                                   ged,
                                   view->defcam,
                                   0xFFFFFFFF,
                                   fdst );

    if ( view->cam != view->defcam ) {
        size += g3dexport_writeChunk ( SIG_G3DUI_VIEW_USECAMERA,
                                       g3duiview_useCamera,
                                       ged,
                                       view->cam,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}

/******************************************************************************/
static uint32_t g3duiviews ( G3DEXPORTDATA *ged, 
                             G3DUI         *gui, 
                             uint32_t       flags, 
                             FILE          *fdst ) {
    LIST *ltmpview = gui->lview;
    uint32_t size = 0x00;

    while ( ltmpview ) {
        G3DUIVIEW *view = ( G3DUIVIEW * ) ltmpview->data;

        size += g3dexport_writeChunk ( SIG_G3DUI_VIEW_ENTRY,
                                       g3duiview_entry,
                                       ged,
                                       view,
                                       0xFFFFFFFF,
                                       fdst );

        ltmpview = ltmpview->next;
    }

    return size;
}

/******************************************************************************/
uint32_t g3dui_write ( G3DEXPORTDATA *ged, 
                       G3DUI         *gui, 
                       uint32_t       flags, 
                       FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_G3DUI_VIEWS,
                                   g3duiviews,
                                   ged,
                                   gui,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
void g3dui_read ( G3DIMPORTDATA *gid, 
                  uint32_t       chunkEnd, 
                  FILE          *fsrc,
                  G3DUI         *gui ) {
    uint32_t chunkSignature, chunkSize;
    LIST *ltmpview = gui->lview;
    G3DUIVIEW *view;

    g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_G3DUI_VIEWS :
            break;

            case SIG_G3DUI_VIEW_ENTRY :
                if ( ltmpview ) {
                    view = ( G3DUIVIEW * ) ltmpview->data;

                    ltmpview = ltmpview->next;
                }
            break;

            case SIG_G3DUI_VIEW_FLAGS : {
                if ( view ) {
                    g3dimport_freadl ( &view->engine_flags, fsrc );
                }
            } break;

            case SIG_G3DUI_VIEW_CAMERA : {
            } break;

            case SIG_G3DUI_VIEW_CAMERA_POSITION :
                if ( view ) {
                    G3DOBJECT *obj = ( G3DOBJECT * ) view->defcam;

                    g3dimport_freadf ( &obj->pos.x, fsrc );
                    g3dimport_freadf ( &obj->pos.y, fsrc );
                    g3dimport_freadf ( &obj->pos.z, fsrc );

                    g3dobject_updateMatrix ( view->defcam );
                }
            break;

            case SIG_G3DUI_VIEW_CAMERA_ROTATION :
                if ( view ) {
                    G3DOBJECT *obj = ( G3DOBJECT * ) view->defcam;

                    g3dimport_freadf ( &obj->rot.x, fsrc );
                    g3dimport_freadf ( &obj->rot.y, fsrc );
                    g3dimport_freadf ( &obj->rot.z, fsrc );

                    g3dobject_updateMatrix ( view->defcam );
                }
            break;

            case SIG_G3DUI_VIEW_CAMERA_SCALING :
                if ( view ) {
                    G3DOBJECT *obj = ( G3DOBJECT * ) view->defcam;

                    g3dimport_freadf ( &obj->sca.x, fsrc );
                    g3dimport_freadf ( &obj->sca.y, fsrc );
                    g3dimport_freadf ( &obj->sca.z, fsrc );

                    g3dobject_updateMatrix ( view->defcam );
                }
            break;

            case SIG_G3DUI_VIEW_CAMERA_FOCAL :
                if ( view ) {
                    g3dimport_freadf ( &view->defcam->focal, fsrc );
                }
            break;

            case SIG_G3DUI_VIEW_USECAMERA : {
                if ( view ) {
                    uint32_t camID;
                    G3DOBJECT *obj;

                    g3dimport_freadl ( &camID, fsrc );

                    obj = g3dobject_getChildByID ( gid->currentScene, camID );

                    if ( obj ) {
                        if ( obj->type == G3DCAMERATYPE ) {
                            G3DCAMERA *cam = ( G3DCAMERA * ) obj;

                            common_g3duiview_useSelectedCamera ( view, cam );
                        }
                    }
                }
            } break;

            default : {
                fseek ( fsrc, chunkSize, SEEK_CUR );
            } break;
        }

        /** hand the file back to the parent function ***/
        if ( ftell ( fsrc ) == chunkEnd ) break;

        g3dimport_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
        g3dimport_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );

    g3dimportdata_decrementIndentLevel ( gid );
}
