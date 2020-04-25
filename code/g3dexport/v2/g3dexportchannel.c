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
#include <g3dexportv2.h>

/******************************************************************************/
static uint32_t g3dexportchannel_proceduralNoiseGeometry ( G3DEXPORTDATA *ged,
                                                           G3DCHANNEL    *cha,
                                                           uint32_t       flags, 
                                                           FILE          *fdst ) {
    G3DPROCEDURALNOISE *noise = ( G3DPROCEDURALNOISE * ) cha->proc;
    uint32_t size = 0x00;


    return size;
}

/******************************************************************************/
static uint32_t g3dexportchannel_proceduralNoiseColors ( G3DEXPORTDATA *ged,
                                                         G3DCHANNEL    *cha,
                                                         uint32_t       flags, 
                                                         FILE          *fdst ) {
    G3DPROCEDURALNOISE *noise = ( G3DPROCEDURALNOISE * ) cha->proc;
    G3DCOLOR color1, color2;
    uint32_t size = 0x00;

    g3drgba_toColor ( &noise->color1, &color1 );
    g3drgba_toColor ( &noise->color2, &color2 );

    size += g3dexport_fwritef ( &color1.r, fdst );
    size += g3dexport_fwritef ( &color1.g, fdst );
    size += g3dexport_fwritef ( &color1.b, fdst );
    size += g3dexport_fwritef ( &color1.a, fdst );

    size += g3dexport_fwritef ( &color2.r, fdst );
    size += g3dexport_fwritef ( &color2.g, fdst );
    size += g3dexport_fwritef ( &color2.b, fdst );
    size += g3dexport_fwritef ( &color2.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportchannel_proceduralNoise ( G3DEXPORTDATA *ged,
                                                   G3DCHANNEL    *cha,
                                                   uint32_t       flags, 
                                                   FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_CHANNEL_PROCEDURAL_NOISE_COLORS,
                                   g3dexportchannel_proceduralNoiseColors,
                                   ged,
                                   cha,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_CHANNEL_PROCEDURAL_NOISE_GEOMETRY,
                                   g3dexportchannel_proceduralNoiseGeometry,
                                   ged,
                                   cha,
                                   0xFFFFFFFF,
                                   fdst );
    return size;
}

/******************************************************************************/
static uint32_t g3dexportchannel_proceduralChessGeometry ( G3DEXPORTDATA *ged,
                                                           G3DCHANNEL    *cha,
                                                           uint32_t       flags, 
                                                           FILE          *fdst ) {
    G3DPROCEDURALCHESS *chess = ( G3DPROCEDURALCHESS * ) cha->proc;
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &chess->udiv, fdst );
    size += g3dexport_fwritel ( &chess->vdiv, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportchannel_proceduralChessColors ( G3DEXPORTDATA *ged,
                                                         G3DCHANNEL    *cha,
                                                         uint32_t       flags, 
                                                         FILE          *fdst ) {
    G3DPROCEDURALCHESS *chess = ( G3DPROCEDURALCHESS * ) cha->proc;
    G3DCOLOR color1, color2;
    uint32_t size = 0x00;

    g3drgba_toColor ( &chess->color1, &color1 );
    g3drgba_toColor ( &chess->color2, &color2 );

    size += g3dexport_fwritef ( &color1.r, fdst );
    size += g3dexport_fwritef ( &color1.g, fdst );
    size += g3dexport_fwritef ( &color1.b, fdst );
    size += g3dexport_fwritef ( &color1.a, fdst );

    size += g3dexport_fwritef ( &color2.r, fdst );
    size += g3dexport_fwritef ( &color2.g, fdst );
    size += g3dexport_fwritef ( &color2.b, fdst );
    size += g3dexport_fwritef ( &color2.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportchannel_proceduralChess ( G3DEXPORTDATA *ged,
                                                   G3DCHANNEL    *cha,
                                                   uint32_t       flags, 
                                                   FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_CHANNEL_PROCEDURAL_CHESS_COLORS,
                                   g3dexportchannel_proceduralChessColors,
                                   ged,
                                   cha,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_CHANNEL_PROCEDURAL_CHESS_GEOMETRY,
                                   g3dexportchannel_proceduralChessGeometry,
                                   ged,
                                   cha,
                                   0xFFFFFFFF,
                                   fdst );
    return size;
}

/******************************************************************************/
static uint32_t g3dexportchannel_proceduralBrickGeometry ( G3DEXPORTDATA *ged,
                                                           G3DCHANNEL    *cha,
                                                           uint32_t       flags, 
                                                           FILE          *fdst ) {
    G3DPROCEDURALBRICK *brick = ( G3DPROCEDURALBRICK * ) cha->proc;
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &brick->nbBricksPerLine, fdst );
    size += g3dexport_fwritel ( &brick->nbLines        , fdst );
    size += g3dexport_fwritef ( &brick->uspacing       , fdst );
    size += g3dexport_fwritef ( &brick->vspacing       , fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportchannel_proceduralBrickColors ( G3DEXPORTDATA *ged,
                                                         G3DCHANNEL    *cha,
                                                         uint32_t       flags, 
                                                         FILE          *fdst ) {
    G3DPROCEDURALBRICK *brick = ( G3DPROCEDURALBRICK * ) cha->proc;
    G3DCOLOR color1, color2;
    uint32_t size = 0x00;

    g3drgba_toColor ( &brick->brickColor  , &color1 );
    g3drgba_toColor ( &brick->spacingColor, &color2 );

    size += g3dexport_fwritef ( &color1.r, fdst );
    size += g3dexport_fwritef ( &color1.g, fdst );
    size += g3dexport_fwritef ( &color1.b, fdst );
    size += g3dexport_fwritef ( &color1.a, fdst );

    size += g3dexport_fwritef ( &color2.r, fdst );
    size += g3dexport_fwritef ( &color2.g, fdst );
    size += g3dexport_fwritef ( &color2.b, fdst );
    size += g3dexport_fwritef ( &color2.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportchannel_proceduralBrick ( G3DEXPORTDATA *ged,
                                                   G3DCHANNEL    *cha,
                                                   uint32_t       flags, 
                                                   FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_CHANNEL_PROCEDURAL_BRICK_COLORS,
                                   g3dexportchannel_proceduralBrickColors,
                                   ged,
                                   cha,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_CHANNEL_PROCEDURAL_BRICK_GEOMETRY,
                                   g3dexportchannel_proceduralBrickGeometry,
                                   ged,
                                   cha,
                                   0xFFFFFFFF,
                                   fdst );
    return size;
}

/******************************************************************************/
static uint32_t g3dexportchannel_proceduralResolution ( G3DEXPORTDATA *ged,
                                                        G3DCHANNEL    *cha,
                                                        uint32_t       flags, 
                                                        FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritel ( &cha->proc->image.width        , fdst );
    size += g3dexport_fwritel ( &cha->proc->image.height       , fdst );
    size += g3dexport_fwritel ( &cha->proc->image.bytesPerPixel, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportchannel_procedural ( G3DEXPORTDATA *ged, 
                                              G3DCHANNEL    *cha, 
                                              uint32_t       flags, 
                                              FILE          *fdst ) {
    uint32_t size = 0x00;

    switch ( cha->proc->type ) {
        case PROCEDURALNOISE :
            size += g3dexport_writeChunk ( SIG_CHANNEL_PROCEDURAL_NOISE,
                                           g3dexportchannel_proceduralNoise,
                                           ged,
                                           cha,
                                           0xFFFFFFFF,
                                           fdst );
        break;

        case PROCEDURALCHESS :
            size += g3dexport_writeChunk ( SIG_CHANNEL_PROCEDURAL_CHESS,
                                           g3dexportchannel_proceduralChess,
                                           ged,
                                           cha,
                                           0xFFFFFFFF,
                                           fdst );
        break;

        case PROCEDURALBRICK :
            size += g3dexport_writeChunk ( SIG_CHANNEL_PROCEDURAL_BRICK,
                                           g3dexportchannel_proceduralBrick,
                                           ged,
                                           cha,
                                           0xFFFFFFFF,
                                           fdst );
        break;

        default :
        break;
    }

    size += g3dexport_writeChunk ( SIG_CHANNEL_PROCEDURAL_RESOLUTION,
                                   g3dexportchannel_proceduralResolution,
                                   ged,
                                   cha,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportchannel_image ( G3DEXPORTDATA *ged,
                                         G3DCHANNEL    *cha,
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t filenameSize = strlen ( cha->image->filename );
    uint32_t size = 0x00;

    size += g3dexport_fwrite ( cha->image->filename, filenameSize, 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportchannel_solid ( G3DEXPORTDATA *ged, 
                                         G3DCHANNEL    *cha, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &cha->solid.r, fdst );
    size += g3dexport_fwritef ( &cha->solid.g, fdst );
    size += g3dexport_fwritef ( &cha->solid.b, fdst );
    size += g3dexport_fwritef ( &cha->solid.a, fdst );

    return size;
}


/******************************************************************************/
uint32_t g3dexportchannel  ( G3DEXPORTDATA *ged, 
                             G3DCHANNEL    *cha,
                             uint32_t       flags, 
                             FILE          *fdst ) {
    uint32_t size = 0x00;

    if ( cha->flags & USESOLIDCOLOR ) {
        size += g3dexport_writeChunk ( SIG_CHANNEL_SOLID,
                                       g3dexportchannel_solid,
                                       ged,
                                       cha,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( ( cha->flags & USEIMAGECOLOR ) &&
         ( cha->image                 ) &&
         ( cha->image->filename       ) ) {
        size += g3dexport_writeChunk ( SIG_CHANNEL_IMAGE,
                                       g3dexportchannel_image,
                                       ged,
                                       cha,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( ( cha->flags & USEPROCEDURAL ) &&
         ( cha->proc                  ) ) {
        size += g3dexport_writeChunk ( SIG_CHANNEL_PROCEDURAL,
                                       g3dexportchannel_procedural,
                                       ged,
                                       cha,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}
