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
uint32_t q3dtriangle_hasTextureSlot ( Q3DTRIANGLE *qtri, 
                                      uint32_t texSlotBit  ) {
    LIST *ltmpfacgrp = qtri->lfacgrp;

    while ( ltmpfacgrp ) {
        G3DFACEGROUP *facgrp = ( G3DFACEGROUP * ) ltmpfacgrp->data;

        if ( facgrp->textureSlots & texSlotBit ) return 0x01;

        ltmpfacgrp = ltmpfacgrp->next;
    }

    return 0x00;
}

/******************************************************************************/
/*******************************************************************************
www.soe.ucsc.edu/classes/cmps160/Fall10/resources/barycentricInterpolation.pdf
*******************************************************************************/
/******************************************************************************/
uint32_t q3dtriangle_pointIn ( Q3DTRIANGLE  *qtri, 
                               Q3DVERTEX    *qver, 
                               Q3DVECTOR3F  *qpnt,
                               float        *RAT0,
                               float        *RAT1,
                               float        *RAT2 ) {
    uint32_t qverID0 = qtri->qverID[0x00],
             qverID1 = qtri->qverID[0x01],
             qverID2 = qtri->qverID[0x02];
    Q3DVECTOR3F V0P = { .x = ( qpnt->x - qver[qverID0].pos.x ),
                        .y = ( qpnt->y - qver[qverID0].pos.y ),
                        .z = ( qpnt->z - qver[qverID0].pos.z ) },
                V1P = { .x = ( qpnt->x - qver[qverID1].pos.x ),
                        .y = ( qpnt->y - qver[qverID1].pos.y ),
                        .z = ( qpnt->z - qver[qverID1].pos.z ) },
                V2P = { .x = ( qpnt->x - qver[qverID2].pos.x ),
                        .y = ( qpnt->y - qver[qverID2].pos.y ),
                        .z = ( qpnt->z - qver[qverID2].pos.z ) },
               V0V1 = { .x = ( qver[qverID1].pos.x - qver[qverID0].pos.x ),
                        .y = ( qver[qverID1].pos.y - qver[qverID0].pos.y ),
                        .z = ( qver[qverID1].pos.z - qver[qverID0].pos.z ) },
               V1V2 = { .x = ( qver[qverID2].pos.x - qver[qverID1].pos.x ),
                        .y = ( qver[qverID2].pos.y - qver[qverID1].pos.y ),
                        .z = ( qver[qverID2].pos.z - qver[qverID1].pos.z ) },
               V2V0 = { .x = ( qver[qverID0].pos.x - qver[qverID2].pos.x ),
                        .y = ( qver[qverID0].pos.y - qver[qverID2].pos.y ),
                        .z = ( qver[qverID0].pos.z - qver[qverID2].pos.z ) };
    Q3DVECTOR3F DOT0, DOT1, DOT2, DOTF;
    double LENF, LEN0, LEN1, LEN2;

    Q3DVECTOR3F_CROSS ( &V0V1, &V1P, &DOT2 );
    Q3DVECTOR3F_CROSS ( &V1V2, &V2P, &DOT0 );
    Q3DVECTOR3F_CROSS ( &V2V0, &V0P, &DOT1 );

    /*** rfc->surface contains the value of the face surface ***/
    LENF = qtri->surface;

    LEN0 = Q3DVECTOR3F_LENGTH ( &DOT0 );
    LEN1 = Q3DVECTOR3F_LENGTH ( &DOT1 );
    LEN2 = Q3DVECTOR3F_LENGTH ( &DOT2 );

    /*if ( rfc->flags & RFACEMIRRORED ) {
        g3ddoublevector_invert ( &DOT0 );
        g3ddoublevector_invert ( &DOT1 );
        g3ddoublevector_invert ( &DOT2 );
    }*/

    /*if ( ( LEN0 + LEN1 + LEN2 ) <= ( LENF * 1.001f ) *//*LENF*//* ) {*/
    if ( ( Q3DVECTOR3F_SCALAR ( &DOT0, &qtri->nor ) >= 0.0f ) && 
         ( Q3DVECTOR3F_SCALAR ( &DOT1, &qtri->nor ) >= 0.0f ) && 
         ( Q3DVECTOR3F_SCALAR ( &DOT2, &qtri->nor ) >= 0.0f ) ) {
        /*** return subtriangles surface ratio if needed ***/
            if ( RAT0 ) (*RAT0) = (float)( LEN0 / LENF );
            if ( RAT1 ) (*RAT1) = (float)( LEN1 / LENF );
            if ( RAT2 ) (*RAT2) = (float)( LEN2 / LENF );

            return 0x01;
        /*}*/
    }

    return 0x00;
}

/******************************************************************************/
/*** From http://www.siggraph.org/education/materials/HyperGraph/raytrace/  ***/
/*** rayplane_intersection.htm                                              ***/
/******************************************************************************/
uint32_t q3dtriangle_intersect ( Q3DTRIANGLE *qtri,
                                 Q3DVERTEX   *qver, 
                                 Q3DRAY      *qray,
                                 uint64_t     query_flags ) {
    double vo = ( qtri->nor.x * qray->src.x ) +
                ( qtri->nor.y * qray->src.y ) +
                ( qtri->nor.z * qray->src.z ) + qtri->nor.w,
           vd = ( qtri->nor.x * qray->dir.x ) + 
                ( qtri->nor.y * qray->dir.y ) +
                ( qtri->nor.z * qray->dir.z );
    float invert = 1.0f;
    uint32_t ret;
    double t;

    if ( vd == 0.0f ) return 0x00;

    t = - ( vo / vd );

    if ( t > 0.0f ) {
        Q3DVECTOR3F qpnt = { .x = qray->src.x + ( qray->dir.x * t ),
                             .y = qray->src.y + ( qray->dir.y * t ),
                             .z = qray->src.z + ( qray->dir.z * t ) },
                    qdir = { qpnt.x - qray->src.x,
                             qpnt.y - qray->src.y,
                             qpnt.z - qray->src.z };

        float distance = /*q3dvector3f_length ( &qdir )*/t;

        /* when we render backface as well */
        if ( ( vd > 0.0f ) && ( query_flags & RAYQUERYIGNOREBACKFACE ) ) {
            invert = -1.0f;
        }

        if ( ( vd < 0.0f ) || ( query_flags & RAYQUERYIGNOREBACKFACE ) ) {
            if ( distance < qray->distance ) {
                float RAT0, RAT1, RAT2;

                if ( q3dtriangle_pointIn ( qtri,
                                           qver,
                                          &qpnt,
                                          &RAT0, 
                                          &RAT1,
                                          &RAT2 ) ) {
                    uint32_t qverID[0x03] = { qtri->qverID[0x00],
                                              qtri->qverID[0x01],
                                              qtri->qverID[0x02] };

                    qray->isx.src.x = qpnt.x;
                    qray->isx.src.y = qpnt.y;
                    qray->isx.src.z = qpnt.z;

                    if ( qtri->flags & TRIANGLEFLAT ) {
                        qray->isx.dir.x = qtri->nor.x;
                        qray->isx.dir.y = qtri->nor.y;
                        qray->isx.dir.z = qtri->nor.z;
                    } else {
                        qray->isx.dir.x = ( qver[qverID[0]].nor.x * RAT0 ) +
                                          ( qver[qverID[1]].nor.x * RAT1 ) +
                                          ( qver[qverID[2]].nor.x * RAT2 );
                        qray->isx.dir.y = ( qver[qverID[0]].nor.y * RAT0 ) +
                                          ( qver[qverID[1]].nor.y * RAT1 ) +
                                          ( qver[qverID[2]].nor.y * RAT2 );
                        qray->isx.dir.z = ( qver[qverID[0]].nor.z * RAT0 ) +
                                          ( qver[qverID[1]].nor.z * RAT1 ) +
                                          ( qver[qverID[2]].nor.z * RAT2 );
                    }

                    /*** backface ***/
                    qray->isx.dir.x *= invert;
                    qray->isx.dir.y *= invert;
                    qray->isx.dir.z *= invert;

                    qray->isx.qsur = ( Q3DSURFACE * ) qtri;

                    memcpy ( &qray->isx.locsrc, 
                             &qray->isx.src, sizeof ( Q3DVECTOR3F ) );

                    /*** save Interpolation ratios for UVs ***/
                    qray->ratio[0x00] = RAT0;
                    qray->ratio[0x01] = RAT1;
                    qray->ratio[0x02] = RAT2;

                    qray->distance = distance;

                    /*** intersection occured, let's remember it ***/
                    qray->flags |= Q3DRAY_HAS_HIT_BIT;
  
                    return 0x01;
                }
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
void q3dtriangle_init ( Q3DTRIANGLE *qtri,
                        Q3DVERTEX   *qver,
                        uint32_t     qverID0,
                        uint32_t     qverID1,
                        uint32_t     qverID2 ) {
    Q3DVECTOR3F v0v1 = { .x = qver[qverID1].pos.x - qver[qverID0].pos.x,
                         .y = qver[qverID1].pos.y - qver[qverID0].pos.y,
                         .z = qver[qverID1].pos.z - qver[qverID0].pos.z },
                v1v2 = { .x = qver[qverID2].pos.x - qver[qverID1].pos.x,
                         .y = qver[qverID2].pos.y - qver[qverID1].pos.y,
                         .z = qver[qverID2].pos.z - qver[qverID1].pos.z };

    qtri->qverID[0x00] = qverID0;
    qtri->qverID[0x01] = qverID1;
    qtri->qverID[0x02] = qverID2;

    q3dvector3f_cross ( &v0v1, &v1v2, ( Q3DVECTOR3F * ) &qtri->nor );

    q3dvector3f_normalize ( ( Q3DVECTOR3F * ) &qtri->nor, &qtri->surface );

    qtri->nor.w = - ( ( qtri->nor.x * qver[qverID0].pos.x ) + 
                      ( qtri->nor.y * qver[qverID0].pos.y ) + 
                      ( qtri->nor.z * qver[qverID0].pos.z ) );
}

/******************************************************************************/
Q3DTRIANGLE *q3dtriangle_new ( Q3DVERTEX *qver,
                               uint32_t   qverID0, 
                               uint32_t   qverID1, 
                               uint32_t   qverID2 ) {
    uint32_t structSize = sizeof ( Q3DTRIANGLE );
    Q3DTRIANGLE *qtri = ( Q3DTRIANGLE * ) calloc ( 0x01, structSize );

    if ( qtri == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );
    }

    q3dtriangle_init ( qtri, qver, qverID0, qverID1, qverID2 );

    return qtri;
}
