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
static void g3dparticle_init ( G3DPARTICLE *prt,
                               G3DOBJECT   *ref,
                               G3DVECTOR   *pos,
                               G3DVECTOR   *sca,
                               G3DVECTOR   *rot,
                               G3DVECTOR   *accel,
                               G3DVECTOR   *speed,
                               int32_t      startAtFrame,
                               float        transparency ) {
    prt->ref = ref;

    prt->pos.x = pos->x;
    prt->pos.y = pos->y;
    prt->pos.z = pos->z;

    prt->initialScaling.x = sca->x;
    prt->initialScaling.y = sca->y;
    prt->initialScaling.z = sca->z;

    prt->sca.x = 0.0f;
    prt->sca.y = 0.0f;
    prt->sca.z = 0.0f;

    prt->rot.x = rot->x;
    prt->rot.y = rot->y;
    prt->rot.z = rot->z;

    prt->accel.x = accel->x;
    prt->accel.y = accel->y;
    prt->accel.z = accel->z;

    prt->speed.x = speed->x;
    prt->speed.y = speed->y;
    prt->speed.z = speed->z;

    prt->startAtFrame = startAtFrame;
    prt->lifeTime = 0;

    prt->transparency = transparency;
}

/******************************************************************************/
static void g3dparticleemitter_initParticle ( G3DPARTICLEEMITTER *pem,
                                              G3DPARTICLE        *prt,
                                              int32_t             startAtFrame ) {
    float rd = ( float ) rand ( ) / RAND_MAX;

    G3DVECTOR initialAccelVariation = { .x = ( pem->initialAccel.x * rd *
                                               pem->initialAccelVariation.x ),
                                        .y = ( pem->initialAccel.y * rd *
                                               pem->initialAccelVariation.y ),
                                        .z = ( pem->initialAccel.z * rd *
                                               pem->initialAccelVariation.z ) };
    G3DVECTOR initialSpeedVariation = { .x = ( pem->initialSpeed.x * rd *
                                               pem->initialSpeedVariation.x ),
                                        .y = ( pem->initialSpeed.y * rd *
                                               pem->initialSpeedVariation.y ),
                                        .z = ( pem->initialSpeed.z * rd *
                                               pem->initialSpeedVariation.z ) };
    G3DVECTOR initialScalingVariation = { .x = ( pem->initialScaling.x * rd *
                                                 pem->initialScalingVariation.x ),
                                          .y = ( pem->initialScaling.y * rd *
                                                 pem->initialScalingVariation.y ),
                                          .z = ( pem->initialScaling.z * rd *
                                                 pem->initialScalingVariation.z ) };
    G3DVECTOR accel = { .x = pem->initialAccel.x + initialAccelVariation.x,
                        .y = pem->initialAccel.y + initialAccelVariation.y,
                        .z = pem->initialAccel.z + initialAccelVariation.z };
    G3DVECTOR speed = { .x = pem->initialSpeed.x + initialSpeedVariation.x,
                        .y = pem->initialSpeed.y + initialSpeedVariation.y,
                        .z = pem->initialSpeed.z + initialSpeedVariation.z },
              wspeed;
    G3DVECTOR pos, wpos,
              sca = { .x = pem->initialScaling.x + initialScalingVariation.x, 
                      .y = pem->initialScaling.y + initialScalingVariation.y,
                      .z = pem->initialScaling.z + initialScalingVariation.z },
              rot = { 0.0f, 0.0f, 0.0f, 1.0f };

    getRandomPointOnSquare ( &pem->seg1, 
                             &pem->seg2, 
                              pem->radius, 
                             &pos );

    g3dvector_matrix ( &pos, pem->obj.wmatrix, &wpos );
    g3dvector_matrix ( &speed, pem->TIWMVX, &wspeed );

    g3dparticle_init ( prt,
                       g3dobject_getRandomChild ( pem ),
                      &wpos,
                      &sca,
                      &rot,
                      &accel,
                      &wspeed,
                       startAtFrame,
                       pem->initialTransparency );
}



/******************************************************************************/
static G3DPARTICLEEMITTER *g3dparticleemitter_copy ( G3DPARTICLEEMITTER *pem, 
                                                     uint32_t            id, 
                                                     unsigned char      *name,
                                                     uint64_t            engine_flags ) {
    G3DPARTICLEEMITTER *cpypem = g3dparticleemitter_new ( ((G3DOBJECT*)pem)->id,
                                                          ((G3DOBJECT*)pem)->name );

    /*cpypem->initialAccel        = pem->initialAccel;
    cpypem->initialSpeed        = pem->initialSpeed;
    cpypem->particlesPerFrame   = pem->particlesPerFrame;
    cpypem->particleLifetime    = pem->particleLifetime;
    cpypem->startAtFrame        = pem->startAtFrame;*/

    g3dparticleemitter_reset ( cpypem );


    return cpypem;
}

/******************************************************************************/
static uint32_t g3dparticleemitter_draw ( G3DPARTICLEEMITTER *pem, 
                                          G3DCAMERA          *curcam, 
                                          uint64_t            engine_flags ) {
    if ( pem->maxParticles ) {
        uint32_t i, j;

        /*glPushAttrib( GL_ALL_ATTRIB_BITS );
        glDisable   ( GL_LIGHTING );
        glColor3ub  ( 0xFF, 0xFF, 0xFF );
        glPointSize ( 3.0f );*/

        glPushMatrix ( );
        /*** cancel world transformation ***/
        glMultMatrixd ( pem->obj.iwmatrix );

        for ( i = 0x00; i < pem->particleLifetime; i++ ) {
            G3DPARTICLE *prt = pem->particles + ( pem->maxParticlesPerFrame * i );

            for ( j = 0x00; j < pem->maxParticlesPerFrame; j++ ) {
                if ( prt[j].lifeTime < pem->particleLifetime ) {
                    glPushMatrix ( );

                    glTranslatef ( prt[j].pos.x, 
                                   prt[j].pos.y, 
                                   prt[j].pos.z );

                    glScalef ( prt[j].sca.x, 
                               prt[j].sca.y, 
                               prt[j].sca.z );


                    if ( prt[j].ref ) {
                        if ( prt[j].ref->draw ) {
                            prt[j].ref->draw ( prt[j].ref, 
                                               curcam, 
                                               engine_flags );
                        }
                    }

                    glPopMatrix();
                }
            }
        }

        /*glPopAttrib ( );*/

        glPopMatrix();
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
        int32_t iFrame = ( int32_t ) ( frame - pem->startAtFrame );
        int32_t localFrame = ( iFrame % pem->particleLifetime );
        int32_t i, j;
        double IWMVX[0x10];

        g3dcore_invertMatrix    ( pem->obj.wmatrix, IWMVX );
        g3dcore_transposeMatrix ( IWMVX, pem->TIWMVX );

        
        if ( frame == pem->startAtFrame ) {
            g3dparticleemitter_reset ( pem );
        }

        for ( i = 0x00; i < pem->particleLifetime; i++ ) {
            G3DPARTICLE *prt = pem->particles + ( pem->maxParticlesPerFrame * i );

            for ( j = 0x00; j < pem->maxParticlesPerFrame; j++ ) {
                if ( frame >= prt[j].startAtFrame ) {
                    float ratio = ( float ) prt[j].lifeTime / pem->particleLifetime;

                    prt[j].pos.x += prt[j].speed.x * deltaFrame;
                    prt[j].pos.y += prt[j].speed.y * deltaFrame;
                    prt[j].pos.z += prt[j].speed.z * deltaFrame;

                    prt[j].sca.x  = prt[j].initialScaling.x + ( ( pem->finalScaling.x - prt[j].initialScaling.x ) * ratio );
                    prt[j].sca.y  = prt[j].initialScaling.y + ( ( pem->finalScaling.y - prt[j].initialScaling.y ) * ratio );
                    prt[j].sca.z  = prt[j].initialScaling.z + ( ( pem->finalScaling.z - prt[j].initialScaling.z ) * ratio );

                    prt[j].transparency = pem->initialTransparency + ( ( pem->finalTransparency - pem->initialTransparency ) * ratio );

                    prt[j].lifeTime = ( frame - prt[j].startAtFrame );

                    if ( prt[j].lifeTime == pem->particleLifetime ) {
                        g3dparticleemitter_initParticle ( pem, 
                                                         &prt[j], 
                                                          prt[j].startAtFrame +
                                                          pem->particleLifetime );
                    }
                }

                if ( engine_flags & ONGOINGRENDERING ) {
                    glPushMatrix ( );
                    glLoadIdentity ( );
                    glTranslatef ( prt[j].pos.x, 
                                   prt[j].pos.y, 
                                   prt[j].pos.z );

                    glScalef ( prt[j].sca.x, 
                               prt[j].sca.y, 
                               prt[j].sca.z );

                    glGetDoublev ( GL_MODELVIEW_MATRIX, prt[j].MVX );

                    glPopMatrix();
                }
            }
        }
    }

    pem->oldFrame = frame;
}

/******************************************************************************/
void g3dparticleemitter_reset ( G3DPARTICLEEMITTER *pem ) {
    /*** segments defining the square from which particles are emitted ***/
    pem->seg1.src.x =  pem->radius;
    pem->seg1.src.y =  0.0f;
    pem->seg1.src.z =  pem->radius;

    pem->seg1.dst.x =  pem->radius;
    pem->seg1.dst.y =  0.0f;
    pem->seg1.dst.z = -pem->radius;

    pem->seg2.src.x = -pem->radius;
    pem->seg2.src.y =  0.0f,
    pem->seg2.src.z =  pem->radius;

    pem->seg2.dst.x =  pem->radius;
    pem->seg2.dst.y =  0.0f;
    pem->seg2.dst.z =  pem->radius;

    pem->maxParticlesPerFrame = ceilf ( pem->particlesPerFrame );
    pem->maxParticles = pem->maxParticlesPerFrame * pem->particleLifetime;

    if ( pem->particles ) free ( pem->particles );

    if ( pem->maxParticles ) {
        uint32_t i, j;

        pem->particles = ( G3DPARTICLE * ) calloc ( pem->maxParticles,
                                                    sizeof ( G3DPARTICLE ) );

        for ( i = 0x00; i < pem->particleLifetime; i++ ) {
            G3DPARTICLE *prt = pem->particles + ( pem->maxParticlesPerFrame * i );

            for ( j = 0x00; j < pem->maxParticlesPerFrame; j++ ) {
                g3dparticleemitter_initParticle ( pem, &prt[j], i );
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
    pem->particlesPerFrame   = 10.0f;
    pem->particleLifetime    = 10;
    /*pem->scaleFactor         = 1.0f;*/
    pem->startAtFrame        = 0.0f;


    pem->initialScaling.x = 0.1f;
    pem->initialScaling.y = 0.1f;
    pem->initialScaling.z = 0.1f;

    pem->initialScalingVariation.x = 0.5f;
    pem->initialScalingVariation.y = 0.5f;
    pem->initialScalingVariation.z = 0.5f;

    pem->finalScaling.x = 0.5f;
    pem->finalScaling.y = 0.5f;
    pem->finalScaling.z = 0.5f;

    pem->initialSpeed.x = 0.0f;
    pem->initialSpeed.y = 0.32f;
    pem->initialSpeed.z = 0.0f;

    pem->initialTransparency = 0.0f;
    pem->finalTransparency = 1.0f;

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

