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
/*** I cannot include setjmp in g3dengine/g3dengine.h because png.h complains about it ***/
#include <setjmp.h>
#include <g3dengine/g3dengine.h>


/******************************************************************************/
/*https://en.wikipedia.org/wiki/Conversion_between_quaternions_and_Euler_angles*/
/* Note: yaw (z), pitch (y), roll (x) */
void g3dcore_eulerToQuaternion ( G3DDOUBLEVECTOR *angles, G3DQUATERNION *qout ) {
    double yaw   = angles->z;
    double pitch = angles->y;
    double roll  = angles->x;
    /* Abbreviations for the various angular functions */
    double cy = cos ( yaw   * 0.5f );
    double sy = sin ( yaw   * 0.5f );
    double cp = cos ( pitch * 0.5f );
    double sp = sin ( pitch * 0.5f );
    double cr = cos ( roll  * 0.5f );
    double sr = sin ( roll  * 0.5f );

    qout->w     = cr * cp * cy + sr * sp * sy;
    qout->roll  = sr * cp * cy - cr * sp * sy;
    qout->pitch = cr * sp * cy + sr * cp * sy;
    qout->yaw   = cr * cp * sy - sr * sp * cy;

    g3dquaternion_normalize ( qout );
}

/******************************************************************************/
void g3dcore_rotateMatrixf( float *matrix,
                            float aDeg,
                            float x,
                            float y,
                            float z ) {
    float aRad = aDeg * M_PI / 180.0f;
    float c = cos( aRad );
    float s = sin( aRad );
    float invc = ( 1.0f - c );
    float xsq = ( x * x );
    float ysq = ( y * y );
    float zsq = ( z * z );

    float xy = ( x * y );
    float yz = ( y * z );
    float zx = ( z * x );

    float xs = ( x * s );
    float ys = ( y * s );
    float zs = ( z * s );
    float resultMatrix[0x10];
    /*** row major representation from OpenGL documentation
 { ( xsq * invc ) + c , ( xy  * invc ) - zs, ( zx  * invc ) + ys, 0.0f,
   ( xy  * invc ) + zs, ( ysq * invc ) + c , ( yz  * invc ) - xs, 0.0f,
   ( zx  * invc ) - ys, ( yz  * invc ) + xs, ( zsq * invc ) + c , 0.0f,
   0.0f               , 0.0f               , 0.0f               , 1.0f };
    ***/
    float rotationMatrix[0x10];

    /*** Note: OpenGL Matrices are column major ***/
    rotationMatrix[0x00] = ( xsq * invc ) + c;
    rotationMatrix[0x01] = ( xy  * invc ) + zs;
    rotationMatrix[0x02] = ( zx  * invc ) - ys;
    rotationMatrix[0x03] = 0.0f;

    rotationMatrix[0x04] = ( xy  * invc ) - zs;
    rotationMatrix[0x05] = ( ysq * invc ) + c;
    rotationMatrix[0x06] = ( yz  * invc ) + xs;
    rotationMatrix[0x07] = 0.0f;

    rotationMatrix[0x08] = ( zx  * invc ) + ys;
    rotationMatrix[0x09] = ( yz  * invc ) - xs;
    rotationMatrix[0x0A] = ( zsq * invc ) + c;
    rotationMatrix[0x0B] = 0.0f;

    rotationMatrix[0x0C] = 0.0f;
    rotationMatrix[0x0D] = 0.0f;
    rotationMatrix[0x0E] = 0.0f;
    rotationMatrix[0x0F] = 1.0f;

    g3dcore_multMatrixf( matrix, rotationMatrix, resultMatrix ); /* revised */

    memcpy( matrix, resultMatrix, sizeof( resultMatrix ) );
}

/******************************************************************************/
void g3dcore_convertMatrixftod( float *inMatrix, double *outMatrix  ) {
    uint32_t i;

    for( i = 0; i < 0x10; i++ ) {
        outMatrix[i] = inMatrix[i];
    }
}

/******************************************************************************/
void g3dcore_unprojectf( double  x,
                         double  y,
                         double  z,
                         float  *MVX,
                         float  *PJX,
                         int    *VPX,
                         double *winx,
                         double *winy,
                         double *winz ) {
    double MVXd[0x10];
    double PJXd[0x10];

    g3dcore_convertMatrixftod( MVX, MVXd );
    g3dcore_convertMatrixftod( PJX, PJXd );

    gluUnProject ( x,
                   y,
                   z,
                   MVXd,
                   PJXd,
                   VPX,
                   winx,
                   winy,
                   winz );
}

/******************************************************************************/
void g3dcore_projectf( double  x,
                       double  y,
                       double  z,
                       float  *MVX,
                       float  *PJX,
                       int    *VPX,
                       double *winx,
                       double *winy,
                       double *winz ) {
    double MVXd[0x10];
    double PJXd[0x10];

    g3dcore_convertMatrixftod( MVX, MVXd );
    g3dcore_convertMatrixftod( PJX, PJXd );

    gluProject ( x,
                 y,
                 z,
                 MVXd,
                 PJXd,
                 VPX,
                 winx,
                 winy,
                 winz );
}

/******************************************************************************/
void g3dcore_rotateMatrixd( double *matrix,
                            float a,
                            float x,
                            float y,
                            float z ) {
    double c = cos( a );
    double s = sin( a );
    double invc = ( 1.0f - c );
    double xsq = ( x * x );
    double ysq = ( y * y );
    double zsq = ( z * z );

    double xy = ( x * y );
    double yz = ( y * z );
    double zx = ( z * x );

    double xs = ( x * s );
    double ys = ( y * s );
    double zs = ( z * s );
    double resultMatrix[0x10];
    /*** row major representation from OpenGL documentation
 { ( xsq * invc ) + c , ( xy  * invc ) - zs, ( zx  * invc ) + ys, 0.0f,
   ( xy  * invc ) + zs, ( ysq * invc ) + c , ( yz  * invc ) - xs, 0.0f,
   ( zx  * invc ) - ys, ( yz  * invc ) + xs, ( zsq * invc ) + c , 0.0f,
   0.0f               , 0.0f               , 0.0f               , 1.0f };
    ***/
    double rotationMatrix[0x10];

    /*** Note: OpenGL Matrices are column major ***/
    rotationMatrix[0x00] = ( xsq * invc ) + c;
    rotationMatrix[0x01] = ( xy  * invc ) + zs;
    rotationMatrix[0x02] = ( zx  * invc ) - ys;
    rotationMatrix[0x03] = 0.0f;

    rotationMatrix[0x04] = ( xy  * invc ) - zs;
    rotationMatrix[0x05] = ( ysq * invc ) + c;
    rotationMatrix[0x06] = ( yz  * invc ) + xs;
    rotationMatrix[0x07] = 0.0f;

    rotationMatrix[0x08] = ( zx  * invc ) + ys;
    rotationMatrix[0x09] = ( yz  * invc ) - xs;
    rotationMatrix[0x0A] = ( zsq * invc ) + c;
    rotationMatrix[0x0B] = 0.0f;

    rotationMatrix[0x0C] = 0.0f;
    rotationMatrix[0x0D] = 0.0f;
    rotationMatrix[0x0E] = 0.0f;
    rotationMatrix[0x0F] = 1.0f;

    g3dcore_multMatrixd( matrix, rotationMatrix, resultMatrix ); /* revised */

    memcpy( matrix, resultMatrix, sizeof( resultMatrix ) );
}

/******************************************************************************/
void g3dcore_scaleMatrixf( float *matrix, float x, float y, float z ) {
    /*** note: row and column major are identical ***/
    float scalingMatrix[] = { x   , 0.0f, 0.0f, 0.0f,
                              0.0f, y   , 0.0f, 0.0f, 
                              0.0f, 0.0f, z   , 0.0f, 
                              0.0f, 0.0f, 0.0f, 1.0f };
    float resultMatrix[0x10];

    g3dcore_multMatrixf( matrix, scalingMatrix, resultMatrix ); /* revised */

    memcpy( matrix, resultMatrix, sizeof( resultMatrix ) );
}

/******************************************************************************/
void g3dcore_translateMatrixf( float *matrix, float x, float y, float z ) {
    float translationMatrix[0x10];
    float resultMatrix[0x10];

    /*** row major representation from OpenGL documentation
   { 1.0f, 0.0f, 0.0f, x,
     0.0f, 1.0f, 0.0f, y, 
     0.0f, 0.0f, 1.0f, z, 
     0.0f, 0.0f, 0.0f, 1.0f };
    ***/

    /*** Note: OpenGL Matrices are column major ***/
    translationMatrix[0x00] = 1.0f;
    translationMatrix[0x01] = 0.0f;
    translationMatrix[0x02] = 0.0f;
    translationMatrix[0x03] = 0.0f;

    translationMatrix[0x04] = 0.0f;
    translationMatrix[0x05] = 1.0f;
    translationMatrix[0x06] = 0.0f;
    translationMatrix[0x07] = 0.0f;

    translationMatrix[0x08] = 0.0f;
    translationMatrix[0x09] = 0.0f;
    translationMatrix[0x0A] = 1.0f;
    translationMatrix[0x0B] = 0.0f;

    translationMatrix[0x0C] = x;
    translationMatrix[0x0D] = y;
    translationMatrix[0x0E] = z;
    translationMatrix[0x0F] = 1.0f;

    g3dcore_multMatrixf( matrix, translationMatrix, resultMatrix ); /* revised */

    memcpy( matrix, resultMatrix, sizeof( resultMatrix ) );
}

/******************************************************************************/
void g3dcore_eulerInDegreesToQuaternion ( G3DDOUBLEVECTOR *angles, 
                                          G3DQUATERNION   *qout ) {
    G3DDOUBLEVECTOR rad = { .x = angles->x * M_PI / 180.0f,
                            .y = angles->y * M_PI / 180.0f,
                            .z = angles->z * M_PI / 180.0f,
                            .w = 1.0f };

    g3dcore_eulerToQuaternion ( &rad, qout );
}

/******************************************************************************/
void g3dcore_gridZX ( G3DCAMERA *cam,
                      G3DENGINE *engine, 
                      uint64_t engine_flags ) {
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
void g3dcore_gridYZ ( G3DCAMERA *cam,
                      G3DENGINE *engine, 
                      uint64_t engine_flags ) {
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
void g3dcore_gridXY ( G3DCAMERA *cam,
                      G3DENGINE *engine, 
                      uint64_t engine_flags ) {
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
void g3dcore_grid3D ( G3DCAMERA *cam,
                      G3DENGINE *engine, 
                      uint64_t engine_flags ) {
    int mvpMatrixLocation = glGetUniformLocation( engine->coloredShaderProgram,
                                                  "mvpMatrix" );
    float mvp[0x10];
    float x1, x2, z1, z2;
    int i;

    glUseProgram( engine->coloredShaderProgram );

    /*** the matrix by which vertices coords are transformed ***/
    g3dcore_multMatrixf( cam->pmatrix
                       , cam->obj.inverseWorldMatrix, mvp );

    glUniformMatrix4fv( mvpMatrixLocation, 1, GL_FALSE, mvp );

    x1 = -10.0f;
    z1 = -10.0f;
    z2 =  10.0f;

    for ( i = 0x00; i < 0x15; i++ ) {
        SHADERVERTEX vertices[0x02];

        vertices[0x00].pos.x = x1;
        vertices[0x00].pos.y = 0.0f;
        vertices[0x00].pos.z = z1;

        vertices[0x01].pos.x = x1;
        vertices[0x01].pos.y = 0.0f;
        vertices[0x01].pos.z = z2;

        if ( i % 0x05 ) {
            vertices[0x00].col.r = vertices[0x01].col.r = 0.5f;
            vertices[0x00].col.g = vertices[0x01].col.g = 0.5f;
            vertices[0x00].col.b = vertices[0x01].col.b = 0.5f;
        } else {
            vertices[0x00].col.r = vertices[0x01].col.r = 0.0f;
            vertices[0x00].col.g = vertices[0x01].col.g = 0.0f;
            vertices[0x00].col.b = vertices[0x01].col.b = 0.0f;
        }

        g3dengine_drawLine( engine,
                            vertices,
                            0x00,
                            engine_flags );

        x1 = ( x1 + 1.0f );
    }

    z1 = -10.0f;
    x1 = -10.0f;
    x2 =  10.0f;

    for ( i = 0x00; i < 0x15; i++ ) {
        SHADERVERTEX vertices[0x02];

        vertices[0x00].pos.x = x1;
        vertices[0x00].pos.y = 0.0f;
        vertices[0x00].pos.z = z1;

        vertices[0x01].pos.x = x2;
        vertices[0x01].pos.y = 0.0f;
        vertices[0x01].pos.z = z1;

        if ( i % 0x05 ) {
            vertices[0x00].col.r = vertices[0x01].col.r = 0.5f;
            vertices[0x00].col.g = vertices[0x01].col.g = 0.5f;
            vertices[0x00].col.b = vertices[0x01].col.b = 0.5f;
        } else {
            vertices[0x00].col.r = vertices[0x01].col.r = 0.0f;
            vertices[0x00].col.g = vertices[0x01].col.g = 0.0f;
            vertices[0x00].col.b = vertices[0x01].col.b = 0.0f;
        }

        g3dengine_drawLine( engine,
                            vertices,
                            0x00,
                            engine_flags );

        z1 = ( z1 + 1.0f );
    }

    glUseProgram( 0 );
}

/******************************************************************************/
G3DGLOBALS *g3dcore_getGlobals ( ) {
    static G3DGLOBALS globals;
    static int init;

    if ( init == 0x00 ) {
        globals.lightID = GL_LIGHT0;
 
        init = 0x01;
    }

    return &globals;
}

/******************************************************************************/
#ifdef __linux__
uint32_t g3dcore_getNumberOfCPUs ( ) {
    uint32_t nbcpu = 0x00;
    char str[0x100];
    FILE *fp;

    if ( ( fp =  fopen ( "/proc/cpuinfo", "r" ) ) == NULL ) {
        fprintf ( stderr, "Could not open /proc/cpuinfo\n" );

        /** There's at leats one CPU. How do you think your computer runs ?! **/
        return 0x01;
    }

    while ( feof ( fp ) == 0x00 ) {
        fgets ( str, sizeof ( str ), fp );

        if ( strstr ( str, "processor" ) ) {
            nbcpu++;
        }

        memset ( str, 0x00, sizeof ( str ) );
    }

    fclose ( fp );

    return nbcpu;
}
#endif

#ifdef __MINGW32__
uint32_t g3dcore_getNumberOfCPUs ( ) {
    SYSTEM_INFO sysinfo;

    GetSystemInfo ( &sysinfo );

    return sysinfo.dwNumberOfProcessors;
}
#endif

/******************************************************************************/
char *g3dcore_strclone ( char *str ) {
    uint32_t len = strlen ( str ) + 0x01;
    char *clone  = malloc ( len );

    memcpy ( clone, str, len );


    return clone;
}

/******************************************************************************/
void g3dcore_drawXYCircle ( float    radius, 
                            uint64_t engine_flags ) {
    g3dcore_drawCircle ( ORIENTATIONXY, radius, engine_flags );
}

/******************************************************************************/
void g3dcore_drawYZCircle ( float    radius, 
                            uint64_t engine_flags ) {
    g3dcore_drawCircle ( ORIENTATIONYZ, radius, engine_flags );
}

/******************************************************************************/
void g3dcore_drawZXCircle ( float    radius, 
                            uint64_t engine_flags ) {
    g3dcore_drawCircle ( ORIENTATIONZX, radius, engine_flags );
}

/******************************************************************************/
void g3dcore_drawCircle ( uint32_t orientation,
                          float    radius,
                          uint64_t engine_flags ) {
    static G3DVECTOR p[16] = { { .x =  1.0f          , .y =  0.0f           },
                               { .x =  0.92387953251f, .y =  0.38268343236f },
                               { .x =  0.70710678118f, .y =  0.70710678118f },
                               { .x =  0.38268343236f, .y =  0.92387953251f },
                               { .x =  0.0f          , .y =  1.0f           },
                               { .x = -0.38268343236f, .y =  0.92387953251f },
                               { .x = -0.70710678118f, .y =  0.70710678118f },
                               { .x = -0.92387953251f, .y =  0.38268343236f },
                               { .x = -1.0f          , .y =  0.0f           },
                               { .x = -0.92387953251f, .y = -0.38268343236f },
                               { .x = -0.70710678118f, .y = -0.70710678118f },
                               { .x = -0.38268343236f, .y = -0.92387953251f },
                               { .x =  0.0f          , .y = -1.0f           },
                               { .x =  0.38268343236f, .y = -0.92387953251f },
                               { .x =  0.70710678118f, .y = -0.70710678118f },
                               { .x =  0.92387953251f, .y = -0.38268343236f } };
    uint32_t i;

    glBegin ( GL_LINE_LOOP );

    switch ( orientation ) {
        case ORIENTATIONXY :
            for ( i = 0x00; i < 0x10; i++ ) {
                glVertex3f ( p[i].x * radius, p[i].y * radius, 0.0f );
            } 
        break;

        case ORIENTATIONYZ :
            for ( i = 0x00; i < 0x10; i++ ) {
                glVertex3f ( 0.0f, p[i].x * radius, p[i].y * radius );
            } 
        break;

        case ORIENTATIONZX :
            for ( i = 0x00; i < 0x10; i++ ) {
                glVertex3f ( p[i].y * radius, 0.0f, p[i].x * radius );
            } 
        break;
    }

    glEnd ( );
}

/******************************************************************************/
uint32_t g3dcore_getNextPowerOfTwo ( uint32_t number ) {
    uint32_t rank = 0x00;
    uint32_t poweroftwo;

    while ( number > ( poweroftwo = ( 0x00000001 << rank++ ) ) );


    return poweroftwo;
}

/******************************************************************************/
/*** Thanks to:                                                             ***/
/** http://download.blender.org/source/chest/blender_2.03_tree/jpeg/jerror.c***/
/*** User deltheil from http://stackoverflow.com/questions/20383815/        ***/
/*** Torsten Martinsen http://www.hardenbergh.org/jch/code/trivo/writejpg.c ***/
/******************************************************************************/
struct g3dcore_error_mgr {
    struct jpeg_error_mgr pub;	/* "public" fields */
    jmp_buf setjmp_buffer;	/* for return to caller */
};

/******************************************************************************/
void g3dcore_jpeg_error_exit ( j_common_ptr cinfo ) {
    struct g3dcore_error_mgr *gerr = ( struct g3dcore_error_mgr * ) cinfo->err;

    /*** Display the ouput message ***/
    (*cinfo->err->output_message) (cinfo);

    jpeg_abort_decompress ( ( j_decompress_ptr ) cinfo );

    jpeg_destroy ( cinfo );

    /*** Return control to the setjmp point ***/
    longjmp ( gerr->setjmp_buffer, 0x01 );
}

/******************************************************************************/
void g3dcore_loadJpeg ( const char *filename, uint32_t      *width,
                                              uint32_t      *height,
                                              uint32_t      *depth,
                                              unsigned char **data ) {
    struct jpeg_decompress_struct cinfo;
    struct g3dcore_error_mgr gerr;
    uint32_t bytesperline;
    FILE *fsrc;

    (*width)  = 0x00;
    (*height) = 0x00;
    (*depth)  = 0x00;
    (*data)   = NULL;

    if ( ( fsrc = fopen ( filename, "rb" ) ) == NULL ) {
        fprintf ( stderr, "g3dcore_loadJpeg(): cannot open %s\n", filename );

        return;
    }

    cinfo.err = jpeg_std_error ( ( struct jpeg_error_mgr * ) &gerr );

    gerr.pub.error_exit = g3dcore_jpeg_error_exit;

    if ( setjmp ( gerr.setjmp_buffer ) ) {
        /* If we get here, the JPEG code has signaled an error.
        * We need to clean up the JPEG object, close the input file, and return.
        */
        jpeg_destroy_decompress(&cinfo);
        fclose ( fsrc );

        return;
    }

    jpeg_create_decompress ( &cinfo );

    /*** read from file ***/
    jpeg_stdio_src ( &cinfo, fsrc );

    /*** get image info ***/

    jpeg_read_header ( &cinfo, TRUE );

    if ( cinfo.out_color_space == JCS_GRAYSCALE ) {
        fprintf ( stderr, "Grayscale jpeg unsupported\n!" );

        return;
    }

    jpeg_start_decompress ( &cinfo );

    bytesperline = cinfo.output_width * cinfo.output_components;

    (*data) = malloc ( cinfo.output_height * bytesperline );

    (*width)  = cinfo.output_width;
    (*height) = cinfo.output_height;
    (*depth)  = cinfo.out_color_components;

    while ( cinfo.output_scanline < cinfo.output_height ) {
        unsigned char *rowp[0x01];

        rowp[0x00] = (*data) + ( bytesperline * cinfo.output_scanline );

        jpeg_read_scanlines ( &cinfo, rowp, 0x01 );
    }

    jpeg_finish_decompress ( &cinfo );

    jpeg_destroy_decompress ( &cinfo ); 

    printf ( "%s loaded\n", filename );

    fclose ( fsrc );
}

/******************************************************************************/
void g3dcore_writeJpeg ( const char *filename, uint32_t      width,
                                               uint32_t      height,
                                               uint32_t      depth,
                                               unsigned char *data ) {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[0x01]; /* pointer to JSAMPLE row[s] */
    int row_stride;     /* physical row width in image buffer */
    FILE *fdst;

    cinfo.err = jpeg_std_error ( &jerr );
    jpeg_create_compress ( &cinfo );

    if ( ( fdst = fopen ( filename, "wb") ) == NULL ) {
        fprintf ( stderr, "Could not open %s for writing\n", filename );

        return;
    }

    jpeg_stdio_dest ( &cinfo, fdst );
 
    cinfo.image_width      = width;
    cinfo.image_height     = height;
    cinfo.input_components = 0x03;
    cinfo.in_color_space   = JCS_RGB;

    jpeg_set_defaults ( &cinfo );
    /*set the quality [0..100]  */
    jpeg_set_quality ( &cinfo, 100, TRUE );
    jpeg_start_compress ( &cinfo, TRUE );

    row_stride = width * 0x03; /* JSAMPLEs per row in image_buffer */

    while ( cinfo.next_scanline < cinfo.image_height ) {
        row_pointer[0] = &data[cinfo.next_scanline * row_stride];

        jpeg_write_scanlines ( &cinfo, ( JSAMPARRAY ) &row_pointer, 0x01 );
    }

    jpeg_finish_compress ( &cinfo );

    fclose ( fdst );

    jpeg_destroy_compress ( &cinfo );
}

/******************************************************************************/
void g3dcore_symmetryMatrixd ( double *matrix, uint32_t orientation ) {
    matrix[0x00] = 1.0f;
    matrix[0x01] = 0.0f;
    matrix[0x02] = 0.0f;
    matrix[0x03] = 0.0f;
    matrix[0x04] = 0.0f;
    matrix[0x05] = 1.0f;
    matrix[0x06] = 0.0f;
    matrix[0x07] = 0.0f;
    matrix[0x08] = 0.0f;
    matrix[0x09] = 0.0f;
    matrix[0x0A] = 1.0f;
    matrix[0x0B] = 0.0f;
    matrix[0x0C] = 0.0f;
    matrix[0x0D] = 0.0f;
    matrix[0x0E] = 0.0f;
    matrix[0x0F] = 1.0f;

    if ( orientation == ORIENTATIONZX ) {
        matrix[0x05] = -1.0f;
    }

    if ( orientation == ORIENTATIONXY ) {
        matrix[0x0A] = -1.0f;
    }

    if ( orientation == ORIENTATIONYZ ) {
        matrix[0x00] = -1.0f;
    }
}

/******************************************************************************/
void g3dcore_symmetryMatrixf ( float *matrix, uint32_t orientation ) {
    matrix[0x00] = 1.0f;
    matrix[0x01] = 0.0f;
    matrix[0x02] = 0.0f;
    matrix[0x03] = 0.0f;
    matrix[0x04] = 0.0f;
    matrix[0x05] = 1.0f;
    matrix[0x06] = 0.0f;
    matrix[0x07] = 0.0f;
    matrix[0x08] = 0.0f;
    matrix[0x09] = 0.0f;
    matrix[0x0A] = 1.0f;
    matrix[0x0B] = 0.0f;
    matrix[0x0C] = 0.0f;
    matrix[0x0D] = 0.0f;
    matrix[0x0E] = 0.0f;
    matrix[0x0F] = 1.0f;

    if ( orientation == ORIENTATIONZX ) {
        matrix[0x05] = -1.0f;
    }

    if ( orientation == ORIENTATIONXY ) {
        matrix[0x0A] = -1.0f;
    }

    if ( orientation == ORIENTATIONYZ ) {
        matrix[0x00] = -1.0f;
    }
}

/******************************************************************************/
void g3dcore_transposeMatrixf ( float *inp, float *out ) {
    float res[0x10];

    res[0x00] = inp[0x00];
    res[0x01] = inp[0x04];
    res[0x02] = inp[0x08];
    res[0x03] = inp[0x0C];

    res[0x04] = inp[0x01];
    res[0x05] = inp[0x05];
    res[0x06] = inp[0x09];
    res[0x07] = inp[0x0D];

    res[0x08] = inp[0x02];
    res[0x09] = inp[0x06];
    res[0x0A] = inp[0x0A];
    res[0x0B] = inp[0x0E];

    res[0x0C] = inp[0x03];
    res[0x0D] = inp[0x07];
    res[0x0E] = inp[0x0B];
    res[0x0F] = inp[0x0F];

    memcpy( out, res, sizeof( res ) );
}

/******************************************************************************/
void g3dcore_transposeMatrixd ( double *inp, double *out ) {
    double res[0x10];

    res[0x00] = inp[0x00];
    res[0x01] = inp[0x04];
    res[0x02] = inp[0x08];
    res[0x03] = inp[0x0C];

    res[0x04] = inp[0x01];
    res[0x05] = inp[0x05];
    res[0x06] = inp[0x09];
    res[0x07] = inp[0x0D];

    res[0x08] = inp[0x02];
    res[0x09] = inp[0x06];
    res[0x0A] = inp[0x0A];
    res[0x0B] = inp[0x0E];

    res[0x0C] = inp[0x03];
    res[0x0D] = inp[0x07];
    res[0x0E] = inp[0x0B];
    res[0x0F] = inp[0x0F];

    memcpy( out, res, sizeof( res ) );
}

/******************************************************************************/
void g3dcore_invertMatrixf ( float *m, float *invOut ) {
    float inv[16], det;
    int i;

    inv[0] = m[5]  * m[10] * m[15] - 
             m[5]  * m[11] * m[14] - 
             m[9]  * m[6]  * m[15] + 
             m[9]  * m[7]  * m[14] +
             m[13] * m[6]  * m[11] - 
             m[13] * m[7]  * m[10];

    inv[4] = -m[4]  * m[10] * m[15] + 
              m[4]  * m[11] * m[14] + 
              m[8]  * m[6]  * m[15] - 
              m[8]  * m[7]  * m[14] - 
              m[12] * m[6]  * m[11] + 
              m[12] * m[7]  * m[10];

    inv[8] = m[4]  * m[9] * m[15] - 
             m[4]  * m[11] * m[13] - 
             m[8]  * m[5] * m[15] + 
             m[8]  * m[7] * m[13] + 
             m[12] * m[5] * m[11] - 
             m[12] * m[7] * m[9];

    inv[12] = -m[4]  * m[9] * m[14] + 
               m[4]  * m[10] * m[13] +
               m[8]  * m[5] * m[14] - 
               m[8]  * m[6] * m[13] - 
               m[12] * m[5] * m[10] + 
               m[12] * m[6] * m[9];

    inv[1] = -m[1]  * m[10] * m[15] + 
              m[1]  * m[11] * m[14] + 
              m[9]  * m[2] * m[15] - 
              m[9]  * m[3] * m[14] - 
              m[13] * m[2] * m[11] + 
              m[13] * m[3] * m[10];

    inv[5] = m[0]  * m[10] * m[15] - 
             m[0]  * m[11] * m[14] - 
             m[8]  * m[2] * m[15] + 
             m[8]  * m[3] * m[14] + 
             m[12] * m[2] * m[11] - 
             m[12] * m[3] * m[10];

    inv[9] = -m[0]  * m[9] * m[15] + 
              m[0]  * m[11] * m[13] + 
              m[8]  * m[1] * m[15] - 
              m[8]  * m[3] * m[13] - 
              m[12] * m[1] * m[11] + 
              m[12] * m[3] * m[9];

    inv[13] = m[0]  * m[9] * m[14] - 
              m[0]  * m[10] * m[13] - 
              m[8]  * m[1] * m[14] + 
              m[8]  * m[2] * m[13] + 
              m[12] * m[1] * m[10] - 
              m[12] * m[2] * m[9];

    inv[2] = m[1]  * m[6] * m[15] - 
             m[1]  * m[7] * m[14] - 
             m[5]  * m[2] * m[15] + 
             m[5]  * m[3] * m[14] + 
             m[13] * m[2] * m[7] - 
             m[13] * m[3] * m[6];

    inv[6] = -m[0]  * m[6] * m[15] + 
              m[0]  * m[7] * m[14] + 
              m[4]  * m[2] * m[15] - 
              m[4]  * m[3] * m[14] - 
              m[12] * m[2] * m[7] + 
              m[12] * m[3] * m[6];

    inv[10] = m[0]  * m[5] * m[15] - 
              m[0]  * m[7] * m[13] - 
              m[4]  * m[1] * m[15] + 
              m[4]  * m[3] * m[13] + 
              m[12] * m[1] * m[7] - 
              m[12] * m[3] * m[5];

    inv[14] = -m[0]  * m[5] * m[14] + 
               m[0]  * m[6] * m[13] + 
               m[4]  * m[1] * m[14] - 
               m[4]  * m[2] * m[13] - 
               m[12] * m[1] * m[6] + 
               m[12] * m[2] * m[5];

    inv[3] = -m[1] * m[6] * m[11] + 
              m[1] * m[7] * m[10] + 
              m[5] * m[2] * m[11] - 
              m[5] * m[3] * m[10] - 
              m[9] * m[2] * m[7] + 
              m[9] * m[3] * m[6];

    inv[7] = m[0] * m[6] * m[11] - 
             m[0] * m[7] * m[10] - 
             m[4] * m[2] * m[11] + 
             m[4] * m[3] * m[10] + 
             m[8] * m[2] * m[7] - 
             m[8] * m[3] * m[6];

    inv[11] = -m[0] * m[5] * m[11] + 
               m[0] * m[7] * m[9] + 
               m[4] * m[1] * m[11] - 
               m[4] * m[3] * m[9] - 
               m[8] * m[1] * m[7] + 
               m[8] * m[3] * m[5];

    inv[15] = m[0] * m[5] * m[10] - 
              m[0] * m[6] * m[9] - 
              m[4] * m[1] * m[10] + 
              m[4] * m[2] * m[9] + 
              m[8] * m[1] * m[6] - 
              m[8] * m[2] * m[5];

    det = m[0] * inv[0] + m[1] * inv[4] + m[2] * inv[8] + m[3] * inv[12];

    if (det == 0)
        /*return false;*/

    det = 1.0 / det;

    for (i = 0; i < 16; i++)
        invOut[i] = inv[i] * det;

    /*return true;*/
}

/********* ftp://download.intel.com/design/PentiumIII/sml/24504301.pdf ********/
void g3dcore_invertMatrixd ( double *mat, double *dst ) {
  float            tmp[12]; /* temp array for pairs             */
  float            src[16]; /* array of transpose source matrix */
  float            det;             /* determinant              */
  int i, j;

  /* transpose matrix */
  for ( i = 0; i < 4; i++) {
          src[i]                    = mat[i*4];
          src[i + 4]                = mat[i*4 + 1];
          src[i + 8]                = mat[i*4 + 2];
          src[i + 12]               = mat[i*4 + 3];
  }
  /* calculate pairs for first 8 elements (cofactors) */
  tmp[0]         =   src[10]        *   src[15];
  tmp[1]         =   src[11]        *   src[14];
  tmp[2]         =   src[9]         *   src[15];
  tmp[3]         =   src[11]        *   src[13];
  tmp[4]         =   src[9]         *   src[14];
  tmp[5]         =   src[10]        *   src[13];
  tmp[6]         =   src[8]         *   src[15];
  tmp[7]         =   src[11]        *   src[12];
  tmp[8]         =   src[8]         *   src[14];
  tmp[9]         =   src[10]        *   src[12];
  tmp[10]        =   src[8]         *   src[13];
  tmp[11]        =   src[9]         *   src[12];
  /* calculate first 8 elements (cofactors) */
  dst[0]         =   tmp[0]*src[5]            +  tmp[3]*src[6]   + tmp[4]*src[7];
  dst[0]       -=    tmp[1]*src[5]            +  tmp[2]*src[6]   + tmp[5]*src[7];
  dst[1]         =   tmp[1]*src[4]            +  tmp[6]*src[6]   + tmp[9]*src[7];
  dst[1]       -=    tmp[0]*src[4]            +  tmp[7]*src[6]   + tmp[8]*src[7];
  dst[2]         =   tmp[2]*src[4]            +  tmp[7]*src[5]   + tmp[10]*src[7];
  dst[2]       -=    tmp[3]*src[4]            +  tmp[6]*src[5]   + tmp[11]*src[7];
  dst[3]         =   tmp[5]*src[4]            +  tmp[8]*src[5]   + tmp[11]*src[6];
  dst[3]       -=    tmp[4]*src[4]            +  tmp[9]*src[5]   + tmp[10]*src[6];
  dst[4]          =  tmp[1]*src[1]            +  tmp[2]*src[2]   + tmp[5]*src[3];
  dst[4]       -=    tmp[0]*src[1]            +  tmp[3]*src[2]   + tmp[4]*src[3];
  dst[5]          =  tmp[0]*src[0]            +  tmp[7]*src[2]   + tmp[8]*src[3];
  dst[5]       -=    tmp[1]*src[0]            +  tmp[6]*src[2]   + tmp[9]*src[3];
  dst[6]          =  tmp[3]*src[0]            +  tmp[6]*src[1]   + tmp[11]*src[3];
  dst[6]       -=    tmp[2]*src[0]            +  tmp[7]*src[1]   + tmp[10]*src[3];
  dst[7]          =  tmp[4]*src[0]            +  tmp[9]*src[1]   + tmp[10]*src[2];
  dst[7]       -=    tmp[5]*src[0]            +  tmp[8]*src[1]   + tmp[11]*src[2];
  /* calculate pairs for second 8 elements (cofactors) */
  tmp[0]         =   src[2]*src[7];
  tmp[1]         =   src[3]*src[6];
  tmp[2]         =   src[1]*src[7];
  tmp[3]         =   src[3]*src[5];
  tmp[4]         =   src[1]*src[6];
  tmp[5]         =   src[2]*src[5];
  tmp[6]        =   src[0]*src[7];
  tmp[7]        =   src[3]*src[4];
  tmp[8]        =   src[0]*src[6];
  tmp[9]        =   src[2]*src[4];
  tmp[10]       =   src[0]*src[5];
  tmp[11]       =   src[1]*src[4];
  /* calculate second 8 elements (cofactors) */
  dst[8] =          tmp[0]*src[13] + tmp[3]*src[14] + tmp[4]*src[15];
  dst[8] -=         tmp[1]*src[13] + tmp[2]*src[14] + tmp[5]*src[15];
  dst[9] =          tmp[1]*src[12] + tmp[6]*src[14] + tmp[9]*src[15];
  dst[9] -=         tmp[0]*src[12] + tmp[7]*src[14] + tmp[8]*src[15];
  dst[10] =         tmp[2]*src[12] + tmp[7]*src[13] + tmp[10]*src[15];
  dst[10]-=         tmp[3]*src[12] + tmp[6]*src[13] + tmp[11]*src[15];
  dst[11] =         tmp[5]*src[12] + tmp[8]*src[13] + tmp[11]*src[14];
  dst[11]-=         tmp[4]*src[12] + tmp[9]*src[13] + tmp[10]*src[14];
  dst[12] =         tmp[2]*src[10] + tmp[5]*src[11] + tmp[1]*src[9];
  dst[12]-=         tmp[4]*src[11] + tmp[0]*src[9] + tmp[3]*src[10];
  dst[13] =         tmp[8]*src[11] + tmp[0]*src[8] + tmp[7]*src[10];
  dst[13]-=         tmp[6]*src[10] + tmp[9]*src[11] + tmp[1]*src[8];
  dst[14] =         tmp[6]*src[9] + tmp[11]*src[11] + tmp[3]*src[8];
  dst[14]-=         tmp[10]*src[11] + tmp[2]*src[8] + tmp[7]*src[9];
  dst[15] =         tmp[10]*src[10] + tmp[4]*src[8] + tmp[9]*src[9];
  dst[15]-=         tmp[8]*src[9] + tmp[11]*src[10] + tmp[5]*src[8];
  /* calculate determinant */
  det=src[0]*dst[0]+src[1]*dst[1]+src[2]*dst[2]+src[3]*dst[3];
  /* calculate matrix inverse */
  det = 1/det;

  for ( j = 0; j < 16; j++)
         dst[j] *= det;
}

/******************************************************************************/
/*
void g3dcore_invertMatrix ( double *inp, double *out ) {
    double tmp[0x20] =  { inp[0x00], inp[0x01], inp[0x02], inp[0x03],
                          1.0f     , 0.0f     , 0.0f     , 0.0f    ,
                          inp[0x04], inp[0x05], inp[0x06], inp[0x07],
                          0.0f     , 1.0f     , 0.0f     , 0.0f    ,
                          inp[0x08], inp[0x09], inp[0x0A], inp[0x0B],
                          0.0f     , 0.0f     , 1.0f     , 0.0f    ,
                          inp[0x0C], inp[0x0D], inp[0x0E], inp[0x0F],
                          0.0f     , 0.0f     , 0.0f     , 1.0f     };
    double piv;
    int i, j;

    for ( i = 0x00; i < 0x04; i++ ) {
        int ioff = ( i << 0x03 ), n = 0x00;
        double t = ( piv = tmp[ioff + n] );

        while ( ( t > -EPSILON ) && ( t < EPSILON ) ) {
            n = ( n + 0x01 );

            t = ( piv = tmp[ioff + n] );
        }

        for ( n = 0x00; n < 0x08; n++ ) {
            int ind = ( ioff + n );

            tmp[ind] = tmp[ind] / piv ;
        }

        for ( j = 0x00; j < 0x04; j++ ) {
            int joff = ( j << 0x03 );

            if ( i != j ) {
                double fac = tmp[joff+i];

                for ( n = 0x00; n < 0x08; n++ ) {
                    tmp[joff+n] -= (fac*tmp[ioff+n]);
                }
            }
        }
    }

    out[0x00] = tmp[0x04];
    out[0x01] = tmp[0x05];
    out[0x02] = tmp[0x06];
    out[0x03] = tmp[0x07];
    out[0x04] = tmp[0x0C];
    out[0x05] = tmp[0x0D];
    out[0x06] = tmp[0x0E];
    out[0x07] = tmp[0x0F];
    out[0x08] = tmp[0x14];
    out[0x09] = tmp[0x15];
    out[0x0A] = tmp[0x16];
    out[0x0B] = tmp[0x17];
    out[0x0C] = tmp[0x1C];
    out[0x0D] = tmp[0x1D];
    out[0x0E] = tmp[0x1E];
    out[0x0F] = tmp[0x1F];
}
*/

/******************************************************************************/
void g3dcore_multMatrixf ( float *A, float *B, float *M ) {
    float T[0x10];
/*** column-major arrangement ***/
#define M00(m) m[0x0]
#define M01(m) m[0x4]
#define M02(m) m[0x8]
#define M03(m) m[0xC]
#define M10(m) m[0x1]
#define M11(m) m[0x5]
#define M12(m) m[0x9]
#define M13(m) m[0xD]
#define M20(m) m[0x2]
#define M21(m) m[0x6]
#define M22(m) m[0xA]
#define M23(m) m[0xE]
#define M30(m) m[0x3]
#define M31(m) m[0x7]
#define M32(m) m[0xB]
#define M33(m) m[0xF]
    /*** first row ***/
    M00(T) = (M00(A)*M00(B))+(M01(A)*M10(B))+(M02(A)*M20(B))+(M03(A)*M30(B));
    M01(T) = (M00(A)*M01(B))+(M01(A)*M11(B))+(M02(A)*M21(B))+(M03(A)*M31(B));
    M02(T) = (M00(A)*M02(B))+(M01(A)*M12(B))+(M02(A)*M22(B))+(M03(A)*M32(B));
    M03(T) = (M00(A)*M03(B))+(M01(A)*M13(B))+(M02(A)*M23(B))+(M03(A)*M33(B));
    /*** second row ***/
    M10(T) = (M10(A)*M00(B))+(M11(A)*M10(B))+(M12(A)*M20(B))+(M13(A)*M30(B));
    M11(T) = (M10(A)*M01(B))+(M11(A)*M11(B))+(M12(A)*M21(B))+(M13(A)*M31(B));
    M12(T) = (M10(A)*M02(B))+(M11(A)*M12(B))+(M12(A)*M22(B))+(M13(A)*M32(B));
    M13(T) = (M10(A)*M03(B))+(M11(A)*M13(B))+(M12(A)*M23(B))+(M13(A)*M33(B));

    M20(T) = (M20(A)*M00(B))+(M21(A)*M10(B))+(M22(A)*M20(B))+(M23(A)*M30(B));
    M21(T) = (M20(A)*M01(B))+(M21(A)*M11(B))+(M22(A)*M21(B))+(M23(A)*M31(B));
    M22(T) = (M20(A)*M02(B))+(M21(A)*M12(B))+(M22(A)*M22(B))+(M23(A)*M32(B));
    M23(T) = (M20(A)*M03(B))+(M21(A)*M13(B))+(M22(A)*M23(B))+(M23(A)*M33(B));

    M30(T) = (M30(A)*M00(B))+(M31(A)*M10(B))+(M32(A)*M20(B))+(M33(A)*M30(B));
    M31(T) = (M30(A)*M01(B))+(M31(A)*M11(B))+(M32(A)*M21(B))+(M33(A)*M31(B));
    M32(T) = (M30(A)*M02(B))+(M31(A)*M12(B))+(M32(A)*M22(B))+(M33(A)*M32(B));
    M33(T) = (M30(A)*M03(B))+(M31(A)*M13(B))+(M32(A)*M23(B))+(M33(A)*M33(B));

    memcpy ( M, T, sizeof ( T ) );
#undef M00
#undef M01
#undef M02
#undef M03
#undef M10
#undef M11
#undef M12
#undef M13
#undef M20
#undef M21
#undef M22
#undef M23
#undef M30
#undef M31
#undef M32
#undef M33
}

/******************************************************************************/
void g3dcore_multMatrixd ( double *A, double *B, double *M ) {
    double T[0x10];
/*** column-major arrangement ***/
#define M00(m) m[0x0]
#define M01(m) m[0x4]
#define M02(m) m[0x8]
#define M03(m) m[0xC]
#define M10(m) m[0x1]
#define M11(m) m[0x5]
#define M12(m) m[0x9]
#define M13(m) m[0xD]
#define M20(m) m[0x2]
#define M21(m) m[0x6]
#define M22(m) m[0xA]
#define M23(m) m[0xE]
#define M30(m) m[0x3]
#define M31(m) m[0x7]
#define M32(m) m[0xB]
#define M33(m) m[0xF]
    /*** first row ***/
    M00(T) = (M00(A)*M00(B))+(M01(A)*M10(B))+(M02(A)*M20(B))+(M03(A)*M30(B));
    M01(T) = (M00(A)*M01(B))+(M01(A)*M11(B))+(M02(A)*M21(B))+(M03(A)*M31(B));
    M02(T) = (M00(A)*M02(B))+(M01(A)*M12(B))+(M02(A)*M22(B))+(M03(A)*M32(B));
    M03(T) = (M00(A)*M03(B))+(M01(A)*M13(B))+(M02(A)*M23(B))+(M03(A)*M33(B));
    /*** second row ***/
    M10(T) = (M10(A)*M00(B))+(M11(A)*M10(B))+(M12(A)*M20(B))+(M13(A)*M30(B));
    M11(T) = (M10(A)*M01(B))+(M11(A)*M11(B))+(M12(A)*M21(B))+(M13(A)*M31(B));
    M12(T) = (M10(A)*M02(B))+(M11(A)*M12(B))+(M12(A)*M22(B))+(M13(A)*M32(B));
    M13(T) = (M10(A)*M03(B))+(M11(A)*M13(B))+(M12(A)*M23(B))+(M13(A)*M33(B));

    M20(T) = (M20(A)*M00(B))+(M21(A)*M10(B))+(M22(A)*M20(B))+(M23(A)*M30(B));
    M21(T) = (M20(A)*M01(B))+(M21(A)*M11(B))+(M22(A)*M21(B))+(M23(A)*M31(B));
    M22(T) = (M20(A)*M02(B))+(M21(A)*M12(B))+(M22(A)*M22(B))+(M23(A)*M32(B));
    M23(T) = (M20(A)*M03(B))+(M21(A)*M13(B))+(M22(A)*M23(B))+(M23(A)*M33(B));

    M30(T) = (M30(A)*M00(B))+(M31(A)*M10(B))+(M32(A)*M20(B))+(M33(A)*M30(B));
    M31(T) = (M30(A)*M01(B))+(M31(A)*M11(B))+(M32(A)*M21(B))+(M33(A)*M31(B));
    M32(T) = (M30(A)*M02(B))+(M31(A)*M12(B))+(M32(A)*M22(B))+(M33(A)*M32(B));
    M33(T) = (M30(A)*M03(B))+(M31(A)*M13(B))+(M32(A)*M23(B))+(M33(A)*M33(B));

    memcpy ( M, T, sizeof ( T ) );
#undef M00
#undef M01
#undef M02
#undef M03
#undef M10
#undef M11
#undef M12
#undef M13
#undef M20
#undef M21
#undef M22
#undef M23
#undef M30
#undef M31
#undef M32
#undef M33
}

/******************************************************************************/
void g3dcore_multmatrixdirect ( double *A, double *B ) {
    double M[0x10];

    g3dcore_multMatrixd ( A, B, M );

    memcpy ( A, M, sizeof ( M ) );
}

/******************************************************************************/
void g3dcore_identityMatrixf ( float *M ) {
    M[0x00] = 1.0f;
    M[0x01] = 0.0f;
    M[0x02] = 0.0f;
    M[0x03] = 0.0f;
    M[0x04] = 0.0f;
    M[0x05] = 1.0f;
    M[0x06] = 0.0f;
    M[0x07] = 0.0f;
    M[0x08] = 0.0f;
    M[0x09] = 0.0f;
    M[0x0A] = 1.0f;
    M[0x0B] = 0.0f;
    M[0x0C] = 0.0f;
    M[0x0D] = 0.0f;
    M[0x0E] = 0.0f;
    M[0x0F] = 1.0f;
}

/******************************************************************************/
void g3dcore_identityMatrixd ( double *M ) {
    M[0x00] = 1.0f;
    M[0x01] = 0.0f;
    M[0x02] = 0.0f;
    M[0x03] = 0.0f;
    M[0x04] = 0.0f;
    M[0x05] = 1.0f;
    M[0x06] = 0.0f;
    M[0x07] = 0.0f;
    M[0x08] = 0.0f;
    M[0x09] = 0.0f;
    M[0x0A] = 1.0f;
    M[0x0B] = 0.0f;
    M[0x0C] = 0.0f;
    M[0x0D] = 0.0f;
    M[0x0E] = 0.0f;
    M[0x0F] = 1.0f;
}

/******************************************************************************/
void g3dcore_printMatrixf ( float *M ) {
    printf("%f %f %f %f\n", M[0x00], M[0x04], M[0x08], M[0x0C] );
    printf("%f %f %f %f\n", M[0x01], M[0x05], M[0x09], M[0x0D] );
    printf("%f %f %f %f\n", M[0x02], M[0x06], M[0x0A], M[0x0E] );
    printf("%f %f %f %f\n", M[0x03], M[0x07], M[0x0B], M[0x0F] );
    printf("\n");
}

/******************************************************************************/
void g3dcore_printMatrixd ( double *M ) {
    printf("%f %f %f %f\n", M[0x00], M[0x04], M[0x08], M[0x0C] );
    printf("%f %f %f %f\n", M[0x01], M[0x05], M[0x09], M[0x0D] );
    printf("%f %f %f %f\n", M[0x02], M[0x06], M[0x0A], M[0x0E] );
    printf("%f %f %f %f\n", M[0x03], M[0x07], M[0x0B], M[0x0F] );
    printf("\n");
}

/******************************************************************************/
void g3dcore_getMatrixScaled ( double *matrix, G3DVECTOR *sca ) {
    float xscale = 0, yscale = 0, zscale = 0;

    sca->x = ( float ) sqrt ( matrix[0x00] * matrix[0x00] +
                              matrix[0x01] * matrix[0x01] +
                              matrix[0x02] * matrix[0x02] );

    sca->y = ( float ) sqrt ( matrix[0x04] * matrix[0x04] +
                              matrix[0x05] * matrix[0x05] +
                              matrix[0x06] * matrix[0x06] );

    sca->z = ( float ) sqrt ( matrix[0x08] * matrix[0x08] +
                              matrix[0x09] * matrix[0x09] +
                              matrix[0x0A] * matrix[0x0A] );
    sca->w = 1.0f;
}

/******************************************************************************/
void g3dcore_getMatrixScalef ( float *matrix, G3DVECTOR *sca ) {
    float xscale = 0, yscale = 0, zscale = 0;

    sca->x = ( float ) sqrt ( matrix[0x00] * matrix[0x00] +
                              matrix[0x01] * matrix[0x01] +
                              matrix[0x02] * matrix[0x02] );

    sca->y = ( float ) sqrt ( matrix[0x04] * matrix[0x04] +
                              matrix[0x05] * matrix[0x05] +
                              matrix[0x06] * matrix[0x06] );

    sca->z = ( float ) sqrt ( matrix[0x08] * matrix[0x08] +
                              matrix[0x09] * matrix[0x09] +
                              matrix[0x0A] * matrix[0x0A] );
    sca->w = 1.0f;
}

/******************************************************************************/
void g3dcore_getMatrixRotationf ( float *matrix, G3DVECTOR *rot ) {
    float mtmp[0x10];
    G3DVECTOR sca;
    int i, j;

    memcpy ( mtmp, matrix, sizeof ( mtmp ) );

    g3dcore_getMatrixScalef ( matrix, &sca );

    for ( i = 0x00; i < 0x03; i++ ) {
        float *ptr = &mtmp[ i * 0x04];

        *(ptr+0) /= sca.x;
        *(ptr+1) /= sca.y;
        *(ptr+2) /= sca.z;
    }

/** Many thanks to https://www.geometrictools.com/Documentation/EulerAngles.pdf ***/
/*** Note: OpenGL Matrix indexes are inverted compared to the latter document ***/
/*** Note2: my rotation matrix order is ZYX ***/

    rot->y = asin  ( -mtmp[0x02] );
    rot->x = atan2 (  mtmp[0x06], mtmp[0x0A] );
    rot->z = atan2 (  mtmp[0x01], mtmp[0x00] );

    rot->x = rot->x * ( 180 / M_PI );
    rot->y = rot->y * ( 180 / M_PI );
    rot->z = rot->z * ( 180 / M_PI );
    rot->w = 1.0f;
}

/******************************************************************************/
void g3dcore_getMatrixRotationd ( double *matrix, G3DVECTOR *rot ) {
    double mtmp[0x10];
    G3DVECTOR sca;
    int i, j;

    memcpy ( mtmp, matrix, sizeof ( mtmp ) );

    g3dcore_getMatrixScaled ( matrix, &sca );

    for ( i = 0x00; i < 0x03; i++ ) {
        double *ptr = &mtmp[ i * 0x04];

        *(ptr+0) /= sca.x;
        *(ptr+1) /= sca.y;
        *(ptr+2) /= sca.z;
    }

/** Many thanks to https://www.geometrictools.com/Documentation/EulerAngles.pdf ***/
/*** Note: OpenGL Matrix indexes are inverted compared to the latter document ***/
/*** Note2: my rotation matrix order is ZYX ***/

    rot->y = asin  ( -mtmp[0x02] );
    rot->x = atan2 (  mtmp[0x06], mtmp[0x0A] );
    rot->z = atan2 (  mtmp[0x01], mtmp[0x00] );

    rot->x = rot->x * ( 180 / M_PI );
    rot->y = rot->y * ( 180 / M_PI );
    rot->z = rot->z * ( 180 / M_PI );
    rot->w = 1.0f;
}

/******************************************************************************/
void g3dcore_extractRotationMatrixf ( float *WMX, float *RMX ) {
    G3DVECTOR rot;

    g3dcore_getMatrixRotationf ( WMX, &rot );

    g3dcore_identityMatrixf( RMX );
    g3dcore_rotateMatrixf( RMX, rot.x, 1.0f, 0.0f, 0.0f );
    g3dcore_rotateMatrixf( RMX, rot.y, 0.0f, 1.0f, 0.0f );
    g3dcore_rotateMatrixf( RMX, rot.z, 0.0f, 0.0f, 1.0f );
}

/******************************************************************************/
void g3dcore_extractRotationMatrixd ( double *WMX, double *RMX ) {
    G3DVECTOR rot;

    g3dcore_getMatrixRotationd ( WMX, &rot );

    g3dcore_identityMatrixd( RMX );
    g3dcore_rotateMatrixd( RMX, rot.x, 1.0f, 0.0f, 0.0f );
    g3dcore_rotateMatrixd( RMX, rot.y, 0.0f, 1.0f, 0.0f );
    g3dcore_rotateMatrixd( RMX, rot.z, 0.0f, 0.0f, 1.0f );
}

/******************************************************************************/
void g3dcore_getMatrixTranslationf ( float *matrix, G3DVECTOR *pos ) {
    pos->x = matrix[0x0C];
    pos->y = matrix[0x0D];
    pos->z = matrix[0x0E];
    pos->w = 1.0f;
}

/******************************************************************************/
void g3dcore_getMatrixTranslationd ( double *matrix, G3DVECTOR *pos ) {
    pos->x = matrix[0x0C];
    pos->y = matrix[0x0D];
    pos->z = matrix[0x0E];
    pos->w = 1.0f;
}

/******************************************************************************/
float g3dcore_intersect ( G3DVECTOR *plane,
                          G3DVECTOR *p1,
                          G3DVECTOR *p2,
                          G3DVECTOR *pout ) {
    G3DVECTOR p1mp2 = { .x = p1->x - p2->x,
                        .y = p1->y - p2->y,
                        .z = p1->z - p2->z,
                        .w = 1.0f };
    G3DVECTOR dir = { .x = p2->x - p1->x,
                      .y = p2->y - p1->y,
                      .z = p2->z - p1->z,
                      .w = 1.0f };
    float vo =  ( ( plane->x * p1->x ) +
                  ( plane->y * p1->y ) +
                  ( plane->z * p1->z ) ) + plane->w,
          vd = ( plane->x * p1mp2.x ) + 
               ( plane->y * p1mp2.y ) +
               ( plane->z * p1mp2.z );
    float t;

    if ( vd == 0.0f ) return 0.0f;

    t = ( vo / vd );

    /*if ( t > 0.0f ) {*/
        /*if ( vd < 0.0f ) {*/
            pout->x = p1->x + ( dir.x * t );
            pout->y = p1->y + ( dir.y * t );
            pout->z = p1->z + ( dir.z * t );
            /*** store the distance we got ***/
            pout->w = t;

            return t;
        /*}*/
    /*}*/

    return 0.0f;
}

/******************************************************************************/
void g3dcore_copyMatrixf( float *inM, float *outM ) {
    memcpy( outM, inM, sizeof( float ) * 0x10 );
}

/******************************************************************************/
void g3dcore_orthof( double left,
                     double right,
                     double bottom,
                     double top,
                     double nearVal,
                     double farVal,
                     float *M ) {
    float tx = - ( float ) ( right + left     ) / ( right  - left    );
    float ty = - ( float ) ( top   + bottom   ) / ( top    - bottom  );
    float tz = - ( float ) ( farVal + nearVal ) / ( farVal - nearVal );

    /** note: OpenGL matrices are column-major */
    M[0x00] = 2.0f / ( right - left );
    M[0x01] = 0.0f;
    M[0x02] = 0.0f;
    M[0x03] = 0.0f;

    M[0x04] = 0.0f;
    M[0x05] = 2.0f / ( top - bottom );
    M[0x06] = 0.0f;
    M[0x07] = 0.0f;

    M[0x08] = 0.0f;
    M[0x09] = 0.0f;
    M[0x0A] = -2.0f / ( farVal - nearVal );
    M[0x0B] = 0.0f;

    M[0x0C] = tx;
    M[0x0D] = ty;
    M[0x0E] = tz;
    M[0x0F] = 1.0f;
}

/******************************************************************************/
void g3dcore_perpespectivef ( double fovy,
                              double aspect,
                              double zNear,
                              double zFar,
                              float *projectionMatrix ) {
    double x = ( fovy  * M_PI / 180.0f ) / 2.0f;
    double f = 1.0f / tan ( x / 2.0f );
    double zfPzn = zFar  + zNear;
    double znMzf = zNear - zFar;
    double zfMzn = zFar  - zNear;
/*
    projectionMatrix[0x00] =  aspect ? ( f / aspect ) : 0.0f;
    projectionMatrix[0x04] =  0.0f;
    projectionMatrix[0x08] =  0.0f;
    projectionMatrix[0x0C] =  0.0f;
    projectionMatrix[0x01] =  0.0f;
    projectionMatrix[0x05] =  f;
    projectionMatrix[0x09] =  0.0f;
    projectionMatrix[0x0D] =  0.0f;
    projectionMatrix[0x02] =  0.0f;
    projectionMatrix[0x06] =  0.0f;
    projectionMatrix[0x0A] = znMzf ? zfPzn / znMzf : 0.0f;
    projectionMatrix[0x0E] = -1.0f;
    projectionMatrix[0x03] =  0.0f;
    projectionMatrix[0x07] =  0.0f;
    projectionMatrix[0x0B] = znMzf ? ( 2.0f * zFar * zNear ) / znMzf : 0.0f;
    projectionMatrix[0x0F] =  0.0f;
*/
    /** we cannot use gluPerspective() because we are in a thread. **/
    /* https://www.opengl.org/sdk/docs/man2/xhtml/gluPerspective.xml */
    projectionMatrix[0x00] =  aspect ? ( f / aspect ) : 0.0f;
    projectionMatrix[0x01] =  0.0f;
    projectionMatrix[0x02] =  0.0f;
    projectionMatrix[0x03] =  0.0f;
    projectionMatrix[0x04] =  0.0f;
    projectionMatrix[0x05] =  f;
    projectionMatrix[0x06] =  0.0f;
    projectionMatrix[0x07] =  0.0f;
    projectionMatrix[0x08] =  0.0f;
    projectionMatrix[0x09] =  0.0f;
    projectionMatrix[0x0A] = znMzf ? zfPzn / znMzf : 0.0f;
    projectionMatrix[0x0B] = -1.0f;
    projectionMatrix[0x0C] =  0.0f;
    projectionMatrix[0x0D] =  0.0f;
    projectionMatrix[0x0E] = znMzf ? ( 2.0f * zFar * zNear ) / znMzf : 0.0f;
    projectionMatrix[0x0F] =  0.0f;
}
