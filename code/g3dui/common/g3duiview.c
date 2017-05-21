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

#define CLEARCOLOR 100

/******************************************************************************/
void common_g3duiview_orbit ( G3DUIVIEW *view, G3DPIVOT *piv,
                                               int32_t x   , int32_t y, 
                                               int32_t xold, int32_t yold ) {

    g3dpivot_orbit ( piv, x, y, xold, yold );
}

/******************************************************************************/
void common_g3duiview_spin ( G3DUIVIEW *view, int32_t x, int32_t xold ) {
    G3DCAMERA *cam = view->cam;
    G3DOBJECT *obj = ( G3DOBJECT * ) cam;
    G3DVECTOR xvec = { 1.0f, 0.0f, 0.0f, 1.0f },
              yvec = { 0.0f, 1.0f, 0.0f, 1.0f },
              zvec = { 0.0f, 0.0f, 1.0f, 1.0f }, camx, camy, camz;
    G3DVECTOR ovec = { 0.0f, 0.0f, 0.0f, 1.0f }, camp;
    GLdouble MVX[0x10];

    obj->rot.z += ( xold - x );


    g3dobject_updateMatrix_r ( obj, 0x00 );
}

/******************************************************************************/
void common_g3duiview_zoom ( G3DUIVIEW *view, int32_t x, int32_t xold ) {
    G3DCAMERA *cam = view->cam;
    G3DOBJECT *obj = ( G3DOBJECT * ) cam;
    int difx = ( xold - x );

    cam->focal -= ( (float) ( difx ) / 20.0f );


    g3dobject_updateMatrix_r ( obj, 0x00 );
}

/******************************************************************************/
void common_g3duiview_moveSideward ( G3DUIVIEW *view, int32_t x   , 
                                                      int32_t y, 
                                                      int32_t xold, 
                                                      int32_t yold ) {
    G3DCAMERA *cam = view->cam;
    G3DOBJECT *obj = ( G3DOBJECT * ) cam;
    G3DVECTOR xvec = { 1.0f, 0.0f, 0.0f, 1.0f },
              yvec = { 0.0f, 1.0f, 0.0f, 1.0f },
              zvec = { 0.0f, 0.0f, 1.0f, 1.0f }, camx, camy, camz;
    float posx, posy, posz;
    int difx = ( xold - x ),
        dify = ( yold - y );

    g3dvector_matrix  ( &xvec, obj->rmatrix, &camx );
    g3dvector_matrix  ( &yvec, obj->rmatrix, &camy );

    obj->pos.x += ( camx.x * ( (float) ( difx ) / 20.0f ) );
    obj->pos.y += ( camx.y * ( (float) ( difx ) / 20.0f ) );
    obj->pos.z += ( camx.z * ( (float) ( difx ) / 20.0f ) );

    obj->pos.x -= ( camy.x * ( (float) ( dify ) / 20.0f ) );
    obj->pos.y -= ( camy.y * ( (float) ( dify ) / 20.0f ) );
    obj->pos.z -= ( camy.z * ( (float) ( dify ) / 20.0f ) );

    g3dobject_updateMatrix_r ( obj, 0x00 );
}


/******************************************************************************/
void common_g3duiview_moveForward ( G3DUIVIEW *view, int32_t x, 
                                                     int32_t xold ) {
    G3DCAMERA *cam = view->cam;
    G3DOBJECT *obj = ( G3DOBJECT * ) cam;
    G3DVECTOR xvec = { 1.0f, 0.0f, 0.0f, 1.0f },
              yvec = { 0.0f, 1.0f, 0.0f, 1.0f },
              zvec = { 0.0f, 0.0f, 1.0f, 1.0f }, camx, camy, camz;
    float posx, posy, posz;
    int difx = ( xold - x );

    g3dvector_matrix  ( &zvec, obj->rmatrix, &camz );

    obj->pos.x += ( camz.x * ( (float) ( difx ) / 20.0f ) );
    obj->pos.y += ( camz.y * ( (float) ( difx ) / 20.0f ) );
    obj->pos.z += ( camz.z * ( (float) ( difx ) / 20.0f ) );

    /*** if orthogonal view, dont let the camera ***/
    /*** go negative ***/
    if ( cam->focal == 2.0f ) {
        if ( obj->pos.z > cam->zfar  ) obj->pos.z = cam->zfar;
        if ( obj->pos.z < cam->znear ) obj->pos.z = cam->znear;
    }

    g3dobject_updateMatrix_r ( obj, 0x00 );
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
int common_g3duiview_getCurrentButton ( G3DUIVIEW *view, int x, int y ) {
    int i;

    for ( i = 0x00; i < NBVIEWBUTTON; i++ ) {
        G3DUIRECTANGLE *rec = &view->rec[i];

        if ( ( ( x >= rec->x ) && ( x <= ( rec->x + rec->width  ) ) ) &&
             ( ( y >= rec->y ) && ( y <= ( rec->y + rec->height ) ) ) ) {
            return i;
        }
    }

    return -1;
}

/******************************************************************************/
void common_g3duiview_init ( G3DUIVIEW *view, uint32_t width,
                                              uint32_t height ) {
    common_g3duiview_resize ( view, width, height );

    view->buttonID = -1;
}

/******************************************************************************/
void common_g3duiview_resize ( G3DUIVIEW *view, uint32_t width, 
                                                uint32_t height ) {
    int i, xpos = ( width - BUTTONSIZE ), brd = 0x02;

    /*** set rectangle position for each button ***/
    for ( i = 0x00; i < NBVIEWBUTTON; i++, xpos = ( xpos - BUTTONSIZE - brd ) ) {
        view->rec[i].x      = xpos;
        view->rec[i].y      = 0x00;
        view->rec[i].width  = BUTTONSIZE;
        view->rec[i].height = BUTTONSIZE;
    }

    view->glrec.x      = 0x00;
    view->glrec.y      = BUTTONSIZE;
    view->glrec.width  = width;
    view->glrec.height = height - BUTTONSIZE;
}

/******************************************************************************/
void common_g3duiview_sizeGL ( G3DUIVIEW *view, uint32_t width, 
                                                uint32_t height ) {
    G3DCAMERA *cam = view->cam;

    glViewport ( 0, 0, width, height );

    /*** retrieve the viewport matrix, we'll use it for rendering ***/
    glGetIntegerv ( GL_VIEWPORT, cam->vmatrix );

    /*** Really, you don't want a divide by zero ***/
    cam->ratio = ( height ) ? ( double ) width / height : 1.0f;
}

/******************************************************************************/
void common_g3duiview_initGL ( G3DUIVIEW *view ) {
    G3DCAMERA *cam = view->cam;
    float      clearColorf = ( float ) CLEARCOLOR / 255.0f;

    /*** we need to update the camera's matrix here because we need and ***/
    /*** OpenGL context for matrix operations ***/
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

/******************************************************************************/
void common_g3duiview_showRenderingArea ( G3DUI   *gui,
                                          uint32_t engine_flags ) {
    R3DRENDERSETTINGS *rsg = gui->currsg;
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
void common_g3duiview_showGL ( G3DUI        *gui, 
                               G3DSCENE     *sce,
                               G3DCAMERA    *cam,
                               G3DMOUSETOOL *mou,
                               uint32_t      current,
                               uint32_t      engine_flags ) {
    int VPX[0x04];
    G3DVECTOR vec = { 0.0f, 0.0f, 0.0f, 1.0f };
    G3DOBJECT *selobj = g3dscene_getSelectedObject ( sce );
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
    R3DRENDERSETTINGS *rsg = gui->currsg;

    glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

    glGetIntegerv ( GL_VIEWPORT, VPX );

    glMatrixMode ( GL_PROJECTION );
    glLoadIdentity ( );

    glOrtho ( VPX[0], VPX[2], VPX[3], VPX[1], 0.0f, 1.0f );

    glMatrixMode ( GL_MODELVIEW );
    glLoadIdentity ( );

    common_g3duiview_showRenderingArea ( gui, engine_flags );

    glDepthMask ( GL_FALSE );

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glDisable ( GL_LIGHTING );
    glEnable ( GL_COLOR_MATERIAL );
    glColor3ub ( 0xFF, 0xFF, 0xFF );

    if ( rsg ) {
        if ( rsg->background.mode & BACKGROUND_IMAGE ) {
            if ( sysinfo->backgroundImage ) {
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
                glBindTexture ( GL_TEXTURE_2D, sysinfo->backgroundImage->id );


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
    glDepthMask ( GL_TRUE );

    glMatrixMode ( GL_PROJECTION );
    glLoadIdentity ( );

    if ( cam ) g3dcamera_project ( cam, engine_flags );

    glMatrixMode ( GL_MODELVIEW );
    glLoadIdentity ( );

    if ( cam ) {
        G3DVECTOR pos = { 0.0f, 0.0f, 0.0f, 1.0f };
        double TMPX[0x10];

        if ( selobj ) {
            if ( ( engine_flags & VIEWOBJECT ) ||
                 ( engine_flags & VIEWAXIS   ) ) {
                g3dvector_matrix ( &pos, selobj->wmatrix, &cam->pivot );
            }

            if ( selobj->type & EDITABLE ) {
                G3DMESH *mes= ( G3DMESH * ) selobj;

                if ( engine_flags & VIEWVERTEX ) {
                    g3dmesh_getSelectedVerticesWorldPosition ( mes, 
                                                               &cam->pivot );
                }

                if ( engine_flags & VIEWFACE ) {
                    g3dmesh_getSelectedFacesWorldPosition ( mes, 
                                                            &cam->pivot );
                }

                if ( engine_flags & VIEWEDGE ) {
                    g3dmesh_getSelectedEdgesWorldPosition ( mes, 
                                                            &cam->pivot );
                }
            }


        }

        g3dcamera_view ( cam, engine_flags );
    }

    g3dobject_draw ( ( G3DOBJECT * ) sce, cam, engine_flags /*| VIEWNORMALS*/ );

    /*** draw the mouse tool only in the current workspace window ***/
    if ( current ) {
        if ( mou && mou->draw ) {
            mou->draw ( mou, sce, engine_flags );
        }
    }
}
