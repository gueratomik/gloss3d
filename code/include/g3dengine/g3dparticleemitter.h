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

/**
 * @file
 */

/******************************************************************************/
#ifndef _G3DPARTICLEEMITTER_H_
#define _G3DPARTICLEEMITTER_H_

#ifdef __cplusplus
extern "C" {
#endif

#define PARTICLE_ISALIVE ( 1 << 0 )

typedef struct _G3DPARTICLE {
    uint32_t   flags;
    G3DOBJECT *ref;
    G3DVECTOR  pos;
    G3DVECTOR  rot;
    G3DVECTOR  sca;
    double     MVX[0x10];
    G3DVECTOR  speed;
    G3DVECTOR  accel;
    int32_t    startAtFrame;
    float    lifeTime; /*** warning ; must be SIGNED int32_t ***/
    float      transparency;
    G3DVECTOR  initialScaling;
} G3DPARTICLE;

/**
 * @struct G3DPARTICLEEMITTER
 * @brief A structure to store a 3D Particle System.
 */
typedef struct _G3DPARTICLEEMITTER {
    G3DOBJECT    obj;

    G3DVECTOR    gravity;

    G3DVECTOR    initialAccel;
    G3DVECTOR    initialSpeed;
    G3DVECTOR    initialScaling;
    G3DVECTOR    initialRotation;
    float        initialTransparency;

    G3DVECTOR    initialVarAngle;
    G3DVECTOR    initialVarAccel;
    G3DVECTOR    initialVarSpeed;
    G3DVECTOR    initialVarScaling;
    G3DVECTOR    initialVarRotation;
    float        initialVarTransparency;

    G3DVECTOR    finalAccel;
    G3DVECTOR    finalSpeed;
    G3DVECTOR    finalRotation;
    G3DVECTOR    finalScaling;
    float        finalTransparency;

    uint32_t     particleLifetime; /* in frames */
    uint32_t     particleLifetimeVariation;
    float        particleMass;

    uint32_t     maxParticlesPerFrame;
    uint32_t     maxPreviewsPerFrame;
    float        particlesPerFrame;

    int32_t      startAtFrame;
    int32_t      endAtFrame;

    G3DPARTICLE *particles;
    uint32_t     maxParticles;
    float        oldFrame;

    G3DSEGMENT   seg1;
    G3DSEGMENT   seg2;
    float        radius;

    uint32_t     type;

    double       TIWMVX[0x10];
} G3DPARTICLEEMITTER;

/******************************************************************************/
/**
 * create a new particle emitter.
 * @param id the object's ID.
 * @param name the object's name.
 * @return the allocated G3DPARTICLEEMITTER.
 */
G3DPARTICLEEMITTER *g3dparticleemitter_new ( uint32_t id, 
                                             char    *name );

void g3dparticleemitter_reset ( G3DPARTICLEEMITTER *pem );

#ifdef __cplusplus
}
#endif

#endif
