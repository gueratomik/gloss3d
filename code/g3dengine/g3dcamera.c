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
#include <g3dengine/g3dengine.h>

/******************************************************************************/
G3DCAMERA *g3dcamera_copy ( G3DCAMERA *cam,
                            uint32_t   engine_flags ) {
    G3DOBJECT *objcam = ( G3DOBJECT * ) cam;
    G3DCAMERA *newcam = g3dcamera_new ( objcam->id, 
                                        objcam->name, 
                                        cam->focal, 
                                        cam->ratio, 
                                        cam->znear, 
                                        cam->zfar );

    memcpy ( &newcam->pivot, &cam->pivot, sizeof ( cam->pivot ) );

    newcam->grid = cam->grid;

    return newcam;
}

/******************************************************************************/
/*** Copy camera position, rotation, scaling and focal ***/
void g3dcamera_import ( G3DCAMERA *dst, G3DCAMERA *src ) {
    g3dobject_importTransformations ( ( G3DOBJECT * ) dst, 
                                      ( G3DOBJECT * ) src );

    dst->focal = src->focal;
}

/******************************************************************************/
void g3dcamera_grid2D_r ( float ubeg, float uend,
                          float vbeg, float vend, float deltamax ) {
    int i;

    for ( i = ubeg; i <= uend; i++ ) {
        glVertex3f ( i, vbeg, 0.0f );
        glVertex3f ( i, vend, 0.0f );
    }

    for ( i = vbeg; i <= vend; i++ ) {
        glVertex3f ( ubeg, i, 0.0f );
        glVertex3f ( uend, i, 0.0f );
    }
}

/******************************************************************************/
void g3dcamera_gridZX ( G3DCAMERA *cam, uint32_t flags ) {
    double zmax, xmax, zmin, xmin, yval;
    GLdouble MVX[0x10], PJX[0x10];
    GLint    VPX[0x04];
    GLint x1, y1, x2, y2, zbeg, xbeg, zend, xend;
    int i;

    glGetDoublev  ( GL_MODELVIEW_MATRIX , MVX );
    glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
    glGetIntegerv ( GL_VIEWPORT         , VPX );

    x1 = VPX[0x00];
    y1 = VPX[0x01];
    x2 = VPX[0x02] - 0x01;
    y2 = VPX[0x03] - 0x01;

    gluUnProject ( x1, y1, 0.0f, MVX, PJX, VPX, &xmin, &yval, &zmax );
    gluUnProject ( x2, y1, 0.0f, MVX, PJX, VPX, &xmax, &yval, &zmax );
    gluUnProject ( x2, y2, 0.0f, MVX, PJX, VPX, &xmax, &yval, &zmin );
    gluUnProject ( x1, y2, 0.0f, MVX, PJX, VPX, &xmin, &yval, &zmin );

    zbeg = ( GLint ) zmin;
    zend = ( GLint ) zmax;
    xbeg = ( GLint ) xmin;
    xend = ( GLint ) xmax;

    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable   ( GL_LIGHTING );
    glBegin ( GL_LINES );

    if ( zbeg < zend ) {
        for ( i = zbeg; i <= zend; i++ ) {
            if ( i % 0x05 ) {
                glColor3ub ( 0x80, 0x80, 0x80 );
            } else {
                glColor3ub ( 0x00, 0x00, 0x00 );
            }

            glVertex3f ( xmin, yval, i );
            glVertex3f ( xmax, yval, i );
        }
    }

    if ( xbeg < xend ) {
        for ( i = xbeg; i <= xend; i++ ) {
            if ( i % 0x05 ) {
                glColor3ub ( 0x80, 0x80, 0x80 );
            } else {
                glColor3ub ( 0x00, 0x00, 0x00 );
            }
   
            glVertex3f ( i, yval, zmin );
            glVertex3f ( i, yval, zmax );
        }
    }

    glEnd ( );

    glPopAttrib ( );
}

/******************************************************************************/
void g3dcamera_gridYZ ( G3DCAMERA *cam, uint32_t flags ) {
    double ymax, zmax, ymin, zmin, xval;
    GLdouble MVX[0x10], PJX[0x10];
    GLint    VPX[0x04];
    GLint x1, y1, x2, y2, ybeg, zbeg, yend, zend;
    int i;

    glGetDoublev  ( GL_MODELVIEW_MATRIX , MVX );
    glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
    glGetIntegerv ( GL_VIEWPORT         , VPX );

    x1 = VPX[0x00];
    y1 = VPX[0x01];
    x2 = VPX[0x02];
    y2 = VPX[0x03];

    gluUnProject ( x1, y1, 0.0f, MVX, PJX, VPX, &xval, &ymax, &zmin );
    gluUnProject ( x2, y1, 0.0f, MVX, PJX, VPX, &xval, &ymax, &zmax );
    gluUnProject ( x2, y2, 0.0f, MVX, PJX, VPX, &xval, &ymin, &zmax );
    gluUnProject ( x1, y2, 0.0f, MVX, PJX, VPX, &xval, &ymin, &zmin );

    ybeg = ( GLint ) ymax;
    yend = ( GLint ) ymin;
    zbeg = ( GLint ) zmin;
    zend = ( GLint ) zmax;

    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable   ( GL_LIGHTING );
    glBegin ( GL_LINES );

    if ( ybeg < yend ) {
        for ( i = ybeg; i <= yend; i++ ) {
            if ( i % 0x05 ) {
                glColor3ub ( 0x80, 0x80, 0x80 );
            } else {
                glColor3ub ( 0x00, 0x00, 0x00 );
            }
   
            glVertex3f ( xval, i, zmin );
            glVertex3f ( xval, i, zmax );
        }
    }

    if ( zbeg < zend ) {
        for ( i = zbeg; i <= zend; i++ ) {
            if ( i % 0x05 ) {
                glColor3ub ( 0x80, 0x80, 0x80 );
            } else {
                glColor3ub ( 0x00, 0x00, 0x00 );
            }
   
            glVertex3f ( xval, ymin, i );
            glVertex3f ( xval, ymax, i );
        }
    }

    glEnd ( );

    glPopAttrib ( );
}

/******************************************************************************/
void g3dcamera_gridXY ( G3DCAMERA *cam, uint32_t flags ) {
    double xmax, ymax, xmin, ymin, zval;
    GLdouble MVX[0x10], PJX[0x10];
    GLint    VPX[0x04];
    GLint x1, y1, x2, y2, xbeg, ybeg, xend, yend;
    int i;

    glGetDoublev  ( GL_MODELVIEW_MATRIX , MVX );
    glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
    glGetIntegerv ( GL_VIEWPORT         , VPX );

    x1 = VPX[0x00];
    y1 = VPX[0x01];
    x2 = VPX[0x02];
    y2 = VPX[0x03];

    gluUnProject ( x1, y1, 0.0f, MVX, PJX, VPX, &xmin, &ymax, &zval );
    gluUnProject ( x2, y1, 0.0f, MVX, PJX, VPX, &xmax, &ymax, &zval );
    gluUnProject ( x2, y2, 0.0f, MVX, PJX, VPX, &xmax, &ymin, &zval );
    gluUnProject ( x1, y2, 0.0f, MVX, PJX, VPX, &xmin, &ymin, &zval );

    xbeg = ( GLint ) xmin;
    xend = ( GLint ) xmax;
    ybeg = ( GLint ) ymax;
    yend = ( GLint ) ymin;

    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable   ( GL_LIGHTING );
    glBegin ( GL_LINES );

    if ( xbeg < xend ) {
        for ( i = xbeg; i <= xend; i++ ) {
            if ( i % 0x05 ) {
                glColor3ub ( 0x80, 0x80, 0x80 );
            } else {
                glColor3ub ( 0x00, 0x00, 0x00 );
            }
   
            glVertex3f ( i, ymin, zval );
            glVertex3f ( i, ymax, zval );
        }
    }

    if ( ybeg < yend ) {
        for ( i = ybeg; i <= yend; i++ ) {
            if ( i % 0x05 ) {
                glColor3ub ( 0x80, 0x80, 0x80 );
            } else {
                glColor3ub ( 0x00, 0x00, 0x00 );
            }
   
            glVertex3f ( xmin, i, zval );
            glVertex3f ( xmax, i, zval );
        }
    }

    glEnd ( );

    glPopAttrib ( );
}

/******************************************************************************/
void g3dcamera_grid3D ( G3DCAMERA *cam, uint32_t flags ) {
    float x1, x2, z1, z2;
    int i;

    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable   ( GL_LIGHTING );
    glBegin ( GL_LINES );

    x1 = -10.0f;
    z1 = -10.0f;
    z2 =  10.0f;

    for ( i = 0x00; i < 0x15; i++ ) {
        if ( i % 0x05 ) {
            glColor3ub ( 0x80, 0x80, 0x80 );
        } else {
            glColor3ub ( 0x00, 0x00, 0x00 );
        }

        glVertex3f ( x1, 0.0f, z1 );
        glVertex3f ( x1, 0.0f, z2 );

        x1 = ( x1 + 1.0f );
    }

    z1 = -10.0f;
    x1 = -10.0f;
    x2 =  10.0f;
    for ( i = 0x00; i < 0x15; i++ ) {
        if ( i % 0x05 ) {
            glColor3ub ( 0x80, 0x80, 0x80 );
        } else {
            glColor3ub ( 0x00, 0x00, 0x00 );
        }

        glVertex3f ( x1, 0.0f, z1 );
        glVertex3f ( x2, 0.0f, z1 );

        z1 = ( z1 + 1.0f );
    }

    glEnd ( );

    glPopAttrib ( );
}

/******************************************************************************/
void g3dcamera_setPivot ( G3DCAMERA *cam, float x, float y, float z ) {
    cam->pivot.x = x;
    cam->pivot.y = y;
    cam->pivot.z = z;
}

/******************************************************************************/
void g3dcamera_updateViewingMatrix ( G3DCAMERA *cam, uint32_t flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) cam;
    G3DVECTOR viewpos = { cam->viewpos.x, 
                          cam->viewpos.y,
                          cam->viewpos.z, 1.0f };
    double ROTX[0x10];
    float angle;

    /*** find camera position ***/
    glLoadIdentity ( );
    glRotatef ( cam->viewrot.z, 0.0f, 0.0f, 1.0f );
    glRotatef ( cam->viewrot.y, 0.0f, 1.0f, 0.0f );
    glRotatef ( cam->viewrot.x, 1.0f, 0.0f, 0.0f );
    glGetDoublev ( GL_MODELVIEW_MATRIX, ROTX );

    g3dvector_matrix ( &viewpos, ROTX, &obj->pos );

    g3dobject_updateMatrix_r ( obj, flags );
}

/******************************************************************************/
void g3dcamera_view ( G3DCAMERA *cam, uint32_t flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) cam;
    G3DVECTOR vec;
    G3DVECTOR rot, pos;

    glLoadIdentity( );
    glMultMatrixd( obj->iwmatrix );

    if ( ( flags & HIDEGRID ) == 0x00 ) {
        if ( cam->grid ) cam->grid ( cam, flags );
    }
}

/******************************************************************************/
void g3dcamera_project ( G3DCAMERA *cam, uint32_t flags ) {
    if ( ((G3DOBJECT*)cam)->flags & CAMERAORTHO ) {
        glOrtho ( 0.0f, cam->width,
                        cam->height, 0.0f, 0.0f, 1.0f );
    } else {
        if ( cam->focal == 2.0f ) {
            static G3DVECTOR origin = { 0.0f, 0.0f, 0.0f, 1.0f }, camwpos;
            G3DOBJECT *camobj = ( G3DOBJECT * ) cam;
            int PJX[0x10];
            float dist;

            glGetIntegerv ( GL_VIEWPORT, PJX );

            g3dvector_matrix ( &origin, camobj->wmatrix, &camwpos );

            if ( cam->grid == g3dcamera_gridXY ) dist = camwpos.z;
            if ( cam->grid == g3dcamera_gridYZ ) dist = camwpos.x;
            if ( cam->grid == g3dcamera_gridZX ) dist = camwpos.y;


            glOrtho ( -PJX[0x02] * 0.00125f * dist,
                       PJX[0x02] * 0.00125f * dist,
                      -PJX[0x03] * 0.00125f * dist,
                       PJX[0x03] * 0.00125f * dist, cam->znear, cam->zfar );
        } else {
            gluPerspective ( cam->focal, cam->ratio, cam->znear, cam->zfar );
        }
    }

    glGetDoublev  ( GL_PROJECTION_MATRIX, cam->pmatrix );
    glGetIntegerv ( GL_VIEWPORT         , cam->vmatrix );
}

/******************************************************************************/
uint32_t g3dcamera_draw ( G3DOBJECT *obj, G3DCAMERA *curcam, uint32_t flags ) {
    G3DCAMERA *cam = ( G3DCAMERA * ) obj;
    GLfloat pnt[0x08][0x03] = { {  0.25,  0.5,  0.5 },
                                {  0.25, -0.5,  0.5 },
                                { -0.25, -0.5,  0.5 },
                                { -0.25,  0.5,  0.5 },

                                {  0.25, -0.5, -0.5 },
                                {  0.25,  0.5, -0.5 },
                                { -0.25,  0.5, -0.5 },
                                { -0.25, -0.5, -0.5 } } ;

    GLfloat lns[0x08][0x03] = { {  0.1,  0.1, -0.5 },
                                {  0.1, -0.1, -0.5 },
                                { -0.1, -0.1, -0.5 },
                                { -0.1,  0.1, -0.5 },

                                {  0.25, -0.25, -0.7 },
                                {  0.25,  0.25, -0.7 },
                                { -0.25,  0.25, -0.7 },
                                { -0.25, -0.25, -0.7 } } ;

    uint32_t index[0x06][0x04] = { { 0x00, 0x01, 0x02, 0x03 },
                                   { 0x04, 0x05, 0x06, 0x07 },
                                   { 0x03, 0x02, 0x07, 0x06 },
                                   { 0x05, 0x04, 0x01, 0x00 },
                                   { 0x01, 0x04, 0x07, 0x02 },
                                   { 0x05, 0x00, 0x03, 0x06 } };
    uint32_t i;

    /*** dont draw itself because the current cam is always at 0,0,0 ***/
    if ( cam == curcam ) return 0x00;

    glPushAttrib( GL_ALL_ATTRIB_BITS );
    glDisable   ( GL_LIGHTING );
    glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );
    glColor3ub  ( 0x00, 0xFF, 0x00 );

    glBegin ( GL_QUADS );
    for ( i = 0x00; i < 0x06; i++ ) {
        uint32_t v0 = index[i][0x00],
                 v1 = index[i][0x01],
                 v2 = index[i][0x02],
                 v3 = index[i][0x03];

        glVertex3fv ( pnt[v0] );
        glVertex3fv ( pnt[v1] );
        glVertex3fv ( pnt[v2] );
        glVertex3fv ( pnt[v3] );

        glVertex3fv ( lns[v0] );
        glVertex3fv ( lns[v1] );
        glVertex3fv ( lns[v2] );
        glVertex3fv ( lns[v3] );
    }
    glEnd ( );

    glPopAttrib ( );

    return 0x00;
}

/******************************************************************************/
void g3dcamera_free ( G3DOBJECT *obj ) {
    /*G3DCAMERA *cam = ( G3DCAMERA * ) obj;*/

    /*** Is the Undo-Redo manager ***/
    /*** going to handle freeing  ***/
    /*** memory ? I have to think ***/
    /*** about it ***/
}

/******************************************************************************/
void g3dcamera_pick ( G3DOBJECT *obj,  G3DCAMERA *curcam, uint32_t flags ) {
    /*G3DCAMERA *cam = ( G3DCAMERA * ) obj;*/

    /*** Is the Undo-Redo manager ***/
    /*** going to handle freeing  ***/
    /*** memory ? I have to think ***/
    /*** about it ***/
}

/******************************************************************************/
void g3dcamera_setGrid ( G3DCAMERA *cam, void (*grid)(G3DCAMERA *, uint32_t) ){
    cam->grid  = grid;
}

/******************************************************************************/
void g3dcamera_setOrtho ( G3DCAMERA *cam,
                          uint32_t   width, 
                          uint32_t   height ) {
    ((G3DOBJECT*)cam)->flags |= CAMERAORTHO;

    cam->width  = width;
    cam->height = height;
}

/******************************************************************************/
G3DCAMERA *g3dcamera_new ( uint32_t id, char *name, 
                           float focal, float ratio,
                           float znear, float zfar ) {

    G3DCAMERA *cam = ( G3DCAMERA * ) calloc ( 0x01, sizeof ( G3DCAMERA ) );
    G3DOBJECT *obj = ( G3DOBJECT * ) cam;

    if ( cam == NULL ) {
        fprintf ( stderr, "g3dcamera_new: calloc failed\n" );

        return NULL;
    }

    g3dobject_init ( obj, G3DCAMERATYPE, id, name, DRAWBEFORECHILDREN,
                                                   g3dcamera_draw,
                                                   g3dcamera_free,
                                                   NULL,
                                                   NULL,
                                     COPY_CALLBACK(g3dcamera_copy),
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL );

    cam->focal = focal;
    cam->ratio = ratio;
    cam->znear = znear;
    cam->zfar  = zfar;

    g3dvector_init ( &cam->pivot, 0.0f, 0.0f, 0.0f, 1.0f );


    return cam;
}
