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
static uint32_t g3duiview_flagsOption ( G3DEXPORTV3DATA *ged,
                                        uint32_t       option,
                                        uint32_t       flags, 
                                        FILE          *fdst ) {
    return g3dexportv3_fwritel ( &option, fdst );
}

/******************************************************************************/
static uint32_t g3duiview_flags ( G3DEXPORTV3DATA *ged,
                                  G3DUIVIEW     *view,
                                  uint32_t       flags, 
                                  FILE          *fdst ) {
    uint32_t showTextures   = ( view->engine_flags & NOTEXTURE         ) ? 0 : 1;
    uint32_t showNormals    = ( view->engine_flags & VIEWNORMALS       ) ? 1 : 0;
    uint32_t showBones      = ( view->engine_flags & HIDEBONES         ) ? 0 : 1;
    uint32_t showGrid       = ( view->engine_flags & HIDEGRID          ) ? 0 : 1;
    uint32_t showBackground = ( view->engine_flags & NOBACKGROUNDIMAGE ) ? 0 : 1;
    uint32_t showLighting   = ( view->engine_flags & NOLIGHTING        ) ? 0 : 1;
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_FLAGS_SHOWTEXTURES,
                                   g3duiview_flagsOption,
                                   ged,
                                   showTextures,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_FLAGS_SHOWNORMALS,
                                   g3duiview_flagsOption,
                                   ged,
                                   showNormals,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_FLAGS_SHOWBONES,
                                   g3duiview_flagsOption,
                                   ged,
                                   showBones,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_FLAGS_SHOWGRID,
                                   g3duiview_flagsOption,
                                   ged,
                                   showGrid,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_FLAGS_SHOWBACKGROUND,
                                   g3duiview_flagsOption,
                                   ged,
                                   showBackground,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_FLAGS_SHOWLIGHTING,
                                   g3duiview_flagsOption,
                                   ged,
                                   showLighting,
                                   0xFFFFFFFF,
                                   fdst );


    return size;
}

/******************************************************************************/
static uint32_t g3duiview_cameraPosition ( G3DEXPORTV3DATA *ged,
                                           G3DVECTOR     *pos,
                                           uint32_t       flags, 
                                           FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &pos->x, fdst );
    size += g3dexportv3_fwritef ( &pos->y, fdst );
    size += g3dexportv3_fwritef ( &pos->z, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3duiview_cameraRotation ( G3DEXPORTV3DATA *ged,
                                           G3DVECTOR     *rot,
                                           uint32_t       flags, 
                                           FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &rot->x, fdst );
    size += g3dexportv3_fwritef ( &rot->y, fdst );
    size += g3dexportv3_fwritef ( &rot->z, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3duiview_cameraScaling ( G3DEXPORTV3DATA *ged,
                                          G3DVECTOR     *sca,
                                          uint32_t       flags, 
                                          FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &sca->x, fdst );
    size += g3dexportv3_fwritef ( &sca->y, fdst );
    size += g3dexportv3_fwritef ( &sca->z, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3duiview_cameraFocal ( G3DEXPORTV3DATA *ged,
                                        G3DCAMERA     *cam,
                                        uint32_t       flags, 
                                        FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &cam->focal, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3duiview_useCamera ( G3DEXPORTV3DATA *ged,
                                      G3DCAMERA     *cam,
                                      uint32_t       flags, 
                                      FILE          *fdst ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) cam;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &obj->id, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3duiview_camera ( G3DEXPORTV3DATA *ged,
                                   G3DCAMERA     *cam,
                                   uint32_t       flags, 
                                   FILE          *fdst ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) cam;
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_CAMERA_POSITION,
                                   g3duiview_cameraPosition,
                                   ged,
                                  &obj->pos,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_CAMERA_ROTATION,
                                   g3duiview_cameraRotation,
                                   ged,
                                  &obj->rot,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_CAMERA_SCALING,
                                   g3duiview_cameraScaling,
                                   ged,
                                  &obj->sca,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_CAMERA_FOCAL,
                                   g3duiview_cameraFocal,
                                   ged,
                                   cam,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3duiview_entry ( G3DEXPORTV3DATA *ged,
                                   G3DUIVIEW     *view,
                                   uint32_t       flags, 
                                   FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_FLAGS,
                                   g3duiview_flags,
                                   ged,
                                   view,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_CAMERA,
                                   g3duiview_camera,
                                   ged,
                                   view->defcam,
                                   0xFFFFFFFF,
                                   fdst );

    if ( view->cam != view->defcam ) {
        size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_USECAMERA,
                                       g3duiview_useCamera,
                                       ged,
                                       view->cam,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}

/******************************************************************************/
static uint32_t g3duiviews ( G3DEXPORTV3DATA *ged, 
                             G3DUI         *gui, 
                             uint32_t       flags, 
                             FILE          *fdst ) {
    LIST *ltmpview = gui->lview;
    uint32_t size = 0x00;

    while ( ltmpview ) {
        G3DUIVIEW *view = ( G3DUIVIEW * ) ltmpview->data;

        size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_ENTRY,
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
uint32_t g3dui_write ( G3DEXPORTV3DATA *ged, 
                       G3DUI         *gui, 
                       uint32_t       flags, 
                       FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEWS,
                                   g3duiviews,
                                   ged,
                                   gui,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
void g3dui_read ( G3DIMPORTV3DATA *gid, 
                  uint32_t       chunkEnd, 
                  FILE          *fsrc,
                  G3DUI         *gui ) {
    uint32_t chunkSignature, chunkSize;
    LIST *ltmpview = gui->lview;
    uint32_t showBackground;
    uint32_t showLighting;
    uint32_t showTextures;
    uint32_t showNormals;
    uint32_t showBones;
    uint32_t showGrid;
    G3DUIVIEW *view;

    g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

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
            } break;

            case SIG_G3DUI_VIEW_FLAGS_SHOWTEXTURES : {
                if ( view ) {
                    g3dimportv3_freadl ( &showTextures, fsrc );

                    view->engine_flags |= ( showTextures ) ? 0 : NOTEXTURE;
                }
            } break;

            case SIG_G3DUI_VIEW_FLAGS_SHOWNORMALS : {
                if ( view ) {
                    g3dimportv3_freadl ( &showNormals, fsrc );

                    view->engine_flags |= ( showNormals ) ? VIEWNORMALS : 0;
                }
            } break;

            case SIG_G3DUI_VIEW_FLAGS_SHOWBONES : {
                if ( view ) {
                    g3dimportv3_freadl ( &showBones, fsrc );

                    view->engine_flags |= ( showBones ) ? 0 : HIDEBONES;
                }
            } break;

            case SIG_G3DUI_VIEW_FLAGS_SHOWGRID : {
                if ( view ) {
                    g3dimportv3_freadl ( &showGrid, fsrc );

                    view->engine_flags |= ( showGrid ) ? 0 : HIDEGRID;
                }
            } break;

            case SIG_G3DUI_VIEW_FLAGS_SHOWBACKGROUND : {
                if ( view ) {
                    g3dimportv3_freadl ( &showBackground, fsrc );

                    view->engine_flags |= ( showBackground ) ? 0 : NOBACKGROUNDIMAGE;
                }
            } break;

            case SIG_G3DUI_VIEW_FLAGS_SHOWLIGHTING : {
                if ( view ) {
                    g3dimportv3_freadl ( &showLighting, fsrc );

                    view->engine_flags |= ( showLighting ) ? 0 : NOLIGHTING;
                }
            } break;

            case SIG_G3DUI_VIEW_CAMERA : {
            } break;

            case SIG_G3DUI_VIEW_CAMERA_POSITION :
                if ( view ) {
                    G3DOBJECT *obj = ( G3DOBJECT * ) view->defcam;

                    g3dimportv3_freadf ( &obj->pos.x, fsrc );
                    g3dimportv3_freadf ( &obj->pos.y, fsrc );
                    g3dimportv3_freadf ( &obj->pos.z, fsrc );

                    g3dobject_updateMatrix ( view->defcam, gui->engine_flags );
                }
            break;

            case SIG_G3DUI_VIEW_CAMERA_ROTATION :
                if ( view ) {
                    G3DOBJECT *obj = ( G3DOBJECT * ) view->defcam;

                    g3dimportv3_freadf ( &obj->rot.x, fsrc );
                    g3dimportv3_freadf ( &obj->rot.y, fsrc );
                    g3dimportv3_freadf ( &obj->rot.z, fsrc );

                    g3dobject_updateMatrix ( view->defcam, gui->engine_flags );
                }
            break;

            case SIG_G3DUI_VIEW_CAMERA_SCALING :
                if ( view ) {
                    G3DOBJECT *obj = ( G3DOBJECT * ) view->defcam;

                    g3dimportv3_freadf ( &obj->sca.x, fsrc );
                    g3dimportv3_freadf ( &obj->sca.y, fsrc );
                    g3dimportv3_freadf ( &obj->sca.z, fsrc );

                    g3dobject_updateMatrix ( view->defcam, gui->engine_flags );
                }
            break;

            case SIG_G3DUI_VIEW_CAMERA_FOCAL :
                if ( view ) {
                    g3dimportv3_freadf ( &view->defcam->focal, fsrc );
                }
            break;

            case SIG_G3DUI_VIEW_USECAMERA : {
                if ( view ) {
                    uint32_t camID;
                    G3DOBJECT *obj;

                    g3dimportv3_freadl ( &camID, fsrc );

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

        g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
        g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );
    } while ( feof ( fsrc ) == 0x00 );

    g3dimportv3data_decrementIndentLevel ( gid );
}
