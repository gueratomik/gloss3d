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
void g3dimage_setFileName ( G3DIMAGE *img, 
                            char     *filename ) {
    img->filename = strdup ( filename );
}

/******************************************************************************/
void g3dimage_writeToDisk ( G3DIMAGE *img ) {
    if ( img->flags & STILLIMAGE ) {
        if ( img->flags & JPGIMAGE ) {
            g3dcore_writeJpeg ( img->filename,
                                img->width,
                                img->height,
                                img->bytesPerPixel,
                                img->data );

            img->flags = img->flags & (~ALTEREDIMAGE);
        }
    }
}

/******************************************************************************/
void g3dimage_getVideoSize ( G3DIMAGE *image,
                             uint64_t engine_flags ) {
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
    char commandLine[BUFFERLEN];

    if ( sysinfo->ffprobePath ) {
        snprintf ( commandLine, BUFFERLEN, 
    #ifdef __linux__
                                           "\"%s\""
    #endif
    #ifdef __MINGW32__
                                           "cmd /c \"\"%s\""
    #endif
                                           " -v error"
                                           " -of flat=s=_"
                                           " -select_streams v:0"
                                           " -show_entries"
                                           " stream=height,width"
    #ifdef __linux__
                                           " \"%s\"",
    #endif
    #ifdef __MINGW32__
                                           " \"%s\"\"",
    #endif
                                           sysinfo->ffprobePath,
                                           image->filename );

        FILE *fp = popen (commandLine, "r" );

        if ( fp ) {
            char line[BUFFERLEN];

            while ( ( fgets ( line, BUFFERLEN, fp ) ) != NULL ) {
                printf("line: %s\n", line);
                sscanf ( line, "streams_stream_0_width=%d", &image->width );
                sscanf ( line, "streams_stream_0_height=%d", &image->height );
            }
            pclose ( fp );
        }

        image->bytesPerPixel = image->previewBytesPerPixel = 3;
        image->bytesPerLine  = image->bytesPerPixel * image->width;
    }
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
                        uint64_t engine_flags ) {
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

    if ( sysinfo->ffmpegPath ) {
        snprintf ( commandLine, BUFFERLEN,
    #ifdef __linux__
                                           "\"%s\""
    #endif
    #ifdef __MINGW32__
                                           "cmd /c \"\"%s\""
    #endif
                                           " -ss %02d:%02d:%02d.%03d"
                                           " -hide_banner"
                                           " -loglevel panic"
                                           " -i \"%s\""
                                           " -frames:v %d"
                                           " -f rawvideo"
                                           " -pix_fmt rgb24"
                                           " -vf scale=%d:%d"
    #ifdef __linux__
                                           " pipe:1",
    #endif
    #ifdef __MINGW32__
                                           " pipe:1\"",
    #endif
                                           sysinfo->ffmpegPath,
                                           deltaHours, 
                                           deltaMinutes,
                                           deltaSeconds,
                                           deltaMillis,
                                           animationFileName,
                                           nbRequestedFrames,
                                           width, height );

        FILE *fp = popen ( commandLine, "r" );

        if ( fp ) {
            fread ( data, height, ( bytesPerPixel * width ) * NBPREVIEWS, fp );

            pclose ( fp );
        }
    }
}

/******************************************************************************/
void g3dimage_resize ( G3DIMAGE *img, 
                       uint32_t  newWidth, 
                       uint32_t  newHeight ) {
    uint32_t newMemsize = newWidth * newHeight * img->bytesPerPixel;
    unsigned char *data = ( unsigned char * ) malloc ( newMemsize );
    float wratio = ( float ) img->width  / newWidth,
          hratio = ( float ) img->height / newHeight;
    uint32_t i, j;

    for ( i = 0x00; i < newHeight; i++ ) {
        uint32_t oi = i * hratio;

        for ( j = 0x00; j < newWidth; j++ ) {
            uint32_t oj = j * wratio;
            uint32_t newOffset = (  i * newWidth   ) +  j;
            uint32_t oldOffset = ( oi * img->width ) + oj;

            switch ( img->bytesPerPixel ) {
                case 0x03 : {
                    unsigned char (*newData)[0x03] = data;
                    unsigned char (*oldData)[0x03] = img->data;

                    newData[newOffset][0x00] = oldData[oldOffset][0x00];
                    newData[newOffset][0x01] = oldData[oldOffset][0x01];
                    newData[newOffset][0x02] = oldData[oldOffset][0x02];
                } break;

                default :
                break;
            }
        }
    }

    free ( img->data );

    img->width        = newWidth;
    img->height       = newHeight;
    img->data         = data;
    img->bytesPerLine = img->bytesPerPixel * newWidth;

    if ( img->flags & GLIMAGE ) g3dimage_bind ( img );

    img->flags |= ALTEREDIMAGE;
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
                                      image->bytesPerPixel,
                                      image->previewWidth,
                                      image->previewHeight,
                                      0x00,
                                      GL_RGB,
                                      GL_UNSIGNED_BYTE,
                                     &image->previewData[offset] );
    } else {
        glTexImage2D ( GL_TEXTURE_2D, 0x00,
                                      image->bytesPerPixel,
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
                             uint64_t engine_flags ) {
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
                        uint64_t engine_flags ) {
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
                              uint32_t    bindGL ) {
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );

    image->flags |= ( ANIMATEDIMAGE | ( ( bindGL ) ? GLIMAGE : 0x00 ) ); 

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
                             uint32_t    bindGL ) {
    unsigned char *jpgdata = NULL;
    uint32_t jpgwidth,
             jpgheight,
             jpgdepth;
    GLint max;

    img->flags |= ( STILLIMAGE | JPGIMAGE | ( ( bindGL ) ? GLIMAGE : 0x00 ) );

    g3dcore_loadJpeg ( filename, &jpgwidth,
                                 &jpgheight,
                                 &jpgdepth, 
                                 &jpgdata );

    if ( jpgdata == NULL ) {
        fprintf ( stderr, "Could not load image %s!\n", filename );

        return;
    }

    img->bytesPerPixel = jpgdepth;

    /*** power of two is commented out. All images must be of power of two ***/
    if ( /*poweroftwo &&*/ ( jpgdepth == 0x03 ) ) {
        uint32_t imgwidth  = g3dcore_getNextPowerOfTwo ( jpgwidth  ),
                 imgheight = g3dcore_getNextPowerOfTwo ( jpgheight );
        uint32_t oribytesperline = jpgwidth * jpgdepth,
                 imgbytesperline = imgwidth * jpgdepth;
        unsigned char (*oridata)[0x03] = jpgdata;
        unsigned char (*imgdata)[0x03];
        uint32_t i, j;

        img->width  = imgwidth;
        img->height = imgheight;
        img->wratio = ( float ) jpgwidth  / img->width;
        img->hratio = ( float ) jpgheight / img->height;

        img->data = imgdata = calloc ( imgheight, imgbytesperline );

        /* gluScaleImage crashes sometimes, I don't know why */
        /*gluScaleImage ( GL_RGB, 
                        jpgwidth, 
                        jpgheight, 
                        GL_UNSIGNED_BYTE, 
                        jpgdata,
                        imgwidth,
                        imgheight, 
                        GL_UNSIGNED_BYTE, imgdata );*/

        for ( i = 0x00; i < imgheight; i++ ) {
            uint32_t ni = i * img->hratio;

            if ( ni < jpgheight ) {
                for ( j = 0x00; j < imgwidth; j++ ) {
                    uint32_t nj = j * img->wratio;

                    if ( nj < jpgwidth ) {
                        uint32_t imgoffset = ( ( imgheight - i - 1 ) * imgwidth ) +  j,
                                 orioffset = ( ni * jpgwidth ) + nj;

                        imgdata[imgoffset][0] = oridata[orioffset][0];
                        imgdata[imgoffset][1] = oridata[orioffset][1];
                        imgdata[imgoffset][2] = oridata[orioffset][2];
                    }
                }
            }
        }

        img->bytesPerLine = imgbytesperline;

        if ( bindGL ) g3dimage_bind ( img );
    }/* else {
        img->width  = jpgwidth;
        img->height = jpgheight;

        img->data   = jpgdata;

        img->bytesPerLine  = img->bytesPerPixel * img->width;
    }*/



    free ( jpgdata );
}

/******************************************************************************/
G3DIMAGE *g3dimage_load ( const char *filename, uint32_t bindGL ) {
    G3DIMAGE *img = ( G3DIMAGE * ) calloc ( 0x01, sizeof ( G3DIMAGE ) );
    char *extension = strrchr ( filename, '.' );

    if ( img == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    if ( filename ) {
        img->filename = strdup ( filename );
    }

    glGenTextures ( 0x01, &img->id );

    if ( extension ) {
        if ( ( strcasecmp ( extension, ".jpg"  ) == 0x00 ) ||
             ( strcasecmp ( extension, ".jpeg" ) == 0x00 ) ) {
            g3dimage_initFromJpeg ( img, filename, bindGL );
        }

        if ( ( strcasecmp ( extension, ".avi" ) == 0x00 ) ||
             ( strcasecmp ( extension, ".mkv" ) == 0x00 ) ||
             ( strcasecmp ( extension, ".gif" ) == 0x00 ) ||
             ( strcasecmp ( extension, ".mp4" ) == 0x00 ) ||
             ( strcasecmp ( extension, ".flv" ) == 0x00 ) ) {
            g3dimage_initFromVideo ( img, filename, bindGL );
        }
    }

    return img;
}

/******************************************************************************/
G3DIMAGE * g3dimage_new ( uint32_t width, 
                          uint32_t height, 
                          uint32_t bpp,
                          uint32_t bindGL ) {
    void *memarea =  calloc ( 0x01, sizeof ( G3DIMAGE ) );
    G3DIMAGE *img = ( G3DIMAGE * ) memarea;
    uint32_t memsize;

    if ( img == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    img->flags |= ( STILLIMAGE | ( ( bindGL ) ? GLIMAGE : 0x00 ) );

    img->width         = width;
    img->height        = height;
    img->bytesPerPixel = ( bpp / 0x08 );
    img->bytesPerLine  = width * img->bytesPerPixel;

    memsize = img->height * img->bytesPerLine;

    img->data = malloc ( memsize );

    memset ( img->data, 0xFF, memsize );

    if ( bindGL ) {
        glGenTextures ( 0x01, &img->id );

        g3dimage_bind ( img );
    }

    return img;
}

/******************************************************************************/
void g3dimage_free ( G3DIMAGE *img ) {
    if ( img->data        ) free ( img->data        );
    if ( img->previewData ) free ( img->previewData );

    free ( img );
}
