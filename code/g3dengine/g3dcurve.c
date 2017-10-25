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
#include <config.h>
#include <g3dengine/g3dengine.h>

/******************************************************************************/
G3DCURVESEGMENT *g3dcurve_seekSegment ( G3DCURVE *curve, 
                                        G3DCURVEPOINT *p0,
                                        G3DCURVEPOINT *p1 ) {
    LIST *ltmpseg = curve->lseg;

    while ( ltmpseg ) {
        G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;

        if ( ( ( seg->pt[0x00] == p0 ) &&  ( seg->pt[0x01] == p1 ) ) ||
             ( ( seg->pt[0x01] == p0 ) &&  ( seg->pt[0x00] == p1 ) ) ) {
            return seg;
        }

        ltmpseg = ltmpseg->next;
    }

    return NULL;
}

/******************************************************************************/
G3DCURVEPOINT *g3dcurve_getConnectablePoint ( G3DCURVE *curve ) {
    LIST *ltmppt = curve->lpt;

    while ( ltmppt ) {
        G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmppt->data;

        if ( pt->nbseg < 0x02 ) {
            return pt;
        }

        ltmppt = ltmppt->next;
    }

    return NULL;
}

/******************************************************************************/
void g3dcurve_cut ( G3DCURVE *curve, 
                    G3DFACE  *knife, 
                    LIST    **laddedPoints,
                    LIST    **laddedSegments,
                    LIST    **lremovedSegments,
                    uint32_t  engine_flags ) {
    LIST *ltmpseg = curve->lseg;

    while ( ltmpseg ) {
        G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;
        /*
         * copy here because this LIST element 
         * might be deleted from the curve.
         */
        LIST *ltmpsegnext = ltmpseg->next;
        float factor = 1.0f / curve->nbStepsPerSegment;
        G3DVECTOR pone, ptwo;
        uint32_t i;

        memcpy ( &pone, &seg->pt[0x00]->pos, sizeof ( G3DVECTOR ) );

        for( i = 0x01; i <= curve->nbStepsPerSegment; i++ ) {
            G3DVECTOR vout;

            seg->getPoint ( seg, ( factor * i ), &ptwo );

            if ( g3dface_intersect ( knife, &pone, &ptwo, &vout ) ) {
                G3DCURVEPOINT *newPt = g3dcurvepoint_new ( vout.x, 
                                                             vout.y, 
                                                             vout.z );
                G3DCURVESEGMENT *newSeg[0x02];

                newSeg[0x00] = g3dcubicsegment_new ( seg->pt[0x00],
                                                     newPt,
                                                     seg->pt[0x03]->pos.x,
                                                     seg->pt[0x03]->pos.y,
                                                     seg->pt[0x03]->pos.z,
                                                     0.0f,
                                                     0.0f,
                                                     0.0f );

                newSeg[0x01] = g3dcubicsegment_new ( newPt,
                                                     seg->pt[0x01],
                                                     0.0f,
                                                     0.0f,
                                                     0.0f,
                                                     seg->pt[0x02]->pos.x,
                                                     seg->pt[0x02]->pos.y,
                                                     seg->pt[0x02]->pos.z );

                g3dcurve_removeSegment ( curve, seg );
                list_insert ( lremovedSegments, seg );
 
                g3dcurve_addPoint    ( curve, newPt );
                g3dcurve_addSegment ( curve, newSeg[0x00] );
                g3dcurve_addSegment ( curve, newSeg[0x01] );

                g3dcurvepoint_roundCubicSegments ( newPt );

                list_insert ( laddedPoints, newPt );
                list_insert ( laddedSegments, newSeg[0x00] );
                list_insert ( laddedSegments, newSeg[0x01] );
            }

            /* prepare jumping to the next sub segment */
            memcpy ( &pone, &ptwo, sizeof ( G3DVECTOR ) );
        }

        ltmpseg = ltmpsegnext;
    }
}

/******************************************************************************/
void g3dcurve_roundSelectedPoints ( G3DCURVE *curve ) {
    LIST *lselectedPoints = NULL;
    LIST *ltmpver = curve->lpt;

    while ( ltmpver ) {
        G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmpver->data;

        if ( pt->nbseg == 0x02 ) {
            g3dcurvepoint_roundCubicSegments ( pt );
        }

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
void g3dcubicsegment_getHandlePositionFromList ( LIST *lseg, 
                                                 G3DVECTOR (*pos)[0x02] ) {
    LIST *ltmpseg = lseg;
    uint32_t segmentID = 0x00;

    while ( ltmpseg ) {
        G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) ltmpseg->data;
        uint32_t size = sizeof ( G3DVECTOR );

        memcpy ( &pos[segmentID][0x00], &csg->handle[0x00].ver.pos, size );
        memcpy ( &pos[segmentID][0x01], &csg->handle[0x01].ver.pos, size );

        segmentID++;

        ltmpseg = ltmpseg->next;
    }
}

/******************************************************************************/
void g3dcubicsegment_setHandlePositionFromList ( LIST *lseg, 
                                                 G3DVECTOR (*pos)[0x02] ) {
    LIST *ltmpseg = lseg;
    uint32_t segmentID = 0x00;

    while ( ltmpseg ) {
        G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) ltmpseg->data;
        uint32_t size = sizeof ( G3DVECTOR );

        memcpy ( &csg->handle[0x00].ver.pos, &pos[segmentID][0x00], size );
        memcpy ( &csg->handle[0x01].ver.pos, &pos[segmentID][0x01], size );

        segmentID++;

        ltmpseg = ltmpseg->next;
    }
}

/******************************************************************************/
LIST *g3dcurve_getSegmentsFromSelectedPoints ( G3DCURVE *curve ) {
    LIST *lsegments = NULL;
    LIST *ltmpver = curve->lpt;

    while ( ltmpver ) {
        G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmpver->data;

        LIST *ltmpseg = pt->lseg;

        while ( ltmpseg ) {
            G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;

            if ( list_seek ( lsegments, seg ) == NULL ) {
                list_insert ( &lsegments, seg );
            }

            ltmpseg = ltmpseg->next;
        }

        ltmpver = ltmpver->next;
    }

    return lsegments;
}

/******************************************************************************/
LIST *g3dcurve_getSelectedPoints ( G3DCURVE *curve ) {
    LIST *lselectedPoints = NULL;
    LIST *ltmpver = curve->lpt;

    while ( ltmpver ) {
        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

        list_insert ( &lselectedPoints, ver );

        ltmpver = ltmpver->next;
    }

    return lselectedPoints;
}

/******************************************************************************/
void g3dcurve_deletePoints ( G3DCURVE  *curve,
                             LIST      *lremovedPoints,
                             LIST     **lremovedSegments,
                             uint32_t   engine_flags ) {
    LIST *ltmpseg = curve->lseg;
    LIST *ltmpver = lremovedPoints;

    while ( ltmpseg ) {
        G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;
        /*
         * copy here because this LIST element 
         * might be deleted from the curve.
         */
        LIST *ltmpsegnext = ltmpseg->next;
        uint32_t nbRemovedPoints = 0x00;

        nbRemovedPoints += list_seek ( lremovedPoints, seg->pt[0] ) ? 1 : 0;
        nbRemovedPoints += list_seek ( lremovedPoints, seg->pt[1] ) ? 1 : 0;

        if ( nbRemovedPoints > 0x00 ) {
            g3dcurve_removeSegment ( curve, seg ); 

            list_insert ( lremovedSegments, seg );
        }

        ltmpseg = ltmpsegnext;
    }

    while ( ltmpver ) {
        G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmpver->data;

        g3dcurve_removePoint ( curve, pt );

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
void g3dcurve_revert ( G3DCURVE *curve,
                        uint32_t   engine_flags ) {
    LIST *ltmpseg = curve->lseg;

    while ( ltmpseg ) {
        G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) ltmpseg->data;
        G3DVERTEX *swappedVertex = csg->seg.pt[0x00];
        G3DVERTEX *swappedHandle = csg->seg.pt[0x02];

        csg->seg.pt[0x00] = csg->seg.pt[0x01];
        csg->seg.pt[0x01] = swappedVertex;

        csg->seg.pt[0x02] = csg->seg.pt[0x03];
        csg->seg.pt[0x03] = swappedHandle;

        ltmpseg = ltmpseg->next;
    }
}

/******************************************************************************/
void g3dcurvepoint_free ( G3DCURVEPOINT *pt ) {
    list_free ( &pt->lseg, NULL );

    free ( pt );
}

/******************************************************************************/
void g3dcurvepoint_roundCubicSegments ( G3DCURVEPOINT *pt ) {
    if ( pt->nbseg == 0x02 ) {
        G3DVECTOR avg = { .x = 0.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f };
        G3DCUBICHANDLE *han[0x02] = { NULL, NULL };
        LIST *ltmpseg = pt->lseg;
        G3DVECTOR segvec[0x02];
        uint32_t nbseg = 0x00;
        G3DVECTOR nor, hanvec[0x02];
        float len[0x02];

        while ( ltmpseg ) {
            G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) ltmpseg->data;

            segvec[nbseg].x = csg->seg.pt[1]->pos.x - csg->seg.pt[0]->pos.x;
            segvec[nbseg].y = csg->seg.pt[1]->pos.y - csg->seg.pt[0]->pos.y;
            segvec[nbseg].z = csg->seg.pt[1]->pos.z - csg->seg.pt[0]->pos.z;

            if ( csg->seg.pt[1] == pt ) {
                segvec[nbseg].x = -segvec[nbseg].x;
                segvec[nbseg].y = -segvec[nbseg].y;
                segvec[nbseg].z = -segvec[nbseg].z;

                han[nbseg] = csg->seg.pt[2];
            } else {
                han[nbseg] = csg->seg.pt[3];
            }

            g3dvector_normalize ( &segvec[nbseg], &len[nbseg] );

            len[nbseg] *= 0.25f;

            avg.x += segvec[nbseg].x;
            avg.y += segvec[nbseg].y;
            avg.z += segvec[nbseg].z;

            nbseg++;

            ltmpseg = ltmpseg->next;
        }

        /*g3dvector_normalize ( &avg, NULL );*/

        g3dvector_cross ( &segvec[0], &segvec[1], &nor );
        g3dvector_cross ( &nor, &avg, &hanvec[0x00] );

        g3dvector_normalize ( &hanvec[0x00], NULL );

        hanvec[0x01].x = -hanvec[0x00].x;
        hanvec[0x01].y = -hanvec[0x00].y;
        hanvec[0x01].z = -hanvec[0x00].z;


        if ( g3dvector_angle ( &hanvec[0x00], &segvec[0] ) > 1,5708 ) {
            len[0] = -len[0];
        }

        if ( g3dvector_angle ( &hanvec[0x01], &segvec[1] ) > 1,5708 ) {
            len[1] = -len[1];
        }

        han[0]->ver.pos.x = ( hanvec[0x00].x * len[0] );
        han[0]->ver.pos.y = ( hanvec[0x00].y * len[0] );
        han[0]->ver.pos.z = ( hanvec[0x00].z * len[0] );

        han[1]->ver.pos.x = ( hanvec[0x01].x * len[1] );
        han[1]->ver.pos.y = ( hanvec[0x01].y * len[1] );
        han[1]->ver.pos.z = ( hanvec[0x01].z * len[1] );
    }
}

/******************************************************************************/
void g3dcurvepoint_addSegment ( G3DCURVEPOINT   *pt, 
                                 G3DCURVESEGMENT *seg ) {
    list_insert ( &pt->lseg, seg );

    pt->nbseg++;
}

/******************************************************************************/
void g3dcurvepoint_removeSegment ( G3DCURVEPOINT   *pt, 
                                    G3DCURVESEGMENT *seg ) {
    list_remove ( &pt->lseg, seg );

    pt->nbseg--;
}

/******************************************************************************/
G3DCURVEPOINT *g3dcurvepoint_new ( float x, float y, float z ) {
    uint32_t size = sizeof ( G3DCURVEPOINT );
    G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) calloc ( 0x01, size );

    if ( pt == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    pt->ver.pos.x = x;
    pt->ver.pos.y = y;
    pt->ver.pos.z = z;

    return pt;
}

/******************************************************************************/
void g3dcubicsegment_free ( G3DCUBICSEGMENT *seg ) {
    free ( seg );
}

/******************************************************************************/
void g3dcubicsegment_getPoint ( G3DCUBICSEGMENT *csg, 
                                float            factor, /* range 0 - 1 */
                                G3DVECTOR       *pout ) {
    G3DCUBICHANDLE han[0x02];
    G3DCURVEPOINT mid[0x05];
    float incFac = factor;
    float decFac = 1.0f - incFac;

 /* A(1 -3t +3t2 -t3)/6 + B(4 -6t2 +3t3)/6 + C(1 +3t +3t2 -3t3)/6 + D(t3)/6 */

    /*** compute absolute position of handles ***/
    han[0].ver.pos.x = ( csg->seg.pt[0]->pos.x + csg->seg.pt[3]->pos.x );
    han[0].ver.pos.y = ( csg->seg.pt[0]->pos.y + csg->seg.pt[3]->pos.y );
    han[0].ver.pos.z = ( csg->seg.pt[0]->pos.z + csg->seg.pt[3]->pos.z );

    han[1].ver.pos.x = ( csg->seg.pt[1]->pos.x + csg->seg.pt[2]->pos.x );
    han[1].ver.pos.y = ( csg->seg.pt[1]->pos.y + csg->seg.pt[2]->pos.y );
    han[1].ver.pos.z = ( csg->seg.pt[1]->pos.z + csg->seg.pt[2]->pos.z );


    /*** mid points ***/
    mid[0].ver.pos.x = ( csg->seg.pt[0]->pos.x * decFac ) + ( han[0].ver.pos.x * incFac );
    mid[0].ver.pos.y = ( csg->seg.pt[0]->pos.y * decFac ) + ( han[0].ver.pos.y * incFac );
    mid[0].ver.pos.z = ( csg->seg.pt[0]->pos.z * decFac ) + ( han[0].ver.pos.z * incFac );

    mid[1].ver.pos.x = ( csg->seg.pt[1]->pos.x * incFac ) + ( han[1].ver.pos.x * decFac );
    mid[1].ver.pos.y = ( csg->seg.pt[1]->pos.y * incFac ) + ( han[1].ver.pos.y * decFac );
    mid[1].ver.pos.z = ( csg->seg.pt[1]->pos.z * incFac ) + ( han[1].ver.pos.z * decFac );

    mid[2].ver.pos.x = ( han[0].ver.pos.x * decFac ) + ( han[1].ver.pos.x * incFac );
    mid[2].ver.pos.y = ( han[0].ver.pos.y * decFac ) + ( han[1].ver.pos.y * incFac );
    mid[2].ver.pos.z = ( han[0].ver.pos.z * decFac ) + ( han[1].ver.pos.z * incFac );


    /*** mid mid points ***/
    mid[3].ver.pos.x = ( mid[0].ver.pos.x * decFac ) + ( mid[2].ver.pos.x * incFac );
    mid[3].ver.pos.y = ( mid[0].ver.pos.y * decFac ) + ( mid[2].ver.pos.y * incFac );
    mid[3].ver.pos.z = ( mid[0].ver.pos.z * decFac ) + ( mid[2].ver.pos.z * incFac );

    mid[4].ver.pos.x = ( mid[1].ver.pos.x * incFac ) + ( mid[2].ver.pos.x * decFac );
    mid[4].ver.pos.y = ( mid[1].ver.pos.y * incFac ) + ( mid[2].ver.pos.y * decFac );
    mid[4].ver.pos.z = ( mid[1].ver.pos.z * incFac ) + ( mid[2].ver.pos.z * decFac );


    /*** mid mid mid point ***/
    pout->x  = ( mid[3].ver.pos.x * decFac ) + ( mid[4].ver.pos.x * incFac );
    pout->y  = ( mid[3].ver.pos.y * decFac ) + ( mid[4].ver.pos.y * incFac );
    pout->z  = ( mid[3].ver.pos.z * decFac ) + ( mid[4].ver.pos.z * incFac );
}

/******************************************************************************/
void g3dcurvesegment_draw ( G3DCURVESEGMENT *seg,
                             float             from, /* range 0 - 1 */
                             float             to,   /* range 0 - 1 */
                             float             nbSteps,
                             GLUtesselator    *tobj,
                             double          (*coords)[0x03],
                             uint32_t          spline_flags,
                             uint32_t          engine_flags ) {
    float factor = ( to - from ) / nbSteps;
    uint32_t i;

    if ( spline_flags & DRAW_FOR_TESSELLATION ) {
        /*** We skip the first and last points. They are drawn already 
             by points lying on the curve ***/
        for( i = 0x01; i < nbSteps - 1; i++ ) {
            G3DVECTOR pout;

            seg->getPoint ( seg, from + ( factor * i ), &pout );

            coords[i][0x00] = pout.x;
            coords[i][0x01] = pout.y;
            coords[i][0x02] = pout.z;
/*printf("%f %f %f\n", pout.x, pout.y, pout.z);*/
            gluTessVertex ( tobj, coords[i], coords[i] );
            /*glVertex3dv ( coords[i] );*/
        }
    } else {
        G3DVECTOR pone, ptwo;

        memcpy ( &pone, &seg->pt[0x00]->pos, sizeof ( G3DVECTOR ) );

        for( i = 0x01; i <= nbSteps; i++ ) {
            seg->getPoint ( seg, from + ( factor * i ), &ptwo );

            glBegin ( GL_LINES );
            glVertex3f ( pone.x, pone.y, pone.z );
            glVertex3f ( ptwo.x, ptwo.y, ptwo.z  );
            glEnd   ( );

            memcpy ( &pone, &ptwo, sizeof ( G3DVECTOR ) );
        }
    }
}

/******************************************************************************/
G3DCUBICSEGMENT *g3dcubicsegment_new ( G3DCURVEPOINT *pt0,
                                       G3DCURVEPOINT *pt1,
                                       float           hx1,
                                       float           hy1,
                                       float           hz1,
                                       float           hx2,
                                       float           hy2,
                                       float           hz2 ) {
    uint32_t size = sizeof ( G3DCUBICSEGMENT );
    G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) calloc ( 0x01, size );

    csg->seg.getPoint = g3dcubicsegment_getPoint;

    csg->seg.pt[0x00] = pt0;
    csg->seg.pt[0x01] = pt1;

    /*** handles ***/
    csg->handle[0x00].pt = pt0;
    csg->handle[0x01].pt = pt1;

    csg->seg.pt[0x03] = ( G3DCURVEPOINT * ) &csg->handle[0x00];
    csg->seg.pt[0x03]->pos.x  = hx1;
    csg->seg.pt[0x03]->pos.y  = hy1;
    csg->seg.pt[0x03]->pos.z  = hz1;
    csg->seg.pt[0x03]->flags |= VERTEXHANDLE;

    csg->seg.pt[0x02] = ( G3DCURVEPOINT * ) &csg->handle[0x01];
    csg->seg.pt[0x02]->pos.x  = hx2;
    csg->seg.pt[0x02]->pos.y  = hy2;
    csg->seg.pt[0x02]->pos.z  = hz2;
    csg->seg.pt[0x02]->flags |= VERTEXHANDLE;

    return csg;
}

/******************************************************************************/
void g3dquadraticsegment_getPoint ( G3DQUADRATICSEGMENT *qsg, 
                                    float                factor, /* range 0 - 1 */
                                    G3DVECTOR           *pout ) {
    G3DQUADRATICHANDLE *han = &qsg->handle;
    G3DCURVEPOINT mid[0x03];
    float incFac = factor;
    float decFac = 1.0f - incFac;

 /* A(1 -3t +3t2 -t3)/6 + B(4 -6t2 +3t3)/6 + C(1 +3t +3t2 -3t3)/6 + D(t3)/6 */

    /*** mid points ***/
    mid[0].ver.pos.x = ( qsg->seg.pt[0]->pos.x * decFac ) + ( han->ver.pos.x * incFac );
    mid[0].ver.pos.y = ( qsg->seg.pt[0]->pos.y * decFac ) + ( han->ver.pos.y * incFac );
    mid[0].ver.pos.z = ( qsg->seg.pt[0]->pos.z * decFac ) + ( han->ver.pos.z * incFac );

    mid[1].ver.pos.x = ( qsg->seg.pt[1]->pos.x * incFac ) + ( han->ver.pos.x * decFac );
    mid[1].ver.pos.y = ( qsg->seg.pt[1]->pos.y * incFac ) + ( han->ver.pos.y * decFac );
    mid[1].ver.pos.z = ( qsg->seg.pt[1]->pos.z * incFac ) + ( han->ver.pos.z * decFac );

    /*** mid mid mid point ***/
    pout->x  = ( mid[0].ver.pos.x * decFac ) + ( mid[1].ver.pos.x * incFac );
    pout->y  = ( mid[0].ver.pos.y * decFac ) + ( mid[1].ver.pos.y * incFac );
    pout->z  = ( mid[0].ver.pos.z * decFac ) + ( mid[1].ver.pos.z * incFac );
}

/******************************************************************************/
void g3dquadraticsegment_free ( G3DQUADRATICSEGMENT *csg ) {
    free ( csg );
}

/******************************************************************************/
void g3dquadraticsegment_init ( G3DQUADRATICSEGMENT *qsg,
                                G3DCURVEPOINT       *pt0,
                                G3DCURVEPOINT       *pt1,
                                float                hx, 
                                float                hy,
                                float                hz ) {
    qsg->seg.getPoint = g3dquadraticsegment_getPoint;

    qsg->seg.pt[0x00] = pt0;
    qsg->seg.pt[0x01] = pt1;

    /*** handles ***/
    qsg->handle.pt[0x00] = pt0;
    qsg->handle.pt[0x01] = pt1;

    qsg->seg.pt[0x02] = ( G3DCURVEPOINT * ) &qsg->handle;
    qsg->seg.pt[0x02]->pos.x  = hx;
    qsg->seg.pt[0x02]->pos.y  = hy;
    qsg->seg.pt[0x02]->pos.z  = hz;
    qsg->seg.pt[0x02]->flags |= VERTEXHANDLE;
}

/******************************************************************************/
G3DQUADRATICSEGMENT *g3dquadraticsegment_new ( G3DCURVEPOINT *pt0,
                                               G3DCURVEPOINT *pt1,
                                               float           hx, 
                                               float           hy,
                                               float           hz ) {
    uint32_t size = sizeof ( G3DQUADRATICSEGMENT );
    G3DQUADRATICSEGMENT *qsg = ( G3DQUADRATICSEGMENT * ) calloc ( 0x01, size );

    g3dquadraticsegment_init ( qsg, pt0, pt1, hx, hy, hz );

    return qsg;
}

/******************************************************************************/
G3DCURVE *g3dcurve_copy ( G3DCURVE *curve, uint32_t engine_flags ) {
    G3DOBJECT *obj      = ( G3DOBJECT * ) curve;
    G3DCURVE *newSpline = g3dcurve_new ( curve->type, engine_flags );
    G3DLOOKUP  pointLookup;

    if ( curve->nbpt ) {
        LIST *ltmpseg = curve->lseg;
        LIST *ltmpver = curve->lpt;

        g3dlookup_init    ( &pointLookup );
        g3dlookup_realloc ( &pointLookup, curve->nbpt );

        while ( ltmpver ) {
            G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmpver->data;

            G3DCURVEPOINT *newPt = g3dcurvepoint_new ( pt->ver.pos.x, 
                                                         pt->ver.pos.y,
                                                         pt->ver.pos.z );

            g3dlookup_add ( &pointLookup, pt, newPt );

            g3dcurve_addPoint ( newSpline, newPt );

            ltmpver = ltmpver->next;
        }

        while ( ltmpseg ) {
            G3DCURVESEGMENT *oriSeg = ( G3DCURVESEGMENT * ) ltmpseg->data;
            G3DCURVEPOINT   *oriPt0 = oriSeg->pt[0x00],
                             *oriPt1 = oriSeg->pt[0x01];
            G3DCUBICHANDLE   *oriHd0 = oriSeg->pt[0x03]; /* handle 0 */
            G3DCUBICHANDLE   *oriHd1 = oriSeg->pt[0x02]; /* handle 1 */
            G3DCURVEPOINT   *newPt0 = g3dlookup_get( &pointLookup, oriPt0 );
            G3DCURVEPOINT   *newPt1 = g3dlookup_get( &pointLookup, oriPt1 );
            G3DCUBICSEGMENT  *newSeg = g3dcubicsegment_new ( newPt0,
                                                             newPt1,
                                                             oriHd0->ver.pos.x,
                                                             oriHd0->ver.pos.y,
                                                             oriHd0->ver.pos.z,
                                                             oriHd1->ver.pos.x,
                                                             oriHd1->ver.pos.y,
                                                             oriHd1->ver.pos.z );



            g3dcurve_addSegment ( newSpline, newSeg );

            ltmpseg = ltmpseg->next;
        }

        newSpline->nbStepsPerSegment = curve->nbStepsPerSegment;

        g3dlookup_freeTable ( &pointLookup );
    }

    return newSpline;
}

/******************************************************************************/
uint32_t g3dcurve_draw ( G3DOBJECT *obj, 
                         G3DCAMERA *curcam, 
                         uint32_t   engine_flags ) {
    G3DCURVE *curve = ( G3DCURVE * ) obj;
    LIST *ltmpseg = curve->lseg;
    LIST *ltmpver = curve->lpt;

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glLineWidth ( 2.0f );
    glPointSize ( 3.0f );

    glDisable ( GL_LIGHTING );

    if ( ( engine_flags & SELECTMODE ) == 0x00 ) {
        while ( ltmpseg ) {
            G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;

            if ( curve->type == CUBIC ) {
                G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) seg;

                /* draw line to the handle */
                /*if ( ( obj->flags   & OBJECTSELECTED ) && ( engine_flags & VIEWVERTEX ) ) {
                    glColor3ub ( 0xFF, 0x7F, 0x00 );
                    glBegin ( GL_LINES );
                    glVertex3f (  csg->seg.pt[0x00]->pos.x, 
                                  csg->seg.pt[0x00]->pos.y, 
                                  csg->seg.pt[0x00]->pos.z );
                    glVertex3f (  csg->seg.pt[0x00]->pos.x + csg->seg.pt[0x03]->pos.x, 
                                  csg->seg.pt[0x00]->pos.y + csg->seg.pt[0x03]->pos.y, 
                                  csg->seg.pt[0x00]->pos.z + csg->seg.pt[0x03]->pos.z );
                    glVertex3f (  csg->seg.pt[0x01]->pos.x, 
                                  csg->seg.pt[0x01]->pos.y, 
                                  csg->seg.pt[0x01]->pos.z );
                    glVertex3f (  csg->seg.pt[0x01]->pos.x + csg->seg.pt[0x02]->pos.x, 
                                  csg->seg.pt[0x01]->pos.y + csg->seg.pt[0x02]->pos.y, 
                                  csg->seg.pt[0x01]->pos.z + csg->seg.pt[0x02]->pos.z );
                    glEnd   ( );
                }*/

                /*if ( ( engine_flags & SELECTMODE ) == 0x00 ) {
                    glColor3ub ( 0xFF, 0x7F, 0x7F );*/
                    g3dcurvesegment_draw ( seg,
                                            0.0f, 
                                            1.0f,
                                            curve->nbStepsPerSegment,
                                            NULL, /* to tessellation object */
                                            NULL, /* no tessellation data **/
                                            0x00,
                                            engine_flags );
                /*}*/
            }

            ltmpseg = ltmpseg->next;
        }
    }
#ifdef YOUPITRALALA
    if ( ( obj->flags   & OBJECTSELECTED ) && ( engine_flags & VIEWVERTEX ) ) {
        glDisable ( GL_DEPTH_TEST );
        while ( ltmpver ) {
            G3DVERTEX * ver = ( G3DVERTEX * ) ltmpver->data;

            if ( engine_flags & SELECTMODE ) {
                glLoadName ( ( GLint ) ver->id );
            }

            glBegin ( GL_POINTS );
            if ( ver->flags & VERTEXHANDLE ) {
                if ( obj->flags & CUBIC ) {
                    G3DCUBICHANDLE *han = ( G3DCUBICHANDLE * ) ver;

                    if ( ver->flags & VERTEXSELECTED ) {
                        glColor3ub ( 0xFF, 0x00, 0xFF );
                    } else {
                        glColor3ub ( 0x00, 0xFF, 0x00 );
                    }
                    glVertex3f (  han->ver.pos.x + han->pt->pos.x, 
                                  han->ver.pos.y + han->pt->pos.y, 
                                  han->ver.pos.z + han->pt->pos.z );
                }
            } else {
                if ( ver->flags & VERTEXSELECTED ) {
                    glColor3ub ( 0xFF, 0x00, 0x00 );
                } else {
                    glColor3ub ( 0x00, 0x00, 0xFF );
                }

                glVertex3f (  ver->pos.x, ver->pos.y, ver->pos.z );
            }
            glEnd ( );

            ltmpver = ltmpver->next;
        }
    }
#endif
    glPopAttrib ( );
#ifdef YOUPITRALALA
    if ( ( ( obj->flags   & OBJECTSELECTED ) == 0x00 ) ||
         ( ( obj->flags   & OBJECTSELECTED ) &&
           ( engine_flags & SELECTMODE     ) == 0x00 ) ) {
        g3dobject_drawModifiers ( obj, curcam, engine_flags );
    }
#endif
    return 0x00;
}

/******************************************************************************/
void g3dcurve_free ( G3DOBJECT *obj ) {
    G3DCURVE *curve = ( G3DCURVE * ) obj;

    list_free ( &curve->lseg, g3dcubicsegment_free );
    list_free ( &curve->lpt, g3dcurvepoint_free );
}

/******************************************************************************/
void g3dcurve_addPoint ( G3DCURVE      *curve, 
                         G3DCURVEPOINT *pt ) {
    list_insert ( &curve->lpt, pt );

    curve->nbpt++;
}

/******************************************************************************/
void g3dcurve_removePoint ( G3DCURVE      *curve, 
                            G3DCURVEPOINT *pt ) {
    list_remove ( &curve->lpt, pt );

    curve->nbpt--;
}

/******************************************************************************/
void g3dcurve_addSegment ( G3DCURVE        *curve, 
                           G3DCURVESEGMENT *seg ) {
    list_insert ( &curve->lseg, seg );

    if ( curve->type == CUBIC ) {
        G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) seg;

        g3dcurvepoint_addSegment ( csg->seg.pt[0x00], csg );
        g3dcurvepoint_addSegment ( csg->seg.pt[0x01], csg );
    }

    curve->nbseg++;
}

/******************************************************************************/
void g3dcurve_removeSegment ( G3DCURVE        *curve, 
                               G3DCURVESEGMENT *seg ) {
    list_remove ( &curve->lseg, seg );

    if ( curve->type == CUBIC ) {
        G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) seg;

        g3dcurvepoint_removeSegment ( csg->seg.pt[0x00], csg );
        g3dcurvepoint_removeSegment ( csg->seg.pt[0x01], csg );
    }

    curve->nbseg--;
}

/******************************************************************************/
G3DCURVE *g3dcurve_new ( uint32_t type, 
                         uint32_t engine_flags ) {
    G3DCURVE *curve = ( G3DCURVE * ) calloc ( 0x01, sizeof ( G3DCURVE ) );

    if ( curve == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    /** type is CUBIC or QUADRATIC **/
    curve->type = type;
    curve->nbStepsPerSegment = 24;

    return curve;
}

