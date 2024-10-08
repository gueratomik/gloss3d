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
void m3duiview_moveSideward ( M3DUIVIEW *view, 
                              float      difx,
                              float      dify ) {
    view->cam.obj.pos.x += ( difx * 0.005f );
    view->cam.obj.pos.y -= ( dify * 0.005f );

    g3dobject_updateMatrix ( ( G3DOBJECT * ) &view->cam, view->mui->engine_flags );
}

/******************************************************************************/
void m3duiview_moveForward ( M3DUIVIEW *view, 
                             float      difx ) {
    view->cam.ortho.z += ( difx * 0.000005f );

    if ( view->cam.ortho.z < 0.00001f ) view->cam.ortho.z = 0.00001f;

    view->cam.ortho.x = view->cam.ortho.z;
    view->cam.ortho.y = view->cam.ortho.z;

    g3dobject_updateMatrix ( ( G3DOBJECT * ) &view->cam, view->mui->engine_flags );
}

/******************************************************************************/
void m3duiview_destroyGL ( M3DUIVIEW *view ) {

}

/******************************************************************************/
void m3duiview_pressButton ( M3DUIVIEW *view, 
                             int        x,
                             int        y ) {
    int i;

    for ( i = 0x00; i < NBVIEWBUTTON; i++ ) {
        G3DUIRECTANGLE *pixrec = &view->pixrec[i];

        if ( ( ( x >= pixrec->x ) && ( x <= ( pixrec->x + pixrec->width  ) ) ) &&
             ( ( y >= pixrec->y ) && ( y <= ( pixrec->y + pixrec->height ) ) ) ) {

            view->pressedButtonID = i;
        }
    }
}

/******************************************************************************/
void m3duiview_releaseButton ( M3DUIVIEW *view ) {
    view->pressedButtonID = -1;
}

/******************************************************************************/
void m3duiview_init ( M3DUIVIEW *view ) {
    view->pressedButtonID = -1;

    /*** as we use an identity projection matrix, the coorinates system will ***/
    /*** be from -1.0f to 1.0f. So we have to move our UVMAP to the center ***/
    /*** of this coordinates system by shifting it by 0.5f ***/

    view->cam.obj.pos.x =  0.5f;
    view->cam.obj.pos.y =  0.5f;
    view->cam.obj.pos.z =  0.0f;

    view->cam.obj.rot.x = 0.0f;
    view->cam.obj.rot.y = 0.0f;
    view->cam.obj.rot.z = 0.0f;

    view->cam.obj.sca.x = 1.0f;
    view->cam.obj.sca.y = 1.0f;
    view->cam.obj.sca.z = 1.0f;

    view->cam.obj.flags = CAMERAORTHOGRAPHIC;

	view->cam.ortho.x  = 0.0f;
    view->cam.ortho.y = 0.0f;
    view->cam.ortho.z = 0.001f;
    view->cam.znear   = -1000.0f;
    view->cam.zfar    = 1000.0f;

	view->cam.obj.name = "UVMapEditor Camera";
	
    /*mui->uvurm = g3durmanager_new ( mui->gui->conf.undolevel );*/

    g3dobject_updateMatrix ( ( G3DOBJECT * ) &view->cam, view->mui->engine_flags );

    /*** projection matrix will never change ***/
    g3dcore_identityMatrixf ( view->cam.pmatrix );
}

/******************************************************************************/
void m3duiview_resize ( M3DUIVIEW *view, 
                        uint32_t   width, 
                        uint32_t   height,
                        uint32_t   menuHeight ) {
    int i, margin = 0x02, xpos;

    view->menurec.x      = 0x00;
    view->menurec.y      = 0x00;
    view->menurec.width  = width;
    view->menurec.height = menuHeight;

    view->glrec.x      = 0x00;
    view->glrec.y      = menuHeight;
    view->glrec.width  = width;
    view->glrec.height = height - menuHeight;

    view->navrec.x      = width - ( margin + ( NBVIEWBUTTON * ( BUTTONSIZE + margin ) ) );
    view->navrec.y      = 0x00;
    view->navrec.width  = NBVIEWBUTTON * ( BUTTONSIZE + margin ) + margin;
    view->navrec.height = menuHeight;

    view->menurec.width -= view->navrec.width;

    for ( i = 0x00, xpos = margin; i < NBVIEWBUTTON; i++, xpos += ( BUTTONSIZE + margin ) ) {
        view->pixrec[i].x      = xpos;
        view->pixrec[i].y      = 0x00;
        view->pixrec[i].width  = BUTTONSIZE;
        view->pixrec[i].height = BUTTONSIZE;
    }
}

/******************************************************************************/
uint64_t m3duiview_inputGL ( M3DUIVIEW *view, G3DEvent *g3dev ) {
    M3DUI         *mui      = view->mui;
    G3DUI         *gui      = mui->gui;
    uint64_t ret = 0x00;

#ifdef __linux__
    if ( glXMakeCurrent ( view->dpy,
                          view->win,
                          view->glctx ) == TRUE ) {
#endif
#ifdef __MINGW32__
    HDC dc = GetDC ( view->hWnd );
    if ( wglMakeCurrent ( dc, view->glctx ) == TRUE ) {
#endif
    if ( mui->curmou ) {
        if ( g3dev->type == G3DButtonPress ) {
            /*** Note: buffers are resized via m3dui_resizeBuffers()    ***/
            /*** located in : gtk3/menu/uvviewmenu.c, gtk3/m3duiview.c  ***/
            /*** and gtk3/m3duichannelimagecreator.c                    ***/

            mui->curmou->tool->mask    = mui->mask;
            mui->curmou->tool->zbuffer = mui->zbuffer;
        }

        if ( mui->curmou->tool->event ) {
            ret = mui->curmou->tool->event ( mui->curmou->tool, 
                                             gui->sce,
                                            &view->cam, 
                                             gui->urm,
                                             mui->engine_flags, 
                                             g3dev );

            if ( g3dev->type == G3DButtonRelease ) {
                g3dcursor_reset ( &gui->sce->csr );
            }
        }
    }

#ifdef __linux__
    }
#endif
#ifdef __MINGW32__
    } ReleaseDC ( view->hWnd, dc );
#endif

    return ret;
}

/******************************************************************************/
void m3duiview_showGL ( M3DUIVIEW    *view,
                        uint64_t      engine_flags ) {
    G3DUI *gui = view->mui->gui;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( gui->sce );
    G3DUIMOUSETOOL *mou = view->mui->curmou;
    G3DMOUSETOOL *tool = ( mou ) ? mou->tool : NULL;
    /*** used to display the selection region ***/
    G3DUIMOUSETOOL *selmou = g3dui_getMouseTool ( gui, SELECTTOOL );
    G3DMOUSETOOL *seltool = ( selmou ) ? selmou->tool : NULL;
#ifdef __linux__
    if ( glXMakeCurrent ( view->dpy,
                          view->win,
                          view->glctx ) == TRUE ) {
#endif
#ifdef __MINGW32__
    HDC dc = GetDC ( view->hWnd );
    if ( wglMakeCurrent ( dc, view->glctx ) == TRUE ) {
#endif

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

#ifdef need_refactor
                            glMatrixMode(GL_PROJECTION);
                            glLoadIdentity();

                            g3dcamera_project ( &view->cam, 0x00  );

                            glMatrixMode ( GL_MODELVIEW );
                            glLoadIdentity ( );

                            /*** preserve image aspect ratio ***/
                            view->cam.obj.sca.y = whRatio;

                            g3dobject_updateMatrix ( &view->cam.obj, view->mui->engine_flags );

                            g3dcamera_view ( &view->cam, 0x00  );

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
#endif

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

/* TOOD: replace NULL with engine */

    if ( seltool && ( seltool != tool ) && seltool->draw ) {
        seltool->draw ( seltool, gui->sce, &view->cam, NULL, engine_flags );
    }

    if ( tool && tool->draw ) {
        tool->draw ( tool, gui->sce, &view->cam, NULL, engine_flags );
    }

#ifdef __linux__
    } glXSwapBuffers ( view->dpy, view->win);

    XFlush ( view->dpy );
    XSync ( view->dpy, False );
#endif
#ifdef __MINGW32__
    } SwapBuffers ( dc );

      ReleaseDC ( view->hWnd, dc );
#endif
}

/******************************************************************************/
void m3duiview_sizeGL ( M3DUIVIEW *view, 
                        uint32_t   width, 
                        uint32_t   height ) {
#ifdef __linux__
    if ( glXMakeCurrent ( view->dpy,
                          view->win,
                          view->glctx ) == TRUE ) {
#endif
#ifdef __MINGW32__
    HDC dc = GetDC ( view->hWnd );
    if ( wglMakeCurrent ( dc, view->glctx ) == TRUE ) {
#endif

    glViewport ( 0, 0, width, height );

#ifdef __linux__
    }
#endif
#ifdef __MINGW32__
    } ReleaseDC ( view->hWnd, dc );
#endif
}

/******************************************************************************/
void m3duiview_initGL ( M3DUIVIEW *view ) {
    float clearColorf = ( float ) CLEARCOLOR / 255.0f;

#ifdef __linux__
    if ( glXMakeCurrent ( view->dpy,
                          view->win,
                          view->glctx ) == TRUE ) {
#endif
#ifdef __MINGW32__
    HDC dc = GetDC ( view->hWnd );
    if ( wglMakeCurrent ( dc, view->glctx ) == TRUE ) {
#endif

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

    m3duiview_init ( view );

#ifdef __linux__
    }
#endif
#ifdef __MINGW32__
    } ReleaseDC ( view->hWnd, dc );
#endif
}
