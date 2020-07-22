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
void common_g3duiuvmapeditor_setUVMouseTool ( G3DUIUVMAPEDITOR *uvme, 
                                              G3DCAMERA        *cam, 
                                              G3DMOUSETOOL     *mou ) {
    G3DUI *gui = uvme->gui;

    /*** Call the mouse tool initialization function once. This ***/
    /*** can be used by this function to initialize some values ***/
    if ( mou ) {
        if ( mou->init ) {
            uint32_t msk = mou->init ( mou, gui->sce,
                                           &uvme->cam,
                                            uvme->uvurm, uvme->engine_flags );

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
void common_g3duiuvmapeditor_uvset2facCbk ( G3DUIUVMAPEDITOR *uvme ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( uvme->gui->sce );

    if ( obj ) {

        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DUVMAP *curmap = g3dmesh_getSelectedUVMap ( mes );

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
                g3durm_mesh_pickFaces  ( uvme->gui->urm, 
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

/******************************************************************************/
void common_g3duiuvmapeditor_fac2uvsetCbk ( G3DUIUVMAPEDITOR *uvme ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( uvme->gui->sce );

    if ( obj ) {

        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DUVMAP *curmap = g3dmesh_getSelectedUVMap ( mes );

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
                g3durm_uvmap_pickUVSets  ( uvme->uvurm, 
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

/******************************************************************************/
void common_g3duiuvmapeditor_uv2verCbk ( G3DUIUVMAPEDITOR *uvme ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( uvme->gui->sce );

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DUVMAP *curmap = g3dmesh_getSelectedUVMap ( mes );

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
                g3durm_mesh_pickVertices  ( uvme->gui->urm,
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

/******************************************************************************/
void common_g3duiuvmapeditor_ver2uvCbk ( G3DUIUVMAPEDITOR *uvme ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( uvme->gui->sce );

    if ( obj ) {

        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DUVMAP *curmap = g3dmesh_getSelectedUVMap ( mes );

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
                g3durm_uvmap_pickUVs  ( uvme->uvurm, 
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

/******************************************************************************/
void common_g3duiuvmapeditor_undoCbk ( G3DUIUVMAPEDITOR *uvme ) {
    uint32_t return_value;

    return_value = g3durmanager_undo ( uvme->uvurm, uvme->engine_flags );

    common_g3dui_interpretMouseToolReturnFlags ( uvme->gui, return_value );

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
void common_g3duiuvmapeditor_redoCbk ( G3DUIUVMAPEDITOR *uvme ) {
    uint32_t return_value;

    return_value = g3durmanager_redo ( uvme->uvurm, uvme->engine_flags );

    common_g3dui_interpretMouseToolReturnFlags ( uvme->gui, return_value );

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
void common_g3duiuvmapeditor_moveSideward ( G3DUIUVMAPEDITOR *uvme, 
                                            int32_t           x, 
                                            int32_t           y, 
                                            int32_t           xold, 
                                            int32_t           yold ) {
    uvme->cam.obj.pos.x -= ( ( float ) ( x - xold ) * 0.005f );
    uvme->cam.obj.pos.y += ( ( float ) ( y - yold ) * 0.005f );

    g3dobject_updateMatrix ( ( G3DOBJECT * ) &uvme->cam );

    common_g3duiuvmapeditor_setCanevas ( uvme );
}


/******************************************************************************/
void common_g3duiuvmapeditor_moveForward ( G3DUIUVMAPEDITOR *uvme, 
                                           int32_t           x, 
                                           int32_t           xold ) {
    uvme->cam.ortho.z -= ( ( float ) ( x - xold ) * 0.00005f );

    if ( uvme->cam.ortho.z < 0.0001f ) uvme->cam.ortho.z = 0.0001f;

    uvme->cam.ortho.x = uvme->cam.ortho.z;
    uvme->cam.ortho.y = uvme->cam.ortho.z;

    g3dobject_updateMatrix ( ( G3DOBJECT * ) &uvme->cam );

    common_g3duiuvmapeditor_setCanevas ( uvme );
}

/******************************************************************************/
void common_g3duiuvmapeditor_setCanevas ( G3DUIUVMAPEDITOR *uvme ) {

}



/******************************************************************************/
/* Find the current button ID according to x and y coordinates. This helsp us */
/* determine which button we are on. We don't use Motif button here because   */
/* we need to track the mouse after a click, something I think, will be easier*/
/* to achieve that way. I might be wrong though :-/.                          */
/******************************************************************************/
int common_g3duiuvmapeditor_getCurrentButton ( G3DUIUVMAPEDITOR *uvme,
                                               int x,
                                               int y ) {
    int i;

    for ( i = 0x00; i < NBUVMAPBUTTON; i++ ) {
        G3DUIRECTANGLE *rec = &uvme->rec[i];

        if ( ( ( x >= rec->x ) && ( x <= ( rec->x + rec->width  ) ) ) &&
             ( ( y >= rec->y ) && ( y <= ( rec->y + rec->height ) ) ) ) {
            return i;
        }
    }

    return -1;
}

/******************************************************************************/
void common_g3duiuvmapeditor_destroyGL ( G3DUIUVMAPEDITOR *uvme ) {

}

/******************************************************************************/
void common_g3duiuvmapeditor_init ( G3DUIUVMAPEDITOR *uvme, 
                                    uint32_t          width,
                                    uint32_t          height ) {
    uvme->buttonID = -1;

    /*** as we use an identity projection matrix, the coorinates system will ***/
    /*** be from -1.0f to 1.0f. So we have to move our UVMAP to the center ***/
    /*** of this coordinates system by shifting it by 0.5f ***/
    uvme->cam.obj.pos.x =  0.5f;
    uvme->cam.obj.pos.y =  0.5f;
    uvme->cam.obj.pos.z =  0.0f;

    uvme->cam.obj.rot.x = 0.0f;
    uvme->cam.obj.rot.y = 0.0f;
    uvme->cam.obj.rot.z = 0.0f;

    uvme->cam.obj.sca.x = 1.0f;
    uvme->cam.obj.sca.y = 1.0f;
    uvme->cam.obj.sca.z = 1.0f;

    uvme->cam.obj.flags |= CAMERAORTHOGRAPHIC;
	uvme->cam.ortho.x = 0.0f;
    uvme->cam.ortho.y = 0.0f;
    uvme->cam.ortho.z = 0.001f;
    uvme->cam.znear   = -1000.0f;
    uvme->cam.zfar    = 1000.0f;
	
	uvme->cam.obj.name = "UVMapEditor Camera";
	
    uvme->uvurm = g3durmanager_new ( uvme->gui->conf.undolevel );

    uvme->engine_flags = VIEWVERTEXUV;

    g3dobject_updateMatrix ( ( G3DOBJECT * ) &uvme->cam );

    /*** projection matrix will never change ***/
    g3dcore_identityMatrix ( uvme->cam.pmatrix );
}

/******************************************************************************/
void common_g3duiuvmapeditor_resize ( G3DUIUVMAPEDITOR *uvme, 
                                      uint32_t          width, 
                                      uint32_t          height ) {
    int i, xpos = ( width - BUTTONSIZE - L3DPATTERNBOARDWIDTH ), brd = 0x02;

    /*** set rectangle position for each button ***/
    for ( i = 0x00; i < NBUVMAPBUTTON; i++, xpos = ( xpos - BUTTONSIZE - brd ) ) {
        uvme->rec[i].x      = xpos;
        uvme->rec[i].y      = TOOLBARBUTTONSIZE + 0x20;
        uvme->rec[i].width  = BUTTONSIZE;
        uvme->rec[i].height = BUTTONSIZE - TOOLBARBUTTONSIZE - 0x20;
    }

    uvme->arearec.x      = MODEBARBUTTONSIZE;
    uvme->arearec.y      = L3DMENUBARHEIGHT + L3DTOOLBARHEIGHT + BUTTONSIZE;
    uvme->arearec.width  = width - MODEBARBUTTONSIZE - L3DBOARDWIDTH;
    uvme->arearec.height = height - uvme->arearec.y;

    uvme->patternrec.x      = uvme->arearec.x + uvme->arearec.width;
    uvme->patternrec.y      = L3DMENUBARHEIGHT + L3DTOOLBARHEIGHT;
    uvme->patternrec.width  = L3DBOARDWIDTH;
    uvme->patternrec.height = L3DPATTERNBOARDHEIGHT;


    uvme->fgbgrec.x      = uvme->arearec.x + uvme->arearec.width + ( L3DPATTERNBOARDWIDTH / 0x02 ) - 0x18;
    uvme->fgbgrec.y      = uvme->patternrec.y + uvme->patternrec.height;
    uvme->fgbgrec.width  = 0x30;
    uvme->fgbgrec.height = 0x30;

    uvme->toolrec.x      = uvme->patternrec.x;
    uvme->toolrec.y      = uvme->patternrec.y + uvme->patternrec.height + 0x30;
    uvme->toolrec.width  = L3DPATTERNBOARDWIDTH;
    uvme->toolrec.height = height - BUTTONSIZE - 0x20 - TOOLBARBUTTONSIZE - L3DPATTERNBOARDHEIGHT - 0x30;


    common_g3duiuvmapeditor_setCanevas ( uvme );
}

/******************************************************************************/
void common_g3duiuvmapeditor_showGL ( G3DUIUVMAPEDITOR *uvme,
                                      G3DUI            *gui,
                                      G3DMOUSETOOL     *mou,
                                      uint64_t          engine_flags ) {
    G3DOBJECT *obj = g3dscene_getSelectedObject ( gui->sce );
    L3DMOUSETOOLSELECTRANDOM *seltool = common_g3dui_getMouseTool ( gui, SELECTRANDOMTOOL );

    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );
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
                                whRatio = ( chn->image->width / 
                                            chn->image->height );
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
                            g3dcamera_project ( &uvme->cam, 0x00  );

                            glMatrixMode ( GL_MODELVIEW );
                            glLoadIdentity ( );

                            /*** preserve image aspect ratio ***/
                            uvme->cam.obj.sca.y = whRatio;

                            g3dobject_updateMatrix ( &uvme->cam.obj );

                            g3dcamera_view ( &uvme->cam, 0x00  );

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

                            glBegin ( GL_LINE_LOOP );
                            glVertex3f   ( 0.0f, 0.0f, 0.0f );
                            glVertex3f   ( 1.0f, 0.0f, 0.0f );
                            glVertex3f   ( 1.0f, 1.0f, 0.0f );
                            glVertex3f   ( 0.0f, 1.0f, 0.0f );
                            glEnd ( );

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
void common_g3duiuvmapeditor_sizeGL ( G3DUIUVMAPEDITOR *uvme, 
                                      uint32_t          width, 
                                      uint32_t          height ) {
    glViewport ( 0, 0, width, height );

    uvme->arearec.width   = width;
    uvme->arearec.height  = height;
}

/******************************************************************************/
void common_g3duiuvmapeditor_initGL ( G3DUIUVMAPEDITOR *uvme ) {
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
