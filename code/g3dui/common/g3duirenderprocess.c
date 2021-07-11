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
void g3duirenderprocess_savejpg ( G3DUIRENDERPROCESS *rps, char *filename ) {
#ifdef unusedQISS3D
    Q3DFILTER *fil = q3djob_getFilter ( rps->qjob, TOBUFFERFILTERNAME );

    if ( fil ) {

        FILTERTOBUFFER *ftb = ( FILTERTOBUFFER * ) fil->data;

        r3draw_to_jpg ( ftb->rawimg, ftb->width,
                                     ftb->height,
                                     ftb->depth,
                                     filename );

    } else {
        fprintf ( stderr, "No buffer filter found !\n" );
    }
#endif
}

/******************************************************************************/
G3DUIRENDERPROCESS *g3duirenderprocess_new ( uint64_t  id,
                                             G3DUI     *gui,
                                             Q3DJOB  *qjob,
                                             Q3DFILTER *filter_to_window,
                                             Q3DFILTER *filter_to_buffer ) {
    uint32_t size = sizeof ( G3DUIRENDERPROCESS );
    G3DUIRENDERPROCESS *rps = ( G3DUIRENDERPROCESS * ) calloc ( 0x01, size );

    if ( rps == NULL ) {
        fprintf ( stderr, "g3duirenderprocess_new: mem allocation failed\n" );

        return NULL;
    }

    rps->id   = id;
    rps->qjob = qjob;

    rps->gui = gui;

    /*rps->filter_to_window = filter_to_window;
    rps->filter_to_buffer = filter_to_buffer;*/

    return rps;
}

/******************************************************************************/
void *g3duirenderprocess_render_frame_t ( G3DUIRENDERPROCESS *rps ) {
    rps->gui->engine_flags |= LOADFULLRESIMAGES;

    q3djob_render_frame_t ( rps->qjob );

    rps->gui->engine_flags &= (~LOADFULLRESIMAGES);

    return NULL;
}

/******************************************************************************/
void *g3duirenderprocess_render_sequence_t ( G3DUIRENDERPROCESS *rps ) {
    rps->gui->engine_flags |= LOADFULLRESIMAGES;

    q3djob_render_sequence_t ( rps->qjob );

    rps->gui->engine_flags &= (~LOADFULLRESIMAGES);

    return NULL;
}

/******************************************************************************/
void g3duirenderprocess_free ( G3DUIRENDERPROCESS *rps ) {
    /*r3dfilter_free ( rps->filter_to_window );
    r3dfilter_free ( rps->filter_to_buffer );*/

    /*if ( rps->filename ) free ( rps->filename );*/

    q3djob_render_t_free ( rps->qjob );

    /*free ( rps );*/
}

/******************************************************************************/
void g3duirenderprocess_filename ( G3DUIRENDERPROCESS *rps, char *filename ) {
    uint32_t len = strlen ( filename ) + 0x01;

    rps->filename = realloc ( rps->filename, len );

    memset ( rps->filename, 0x00, len );

    memcpy ( rps->filename, filename, len );
}
