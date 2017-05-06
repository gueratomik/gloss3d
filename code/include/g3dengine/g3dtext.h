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
#ifndef _G3DTEXT_H_
#define _G3DTEXT_H_

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @struct G3DTEXT
 * @brief A structure to store a 3D text.
 */
typedef struct _G3DTEXT {
    G3DMESH mes;
    FT_Face face;
    char *fontFaceName;
    char *text;
    GLenum triangleTesselationType;
    int vertexCount;
    G3DVERTEX *verTab[0x03];
    LIST *lcurVertices;
} G3DTEXT;

/******************************************************************************/

uint32_t g3dtext_draw ( G3DOBJECT *obj, G3DCAMERA *curcam, 
                                        uint32_t   engine_flags );
void g3dtext_free ( G3DOBJECT *obj );
void g3dtext_init ( G3DTEXT *spline, uint32_t id, 
                                     char    *name,
                                     uint32_t engine_flags );
G3DTEXT *g3dtext_new ( uint32_t id, char *name, uint32_t engine_flags );

#ifdef __cplusplus
}
#endif

#endif
