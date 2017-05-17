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

/******************************** Static MACROS *******************************/
#define BUFFERLEN 0x200
#define NBPREVIEWS 24
#define PREVIEWSIZE 128

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

    snprintf ( commandLine, BUFFERLEN, "\"%s\""
                                       " -v error"
                                       " -of flat=s=_"
                                       " -select_streams v:0"
                                       " -show_entries"
                                       " stream=height,width"
                                       " \"%s\"", sysinfo->ffprobePath,
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

    image->bytesPerPixel = image->previewBytesPerPixel = 3;
    image->bytesPerLine  = image->bytesPerPixel * image->width;
}

/******************************************************************************/
static void loadFrame ( char     *animationFileName,
                        uint32_t  width,
                        uint32_t  height,
                        uint32_t  bytesPerPixel,
                        char     *data,
                        uint32_t  startFrame, 
                        uint32_t  previewFromFrame,
                        uint32_t  previewToFrame,
                        uint32_t  endFrame,
                        uint32_t  frameRate,
                        uint32_t  engine_flags ) {
    uint32_t nbRequestedFrames = previewToFrame - previewFromFrame;
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
    float    millis  = ( 1000 * ( previewFromFrame - startFrame ) ) / frameRate;
    uint32_t seconds = millis  / 1000,
             minutes = seconds / 60,
             hours   = minutes / 60;
    uint32_t deltaMillis  = millis  - ( seconds * 1000 ),
             deltaSeconds = seconds - ( minutes * 60 ),
             deltaMinutes = minutes - ( hours * 60 ),
             deltaHours   = hours;


    char commandLine[BUFFERLEN];

    snprintf ( commandLine, BUFFERLEN, "\"%s\""
                                       " -ss %02d:%02d:%02d.%03d"
                                       " -hide_banner"
                                       " -loglevel panic"
                                       " -i \"%s\""
                                       " -frames:v %d"
                                       " -f rawvideo"
                                       " -pix_fmt rgb24"
                                       " -vf scale=%d:%d"
                                       " pipe:1", sysinfo->ffmpegPath,
                                                  deltaHours, 
                                                  deltaMinutes,
                                                  deltaSeconds,
                                                  deltaMillis,
                                                  animationFileName,
                                                  nbRequestedFrames,
                                                  width, height );

    #ifdef __linux__
    FILE *fp = popen ( commandLine, "r" );

    if ( fp ) {
        fread ( data, height, ( bytesPerPixel * width ) * NBPREVIEWS, fp );

        pclose ( fp );
    }
    #endif
    #ifdef __MINGW32__
    fprintf ( stderr, "%s not implemented for Windows systems!\n", __func__ );
    #endif
}

/******************************************************************************/
void g3dimage_bind ( G3DIMAGE *image ) {
    glEnable ( GL_TEXTURE_2D );

    /*glActiveTextureARB ( GL_TEXTURE0 );*/

    glBindTexture ( GL_TEXTURE_2D, image->id );

    /* 
     * Both line are mandatory or else OpenGL will expect MipMapping.
     * https://www.khronos.org/opengl/wiki/Common_Mistakes#Creating_a_complete_texture
     */
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_BASE_LEVEL, 0 );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_MAX_LEVEL, 0 );

    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT );
    glTexParameteri ( GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT );

    if ( image->previewData ) {
        uint32_t bytesPerPreview = ( image->previewHeight * 
                                     image->previewWidth  * 
                                     image->previewBytesPerPixel );
        uint32_t offset = ( image->previewId * bytesPerPreview );

        glTexImage2D ( GL_TEXTURE_2D, 0x00,
                                      0x03,
                                      image->previewWidth,
                                      image->previewHeight,
                                      0x00,
                                      GL_RGB,
                                      GL_UNSIGNED_BYTE,
                                     &image->previewData[offset] );
    } else {
        glTexImage2D ( GL_TEXTURE_2D, 0x00,
                                      0x03,
                                      image->width,
                                      image->height,
                                      0x00,
                                      GL_RGB,
                                      GL_UNSIGNED_BYTE,
                                      image->data );

    }

    glDisable ( GL_TEXTURE_2D );
}

/******************************************************************************/
void g3dimage_loadPreviews ( G3DIMAGE *image,
                             uint32_t  previewFromFrame,
                             uint32_t  previewToFrame,
                             uint32_t  sceneStartFrame,
                             uint32_t  sceneEndFrame,
                             uint32_t  sceneFramesPerSecond,
                             uint32_t  engine_flags ) {
    uint32_t bytesPerPreview = ( image->previewHeight * 
                                 image->previewWidth  * 
                                 image->previewBytesPerPixel );
    uint32_t nbPreviews = previewToFrame - previewFromFrame;
    unsigned char *previewData;
    uint32_t i;

    image->previewFromFrame = previewFromFrame;
    image->previewToFrame   = previewToFrame;

    image->previewData = realloc ( image->previewData, ( bytesPerPreview * 
                                                         nbPreviews ) );
    previewData = image->previewData;

    loadFrame ( image->filename,
                image->previewWidth, 
                image->previewHeight,
                image->previewBytesPerPixel,
                previewData,
                sceneStartFrame,     /* startFrame       */
                previewFromFrame,    /* previewFromFrame */
                previewToFrame,      /* previewFromFrame */
                sceneEndFrame,       /* endFrame         */
                sceneFramesPerSecond,/* 3D scene FPS     */
                engine_flags );

    previewData += bytesPerPreview;

}

/******************************************************************************/
void g3dimage_animate ( G3DIMAGE *image,
                        float     sceneStartFrame, 
                        float     currentFrame,
                        float     sceneEndFrame,
                        float     sceneFramesPerSecond,
                        uint32_t  engine_flags ) {
    uint32_t deltaFrame = ( currentFrame - sceneStartFrame );
    uint32_t bytesPerPreview = ( image->previewHeight * 
                                 image->previewWidth  * 
                                 image->previewBytesPerPixel );
    uint32_t deltaPreview = ( deltaFrame / NBPREVIEWS );
    uint32_t previewFromFrame = sceneStartFrame + ( deltaPreview * NBPREVIEWS );
    /* We want one second of previews */
    uint32_t previewToFrame = previewFromFrame + NBPREVIEWS;

    if ( engine_flags & LOADFULLRESIMAGES ) {
        loadFrame ( image->filename,
                    image->width,
                    image->height,
                    image->bytesPerPixel,
                    image->data,
                    sceneStartFrame, 
                    currentFrame,
                    currentFrame + 1,
                    sceneEndFrame,
                    sceneFramesPerSecond,
                    engine_flags );
    }

    /* 
     * If the current set of previews does not match the required time segments
     * generate a new set.
     */
    if ( ( previewFromFrame != image->previewFromFrame ) ||
         ( previewToFrame   != image->previewToFrame ) ) {
        g3dimage_loadPreviews ( image,
                                previewFromFrame,
                                previewToFrame,
                                sceneStartFrame,
                                sceneEndFrame,
                                sceneFramesPerSecond,
                                engine_flags );
    }

    image->previewId = deltaFrame % NBPREVIEWS;
}

/******************************************************************************/
void g3dimage_initFromVideo ( G3DIMAGE   *image, 
                              const char *filename, 
                              uint32_t    poweroftwo ) {
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );

    g3dimage_getVideoSize ( image, 0x00 );

    image->bytesPerLine = ( image->width * image->bytesPerPixel );
    image->data = malloc ( image->height * image->bytesPerLine );

    /* Adjust video size so for faster processing */
    if ( image->height && image->width ) {
        float ratio = ( float ) image->width / image->height;

        /* width > height */
        if ( ratio > 1.0f ) {
            if ( image->width > PREVIEWSIZE ) {
                image->previewWidth = PREVIEWSIZE;
                image->previewHeight = ( float ) image->previewWidth / ratio;
            }
        } else {
            if ( image->height > PREVIEWSIZE ) {
                image->previewHeight = PREVIEWSIZE;
                image->previewWidth  = ( float ) image->previewHeight * ratio;
            }
        }
    }

    /* Load a first set of previews */
    g3dimage_loadPreviews ( image, 0, NBPREVIEWS, 0, 24, 24, 0x00 );

    list_insert ( &sysinfo->lanimatedImages, image );
}

/******************************************************************************/
void g3dimage_initFromJpeg ( G3DIMAGE   *img,
                             const char *filename,
                             uint32_t    poweroftwo ) {
    unsigned char *jpgdata = NULL;
    uint32_t jpgwidth,
             jpgheight,
             jpgdepth;
    GLint max;

    g3dcore_loadJpeg ( filename, &jpgwidth,
                                 &jpgheight,
                                 &jpgdepth, 
                                 &jpgdata );

    if ( jpgdata == NULL ) {
        fprintf ( stderr, "Could not load image %s!\n", filename );

        return;
    }

    img->bytesPerPixel = jpgdepth;

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

        img->bytesPerLine  = imgbytesperline;

        free ( jpgdata );
    } else {
        img->width  = jpgwidth;
        img->height = jpgheight;

        img->data   = jpgdata;

        img->bytesPerLine  = img->bytesPerPixel * img->width;
    }
}

/******************************************************************************/
G3DIMAGE *g3dimage_new ( const char *filename, uint32_t poweroftwo ) {
    G3DIMAGE *img = ( G3DIMAGE * ) calloc ( 0x01, sizeof ( G3DIMAGE ) );
    char *extension = strrchr ( filename, '.' );

    if ( img == NULL ) {
        fprintf ( stderr, "g3dimage_new(): calloc failed\n" );

        return NULL;
    }

    if ( filename ) {
        img->filename = strdup ( filename );
    }

    glGenTextures ( 0x01, &img->id );

    if ( extension ) {
        if ( ( strcasecmp ( extension, ".jpg"  ) == 0x00 ) ||
             ( strcasecmp ( extension, ".jpeg" ) == 0x00 ) ) {
            g3dimage_initFromJpeg ( img, filename, poweroftwo );
        }

        if ( ( strcasecmp ( extension, ".avi" ) == 0x00 ) ||
             ( strcasecmp ( extension, ".mkv" ) == 0x00 ) ||
             ( strcasecmp ( extension, ".gif" ) == 0x00 ) ||
             ( strcasecmp ( extension, ".mp4" ) == 0x00 ) ||
             ( strcasecmp ( extension, ".flv" ) == 0x00 ) ) {
            g3dimage_initFromVideo ( img, filename, poweroftwo );
        }
    }

    return img;
}

/******************************************************************************/
void g3dimage_free ( G3DIMAGE *img ) {
    if ( img->data        ) free ( img->data        );
    if ( img->previewData ) free ( img->previewData );

    free ( img );
}
