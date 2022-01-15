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

    float     shadowRadius; /*** radius for area shadows ***/
    uint32_t  shadowSample; /*** radius for area shadows ***/

/******************************************************************************/
static uint32_t g3dexportv3light_spot ( G3DEXPORTV3DATA *ged, 
                                      G3DLIGHT      *lig, 
                                      uint32_t       flags, 
                                      FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &lig->spotLength   , fdst );
    size += g3dexportv3_fwritef ( &lig->spotAngle    , fdst );
    size += g3dexportv3_fwritef ( &lig->spotFadeAngle, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3light_shadowSoftRadius ( G3DEXPORTV3DATA *ged, 
                                                    G3DLIGHT        *lig, 
                                                    uint32_t         flags, 
                                                    FILE            *fdst ) {
    return g3dexportv3_fwritef ( &lig->shadowRadius, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv3light_shadowSoftSampling ( G3DEXPORTV3DATA *ged, 
                                                      G3DLIGHT        *lig, 
                                                      uint32_t         flags, 
                                                      FILE            *fdst ) {
    return g3dexportv3_fwritel ( &lig->shadowSample, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv3light_shadowSoft ( G3DEXPORTV3DATA *ged, 
                                              G3DLIGHT      *lig, 
                                              uint32_t       flags, 
                                              FILE          *fdst ) {
    uint32_t soft = ( ((G3DOBJECT*)lig)->flags & LIGHTSOFTSHADOWS ) ? 0x01 : 0x00;

    return g3dexportv3_fwritel ( &soft, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv3light_shadowColor ( G3DEXPORTV3DATA *ged, 
                                             G3DLIGHT      *lig, 
                                             uint32_t       flags, 
                                             FILE          *fdst ) {
    uint32_t size = 0x00;
    G3DCOLOR color;

    g3drgba_toColor ( &lig->shadowColor, &color );

    size += g3dexportv3_fwritef ( &color.r, fdst );
    size += g3dexportv3_fwritef ( &color.g, fdst );
    size += g3dexportv3_fwritef ( &color.b, fdst );
    size += g3dexportv3_fwritef ( &color.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3light_shadowCasting ( G3DEXPORTV3DATA *ged, 
                                               G3DLIGHT      *lig, 
                                               uint32_t       flags, 
                                               FILE          *fdst ) {
    uint32_t shadowing = ( ((G3DOBJECT*)lig)->flags & LIGHTCASTSHADOWS ) ? 0x01 : 0x00;

    return g3dexportv3_fwritel ( &shadowing, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv3light_shadow ( G3DEXPORTV3DATA *ged, 
                                        G3DLIGHT      *lig, 
                                        uint32_t       flags, 
                                        FILE          *fdst ) {
    uint32_t shadowing = ( ((G3DOBJECT*)lig)->flags & LIGHTCASTSHADOWS ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_LIGHT_SHADOW_CASTING,
                   EXPORTV3_CALLBACK(g3dexportv3light_shadowCasting),
                                     ged,
                                     lig,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_LIGHT_SHADOW_COLOR,
                   EXPORTV3_CALLBACK(g3dexportv3light_shadowColor),
                                     ged,
                                     lig,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_LIGHT_SHADOW_SOFT,
                   EXPORTV3_CALLBACK(g3dexportv3light_shadowSoft),
                                     ged,
                                     lig,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_LIGHT_SHADOW_SOFT_SAMPLING,
                   EXPORTV3_CALLBACK(g3dexportv3light_shadowSoftSampling),
                                     ged,
                                     lig,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_LIGHT_SHADOW_SOFT_RADIUS,
                   EXPORTV3_CALLBACK(g3dexportv3light_shadowSoftRadius),
                                     ged,
                                     lig,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3light_ambient ( G3DEXPORTV3DATA *ged, 
                                         G3DLIGHT      *lig, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t size = 0x00;
    G3DCOLOR color;

    g3drgba_toColor ( &lig->ambientColor, &color );

    size += g3dexportv3_fwritef ( &color.r, fdst );
    size += g3dexportv3_fwritef ( &color.g, fdst );
    size += g3dexportv3_fwritef ( &color.b, fdst );
    size += g3dexportv3_fwritef ( &color.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3light_specular ( G3DEXPORTV3DATA *ged, 
                                          G3DLIGHT      *lig, 
                                          uint32_t       flags, 
                                          FILE          *fdst ) {
    uint32_t size = 0x00;
    G3DCOLOR color;

    g3drgba_toColor ( &lig->specularColor, &color );

    size += g3dexportv3_fwritef ( &color.r, fdst );
    size += g3dexportv3_fwritef ( &color.g, fdst );
    size += g3dexportv3_fwritef ( &color.b, fdst );
    size += g3dexportv3_fwritef ( &color.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3light_diffuse ( G3DEXPORTV3DATA *ged, 
                                         G3DLIGHT      *lig, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t size = 0x00;
    G3DCOLOR color;

    g3drgba_toColor ( &lig->diffuseColor, &color );

    size += g3dexportv3_fwritef ( &color.r, fdst );
    size += g3dexportv3_fwritef ( &color.g, fdst );
    size += g3dexportv3_fwritef ( &color.b, fdst );
    size += g3dexportv3_fwritef ( &color.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3light_intensity ( G3DEXPORTV3DATA *ged, 
                                           G3DLIGHT      *lig, 
                                           uint32_t       flags, 
                                           FILE          *fdst ) {
    return g3dexportv3_fwritef ( &lig->intensity, fdst );
}


/******************************************************************************/
uint32_t g3dexportv3light ( G3DEXPORTV3DATA *ged, 
                          G3DLIGHT     *lig, 
                          uint32_t      flags, 
                          FILE         *fdst ) {
    uint32_t size = 0x00;


    size += g3dexportv3_writeChunk ( SIG_OBJECT_LIGHT_INTENSITY,
                   EXPORTV3_CALLBACK(g3dexportv3light_intensity),
                                     ged,
                                     lig,
                                     0xFFFFFFFF,
                                     fdst );

    if ( ((G3DOBJECT*)lig)->flags & LIGHTCASTSHADOWS ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_LIGHT_SHADOW,
                       EXPORTV3_CALLBACK(g3dexportv3light_shadow),
                                         ged,
                                         lig,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( ((G3DOBJECT*)lig)->flags & SPOTLIGHT ) {
        size += g3dexportv3_writeChunk ( SIG_OBJECT_LIGHT_SPOT,
                       EXPORTV3_CALLBACK(g3dexportv3light_spot),
                                         ged,
                                         lig,
                                         0xFFFFFFFF,
                                         fdst );
    }

    size += g3dexportv3_writeChunk ( SIG_OBJECT_LIGHT_DIFFUSE,
                   EXPORTV3_CALLBACK(g3dexportv3light_diffuse),
                                     ged,
                                     lig,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_LIGHT_SPECULAR,
                   EXPORTV3_CALLBACK(g3dexportv3light_specular),
                                     ged,
                                     lig,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_LIGHT_AMBIENT,
                   EXPORTV3_CALLBACK(g3dexportv3light_ambient),
                                     ged,
                                     lig,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}
