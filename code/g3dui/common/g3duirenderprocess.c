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
    R3DFILTER *fil = r3dscene_getFilter ( rps->rsce, TOBUFFERFILTERNAME );

    if ( fil ) {
        FILTERTOBUFFER *ftb = ( FILTERTOBUFFER * ) fil->data;

        r3draw_to_jpg ( ftb->rawimg, ftb->width,
                                     ftb->height,
                                     ftb->depth,
                                     filename );
    } else {
        fprintf ( stderr, "No buffer filter found !\n" );
    }
}

/******************************************************************************/
G3DUIRENDERPROCESS *g3duirenderprocess_new ( uint64_t  id,
                                             G3DUI     *gui,
                                             R3DSCENE  *rsce,
                                             R3DFILTER *filter_to_window,
                                             R3DFILTER *filter_to_buffer ) {
    uint32_t size = sizeof ( G3DUIRENDERPROCESS );
    G3DUIRENDERPROCESS *rps = ( G3DUIRENDERPROCESS * ) calloc ( 0x01, size );

    if ( rps == NULL ) {
        fprintf ( stderr, "g3duirenderprocess_new: mem allocation failed\n" );

        return NULL;
    }

    rps->id   = id;
    rps->rsce = rsce;
    rps->qjob = rsce;

    rps->gui = gui;

    /*rps->filter_to_window = filter_to_window;
    rps->filter_to_buffer = filter_to_buffer;*/

    return rps;
}

/******************************************************************************/
void *g3duirenderprocess_render_frame_t ( G3DUIRENDERPROCESS *rps ) {
    static GOTOFRAME gtf = { .action = { .done = PTHREAD_MUTEX_INITIALIZER } };

    rps->gui->engine_flags |= LOADFULLRESIMAGES;

    /*** jump to the next frame (this is a image filter, ran on image rendering completion) ***/
    /*gtf.action.wait = 0x00;*/
    gtf.action.type = ACTION_GOTOFRAME;
    gtf.action.gui  = rps->gui;
    gtf.frame       = rps->gui->curframe;

    g3duicom_requestActionFromMainThread ( rps->gui, &gtf );

    /*r3dscene_render_frame_t ( rps->rsce );*/
    q3djob_render_frame_t ( rps->qjob );

    rps->gui->engine_flags &= (~LOADFULLRESIMAGES);

    return NULL;
}

/******************************************************************************/
void *g3duirenderprocess_render_sequence_t ( G3DUIRENDERPROCESS *rps ) {
    static GOTOFRAME gtf = { .action = { .done = PTHREAD_MUTEX_INITIALIZER } };

    rps->gui->engine_flags |= LOADFULLRESIMAGES;

    /*** jump to the next frame (this is a image filter, ran on image rendering completion) ***/
    /*gtf.action.wait = 0x00;*/
    gtf.action.type = ACTION_GOTOFRAME;
    gtf.action.gui  = rps->gui;
    gtf.frame       = rps->gui->currsg->output.startframe;

    g3duicom_requestActionFromMainThread ( rps->gui, &gtf );

    r3dscene_render_sequence_t ( rps->rsce );

    rps->gui->engine_flags &= (~LOADFULLRESIMAGES);

    return NULL;
}

/******************************************************************************/
void g3duirenderprocess_free ( G3DUIRENDERPROCESS *rps ) {
    /*r3dfilter_free ( rps->filter_to_window );
    r3dfilter_free ( rps->filter_to_buffer );*/

    /*if ( rps->filename ) free ( rps->filename );*/

    r3dscene_render_t_free ( rps->rsce );

    /*free ( rps );*/
}

/******************************************************************************/
void g3duirenderprocess_filename ( G3DUIRENDERPROCESS *rps, char *filename ) {
    uint32_t len = strlen ( filename ) + 0x01;

    rps->filename = realloc ( rps->filename, len );

    memset ( rps->filename, 0x00, len );

    memcpy ( rps->filename, filename, len );
}
