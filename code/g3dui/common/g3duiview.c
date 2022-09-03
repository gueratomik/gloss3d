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
uint64_t g3duiview_setShadingCbk ( G3DUIVIEW *view,
                                   char      *shading ) {
    G3DUI *gui = view->gui;
#ifdef __linux__
    glXMakeCurrent ( view->dpy, view->win, view->glctx );
#endif
#ifdef __MINGW32__
    HWND hWnd = GDK_WINDOW_HWND ( gdkwin );
    HDC dc = GetDC ( hWnd );

    wglMakeCurrent ( dc, view->glctx );

    ReleaseDC ( hWnd, dc );
#endif

    if ( strcmp ( shading, SHADINGMENU_GOURAUD ) == 0x00 ) {
        glEnable      ( GL_LIGHTING );
        glShadeModel  ( GL_SMOOTH );
        glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );

        /*** The no texture flags is VIEW dependent, not GUI dependent ***/
        view->engine_flags &= (~NOTEXTURE);
        view->mode          = GLVIEWGOURAUD;
    }

    if ( strcmp ( shading, SHADINGMENU_FLAT ) == 0x00 ) {
        glEnable      ( GL_LIGHTING );
        glShadeModel  ( GL_FLAT );
        glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );

        /*** The no texture flags is VIEW dependent, not GUI dependent ***/
        view->engine_flags &= (~NOTEXTURE);
        view->mode          = GLVIEWFLAT;
    }

    if ( strcmp ( shading, SHADINGMENU_WIREFRAME ) == 0x00 ) {
        glDisable     ( GL_LIGHTING );
        glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );

        /*g3dui_disableTextureImages ( gui );*/

        /*** The no texture flags is VIEW dependent, not GUI dependent ***/
        view->engine_flags |= NOTEXTURE;
        view->mode          = GLVIEWWIREFRAME;
    }

    return REDRAWVIEW | REDRAWVIEWMENU;
}

/******************************************************************************/
static G3DCAMERA *getCamera ( G3DUIVIEW *view ) {
    G3DSCENE *sce = view->gui->sce;

    /*** Note : scene does not exist at first when the widget is realized ***/
    if ( sce ) {
        if ( g3dscene_isObjectReferred ( sce, ( G3DOBJECT * ) view->cam ) ) {
            return view->cam;
        }
    }

    return view->defcam;
}

/******************************************************************************/
uint64_t g3duiview_orbit ( G3DUIVIEW *view,
                           G3DPIVOT  *piv,
                           float      diffx, 
                           float      diffy ) {
    G3DCAMERA *cam = getCamera ( view );

    g3dpivot_orbit ( piv, diffx, diffy );

    if ( view->defcam != cam ) return REDRAWCOORDS;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiview_spin ( G3DUIVIEW *view, 
                          float      diffx ) {
    G3DCAMERA *cam = getCamera ( view );
    G3DOBJECT *obj = ( G3DOBJECT * ) cam;
    G3DVECTOR xvec = { 1.0f, 0.0f, 0.0f, 1.0f },
              yvec = { 0.0f, 1.0f, 0.0f, 1.0f },
              zvec = { 0.0f, 0.0f, 1.0f, 1.0f }, camx, camy, camz;
    G3DVECTOR ovec = { 0.0f, 0.0f, 0.0f, 1.0f }, camp;
    GLdouble MVX[0x10];

    g3dcamera_spin ( cam, diffx );

    if ( view->defcam != cam ) return REDRAWCOORDS;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiview_zoom ( G3DUIVIEW *view,
                          float      diffx ) {
    G3DCAMERA *cam = getCamera ( view );
    G3DOBJECT *obj = ( G3DOBJECT * ) cam;

    cam->focal -= ( (float) ( diffx ) / 20.0f );

    g3dobject_updateMatrix_r ( obj, 0x00 );

    if ( view->defcam != cam ) return REDRAWCOORDS;

    return 0x00;
}

/******************************************************************************/
uint64_t g3duiview_moveSideward ( G3DUIVIEW *view,
                                  float diffx, 
                                  float diffy ) {
    G3DCAMERA *cam = getCamera ( view );
    G3DOBJECT *obj = ( G3DOBJECT * ) cam;
    G3DVECTOR xvec = { 1.0f, 0.0f, 0.0f, 1.0f },
              yvec = { 0.0f, 1.0f, 0.0f, 1.0f },
              zvec = { 0.0f, 0.0f, 1.0f, 1.0f }, camx, camy, camz;
    float posx, posy, posz;

    if ( obj->flags & CAMERAORTHOGRAPHIC ) {
        cam->ortho.x += ( diffx * cam->ortho.z );
        cam->ortho.y -= ( diffy * cam->ortho.z );
    } else {
        g3dvector_matrix  ( &xvec, obj->rmatrix, &camx );
        g3dvector_matrix  ( &yvec, obj->rmatrix, &camy );

        obj->pos.x += ( camx.x * ( (float) ( diffx ) / 20.0f ) );
        obj->pos.y += ( camx.y * ( (float) ( diffx ) / 20.0f ) );
        obj->pos.z += ( camx.z * ( (float) ( diffx ) / 20.0f ) );

        obj->pos.x -= ( camy.x * ( (float) ( diffy ) / 20.0f ) );
        obj->pos.y -= ( camy.y * ( (float) ( diffy ) / 20.0f ) );
        obj->pos.z -= ( camy.z * ( (float) ( diffy ) / 20.0f ) );
    }

    g3dobject_updateMatrix_r ( obj, 0x00 );

    if ( view->defcam != cam ) return REDRAWCOORDS;

    return 0x00;
}


/******************************************************************************/
uint64_t g3duiview_moveForward ( G3DUIVIEW *view, 
                                 float      diffx ) {
    G3DCAMERA *cam = getCamera ( view );
    G3DOBJECT *obj = ( G3DOBJECT * ) cam;
    G3DVECTOR xvec = { 1.0f, 0.0f, 0.0f, 1.0f },
              yvec = { 0.0f, 1.0f, 0.0f, 1.0f },
              zvec = { 0.0f, 0.0f, 1.0f, 1.0f }, camx, camy, camz;
    float posx, posy, posz;

    if ( obj->flags & CAMERAORTHOGRAPHIC ) {
        cam->ortho.z += ( diffx * 0.000005f );

        if ( cam->ortho.z < 0.0f ) cam->ortho.z = 0.0f;
    } else {
        g3dvector_matrix  ( &zvec, obj->rmatrix, &camz );

        obj->pos.x += ( camz.x * ( (float) ( diffx ) / 40.0f ) );
        obj->pos.y += ( camz.y * ( (float) ( diffx ) / 40.0f ) );
        obj->pos.z += ( camz.z * ( (float) ( diffx ) / 40.0f ) );

        /*** if orthogonal view, dont let the camera ***/
        /*** go negative ***/
        if ( cam->focal == 2.0f ) {
            if ( obj->pos.z > cam->zfar  ) obj->pos.z = cam->zfar;
            if ( obj->pos.z < cam->znear ) obj->pos.z = cam->znear;
        }
    }

    g3dobject_updateMatrix_r ( obj, 0x00 );

    if ( view->defcam != cam ) return REDRAWCOORDS;

    return 0x00;
}

/******************************************************************************/
static void g3duiview_init2D ( G3DUIVIEW *view ) {
    float spec[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    glDisable ( GL_LIGHTING );
    glEnable ( GL_LIGHT0 );
    /*glEnable ( GL_COLOR_MATERIAL );*/

    glLightfv ( GL_LIGHT0, GL_SPECULAR, ( const float * ) spec );

    glEnable ( GL_DEPTH_TEST );

    /*** I still have to understand this below ***/
    /*** I got it from Internet for avoiding Z-Fighting ***/
    glPolygonOffset ( -1.0f, -1.0f );
    glEnable(GL_POLYGON_OFFSET_FILL);

    glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );

    view->mode = GLVIEWWIREFRAME;
}

/******************************************************************************/
static void g3duiview_init3D ( G3DUIVIEW *view ) {
    float spec[] = { 1.0f, 1.0f, 1.0f, 0.0f };

    glEnable ( GL_LIGHTING );
    glEnable ( GL_LIGHT0 );
    /*glEnable ( GL_COLOR_MATERIAL );*/

    glLightfv ( GL_LIGHT0, GL_SPECULAR, ( const float * ) spec );

    glEnable ( GL_DEPTH_TEST );

    /*** I still have to understand this below ***/
    /*** I got it from Internet for avoiding Z-Fighting ***/
    glPolygonOffset ( 1.0f, 1.0f );
    glEnable(GL_POLYGON_OFFSET_FILL);

    glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
    /*glLightModeli ( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );*/

    view->mode = GLVIEWGOURAUD;
}

/******************************************************************************/
/* Find the current button ID according to x and y coordinates. This helsp us */
/* determine which button we are on. We don't use Motif button here because   */
/* we need to track the mouse after a click, something I think, will be easier*/
/* to achieve that way. I might be wrong though :-/.                          */
/******************************************************************************/
void g3duiview_pressButton ( G3DUIVIEW *view, 
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
void g3duiview_releaseButton ( G3DUIVIEW *view ) {
    view->pressedButtonID = -1;
}

/******************************************************************************/
void g3duiview_init ( G3DUIVIEW *view,
                      uint32_t   width,
                      uint32_t   height,
                      uint32_t   menuHeight ) {
    g3duiview_resize ( view, width, height, menuHeight );

    view->pressedButtonID = -1;
}

/******************************************************************************/
void g3duiview_resize ( G3DUIVIEW *view, 
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
void g3duiview_sizeGL ( G3DUIVIEW *view, 
                        uint32_t   width, 
                        uint32_t   height ) {
    G3DCAMERA *cam = getCamera ( view );

    if ( cam ) {
        glViewport ( 0, 0, width, height );

        /*** retrieve the viewport matrix, we'll use it for rendering ***/
        glGetIntegerv ( GL_VIEWPORT, cam->vmatrix );

        /*** Really, you don't want a divide by zero ***/
        cam->ratio = ( height ) ? ( double ) width / height : 1.0f;
    }
}

/******************************************************************************/
void g3duiview_initGL ( G3DUIVIEW *view ) {
    G3DCAMERA *cam = getCamera ( view );

    if ( cam ) {
        float      clearColorf = ( float ) CLEARCOLOR / 255.0f;

        g3dobject_updateMatrix_r (  cam, 0x00 );

        /*** Set clear color for the OpenGL Window ***/
        glClearColor ( clearColorf, clearColorf, clearColorf, 1.0f );

        /*if ( vi->depth < 0x20 ) {
            glEnable ( GL_DITHER );
        }*/

        /*** Temp, this must be done by the camera in the future ***/
        if ( cam && ((G3DOBJECT*)cam)->id == 0x00 ) {
            g3duiview_init3D ( view );
        } else {
            g3duiview_init2D ( view );
        }
    }
}

/******************************************************************************/
void g3duiview_useSelectedCamera ( G3DUIVIEW *view, 
                                   G3DCAMERA *cam ) {
    cam->ratio = view->cam->ratio;

    view->cam = cam;

    g3dscene_addReferredObject ( view->gui->sce, ( G3DOBJECT * ) cam );
}

/******************************************************************************/
void g3duiview_showRenderingArea ( G3DUIVIEW *view,
                                   uint64_t engine_flags ) {
    G3DUI *gui = view->gui;
    Q3DSETTINGS *rsg = gui->currsg;
    float renderRatio = ( float ) rsg->output.width / rsg->output.height;
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
    int32_t deltaWidth;
    int VPX[0x04], i;

    glGetIntegerv ( GL_VIEWPORT, VPX );

    deltaWidth = ( VPX[0x02] - ( VPX[0x03] * renderRatio ) ) / 2;

    sysinfo->renderRectangle[0x00].x = 0x00 + deltaWidth;
    sysinfo->renderRectangle[0x00].y = 0x00;

    sysinfo->renderRectangle[0x01].x = VPX[0x03] * renderRatio + deltaWidth;
    sysinfo->renderRectangle[0x01].y = 0x00;

    sysinfo->renderRectangle[0x02].x = VPX[0x03] * renderRatio + deltaWidth;
    sysinfo->renderRectangle[0x02].y = VPX[0x03];

    sysinfo->renderRectangle[0x03].x = 0x00 + deltaWidth;
    sysinfo->renderRectangle[0x03].y = VPX[0x03];

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glDisable ( GL_LIGHTING );
    glColor3ub ( CLEARCOLOR + 0x08, 
                 CLEARCOLOR + 0x08, 
                 CLEARCOLOR + 0x08 );
    glBegin ( GL_LINE_LOOP );
    for ( i = 0x00; i < 0x04; i++ ) {
        glVertex3f ( sysinfo->renderRectangle[i].x, 
                     sysinfo->renderRectangle[i].y, 0.0f );
    }
    glEnd ( );
    glPopAttrib ( );
}

/******************************************************************************/
void g3duiview_showGL ( G3DUIVIEW    *view,
                        uint64_t      engine_flags ) {
    G3DUI *gui = view->gui;
    G3DSCENE *sce = gui->sce;
    G3DCAMERA *cam = view->cam;
    G3DMOUSETOOL  *mou = gui->mou;

    engine_flags |= gui->engine_flags;

    if ( sce ) {
        int VPX[0x04];
        G3DVECTOR vec = { 0.0f, 0.0f, 0.0f, 1.0f };
        G3DOBJECT *selobj = g3dscene_getSelectedObject ( sce );
        G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
        Q3DSETTINGS *rsg = gui->currsg;
        G3DRGBA backgroundRGBA;
        uint32_t ret;
        /*** This helps the drawarea to determine if it should draw mouse tools ***/
        /*** for example (we don't draw mousetool on all window widget. ***/
        uint32_t current = ( view == gui->currentView ) ? 0x01 : 0x00;

        sysinfo->sce = sce; /* for debugging purpose */

        if ( gui->playLock ) {
             /*** Force disabling real time subdivision ***/
            gui->engine_flags |= ONGOINGANIMATION;
        }

        if ( ( engine_flags & NOBACKGROUNDIMAGE  ) == 0x00 ) {
            g3drgba_fromLong ( &backgroundRGBA, rsg->background.color );
        } else {
            /*** Note: CLEARCOLOR is defined in r3d.h" */
            backgroundRGBA.r = backgroundRGBA.g = backgroundRGBA.b = CLEARCOLOR;
        }

        /*** Set clear color for the OpenGL Window ***/
        glClearColor ( ( float ) backgroundRGBA.r / 255.0f, 
                       ( float ) backgroundRGBA.g / 255.0f,
                       ( float ) backgroundRGBA.b / 255.0f, 1.0f );

        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glLightModeli ( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );

        glGetIntegerv ( GL_VIEWPORT, VPX );

        glMatrixMode ( GL_PROJECTION );
        glLoadIdentity ( );

        glOrtho ( VPX[0], VPX[2], VPX[3], VPX[1], 0.0f, 1.0f );

        glMatrixMode ( GL_MODELVIEW );
        glLoadIdentity ( );

        g3duiview_showRenderingArea ( view, engine_flags );

        glDepthMask ( GL_FALSE );

        if ( ( engine_flags & NOBACKGROUNDIMAGE  ) == 0x00 ) {
            glPushAttrib ( GL_ALL_ATTRIB_BITS );
            glDisable ( GL_LIGHTING );
            glEnable ( GL_COLOR_MATERIAL );
            glColor3ub ( 0xFF, 0xFF, 0xFF );

            if ( rsg ) {
                if ( rsg->background.mode & BACKGROUND_IMAGE ) {
                    if ( rsg->background.image ) {
                        float renderRatio  = ( float ) rsg->output.width / 
                                                       rsg->output.height;
                        float color[] = { 0.25f, 0.25f, 0.25f, 1.0f };
                        G3DVECTOR uv[0x04] = { { .x =     0, .y =     0, .z = 0.0f },
                                               { .x =  1.0f, .y =     0, .z = 0.0f },
                                               { .x =  1.0f, .y =  1.0f, .z = 0.0f },
                                               { .x =     0, .y =  1.0f, .z = 0.0f } }; 
                        double MVX[0x10], PJX[0x10];
                        int VPX[0x04], i;

                        glGetIntegerv ( GL_VIEWPORT, VPX );

                        glEnable      ( GL_TEXTURE_2D );
                        glBindTexture ( GL_TEXTURE_2D, rsg->background.image->id );


                        glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_BORDER );
                        glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_BORDER );
                        glTexParameterfv( GL_TEXTURE_2D, GL_TEXTURE_BORDER_COLOR, color );

                        glBegin ( GL_QUADS );
                        for ( i = 0x00; i < 0x04; i++ ) {
                            glTexCoord2f ( uv[i].x,  uv[i].y );
                            glVertex3f  ( sysinfo->renderRectangle[i].x, 
                                          sysinfo->renderRectangle[i].y, 0.0f );
                        }
                        glEnd ( );
                        glDisable ( GL_TEXTURE_2D );
                    }
                }
            }

            glPopAttrib ( );
        }

        glDepthMask ( GL_TRUE );

        glMatrixMode ( GL_PROJECTION );
        glLoadIdentity ( );

        if ( cam ) g3dcamera_project ( cam, engine_flags );

        glMatrixMode ( GL_MODELVIEW );
        glLoadIdentity ( );

        if ( cam ) {
            g3dcamera_view ( cam, engine_flags );

            if ( ( engine_flags & HIDEGRID ) == 0x00 ) {
                if ( view->cam != view->defcam ) {
                    g3dcore_grid3D ( engine_flags );
                } else {
                    if ( view->grid ) view->grid ( engine_flags );
                }
            }
        }

        ret = g3dobject_draw_r ( ( G3DOBJECT * ) sce, cam, engine_flags /*| VIEWNORMALS*/ );

        if ( ret & DRAW_LIGHTON ) {
            glDisable ( GL_LIGHT0 );
        } else {
            glEnable ( GL_LIGHT0 );
        }

        g3dscene_draw ( ( G3DOBJECT * ) sce, cam, engine_flags );

        /*** draw the mouse tool only in the current workspace window ***/
        if ( current ) {
            if ( mou && mou->draw ) {
                mou->draw ( mou, sce, engine_flags );
            }
        }

        if ( gui->playLock ) {
            /*** Re-enable real time subdivision ***/
            gui->engine_flags &= (~ONGOINGANIMATION);
        }
    }
}
