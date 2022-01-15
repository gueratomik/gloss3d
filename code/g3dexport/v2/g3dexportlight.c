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
static uint32_t g3dexportv2light_spot ( G3DEXPORTV2DATA *ged, 
                                      G3DLIGHT      *lig, 
                                      uint32_t       flags, 
                                      FILE          *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv2_fwritef ( &lig->spotLength   , fdst );
    size += g3dexportv2_fwritef ( &lig->spotAngle    , fdst );
    size += g3dexportv2_fwritef ( &lig->spotFadeAngle, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2light_shadowColor ( G3DEXPORTV2DATA *ged, 
                                             G3DLIGHT      *lig, 
                                             uint32_t       flags, 
                                             FILE          *fdst ) {
    uint32_t size = 0x00;
    G3DCOLOR color;

    g3drgba_toColor ( &lig->shadowColor, &color );

    size += g3dexportv2_fwritef ( &color.r, fdst );
    size += g3dexportv2_fwritef ( &color.g, fdst );
    size += g3dexportv2_fwritef ( &color.b, fdst );
    size += g3dexportv2_fwritef ( &color.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2light_shadowCasting ( G3DEXPORTV2DATA *ged, 
                                               G3DLIGHT      *lig, 
                                               uint32_t       flags, 
                                               FILE          *fdst ) {
    uint32_t shadowing = ( ((G3DOBJECT*)lig)->flags & LIGHTCASTSHADOWS ) ? 0x01 : 0x00;

    return g3dexportv2_fwritel ( &shadowing, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv2light_shadow ( G3DEXPORTV2DATA *ged, 
                                        G3DLIGHT      *lig, 
                                        uint32_t       flags, 
                                        FILE          *fdst ) {
    uint32_t shadowing = ( ((G3DOBJECT*)lig)->flags & LIGHTCASTSHADOWS ) ? 0x01 : 0x00;
    uint32_t size = 0x00;

    size += g3dexportv2_writeChunk ( SIG_OBJECT_LIGHT_SHADOW_CASTING,
                   EXPORTV2_CALLBACK(g3dexportv2light_shadowCasting),
                                     ged,
                                     lig,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv2_writeChunk ( SIG_OBJECT_LIGHT_SHADOW_COLOR,
                   EXPORTV2_CALLBACK(g3dexportv2light_shadowColor),
                                     ged,
                                     lig,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2light_ambient ( G3DEXPORTV2DATA *ged, 
                                         G3DLIGHT      *lig, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t size = 0x00;
    G3DCOLOR color;

    g3drgba_toColor ( &lig->ambientColor, &color );

    size += g3dexportv2_fwritef ( &color.r, fdst );
    size += g3dexportv2_fwritef ( &color.g, fdst );
    size += g3dexportv2_fwritef ( &color.b, fdst );
    size += g3dexportv2_fwritef ( &color.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2light_specular ( G3DEXPORTV2DATA *ged, 
                                          G3DLIGHT      *lig, 
                                          uint32_t       flags, 
                                          FILE          *fdst ) {
    uint32_t size = 0x00;
    G3DCOLOR color;

    g3drgba_toColor ( &lig->specularColor, &color );

    size += g3dexportv2_fwritef ( &color.r, fdst );
    size += g3dexportv2_fwritef ( &color.g, fdst );
    size += g3dexportv2_fwritef ( &color.b, fdst );
    size += g3dexportv2_fwritef ( &color.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2light_diffuse ( G3DEXPORTV2DATA *ged, 
                                         G3DLIGHT      *lig, 
                                         uint32_t       flags, 
                                         FILE          *fdst ) {
    uint32_t size = 0x00;
    G3DCOLOR color;

    g3drgba_toColor ( &lig->diffuseColor, &color );

    size += g3dexportv2_fwritef ( &color.r, fdst );
    size += g3dexportv2_fwritef ( &color.g, fdst );
    size += g3dexportv2_fwritef ( &color.b, fdst );
    size += g3dexportv2_fwritef ( &color.a, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv2light_intensity ( G3DEXPORTV2DATA *ged, 
                                           G3DLIGHT      *lig, 
                                           uint32_t       flags, 
                                           FILE          *fdst ) {
    return g3dexportv2_fwritef ( &lig->intensity, fdst );
}


/******************************************************************************/
uint32_t g3dexportv2light ( G3DEXPORTV2DATA *ged, 
                          G3DLIGHT     *lig, 
                          uint32_t      flags, 
                          FILE         *fdst ) {
    uint32_t size = 0x00;


    size += g3dexportv2_writeChunk ( SIG_OBJECT_LIGHT_INTENSITY,
                   EXPORTV2_CALLBACK(g3dexportv2light_intensity),
                                     ged,
                                     lig,
                                     0xFFFFFFFF,
                                     fdst );

    if ( ((G3DOBJECT*)lig)->flags & LIGHTCASTSHADOWS ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_LIGHT_SHADOW,
                       EXPORTV2_CALLBACK(g3dexportv2light_shadow),
                                         ged,
                                         lig,
                                         0xFFFFFFFF,
                                         fdst );
    }

    if ( ((G3DOBJECT*)lig)->flags & SPOTLIGHT ) {
        size += g3dexportv2_writeChunk ( SIG_OBJECT_LIGHT_SPOT,
                       EXPORTV2_CALLBACK(g3dexportv2light_spot),
                                         ged,
                                         lig,
                                         0xFFFFFFFF,
                                         fdst );
    }

    size += g3dexportv2_writeChunk ( SIG_OBJECT_LIGHT_DIFFUSE,
                   EXPORTV2_CALLBACK(g3dexportv2light_diffuse),
                                     ged,
                                     lig,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv2_writeChunk ( SIG_OBJECT_LIGHT_SPECULAR,
                   EXPORTV2_CALLBACK(g3dexportv2light_specular),
                                     ged,
                                     lig,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv2_writeChunk ( SIG_OBJECT_LIGHT_AMBIENT,
                   EXPORTV2_CALLBACK(g3dexportv2light_ambient),
                                     ged,
                                     lig,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}
