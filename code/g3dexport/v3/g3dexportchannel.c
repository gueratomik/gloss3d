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
static uint32_t g3dexportv3channel_proceduralNoiseColorPairs ( G3DEXPORTV3DATA *ged,
                                                               G3DCHANNEL    *cha,
                                                               uint32_t       flags, 
                                                               FILE          *fdst ) {
    G3DPROCEDURALNOISE *noise = ( G3DPROCEDURALNOISE * ) cha->proc;
    uint32_t size = 0x00;
    uint32_t i;

    for ( i = 0x00; i < noise->nbColors; i++ ) {
        size += g3dexportv3_fwritef ( &noise->colorPair[i][0x00].r, fdst );
        size += g3dexportv3_fwritef ( &noise->colorPair[i][0x00].g, fdst );
        size += g3dexportv3_fwritef ( &noise->colorPair[i][0x00].b, fdst );
        size += g3dexportv3_fwritef ( &noise->colorPair[i][0x00].a, fdst );

        size += g3dexportv3_fwritef ( &noise->colorPair[i][0x01].r, fdst );
        size += g3dexportv3_fwritef ( &noise->colorPair[i][0x01].g, fdst );
        size += g3dexportv3_fwritef ( &noise->colorPair[i][0x01].b, fdst );
        size += g3dexportv3_fwritef ( &noise->colorPair[i][0x01].a, fdst );
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_proceduralNoiseThresholds ( G3DEXPORTV3DATA *ged,
                                                             G3DCHANNEL    *cha,
                                                             uint32_t       flags, 
                                                             FILE          *fdst ) {
    G3DPROCEDURALNOISE *noise = ( G3DPROCEDURALNOISE * ) cha->proc;
    uint32_t size = 0x00;
    uint32_t i;

    for ( i = 0x00; i < noise->nbColors; i++ ) {
        size += g3dexportv3_fwritef ( &noise->threshold[i], fdst );
    }

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_proceduralNoiseColorPairCount ( G3DEXPORTV3DATA *ged,
                                                                 G3DCHANNEL    *cha,
                                                                 uint32_t       flags, 
                                                                 FILE          *fdst ) {
    G3DPROCEDURALNOISE *noise = ( G3DPROCEDURALNOISE * ) cha->proc;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &noise->nbColors, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_proceduralNoiseOctaves ( G3DEXPORTV3DATA *ged,
                                                          G3DCHANNEL    *cha,
                                                          uint32_t       flags, 
                                                          FILE          *fdst ) {
    G3DPROCEDURALNOISE *noise = ( G3DPROCEDURALNOISE * ) cha->proc;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &noise->nbOctaves, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_proceduralNoiseInterpolation ( G3DEXPORTV3DATA *ged,
                                                                G3DCHANNEL    *cha,
                                                                uint32_t       flags, 
                                                                FILE          *fdst ) {
    G3DPROCEDURALNOISE *noise = ( G3DPROCEDURALNOISE * ) cha->proc;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &noise->interpolation, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_proceduralNoiseGradients ( G3DEXPORTV3DATA *ged,
                                                            G3DCHANNEL    *cha,
                                                            uint32_t       flags, 
                                                            FILE          *fdst ) {
    G3DPROCEDURALNOISE *noise = ( G3DPROCEDURALNOISE * ) cha->proc;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &noise->nbGradientX, fdst );
    size += g3dexportv3_fwritel ( &noise->nbGradientY, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_proceduralNoise ( G3DEXPORTV3DATA *ged,
                                                   G3DCHANNEL    *cha,
                                                   uint32_t       flags, 
                                                   FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_CHANNEL_PROCEDURAL_NOISE_COLORPAIR_COUNT,
                   EXPORTV3_CALLBACK(g3dexportv3channel_proceduralNoiseColorPairCount),
                                     ged,
                                     cha,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_CHANNEL_PROCEDURAL_NOISE_COLORPAIRS,
                   EXPORTV3_CALLBACK(g3dexportv3channel_proceduralNoiseColorPairs),
                                     ged,
                                     cha,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_CHANNEL_PROCEDURAL_NOISE_THRESHOLDS,
                   EXPORTV3_CALLBACK(g3dexportv3channel_proceduralNoiseThresholds),
                                     ged,
                                     cha,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_CHANNEL_PROCEDURAL_NOISE_OCTAVES,
                   EXPORTV3_CALLBACK(g3dexportv3channel_proceduralNoiseOctaves),
                                     ged,
                                     cha,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_CHANNEL_PROCEDURAL_NOISE_INTERPOLATION,
                   EXPORTV3_CALLBACK(g3dexportv3channel_proceduralNoiseInterpolation),
                                     ged,
                                     cha,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_CHANNEL_PROCEDURAL_NOISE_GRADIENTS,
                   EXPORTV3_CALLBACK(g3dexportv3channel_proceduralNoiseGradients),
                                     ged,
                                     cha,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_proceduralChessGeometry ( G3DEXPORTV3DATA *ged,
                                                           G3DCHANNEL    *cha,
                                                           uint32_t       flags, 
                                                           FILE          *fdst ) {
    G3DPROCEDURALCHESS *chess = ( G3DPROCEDURALCHESS * ) cha->proc;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &chess->udiv, fdst );
    size += g3dexportv3_fwritel ( &chess->vdiv, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_proceduralChessColors ( G3DEXPORTV3DATA *ged,
                                                         G3DCHANNEL    *cha,
                                                         uint32_t       flags, 
                                                         FILE          *fdst ) {
    G3DPROCEDURALCHESS *chess = ( G3DPROCEDURALCHESS * ) cha->proc;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &chess->color1.r, fdst );
    size += g3dexportv3_fwritef ( &chess->color1.g, fdst );
    size += g3dexportv3_fwritef ( &chess->color1.b, fdst );
    size += g3dexportv3_fwritef ( &chess->color1.a, fdst );

    size += g3dexportv3_fwritef ( &chess->color2.r, fdst );
    size += g3dexportv3_fwritef ( &chess->color2.g, fdst );
    size += g3dexportv3_fwritef ( &chess->color2.b, fdst );
    size += g3dexportv3_fwritef ( &chess->color2.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_proceduralChess ( G3DEXPORTV3DATA *ged,
                                                   G3DCHANNEL    *cha,
                                                   uint32_t       flags, 
                                                   FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_CHANNEL_PROCEDURAL_CHESS_COLORS,
                   EXPORTV3_CALLBACK(g3dexportv3channel_proceduralChessColors),
                                     ged,
                                     cha,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_CHANNEL_PROCEDURAL_CHESS_GEOMETRY,
                   EXPORTV3_CALLBACK(g3dexportv3channel_proceduralChessGeometry),
                                     ged,
                                     cha,
                                     0xFFFFFFFF,
                                     fdst );
    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_proceduralGradientGeometry ( G3DEXPORTV3DATA *ged,
                                                                G3DCHANNEL      *cha,
                                                                uint32_t         flags, 
                                                                FILE            *fdst ) {
    G3DPROCEDURALGRADIENT *gradient = ( G3DPROCEDURALGRADIENT * ) cha->proc;
    uint32_t size = 0x00;
    uint32_t horizontal = gradient->horizontal;

    size += g3dexportv3_fwritel ( &horizontal, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_proceduralGradientColors ( G3DEXPORTV3DATA *ged,
                                                              G3DCHANNEL      *cha,
                                                              uint32_t         flags, 
                                                              FILE            *fdst ) {
    G3DPROCEDURALGRADIENT *gradient = ( G3DPROCEDURALGRADIENT * ) cha->proc;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &gradient->color1.r, fdst );
    size += g3dexportv3_fwritef ( &gradient->color1.g, fdst );
    size += g3dexportv3_fwritef ( &gradient->color1.b, fdst );
    size += g3dexportv3_fwritef ( &gradient->color1.a, fdst );

    size += g3dexportv3_fwritef ( &gradient->color2.r, fdst );
    size += g3dexportv3_fwritef ( &gradient->color2.g, fdst );
    size += g3dexportv3_fwritef ( &gradient->color2.b, fdst );
    size += g3dexportv3_fwritef ( &gradient->color2.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_proceduralGradient ( G3DEXPORTV3DATA *ged,
                                                        G3DCHANNEL      *cha,
                                                        uint32_t         flags, 
                                                        FILE            *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_CHANNEL_PROCEDURAL_GRADIENT_COLORS,
                   EXPORTV3_CALLBACK(g3dexportv3channel_proceduralGradientColors),
                                     ged,
                                     cha,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_CHANNEL_PROCEDURAL_GRADIENT_GEOMETRY,
                   EXPORTV3_CALLBACK(g3dexportv3channel_proceduralGradientGeometry),
                                     ged,
                                     cha,
                                     0xFFFFFFFF,
                                     fdst );
    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_proceduralBrickGeometry ( G3DEXPORTV3DATA *ged,
                                                           G3DCHANNEL    *cha,
                                                           uint32_t       flags, 
                                                           FILE          *fdst ) {
    G3DPROCEDURALBRICK *brick = ( G3DPROCEDURALBRICK * ) cha->proc;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &brick->nbBricksPerLine, fdst );
    size += g3dexportv3_fwritel ( &brick->nbLines        , fdst );
    size += g3dexportv3_fwritef ( &brick->uspacing       , fdst );
    size += g3dexportv3_fwritef ( &brick->vspacing       , fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_proceduralBrickColors ( G3DEXPORTV3DATA *ged,
                                                         G3DCHANNEL    *cha,
                                                         uint32_t       flags, 
                                                         FILE          *fdst ) {
    G3DPROCEDURALBRICK *brick = ( G3DPROCEDURALBRICK * ) cha->proc;
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &brick->brickColor.r, fdst );
    size += g3dexportv3_fwritef ( &brick->brickColor.g, fdst );
    size += g3dexportv3_fwritef ( &brick->brickColor.b, fdst );
    size += g3dexportv3_fwritef ( &brick->brickColor.a, fdst );

    size += g3dexportv3_fwritef ( &brick->spacingColor.r, fdst );
    size += g3dexportv3_fwritef ( &brick->spacingColor.g, fdst );
    size += g3dexportv3_fwritef ( &brick->spacingColor.b, fdst );
    size += g3dexportv3_fwritef ( &brick->spacingColor.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_proceduralBrick ( G3DEXPORTV3DATA *ged,
                                                   G3DCHANNEL    *cha,
                                                   uint32_t       flags, 
                                                   FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_CHANNEL_PROCEDURAL_BRICK_COLORS,
                   EXPORTV3_CALLBACK(g3dexportv3channel_proceduralBrickColors),
                                     ged,
                                     cha,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_CHANNEL_PROCEDURAL_BRICK_GEOMETRY,
                   EXPORTV3_CALLBACK(g3dexportv3channel_proceduralBrickGeometry),
                                     ged,
                                     cha,
                                     0xFFFFFFFF,
                                     fdst );
    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_proceduralResolution ( G3DEXPORTV3DATA *ged,
                                                        G3DCHANNEL    *cha,
                                                        uint32_t       flags, 
                                                        FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritel ( &cha->proc->image.width        , fdst );
    size += g3dexportv3_fwritel ( &cha->proc->image.height       , fdst );
    size += g3dexportv3_fwritel ( &cha->proc->image.bytesPerPixel, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_procedural ( G3DEXPORTV3DATA *ged, 
                                              G3DCHANNEL    *cha, 
                                              uint32_t       flags, 
                                              FILE          *fdst ) {
    uint32_t size = 0x00;

    switch ( cha->proc->type ) {
        case PROCEDURALNOISE :
            size += g3dexportv3_writeChunk ( SIG_CHANNEL_PROCEDURAL_NOISE,
                           EXPORTV3_CALLBACK(g3dexportv3channel_proceduralNoise),
                                             ged,
                                             cha,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case PROCEDURALCHESS :
            size += g3dexportv3_writeChunk ( SIG_CHANNEL_PROCEDURAL_CHESS,
                           EXPORTV3_CALLBACK(g3dexportv3channel_proceduralChess),
                                             ged,
                                             cha,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case PROCEDURALBRICK :
            size += g3dexportv3_writeChunk ( SIG_CHANNEL_PROCEDURAL_BRICK,
                           EXPORTV3_CALLBACK(g3dexportv3channel_proceduralBrick),
                                             ged,
                                             cha,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        case PROCEDURALGRADIENT :
            size += g3dexportv3_writeChunk ( SIG_CHANNEL_PROCEDURAL_GRADIENT,
                           EXPORTV3_CALLBACK(g3dexportv3channel_proceduralGradient),
                                             ged,
                                             cha,
                                             0xFFFFFFFF,
                                             fdst );
        break;

        default :
        break;
    }

    size += g3dexportv3_writeChunk ( SIG_CHANNEL_PROCEDURAL_RESOLUTION,
                   EXPORTV3_CALLBACK(g3dexportv3channel_proceduralResolution),
                                     ged,
                                     cha,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_image ( G3DEXPORTV3DATA *ged,
                                         G3DCHANNEL    *cha,
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t filenameSize = strlen ( cha->image->filename );
    uint32_t size = 0x00;

    size += g3dexportv3_fwrite ( cha->image->filename, 
                                 filenameSize, 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3channel_solid ( G3DEXPORTV3DATA *ged, 
                                         G3DCHANNEL    *cha, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &cha->solid.r, fdst );
    size += g3dexportv3_fwritef ( &cha->solid.g, fdst );
    size += g3dexportv3_fwritef ( &cha->solid.b, fdst );
    size += g3dexportv3_fwritef ( &cha->solid.a, fdst );

    return size;
}


/******************************************************************************/
uint32_t g3dexportv3channel  ( G3DEXPORTV3DATA *ged, 
                             G3DCHANNEL    *cha,
                             uint32_t       flags, 
                             FILE          *fdst ) {
    uint32_t size = 0x00;

    if ( flags & EXPORTV3CHANNEL_SOLIDCOLOR ) {
        if ( cha->flags & USESOLIDCOLOR ) {
            size += g3dexportv3_writeChunk ( SIG_CHANNEL_SOLID,
                           EXPORTV3_CALLBACK(g3dexportv3channel_solid),
                                             ged,
                                             cha,
                                             0xFFFFFFFF,
                                             fdst );
        }
    }

    if ( flags & EXPORTV3CHANNEL_IMAGECOLOR ) {
        if ( ( cha->flags & USEIMAGECOLOR ) &&
             ( cha->image                 ) &&
             ( cha->image->filename       ) ) {
            size += g3dexportv3_writeChunk ( SIG_CHANNEL_IMAGE,
                           EXPORTV3_CALLBACK(g3dexportv3channel_image),
                                             ged,
                                             cha,
                                             0xFFFFFFFF,
                                             fdst );
        }
    }

    if ( flags & EXPORTV3CHANNEL_PROCEDURAL ) {
        if ( ( cha->flags & USEPROCEDURAL ) &&
             ( cha->proc                  ) ) {
            size += g3dexportv3_writeChunk ( SIG_CHANNEL_PROCEDURAL,
                           EXPORTV3_CALLBACK(g3dexportv3channel_procedural),
                                             ged,
                                             cha,
                                             0xFFFFFFFF,
                                             fdst );
        }
    }

    return size;
}
