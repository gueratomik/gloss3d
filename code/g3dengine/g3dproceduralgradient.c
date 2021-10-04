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
void g3dproceduralgradient_copySettings ( G3DPROCEDURALGRADIENT *gradient, 
                                          G3DPROCEDURALGRADIENT *pout ) {
    uint32_t settingsSize = sizeof ( G3DPROCEDURALGRADIENT ) - 
                            sizeof ( G3DPROCEDURAL );

    memcpy ( ( char * ) pout     + sizeof ( G3DPROCEDURAL ),
             ( char * ) gradient + sizeof ( G3DPROCEDURAL ), settingsSize );
}

/******************************************************************************/
static void getColor ( G3DPROCEDURAL *proc, 
                       double         x, 
                       double         y,
                       double         z,
                       G3DCOLOR      *color ) {
    G3DPROCEDURALGRADIENT *grd = ( G3DPROCEDURALGRADIENT * ) proc;

    if ( grd->horizontal ) {
        float u = ( x >= 0.0f ) ?          ( x - ( int ) x ) : 
                                  ( 1.0f - ( x - ( int ) x ) ),
              invu = ( 1.0f - u );

        color->r = ( grd->color1.r * invu ) + ( grd->color2.r * u );
        color->g = ( grd->color1.g * invu ) + ( grd->color2.g * u );
        color->b = ( grd->color1.b * invu ) + ( grd->color2.b * u );
    } else {
        float v = ( y >= 0.0f ) ?          ( y - ( int ) y ) : 
                                  ( 1.0f - ( y - ( int ) y ) ),
              invv = ( 1.0f - v );

        color->r = ( grd->color1.r * invv ) + ( grd->color2.r * v );
        color->g = ( grd->color1.g * invv ) + ( grd->color2.g * v );
        color->b = ( grd->color1.b * invv ) + ( grd->color2.b * v );
    }
}

/******************************************************************************/
G3DPROCEDURALGRADIENT *g3dproceduralgradient_new ( ) {
    G3DPROCEDURALGRADIENT *grd = ( G3DPROCEDURALGRADIENT * ) calloc ( 0x01, sizeof ( G3DPROCEDURALGRADIENT ) );

    if ( grd == NULL ) {
        fprintf ( stderr, "%s : calloc failed\n", __func__ );

        return NULL;
    }

    g3dprocedural_init ( ( G3DPROCEDURAL * ) grd, PROCEDURALGRADIENT, 0x00, getColor );

    grd->color1.r = grd->color1.g = grd->color1.b = ( float ) 0xFF / 255.0f;
    grd->color2.r = grd->color2.g = grd->color2.b = ( float ) 0x00 / 255.0f;

    return grd;
}
