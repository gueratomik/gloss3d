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
#include <g3dexportv3.h>

/******************************************************************************/
static uint32_t g3dexportv3text_thickness ( G3DEXPORTV3DATA *ged, 
                                          G3DTEXT       *txt, 
                                          uint32_t       flags, 
                                          FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &txt->thickness, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3text_roundness ( G3DEXPORTV3DATA *ged, 
                                          G3DTEXT       *txt, 
                                          uint32_t       flags, 
                                          FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &txt->roundness, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3text_string ( G3DEXPORTV3DATA *ged, 
                                       G3DTEXT       *txt, 
                                       uint32_t       flags, 
                                       FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwrite ( txt->text, strlen ( txt->text ), 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3text_fontFile ( G3DEXPORTV3DATA *ged, 
                                         G3DTEXT       *txt, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t len = strlen ( txt->fontFaceFile );
    uint32_t size = 0x00;

    size += g3dexportv3_fwrite ( txt->fontFaceFile, len, 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3text_fontFace ( G3DEXPORTV3DATA *ged, 
                                         G3DTEXT       *txt, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t len = strlen ( txt->fontFaceName );
    uint32_t size = 0x00;

    size += g3dexportv3_fwrite ( txt->fontFaceName, len, 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3text_fontSize ( G3DEXPORTV3DATA *ged, 
                                         G3DTEXT       *txt, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &txt->fontFaceSize, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3text_font ( G3DEXPORTV3DATA *ged, 
                                     G3DTEXT       *txt, 
                                     uint32_t       flags, 
                                     FILE          *fdst ) {
    uint32_t size = 0x00;

    if ( txt->fontFaceFile ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_TEXT_FONT_FILE,
                                       g3dexportv3text_fontFile,
                                       ged,
                                       txt,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( txt->fontFaceName ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_TEXT_FONT_FACE,
                                       g3dexportv3text_fontFace,
                                       ged,
                                       txt,
                                       0xFFFFFFFF,
                                       fdst );
    }

    size += g3dexportv3_writeChunk ( SIG_OBJECT_TEXT_FONT_SIZE,
                                   g3dexportv3text_fontSize,
                                   ged,
                                   txt,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
uint32_t g3dexportv3text ( G3DEXPORTV3DATA *ged, 
                         G3DTEXT       *txt, 
                         uint32_t       flags, 
                         FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_TEXT_FONT,
                                   g3dexportv3text_font,
                                   ged,
                                   txt,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_TEXT_THICKNESS,
                                   g3dexportv3text_thickness,
                                   ged,
                                   txt,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_TEXT_ROUNDNESS,
                                   g3dexportv3text_roundness,
                                   ged,
                                   txt,
                                   0xFFFFFFFF,
                                   fdst );

    if ( txt->text ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_TEXT_STRING,
                                       g3dexportv3text_string,
                                       ged,
                                       txt,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}
