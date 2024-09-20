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
#ifndef _G3DTEXT_H_
#define _G3DTEXT_H_

#ifdef __cplusplus
extern "C" {
#endif

/******************************************************************************/
typedef struct _G3DCHARACTER {
    uint32_t code;
    LIST    *vertexList;
    LIST    *edgeList;
    LIST    *faceList;
    uint32_t vertexCount;
    uint32_t edgeCount;
    uint32_t faceCount;
    float    width;
} G3DCHARACTER;

/**
 * @struct G3DTEXT
 * @brief A structure to store a 3D text.
 */
typedef struct _G3DTEXT {
    G3DMESH       mes;
    FT_Face       face;
    char         *fontFaceName;
    char         *fontFaceFile;
    uint32_t      fontFaceSize;
    char         *text;
    GLenum        triangleTesselationType;
    int           vertexCount;
    float         thickness;
    uint32_t      roundness;
    G3DVERTEX    *verTab[0x03];
    LIST         *lchr; /*** list of characters for the current font ***/
    uint32_t      nbchr;
    G3DCHARACTER *currentCharacter; /*** current character being generated ***/
    G3DVECTOR3F   nextCharacterPosition;
    float height;
} G3DTEXT;

/******************************************************************************/

G3DCHARACTER *g3dcharacter_new ( uint32_t code );
void g3dcharacter_empty ( G3DCHARACTER *chr );
void g3dcharacter_free ( G3DCHARACTER *chr );
void g3dcharacter_addVertex ( G3DCHARACTER *chr, G3DVERTEX *ver );
void g3dcharacter_addFace ( G3DCHARACTER *chr, G3DFACE *fac );
G3DCHARACTER *g3dtext_getCharacterByCode ( G3DTEXT *txt, uint32_t code );
void g3dtext_mergeCharacter ( G3DTEXT      *txt, 
                              G3DCHARACTER *chr,
                              uint32_t      invert, /* 0 or 1 */
                              uint64_t engine_flags );
void g3dtext_empty ( G3DTEXT *txt );
void g3dtext_setText ( G3DTEXT *txt,
                       char    *text,
                       uint64_t engine_flags );
void g3dtext_addText ( G3DTEXT *txt, 
                       char    *addedString,
                       uint64_t engine_flags );
void g3dtext_setFont ( G3DTEXT *txt,
                       char    *fontFaceName,
                       char    *fontFaceFile,
                       uint32_t fontFaceSize,
                       uint64_t engine_flags );
G3DCHARACTER *g3dtext_generateCharacter ( G3DTEXT       *txt,
                                          uint32_t       code,
                                          GLUtesselator *tobj,
                                          uint64_t engine_flags );
void g3dcharacter_generateThickness ( G3DCHARACTER *chr,
                                      float         thickness,
                                      uint64_t engine_flags );
void g3dtext_generate ( G3DTEXT   *txt,
                        uint32_t   fromCharacter,
                        uint32_t   toCharacter,
                        uint64_t engine_flags );
void g3dtext_free ( G3DOBJECT *obj );

void g3dtext_init ( G3DTEXT *txt, 
                    uint32_t id, 
                    char    *name,
                    uint64_t engine_flags );
void g3dtext_configure ( G3DTEXT *txt, 
                         char    *fontFaceName,
                         char    *fontFaceFile,
                         uint32_t fontFaceSize,
                         float    thickness,
                         uint32_t roundness,
                         uint64_t engine_flags );

void g3dtext_setRoundness ( G3DTEXT *txt,
                            uint32_t roundness,
                            uint64_t engine_flags );

void g3dtext_setThickness ( G3DTEXT *txt,
                            float    thickness,
                            uint64_t engine_flags );

G3DTEXT *g3dtext_new ( uint32_t id, 
                       char    *name,
                       uint64_t engine_flags );
void g3dtext_setSize ( G3DTEXT *txt,
                       uint32_t fontFaceSize,
                       uint64_t engine_flags );


#ifdef __cplusplus
}
#endif

#endif
