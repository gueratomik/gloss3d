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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2013         */
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

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct G3DIMAGE
 * @brief A structure to store an image that can be used as a texture.
 */
/******************************************************************************/
typedef struct _G3DIMAGE {
    char    *filename;
    uint32_t width;
    uint32_t height;
    uint32_t bytesPerPixel;
    uint32_t bytesPerLine;
    unsigned char *data;
    float    wratio; /* Used when the image dimension must be a power of 2 */
    float    hratio; /* Used when the image dimension must be a power of 2 */
    GLuint   id; /* texture ID */
    char   **previewData;
    uint32_t previewWidth;
    uint32_t previewHeight;
    uint32_t previewBytesPerPixel;
    float    previewFromFrame;
    float    previewToFrame;
} G3DIMAGE;

/******************************************************************************/

/**
 * Create a G3DIMAGE from a jpeg file.
 * @param the jpeg file name.
 * @param the 3D engine flags.
 * @return the allocated G3DIMAGE. Can be freed with g3dimage_free(...).
 */
G3DIMAGE *g3dimage_newFromJpeg ( const char *filename,
                                 uint32_t    engine_flags );


/**
 * Free an image (along with its pixel data)
 * @param a pointer to a previously allocated G3DIMAGE.
 */
void g3dimage_free ( G3DIMAGE *image );


/**
 * Generate OpenGL GLU MipMaps.
 * @param a pointer to a previously allocated G3DIMAGE.
 */
void g3dimage_bind ( G3DIMAGE *image );

/**
 * For animated texture: load the requested image at frame "currentFrame".
 * This function uses startFrame as its starting time, which does not have
 * to be zero. The animated image renders at its own framerate.
 * @param a pointer to a previously allocated G3DIMAGE.
 * @param the starting frame
 * @param the current frame that must be loaded.
 * @param the ending frame
 * @param the frame rate of the 3D engine (not of the animated image).
 * @param the 3D engine flags.
 */
void g3dimage_animate ( G3DIMAGE *image,
                        float     startFrame,
                        float     currentFrame,
                        float     endFrame,
                        float     frameRate,
                        uint32_t  engine_flags );

#ifdef __cplusplus
}
#endif

#endif
