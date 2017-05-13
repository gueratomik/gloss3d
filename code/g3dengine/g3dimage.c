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
#include <g3dengine/g3dengine.h>

/******************************** Static MACROS *******************************/
#define BUFFERLEN 0x200

/**************************** Static declarations *****************************/
static G3DIMAGE *g3dimage_new ( const char * );

/*
ffprobe -v error -of flat=s=_ -select_streams v:0 -show_entries stream=height,width capture.avi 
streams_stream_0_width=640
streams_stream_0_height=480


ffmpeg -ss 00:00:00.100 -i Untitled.avi.avi -frames:v 1 -f rawvideo -pix_fmt rgb24 pipe:1
*/

/******************************************************************************/
void g3dimage_getVideoSize ( G3DIMAGE *image,
                             uint32_t  engine_flags ) {
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
    char commandLine[BUFFERLEN];

    snprintf ( commandLine, BUFFERLEN, "%s"
                                       " -v error"
                                       " -of flat=s=_"
                                       " -select_streams v:0"
                                       " -show_entries"
                                       " stream=height,width"
                                       " %s", sysinfo->ffprobePath,
                                              image->filename );

    #ifdef __linux__
    FILE *fp = popen (commandLine, "r" );

    if ( fp ) {
        char line[BUFFERLEN];

        while ( ( fgets ( line, BUFFERLEN, fp ) ) != NULL ) {
            sscanf ( line, "streams_stream_0_width=%d", &image->width );
            sscanf ( line, "streams_stream_0_height=%d", &image->height );
        }
        pclose ( fp );
    }
    #endif
    #ifdef __MINGW32__
    fprintf ( stderr, "%s not implemented for Windows systems!\n", __func__ );
    #endif

    image->depth         = 24;
    image->bytesperpixel =  3;
    image->bytesperline  = image->bytesperpixel * image->width;
}

/******************************************************************************/
void g3dimage_animate ( G3DIMAGE *image,
                        float     startFrame, 
                        float     currentFrame,
                        float     endFrame,
                        float     frameRate,
                        uint32_t  engine_flags ) {
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
    float    millis  = ( currentFrame - startFrame ) / frameRate * 1000;
    uint32_t seconds = millis  / 1000,
             minutes = seconds / 60,
             hours   = minutes / 60;
    uint32_t deltaMillis  = millis  - ( seconds * 1000 ),
             deltaSeconds = seconds - ( minutes * 60 ),
             deltaMinutes = minutes - ( hours * 60 ),
             deltaHours   = hours;

    char commandLine[BUFFERLEN];

    snprintf ( commandLine, BUFFERLEN, "%s"
                                       " -ss %02d:%02d:%02d.%03d"
                                       " -i %s"
                                       " -frames:v 1"
                                       " -f rawvideo"
                                       " -pix_fmt rgb24"
                                       " pipe:1", sysinfo->ffmpegPath,
                                                  deltaHours, 
                                                  deltaMinutes,
                                                  deltaSeconds,
                                                  deltaMillis,
                                                  image->filename );

    #ifdef __linux__
    FILE *fp = popen (commandLine, "r" );

    if ( fp ) {
        fread ( image->data, image->height, image->bytesperline, fp );

        pclose ( fp );
    }
    #endif
    #ifdef __MINGW32__
    fprintf ( stderr, "%s not implemented for Windows systems!\n", __func__ );
    #endif
}

/******************************************************************************/
void g3dimage_bind ( G3DIMAGE *img ) {
    glEnable ( GL_TEXTURE_2D );

    glGenTextures ( 0x01, &img->id );

    /*glActiveTextureARB ( GL_TEXTURE0 );*/

    glBindTexture ( GL_TEXTURE_2D, img->id );

    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    glTexImage2D ( GL_TEXTURE_2D, 0x00,
                                  0x03,
                                  img->width,
                                  img->height,
                                  0x00,
                                  GL_RGB,
                                  GL_UNSIGNED_BYTE,
                                  img->data );

    #ifdef __linux__
    glGenerateMipmap ( GL_TEXTURE_2D );
    #endif
    #ifdef __MINGW32__
    if ( ext_glGenerateMipmap ) ext_glGenerateMipmap ( GL_TEXTURE_2D );
    #endif

    glDisable ( GL_TEXTURE_2D );
}

/******************************************************************************/
G3DIMAGE *g3dimage_newFromVideo ( const char *filename, uint32_t poweroftwo ) {
    G3DIMAGE *img = g3dimage_new ( filename );

    g3dimage_getVideoSize ( img, 0x00 );

    img->data = malloc ( img->height * img->bytesperline );

    return img;
}

/******************************************************************************/
G3DIMAGE *g3dimage_newFromJpeg ( const char *filename, uint32_t poweroftwo ) {
    unsigned char *jpgdata = NULL;
    uint32_t jpgwidth,
             jpgheight,
             jpgdepth;
    G3DIMAGE *img;
    GLint max;

    g3dcore_loadJpeg ( filename, &jpgwidth,
                                 &jpgheight,
                                 &jpgdepth, 
                                 &jpgdata );

    if ( jpgdata == NULL ) {
        fprintf ( stderr, "Could not load image %s!\n", filename );

        return NULL;
    }

    img = g3dimage_new ( filename );

    img->depth  = jpgdepth * 0x08;

    img->bytesperpixel = jpgdepth;

    if ( poweroftwo ) {
        uint32_t imgwidth  = g3dcore_getNextPowerOfTwo ( jpgwidth  ),
                 imgheight = g3dcore_getNextPowerOfTwo ( jpgheight );
        uint32_t jpgbytesperline = jpgwidth * jpgdepth,
                 imgbytesperline = imgwidth * jpgdepth;
        unsigned char *imgdata;
        uint32_t i;

        img->width  = imgwidth;
        img->height = imgheight;
        img->wratio = ( float ) jpgwidth  / img->width;
        img->hratio = ( float ) jpgheight / img->height;

        img->data = imgdata = calloc ( imgheight, imgbytesperline );

        gluScaleImage ( GL_RGB, 
                        jpgwidth, 
                        jpgheight, 
                        GL_UNSIGNED_BYTE, 
                        jpgdata,
                        imgwidth,
                        imgheight, 
                        GL_UNSIGNED_BYTE, imgdata );

        /*for ( i = 0x00; i < jpgheight; i++ ) {
            uint32_t imgoffset = ( i * imgbytesperline ),
                     jpgoffset = ( i * jpgbytesperline );

            memcpy ( &imgdata[imgoffset],
                     &jpgdata[jpgoffset], jpgbytesperline );
        }*/

        img->bytesperline  = imgbytesperline;

        free ( jpgdata );
    } else {
        img->width  = jpgwidth;
        img->height = jpgheight;

        img->data   = jpgdata;

        img->bytesperline  = img->bytesperpixel * img->width;
    }

    return img;
}

/******************************************************************************/
static G3DIMAGE *g3dimage_new ( const char *filename ) {
    G3DIMAGE *img = ( G3DIMAGE * ) calloc ( 0x01, sizeof ( G3DIMAGE ) );

    if ( img == NULL ) {
        fprintf ( stderr, "g3dimage_new(): calloc failed\n" );

        return NULL;
    }

    if ( filename ) {
        img->filename = strdup ( filename );
    }

    return img;
}

/******************************************************************************/
void g3dimage_free ( G3DIMAGE *img ) {
    if ( img->data ) free ( img->data );

    free ( img );
}
