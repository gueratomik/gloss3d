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
static uint32_t g3dexportv3particleemitter_initialVarTransparency ( G3DEXPORTV3DATA    *ged, 
                                                                    G3DPARTICLEEMITTER *pem, 
                                                                    uint32_t            flags, 
                                                                    FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &pem->initialVarTransparency, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3particleemitter_initialVarRotation ( G3DEXPORTV3DATA    *ged, 
                                                                G3DPARTICLEEMITTER *pem, 
                                                                uint32_t            flags, 
                                                                FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &pem->initialVarRotation.x, fdst );
    size += g3dexportv3_fwritef ( &pem->initialVarRotation.y, fdst );
    size += g3dexportv3_fwritef ( &pem->initialVarRotation.z, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3particleemitter_initialVarScaling ( G3DEXPORTV3DATA    *ged, 
                                                               G3DPARTICLEEMITTER *pem, 
                                                               uint32_t            flags, 
                                                               FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &pem->initialVarScaling.x, fdst );
    size += g3dexportv3_fwritef ( &pem->initialVarScaling.y, fdst );
    size += g3dexportv3_fwritef ( &pem->initialVarScaling.z, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3particleemitter_initialVarSpeed ( G3DEXPORTV3DATA    *ged, 
                                                             G3DPARTICLEEMITTER *pem, 
                                                             uint32_t            flags, 
                                                             FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &pem->initialVarSpeed.x, fdst );
    size += g3dexportv3_fwritef ( &pem->initialVarSpeed.y, fdst );
    size += g3dexportv3_fwritef ( &pem->initialVarSpeed.z, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3particleemitter_initialVarAcceleration ( G3DEXPORTV3DATA    *ged, 
                                                                    G3DPARTICLEEMITTER *pem, 
                                                                    uint32_t            flags, 
                                                                    FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &pem->initialVarAccel.x, fdst );
    size += g3dexportv3_fwritef ( &pem->initialVarAccel.y, fdst );
    size += g3dexportv3_fwritef ( &pem->initialVarAccel.z, fdst );

    return size;
}

/******************************************************************************/
/******************************************************************************/
static uint32_t g3dexportv3particleemitter_finalTransparency ( G3DEXPORTV3DATA    *ged, 
                                                               G3DPARTICLEEMITTER *pem, 
                                                               uint32_t            flags, 
                                                               FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &pem->finalTransparency, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3particleemitter_finalRotation ( G3DEXPORTV3DATA    *ged, 
                                                           G3DPARTICLEEMITTER *pem, 
                                                           uint32_t            flags, 
                                                           FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &pem->finalRotation.x, fdst );
    size += g3dexportv3_fwritef ( &pem->finalRotation.y, fdst );
    size += g3dexportv3_fwritef ( &pem->finalRotation.z, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3particleemitter_finalScaling ( G3DEXPORTV3DATA    *ged, 
                                                          G3DPARTICLEEMITTER *pem, 
                                                          uint32_t            flags, 
                                                          FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &pem->finalScaling.x, fdst );
    size += g3dexportv3_fwritef ( &pem->finalScaling.y, fdst );
    size += g3dexportv3_fwritef ( &pem->finalScaling.z, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3particleemitter_finalSpeed ( G3DEXPORTV3DATA    *ged, 
                                                        G3DPARTICLEEMITTER *pem, 
                                                        uint32_t            flags, 
                                                        FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &pem->finalSpeed.x, fdst );
    size += g3dexportv3_fwritef ( &pem->finalSpeed.y, fdst );
    size += g3dexportv3_fwritef ( &pem->finalSpeed.z, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3particleemitter_finalAcceleration ( G3DEXPORTV3DATA    *ged, 
                                                               G3DPARTICLEEMITTER *pem, 
                                                               uint32_t            flags, 
                                                               FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &pem->finalAccel.x, fdst );
    size += g3dexportv3_fwritef ( &pem->finalAccel.y, fdst );
    size += g3dexportv3_fwritef ( &pem->finalAccel.z, fdst );

    return size;
}

/******************************************************************************/
/******************************************************************************/
static uint32_t g3dexportv3particleemitter_initialTransparency ( G3DEXPORTV3DATA    *ged, 
                                                                 G3DPARTICLEEMITTER *pem, 
                                                                 uint32_t            flags, 
                                                                 FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &pem->initialTransparency, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3particleemitter_initialRotation ( G3DEXPORTV3DATA    *ged, 
                                                             G3DPARTICLEEMITTER *pem, 
                                                             uint32_t            flags, 
                                                             FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &pem->initialRotation.x, fdst );
    size += g3dexportv3_fwritef ( &pem->initialRotation.y, fdst );
    size += g3dexportv3_fwritef ( &pem->initialRotation.z, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3particleemitter_initialScaling ( G3DEXPORTV3DATA    *ged, 
                                                            G3DPARTICLEEMITTER *pem, 
                                                            uint32_t            flags, 
                                                            FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &pem->initialScaling.x, fdst );
    size += g3dexportv3_fwritef ( &pem->initialScaling.y, fdst );
    size += g3dexportv3_fwritef ( &pem->initialScaling.z, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3particleemitter_initialSpeed ( G3DEXPORTV3DATA    *ged, 
                                                          G3DPARTICLEEMITTER *pem, 
                                                          uint32_t            flags, 
                                                          FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &pem->initialSpeed.x, fdst );
    size += g3dexportv3_fwritef ( &pem->initialSpeed.y, fdst );
    size += g3dexportv3_fwritef ( &pem->initialSpeed.z, fdst );

    return size;
}

/******************************************************************************/
static uint32_t g3dexportv3particleemitter_initialAcceleration ( G3DEXPORTV3DATA    *ged, 
                                                                 G3DPARTICLEEMITTER *pem, 
                                                                 uint32_t            flags, 
                                                                 FILE               *fdst ) {
    uint32_t size = 0x00;

    size += g3dexportv3_fwritef ( &pem->initialAccel.x, fdst );
    size += g3dexportv3_fwritef ( &pem->initialAccel.y, fdst );
    size += g3dexportv3_fwritef ( &pem->initialAccel.z, fdst );

    return size;
}

/******************************************************************************/
/******************************************************************************/
static uint32_t g3dexportv3particleemitter_displayParticles ( G3DEXPORTV3DATA    *ged, 
                                                              G3DPARTICLEEMITTER *pem, 
                                                              uint32_t            flags, 
                                                              FILE               *fdst ) {
    uint32_t displayPart = ( pem->obj.flags & DISPLAYPARTICLES ) ? 0x01 : 0x00;

    return g3dexportv3_fwritel ( &displayPart, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv3particleemitter_radius ( G3DEXPORTV3DATA    *ged, 
                                                    G3DPARTICLEEMITTER *pem, 
                                                    uint32_t            flags, 
                                                    FILE               *fdst ) {
    return g3dexportv3_fwritef ( &pem->radius, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv3particleemitter_lifetime ( G3DEXPORTV3DATA    *ged, 
                                                      G3DPARTICLEEMITTER *pem, 
                                                      uint32_t            flags, 
                                                      FILE               *fdst ) {
    return g3dexportv3_fwritel ( &pem->particleLifetime, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv3particleemitter_perFrame ( G3DEXPORTV3DATA    *ged, 
                                                      G3DPARTICLEEMITTER *pem, 
                                                      uint32_t            flags, 
                                                      FILE               *fdst ) {
    return g3dexportv3_fwritef ( &pem->particlesPerFrame, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv3particleemitter_type ( G3DEXPORTV3DATA    *ged, 
                                                  G3DPARTICLEEMITTER *pem, 
                                                  uint32_t            flags, 
                                                  FILE               *fdst ) {
    return g3dexportv3_fwritel ( &pem->type, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv3particleemitter_endAt ( G3DEXPORTV3DATA    *ged, 
                                                   G3DPARTICLEEMITTER *pem, 
                                                   uint32_t            flags, 
                                                   FILE               *fdst ) {
    return g3dexportv3_fwritel ( &pem->endAtFrame, fdst );
}

/******************************************************************************/
static uint32_t g3dexportv3particleemitter_startAt ( G3DEXPORTV3DATA    *ged, 
                                                     G3DPARTICLEEMITTER *pem, 
                                                     uint32_t            flags, 
                                                     FILE               *fdst ) {
    return g3dexportv3_fwritel ( &pem->startAtFrame, fdst );
}

/******************************************************************************/
uint32_t g3dexportv3particleemitter ( G3DEXPORTV3DATA    *ged, 
                                      G3DPARTICLEEMITTER *pem, 
                                      uint32_t            flags, 
                                      FILE               *fdst ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) pem;
    uint32_t size = 0x00;

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_STARTAT,
                                     g3dexportv3particleemitter_startAt,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_ENDAT,
                                     g3dexportv3particleemitter_endAt,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_TYPE,
                                     g3dexportv3particleemitter_type,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_PERFRAME,
                                     g3dexportv3particleemitter_perFrame,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_LIFETIME,
                                     g3dexportv3particleemitter_lifetime,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_RADIUS,
                                     g3dexportv3particleemitter_radius,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_DISPLAYPARTICLES,
                                     g3dexportv3particleemitter_displayParticles,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    /*******************************************************************/

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_INITIAL_ACCELERATION,
                                     g3dexportv3particleemitter_initialAcceleration,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_INITIAL_SPEED,
                                     g3dexportv3particleemitter_initialSpeed,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_INITIAL_SCALING,
                                     g3dexportv3particleemitter_initialScaling,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_INITIAL_ROTATION,
                                     g3dexportv3particleemitter_initialRotation,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_INITIAL_TRANSPARENCY,
                                     g3dexportv3particleemitter_initialTransparency,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    /*******************************************************************/

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_FINAL_ACCELERATION,
                                     g3dexportv3particleemitter_finalAcceleration,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_FINAL_SPEED,
                                     g3dexportv3particleemitter_finalSpeed,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_FINAL_SCALING,
                                     g3dexportv3particleemitter_finalScaling,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_FINAL_ROTATION,
                                     g3dexportv3particleemitter_finalRotation,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_FINAL_TRANSPARENCY,
                                     g3dexportv3particleemitter_finalTransparency,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    /*******************************************************************/

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_INITIALVAR_ACCELERATION,
                                     g3dexportv3particleemitter_initialVarAcceleration,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_INITIALVAR_SPEED,
                                     g3dexportv3particleemitter_initialVarSpeed,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_INITIALVAR_SCALING,
                                     g3dexportv3particleemitter_initialVarScaling,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_INITIALVAR_ROTATION,
                                     g3dexportv3particleemitter_initialVarRotation,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    size += g3dexportv3_writeChunk ( SIG_OBJECT_PARTICLEEMITTER_INITIALVAR_TRANSPARENCY,
                                     g3dexportv3particleemitter_initialVarTransparency,
                                     ged,
                                     pem,
                                     0xFFFFFFFF,
                                     fdst );

    return size;
}
