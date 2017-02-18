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
#include <r3d.h>

/* Thanks to
http://www.andrewewhite.net/wordpress/2008/09/02/very-simple-jpeg-writer-in-c-c/
*/

/******************************************************************************/
void r3draw_to_jpg ( char *rawimg, uint32_t width,
                                   uint32_t height,
                                   uint32_t depth,
                                   char *filename ) {
    uint32_t bytesperline = width * ( depth >> 0x03 );
    struct jpeg_compress_struct cinfo;
    struct jpeg_error_mgr       jerr;
    FILE *fdst = fopen ( filename, "wb");
    JSAMPROW row[1];
    int i=0;
    if ( fdst == NULL ) {
        fprintf ( stderr, "failed to open %s for writing\n", filename );

        return;
    }

    cinfo.err = jpeg_std_error ( &jerr );

    jpeg_create_compress ( &cinfo );
    jpeg_stdio_dest      ( &cinfo, fdst );
 
    cinfo.image_width      = width;
    cinfo.image_height     = height;
    cinfo.input_components = 0x03;
    cinfo.in_color_space   = JCS_RGB;

    jpeg_set_defaults   ( &cinfo );
    /*set the quality [0..100]  */
    jpeg_set_quality    ( &cinfo, 100, 0x01 );
    jpeg_start_compress ( &cinfo, TRUE );

    while ( cinfo.next_scanline < cinfo.image_height ) {
        row[0] = ( JSAMPROW ) &rawimg[cinfo.next_scanline*bytesperline];
        jpeg_write_scanlines ( &cinfo, row, 0x01 );
    }

    jpeg_finish_compress ( &cinfo );

    jpeg_destroy_compress ( &cinfo );

    fclose ( fdst );
}
