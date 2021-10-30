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

#ifdef unused
/******************************************************************************/
static G3DPARTICLEEMITTER *g3dparticleemitter_copy ( G3DPARTICLEEMITTER *pem, 
                                                     uint32_t            id, 
                                                     unsigned char      *name,
                                                     uint64_t            engine_flags ) {
    G3DPARTICLEEMITTER *cpypem = g3dparticleemitter_new ( ((G3DOBJECT*)pem)->id,
                                                          ((G3DOBJECT*)pem)->name );

    cpypem->initialAcceleration       = pem->initialAcceleration;
    cpypem->particlesPerSecondPreview = pem->particlesPerSecondPreview;
    cpypem->particlesPerSecondRender  = pem->particlesPerSecondRender;
    cpypem->particleLifeTime          = pem->particleLifeTime;
    cpypem->scaleFactor               = pem->scaleFactor;
    cpypem->particlesPreview          = pem->particlesPreview;
    cpypem->particlesRender           = pem->particlesRender;
    cpypem->maxParticlesPreview       = pem->maxParticlesPreview;
    cpypem->maxParticlesRender        = pem->maxParticlesRender;
    cpypem->startAtFrame              = pem->startAtFrame;

    g3dparticleemitter_reset ( cpypem );


    return cpypem;
}

/******************************************************************************/
static uint32_t g3dparticleemitter_draw ( G3DPARTICLEEMITTER *pem, 
                                          G3DCAMERA          *curcam, 
                                          uint64_t            engine_flags ) {



    return 0x00;
}

/******************************************************************************/
static void g3dparticleemitter_free ( G3DPARTICLEEMITTER *pem ) {

}

/******************************************************************************/
static uint32_t g3dparticleemitter_pick ( G3DPARTICLEEMITTER *pem, 
                                          G3DCAMERA          *curcam, 
                                          uint64_t            engine_flags ) {

    return 0x00;
}

/******************************************************************************/
static void g3dparticleemitter_transform ( G3DPARTICLEEMITTER *pem,
                                           uint64_t            engine_flags ) {

}

/******************************************************************************/
static void g3dparticleemitter_anim ( G3DMORPHER *mpr, 
                                      float       frame, 
                                      uint64_t    engine_flags ) {
    float localFrame = ( int32_t ) ( pem->startAtFrame - frame ) %
                       ( int32_t ) ( pem->lifeTime ) + frame - ( int32_t ) frame;
    uint32_t i;

    printf ("%f\n", localFrame );

    for ( i = 0x00; i < pem->maxParticlesPreview; i++ ) {
        float emittedAtFrame = pem->particlesPreview[i].emmittedAtFrame;

        pem->particlesPreview[i].lifeTime = emittedAtFrame + localFrame;

        if ( pem->particlesPreview[i].lifeTime > pem->particleLifeTime ) {
            pem->particlesPreview[i].lifeTime
        }
    }
}

/******************************************************************************/
void g3dparticleemitter_reset ( G3DPARTICLEEMITTER *pem, 
                                uint32_t            fps ) {
    float maxParticlesPreview = ( pem->particleLifeTime * 
                                  pem->particlesPerFramePreview );

    pem->maxParticlesPreview = 

    if ( pem->maxParticlesPreview ) {
        pem->particlesPreview = ( G3DPARTICLE * ) calloc ( pem->maxParticlesPreview,
                                                           sizeof ( G3DPARTICLE ) );
    }
}

/******************************************************************************/
void g3dparticleemitter_init ( G3DPARTICLEEMITTER *pem, 
                               uint32_t            id, 
                               char               *name ) {
    g3dobject_init ( pem, 
                     G3DPARTICLEEMITTERTYPE,
                     id, 
                     name, 
                     0x00,
       DRAW_CALLBACK(g3dparticleemitter_draw),
       FREE_CALLBACK(g3dparticleemitter_free),
       PICK_CALLBACK(g3dparticleemitter_pick),
                     NULL,
       COPY_CALLBACK(g3dparticleemitter_copy),
                     NULL,
                     NULL,
                     NULL,
   ADDCHILD_CALLBACK(NULL),
                     NULL );

    ((G3DOBJECT*)pem)->transform = g3dparticleemitter_transform;

    pem->orientation = INSTANCEYZ;

    /*mes->dump           = g3dmesh_default_dump;*/
}

/******************************************************************************/
G3DPARTICLEEMITTER *g3dparticleemitter_new ( uint32_t id, 
                                             char    *name ) {
    G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) calloc ( 0x01, sizeof ( G3DPARTICLEEMITTER ) );

    if ( pem == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    g3dparticleemitter_init ( pem, id, name );


    return pem;
}
#endif
