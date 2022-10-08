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

#define CLEARCOLOR 100

/******************************************************************************/
G3DIMAGE *common_m3dui_getWorkingChannel ( M3DUI *mui ) {
    G3DUI        *gui = mui->gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( mui->gui->sce );

    if ( obj ) {
        if ( obj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            /*** try the first texture in case no texture is selected ***/
            if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

            if ( tex ) {
                G3DMATERIAL *mat = tex->mat;

                if ( mat ) {
                    uint32_t channelID = GETCHANNEL(mui->engine_flags);
                    G3DCHANNEL *chn = g3dmaterial_getChannelByID(mat,channelID);

                    if ( chn ) {
                        return chn;
                    }
                }
            }
        }
    }

    return NULL;
}

/******************************************************************************/
G3DIMAGE *common_m3dui_getWorkingImage ( M3DUI *mui ) {
    G3DCHANNEL *chn = common_m3dui_getWorkingChannel ( mui );

    if ( chn ) {
        if ( chn->flags & USEIMAGECOLOR ) {
            if ( chn->image ) {
                return chn->image;
            }
        }
    }

    return NULL;
}

/******************************************************************************/
void common_m3dui_setUVMouseTool ( M3DUI *mui, 
                                              G3DCAMERA        *cam, 
                                              G3DMOUSETOOL     *mou ) {
    G3DUI *gui = mui->gui;

    /*** Call the mouse tool initialization function once. This ***/
    /*** can be used by this function to initialize some values ***/
    if ( mou ) {
        if ( mou->init ) {
            uint32_t msk = mou->init ( mou, gui->sce,
                                           &mui->cam,
                                            /*mui->uvurm*/
                                            gui->urm, mui->engine_flags );

            common_g3dui_interpretMouseToolReturnFlags ( gui, msk );
        }

        if ( ( mou->flags & MOUSETOOLNOCURRENT ) == 0x00 ) {
            gui->uvmou = mou;
        }
    } else {
        gui->uvmou = NULL;
    }
}

/******************************************************************************/
void common_m3dui_fillWithColor ( M3DUI *mui, 
                                             uint32_t          color ) {
    G3DIMAGE *img = common_m3dui_getWorkingImage ( mui );

    if ( img ) {
        unsigned char R = ( color & 0x00FF0000 ) >> 0x10,
                      G = ( color & 0x0000FF00 ) >> 0x08,
                      B = ( color & 0x000000FF ) >> 0x00;
        uint32_t i, j;

        for ( i = 0x00; i < img->height; i++ ) {
            for ( j = 0x00; j < img->width; j++ ) {
                uint32_t offset = ( i * img->width ) + j;

                if ( mui->mask[offset] ) {
                    switch ( img->bytesPerPixel ) {
                        case 0x03 : {
                            unsigned char (*buffer)[0x03] = img->data;

                            buffer[offset][0x00] = R;
                            buffer[offset][0x01] = G;
                            buffer[offset][0x02] = B;
                        } break;

                        default :
                        break;
                    }
                }
            }
        }

        if ( img->flags & GLIMAGE ) g3dimage_bind ( img );

        img->flags |= ALTEREDIMAGE;
    }
}

/******************************************************************************/
void common_m3dui_resizeBuffers ( M3DUI *mui ) {
    G3DIMAGE *img = common_m3dui_getWorkingImage ( mui );
    M3DMOUSETOOL *seltool = common_g3dui_getMouseTool ( mui->gui,
                                                        SELECTTOOL );

    seltool->obj->reset ( seltool->obj, mui->engine_flags );

    if ( img ) {
        if ( img->width && img->height ) {
            uint32_t size = img->width *
                            img->height;
            mui->mask    = realloc ( mui->mask   , size );
            mui->zbuffer = realloc ( mui->zbuffer, size );

            memset ( mui->mask, 0xFF, size );
        }
    }
}

/******************************************************************************/
void common_m3dui_uvset2facCbk ( M3DUI *mui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( mui->gui->sce );

    if ( obj ) {

        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            if ( tex ) {
                G3DUVMAP *curmap = tex->map;

                if ( curmap ) {
                    LIST *ltmpfac = mes->lfac;
                    LIST *lselold, *lselnew;

                    lselold = list_copy ( mes->lselfac );

                    g3dmesh_unselectAllFaces ( mes );

                    while ( ltmpfac ) {
                        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                        G3DUVSET *uvset = g3dface_getUVSet ( fac, curmap );
                        int i;

                        if ( uvset->flags & UVSETSELECTED ) {
                            if ( ( fac->flags & FACESELECTED ) == 0x00 ) {
                                g3dmesh_selectFace ( mes, fac );
                            }
                        }

                        ltmpfac = ltmpfac->next;
                    }

                    lselnew = list_copy ( mes->lselfac );

                    /*** remember selection ***/
                    g3durm_mesh_pickFaces  ( mui->gui->urm,
                                             mui->gui->sce,
                                             mes,
                                             lselold,
                                             lselnew,
                                             VIEWFACE,
                                             REDRAWVIEW |
                                             REDRAWUVMAPEDITOR );
                }
            }
        }
    }
}

/******************************************************************************/
void common_m3dui_fac2uvsetCbk ( M3DUI *mui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( mui->gui->sce );

    if ( obj ) {

        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            if ( tex ) {
                G3DUVMAP *curmap = tex->map;

                if ( curmap ) {
                    LIST *ltmpfac = mes->lfac;
                    LIST *lselold, *lselnew;

                    lselold = list_copy ( curmap->lseluvset );

                    g3duvmap_unselectAllUVSets ( curmap );

                    while ( ltmpfac ) {
                        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                        int i;

                        if ( fac->flags & FACESELECTED ) {
                            G3DUVSET *uvset = g3dface_getUVSet ( fac, curmap );

                            if ( ( uvset->flags & UVSETSELECTED ) == 0x00 ) {
                                g3duvmap_selectUVSet ( curmap, uvset );
                            }
                        }

                        ltmpfac = ltmpfac->next;
                    }

                    lselnew = list_copy ( curmap->lseluvset );

                    /*** remember selection ***/
                    g3durm_uvmap_pickUVSets  ( /*mui->uvurm*/
                                               mui->gui->urm,
                                               mui->gui->sce,
                                               curmap,
                                               lselold,
                                               lselnew,
                                               VIEWFACEUV,
                                               REDRAWVIEW |
                                               REDRAWUVMAPEDITOR );
                }
            }
        }
    }
}

/******************************************************************************/
void common_m3dui_uv2verCbk ( M3DUI *mui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( mui->gui->sce );

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            if ( tex ) {
                G3DUVMAP *curmap = tex->map;

                if ( curmap ) {
                    LIST *ltmpfac = mes->lfac;
                    LIST *lselold, *lselnew;

                    lselold = list_copy ( mes->lselver );

                    g3dmesh_unselectAllVertices ( mes );

                    while ( ltmpfac ) {
                        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                        G3DUVSET *uvset = g3dface_getUVSet ( fac, curmap );
                        int i;

                        for ( i = 0x00; i < uvset->nbuv; i++ ) {
                            G3DUV *uv = &uvset->veruv[i];

                            if ( uv->flags & UVSELECTED ) {
                                G3DVERTEX *ver = fac->ver[i];

                                if ( ( ver->flags & VERTEXSELECTED ) == 0x00 ) {
                                    g3dmesh_selectVertex ( mes, ver );
                                }
                            }
                        }

                        ltmpfac = ltmpfac->next;
                    }

                    lselnew = list_copy ( mes->lselver );

                    /*** remember selection ***/
                    g3durm_mesh_pickVertices  ( mui->gui->urm,
                                                mui->gui->sce,
                                                mes,
                                                lselold,
                                                lselnew,
                                                VIEWVERTEX,
                                                REDRAWVIEW |
                                                REDRAWUVMAPEDITOR );
                }
            }
        }
    }
}

/******************************************************************************/
void common_m3dui_ver2uvCbk ( M3DUI *mui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( mui->gui->sce );

    if ( obj ) {

        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            if ( tex ) {
                G3DUVMAP *curmap = tex->map;

                if ( curmap ) {
                    LIST *ltmpfac = mes->lfac;
                    LIST *lselold, *lselnew;

                    lselold = list_copy ( curmap->lseluv );

                    g3duvmap_unselectAllUVs ( curmap );

                    while ( ltmpfac ) {
                        G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;
                        G3DUVSET *uvset = g3dface_getUVSet ( fac, curmap );
                        int i;

                        for ( i = 0x00; i < fac->nbver; i++ ) {
                            G3DVERTEX *ver = fac->ver[i];

                            if ( ver->flags & VERTEXSELECTED ) {
                                G3DUV *uv = &uvset->veruv[i];

                                if ( ( uv->flags & UVSELECTED ) == 0x00 ) {
                                    g3duvmap_selectUV ( curmap, uv );
                                }
                            }
                        }

                        ltmpfac = ltmpfac->next;
                    }

                    lselnew = list_copy ( curmap->lseluv );

                    /*** remember selection ***/
                    g3durm_uvmap_pickUVs  ( /*mui->uvurm*/
                                            mui->gui->urm, 
                                            mui->gui->sce,
                                            curmap,
                                            lselold,
                                            lselnew,
                                            VIEWVERTEXUV,
                                            REDRAWVIEW |
                                            REDRAWUVMAPEDITOR );
                }
            }
        }
    }
}

/******************************************************************************/
void common_m3dui_undoCbk ( M3DUI *mui ) {
    uint32_t return_value;

    return_value = g3durmanager_undo ( /*mui->uvurm*/mui->gui->urm, mui->engine_flags );

    common_g3dui_interpretMouseToolReturnFlags ( mui->gui, return_value );

    /*if ( return_value & REDRAWVIEW ) {
        g3dui_redrawGLViews ( gui );
    }

    if ( return_value & REDRAWOBJECTLIST ) {
        g3dui_redrawObjectList ( gui );
    }

    if ( return_value & UPDATECURRENTOBJECT ) {
        g3dui_updateAllCurrentEdit ( gui );
    }*/
}

/******************************************************************************/
void common_m3dui_redoCbk ( M3DUI *mui ) {
    uint32_t return_value;

    return_value = g3durmanager_redo ( /*mui->uvurm*/mui->gui->urm, mui->engine_flags );

    common_g3dui_interpretMouseToolReturnFlags ( mui->gui, return_value );

    /*if ( return_value & REDRAWVIEW ) {
        g3dui_redrawGLViews ( gui );
    }

    if ( return_value & REDRAWOBJECTLIST ) {
        g3dui_redrawObjectList ( gui );
    }

    if ( return_value & UPDATECURRENTOBJECT ) {
        g3dui_updateAllCurrentEdit ( gui );
    }*/
}

/******************************************************************************/
void common_m3dui_moveSideward ( M3DUI *mui, 
                                            int32_t           x, 
                                            int32_t           y, 
                                            int32_t           xold, 
                                            int32_t           yold ) {
    mui->cam.obj.pos.x -= ( ( float ) ( x - xold ) * 0.005f );
    mui->cam.obj.pos.y += ( ( float ) ( y - yold ) * 0.005f );

    g3dobject_updateMatrix ( ( G3DOBJECT * ) &mui->cam, mui->gui->engine_flags );

    common_m3dui_setCanevas ( mui );
}


/******************************************************************************/
void common_m3dui_moveForward ( M3DUI *mui, 
                                           int32_t           x, 
                                           int32_t           xold ) {
    mui->cam.ortho.z -= ( ( float ) ( x - xold ) * 0.000005f );

    if ( mui->cam.ortho.z < 0.00001f ) mui->cam.ortho.z = 0.00001f;

    mui->cam.ortho.x = mui->cam.ortho.z;
    mui->cam.ortho.y = mui->cam.ortho.z;

    g3dobject_updateMatrix ( ( G3DOBJECT * ) &mui->cam, mui->gui->engine_flags );

    common_m3dui_setCanevas ( mui );
}

/******************************************************************************/
void common_m3dui_setCanevas ( M3DUI *mui ) {

}



/******************************************************************************/
/* Find the current button ID according to x and y coordinates. This helsp us */
/* determine which button we are on. We don't use Motif button here because   */
/* we need to track the mouse after a click, something I think, will be easier*/
/* to achieve that way. I might be wrong though :-/.                          */
/******************************************************************************/
int common_m3dui_getCurrentButton ( M3DUI *mui,
                                               int x,
                                               int y ) {
    int i;

    for ( i = 0x00; i < NBUVMAPBUTTON; i++ ) {
        G3DUIRECTANGLE *rec = &mui->rec[i];

        if ( ( ( x >= rec->x ) && ( x <= ( rec->x + rec->width  ) ) ) &&
             ( ( y >= rec->y ) && ( y <= ( rec->y + rec->height ) ) ) ) {
            return i;
        }
    }

    return -1;
}

/******************************************************************************/
void common_m3dui_destroyGL ( M3DUI *mui ) {

}

/******************************************************************************/
void common_m3dui_init ( M3DUI *mui, 
                                    uint32_t          width,
                                    uint32_t          height ) {
    mui->buttonID = -1;

    /*** as we use an identity projection matrix, the coorinates system will ***/
    /*** be from -1.0f to 1.0f. So we have to move our UVMAP to the center ***/
    /*** of this coordinates system by shifting it by 0.5f ***/
    mui->cam.obj.pos.x =  0.5f;
    mui->cam.obj.pos.y =  0.5f;
    mui->cam.obj.pos.z =  0.0f;

    mui->cam.obj.rot.x = 0.0f;
    mui->cam.obj.rot.y = 0.0f;
    mui->cam.obj.rot.z = 0.0f;

    mui->cam.obj.sca.x = 1.0f;
    mui->cam.obj.sca.y = 1.0f;
    mui->cam.obj.sca.z = 1.0f;

    mui->cam.obj.flags |= CAMERAORTHOGRAPHIC;
	mui->cam.ortho.x = 0.0f;
    mui->cam.ortho.y = 0.0f;
    mui->cam.ortho.z = 0.001f;
    mui->cam.znear   = -1000.0f;
    mui->cam.zfar    = 1000.0f;
	
	mui->cam.obj.name = "UVMapEditor Camera";
	
    /*mui->uvurm = g3durmanager_new ( mui->gui->conf.undolevel );*/

    mui->engine_flags = VIEWVERTEXUV;

    g3dobject_updateMatrix ( ( G3DOBJECT * ) &mui->cam, mui->gui->engine_flags );

    /*** projection matrix will never change ***/
    g3dcore_identityMatrix ( mui->cam.pmatrix );
}

/******************************************************************************/
void common_m3dui_resize ( M3DUI *mui, 
                                      uint32_t          width, 
                                      uint32_t          height ) {
    int i, xpos = ( width - BUTTONSIZE - M3DPATTERNBOARDWIDTH ), brd = 0x02;

    /*** set rectangle position for each button ***/
    for ( i = 0x00; i < NBUVMAPBUTTON; i++, xpos = ( xpos - BUTTONSIZE - brd ) ) {
        mui->rec[i].x      = xpos;
        mui->rec[i].y      = TOOLBARBUTTONSIZE + 0x20;
        mui->rec[i].width  = BUTTONSIZE;
        mui->rec[i].height = BUTTONSIZE - TOOLBARBUTTONSIZE - 0x20;
    }

    mui->arearec.x      = MODEBARBUTTONSIZE;
    mui->arearec.y      = M3DMENUBARHEIGHT + M3DTOOLBARHEIGHT + BUTTONSIZE;
    mui->arearec.width  = width - MODEBARBUTTONSIZE - M3DBOARDWIDTH;
    mui->arearec.height = height - mui->arearec.y;

    mui->patternrec.x      = mui->arearec.x + mui->arearec.width;
    mui->patternrec.y      = M3DMENUBARHEIGHT + M3DTOOLBARHEIGHT;
    mui->patternrec.width  = M3DBOARDWIDTH;
    mui->patternrec.height = M3DPATTERNBOARDHEIGHT;


    mui->fgbgrec.x      = mui->arearec.x + mui->arearec.width + ( M3DPATTERNBOARDWIDTH / 0x02 ) - 0x18;
    mui->fgbgrec.y      = mui->patternrec.y + mui->patternrec.height;
    mui->fgbgrec.width  = 0x30;
    mui->fgbgrec.height = 0x30;

    mui->toolrec.x      = mui->patternrec.x;
    mui->toolrec.y      = mui->patternrec.y + mui->patternrec.height + 0x30;
    mui->toolrec.width  = M3DPATTERNBOARDWIDTH;
    mui->toolrec.height = height - BUTTONSIZE - 0x20 - TOOLBARBUTTONSIZE - M3DPATTERNBOARDHEIGHT - 0x30;


    common_m3dui_setCanevas ( mui );
}

/******************************************************************************/
void common_m3dui_showGL ( M3DUI *mui,
                                      G3DUI            *gui,
                                      G3DMOUSETOOL     *mou,
                                      uint64_t          engine_flags ) {
    G3DOBJECT *obj = g3dscene_getSelectedObject ( gui->sce );
    M3DMOUSETOOL *seltool = common_g3dui_getMouseTool ( gui, SELECTTOOL );

    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

        /*** try the first texture in case no texture is selected ***/
        if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

        if ( tex ) {
            G3DMATERIAL *mat = tex->mat;

            if ( mat ) {
                uint32_t channelID = GETCHANNEL(engine_flags);
                G3DCHANNEL *chn = g3dmaterial_getChannelByID(mat,channelID);

                if ( chn ) {
                    float whRatio = 1.0f;

                    glEnable ( GL_TEXTURE_2D );

                    if ( chn->flags & USEIMAGECOLOR ) {
                        if ( chn->image ) {
                            glBindTexture ( GL_TEXTURE_2D, chn->image->id );

                            if ( chn->image->height ) {
                                whRatio = ( float )  chn->image->width / 
                                                     chn->image->height;
                            }

                            /*** disable bilinear filtering ***/
                            glTexParameteri ( GL_TEXTURE_2D, 
                                              GL_TEXTURE_MIN_FILTER, 
                                              GL_NEAREST );
                            glTexParameteri( GL_TEXTURE_2D, 
                                             GL_TEXTURE_MAG_FILTER, 
                                             GL_NEAREST );

                            glMatrixMode(GL_PROJECTION);
                            glLoadIdentity();
                            g3dcamera_project ( &mui->cam, 0x00  );

                            glMatrixMode ( GL_MODELVIEW );
                            glLoadIdentity ( );

                            /*** preserve image aspect ratio ***/
                            mui->cam.obj.sca.y = whRatio;

                            g3dobject_updateMatrix ( &mui->cam.obj, mui->gui->engine_flags );

                            g3dcamera_view ( &mui->cam, 0x00  );

                            glEnable ( GL_COLOR_MATERIAL );
                            glColor3ub ( 0xFF, 0xFF, 0xFF );

                            glBegin ( GL_QUADS );
                            glTexCoord2f ( 0.0f, 0.0f );
                            glVertex3f   ( 0.0f, 0.0f, 0.0f );
                            glTexCoord2f ( 1.0f, 0.0f );
                            glVertex3f   ( 1.0f, 0.0f, 0.0f );
                            glTexCoord2f ( 1.0f, 1.0f );
                            glVertex3f   ( 1.0f, 1.0f, 0.0f );
                            glTexCoord2f ( 0.0f, 1.0f );
                            glVertex3f   ( 0.0f, 1.0f, 0.0f );
                            glEnd ( );

                            /*** reenable bilinear filtering ***/
                            glTexParameteri ( GL_TEXTURE_2D, 
                                              GL_TEXTURE_MIN_FILTER, 
                                              GL_NEAREST_MIPMAP_LINEAR );
                            glTexParameteri( GL_TEXTURE_2D, 
                                             GL_TEXTURE_MAG_FILTER, 
                                             GL_LINEAR );

                            glDisable ( GL_TEXTURE_2D );

                            /*glBegin ( GL_LINE_LOOP );
                            glVertex3f   ( 0.0f, 0.0f, 0.0f );
                            glVertex3f   ( 1.0f, 0.0f, 0.0f );
                            glVertex3f   ( 1.0f, 1.0f, 0.0f );
                            glVertex3f   ( 0.0f, 1.0f, 0.0f );
                            glEnd ( );*/

                            if ( engine_flags & VIEWVERTEXUV ) g3dmesh_drawVertexUVs ( mes, engine_flags );
                            if ( engine_flags & VIEWFACEUV   ) g3dmesh_drawFaceUVs   ( mes, engine_flags );
                        }
                    }


                }
            }
        }
    }

    if ( seltool && ( seltool != mou ) && seltool->gtool.draw ) {
        seltool->gtool.draw ( seltool, gui->sce, engine_flags );
    }

    if ( mou && mou->draw ) {
        mou->draw ( mou, gui->sce, engine_flags );
    }
}

/******************************************************************************/
void common_m3dui_sizeGL ( M3DUI *mui, 
                                      uint32_t          width, 
                                      uint32_t          height ) {
    glViewport ( 0, 0, width, height );

    mui->arearec.width   = width;
    mui->arearec.height  = height;
}

/******************************************************************************/
void common_m3dui_initGL ( M3DUI *mui ) {
    float clearColorf = ( float ) CLEARCOLOR / 255.0f;

    /*** Set clear color for the OpenGL Window ***/
    glClearColor ( clearColorf, clearColorf, clearColorf, 1.0f );

    glDisable ( GL_LIGHTING   );
    glEnable  ( GL_DEPTH_TEST );

    /*** I still have to understand this below ***/
    /*** I got it from the almighty Internet for avoiding Z-Fighting ***/
    glPolygonOffset ( 1.0f, 1.0f );
    glEnable ( GL_POLYGON_OFFSET_FILL );

    glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
    /*glLightModeli ( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );*/
}
