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
#include <config.h>
#include <g3dengine/g3dengine.h>


/***            http://www.massal.net/article/raytrace/page3.html           ***/
/***  http://gamedev.stackexchange.com/questions/23625/how-do-you-generate-tileable-perlin-noise ***/

/******************************************************************************/
static int permutation[] = { 151,160,137,91,90,15,
   131,13,201,95,96,53,194,233,7,225,140,36,103,30,69,142,8,99,37,240,21,10,23,
   190, 6,148,247,120,234,75,0,26,197,62,94,252,219,203,117,35,11,32,57,177,33,
   88,237,149,56,87,174,20,125,136,171,168, 68,175,74,165,71,134,139,48,27,166,
   77,146,158,231,83,111,229,122,60,211,133,230,220,105,92,41,55,46,245,40,244,
   102,143,54, 65,25,63,161, 1,216,80,73,209,76,132,187,208, 89,18,169,200,196,
   135,130,116,188,159,86,164,100,109,198,173,186, 3,64,52,217,226,250,124,123,
   5,202,38,147,118,126,255,82,85,212,207,206,59,227,47,16,58,17,182,189,28,42,
   223,183,170,213,119,248,152, 2,44,154,163, 70,221,153,101,155,167, 43,172,9,
   129,22,39,253, 19,98,108,110,79,113,224,232,178,185, 112,104,218,246,97,228,
   251,34,242,193,238,210,144,12,191,179,162,241, 81,51,145,235,249,14,239,107,
   49,192,214, 31,181,199,106,157,184, 84,204,176,115,121,50,45,127, 4,150,254,
   138,236,205,93,222,114,67,29,24,72,243,141,128,195,78,66,215,61,156,180
   };
static int p[512];

/******************************************************************************/
static double fade ( double t ) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}

/******************************************************************************/
static double lerp ( double t, double a, double b ) {
    return a + t * (b - a);
}

/******************************************************************************/
static double grad ( int hash, double x, double y, double z ) {
    int h = hash & 15;                      /* CONVERT LO 4 BITS OF HASH CODE */
    double u = h<8||h==12||h==13 ? x : y,   /* INTO 12 GRADIENT DIRECTIONS. */
           v = h<4||h==12||h==13 ? y : z;
    return ((h&1) == 0 ? u : -u) + ((h&2) == 0 ? v : -v);
}

/******************************************************************************/
double noise(double x, double y, double z) {
    int X = (int)floor(x) & 255,                  /* FIND UNIT CUBE THAT */
        Y = (int)floor(y) & 255,                  /* CONTAINS POINT. */
        Z = (int)floor(z) & 255;
    x -= floor(x);                                /* FIND RELATIVE X,Y,Z */
    y -= floor(y);                                /* OF POINT IN CUBE. */
    z -= floor(z);
    double u = fade(x),                           /* COMPUTE FADE CURVES */
           v = fade(y),                           /* FOR EACH OF X,Y,Z. */
           w = fade(z);
    int A = p[X  ]+Y, AA = p[A]+Z, AB = p[A+1]+Z, /* HASH COORDINATES OF */
        B = p[X+1]+Y, BA = p[B]+Z, BB = p[B+1]+Z; /* THE 8 CUBE CORNERS, */

    return lerp(w, lerp(v, lerp(u, grad(p[AA  ], x  , y  , z   ),  /* AND ADD */
                                   grad(p[BA  ], x-1, y  , z   )), /* BLENDED */
                           lerp(u, grad(p[AB  ], x  , y-1, z   ),  /* RESULTS */
                                   grad(p[BB  ], x-1, y-1, z   ))),/* FROM  8 */
                   lerp(v, lerp(u, grad(p[AA+1], x  , y  , z-1 ),  /* CORNERS */
                                   grad(p[BA+1], x-1, y  , z-1 )), /* OF CUBE */
                           lerp(u, grad(p[AB+1], x  , y-1, z-1 ),
                                   grad(p[BB+1], x-1, y-1, z-1 ))));
}

/******************************************************************************/
static double fbm ( double x, double y, double z ) {
    double coef = 0.0f;
    int i;

    for ( i = 0; i < 10; i++ ) {
        coef += ( pow ( 0.5f, i ) * fabs ( noise ( pow ( 2, i ) * x, 
                                                   pow ( 2, i ) * y,
                                                   pow ( 2, i ) * z ) ) );
    }

    return coef;
}

/******************************************************************************/
static void getColor ( G3DPROCEDURAL *proc, 
                       double         x, 
                       double         y,
                       double         z,
                       G3DRGBA       *rgba ) {
    G3DPROCEDURALNOISE *pns = ( G3DPROCEDURALNOISE * ) proc;
    static int init;
    double coef = 0.0f;
    int i, j;

    if ( init == 0x00 ) {
        for ( i = 0; i < 256 ; i++) {
            p[256+i] = p[i] = permutation[i];
        }

        init = 0x01;
    }

    coef = fbm ( x, y, 5 );

    rgba->r = ( pns->color1.r * coef ) + ( pns->color2.r * ( 1.0f - coef ) );
    rgba->g = ( pns->color1.g * coef ) + ( pns->color2.g * ( 1.0f - coef ) );
    rgba->b = ( pns->color1.b * coef ) + ( pns->color2.b * ( 1.0f - coef ) );
}

/******************************************************************************/
G3DPROCEDURALNOISE *g3dproceduralnoise_new ( ) {
    G3DPROCEDURALNOISE *noi = ( G3DPROCEDURALNOISE * ) calloc ( 0x01, sizeof ( G3DPROCEDURALNOISE ) );

    if ( noi == NULL ) {
        fprintf ( stderr, "g3dproceduralnoise_new(): calloc failed\n" );

        return NULL;
    }

    g3dprocedural_init ( (G3DPROCEDURAL*)noi, PROCEDURALNOISE, getColor );

    noi->color1.r = noi->color1.g = noi->color1.b = 0xFF;
    noi->color2.r = noi->color2.g = noi->color2.b = 0x00;

    return noi;
}
