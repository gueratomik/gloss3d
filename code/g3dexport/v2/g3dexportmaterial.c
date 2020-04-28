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
static uint32_t g3dexportchannel_alpha ( G3DEXPORTDATA *ged, 
                                         G3DMATERIAL   *mat, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_CHANNEL,
                                   g3dexportchannel,
                                   ged,
                                  &mat->alpha,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmaterial_displacementStrength ( G3DEXPORTDATA *ged, 
                                                         G3DMATERIAL   *mat, 
                                                         uint32_t       flags, 
                                                         FILE          *fdst ) {
    uint32_t size = 0x00;

    /*** displacement strength is stored as solid color ***/
    size += g3dexport_fwritef ( &mat->displacement.solid.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportchannel_displacement ( G3DEXPORTDATA *ged, 
                                               G3DMATERIAL   *mat, 
                                               uint32_t       flags, 
                                               FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_MATERIAL_DISPLACEMENT_STRENGTH,
                                   g3dexportmaterial_displacementStrength,
                                   ged,
                                   mat,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_CHANNEL,
                                   g3dexportchannel,
                                   ged,
                                  &mat->displacement,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmaterial_bumpStrength ( G3DEXPORTDATA *ged, 
                                                 G3DMATERIAL   *mat, 
                                                 uint32_t       flags, 
                                                 FILE          *fdst ) {
    uint32_t size = 0x00;

    /*** bump strength is stored as solid color ***/
    size += g3dexport_fwritef ( &mat->bump.solid.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportchannel_bump ( G3DEXPORTDATA *ged, 
                                        G3DMATERIAL   *mat, 
                                        uint32_t       flags, 
                                        FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_MATERIAL_BUMP_STRENGTH,
                                   g3dexportmaterial_bumpStrength,
                                   ged,
                                   mat,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_CHANNEL,
                                   g3dexportchannel,
                                   ged,
                                  &mat->bump,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportchannel_refraction ( G3DEXPORTDATA *ged, 
                                              G3DMATERIAL   *mat, 
                                              uint32_t       flags, 
                                              FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_CHANNEL,
                                   g3dexportchannel,
                                   ged,
                                  &mat->refraction,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportchannel_reflection ( G3DEXPORTDATA *ged, 
                                              G3DMATERIAL   *mat, 
                                              uint32_t       flags, 
                                              FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_CHANNEL,
                                   g3dexportchannel,
                                   ged,
                                  &mat->reflection,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmaterial_specularLevel ( G3DEXPORTDATA *ged, 
                                                  G3DMATERIAL   *mat, 
                                                  uint32_t       flags, 
                                                  FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &mat->specular_level, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmaterial_specularShininess ( G3DEXPORTDATA *ged, 
                                                      G3DMATERIAL   *mat, 
                                                      uint32_t       flags, 
                                                      FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &mat->shininess, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmaterial_specular ( G3DEXPORTDATA *ged, 
                                            G3DMATERIAL   *mat, 
                                            uint32_t       flags, 
                                            FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_MATERIAL_SPECULAR_SHININESS,
                                   g3dexportmaterial_specularShininess,
                                   ged,
                                   mat,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_MATERIAL_SPECULAR_LEVEL,
                                   g3dexportmaterial_specularLevel,
                                   ged,
                                   mat,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_CHANNEL,
                                   g3dexportchannel,
                                   ged,
                                  &mat->specular,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmaterial_diffuse ( G3DEXPORTDATA *ged, 
                                            G3DMATERIAL   *mat, 
                                            uint32_t       flags, 
                                            FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_CHANNEL,
                                   g3dexportchannel,
                                   ged,
                                  &mat->diffuse,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmaterial_name ( G3DEXPORTDATA *ged, 
                                         G3DMATERIAL   *mat, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwrite ( mat->name, strlen ( mat->name ), 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportmaterial_flags ( G3DEXPORTDATA *ged, 
                                          G3DMATERIAL   *mat, 
                                          uint32_t       flags, 
                                          FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &mat->flags, fdst );

    return size;
}

/******************************************************************************/
uint32_t g3dexportmaterial  ( G3DEXPORTDATA *ged, 
                              G3DMATERIAL   *mat, 
                              uint32_t       flags, 
                              FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_MATERIAL_NAME,
                                   g3dexportmaterial_name,
                                   ged,
                                   mat,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_MATERIAL_FLAGS,
                                   g3dexportmaterial_flags,
                                   ged,
                                   mat,
                                   0xFFFFFFFF,
                                   fdst );

    if ( mat->flags & DIFFUSE_ENABLED ) {
        size += g3dexport_writeChunk ( SIG_MATERIAL_DIFFUSE,
                                       g3dexportmaterial_diffuse,
                                       ged,
                                       mat,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( mat->flags & SPECULAR_ENABLED ) {
        size += g3dexport_writeChunk ( SIG_MATERIAL_SPECULAR,
                                       g3dexportmaterial_specular,
                                       ged,
                                       mat,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( mat->flags & REFLECTION_ENABLED ) {
        size += g3dexport_writeChunk ( SIG_MATERIAL_REFLECTION,
                                       g3dexportchannel_reflection,
                                       ged,
                                       mat,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( mat->flags & REFRACTION_ENABLED ) {
        size += g3dexport_writeChunk ( SIG_MATERIAL_REFRACTION,
                                       g3dexportchannel_refraction,
                                       ged,
                                       mat,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( mat->flags & BUMP_ENABLED ) {
        size += g3dexport_writeChunk ( SIG_MATERIAL_BUMP,
                                       g3dexportchannel_bump,
                                       ged,
                                       mat,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( mat->flags & DISPLACEMENT_ENABLED ) {
        size += g3dexport_writeChunk ( SIG_MATERIAL_DISPLACEMENT,
                                       g3dexportchannel_displacement,
                                       ged,
                                       mat,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( mat->flags & ALPHA_ENABLED ) {
        size += g3dexport_writeChunk ( SIG_MATERIAL_ALPHA,
                                       g3dexportchannel_alpha,
                                       ged,
                                       mat,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}
