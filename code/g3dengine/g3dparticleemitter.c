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

/******************************************************************************/
static inline void getRandomPointOnSquare ( G3DSEGMENT *s1, 
                                            G3DSEGMENT *s2,
                                            float radius,
                                            G3DVECTOR  *pnt ) {
    float ratio2 = -1+2*((float)rand())/RAND_MAX;
    float ratio3 = -1+2*((float)rand())/RAND_MAX;

    pnt->x = ratio2 * radius;
    pnt->y = 0.0f;
    pnt->z = ratio3 * radius;
}

/******************************************************************************/
static G3DPARTICLEEMITTER *g3dparticleemitter_copy ( G3DPARTICLEEMITTER *pem, 
                                                     uint32_t            id, 
                                                     unsigned char      *name,
                                                     uint64_t            engine_flags ) {
    G3DPARTICLEEMITTER *cpypem = g3dparticleemitter_new ( ((G3DOBJECT*)pem)->id,
                                                          ((G3DOBJECT*)pem)->name );

    cpypem->initialAccel        = pem->initialAccel;
    cpypem->initialSpeed        = pem->initialSpeed;
    cpypem->particlesPerFrame   = pem->particlesPerFrame;
    cpypem->particleLifeTime    = pem->particleLifeTime;
    cpypem->scaleFactor         = pem->scaleFactor;
    cpypem->startAtFrame        = pem->startAtFrame;

    g3dparticleemitter_reset ( cpypem );


    return cpypem;
}

/******************************************************************************/
static uint32_t g3dparticleemitter_draw ( G3DPARTICLEEMITTER *pem, 
                                          G3DCAMERA          *curcam, 
                                          uint64_t            engine_flags ) {
    if ( pem->maxParticles ) {
        uint32_t i, j;

        glPushAttrib( GL_ALL_ATTRIB_BITS );
        glDisable   ( GL_LIGHTING );
        glColor3ub  ( 0xFF, 0xFF, 0xFF );
        glPointSize ( 3.0f );

        glBegin ( GL_POINTS );

        for ( i = 0x00; i < pem->particleLifeTime; i++ ) {
            G3DPARTICLE *prt = pem->particles + ( pem->maxParticlesPerFrame * i );

            for ( j = 0x00; j < pem->maxParticlesPerFrame; j++ ) {
                if ( prt[j].lifeTime != pem->particleLifeTime ) {
                    glPushMatrix ( );
                    glTranslatef ( prt[j].pos.x, 
                                   prt[j].pos.y, 
                                   prt[j].pos.z );

                    if ( prt[j].ref ) {
                        if ( prt[j].ref->draw ) prt[j].ref->draw ( prt[j].ref, curcam, engine_flags );
                    }

                    glPopMatrix();
                }
            }
        }


        glEnd ( );

        glPopAttrib ( );
    }


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
static void g3dparticleemitter_anim ( G3DPARTICLEEMITTER *pem, 
                                      float               frame, 
                                      uint64_t            engine_flags ) {
    float deltaFrame = frame - pem->oldFrame;

    if ( pem->maxParticles ) {
        G3DSEGMENT s1 = { .src = { .x =  pem->radius, .y = 0.0f, .z =  pem->radius },
                          .dst = { .x =  pem->radius, .y = 0.0f, .z = -pem->radius } },
                   s2 = { .src = { .x = -pem->radius, .y = 0.0f, .z =  pem->radius },
                          .dst = { .x =  pem->radius, .y = 0.0f, .z =  pem->radius } };
        int32_t iFrame = ( int32_t ) ( frame - pem->startAtFrame );
        int32_t localFrame = ( iFrame % pem->particleLifeTime );
        int32_t i, j;

        if ( frame == pem->startAtFrame ) {
            g3dparticleemitter_reset ( pem );
        }

        for ( i = 0x00; i < pem->particleLifeTime; i++ ) {
            G3DPARTICLE *prt = pem->particles + ( pem->maxParticlesPerFrame * i );

            for ( j = 0x00; j < pem->maxParticlesPerFrame; j++ ) {
                /*prt[j].lifeTime = ( int32_t ) i - localFrame;*/

                if ( prt[j].lifeTime < 0x00 ) {
                    getRandomPointOnSquare ( &s1, &s2, pem->radius, &prt[j].pos );

                    prt[j].startAtFrame = i;
                    prt[j].lifeTime = pem->particleLifeTime;
                } else {
                    if ( frame >= prt[j].startAtFrame ) {
                         prt[j].pos.x += ( float ) prt[j].speed.x * deltaFrame;
                         prt[j].pos.y += ( float ) prt[j].speed.y * deltaFrame;
                         prt[j].pos.z += ( float ) prt[j].speed.z * deltaFrame;

                         prt[j].lifeTime -= deltaFrame;
                    }
                }
            }
        }
    }

    pem->oldFrame = frame;
}

/******************************************************************************/
void g3dparticleemitter_reset ( G3DPARTICLEEMITTER *pem ) {
    pem->maxParticlesPerFrame = ceilf ( pem->particlesPerFrame );
    pem->maxParticles = pem->maxParticlesPerFrame * pem->particleLifeTime;

    if ( pem->particles ) free ( pem->particles );

    if ( pem->maxParticles ) {
        G3DSEGMENT s1 = { .src = { .x =  pem->radius, .y = 0.0f, .z =  pem->radius },
                          .dst = { .x =  pem->radius, .y = 0.0f, .z = -pem->radius } },
                   s2 = { .src = { .x = -pem->radius, .y = 0.0f, .z =  pem->radius },
                          .dst = { .x =  pem->radius, .y = 0.0f, .z =  pem->radius } };
        uint32_t i, j;

        pem->particles = ( G3DPARTICLE * ) calloc ( pem->maxParticles,
                                                    sizeof ( G3DPARTICLE ) );

        for ( i = 0x00; i < pem->particleLifeTime; i++ ) {
            G3DPARTICLE *prt = pem->particles + ( pem->maxParticlesPerFrame * i );

            for ( j = 0x00; j < pem->maxParticlesPerFrame; j++ ) {
                getRandomPointOnSquare ( &s1, &s2, pem->radius, &prt[j].pos );

                prt[j].ref = g3dobject_getRandomChild ( pem );

                prt[j].accel.x = 
                prt[j].accel.y = 
                prt[j].accel.z = 0.0f;

                prt[j].speed.x = pem->initialSpeed.x;
                prt[j].speed.y = pem->initialSpeed.y;
                prt[j].speed.z = pem->initialSpeed.z;

                prt[j].startAtFrame = i;
                prt[j].lifeTime = pem->particleLifeTime;
            }
        }
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

    /*pem->orientation = INSTANCEYZ;*/

    /*mes->dump           = g3dmesh_default_dump;*/

    ((G3DOBJECT*)pem)->anim = ANIM_CALLBACK(g3dparticleemitter_anim);

    /*pem->initialAcceleration = 0.0f;*/
    pem->particlesPerFrame   = 1.0f;
    pem->particleLifeTime    = 10;
    /*pem->scaleFactor         = 1.0f;*/
    pem->startAtFrame        = 0.0f;

    pem->initialSpeed.x = 0.0f;
    pem->initialSpeed.y = 0.32f;
    pem->initialSpeed.z = 0.0f;

    pem->radius = 1.0f;
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

    g3dparticleemitter_reset ( pem );


    return pem;
}

