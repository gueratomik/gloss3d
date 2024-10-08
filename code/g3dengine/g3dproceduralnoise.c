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

/***  https://en.wikipedia.org/wiki/Perlin_noise ***/

#define s_curve(t) ( t * t * (3. - 2. * t) )

/******************************************************************************/
/*static double fade ( double t ) {
    return t * t * t * (t * (t * 6 - 15) + 10);
}*/

/******************************************************************************/
void g3dproceduralnoise_copySettings ( G3DPROCEDURALNOISE *noise, 
                                       G3DPROCEDURALNOISE *pout ) {
    uint32_t settingsSize = sizeof ( G3DPROCEDURALNOISE ) - 
                            sizeof ( G3DPROCEDURAL );

    memcpy ( ( char * ) pout  + sizeof ( G3DPROCEDURAL ),
             ( char * ) noise + sizeof ( G3DPROCEDURAL ), settingsSize );
}

/******************************************************************************/
static float fade ( float p1, float p2, float p ) {
   float m = ( 1 - cos ( p * M_PI ) ) / 2.0f;

   return ( p1 * ( 1 - m ) + p2 * m );
}

/******************************************************************************/
void g3dproceduralnoise_buildGradients ( G3DPROCEDURALNOISE *noise,
                                         uint32_t            nbGradientX,
                                         uint32_t            nbGradientY ) {
    uint32_t size = sizeof ( G3DVECTOR3F ) * nbGradientX * nbGradientY;
    int i, j, x, y;

    noise->nbGradientX = nbGradientX;
    noise->nbGradientY = nbGradientY;

    noise->gradients = realloc ( noise->gradients, size );

    for ( x = 0x00; x < noise->nbGradientX; x++ ) {
        for ( y = 0x00; y < noise->nbGradientY; y++ ) {
            /*** From wikipedia b/c unfortunately I'm no genius in Maths ***/
            float random = 2920.f * sin(x * 21942.f + y * 171324.f + 8912.f) * cos(x * 23157.f * y * 217832.f + 9758.f);
            uint32_t offset = ( y * noise->nbGradientX ) + x;

            noise->gradients[offset].x = cos(random);
            noise->gradients[offset].y = sin(random);
            noise->gradients[offset].z = 0.0f;

            g3dvector3f_normalize ( &noise->gradients[offset] );
        }
    }
}

/******************************************************************************/
static float lerp ( float a0, float a1, float w ) {
    return ( ( 1.0f - w ) * a0 ) + ( w * a1 );
}

/******************************************************************************/
/***     Computes the dot product of the distance and gradient vectors.     ***/
static float g3dproceduralnoise_dotGradients ( G3DPROCEDURALNOISE *noise,
                                               int  ix, 
                                               int  iy,
                                               float x, 
                                               float y ) {
    uint32_t offset = ( ( iy % noise->nbGradientY ) * noise->nbGradientX ) + 
                        ( ix % noise->nbGradientX );
    /*** Compute the distance vector. Note: we cast x and y and dont use ix ***/
    /*** because ix and iy are modulos and we dont want modulos to compute ***/
    /*** vectors ***/
    float dx = x - ( int ) ix;
    float dy = y - ( int ) iy;
    float dot = ( dx * noise->gradients[offset].x + 
                  dy * noise->gradients[offset].y );

    return dot;
}

/******************************************************************************/
/***                   Compute Perlin noise at coordinates x, y             ***/
static float g3dproceduralnoise_perlin ( G3DPROCEDURALNOISE *noise, 
                                         float                   x, 
                                         float                   y ) {
    float lenx = ( x == 0.0f ) ?  0.0f : (int) x / noise->nbGradientX,
          leny = ( y == 0.0f ) ?  0.0f : (int) y / noise->nbGradientY;
    float rx = x - ( lenx * noise->nbGradientX ),
          ry = y - ( leny * noise->nbGradientY );
    /*** Determine grid cell coordinates ***/
    int x0 = ((int) rx);
    int x1 = (x0 + 1 );
    int y0 = ((int) ry);
    int y1 = (y0 + 1);
    /*** Determine interpolation weights ***/
    float sx =  ( rx - ( float ) x0 );
    float sy =  ( ry - ( float ) y0 );

    /*** Interpolate between grid point gradients ***/
    float n0, n1, ix0, ix1;

    n0  = g3dproceduralnoise_dotGradients ( noise, x0, y0, rx, ry );
    n1  = g3dproceduralnoise_dotGradients ( noise, x1, y0, rx, ry );

    ix0 = fade ( n0, n1, sx );

    n0  = g3dproceduralnoise_dotGradients ( noise, x0, y1, rx, ry );
    n1  = g3dproceduralnoise_dotGradients ( noise, x1, y1, rx, ry );

    ix1 = fade ( n0, n1, sx );

    return fade ( ix0, ix1, sy );
}

/******************************************************************************/
static void getColor ( G3DPROCEDURAL *proc, 
                       double         x, 
                       double         y,
                       double         z,
                       G3DCOLOR      *color ) {
    G3DPROCEDURALNOISE *noise = ( G3DPROCEDURALNOISE * ) proc;
    uint32_t nbGradX = noise->nbGradientX,
             nbGradY = noise->nbGradientY;
    float persistence = 0.5f;
    float total = 0.0f;
    float coef, invcoef;
    float amp = 1.0f;
    float max = 0.0f; /*** used to normalize total ***/
    uint32_t i;
    float frq = 1.0f;

    x = ( x >= 0.0f ) ? ( x - ( int ) x ) : ( 1.0f - ( x - ( int ) x ) );
    y = ( y >= 0.0f ) ? ( y - ( int ) y ) : ( 1.0f - ( y - ( int ) y ) );

    for ( i = 0x00; i < noise->nbOctaves; i++ ) {
        float p =  g3dproceduralnoise_perlin ( noise,
                                     ( float ) x * nbGradX * frq , 
                                     ( float ) y * nbGradY * frq ) * amp;

        total += p;
        max += amp;

        amp *= persistence;
        frq *= 2;
    }

    if ( max ) {
        float coef    = ( ( ( total / max ) * 0.5f ) + 0.5f );
        float invcoef = 1.0f - coef;

        for ( i = 0x00; i < noise->nbColors; i++ ) {
            if ( coef <= noise->threshold[i] ) {
                color->r = ( noise->colorPair[i][0x00].r *    coef ) + 
                           ( noise->colorPair[i][0x01].r * invcoef );

                color->g = ( noise->colorPair[i][0x00].g *    coef ) + 
                           ( noise->colorPair[i][0x01].g * invcoef );

                color->b = ( noise->colorPair[i][0x00].b *    coef ) + 
                           ( noise->colorPair[i][0x01].b * invcoef );
            }
        }
    }
}

/******************************************************************************/
G3DPROCEDURALNOISE *g3dproceduralnoise_new ( ) {
    G3DPROCEDURALNOISE *noise = ( G3DPROCEDURALNOISE * ) calloc ( 0x01, sizeof ( G3DPROCEDURALNOISE ) );
    uint32_t i;

    if ( noise == NULL ) {
        fprintf ( stderr, "g3dproceduralnoise_new(): calloc failed\n" );

        return NULL;
    }

    g3dprocedural_init ( ( G3DPROCEDURAL * ) noise, PROCEDURALNOISE, 0x00, getColor );

    /*** Build default normal vectors (gradients) ***/
    g3dproceduralnoise_buildGradients ( noise, 0x10, 0x10 );

    /*** init all color slots ***/
    for ( i = 0x00; i < MAXNOISECOLORS; i++ ) {
        noise->colorPair[i][0x00].r = 
        noise->colorPair[i][0x00].g = 
        noise->colorPair[i][0x00].b = ( float ) 0xFF / 255.0f;

        noise->colorPair[i][0x01].r = 
        noise->colorPair[i][0x01].g = 
        noise->colorPair[i][0x01].b = ( float ) 0x00 / 255.0f;

        noise->threshold[i] = 1.0f;
    }

    noise->nbColors  = 0x01;
    noise->nbOctaves = 0x01;

    noise->interpolation = NOISE_INTERPOLATION_COSINE;


    return noise;
}

#ifdef UNUSED


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

#endif
#ifdef UNUSED

/******************************************************************************/
static uint8_t *getPermutations ( ) {
    static uint8_t permutations[256]; 
    static int init;

    if ( init == 0x00 ) {
        uint32_t i;

        for ( i = 0x00; i < 256; i++ ) {
            uint32_t val = rand() % 256;

            permutations[i] = val;
        }

        init = 0x01;
    }

    return permutations;
}

/******************************************************************************/
static uint8_t lerp ( uint8_t a0, uint8_t a1, uint8_t w ) {
    return ( ( 255 - w ) * a0 ) + ( w * a1 );
}

/******************************************************************************/
static float cosineInterpolate ( float p1, float p2, float p ) {
   float m = ( 1 - cos ( p * M_PI ) ) / 2.0f;

   return ( p1 * ( 1 - m ) + p2 * m );
}

/******************************************************************************/
float noise ( uint8_t u, uint8_t octave ) {
    uint8_t *permutations = getPermutations ( );
    /*uint8_t step = 1 << octave;*/ /** equals pow ( 2, octave) ***/
    uint8_t wavelength = 256 >> octave;
    uint8_t pu = ( u - ( u % wavelength ) );
    uint8_t nu = ( pu + wavelength );
    float angle = ( float ) ( ( float ) u - pu ) / wavelength;

    return cosineInterpolate ( ( float ) permutations[pu] / 255, 
                               ( float ) permutations[nu] / 255,
                               ( float ) angle );
}

/******************************************************************************/
static void getColor ( G3DPROCEDURAL *proc, 
                       double         x, 
                       double         y,
                       double         z,
                       G3DCOLOR      *color ) {
    G3DPROCEDURALNOISE *pns = ( G3DPROCEDURALNOISE * ) proc;
    static int init;
    double coef = 0.0f;
    int i, j;

    x = ( x >= 0.0f ) ? ( x - ( int ) x ) : ( 1.0f - ( x - ( int ) x ) );
    y = ( y >= 0.0f ) ? ( y - ( int ) y ) : ( 1.0f - ( y - ( int ) y ) );

    color->r = (float) ( (float) noise ( x * noise ( y * 255, 2 ) * 255, 1 ) );
    color->g = (float) ( (float) noise ( x * noise ( y * 255, 2 ) * 255, 1 ) );
    color->b = (float) ( (float) noise ( x * noise ( y * 255, 2 ) * 255, 1 ) );
}

/******************************************************************************/
G3DPROCEDURALNOISE *g3dproceduralnoise_new ( ) {
    G3DPROCEDURALNOISE *noi = ( G3DPROCEDURALNOISE * ) calloc ( 0x01, sizeof ( G3DPROCEDURALNOISE ) );

    if ( noi == NULL ) {
        fprintf ( stderr, "g3dproceduralnoise_new(): calloc failed\n" );

        return NULL;
    }

    g3dprocedural_init ( ( G3DPROCEDURAL * ) noi, PROCEDURALNOISE, 0x00, getColor );

    noise->nbGradientX = 0x10;
    noise->nbGradientY = 0x10;

    noi->color1.r = noi->color1.g = noi->color1.b = ( float ) 0xFF / 255.0f;
    noi->color2.r = noi->color2.g = noi->color2.b = ( float ) 0x00 / 255.0f;

    return noi;
}

#endif
