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

/**
 * @file
 */

/******************************************************************************/
#ifndef _G3DIMAGE_H_
#define _G3DIMAGE_H_

/********************************* Image flags ********************************/
#define ALTEREDIMAGE  ( 1 << 0 )
#define ANIMATEDIMAGE ( 1 << 1 )
#define STILLIMAGE    ( 1 << 2 )
#define GLIMAGE       ( 1 << 3 ) /*** OpenGL binded bit ***/
#define JPGIMAGE      ( 1 << 4 )
#define PNGIMAGE      ( 1 << 5 )

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct G3DIMAGE
 * @brief A structure to store an image that can be used as a texture.
 */
/******************************************************************************/
typedef struct _G3DIMAGE {
    uint32_t flags;
    char    *filename;
    uint32_t width;
    uint32_t height;
    uint32_t bytesPerPixel;
    uint32_t bytesPerLine;
    unsigned char *data;
    float    wratio; /* Used when the image dimension must be a power of 2 */
    float    hratio; /* Used when the image dimension must be a power of 2 */
    GLuint   id; /* texture ID */
    uint32_t previewId;
    char    *previewData;
    uint32_t previewWidth;
    uint32_t previewHeight;
    uint32_t previewBytesPerPixel;
    float    previewFromFrame;
    float    previewToFrame;
} G3DIMAGE;

/******************************************************************************/


/**
 * Free an image (along with its pixel data)
 * @param image a pointer to a previously allocated G3DIMAGE.
 */
void g3dimage_free ( G3DIMAGE *image );


/**
 * Generate OpenGL GLU MipMaps.
 * @param image a pointer to a previously allocated G3DIMAGE.
 */
void g3dimage_bind ( G3DIMAGE *image );

/**
 * For animated texture: load the requested image at frame "currentFrame".
 * This function uses startFrame as its starting time, which does not have
 * to be zero. The animated image renders at its own framerate.
 * @param image a pointer to a previously allocated G3DIMAGE.
 * @param startFrame the starting frame
 * @param currentFrame the current frame that must be loaded.
 * @param endFrame the ending frame
 * @param frameRate the frame rate of the 3D engine (not of the animated image).
 * @param engine_flags the 3D engine flags.
 */
void g3dimage_animate ( G3DIMAGE *image,
                        float     startFrame,
                        float     currentFrame,
                        float     endFrame,
                        float     frameRate,
                        uint64_t engine_flags );

/**
 * Create a G3DIMAGE depending on the file extension.
 * @param filename the media file name (image or video).
 * @param poweroftwo.
 * @return the allocated G3DIMAGE. Can be freed with g3dimage_free(...).
 */
G3DIMAGE *g3dimage_load ( const char *filename,
                          uint32_t    poweroftwo );

G3DIMAGE * g3dimage_new ( uint32_t width, 
                          uint32_t height, 
                          uint32_t bpp,
                          uint32_t bindGL );

void g3dimage_setFileName ( G3DIMAGE *img, 
                            char     *filename );

void g3dimage_resize ( G3DIMAGE *image, 
                       uint32_t  newWidth, 
                       uint32_t  newHeight );

#ifdef __cplusplus
}
#endif

#endif
