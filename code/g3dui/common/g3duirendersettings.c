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
#include <g3dui.h>

/******************************************************************************/
void g3duirendersettings_getFfmpegPath ( G3DUIRENDERSETTINGS *rsg ) {
    FILE *fp = popen ("which ffmpeg", "r" );

    if ( fp ) {
        uint32_t len;

        fgets ( rsg->ffmpegpath, FFMPEGPATHLEN, fp );

        len = strlen ( rsg->ffmpegpath );

        if ( len ) {
            /*** trim the final newline ***/
            rsg->ffmpegpath[len-1] = 0x00;
        }

        pclose ( fp );
    }
}

/******************************************************************************/
void g3duirendersettings_getFfplayPath ( G3DUIRENDERSETTINGS *rsg ) {
    FILE *fp = popen ("which ffplay", "r" );

    if ( fp ) {
        uint32_t len;

        fgets ( rsg->ffplaypath, FFMPEGPATHLEN, fp );

        len = strlen ( rsg->ffplaypath );

        if ( len ) {
            /*** trim the final newline ***/
            rsg->ffplaypath[len-1] = 0x00;
        }

        pclose ( fp );
    }
}

/******************************************************************************/
G3DUIRENDERSETTINGS *g3duirendersettings_new ( ) {
    G3DUIRENDERSETTINGS *rsg = calloc ( 0x01, sizeof ( G3DUIRENDERSETTINGS ) );

    if ( rsg == NULL ) {
        fprintf ( stderr, "g3duirendersettings_new: memory allocation failed\n" );

        return NULL;
    }

    rsg->flags       = 0x00;
    rsg->fps         = 0x18;
    rsg->depth       = 0x18;
    rsg->startframe  = 0x00;
    rsg->endframe    = 0x00;
    rsg->outfile     = "Untitled";
    rsg->format      = RENDERTOIMAGE;
    rsg->width       = 640;
    rsg->height      = 480;
    rsg->ratio       = 1.33f;
    rsg->background  = 0x00404040;

    g3duirendersettings_getFfmpegPath ( rsg );
    g3duirendersettings_getFfplayPath ( rsg );

    rsg->lfilter     = NULL;


    return rsg;
}

