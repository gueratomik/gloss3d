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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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
#include <r3dnet.h>

#define PNG_DEBUG 3

/**************** http://zarb.org/~gc/html/libpng.html ************************/
void r3dnetframe_writepng ( R3DNETFRAME *frm, const char *dir ) {
    FILE *fdst;
    char *tmpcol = ( char * ) malloc ( frm->img->width  *
                                       frm->img->height * 0x03 );
    char **tmprow = ( char ** ) malloc ( frm->img->height * sizeof ( char * ) );
    char *tmpbuf = tmpcol;
    png_structp png_ptr;
    png_infop info_ptr;
    char buffer[0x100];
    int i, j;

    for ( i = 0x00; i < frm->img->height; i++ ) {
        tmprow[i] = tmpbuf;

        for ( j = 0x00; j < frm->img->width; j++ ) {
            uint32_t color = frm->img->f.get_pixel ( frm->img, j, i );
            uint32_t R = ( color & 0x00FF0000 ) >> 0x10;
            uint32_t G = ( color & 0x0000FF00 ) >> 0x08;
            uint32_t B = ( color & 0x000000FF );

            (*tmpbuf++) = R;
            (*tmpbuf++) = G;
            (*tmpbuf++) = B;
        }
    }

    /*** Not safe. TODO : some malloc magic ***/
    snprintf ( buffer, 0xFF, "%s/frame%04i.png", dir, frm->frmid );

    printf ( "Writing png to %s\n", buffer );

    if ( ( fdst = fopen ( buffer, "wb") ) == NULL ) {
        fprintf ( stderr, "Could not open %s for writing\n", buffer );

        return;
    }

    /* initialize stuff */
    png_ptr = png_create_write_struct(PNG_LIBPNG_VER_STRING, NULL, NULL, NULL);

    if ( png_ptr == NULL ) {
        fprintf ( stderr, "png_create_write_struct failed\n" );
    }

    info_ptr = png_create_info_struct ( png_ptr );

    if ( info_ptr == NULL ) {
        fprintf ( stderr, "png_create_info_struct failed\n" );
    }

    if ( setjmp ( png_jmpbuf ( png_ptr ) ) ) {
        fprintf ( stderr, "Error during init_io" );
    }

    png_init_io ( png_ptr, fdst );

    /* write header */
    if ( setjmp ( png_jmpbuf ( png_ptr ) ) ) {
        fprintf ( stderr, "Error during writing header");
    }

    png_set_IHDR( png_ptr, info_ptr, frm->img->width, frm->img->height,
                  0x08, PNG_COLOR_TYPE_RGB, PNG_INTERLACE_NONE,
                  PNG_COMPRESSION_TYPE_BASE, PNG_FILTER_TYPE_BASE);

    png_write_info ( png_ptr, info_ptr );


    /* write bytes */
    if ( setjmp ( png_jmpbuf ( png_ptr ) ) ) {
        fprintf ( stderr, "Error during writing bytes");
    }

    png_write_image ( png_ptr, ( png_bytepp ) tmprow );


    /* end write */
    if ( setjmp ( png_jmpbuf ( png_ptr ) ) ) {
        fprintf ( stderr, "Error during end of write");
    }

    png_write_end ( png_ptr, NULL );

    free ( tmprow );
    free ( tmpcol );

    fclose ( fdst );
}

/******************************************************************************/
/**  http://www.andrewewhite.net/wordpress/2008/09/02/                        */
/**  very-simple-jpeg-writer-in-c-c/                                          */
/******************************************************************************/
void r3dnetframe_writejpeg ( R3DNETFRAME *frm, const char *dir ) {
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr jerr;
    JSAMPROW row_pointer[0x01]; /* pointer to JSAMPLE row[s] */
    int row_stride;     /* physical row width in image buffer */
    char buffer[0x100];
    FILE *fdst;
    char *tmpcol = ( char * ) malloc ( frm->img->width  *
                                       frm->img->height * 0x03 );
    char *tmpbuf = tmpcol;
    int i, j;

    for ( i = 0x00; i < frm->img->height; i++ ) {
        for ( j = 0x00; j < frm->img->width; j++ ) {
            uint32_t color = frm->img->f.get_pixel ( frm->img, j, i );
            uint32_t R = ( color & 0x00FF0000 ) >> 0x10;
            uint32_t G = ( color & 0x0000FF00 ) >> 0x08;
            uint32_t B = ( color & 0x000000FF );

            (*tmpbuf++) = R;
            (*tmpbuf++) = G;
            (*tmpbuf++) = B;
        }
    }

    /*** Not safe. TODO : some malloc magic ***/
    snprintf ( buffer, 0xFF, "%s/frame%04i.jpg", dir, frm->frmid );

    printf ( "Writing jpeg to %s\n", buffer );

    cinfo.err = jpeg_std_error ( &jerr );
    jpeg_create_compress ( &cinfo );

    if ( ( fdst = fopen ( buffer, "wb") ) == NULL ) {
        fprintf ( stderr, "Could not open %s for writing\n", buffer );

        return;
    }

    jpeg_stdio_dest ( &cinfo, fdst );
 
    cinfo.image_width      = frm->img->width;
    cinfo.image_height     = frm->img->height;
    cinfo.input_components = 0x03;
    cinfo.in_color_space   = JCS_RGB;

    jpeg_set_defaults ( &cinfo );
    /*set the quality [0..100]  */
    jpeg_set_quality ( &cinfo, 100, TRUE );
    jpeg_start_compress ( &cinfo, TRUE );

    row_stride = frm->img->width * 0x03; /* JSAMPLEs per row in image_buffer */

    while ( cinfo.next_scanline < cinfo.image_height ) {
        row_pointer[0] = & tmpcol[cinfo.next_scanline * row_stride];

        jpeg_write_scanlines ( &cinfo, ( JSAMPARRAY ) &row_pointer, 0x01 );
    }

    jpeg_finish_compress ( &cinfo );

    fclose ( fdst );

    free ( tmpcol );

    jpeg_destroy_compress ( &cinfo );
}

/******************************************************************************/
void r3dnetframe_dispatchjobs ( R3DNETFRAME *frm ) {
    uint32_t nbjobs = frm->nbjobs;
    uint32_t from = 0x00;
    uint32_t to   = 0x00;
    uint32_t nblines = ( nbjobs ) ? ( frm->img->height / nbjobs ): 0x00;
    int n = 0x00;
    LIST *ltmp = frm->ljob;

    printf ( "%d clients for frame %llu\n", nbjobs, ( long long unsigned int ) frm );

    while ( ltmp ) {
        R3DNETJOB *job = ( R3DNETJOB * ) ltmp->data;
        n++;

        if ( n == nbjobs ) {
            job->from = from;
            job->to   = ( frm->img->height - 0x01 );
        } else {
            job->from = from;
            job->to   = from + nblines - 0x01; 

            from += nblines;
        }

        printf ( "Job assigned From:%d To:%d\n", job->from, job->to );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
R3DNETJOB *r3dnetjob_new ( uint32_t frmid ) {
    uint32_t structsize = sizeof ( R3DNETJOB );
    R3DNETJOB *job;

    if ( ( job = ( R3DNETJOB * ) calloc ( 0x01, structsize ) ) == NULL ) {
        fprintf ( stderr, "r3dnetjob_new: memory allocation failed\n" );

        return NULL;
    } 

    job->frmid = frmid;


    return job;
}

/******************************************************************************/
void r3dnetjob_free ( R3DNETJOB *job ) {
    free ( job );
}

/******************************************************************************/
void r3dnetframe_addjob ( R3DNETFRAME *frm, R3DNETJOB *job ) {
    list_insert ( &frm->ljob, job );

    frm->nbjobs++;
}

/******************************************************************************/
R3DNETFRAME *r3dnetframe_new ( Display *dis, uint32_t width,
                                             uint32_t height,
                                             uint32_t depth,
                                             uint32_t frmid ) {
    uint32_t structsize = sizeof ( R3DNETFRAME );
    R3DNETFRAME *frm;
    Visual *vis = DefaultVisual ( dis, 0x00 );

    if ( ( frm = ( R3DNETFRAME * ) calloc ( 0x01, structsize ) ) == NULL ) {
        fprintf ( stderr, "r3dnetframe_new: memory allocation failed\n" );

        return NULL;
    } 

    frm->frmid = frmid;
    frm->img   = XCreateImage ( dis , vis  , depth , ZPixmap, 0x00,
                                NULL, width, height, 0x20   , 0x00 );
    XInitImage ( frm->img );


    frm->img->data = ( char * ) malloc ( frm->img->bytes_per_line *
                                         frm->img->height );

    if ( frm->img->data == NULL ) {
        fprintf ( stderr, "r3dnetframe_new: Image memory allocation failed\n" );

        /*XDestroyImage ( frm->img );*/ /* TODO*/

        free ( frm );
    }


    return frm;
}

/******************************************************************************/
void r3dnetframe_free ( R3DNETFRAME *frm ) {
    list_free ( &frm->ljob, NULL );

    XDestroyImage ( frm->img );
}

/******************************************************************************/
void r3dnetframe_removejob ( R3DNETFRAME *frm, R3DNETJOB *job ) {
    list_remove ( &frm->ljob, job );

    frm->nbjobs--;
}
