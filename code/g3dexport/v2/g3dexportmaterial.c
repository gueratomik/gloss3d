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
static uint32_t g3dexportv2channel_alpha ( G3DEXPORTV2DATA *ged, 
                                         G3DMATERIAL   *mat, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_writeChunk ( SIG_CHANNEL,
                                   g3dexportv2channel,
                                   ged,
                                  &mat->alpha,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2material_displacementStrength ( G3DEXPORTV2DATA *ged, 
                                                         G3DMATERIAL   *mat, 
                                                         uint32_t       flags, 
                                                         FILE          *fdst ) {
    uint32_t size = 0x00;

    /*** displacement strength is stored as solid color ***/
    size += g3dexportv2_fwritef ( &mat->displacement.solid.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2channel_displacement ( G3DEXPORTV2DATA *ged, 
                                               G3DMATERIAL   *mat, 
                                               uint32_t       flags, 
                                               FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_writeChunk ( SIG_MATERIAL_DISPLACEMENT_STRENGTH,
                                   g3dexportv2material_displacementStrength,
                                   ged,
                                   mat,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv2_writeChunk ( SIG_CHANNEL,
                                   g3dexportv2channel,
                                   ged,
                                  &mat->displacement,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2material_bumpStrength ( G3DEXPORTV2DATA *ged, 
                                                 G3DMATERIAL   *mat, 
                                                 uint32_t       flags, 
                                                 FILE          *fdst ) {
    uint32_t size = 0x00;

    /*** bump strength is stored as solid color ***/
    size += g3dexportv2_fwritef ( &mat->bump.solid.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2channel_bump ( G3DEXPORTV2DATA *ged, 
                                        G3DMATERIAL   *mat, 
                                        uint32_t       flags, 
                                        FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_writeChunk ( SIG_MATERIAL_BUMP_STRENGTH,
                                   g3dexportv2material_bumpStrength,
                                   ged,
                                   mat,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv2_writeChunk ( SIG_CHANNEL,
                                   g3dexportv2channel,
                                   ged,
                                  &mat->bump,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2channel_refraction ( G3DEXPORTV2DATA *ged, 
                                              G3DMATERIAL   *mat, 
                                              uint32_t       flags, 
                                              FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_writeChunk ( SIG_CHANNEL,
                                   g3dexportv2channel,
                                   ged,
                                  &mat->refraction,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2channel_reflection ( G3DEXPORTV2DATA *ged, 
                                              G3DMATERIAL   *mat, 
                                              uint32_t       flags, 
                                              FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_writeChunk ( SIG_CHANNEL,
                                   g3dexportv2channel,
                                   ged,
                                  &mat->reflection,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2material_specularLevel ( G3DEXPORTV2DATA *ged, 
                                                  G3DMATERIAL   *mat, 
                                                  uint32_t       flags, 
                                                  FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_fwritef ( &mat->specular_level, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2material_specularShininess ( G3DEXPORTV2DATA *ged, 
                                                      G3DMATERIAL   *mat, 
                                                      uint32_t       flags, 
                                                      FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_fwritef ( &mat->shininess, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2material_specular ( G3DEXPORTV2DATA *ged, 
                                            G3DMATERIAL   *mat, 
                                            uint32_t       flags, 
                                            FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_writeChunk ( SIG_MATERIAL_SPECULAR_SHININESS,
                                   g3dexportv2material_specularShininess,
                                   ged,
                                   mat,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv2_writeChunk ( SIG_MATERIAL_SPECULAR_LEVEL,
                                   g3dexportv2material_specularLevel,
                                   ged,
                                   mat,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv2_writeChunk ( SIG_CHANNEL,
                                   g3dexportv2channel,
                                   ged,
                                  &mat->specular,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2material_diffuse ( G3DEXPORTV2DATA *ged, 
                                            G3DMATERIAL   *mat, 
                                            uint32_t       flags, 
                                            FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_writeChunk ( SIG_CHANNEL,
                                   g3dexportv2channel,
                                   ged,
                                  &mat->diffuse,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2material_name ( G3DEXPORTV2DATA *ged, 
                                         G3DMATERIAL   *mat, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_fwrite ( mat->name, strlen ( mat->name ), 0x01, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2material_flags ( G3DEXPORTV2DATA *ged, 
                                          G3DMATERIAL   *mat, 
                                          uint32_t       flags, 
                                          FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_fwritef ( &mat->flags, fdst );

    return size;
}

/******************************************************************************/
uint32_t g3dexportv2material  ( G3DEXPORTV2DATA *ged, 
                              G3DMATERIAL   *mat, 
                              uint32_t       flags, 
                              FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_writeChunk ( SIG_MATERIAL_NAME,
                                   g3dexportv2material_name,
                                   ged,
                                   mat,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexportv2_writeChunk ( SIG_MATERIAL_FLAGS,
                                   g3dexportv2material_flags,
                                   ged,
                                   mat,
                                   0xFFFFFFFF,
                                   fdst );

    if ( mat->flags & DIFFUSE_ENABLED ) {
        size += g3dexportv2_writeChunk ( SIG_MATERIAL_DIFFUSE,
                                       g3dexportv2material_diffuse,
                                       ged,
                                       mat,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( mat->flags & SPECULAR_ENABLED ) {
        size += g3dexportv2_writeChunk ( SIG_MATERIAL_SPECULAR,
                                       g3dexportv2material_specular,
                                       ged,
                                       mat,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( mat->flags & REFLECTION_ENABLED ) {
        size += g3dexportv2_writeChunk ( SIG_MATERIAL_REFLECTION,
                                       g3dexportv2channel_reflection,
                                       ged,
                                       mat,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( mat->flags & REFRACTION_ENABLED ) {
        size += g3dexportv2_writeChunk ( SIG_MATERIAL_REFRACTION,
                                       g3dexportv2channel_refraction,
                                       ged,
                                       mat,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( mat->flags & BUMP_ENABLED ) {
        size += g3dexportv2_writeChunk ( SIG_MATERIAL_BUMP,
                                       g3dexportv2channel_bump,
                                       ged,
                                       mat,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( mat->flags & DISPLACEMENT_ENABLED ) {
        size += g3dexportv2_writeChunk ( SIG_MATERIAL_DISPLACEMENT,
                                       g3dexportv2channel_displacement,
                                       ged,
                                       mat,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( mat->flags & ALPHA_ENABLED ) {
        size += g3dexportv2_writeChunk ( SIG_MATERIAL_ALPHA,
                                       g3dexportv2channel_alpha,
                                       ged,
                                       mat,
                                       0xFFFFFFFF,
                                       fdst );
    }

    return size;
}
