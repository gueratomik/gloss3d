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
                                        void            *data,
                                        uint32_t         flags, 
                                        FILE            *fdst ) {
    uintptr_t val = ( uintptr_t ) data;
    uint32_t option = ( uint32_t ) val;

    return g3dexportv3_fwritel ( &option, fdst );
}

/******************************************************************************/
static uint32_t g3duiview_flags ( G3DEXPORTV3DATA *ged,
                                  G3DUIVIEW     *view,
                                  uint32_t       flags, 
                                  FILE          *fdst ) {
    uintptr_t showTextures   = ( view->engine_flags & NOTEXTURE         ) ? 0 : 1;
    uintptr_t showNormals    = ( view->engine_flags & VIEWNORMALS       ) ? 1 : 0;
    uintptr_t showBones      = ( view->engine_flags & HIDEBONES         ) ? 0 : 1;
    uintptr_t showGrid       = ( view->engine_flags & HIDEGRID          ) ? 0 : 1;
    uintptr_t showBackground = ( view->engine_flags & NOBACKGROUNDIMAGE ) ? 0 : 1;
    uintptr_t showLighting   = ( view->engine_flags & NOLIGHTING        ) ? 0 : 1;
    uint32_t size = 0x00;


    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_FLAGS_SHOWTEXTURES,
                   EXPORTV3_CALLBACK(g3duiview_flagsOption),
                                     ged,
                              (void*)showTextures,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_FLAGS_SHOWNORMALS,
                   EXPORTV3_CALLBACK(g3duiview_flagsOption),
                                     ged,
                              (void*)showNormals,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_FLAGS_SHOWBONES,
                   EXPORTV3_CALLBACK(g3duiview_flagsOption),
                                     ged,
                              (void*)showBones,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_FLAGS_SHOWGRID,
                   EXPORTV3_CALLBACK(g3duiview_flagsOption),
                                     ged,
                              (void*)showGrid,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_FLAGS_SHOWBACKGROUND,
                   EXPORTV3_CALLBACK(g3duiview_flagsOption),
                                     ged,
                              (void*)showBackground,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_FLAGS_SHOWLIGHTING,
                   EXPORTV3_CALLBACK(g3duiview_flagsOption),
                                     ged,
                              (void*)showLighting,
                                     0xFFFFFFFF,
                                     fdst );


    return size;
}

/******************************************************************************/
static uint32_t g3duiview_cameraPosition ( G3DEXPORTV3DATA *ged,
                                           G3DVECTOR3F     *pos,
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
                                           G3DVECTOR3F     *rot,
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
                                          G3DVECTOR3F     *sca,
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
                   EXPORTV3_CALLBACK(g3duiview_cameraPosition),
                                     ged,
                                    &obj->pos,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_CAMERA_ROTATION,
                   EXPORTV3_CALLBACK(g3duiview_cameraRotation),
                                     ged,
                                    &obj->rot,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_CAMERA_SCALING,
                   EXPORTV3_CALLBACK(g3duiview_cameraScaling),
                                     ged,
                                    &obj->sca,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_CAMERA_FOCAL,
                   EXPORTV3_CALLBACK(g3duiview_cameraFocal),
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
                   EXPORTV3_CALLBACK(g3duiview_flags),
                                     ged,
                                     view,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_CAMERA,
                   EXPORTV3_CALLBACK(g3duiview_camera),
                                     ged,
                                    &view->defcam,
                                     0xFFFFFFFF,
                                     fdst );

    if ( view->cam != &view->defcam ) {
        if ( g3dscene_isObjectReferred ( ged->currentScene, ( G3DOBJECT * ) view->cam ) ) {
            size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEW_USECAMERA,
                           EXPORTV3_CALLBACK(g3duiview_useCamera),
                                             ged,
                                             view->cam,
                                             0xFFFFFFFF,
                                             fdst );
        }  
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
                     EXPORTV3_CALLBACK(g3duiview_entry),
                                       ged,
                                       view,
                                       0xFFFFFFFF,
                                       fdst );

        ltmpview = ltmpview->next;
    }

    return size;
}

/******************************************************************************/
static uint32_t g3duiobject_id ( G3DEXPORTV3DATA *ged,
                                 G3DOBJECT       *obj,
                                 uint32_t         flags, 
                                 FILE            *fdst ) {
    uint32_t id = obj->id;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &id, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3duiobject_collapsed ( G3DEXPORTV3DATA *ged,
                                        G3DOBJECT       *obj,
                                        uint32_t         flags, 
                                        FILE            *fdst ) {
    uint32_t collpased = ( obj->flags & OBJECTCOLLAPSED ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &collpased, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3duiobject_entry ( G3DEXPORTV3DATA *ged,
                                    G3DOBJECT       *obj,
                                    uint32_t         flags, 
                                    FILE            *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_G3DUI_OBJECT_ID,
                   EXPORTV3_CALLBACK(g3duiobject_id),
                                     ged,
                                     obj,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_G3DUI_OBJECT_COLLAPSED,
                   EXPORTV3_CALLBACK(g3duiobject_collapsed),
                                     ged,
                                     obj,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3duiobjects ( G3DEXPORTV3DATA *ged, 
                               G3DUI           *gui, 
                               uint32_t         flags, 
                               FILE            *fdst ) {
    LIST *lobj = NULL, *ltmpobj; 
    uint32_t size = 0x00;

    /*** flatten the object tree ***/
    g3dobject_treeToList_r ( ( G3DOBJECT * ) ged->currentScene, &lobj );

    ltmpobj = lobj;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        size += g3dexportv3_writeChunk ( SIG_G3DUI_OBJECT_ENTRY,
                       EXPORTV3_CALLBACK(g3duiobject_entry),
                                         ged,
                                         obj,
                                         0xFFFFFFFF,
                                         fdst );

        ltmpobj = ltmpobj->next;
    }

    return size;
}

/******************************************************************************/
uint32_t g3dui_write ( G3DEXPORTV3DATA *ged, 
                       G3DUI         *gui, 
                       uint32_t       flags, 
                       FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_G3DUI_OBJECTS,
                   EXPORTV3_CALLBACK(g3duiobjects),
                                     ged,
                                     gui,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_G3DUI_VIEWS,
                   EXPORTV3_CALLBACK(g3duiviews),
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
    G3DOBJECT *obj;

    g3dimportv3_fread ( &chunkSignature, sizeof ( uint32_t ), 0x01, fsrc );
    g3dimportv3_fread ( &chunkSize     , sizeof ( uint32_t ), 0x01, fsrc );

    do {
        PRINT_CHUNK_INFO(chunkSignature,chunkSize,gid->indentLevel);

        switch ( chunkSignature ) {
            case SIG_G3DUI_OBJECTS :
            break;

            case SIG_G3DUI_OBJECT_ENTRY :
            break;

            case SIG_G3DUI_OBJECT_ID : {

                uint32_t objID;

                g3dimportv3_freadl ( &objID, fsrc );

                obj = g3dobject_getChildByID ( ( G3DOBJECT * ) gid->currentScene, objID );
            } break;

            case SIG_G3DUI_OBJECT_COLLAPSED : {
                uint32_t collapsed;

                g3dimportv3_freadl ( &collapsed, fsrc );

                if ( collapsed ) obj->flags |= ( OBJECTCOLLAPSED );
            } break;

            case SIG_G3DUI_VIEWS :
            break;

            case SIG_G3DUI_VIEW_ENTRY :
                if ( ltmpview ) {
                    view = ( G3DUIVIEW * ) ltmpview->data;

                    view->gui->sce = gid->currentScene;

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
                    obj = ( G3DOBJECT * ) &view->defcam;

                    g3dimportv3_freadf ( &obj->pos.x, fsrc );
                    g3dimportv3_freadf ( &obj->pos.y, fsrc );
                    g3dimportv3_freadf ( &obj->pos.z, fsrc );

                    g3dobject_updateMatrix ( ( G3DOBJECT * ) &view->defcam, gui->engine_flags );
                }
            break;

            case SIG_G3DUI_VIEW_CAMERA_ROTATION :
                if ( view ) {
                    obj = ( G3DOBJECT * ) &view->defcam;

                    g3dimportv3_freadf ( &obj->rot.x, fsrc );
                    g3dimportv3_freadf ( &obj->rot.y, fsrc );
                    g3dimportv3_freadf ( &obj->rot.z, fsrc );

                    g3dobject_updateMatrix ( ( G3DOBJECT * ) &view->defcam, gui->engine_flags );
                }
            break;

            case SIG_G3DUI_VIEW_CAMERA_SCALING :
                if ( view ) {
                    obj = ( G3DOBJECT * ) &view->defcam;

                    g3dimportv3_freadf ( &obj->sca.x, fsrc );
                    g3dimportv3_freadf ( &obj->sca.y, fsrc );
                    g3dimportv3_freadf ( &obj->sca.z, fsrc );

                    g3dobject_updateMatrix ( ( G3DOBJECT * ) &view->defcam, gui->engine_flags );
                }
            break;

            case SIG_G3DUI_VIEW_CAMERA_FOCAL :
                if ( view ) {
                    g3dimportv3_freadf ( &view->defcam.focal, fsrc );
                }
            break;

            case SIG_G3DUI_VIEW_USECAMERA : {
                if ( view ) {
                    uint32_t camID;

                    g3dimportv3_freadl ( &camID, fsrc );

                    obj = g3dobject_getChildByID ( ( G3DOBJECT * ) gid->currentScene, camID );

                    if ( obj ) {
                        if ( obj->type == G3DCAMERATYPE ) {
                            G3DCAMERA *cam = ( G3DCAMERA * ) obj;

                            g3duiview_useSelectedCamera ( view, cam );
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
