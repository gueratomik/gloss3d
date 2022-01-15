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
#include <qiss3d/q3d.h>

/******************************************************************************/
static void q3dparticleemitter_import ( Q3DPARTICLEEMITTER *qpem, 
                                        Q3DSCENE           *qsce ) {
    G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) qpem->qobj.obj;
    uint32_t i, j;

    if ( pem->maxParticles ) {
        if ( pem->maxParticlesPerFrame ) {
            qpem->qprt = calloc ( pem->maxParticlesPerFrame * 
                                  pem->particleLifetime, sizeof ( Q3DPARTICLE ) );

            for ( i = 0x00; i < pem->particleLifetime; i++ ) {
                G3DPARTICLE  *prt =  pem->particles + ( pem->maxParticlesPerFrame * i );
                Q3DPARTICLE *qprt = qpem->qprt      + ( pem->maxParticlesPerFrame * i );

                for ( j = 0x00; j < pem->maxParticlesPerFrame; j++ ) {
                    if ( prt[j].ref ) {
                        qprt[j].qref = q3dscene_getByObject ( qsce, prt[j].ref );

                        memcpy ( qprt[j].MVX, 
                                  prt[j].MVX, sizeof ( double ) * 0x10 );

                        g3dcore_invertMatrix    (  prt[j].MVX , qprt[j].IMVX  );
                        g3dcore_transposeMatrix ( qprt[j].IMVX, qprt[j].TIMVX );
                    }
                }
            }
        }
    }
}

/******************************************************************************/
static void q3dparticleemitter_free ( Q3DPARTICLEEMITTER *qpem ) {
    if ( qpem->qprt ) {
        free ( qpem->qprt );
    }
}

/******************************************************************************/
static uint32_t q3dparticleemitter_intersect ( Q3DPARTICLEEMITTER *qpem,
                                               Q3DRAY      *qray,
                                               Q3DSURFACE  *discard,
                                               uint32_t   (*cond)(Q3DOBJECT*,void *),
                                               void        *condData,
                                               float        frame,
                                               uint64_t     query_flags,
                                               uint64_t     render_flags ) {
    Q3DOBJECT *qobjpem = ( Q3DOBJECT * ) qpem;
    G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) q3dobject_getObject ( qobjpem );
    uint32_t hit = 0x00;
    Q3DPARTICLE *hitqprt = NULL;
    uint32_t i, j;
    Q3DRAY locqray, hitqray, wqray;
    
/*
    memcpy ( &wqray, qray, sizeof ( Q3DRAY ) );

    q3dvector3f_matrix ( &qray->src, qpem->qobj.obj->wmatrix, &wqray.src );
    q3dvector3f_matrix ( &qray->dir, qpem->TIWMVX, &wqray.dir );
*/
    q3dcore_buildLocalQRay ( qray, qpem->qobj.obj->wmatrix, &wqray );

    if ( pem->maxParticles ) {
        if ( pem->maxParticlesPerFrame ) {
            for ( i = 0x00; i < pem->particleLifetime; i++ ) {
                G3DPARTICLE  *prt =  pem->particles + ( pem->maxParticlesPerFrame * i );
                Q3DPARTICLE *qprt = qpem->qprt      + ( pem->maxParticlesPerFrame * i );

                for ( j = 0x00; j < pem->maxParticlesPerFrame; j++ ) {
                    if ( prt[j].flags & PARTICLE_ISALIVE ) {
                        q3dcore_buildLocalQRay ( &wqray, qprt[j].IMVX, &locqray );

                        if ( qprt[j].qref ) {
                            if ( qprt[j].qref->intersect ) {
                                hit = qprt[j].qref->intersect ( qprt[j].qref, 
                                                               &locqray,
                                                                discard,
                                                                cond,
                                                                condData,
                                                                frame,
                                                                query_flags,
                                                                render_flags );

                                if ( hit ) { 
                                    hitqprt = &qprt[j];

                                    /*** we copy locqray to another      ***/
                                    /*** structure because locqray is    ***/
                                    /*** resetted for each particle test ***/
                                    memcpy ( &hitqray, 
                                             &locqray, sizeof ( Q3DRAY ) );

                                    hitqray.objectTransparency = prt[j].transparency;

                                    /*** this is need here because locqray is built
                                    from qray ***/
                                    wqray.flags   |= hitqray.flags;
                                    wqray.distance = hitqray.distance;
                                }
                            }
                        }
                    }
                }
            }

            memcpy ( qray, &wqray, sizeof ( Q3DRAY ) );

            if ( hitqprt ) {
                qray->color       = hitqray.color;

                qray->ratio[0x00] = hitqray.ratio[0x00];
                qray->ratio[0x01] = hitqray.ratio[0x01];
                qray->ratio[0x02] = hitqray.ratio[0x02];

                qray->isx.qobj    = hitqray.isx.qobj;
                qray->isx.qsur    = hitqray.isx.qsur;

                q3dvector3f_matrix ( &hitqray.isx.src, hitqprt->MVX  , &wqray.isx.src );
                q3dvector3f_matrix ( &hitqray.isx.dir, hitqprt->TIMVX, &wqray.isx.dir );

                q3dvector3f_normalize ( &qray->isx.dir, NULL );

                qray->objectTransparency = hitqray.objectTransparency;

                /*qray->distance = hitqray.distance;*/


    q3dvector3f_matrix ( &wqray.isx.src, qpem->qobj.obj->iwmatrix, &qray->isx.src );
    q3dvector3f_matrix ( &wqray.isx.dir, qpem->TWMVX, &qray->isx.dir );


                return 0x01;
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
static void q3dparticleemitter_init ( Q3DPARTICLEEMITTER *qpem, 
                                      G3DPARTICLEEMITTER *pem,
                                      uint32_t     id,
                                      uint64_t     object_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) pem;
    double IWMVX[0x10];

    q3dobject_init ( ( Q3DOBJECT * ) qpem,
                     ( G3DOBJECT * ) pem,
                     id,
                     object_flags,
    Q3DFREE_CALLBACK(q3dparticleemitter_free),
Q3DINTERSECT_CALLBACK(q3dparticleemitter_intersect) );

    ((Q3DOBJECT*)qpem)->import = Q3DIMPORT_CALLBACK(q3dparticleemitter_import);


    g3dcore_invertMatrix    ( pem->obj.wmatrix, IWMVX );
    g3dcore_transposeMatrix ( IWMVX, qpem->TIWMVX );

    g3dcore_transposeMatrix ( pem->obj.wmatrix, qpem->TWMVX );
}

/******************************************************************************/
Q3DPARTICLEEMITTER *q3dparticleemitter_new ( G3DPARTICLEEMITTER *pem,
                                             uint32_t     id,
                                             uint64_t     object_flags ) {
    Q3DPARTICLEEMITTER *qpem = ( Q3DPARTICLEEMITTER * ) calloc ( 0x01, sizeof ( Q3DPARTICLEEMITTER ) );

    if ( qpem == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    q3dparticleemitter_init ( qpem, pem, id, object_flags );


    return qpem;
}
