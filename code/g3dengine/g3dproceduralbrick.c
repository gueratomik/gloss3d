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
                       G3DRGBA       *rgba ) {
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
        rgba->r = pbd->spacingColor.r;
        rgba->g = pbd->spacingColor.g;
        rgba->b = pbd->spacingColor.b;
    } else {
        rgba->r = pbd->brickColor.r;
        rgba->g = pbd->brickColor.g;
        rgba->b = pbd->brickColor.b;
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
    pbd->brickColor.r   = 0x8B;
    pbd->brickColor.g   = 0x4F;
    pbd->brickColor.b   = 0x39;

    /*** hhttps://encycolorpedia.com/47281e ***/
    pbd->spacingColor.r = 0x47;
    pbd->spacingColor.g = 0x28;
    pbd->spacingColor.b = 0x1E;

    pbd->nbBricksPerLine = 3;
    pbd->nbLines = 6;

    pbd->uspacing = 0.025f;
    pbd->vspacing = 0.05f;

    return pbd;
}
