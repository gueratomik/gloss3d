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
static uint32_t g3dexportlight_spot ( G3DEXPORTDATA *ged, 
                                      G3DLIGHT      *lig, 
                                      uint32_t       flags, 
                                      FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexport_fwritef ( &lig->spotLength   , fdst );
    size += g3dexport_fwritef ( &lig->spotAngle    , fdst );
    size += g3dexport_fwritef ( &lig->spotFadeAngle, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportlight_shadowColor ( G3DEXPORTDATA *ged, 
                                             G3DLIGHT      *lig, 
                                             uint32_t       flags, 
                                             FILE          *fdst ) {
    uint32_t size = 0x00;
    G3DCOLOR color;

    g3drgba_toColor ( &lig->shadowColor, &color );

    size += g3dexport_fwritef ( &color.r, fdst );
    size += g3dexport_fwritef ( &color.g, fdst );
    size += g3dexport_fwritef ( &color.b, fdst );
    size += g3dexport_fwritef ( &color.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportlight_shadowCasting ( G3DEXPORTDATA *ged, 
                                               G3DLIGHT      *lig, 
                                               uint32_t       flags, 
                                               FILE          *fdst ) {
    uint32_t shadowing = ( ((G3DOBJECT*)lig)->flags & LIGHTCASTSHADOWS ) ? 0x01 : 0x00;

    return g3dexport_fwritel ( &shadowing, fdst );
}

/******************************************************************************/
static uint32_t g3dexportlight_shadow ( G3DEXPORTDATA *ged, 
                                        G3DLIGHT      *lig, 
                                        uint32_t       flags, 
                                        FILE          *fdst ) {
    uint32_t shadowing = ( ((G3DOBJECT*)lig)->flags & LIGHTCASTSHADOWS ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexport_writeChunk ( SIG_OBJECT_LIGHT_SHADOW_CASTING,
                                   g3dexportlight_shadowCasting,
                                   ged,
                                   lig,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_LIGHT_SHADOW_COLOR,
                                   g3dexportlight_shadowColor,
                                   ged,
                                   lig,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportlight_ambient ( G3DEXPORTDATA *ged, 
                                         G3DLIGHT      *lig, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t size = 0x00;
    G3DCOLOR color;

    g3drgba_toColor ( &lig->ambientColor, &color );

    size += g3dexport_fwritef ( &color.r, fdst );
    size += g3dexport_fwritef ( &color.g, fdst );
    size += g3dexport_fwritef ( &color.b, fdst );
    size += g3dexport_fwritef ( &color.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportlight_specular ( G3DEXPORTDATA *ged, 
                                          G3DLIGHT      *lig, 
                                          uint32_t       flags, 
                                          FILE          *fdst ) {
    uint32_t size = 0x00;
    G3DCOLOR color;

    g3drgba_toColor ( &lig->specularColor, &color );

    size += g3dexport_fwritef ( &color.r, fdst );
    size += g3dexport_fwritef ( &color.g, fdst );
    size += g3dexport_fwritef ( &color.b, fdst );
    size += g3dexport_fwritef ( &color.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportlight_diffuse ( G3DEXPORTDATA *ged, 
                                         G3DLIGHT      *lig, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t size = 0x00;
    G3DCOLOR color;

    g3drgba_toColor ( &lig->diffuseColor, &color );

    size += g3dexport_fwritef ( &color.r, fdst );
    size += g3dexport_fwritef ( &color.g, fdst );
    size += g3dexport_fwritef ( &color.b, fdst );
    size += g3dexport_fwritef ( &color.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportlight_intensity ( G3DEXPORTDATA *ged, 
                                           G3DLIGHT      *lig, 
                                           uint32_t       flags, 
                                           FILE          *fdst ) {
    return g3dexport_fwritef ( &lig->intensity, fdst );
}


/******************************************************************************/
uint32_t g3dexportlight ( G3DEXPORTDATA *ged, 
                          G3DLIGHT     *lig, 
                          uint32_t      flags, 
                          FILE         *fdst ) {
    uint32_t size = 0x00;


    size += g3dexport_writeChunk ( SIG_OBJECT_LIGHT_INTENSITY,
                                   g3dexportlight_intensity,
                                   ged,
                                   lig,
                                   0xFFFFFFFF,
                                   fdst );

    if ( ((G3DOBJECT*)lig)->flags & LIGHTCASTSHADOWS ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_LIGHT_SHADOW,
                                       g3dexportlight_shadow,
                                       ged,
                                       lig,
                                       0xFFFFFFFF,
                                       fdst );
    }

    if ( ((G3DOBJECT*)lig)->flags & SPOTLIGHT ) {
        size += g3dexport_writeChunk ( SIG_OBJECT_LIGHT_SPOT,
                                       g3dexportlight_spot,
                                       ged,
                                       lig,
                                       0xFFFFFFFF,
                                       fdst );
    }

    size += g3dexport_writeChunk ( SIG_OBJECT_LIGHT_DIFFUSE,
                                   g3dexportlight_diffuse,
                                   ged,
                                   lig,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_LIGHT_SPECULAR,
                                   g3dexportlight_specular,
                                   ged,
                                   lig,
                                   0xFFFFFFFF,
                                   fdst );

    size += g3dexport_writeChunk ( SIG_OBJECT_LIGHT_AMBIENT,
                                   g3dexportlight_ambient,
                                   ged,
                                   lig,
                                   0xFFFFFFFF,
                                   fdst );

    return size;
}
