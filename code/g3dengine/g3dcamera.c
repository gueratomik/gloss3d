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
#include <g3dengine/g3dengine.h>

/******************************************************************************/
void g3dcamera_spin ( G3DCAMERA *cam, float diffz ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) cam;
    double ROTX[0x10];

    glPushMatrix ( );
    glLoadMatrixd ( obj->rmatrix );

    glRotatef     ( diffz, 0.0f, 0.0f, 1.0f );

    glGetDoublev  ( GL_MODELVIEW_MATRIX, ROTX );

    g3dcore_getMatrixRotation ( ROTX, &obj->rot );

    glPopMatrix ( );

    g3dobject_updateMatrix_r ( obj, 0x00 );
}

/******************************************************************************/
G3DCAMERA *g3dcamera_copy ( G3DCAMERA *cam,
                            uint64_t engine_flags ) {
    G3DOBJECT *objcam = ( G3DOBJECT * ) cam;
    G3DCAMERA *newcam = g3dcamera_new ( objcam->id, 
                                        objcam->name, 
                                        cam->focal, 
                                        cam->ratio, 
                                        cam->znear, 
                                        cam->zfar );

    return newcam;
}

/******************************************************************************/
float g3dcamera_getDistanceToCursor ( G3DCAMERA *cam, 
                                      G3DCURSOR *csr ) {
    G3DVECTOR origin = { 0.0f, 0.0f, 0.0f, 1.0f };
    G3DVECTOR campos;
    G3DVECTOR csrpos;
    G3DVECTOR distance;

    g3dvector_matrix ( &origin, ((G3DOBJECT*)cam)->wmatrix, &campos );

    g3dvector_matrix ( &origin, csr->matrix, &csrpos );

    distance.x = ( csrpos.x - campos.x );
    distance.y = ( csrpos.y - campos.y );
    distance.z = ( csrpos.z - campos.z );

    return g3dvector_length ( &distance );
}

/******************************************************************************/
static void g3dcamera_transform ( G3DCAMERA *cam, 
                                  uint64_t   engine_flags ) {

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
void g3dcamera_view ( G3DCAMERA *cam, 
                      uint64_t   engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) cam;
    G3DVECTOR vec;
    G3DVECTOR rot, pos;

    glLoadIdentity( );
    glMultMatrixd( obj->iwmatrix );
}

/******************************************************************************/
void g3dcamera_project ( G3DCAMERA *cam, 
                         uint64_t   engine_flags ) {
    if ( ((G3DOBJECT*)cam)->flags & CAMERAORTHOGRAPHIC ) {
        int VPX[0x10];
        float dist;

        glGetIntegerv ( GL_VIEWPORT, VPX );

        glOrtho ( ( -VPX[0x02] * cam->ortho.z ) + cam->ortho.x,
                  (  VPX[0x02] * cam->ortho.z ) + cam->ortho.x,
                  ( -VPX[0x03] * cam->ortho.z ) + cam->ortho.y,
                  (  VPX[0x03] * cam->ortho.z ) + cam->ortho.y, cam->znear, cam->zfar );
    } else {
        gluPerspective ( cam->focal, cam->ratio, cam->znear, cam->zfar );
    }

    glGetDoublev  ( GL_PROJECTION_MATRIX, cam->pmatrix );
    glGetIntegerv ( GL_VIEWPORT         , cam->vmatrix );
}

/******************************************************************************/
uint32_t g3dcamera_pickObject ( G3DCAMERA *cam, uint64_t engine_flags ) {
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

    g3dpick_setName (  ( uint64_t ) cam );

    for ( i = 0x00; i < 0x06; i++ ) {
        uint32_t v[0x04] = { index[i][0x00],
                             index[i][0x01],
                             index[i][0x02],
                             index[i][0x03] };
        uint32_t j;

        for ( j = 0x00; j < 0x04; j++ ) {
            uint32_t n = ( j + 0x01 ) % 0x04;

            g3dpick_drawLine ( pnt[v[j]][0x00], 
                               pnt[v[j]][0x01],
                               pnt[v[j]][0x02],
                               pnt[v[n]][0x00],
                               pnt[v[n]][0x01],
                               pnt[v[n]][0x02] );

            g3dpick_drawLine ( lns[v[j]][0x00], 
                               lns[v[j]][0x01],
                               lns[v[j]][0x02],
                               lns[v[n]][0x00],
                               lns[v[n]][0x01],
                               lns[v[n]][0x02] );
        }
    }
    return 0;
}

/******************************************************************************/
uint32_t g3dcamera_pick ( G3DCAMERA *cam, 
                          G3DCAMERA *curcam, 
                          uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) cam;

    if ( obj->type & OBJECTSELECTED ) {
        if ( engine_flags & VIEWOBJECT ) g3dcamera_pickObject   ( cam, engine_flags );
    } else {
        if ( engine_flags & VIEWOBJECT ) g3dcamera_pickObject   ( cam, engine_flags );
    }
    return 0;
}

/******************************************************************************/
uint32_t g3dcamera_draw ( G3DOBJECT *obj,
                          G3DCAMERA *curcam, 
                          uint64_t   engine_flags ) {
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

    if ( obj->flags & CAMERADOF ) {
        G3DVECTOR vec[0x04] = { { .x =  1.0f, .y =  1.0f, .z = -cam->dof.farBlur },
                                { .x =  1.0f, .y = -1.0f, .z = -cam->dof.farBlur },
                                { .x = -1.0f, .y = -1.0f, .z = -cam->dof.farBlur },
                                { .x = -1.0f, .y =  1.0f, .z = -cam->dof.farBlur } };

        float noBlurFactor   = ( cam->dof.farBlur ) ? cam->dof.noBlur   / cam->dof.farBlur : 0.0f,
              nearBlurFactor = ( cam->dof.farBlur ) ? cam->dof.nearBlur / cam->dof.farBlur : 0.0f;

        glBegin ( GL_QUADS );

        glColor3ub ( 0xFF, 0x00, 0xFF );
        glVertex3f (  vec[0].x * nearBlurFactor,  vec[0].y * nearBlurFactor, vec[0].z * nearBlurFactor );
        glVertex3f (  vec[1].x * nearBlurFactor,  vec[1].y * nearBlurFactor, vec[1].z * nearBlurFactor );
        glVertex3f (  vec[2].x * nearBlurFactor,  vec[2].y * nearBlurFactor, vec[2].z * nearBlurFactor );
        glVertex3f (  vec[3].x * nearBlurFactor,  vec[3].y * nearBlurFactor, vec[3].z * nearBlurFactor );

        glColor3ub ( 0x7F, 0x00, 0xFF );
        glVertex3f (  vec[0].x * noBlurFactor,  vec[0].y * noBlurFactor, vec[0].z * noBlurFactor );
        glVertex3f (  vec[1].x * noBlurFactor,  vec[1].y * noBlurFactor, vec[1].z * noBlurFactor );
        glVertex3f (  vec[2].x * noBlurFactor,  vec[2].y * noBlurFactor, vec[2].z * noBlurFactor );
        glVertex3f (  vec[3].x * noBlurFactor,  vec[3].y * noBlurFactor, vec[3].z * noBlurFactor );

        glColor3ub ( 0xFF, 0x00, 0xFF );
        glVertex3f (  vec[0].x,  vec[0].y, vec[0].z );
        glVertex3f (  vec[1].x,  vec[1].y, vec[1].z );
        glVertex3f (  vec[2].x,  vec[2].y, vec[2].z );
        glVertex3f (  vec[3].x,  vec[3].y, vec[3].z );

        glEnd ( );

        glBegin ( GL_LINES );

        glColor3ub ( 0xFF, 0x00, 0xFF );
        glVertex3f (     0.0f,     0.0f,     0.0f );
        glVertex3f ( vec[0].x, vec[0].y, vec[0].z );

        glVertex3f (     0.0f,     0.0f,     0.0f );
        glVertex3f ( vec[1].x, vec[1].y, vec[1].z );

        glVertex3f (     0.0f,     0.0f,     0.0f );
        glVertex3f ( vec[2].x, vec[2].y, vec[2].z );

        glVertex3f (     0.0f,     0.0f,     0.0f );
        glVertex3f ( vec[3].x, vec[3].y, vec[3].z );

        glEnd ( );
    }

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
void g3dcamera_setOrtho ( G3DCAMERA *cam,
                          uint32_t   width, 
                          uint32_t   height ) {
    ((G3DOBJECT*)cam)->flags |= CAMERAORTHOGRAPHIC;

    cam->width  = width;
    cam->height = height;
}

/******************************************************************************/
G3DCAMERA *g3dcamera_new ( uint32_t id,
                           char    *name, 
                           float    focal, 
                           float    ratio,
                           float    znear, 
                           float    zfar ) {

    G3DCAMERA *cam = ( G3DCAMERA * ) calloc ( 0x01, sizeof ( G3DCAMERA ) );
    G3DOBJECT *obj = ( G3DOBJECT * ) cam;

    if ( cam == NULL ) {
        fprintf ( stderr, "g3dcamera_new: calloc failed\n" );

        return NULL;
    }

    g3dobject_init ( obj, G3DCAMERATYPE, id, name, DRAWBEFORECHILDREN,
                                                   g3dcamera_draw,
                                                   g3dcamera_free,
                                                   g3dcamera_pick,
                                                   NULL,
                                     COPY_CALLBACK(g3dcamera_copy),
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                                   NULL );

    obj->transform = g3dcamera_transform;


    cam->focal = focal;
    cam->ratio = ratio;
    cam->znear = znear;
    cam->zfar  = zfar;

    cam->dof.nearBlur =  0.0f;
    cam->dof.noBlur   =  6.0f;
    cam->dof.farBlur  = 12.0f;
    cam->dof.radius   =    10;

    cam->ortho.x = 0.0f;
    cam->ortho.y = 0.0f;
    cam->ortho.z = 0.00750f;


    return cam;
}
