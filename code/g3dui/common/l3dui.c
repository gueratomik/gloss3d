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
G3DIMAGE *common_l3dui_getWorkingChannel ( L3DUI *lui ) {
    G3DUI        *gui = lui->gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( lui->gui->sce );

    if ( obj ) {
        if ( obj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            /*** try the first texture in case no texture is selected ***/
            if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

            if ( tex ) {
                G3DMATERIAL *mat = tex->mat;

                if ( mat ) {
                    uint32_t channelID = GETCHANNEL(lui->engine_flags);
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
G3DIMAGE *common_l3dui_getWorkingImage ( L3DUI *lui ) {
    G3DCHANNEL *chn = common_l3dui_getWorkingChannel ( lui );

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
void common_l3dui_setUVMouseTool ( L3DUI *lui, 
                                              G3DCAMERA        *cam, 
                                              G3DMOUSETOOL     *mou ) {
    G3DUI *gui = lui->gui;

    /*** Call the mouse tool initialization function once. This ***/
    /*** can be used by this function to initialize some values ***/
    if ( mou ) {
        if ( mou->init ) {
            uint32_t msk = mou->init ( mou, gui->sce,
                                           &lui->cam,
                                            /*lui->uvurm*/
                                            gui->urm, lui->engine_flags );

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
void common_l3dui_fillWithColor ( L3DUI *lui, 
                                             uint32_t          color ) {
    G3DIMAGE *img = common_l3dui_getWorkingImage ( lui );

    if ( img ) {
        unsigned char R = ( color & 0x00FF0000 ) >> 0x10,
                      G = ( color & 0x0000FF00 ) >> 0x08,
                      B = ( color & 0x000000FF ) >> 0x00;
        uint32_t i, j;

        for ( i = 0x00; i < img->height; i++ ) {
            for ( j = 0x00; j < img->width; j++ ) {
                uint32_t offset = ( i * img->width ) + j;

                if ( lui->mask[offset] ) {
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
void common_l3dui_resizeBuffers ( L3DUI *lui ) {
    G3DIMAGE *img = common_l3dui_getWorkingImage ( lui );
    L3DMOUSETOOL *seltool = common_g3dui_getMouseTool ( lui->gui,
                                                        SELECTTOOL );

    seltool->obj->reset ( seltool->obj, lui->engine_flags );

    if ( img ) {
        if ( img->width && img->height ) {
            uint32_t size = img->width *
                            img->height;
            lui->mask    = realloc ( lui->mask   , size );
            lui->zbuffer = realloc ( lui->zbuffer, size );

            memset ( lui->mask, 0xFF, size );
        }
    }
}

/******************************************************************************/
void common_l3dui_uvset2facCbk ( L3DUI *lui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( lui->gui->sce );

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
                    g3durm_mesh_pickFaces  ( lui->gui->urm,
                                             lui->gui->sce,
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
void common_l3dui_fac2uvsetCbk ( L3DUI *lui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( lui->gui->sce );

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
                    g3durm_uvmap_pickUVSets  ( /*lui->uvurm*/
                                               lui->gui->urm,
                                               lui->gui->sce,
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
void common_l3dui_uv2verCbk ( L3DUI *lui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( lui->gui->sce );

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
                    g3durm_mesh_pickVertices  ( lui->gui->urm,
                                                lui->gui->sce,
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
void common_l3dui_ver2uvCbk ( L3DUI *lui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( lui->gui->sce );

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
                    g3durm_uvmap_pickUVs  ( /*lui->uvurm*/
                                            lui->gui->urm, 
                                            lui->gui->sce,
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
void common_l3dui_undoCbk ( L3DUI *lui ) {
    uint32_t return_value;

    return_value = g3durmanager_undo ( /*lui->uvurm*/lui->gui->urm, lui->engine_flags );

    common_g3dui_interpretMouseToolReturnFlags ( lui->gui, return_value );

    /*if ( return_value & REDRAWVIEW ) {
        g3dui_redrawGLViews ( gui );
    }

    if ( return_value & REDRAWLIST ) {
        g3dui_redrawObjectList ( gui );
    }

    if ( return_value & REDRAWCURRENTOBJECT ) {
        g3dui_updateAllCurrentEdit ( gui );
    }*/
}

/******************************************************************************/
void common_l3dui_redoCbk ( L3DUI *lui ) {
    uint32_t return_value;

    return_value = g3durmanager_redo ( /*lui->uvurm*/lui->gui->urm, lui->engine_flags );

    common_g3dui_interpretMouseToolReturnFlags ( lui->gui, return_value );

    /*if ( return_value & REDRAWVIEW ) {
        g3dui_redrawGLViews ( gui );
    }

    if ( return_value & REDRAWLIST ) {
        g3dui_redrawObjectList ( gui );
    }

    if ( return_value & REDRAWCURRENTOBJECT ) {
        g3dui_updateAllCurrentEdit ( gui );
    }*/
}

/******************************************************************************/
void common_l3dui_moveSideward ( L3DUI *lui, 
                                            int32_t           x, 
                                            int32_t           y, 
                                            int32_t           xold, 
                                            int32_t           yold ) {
    lui->cam.obj.pos.x -= ( ( float ) ( x - xold ) * 0.005f );
    lui->cam.obj.pos.y += ( ( float ) ( y - yold ) * 0.005f );

    g3dobject_updateMatrix ( ( G3DOBJECT * ) &lui->cam, lui->gui->engine_flags );

    common_l3dui_setCanevas ( lui );
}


/******************************************************************************/
void common_l3dui_moveForward ( L3DUI *lui, 
                                           int32_t           x, 
                                           int32_t           xold ) {
    lui->cam.ortho.z -= ( ( float ) ( x - xold ) * 0.000005f );

    if ( lui->cam.ortho.z < 0.00001f ) lui->cam.ortho.z = 0.00001f;

    lui->cam.ortho.x = lui->cam.ortho.z;
    lui->cam.ortho.y = lui->cam.ortho.z;

    g3dobject_updateMatrix ( ( G3DOBJECT * ) &lui->cam, lui->gui->engine_flags );

    common_l3dui_setCanevas ( lui );
}

/******************************************************************************/
void common_l3dui_setCanevas ( L3DUI *lui ) {

}



/******************************************************************************/
/* Find the current button ID according to x and y coordinates. This helsp us */
/* determine which button we are on. We don't use Motif button here because   */
/* we need to track the mouse after a click, something I think, will be easier*/
/* to achieve that way. I might be wrong though :-/.                          */
/******************************************************************************/
int common_l3dui_getCurrentButton ( L3DUI *lui,
                                               int x,
                                               int y ) {
    int i;

    for ( i = 0x00; i < NBUVMAPBUTTON; i++ ) {
        G3DUIRECTANGLE *rec = &lui->rec[i];

        if ( ( ( x >= rec->x ) && ( x <= ( rec->x + rec->width  ) ) ) &&
             ( ( y >= rec->y ) && ( y <= ( rec->y + rec->height ) ) ) ) {
            return i;
        }
    }

    return -1;
}

/******************************************************************************/
void common_l3dui_destroyGL ( L3DUI *lui ) {

}

/******************************************************************************/
void common_l3dui_init ( L3DUI *lui, 
                                    uint32_t          width,
                                    uint32_t          height ) {
    lui->buttonID = -1;

    /*** as we use an identity projection matrix, the coorinates system will ***/
    /*** be from -1.0f to 1.0f. So we have to move our UVMAP to the center ***/
    /*** of this coordinates system by shifting it by 0.5f ***/
    lui->cam.obj.pos.x =  0.5f;
    lui->cam.obj.pos.y =  0.5f;
    lui->cam.obj.pos.z =  0.0f;

    lui->cam.obj.rot.x = 0.0f;
    lui->cam.obj.rot.y = 0.0f;
    lui->cam.obj.rot.z = 0.0f;

    lui->cam.obj.sca.x = 1.0f;
    lui->cam.obj.sca.y = 1.0f;
    lui->cam.obj.sca.z = 1.0f;

    lui->cam.obj.flags |= CAMERAORTHOGRAPHIC;
	lui->cam.ortho.x = 0.0f;
    lui->cam.ortho.y = 0.0f;
    lui->cam.ortho.z = 0.001f;
    lui->cam.znear   = -1000.0f;
    lui->cam.zfar    = 1000.0f;
	
	lui->cam.obj.name = "UVMapEditor Camera";
	
    /*lui->uvurm = g3durmanager_new ( lui->gui->conf.undolevel );*/

    lui->engine_flags = VIEWVERTEXUV;

    g3dobject_updateMatrix ( ( G3DOBJECT * ) &lui->cam, lui->gui->engine_flags );

    /*** projection matrix will never change ***/
    g3dcore_identityMatrix ( lui->cam.pmatrix );
}

/******************************************************************************/
void common_l3dui_resize ( L3DUI *lui, 
                                      uint32_t          width, 
                                      uint32_t          height ) {
    int i, xpos = ( width - BUTTONSIZE - L3DPATTERNBOARDWIDTH ), brd = 0x02;

    /*** set rectangle position for each button ***/
    for ( i = 0x00; i < NBUVMAPBUTTON; i++, xpos = ( xpos - BUTTONSIZE - brd ) ) {
        lui->rec[i].x      = xpos;
        lui->rec[i].y      = TOOLBARBUTTONSIZE + 0x20;
        lui->rec[i].width  = BUTTONSIZE;
        lui->rec[i].height = BUTTONSIZE - TOOLBARBUTTONSIZE - 0x20;
    }

    lui->arearec.x      = MODEBARBUTTONSIZE;
    lui->arearec.y      = L3DMENUBARHEIGHT + L3DTOOLBARHEIGHT + BUTTONSIZE;
    lui->arearec.width  = width - MODEBARBUTTONSIZE - L3DBOARDWIDTH;
    lui->arearec.height = height - lui->arearec.y;

    lui->patternrec.x      = lui->arearec.x + lui->arearec.width;
    lui->patternrec.y      = L3DMENUBARHEIGHT + L3DTOOLBARHEIGHT;
    lui->patternrec.width  = L3DBOARDWIDTH;
    lui->patternrec.height = L3DPATTERNBOARDHEIGHT;


    lui->fgbgrec.x      = lui->arearec.x + lui->arearec.width + ( L3DPATTERNBOARDWIDTH / 0x02 ) - 0x18;
    lui->fgbgrec.y      = lui->patternrec.y + lui->patternrec.height;
    lui->fgbgrec.width  = 0x30;
    lui->fgbgrec.height = 0x30;

    lui->toolrec.x      = lui->patternrec.x;
    lui->toolrec.y      = lui->patternrec.y + lui->patternrec.height + 0x30;
    lui->toolrec.width  = L3DPATTERNBOARDWIDTH;
    lui->toolrec.height = height - BUTTONSIZE - 0x20 - TOOLBARBUTTONSIZE - L3DPATTERNBOARDHEIGHT - 0x30;


    common_l3dui_setCanevas ( lui );
}

/******************************************************************************/
void common_l3dui_showGL ( L3DUI *lui,
                                      G3DUI            *gui,
                                      G3DMOUSETOOL     *mou,
                                      uint64_t          engine_flags ) {
    G3DOBJECT *obj = g3dscene_getSelectedObject ( gui->sce );
    L3DMOUSETOOL *seltool = common_g3dui_getMouseTool ( gui, SELECTTOOL );

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
                            g3dcamera_project ( &lui->cam, 0x00  );

                            glMatrixMode ( GL_MODELVIEW );
                            glLoadIdentity ( );

                            /*** preserve image aspect ratio ***/
                            lui->cam.obj.sca.y = whRatio;

                            g3dobject_updateMatrix ( &lui->cam.obj, lui->gui->engine_flags );

                            g3dcamera_view ( &lui->cam, 0x00  );

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
void common_l3dui_sizeGL ( L3DUI *lui, 
                                      uint32_t          width, 
                                      uint32_t          height ) {
    glViewport ( 0, 0, width, height );

    lui->arearec.width   = width;
    lui->arearec.height  = height;
}

/******************************************************************************/
void common_l3dui_initGL ( L3DUI *lui ) {
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
