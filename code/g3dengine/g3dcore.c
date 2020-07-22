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
/* Note: yaw (Y), pitch (X), roll (Z) */
void g3dcore_eulerToQuaternion ( G3DVECTOR *angles, G3DQUATERNION *qout ) {
    double pitch = angles->x;
    double yaw   = angles->y;
    double roll  = angles->z;
    /* Abbreviations for the various angular functions */
    double cy = cos ( yaw   * 0.5f );
    double sy = sin ( yaw   * 0.5f );
    double cp = cos ( pitch * 0.5f );
    double sp = sin ( pitch * 0.5f );
    double cr = cos ( roll  * 0.5f );
    double sr = sin ( roll  * 0.5f );

    qout->w = cr * cp * cy + sr * sp * sy;
    qout->x = cr * sp * cy + sr * cp * sy;
    qout->y = cr * cp * sy - sr * sp * cy;
    qout->z = sr * cp * cy - cr * sp * sy;

    g3dquaternion_normalize ( qout );
}

/******************************************************************************/
void g3dcore_eulerInDegreesToQuaternion ( G3DVECTOR     *angles, 
                                          G3DQUATERNION *qout ) {
    G3DVECTOR rad = { .x = angles->x * M_PI / 180.0f,
                      .y = angles->y * M_PI / 180.0f,
                      .z = angles->z * M_PI / 180.0f,
                      .w = 1.0f };

    g3dcore_eulerToQuaternion ( &rad, qout );
}

/******************************************************************************/
void g3dcore_gridZX ( uint64_t engine_flags ) {
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
void g3dcore_gridYZ ( uint64_t engine_flags ) {
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
void g3dcore_gridXY ( uint64_t engine_flags ) {
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
void g3dcore_grid3D ( uint64_t engine_flags ) {
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
void g3dcore_symmetryMatrix ( double *matrix, uint32_t orientation ) {
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
void g3dcore_transposeMatrix ( double *inp, double *out ) {
    out[0x00] = inp[0x00];
    out[0x01] = inp[0x04];
    out[0x02] = inp[0x08];
    out[0x03] = inp[0x0C];

    out[0x04] = inp[0x01];
    out[0x05] = inp[0x05];
    out[0x06] = inp[0x09];
    out[0x07] = inp[0x0D];

    out[0x08] = inp[0x02];
    out[0x09] = inp[0x06];
    out[0x0A] = inp[0x0A];
    out[0x0B] = inp[0x0E];

    out[0x0C] = inp[0x03];
    out[0x0D] = inp[0x07];
    out[0x0E] = inp[0x0B];
    out[0x0F] = inp[0x0F];
}

/********* ftp://download.intel.com/design/PentiumIII/sml/24504301.pdf ********/
void g3dcore_invertMatrix ( double *mat, double *dst ) {
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
void g3dcore_multmatrix ( double *A, double *B, double *M ) {
    M[0x0] = (A[0x0]*B[0x0])+(A[0x1]*B[0x4])+(A[0x2]*B[0x8])+(A[0x3]*B[0xC]);
    M[0x1] = (A[0x0]*B[0x1])+(A[0x1]*B[0x5])+(A[0x2]*B[0x9])+(A[0x3]*B[0xD]);
    M[0x2] = (A[0x0]*B[0x2])+(A[0x1]*B[0x6])+(A[0x2]*B[0xA])+(A[0x3]*B[0xE]);
    M[0x3] = (A[0x0]*B[0x3])+(A[0x1]*B[0x7])+(A[0x2]*B[0xB])+(A[0x3]*B[0xF]);

    M[0x4] = (A[0x4]*B[0x0])+(A[0x5]*B[0x4])+(A[0x6]*B[0x8])+(A[0x7]*B[0xC]);
    M[0x5] = (A[0x4]*B[0x1])+(A[0x5]*B[0x5])+(A[0x6]*B[0x9])+(A[0x7]*B[0xD]);
    M[0x6] = (A[0x4]*B[0x2])+(A[0x5]*B[0x6])+(A[0x6]*B[0xA])+(A[0x7]*B[0xE]);
    M[0x7] = (A[0x4]*B[0x3])+(A[0x5]*B[0x7])+(A[0x6]*B[0xB])+(A[0x7]*B[0xF]);

    M[0x8] = (A[0x8]*B[0x0])+(A[0x9]*B[0x4])+(A[0xA]*B[0x8])+(A[0xB]*B[0xC]);
    M[0x9] = (A[0x8]*B[0x1])+(A[0x9]*B[0x5])+(A[0xA]*B[0x9])+(A[0xB]*B[0xD]);
    M[0xA] = (A[0x8]*B[0x2])+(A[0x9]*B[0x6])+(A[0xA]*B[0xA])+(A[0xB]*B[0xE]);
    M[0xB] = (A[0x8]*B[0x3])+(A[0x9]*B[0x7])+(A[0xA]*B[0xB])+(A[0xB]*B[0xF]);

    M[0xC] = (A[0xC]*B[0x0])+(A[0xD]*B[0x4])+(A[0xE]*B[0x8])+(A[0xF]*B[0xC]);
    M[0xD] = (A[0xC]*B[0x1])+(A[0xD]*B[0x5])+(A[0xE]*B[0x9])+(A[0xF]*B[0xD]);
    M[0xE] = (A[0xC]*B[0x2])+(A[0xD]*B[0x6])+(A[0xE]*B[0xA])+(A[0xF]*B[0xE]);
    M[0xF] = (A[0xC]*B[0x3])+(A[0xD]*B[0x7])+(A[0xE]*B[0xB])+(A[0xF]*B[0xF]);
}

/******************************************************************************/
void g3dcore_multmatrixdirect ( double *A, double *B ) {
    double M[0x10];

    g3dcore_multmatrix ( A, B, M );

    memcpy ( A, M, sizeof ( M ) );
}

/******************************************************************************/
void g3dcore_identityMatrix ( double *M ) {
    uint32_t i;

    for ( i = 0x00; i < 0x10; i++ ) {
        if ( i % 0x05 ) {
            M[i] = 0.0f;
        } else {
            M[i] = 1.0f;
        }
    }
}

/******************************************************************************/
void g3dcore_printMatrix ( double *MX, uint32_t nx, uint32_t ny ) {
    uint32_t i, j;

    for ( i = 0x00; i < ny; i++ ) {
        for ( j = 0x00; j < nx; j++ ) {
            printf ( "%f ", ( float ) MX[(i*nx)+j] );
        }

        printf ( "\n" );
    }
    printf ( "\n" );
}

/******************************************************************************/
void g3dcore_getMatrixScale ( double *matrix, G3DVECTOR *sca ) {
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
void g3dcore_getMatrixRotation ( double *matrix, G3DVECTOR *rot ) {
    double mtmp[0x10];
    G3DVECTOR sca;
    int i, j;

    memcpy ( mtmp, matrix, sizeof ( mtmp ) );

    g3dcore_getMatrixScale ( matrix, &sca );

    for ( i = 0x00; i < 0x03; i++ ) {
        double *ptr = &mtmp[ i * 0x04];

        *(ptr+0) /= sca.x;
        *(ptr+1) /= sca.y;
        *(ptr+2) /= sca.z;
    }

    rot->y = ( float ) asin ( mtmp[0x08] );

    if( mtmp[0x08] == 1.0f || mtmp[0x08] == -1.0f ) {
        rot->x = ( float ) atan2 ( -mtmp[0x01], mtmp[0x06] );
        rot->z = 0.0f;
    }else{
        rot->x = ( float ) atan2 ( -mtmp[0x09], mtmp[0x0A] );
        rot->z = ( float ) atan2 ( -mtmp[0x04], mtmp[0x00] );
    }

    rot->x = rot->x * ( 180 / M_PI );
    rot->y = rot->y * ( 180 / M_PI );
    rot->z = rot->z * ( 180 / M_PI );
    rot->w = 1.0f;
}

/******************************************************************************/
void g3dcore_extractRotationMatrix ( double *WMX, double *RMX ) {
    G3DVECTOR rot;

    g3dcore_getMatrixRotation ( WMX, &rot );

    glPushMatrix ( );
    glLoadIdentity ( );
    glRotatef ( rot.x, 1.0f, 0.0f, 0.0f );
    glRotatef ( rot.y, 0.0f, 1.0f, 0.0f );
    glRotatef ( rot.z, 0.0f, 0.0f, 1.0f );
    glGetDoublev ( GL_MODELVIEW_MATRIX, RMX );
    glPopMatrix ( );
}

/******************************************************************************/
void g3dcore_getMatrixTranslation ( double *matrix, G3DVECTOR *pos ) {
    pos->x = matrix[0x0C];
    pos->y = matrix[0x0D];
    pos->z = matrix[0x0E];
    pos->w = 1.0f;
}

/******************************************************************************/
float g3dcore_intersect ( G3DDOUBLEVECTOR *plane,
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
