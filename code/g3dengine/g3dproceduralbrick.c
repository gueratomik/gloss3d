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


/***            http://www.massal.net/article/raytrace/page3.html           ***/
/***  http://gamedev.stackexchange.com/questions/23625/how-do-you-generate-tileable-perlin-noise ***/

/******************************************************************************/
static void getColor ( G3DPROCEDURAL *proc, 
                       double         x, 
                       double         y,
                       double         z,
                       G3DCOLOR      *color ) {
    G3DPROCEDURALBRICK *pbd = ( G3DPROCEDURALBRICK * ) proc;
    float brickSizeU = ( 1.0f / pbd->nbBricksPerLine );
    float halfBrickU = brickSizeU / 2;
    float brickSizeV = ( 1.0f / pbd->nbLines         );
    float posv = y / brickSizeV;
    uint32_t evenv = ( ( ( uint32_t ) posv ) % 2 ) ? 0 : 1;
    float posu = ( evenv ) ? x / brickSizeU :  ( x + halfBrickU ) / brickSizeU;

    if ( ( posu - ( uint32_t ) posu ) < (        pbd->uspacing ) ||
         ( posu - ( uint32_t ) posu ) > ( 1.0f - pbd->uspacing ) || 
         ( posv - ( uint32_t ) posv ) < (        pbd->vspacing ) || 
         ( posv - ( uint32_t ) posv ) > ( 1.0f - pbd->vspacing ) ) {
        color->r = pbd->spacingColor.r;
        color->g = pbd->spacingColor.g;
        color->b = pbd->spacingColor.b;
    } else {
        color->r = pbd->brickColor.r;
        color->g = pbd->brickColor.g;
        color->b = pbd->brickColor.b;
    }
}

/******************************************************************************/
G3DPROCEDURALBRICK *g3dproceduralbrick_new ( ) {
    G3DPROCEDURALBRICK *pbd = ( G3DPROCEDURALBRICK * ) calloc ( 0x01, sizeof ( G3DPROCEDURALBRICK ) );

    if ( pbd == NULL ) {
        fprintf ( stderr,  "%s : calloc failed\n", __func__ );

        return NULL;
    }

    g3dprocedural_init ( ( G3DPROCEDURAL * ) pbd, PROCEDURALBRICK, getColor );

    /*** https://encycolorpedia.com/8b4f39 ***/
    pbd->brickColor.r   = ( float ) 0x8B / 255.0f;
    pbd->brickColor.g   = ( float ) 0x4F / 255.0f;
    pbd->brickColor.b   = ( float ) 0x39 / 255.0f;

    /*** hhttps://encycolorpedia.com/47281e ***/
    pbd->spacingColor.r = ( float ) 0x47 / 255.0f;
    pbd->spacingColor.g = ( float ) 0x28 / 255.0f;
    pbd->spacingColor.b = ( float ) 0x1E / 255.0f;

    pbd->nbBricksPerLine = 3;
    pbd->nbLines = 6;

    pbd->uspacing = 0.025f;
    pbd->vspacing = 0.05f;

    return pbd;
}
