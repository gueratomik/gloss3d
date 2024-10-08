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
#include <g3dengine/vtable/g3dparticleemittervtable.h>

/******************************************************************************/
static G3DPARTICLEEMITTERVTABLE _vtable = { G3DPARTICLEEMITTERVTABLE_DEFAULT };

/******************************************************************************/
typedef struct _PARTICLEEMITTERKEYDATA {
    G3DPARTICLEEMITTER keypem;
} PARTICLEEMITTERKEYDATA;

/******************************************************************************/
static void g3dparticleemitterkey_free ( G3DKEY *key ) {
    PARTICLEEMITTERKEYDATA *pkd = ( PARTICLEEMITTERKEYDATA * ) key->data.ptr;

    free ( pkd );
}

/******************************************************************************/
PARTICLEEMITTERKEYDATA *particleemitterkeydata_new ( ) {
    PARTICLEEMITTERKEYDATA *pkd = calloc ( 0x01, sizeof ( PARTICLEEMITTERKEYDATA ) );
    uint32_t i;

    if ( pkd == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );
    }

    return pkd;
}

/******************************************************************************/
void g3dparticleemitter_default_pose ( G3DPARTICLEEMITTER *pem,
                                       G3DKEY             *key ) {
    if ( key->data.ptr == NULL ) {
        PARTICLEEMITTERKEYDATA *pkd = particleemitterkeydata_new ( );

        key->free = g3dparticleemitterkey_free; /*** callback for freeing memory ***/

        key->data.ptr = pkd;

        memcpy ( &pkd->keypem, pem, sizeof ( G3DPARTICLEEMITTER ) );

        pkd->keypem.obj.flags |= KEYPARTICLEEMITTER;
    }
}

/******************************************************************************/
static inline void getRandomPointOnSquare ( G3DSEGMENT *s1, 
                                            G3DSEGMENT *s2,
                                            float radius,
                                            G3DVECTOR3F  *pnt ) {
    float ratio2 = -1+2*((float)rand())/RAND_MAX;
    float ratio3 = -1+2*((float)rand())/RAND_MAX;

    pnt->x = ratio2 * radius;
    pnt->y = 0.0f;
    pnt->z = ratio3 * radius;
}

/******************************************************************************/
static void g3dparticle_init ( G3DPARTICLE *prt,
                               G3DOBJECT   *ref,
                               G3DVECTOR3F   *pos,
                               G3DVECTOR3F   *sca,
                               G3DVECTOR3F   *rot,
                               G3DVECTOR3F   *accel,
                               G3DVECTOR3F   *speed,
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
    prt->lifeTime = 0.0f;

    prt->transparency = transparency;

    prt->flags &= (~PARTICLE_ISALIVE);
}

/******************************************************************************/
static void g3dparticleemitter_initParticle ( G3DPARTICLEEMITTER *pem,
                                              G3DPARTICLE        *prt,
                                              int32_t             startAtFrame ) {
    int32_t prevFrame = startAtFrame - 0x01;
    float accu      = ( startAtFrame - pem->startAtFrame + 1 ) * pem->particlesPerFrame;
    float prevaccu  = ( prevFrame    - pem->startAtFrame + 1 ) * pem->particlesPerFrame;

/*printf("%d %f %f\n", startAtFrame, accu, prevaccu );*/

    if ( ( int32_t ) accu - ( int32_t ) prevaccu ) {
        float rd = ( float ) rand ( ) / RAND_MAX;

        G3DVECTOR3F initialVarAccel = { .x = ( pem->initialAccel.x * rd *
                                               pem->initialVarAccel.x ),
                                        .y = ( pem->initialAccel.y * rd *
                                               pem->initialVarAccel.y ),
                                        .z = ( pem->initialAccel.z * rd *
                                               pem->initialVarAccel.z ) };
        G3DVECTOR3F initialVarSpeed = { .x = ( pem->initialSpeed.x * rd *
                                               pem->initialVarSpeed.x ),
                                        .y = ( pem->initialSpeed.y * rd *
                                               pem->initialVarSpeed.y ),
                                        .z = ( pem->initialSpeed.z * rd *
                                               pem->initialVarSpeed.z ) };
        G3DVECTOR3F initialVarScaling = { .x = ( pem->initialScaling.x * rd *
                                                 pem->initialVarScaling.x ),
                                          .y = ( pem->initialScaling.y * rd *
                                                 pem->initialVarScaling.y ),
                                          .z = ( pem->initialScaling.z * rd *
                                               pem->initialVarScaling.z ) };
        G3DVECTOR3F accel = { .x = pem->initialAccel.x + initialVarAccel.x,
                              .y = pem->initialAccel.y + initialVarAccel.y,
                              .z = pem->initialAccel.z + initialVarAccel.z };
        G3DVECTOR3F speed = { .x = pem->initialSpeed.x + initialVarSpeed.x,
                              .y = pem->initialSpeed.y + initialVarSpeed.y,
                              .z = pem->initialSpeed.z + initialVarSpeed.z },
                  wspeed;
        G3DVECTOR3F pos, wpos,
                  sca = { .x = pem->initialScaling.x + initialVarScaling.x, 
                          .y = pem->initialScaling.y + initialVarScaling.y,
                          .z = pem->initialScaling.z + initialVarScaling.z },
                  rot = { 0.0f, 0.0f, 0.0f };

        getRandomPointOnSquare ( &pem->seg1, 
                                 &pem->seg2, 
                                  pem->radius, 
                                 &pos );

        g3dvector3f_matrixf ( &pos, pem->obj.worldMatrix, &wpos );
        g3dvector3f_matrixf ( &speed, pem->TIWMVX, &wspeed );

        g3dparticle_init ( prt,
                           g3dobject_getRandomChild ( ( G3DOBJECT * ) pem ),
                          &wpos,
                          &sca,
                          &rot,
                          &accel,
                          &wspeed,
                           startAtFrame,
                           pem->initialTransparency );
    }
}

/******************************************************************************/
static void g3dparticleemitter_animParticle ( G3DPARTICLEEMITTER *pem,
                                              G3DPARTICLE        *prt,
                                              float               deltaFrame ) {
    if ( pem->particleLifetime && pem->sce->fps ) {
        float ratio = ( float ) prt->lifeTime / pem->particleLifetime;
        float deltaSeconds = deltaFrame / pem->sce->fps;

        if ( pem->particleMass ) {
            prt->accel.x = ( pem->gravity.x ) / pem->particleMass;
            prt->accel.y = ( pem->gravity.y ) / pem->particleMass;
            prt->accel.z = ( pem->gravity.z ) / pem->particleMass;
        }

        prt->speed.x += ( prt->accel.x * deltaSeconds );
        prt->speed.y += ( prt->accel.y * deltaSeconds );
        prt->speed.z += ( prt->accel.z * deltaSeconds );

        prt->pos.x += ( prt->speed.x * deltaSeconds );
        prt->pos.y += ( prt->speed.y * deltaSeconds );
        prt->pos.z += ( prt->speed.z * deltaSeconds );

        prt->sca.x  = prt->initialScaling.x +
                  ( ( pem->finalScaling.x - 
                      prt->initialScaling.x ) * ratio );

        prt->sca.y  = prt->initialScaling.y +
                  ( ( pem->finalScaling.y - 
                      prt->initialScaling.y ) * ratio );

        prt->sca.z  = prt->initialScaling.z +
                  ( ( pem->finalScaling.z - 
                      prt->initialScaling.z ) * ratio );

        prt->transparency = pem->initialTransparency + 
                        ( ( pem->finalTransparency -
                            pem->initialTransparency ) * ratio );

        prt->lifeTime += ( deltaFrame );

        prt->flags |= PARTICLE_ISALIVE;

        if ( ( int32_t ) prt->lifeTime > pem->particleLifetime ) {
            g3dparticleemitter_initParticle ( pem,
                                              prt,
                                              prt->startAtFrame + ( int32_t ) prt->lifeTime );
        }
    }
}

/******************************************************************************/
static void g3dparticleemitter_copySettings ( G3DPARTICLEEMITTER *dstpem, 
                                              G3DPARTICLEEMITTER *srcpem ) {
    dstpem->gravity.x = srcpem->gravity.x;
    dstpem->gravity.y = srcpem->gravity.y;
    dstpem->gravity.z = srcpem->gravity.z;

    dstpem->initialAccel        = srcpem->initialAccel;
    dstpem->initialSpeed        = srcpem->initialSpeed;
    dstpem->initialScaling      = srcpem->initialScaling;
    dstpem->initialRotation     = srcpem->initialRotation;
    dstpem->initialTransparency = srcpem->initialTransparency;

    dstpem->initialVarAngle        = srcpem->initialVarAngle;
    dstpem->initialVarAccel        = srcpem->initialVarAccel;
    dstpem->initialVarSpeed        = srcpem->initialVarSpeed;
    dstpem->initialVarScaling      = srcpem->initialVarScaling;
    dstpem->initialVarRotation     = srcpem->initialVarRotation;
    dstpem->initialVarTransparency = srcpem->initialVarTransparency;

    dstpem->finalAccel        = srcpem->finalAccel;
    dstpem->finalSpeed        = srcpem->finalSpeed;
    dstpem->finalRotation     = srcpem->finalRotation;
    dstpem->finalScaling      = srcpem->finalScaling;
    dstpem->finalTransparency = srcpem->finalTransparency;

    dstpem->particleLifetime          = srcpem->particleLifetime;
    dstpem->particleLifetimeVariation = srcpem->particleLifetimeVariation;
    dstpem->particleMass              = srcpem->particleMass;

    dstpem->maxPreviewsPerFrame  = srcpem->maxPreviewsPerFrame; 
    dstpem->particlesPerFrame    = srcpem->particlesPerFrame;

    dstpem->startAtFrame = srcpem->startAtFrame;
    dstpem->endAtFrame   = srcpem->endAtFrame;

    dstpem->radius = srcpem->radius;
    dstpem->type   = srcpem->type;

    g3dparticleemitter_reset ( dstpem );
}

/******************************************************************************/
G3DPARTICLEEMITTER *g3dparticleemitter_default_copy ( G3DPARTICLEEMITTER *pem, 
                                                      uint32_t            id, 
                                                      const char         *name,
                                                      uint64_t            engineFlags ) {
    uint32_t cpyID = ((G3DOBJECT*)pem)->id; /*** Does not need to be unique ***/
    char *cpyname = ((G3DOBJECT*)pem)->name;

    G3DPARTICLEEMITTER *cpypem = g3dparticleemitter_new ( cpyID,
                                                          cpyname,
                                                          pem->sce );


    g3dparticleemitter_copySettings ( cpypem, pem );


    return cpypem;
}

/******************************************************************************/
uint32_t g3dparticleemitter_default_draw ( G3DPARTICLEEMITTER *pem, 
                                           G3DCAMERA          *curcam, 
                                           G3DENGINE          *engine, 
                                           uint64_t            engineFlags ) {
    if ( pem->maxParticles ) {
        uint32_t i, j;

        g3dengine_pushModelMatrix ( engine );

        /** cancel world transformation, particles are in world corrdinates **/
        g3dengine_multModelMatrixf ( pem->obj.iwmatrix );

        glPushAttrib( GL_ALL_ATTRIB_BITS );
        glPointSize ( 3.0f );

        for ( i = 0x00; i < pem->particleLifetime; i++ ) {
            G3DPARTICLE *prt = pem->particles + ( pem->maxParticlesPerFrame * i );

            for ( j = 0x00; j < pem->maxParticlesPerFrame; j++ ) {
                if ( prt[j].flags & PARTICLE_ISALIVE ) {
                    if ( prt[j].ref ) {
                        if ( pem->obj.flags & DISPLAYPARTICLES ) {
                            g3dengine_pushModelMatrix ( engine );

                            g3dengine_translateModelMatrixf ( engine,
                                                              prt[j].pos.x, 
                                                              prt[j].pos.y, 
                                                              prt[j].pos.z );

                            g3dengine_scaleModelMatrixf ( engine,
                                                          prt[j].sca.x, 
                                                          prt[j].sca.y, 
                                                          prt[j].sca.z );

                            if ( prt[j].ref->vtable->draw ) {
                                prt[j].ref->vtable->draw ( prt[j].ref, 
                                                           curcam, 
                                                           engine,
                                                           engineFlags & (~MODEMASK) );
                            }
                        }
/*
 else {
                            SHADERVERTEX vertex = { .pos = { prt[j].pos.x,
                                                             prt[j].pos.y,
                                                             prt[j].pos.z },
                                                    .col = { 1.0f,
                                                             1.0f,
                                                             1.0f,
                                                             1.0f } };

                            g3dengine_drawPoint ( engine,
                                                  vertex,
                                                  0x00,
                                                  engineFlags );

                        }
*/
                    }
                }
            }
        }


        glPopAttrib ( );

        g3dengine_popModelMatrix ( engine );
    }


    return 0x00;
}

/******************************************************************************/
void g3dparticleemitter_default_free ( G3DPARTICLEEMITTER *pem ) {
    if ( pem->particles ) {
        free ( pem->particles );
   }
}

/******************************************************************************/
void g3dparticleemitter_default_anim ( G3DPARTICLEEMITTER *pem, 
                                       float               frame, 
                                       uint64_t            engineFlags ) {

    G3DKEY *prevKey = NULL,
           *nextKey = NULL,
           *currKey = NULL;
    float deltaFrame;

    if ( g3dobject_isActive ( ( G3DOBJECT * ) pem ) == 0x00 ) return;

    frame = g3dobject_getKeys ( &pem->obj, 
                                frame, 
                               &currKey,
                               &prevKey, 
                               &nextKey, 
                                KEYDATA,
                                0x00 );

    if ( currKey ) {
        PARTICLEEMITTERKEYDATA *pkd = currKey->data.ptr;

        g3dparticleemitter_copySettings ( pem, &pkd->keypem );
    }

    deltaFrame = frame - pem->oldFrame;

    /*printf("%s : %f %f %f\n", __func__, frame, pem->oldFrame, deltaFrame );*/

    if ( pem->maxParticles ) {
        int32_t iFrame = ( int32_t ) ( frame - pem->startAtFrame );
        int32_t localFrame = ( iFrame % ( int32_t ) pem->particleLifetime );
        int32_t i, j;
        float IWMVX[0x10];

        g3dcore_invertMatrixf    ( pem->obj.worldMatrix, IWMVX );
        g3dcore_transposeMatrixf ( IWMVX, pem->TIWMVX );

        
        if ( frame == pem->startAtFrame ) {
            g3dparticleemitter_reset ( pem );
        }

        for ( i = 0x00; i < pem->particleLifetime; i++ ) {
            G3DPARTICLE *prt = pem->particles + ( pem->maxParticlesPerFrame * i );

            for ( j = 0x00; j < pem->maxParticlesPerFrame; j++ ) {
                if ( frame == pem->startAtFrame ) {
                    g3dparticleemitter_initParticle ( pem, 
                                                     &prt[j], 
                                                      pem->startAtFrame + i );
                }

                if ( ( frame > prt[j].startAtFrame ) &&
                     ( prt[j].startAtFrame < pem->endAtFrame ) ) {

                    g3dparticleemitter_animParticle ( pem,
                                                     &prt[j],
                                                      deltaFrame );

                    if ( engineFlags & ONGOINGRENDERING ) {
                        g3dcore_identityMatrixf( prt[j].MVX );
                        g3dcore_translateMatrixf( prt[j].MVX,
                                                  prt[j].pos.x, 
                                                  prt[j].pos.y, 
                                                  prt[j].pos.z );
                        g3dcore_scaleMatrixf( prt[j].MVX,
                                              prt[j].sca.x, 
                                              prt[j].sca.y, 
                                              prt[j].sca.z );
                    }
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

    if ( pem->maxParticles ) {
        uint32_t memarea = ( pem->maxParticles * sizeof ( G3DPARTICLE ) );
        uint32_t i, j;

        pem->particles = ( G3DPARTICLE * ) realloc ( pem->particles, memarea );

        memset ( pem->particles, 0x00, memarea );

        for ( i = 0x00; i < pem->particleLifetime; i++ ) {
            G3DPARTICLE *prt = pem->particles + ( pem->maxParticlesPerFrame * i );

            for ( j = 0x00; j < pem->maxParticlesPerFrame; j++ ) {
                g3dparticleemitter_initParticle ( pem, &prt[j], INT_MAX );
            }
        }
    }
}

/******************************************************************************/
void g3dparticleemitter_init ( G3DPARTICLEEMITTER       *pem, 
                               uint32_t                  id, 
                               char                     *name,
                               G3DSCENE                 *sce,
                               G3DPARTICLEEMITTERVTABLE *vtable ) {
    g3dobject_init ( G3DOBJECTCAST(pem), 
                     G3DPARTICLEEMITTERTYPE,
                     id, 
                     name, 
                     0x00,
                     vtable ? G3DOBJECTVTABLECAST(vtable) 
                            : G3DOBJECTVTABLECAST(&_vtable) );

    /*pem->orientation = INSTANCEYZ;*/

    /*mes->dump           = g3dmesh_default_dump;*/

    /*pem->initialAcceleration = 0.0f;*/

    pem->particlesPerFrame = 10.0f;
    pem->particleLifetime  = 10;

    pem->gravity.x =  0.0f;
    pem->gravity.y = -9.87f;
    pem->gravity.z =  0.0f;

    pem->particleMass = 1.0f;

    /*pem->scaleFactor         = 1.0f;*/

    pem->startAtFrame      = 0x00;
    pem->endAtFrame        = 100;

    pem->initialScaling.x  = 1.0f;
    pem->initialScaling.y  = 1.0f;
    pem->initialScaling.z  = 1.0f;

    pem->finalScaling.x    = 1.0f;
    pem->finalScaling.y    = 1.0f;
    pem->finalScaling.z    = 1.0f;

    pem->initialSpeed.y    = 1.0f;
    pem->initialVarSpeed.y = 0.1f;

    pem->radius = 0.5f;

    pem->sce = sce;
}

/******************************************************************************/
/*** Note: particle emitter needs the G3DSCENE as arg to retrieve the FPS ***/
G3DPARTICLEEMITTER *g3dparticleemitter_new ( uint32_t  id, 
                                             char     *name,
                                             G3DSCENE *sce ) {
    G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) calloc ( 0x01, sizeof ( G3DPARTICLEEMITTER ) );

    if ( pem == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    g3dparticleemitter_init ( pem, id, name, sce, NULL );

    g3dparticleemitter_reset ( pem );

    return pem;
}

