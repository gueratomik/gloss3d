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
void g3dproceduralchess_copySettings ( G3DPROCEDURALCHESS *chess, 
                                       G3DPROCEDURALCHESS *pout ) {
    uint32_t settingsSize = sizeof ( G3DPROCEDURALCHESS ) - 
                            sizeof ( G3DPROCEDURAL );

    memcpy ( ( char * ) pout  + sizeof ( G3DPROCEDURAL ),
             ( char * ) chess + sizeof ( G3DPROCEDURAL ), settingsSize );
}

/******************************************************************************/
static void getColor ( G3DPROCEDURAL *proc, 
                       double         x, 
                       double         y,
                       double         z,
                       G3DCOLOR      *color ) {
    G3DPROCEDURALCHESS *cbd = ( G3DPROCEDURALCHESS * ) proc;
    float posu = x * ( cbd->udiv * 2 ),
          posv = y * ( cbd->vdiv * 2 );
    uint32_t evenu = ( ( ( uint32_t ) posu ) % 2 ) ? 0 : 1,
             evenv = ( ( ( uint32_t ) posv ) % 2 ) ? 0 : 1;

    if ( ( evenu + evenv ) % 2 ) {
        color->r = cbd->color1.r;
        color->g = cbd->color1.g;
        color->b = cbd->color1.b;
    } else {
        color->r = cbd->color2.r;
        color->g = cbd->color2.g;
        color->b = cbd->color2.b;
    }
}

/******************************************************************************/
G3DPROCEDURALCHESS *g3dproceduralchess_new ( ) {
    G3DPROCEDURALCHESS *cbd = ( G3DPROCEDURALCHESS * ) calloc ( 0x01, sizeof ( G3DPROCEDURALCHESS ) );

    if ( cbd == NULL ) {
        fprintf ( stderr, "%s : calloc failed\n", __func__ );

        return NULL;
    }

    g3dprocedural_init ( ( G3DPROCEDURAL * ) cbd, PROCEDURALCHESS, getColor );

    cbd->color1.r = cbd->color1.g = cbd->color1.b = ( float ) 0xFF / 255.0f;
    cbd->color2.r = cbd->color2.g = cbd->color2.b = ( float ) 0x00 / 255.0f;

    cbd->udiv = 4;
    cbd->vdiv = 4;

    return cbd;
}
