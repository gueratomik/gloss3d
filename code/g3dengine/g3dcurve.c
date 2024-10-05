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
void g3dcurvepoint_getAveragePositionFromList ( LIST *lpt, G3DVECTOR3F *pos ) {
    uint32_t nb = 0x00;
    LIST *ltmppt = lpt;

    pos->x = 0.0f;
    pos->y = 0.0f;
    pos->z = 0.0f;

    while ( ltmppt ) {
        G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmppt->data;

        pos->x += pt->pos.x;
        pos->y += pt->pos.y;
        pos->z += pt->pos.z;

        nb++;

        ltmppt = ltmppt->next;
    }

    if ( nb ) {
        pos->x /= nb;
        pos->y /= nb;
        pos->z /= nb;
    }
}

/******************************************************************************/
G3DCURVESEGMENT *g3dcurvepoint_seekSegment ( G3DCURVEPOINT *p0, 
                                             G3DCURVEPOINT *p1 ) {
    LIST *ltmpseg = p0->lseg;

    while ( ltmpseg ) {
        G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;

        if ( ( ( seg->pt[P0IDX] == p0 ) &&  ( seg->pt[P1IDX] == p1 ) ) ||
             ( ( seg->pt[P1IDX] == p0 ) &&  ( seg->pt[P0IDX] == p1 ) ) ) {
            return seg;
        }

        ltmpseg = ltmpseg->next;
    }

    return NULL;
}

/******************************************************************************/
G3DCURVESEGMENT *g3dcurve_seekSegment ( G3DCURVE *curve, 
                                        G3DCURVEPOINT *p0,
                                        G3DCURVEPOINT *p1 ) {
    LIST *ltmpseg = curve->lseg;

    while ( ltmpseg ) {
        G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;

        if ( ( ( seg->pt[P0IDX] == p0 ) &&  ( seg->pt[P1IDX] == p1 ) ) ||
             ( ( seg->pt[P1IDX] == p0 ) &&  ( seg->pt[P0IDX] == p1 ) ) ) {
            return seg;
        }

        ltmpseg = ltmpseg->next;
    }

    return NULL;
}

/******************************************************************************/
G3DCURVESEGMENT *g3dcurve_seekSegmentByID ( G3DCURVE *curve, 
                                            uint32_t  id0,
                                            uint32_t  id1 ) {
    LIST *ltmpseg = curve->lseg;

    while ( ltmpseg ) {
        G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;

        if ( ( ( seg->pt[P0IDX]->id == id0 ) &&  ( seg->pt[P1IDX]->id == id1 ) ) ||
             ( ( seg->pt[P1IDX]->id == id0 ) &&  ( seg->pt[P0IDX]->id == id1 ) ) ) {
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
                    uint64_t engine_flags ) {
    LIST *ltmpseg = curve->lseg;

    while ( ltmpseg ) {
        G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;
        /*
         * copy here because this LIST element 
         * might be deleted from the curve.
         */
        LIST *ltmpsegnext = ltmpseg->next;
        float factor = 1.0f / SEGMENTDIV;
        G3DVECTOR3F pone, ptwo;
        uint32_t i;

        memcpy ( &pone, &seg->pt[P0IDX]->pos, sizeof ( G3DVECTOR3F ) );

        for( i = 0x01; i <= SEGMENTDIV; i++ ) {
            G3DVECTOR3F vout;

            seg->getPoint ( seg, ( factor * i ), &ptwo );

            if ( g3dface_intersect ( knife, &pone, &ptwo, &vout ) ) {
                G3DCURVEPOINT *newPt = g3dcurvepoint_new ( vout.x, 
                                                           vout.y, 
                                                           vout.z );
                g3dcurve_insertPointWithinSegment ( curve, 
                                                    newPt,
                                                    seg,
                                                    laddedSegments,
                                                    lremovedSegments );
            }

            /* prepare jumping to the next sub segment */
            memcpy ( &pone, &ptwo, sizeof ( G3DVECTOR3F ) );
        }

        ltmpseg = ltmpsegnext;
    }
}

/******************************************************************************/
void g3dcurve_roundSelectedPoints ( G3DCURVE *curve ) {
    LIST *ltmppt = curve->lselpt;

    while ( ltmppt ) {
        G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmppt->data;

        if ( pt->nbseg == 0x02 ) {
            g3dcurvepoint_roundCubicSegments ( pt );
        }

        ltmppt = ltmppt->next;
    }
}

/******************************************************************************/
void g3dcubicsegment_getHandlePositionFromList ( LIST *lseg, 
                                                 G3DVECTOR3F (*pos)[0x02] ) {
    LIST *ltmpseg = lseg;
    uint32_t segmentID = 0x00;

    while ( ltmpseg ) {
        G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) ltmpseg->data;
        uint32_t size = sizeof ( G3DVECTOR3F );

        memcpy ( &pos[segmentID][0x00], &csg->handle[0x00].pos, size );
        memcpy ( &pos[segmentID][0x01], &csg->handle[0x01].pos, size );

        segmentID++;

        ltmpseg = ltmpseg->next;
    }
}

/******************************************************************************/
void g3dcubicsegment_setHandlePositionFromList ( LIST *lseg, 
                                                 G3DVECTOR3F (*pos)[0x02] ) {
    LIST *ltmpseg = lseg;
    uint32_t segmentID = 0x00;

    while ( ltmpseg ) {
        G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) ltmpseg->data;
        uint32_t size = sizeof ( G3DVECTOR3F );

        memcpy ( &csg->handle[0x00].pos, &pos[segmentID][0x00], size );
        memcpy ( &csg->handle[0x01].pos, &pos[segmentID][0x01], size );

        segmentID++;

        ltmpseg = ltmpseg->next;
    }
}

/******************************************************************************/
LIST *g3dcurve_getSegmentsFromSelectedPoints ( G3DCURVE *curve ) {
    LIST *lsegments = NULL;
    LIST *ltmppt = curve->lselpt;

    while ( ltmppt ) {
        G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmppt->data;

        LIST *ltmpseg = pt->lseg;

        while ( ltmpseg ) {
            G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;

            if ( list_seek ( lsegments, seg ) == NULL ) {
                list_insert ( &lsegments, seg );
            }

            ltmpseg = ltmpseg->next;
        }

        ltmppt = ltmppt->next;
    }

    return lsegments;
}

/******************************************************************************/
LIST *g3dcurve_getSelectedPoints ( G3DCURVE *curve ) {
    return list_copy ( curve->lselpt );
}

/******************************************************************************/
void g3dcurve_deletePoints ( G3DCURVE  *curve,
                             LIST      *lremovedPoints,
                             LIST     **lremovedSegments,
                             uint64_t engine_flags ) {
    LIST *ltmpver = lremovedPoints;

    while ( ltmpver ) {
        G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmpver->data;

        g3dcurve_removePoint ( curve, pt, lremovedSegments );

        ltmpver = ltmpver->next;
    }
}

/******************************************************************************/
void g3dcurve_revert ( G3DCURVE *curve,
                        uint64_t engine_flags ) {
    LIST *ltmpseg = curve->lseg;

    while ( ltmpseg ) {
        G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) ltmpseg->data;
        G3DCURVEPOINT *swappedPoint  = csg->seg.pt[P0IDX];
        G3DCURVEPOINT *swappedHandle = csg->seg.pt[P0HANDLEIDX];

        csg->seg.pt[P0IDX] = csg->seg.pt[P1IDX];
        csg->seg.pt[P1IDX] = swappedPoint;

        csg->seg.pt[P0HANDLEIDX] = csg->seg.pt[P1HANDLEIDX];
        csg->seg.pt[P1HANDLEIDX] = swappedHandle;

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
        G3DVECTOR3F avg = { .x = 0.0f, .y = 0.0f, .z = 0.0f };
        G3DCUBICHANDLE *han[0x02] = { NULL, NULL };
        LIST *ltmpseg = pt->lseg;
        G3DVECTOR3F segvec[0x02];
        uint32_t segCount = 0x00;
        G3DVECTOR3F nor, hanvec;
        float len[0x02];

        while ( ltmpseg ) {
            G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) ltmpseg->data;

            if ( csg->seg.pt[P0IDX] == pt ) {
                segvec[segCount].x = csg->seg.pt[P1IDX]->pos.x - csg->seg.pt[P0IDX]->pos.x;
                segvec[segCount].y = csg->seg.pt[P1IDX]->pos.y - csg->seg.pt[P0IDX]->pos.y;
                segvec[segCount].z = csg->seg.pt[P1IDX]->pos.z - csg->seg.pt[P0IDX]->pos.z;

                han[segCount] = csg->seg.pt[P0HANDLEIDX];
            } else {
                segvec[segCount].x = csg->seg.pt[P0IDX]->pos.x - csg->seg.pt[P1IDX]->pos.x;
                segvec[segCount].y = csg->seg.pt[P0IDX]->pos.y - csg->seg.pt[P1IDX]->pos.y;
                segvec[segCount].z = csg->seg.pt[P0IDX]->pos.z - csg->seg.pt[P1IDX]->pos.z;

                han[segCount] = csg->seg.pt[P1HANDLEIDX];
            }

            len[segCount] = g3dvector3f_normalize ( &segvec[segCount] ) * 0.25f;

            avg.x += segvec[segCount].x;
            avg.y += segvec[segCount].y;
            avg.z += segvec[segCount].z;

            segCount++;

            ltmpseg = ltmpseg->next;
        }

        g3dvector3f_cross ( &segvec[0], &segvec[1], &nor    );
        g3dvector3f_cross ( &nor      , &avg      , &hanvec );

        g3dvector3f_normalize ( &hanvec );

        if ( g3dvector3f_scalar ( &segvec[0x00], &hanvec ) < 0.0f ) {
            len[0] = -len[0];
        }

        if ( g3dvector3f_scalar ( &segvec[0x01], &hanvec ) < 0.0f ) {
            len[1] = -len[1];
        }

        han[0]->pos.x = ( hanvec.x * len[0] );
        han[0]->pos.y = ( hanvec.y * len[0] );
        han[0]->pos.z = ( hanvec.z * len[0] );

        han[1]->pos.x = ( hanvec.x * len[1] );
        han[1]->pos.y = ( hanvec.y * len[1] );
        han[1]->pos.z = ( hanvec.z * len[1] );
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

    pt->flags |= CURVEPOINTISPOINT;
    pt->pos.x = x;
    pt->pos.y = y;
    pt->pos.z = z;

    return pt;
}

/******************************************************************************/
void g3dcubicsegment_free ( G3DCUBICSEGMENT *seg ) {
    free ( seg );
}

/******************************************************************************/
void g3dcubicsegment_getPoint ( G3DCUBICSEGMENT *csg, 
                                float            t, /* range 0 - 1 */
                                G3DVECTOR3F     *pout ) {
    float t2 = (   t  * t );
    float t3 = (   t2 * t );
    float u  = ( 1.0f - t );
    float u2 = (   u  * u );
    float u3 = (   u2 * u );

    pout->x = ( u3 * csg->seg.pt[P0IDX]->pos.x ) +
              ( ( 3 * u2 * t  ) * csg->seg.pt[P0HANDLEIDX]->pos.x ) +
              ( ( 3 * u  * t2 ) * csg->seg.pt[P1HANDLEIDX]->pos.x ) +
              ( t3 * csg->seg.pt[P1IDX]->pos.x );

    pout->y = ( u3 * csg->seg.pt[P0IDX]->pos.y ) +
              ( ( 3 * u2 * t  ) * csg->seg.pt[P0HANDLEIDX]->pos.y ) +
              ( ( 3 * u  * t2 ) * csg->seg.pt[P1HANDLEIDX]->pos.y ) +
              ( t3 * csg->seg.pt[P1IDX]->pos.y );
}

/******************************************************************************/
void g3dcurvesegment_pick ( G3DCURVESEGMENT *seg,
                            float             from, /* range 0 - 1 */
                            float             to,   /* range 0 - 1 */
                            float             nbSteps,
                            uint64_t engine_flags ) {
    float factor = ( to - from ) / nbSteps;
    G3DVECTOR3F pone, ptwo;
    uint32_t i;

    memcpy ( &pone, &seg->pt[P0IDX]->pos, sizeof ( G3DVECTOR3F ) );

    for( i = 0x01; i <= nbSteps; i++ ) {
        seg->getPoint ( seg, from + ( factor * i ), &ptwo );

        g3dpick_drawLine ( pone.x, pone.y, pone.z,
                           ptwo.x, ptwo.y, ptwo.z );

        memcpy ( &pone, &ptwo, sizeof ( G3DVECTOR3F ) );
    }
}

/******************************************************************************/
void g3dcurvesegment_draw ( G3DCURVESEGMENT *seg,
                            float             from, /* range 0 - 1 */
                            float             to,   /* range 0 - 1 */
                            float             nbSteps,
                            GLUtesselator    *tobj,
                            double          (*coords)[0x03],
                            uint32_t          splineFlags,
                            uint64_t          engineFlags ) {
    float factor = ( to - from ) / nbSteps;
    uint32_t i;

    if ( splineFlags & DRAW_FOR_TESSELLATION ) {
        /*** We skip the first and last points. They are drawn already 
             by points lying on the curve ***/
        for( i = 0x01; i < nbSteps - 1; i++ ) {
            G3DVECTOR3F pout;

            seg->getPoint ( seg, from + ( factor * i ), &pout );

            coords[i][0x00] = pout.x;
            coords[i][0x01] = pout.y;
            coords[i][0x02] = pout.z;

            gluTessVertex ( tobj, coords[i], coords[i] );
            /*glVertex3dv ( coords[i] );*/
        }
    } else {
        G3DVECTOR3F pone, ptwo;
#ifdef need_refactor
        memcpy ( &pone, &seg->pt[P0IDX]->pos, sizeof ( G3DVECTOR3F ) );

        for( i = 0x01; i <= nbSteps; i++ ) {
            seg->getPoint ( seg, from + ( factor * i ), &ptwo );

            glBegin ( GL_LINES );
            glVertex3f ( pone.x, pone.y, pone.z );
            glVertex3f ( ptwo.x, ptwo.y, ptwo.z  );
            glEnd   ( );

            memcpy ( &pone, &ptwo, sizeof ( G3DVECTOR3F ) );
        }
#endif
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

    csg->seg.getPoint = GETPOINT_CALLBACK(g3dcubicsegment_getPoint);

    csg->seg.pt[P0IDX] = pt0;
    csg->seg.pt[P1IDX] = pt1;

    /*** handles ***/
    /*csg->handle[0x00].pt = pt0;
    csg->handle[0x01].pt = pt1;*/

    csg->seg.pt[P0HANDLEIDX] = ( G3DCURVEPOINT * ) &csg->handle[0x00];
    csg->seg.pt[P0HANDLEIDX]->pos.x  = hx1;
    csg->seg.pt[P0HANDLEIDX]->pos.y  = hy1;
    csg->seg.pt[P0HANDLEIDX]->pos.z  = hz1;

    csg->seg.pt[P1HANDLEIDX] = ( G3DCURVEPOINT * ) &csg->handle[0x01];
    csg->seg.pt[P1HANDLEIDX]->pos.x  = hx2;
    csg->seg.pt[P1HANDLEIDX]->pos.y  = hy2;
    csg->seg.pt[P1HANDLEIDX]->pos.z  = hz2;

    csg->seg.pt[P0HANDLEIDX]->flags |= CURVEPOINTISHANDLE;
    csg->seg.pt[P1HANDLEIDX]->flags |= CURVEPOINTISHANDLE;

    return csg;
}

/******************************************************************************/
void g3dquadraticsegment_getPoint ( G3DQUADRATICSEGMENT *qsg, 
                                    float                factor, /* range 0 - 1 */
                                    G3DVECTOR3F           *pout ) {
    G3DQUADRATICHANDLE *han = &qsg->handle;
    G3DCURVEPOINT mid[0x03];
    float incFac = factor;
    float decFac = 1.0f - incFac;
#ifdef need_refactor
 /* A(1 -3t +3t2 -t3)/6 + B(4 -6t2 +3t3)/6 + C(1 +3t +3t2 -3t3)/6 + D(t3)/6 */

    /*** mid points ***/
    mid[0].pos.x = ( qsg->seg.pt[P0IDX]->pos.x * decFac ) + ( han->pos.x * incFac );
    mid[0].pos.y = ( qsg->seg.pt[P0IDX]->pos.y * decFac ) + ( han->pos.y * incFac );
    mid[0].pos.z = ( qsg->seg.pt[P0IDX]->pos.z * decFac ) + ( han->pos.z * incFac );

    mid[1].pos.x = ( qsg->seg.pt[P1IDX]->pos.x * incFac ) + ( han->pos.x * decFac );
    mid[1].pos.y = ( qsg->seg.pt[P1IDX]->pos.y * incFac ) + ( han->pos.y * decFac );
    mid[1].pos.z = ( qsg->seg.pt[P1IDX]->pos.z * incFac ) + ( han->pos.z * decFac );

    /*** mid mid mid point ***/
    pout->x  = ( mid[0].pos.x * decFac ) + ( mid[1].pos.x * incFac );
    pout->y  = ( mid[0].pos.y * decFac ) + ( mid[1].pos.y * incFac );
    pout->z  = ( mid[0].pos.z * decFac ) + ( mid[1].pos.z * incFac );
#endif
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
    qsg->seg.getPoint = GETPOINT_CALLBACK(g3dquadraticsegment_getPoint);

    qsg->seg.pt[P0IDX] = pt0;
    qsg->seg.pt[P1IDX] = pt1;

    qsg->handle.flags |= CURVEPOINTISPOINT;

    /*** handles ***/
    /*qsg->handle.pt[0x00] = pt0;
    qsg->handle.pt[0x01] = pt1;*/

    qsg->seg.pt[0x02] = ( G3DCURVEPOINT * ) &qsg->handle;
    qsg->seg.pt[0x02]->pos.x  = hx;
    qsg->seg.pt[0x02]->pos.y  = hy;
    qsg->seg.pt[0x02]->pos.z  = hz;
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
G3DCURVE *g3dcurve_copy ( G3DCURVE *curve, uint64_t engine_flags ) {
    G3DOBJECT *obj      = ( G3DOBJECT * ) curve;
    G3DCURVE *newSpline = g3dcurve_new ( curve->type );
    G3DLOOKUP  pointLookup;

    if ( curve->nbpt ) {
        LIST *ltmpseg = curve->lseg;
        LIST *ltmpver = curve->lpt;

        g3dlookup_init    ( &pointLookup );
        g3dlookup_realloc ( &pointLookup, curve->nbpt );

        while ( ltmpver ) {
            G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmpver->data;

            G3DCURVEPOINT *newPt = g3dcurvepoint_new ( pt->pos.x, 
                                                       pt->pos.y,
                                                       pt->pos.z );

            g3dlookup_add ( &pointLookup, pt, newPt );

            g3dcurve_addPoint ( newSpline, newPt );

            ltmpver = ltmpver->next;
        }

        while ( ltmpseg ) {
            G3DCURVESEGMENT *oriSeg = ( G3DCURVESEGMENT * ) ltmpseg->data;
            G3DCURVEPOINT   *oriPt0 = oriSeg->pt[P0IDX],
                            *oriPt1 = oriSeg->pt[P1IDX];
            G3DCUBICHANDLE  *oriHd0 = oriSeg->pt[P0HANDLEIDX]; /* handle 0 */
            G3DCUBICHANDLE  *oriHd1 = oriSeg->pt[P1HANDLEIDX]; /* handle 1 */
            G3DCURVEPOINT   *newPt0 = g3dlookup_get( &pointLookup, oriPt0 );
            G3DCURVEPOINT   *newPt1 = g3dlookup_get( &pointLookup, oriPt1 );
            G3DCUBICSEGMENT *newSeg = g3dcubicsegment_new ( newPt0,
                                                            newPt1,
                                                            oriHd0->pos.x,
                                                            oriHd0->pos.y,
                                                            oriHd0->pos.z,
                                                            oriHd1->pos.x,
                                                            oriHd1->pos.y,
                                                            oriHd1->pos.z );



            g3dcurve_addSegment ( newSpline, ( G3DCURVESEGMENT * ) newSeg );

            ltmpseg = ltmpseg->next;
        }

        g3dlookup_freeTable ( &pointLookup );
    }

    return newSpline;
}

/******************************************************************************/
void g3dcurve_pickHandles ( G3DCURVE *curve, uint64_t engine_flags ) {
    LIST *ltmpseg = curve->lseg;

    while ( ltmpseg ) {
        G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;

        if ( curve->type == CUBIC ) {
            G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) seg;
            G3DCURVEPOINT *pt0 = csg->seg.pt[P0IDX],
                          *pt1 = csg->seg.pt[P1IDX],
                          *han0 = csg->seg.pt[P0HANDLEIDX],
                          *han1 = csg->seg.pt[P1HANDLEIDX];

            g3dpick_setName ( ( uint64_t ) han0 );
            g3dpick_drawPoint ( han0->pos.x,
                                han0->pos.y,
                                han0->pos.z );

            g3dpick_setName ( ( uint64_t ) han1 );
            g3dpick_drawPoint ( han1->pos.x,
                                han1->pos.y,
                                han1->pos.z );
        }

        ltmpseg = ltmpseg->next;
    }
}

/******************************************************************************/
static void _drawHandles ( G3DCURVE  *curve,
                           G3DENGINE *engine,
                           uint64_t   engine_flags ) {
    LIST *ltmpseg = curve->lseg;

    while ( ltmpseg ) {
        G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;

        if ( curve->type == CUBIC ) {
            G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) seg;
            G3DCURVEPOINT *pt0 = csg->seg.pt[P0IDX],
                          *pt1 = csg->seg.pt[P1IDX],
                          *han0 = csg->seg.pt[P0HANDLEIDX],
                          *han1 = csg->seg.pt[P1HANDLEIDX];
            SHADERVERTEX handle0vertices[0x02] = { 0 };
            SHADERVERTEX handle1vertices[0x02] = { 0 };

            handle0vertices[0x00].pos   = pt0->pos;
            handle0vertices[0x00].col.g = 1.0f;
            handle0vertices[0x01].pos   = han0->pos;
            handle0vertices[0x01].col.g = 1.0f;

            handle1vertices[0x00].pos   = pt1->pos;
            handle1vertices[0x00].col.g = 1.0f;
            handle1vertices[0x01].pos   = han1->pos;
            handle1vertices[0x01].col.g = 1.0f;

            g3dengine_drawPoint ( engine,
                                 &handle0vertices[0x01],
                                  0,
                                  engine_flags );

            g3dengine_drawPoint ( engine,
                                 &handle1vertices[0x01],
                                  0,
                                  engine_flags );

            handle0vertices[0x00].col.r =
            handle0vertices[0x00].col.b =
            handle0vertices[0x01].col.r =
            handle0vertices[0x01].col.b = 1.0f;

            handle1vertices[0x00].col.r =
            handle1vertices[0x00].col.b =
            handle1vertices[0x01].col.r =
            handle1vertices[0x01].col.b = 1.0f;

            g3dengine_drawLine ( engine,
                                 handle0vertices,
                                 0,
                                 engine_flags );

            g3dengine_drawLine ( engine,
                                 handle1vertices,
                                 0,
                                 engine_flags );
        }

        ltmpseg = ltmpseg->next;
    }
}

/******************************************************************************/
void g3dcurve_drawHandles ( G3DCURVE  *curve,
                            G3DOBJECT *owner,
                            G3DCAMERA *curcam,
                            G3DENGINE *engine,
                            uint64_t   engine_flags ) {
    int mvpMatrixLocation = glGetUniformLocation( engine->coloredShaderProgram,
                                                  "mvpMatrix" );
    float mvp[0x10];
    float mod[0x10];

    g3dcore_multMatrixf( curcam->obj.inverseWorldMatrix,
                         owner->worldMatrix,
                         mod );

    /*** the matrix by which vertices coords are transformed ***/
    g3dcore_multMatrixf( curcam->pmatrix, mod, mvp );

    glUseProgram( engine->coloredShaderProgram );

    glUniformMatrix4fv( mvpMatrixLocation, 1, GL_FALSE, mvp );

    _drawHandles( curve, engine, engine_flags );

    glUseProgram( 0 );
}

/******************************************************************************/
void g3dcurve_pickPoints ( G3DCURVE *curve, uint64_t engine_flags ) {
    LIST *ltmppt  = curve->lpt;

    while ( ltmppt ) {
        G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmppt->data;

        g3dpick_setName ( ( uint64_t ) pt );
        g3dpick_drawPoint ( pt->pos.x, pt->pos.y, pt->pos.z );

        ltmppt = ltmppt->next;
    }
}

/******************************************************************************/
static void _drawPoints ( G3DCURVE *curve,
                          G3DENGINE *engine,
                          uint64_t   engine_flags ) {
    LIST *ltmppt  = curve->lpt;

    while ( ltmppt ) {
        G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmppt->data;
        SHADERVERTEX vertex = { 0 };

        vertex.pos.x = pt->pos.x;
        vertex.pos.y = pt->pos.y;
        vertex.pos.z = pt->pos.z;

        if ( pt->flags & CURVEPOINTSELECTED ) {
            vertex.col.r = 1.0f;
        } else {
            vertex.col.b = 1.0f;
        }

        g3dengine_drawPoint ( engine, &vertex, 0, engine_flags );

        ltmppt = ltmppt->next;
    }
}

/******************************************************************************/
void g3dcurve_drawPoints ( G3DCURVE  *curve,
                           G3DOBJECT *owner,
                           G3DCAMERA *cam,
                           G3DENGINE *engine,
                           uint64_t   engine_flags ) {
    int mvpMatrixLocation = glGetUniformLocation( engine->coloredShaderProgram,
                                                  "mvpMatrix" );
    float mvp[0x10];
    float mod[0x10];

    glPointSize ( 3.0f );

    g3dcore_multMatrixf( cam->obj.inverseWorldMatrix,
                         owner->worldMatrix,
                         mod );

    /*** the matrix by which vertices coords are transformed ***/
    g3dcore_multMatrixf( cam->pmatrix, mod, mvp );

    glUseProgram( engine->coloredShaderProgram );

    glUniformMatrix4fv( mvpMatrixLocation, 1, GL_FALSE, mvp );

    _drawPoints( curve, engine, engine_flags );

    glUseProgram( 0 );
}

/******************************************************************************/
void g3dcurve_pickSegments ( G3DCURVE *curve, uint64_t engine_flags ) {
    LIST *ltmpseg = curve->lseg;

    while ( ltmpseg ) {
        G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;

        if ( curve->type == CUBIC ) {
            G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) seg;

            g3dpick_setName ( ( uint64_t ) csg );
            g3dcurvesegment_pick ( seg,
                                   0.0f, 
                                   1.0f,
                                   SEGMENTDIV,
                                   engine_flags );
        }

        ltmpseg = ltmpseg->next;
    }
}

/******************************************************************************/
static void _drawSegments( G3DCURVE  *curve,
                           G3DENGINE *engine,
                           uint64_t   engine_flags ) {
    float step = ( 1.0f / ( SEGMENTDIV - 1 ) );
    float t = 0.0f;
    LIST *ltmpseg = curve->lseg;
    SHADERVERTEX vertices[SEGMENTDIV] = { 0 };

    while ( ltmpseg ) {
        G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;

        if ( curve->type == CUBIC ) {
            G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) seg;
            G3DVECTOR3F pt;
            uint32_t i;

            for( i = 0x00; i < SEGMENTDIV; i++ ) {
                float sanT = ( t < 1.0f ) ? t : 1.0f;

                seg->getPoint ( seg, sanT, &vertices[i].pos );

                vertices[i].col.r = 1.0f;
                vertices[i].col.g = 0.5f;
                vertices[i].col.b = 0.0f;

                t += step;
            }
        }

        ltmpseg = ltmpseg->next;
    }

    g3dengine_drawSegment ( engine, vertices, 0, engine_flags );
}

/******************************************************************************/
void g3dcurve_drawSegments ( G3DCURVE  *curve, 
                             G3DOBJECT *owner,
                             G3DCAMERA *curcam,
                             G3DENGINE *engine,
                             uint64_t   engine_flags ) {
    int mvpMatrixLocation = glGetUniformLocation( engine->coloredShaderProgram,
                                                  "mvpMatrix" );
    float mvp[0x10];
    float mod[0x10];

    g3dcore_multMatrixf( curcam->obj.inverseWorldMatrix,
                         owner->worldMatrix,
                         mod );

    /*** the matrix by which vertices coords are transformed ***/
    g3dcore_multMatrixf( curcam->pmatrix, mod, mvp );

    glUseProgram( engine->coloredShaderProgram );

    glUniformMatrix4fv( mvpMatrixLocation, 1, GL_FALSE, mvp );

    _drawSegments( curve, engine, engine_flags );

    glUseProgram( 0 );
}

/******************************************************************************/
void g3dcurve_pick ( G3DCURVE *curve, uint64_t engine_flags ) {
    LIST *ltmpseg = curve->lseg;

    while ( ltmpseg ) {
        G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;

        if ( curve->type == CUBIC ) {
            G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) seg;

            g3dcurvesegment_pick ( seg,
                                   0.0f, 
                                   1.0f,
                                   SEGMENTDIV,
                                   engine_flags );
        }

        ltmpseg = ltmpseg->next;
    }
}

/******************************************************************************/
/*** currently same code as g3dcurve_drawSegments but there will be a ***/
/*** difference when we will code segments selection (color changing) ***/
/*** in g3dcurve_drawSegments ***/
void g3dcurve_draw ( G3DCURVE  *curve,
                     G3DOBJECT *owner,
                     G3DCAMERA *curcam, 
                     G3DENGINE *engine, 
                     uint64_t   engine_flags ) {
    int mvpMatrixLocation = glGetUniformLocation( engine->coloredShaderProgram,
                                                  "mvpMatrix" );
    float mvp[0x10];
    float mod[0x10];

    g3dcore_multMatrixf( curcam->obj.inverseWorldMatrix,
                         owner->worldMatrix,
                         mod );

    /*** the matrix by which vertices coords are transformed ***/
    g3dcore_multMatrixf( curcam->pmatrix, mod, mvp );

    glUseProgram( engine->coloredShaderProgram );

    glUniformMatrix4fv( mvpMatrixLocation, 1, GL_FALSE, mvp );

    if( engine_flags & VIEWPATH ) {
        _drawSegments( curve, engine, engine_flags );
        _drawHandles ( curve, engine, engine_flags );
        _drawPoints  ( curve, engine, engine_flags );
    }

    glUseProgram( 0 );
}

/******************************************************************************/
/*uint32_t g3dcurve_draw ( G3DOBJECT *obj, 
                         G3DCAMERA *curcam, 
                         uint64_t engine_flags ) {
    G3DCURVE *curve = ( G3DCURVE * ) obj;

    if ( engine_flags & SELECTMODE ) {
        g3dcurve_drawSegments ( curve, engine_flags );
    } else {
        glPushAttrib ( GL_ALL_ATTRIB_BITS );
        glLineWidth ( 2.0f );
        glPointSize ( 3.0f );

        glDisable ( GL_LIGHTING );

        g3dcurve_drawSegments ( curve, engine_flags );

        glPopAttrib ( );
    }

    return 0x00;
}*/

/******************************************************************************/
void g3dcurve_free ( G3DCURVE *curve ) {

    list_free ( &curve->lseg, LIST_FUNCDATA(g3dcubicsegment_free) );
    list_free ( &curve->lpt, LIST_FUNCDATA(g3dcurvepoint_free) );
}

/******************************************************************************/
void g3dcurve_unselectPoint ( G3DCURVE *curve, G3DCURVEPOINT *pt ) {
    list_remove ( &curve->lselpt, pt );

    pt->flags &= ~(CURVEPOINTSELECTED);

    curve->avgSelPtPos.x = ( curve->avgSelPtPos.x * curve->nbselpt ) - pt->pos.x;
    curve->avgSelPtPos.y = ( curve->avgSelPtPos.y * curve->nbselpt ) - pt->pos.y;
    curve->avgSelPtPos.z = ( curve->avgSelPtPos.z * curve->nbselpt ) - pt->pos.z;

    curve->nbselpt--;

    if ( curve->nbselpt ) {
        curve->avgSelPtPos.x /= curve->nbselpt;
        curve->avgSelPtPos.y /= curve->nbselpt;
        curve->avgSelPtPos.z /= curve->nbselpt;
    }
}

/******************************************************************************/
void g3dcurve_getSurroundingPoints ( G3DCURVE       *curve, 
                                     G3DCURVEPOINT  *pt, 
                                     G3DCURVEPOINT **prevPt, 
                                     G3DCURVEPOINT **nextPt ) {
    LIST *ltmppt = curve->lpt;

    while ( ltmppt ) {
        G3DCURVEPOINT *curPt = ( G3DCURVEPOINT * ) ltmppt->data;

        if ( curPt == pt ) {
            if (*prevPt) *prevPt = ( ltmppt->prev ) ? ltmppt->prev->data : NULL;
            if (*nextPt) *nextPt = ( ltmppt->next ) ? ltmppt->next->data : NULL;
        }

        ltmppt = ltmppt->next;
    }
}

/******************************************************************************/
void g3dcurve_insertPointWithinSegment ( G3DCURVE        *curve, 
                                         G3DCURVEPOINT   *pt,
                                         G3DCURVESEGMENT *seg,
                                         LIST           **laddedSegments,
                                         LIST           **lremovedSegments ) {
    G3DCURVESEGMENT *newSeg[0x02];

    switch ( curve->type ) {
        case CUBIC : {
            newSeg[0x00] = ( G3DCURVESEGMENT * ) 
                           g3dcubicsegment_new ( seg->pt[P0IDX],
                                                 pt,
                                                 seg->pt[P0HANDLEIDX]->pos.x,
                                                 seg->pt[P0HANDLEIDX]->pos.y,
                                                 seg->pt[P0HANDLEIDX]->pos.z,
                                                 pt->pos.x,
                                                 pt->pos.y,
                                                 pt->pos.z );

            newSeg[0x01] = ( G3DCURVESEGMENT * ) 
                           g3dcubicsegment_new ( pt,
                                                 seg->pt[P1IDX],
                                                 pt->pos.x,
                                                 pt->pos.y,
                                                 pt->pos.z,
                                                 seg->pt[P1HANDLEIDX]->pos.x,
                                                 seg->pt[P1HANDLEIDX]->pos.y,
                                                 seg->pt[P1HANDLEIDX]->pos.z );

            g3dcurve_removeSegment ( curve, seg );

            g3dcurve_addPoint ( curve, pt );

            g3dcurve_addSegment ( curve, newSeg[0x00] );
            g3dcurve_addSegment ( curve, newSeg[0x01] );

            g3dcurvepoint_roundCubicSegments ( pt );

            if ( laddedSegments   ) list_insert ( laddedSegments  , newSeg[0x00] );
            if ( laddedSegments   ) list_insert ( laddedSegments  , newSeg[0x01] );
            if ( lremovedSegments ) list_insert ( lremovedSegments, seg          );
        } break;

        default :
        break;
    }
}

/******************************************************************************/
void g3dcurve_selectPoint ( G3DCURVE *curve, G3DCURVEPOINT *pt ) {
    list_insert ( &curve->lselpt, pt ); 

    pt->flags |= CURVEPOINTSELECTED;

    curve->avgSelPtPos.x = ( curve->avgSelPtPos.x * curve->nbselpt ) + pt->pos.x;
    curve->avgSelPtPos.y = ( curve->avgSelPtPos.y * curve->nbselpt ) + pt->pos.y;
    curve->avgSelPtPos.z = ( curve->avgSelPtPos.z * curve->nbselpt ) + pt->pos.z;

    curve->nbselpt++;

    curve->avgSelPtPos.x /= curve->nbselpt;
    curve->avgSelPtPos.y /= curve->nbselpt;
    curve->avgSelPtPos.z /= curve->nbselpt;
}

/******************************************************************************/
void g3dcurvepoint_unsetSelected ( G3DCURVEPOINT *pt ) {
     pt->flags &= ~(CURVEPOINTSELECTED);
}

/******************************************************************************/
void g3dcurve_unselectAllPoints ( G3DCURVE *curve ) {
    list_free ( &curve->lselpt, (void(*)(void*)) g3dcurvepoint_unsetSelected );

    curve->nbselpt = 0x00;

    curve->avgSelPtPos.x = 0.0f;
    curve->avgSelPtPos.y = 0.0f;
    curve->avgSelPtPos.z = 0.0f;
}

/******************************************************************************/
void g3dcurve_addPoint ( G3DCURVE      *curve, 
                         G3DCURVEPOINT *pt ) {
    list_insert ( &curve->lpt, pt );

    curve->nbpt++;
}

/******************************************************************************/
void g3dcurve_addSelectedPoint ( G3DCURVE      *curve, 
                                 G3DCURVEPOINT *pt ) {
    g3dcurve_addPoint ( curve, pt );
    g3dcurve_selectPoint ( curve, pt );
}

/******************************************************************************/
void g3dcurve_removePoint ( G3DCURVE      *curve, 
                            G3DCURVEPOINT *pt,
                            LIST         **lremovedSegments ) {
    LIST *ltmpseg = pt->lseg;

    while ( ltmpseg ) {
        G3DCURVESEGMENT *seg = ( G3DCURVESEGMENT * ) ltmpseg->data;
        LIST *ltmpsegnext = ltmpseg->next;

        g3dcurve_removeSegment ( curve, seg );

        if ( lremovedSegments ) list_insert ( lremovedSegments, seg );

        ltmpseg = ltmpsegnext;
    }

    list_remove ( &curve->lpt, pt );

    curve->nbpt--;
}

/******************************************************************************/
void g3dcurve_addSegment ( G3DCURVE        *curve, 
                           G3DCURVESEGMENT *seg ) {
    list_insert ( &curve->lseg, seg );

    if ( curve->type == CUBIC ) {
        G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) seg;

        g3dcurvepoint_addSegment ( csg->seg.pt[P0IDX], ( G3DCURVESEGMENT * ) csg );
        g3dcurvepoint_addSegment ( csg->seg.pt[P1IDX], ( G3DCURVESEGMENT * ) csg );
    }

    curve->nbseg++;
}

/******************************************************************************/
void g3dcurve_removeSegment ( G3DCURVE        *curve, 
                               G3DCURVESEGMENT *seg ) {
    list_remove ( &curve->lseg, seg );

    if ( curve->type == CUBIC ) {
        G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) seg;

        g3dcurvepoint_removeSegment ( csg->seg.pt[P0IDX], ( G3DCURVESEGMENT * ) csg );
        g3dcurvepoint_removeSegment ( csg->seg.pt[P1IDX], ( G3DCURVESEGMENT * ) csg );
    }

    curve->nbseg--;
}

/******************************************************************************/
G3DCURVE *g3dcurve_new ( uint32_t type ) {
    G3DCURVE *curve = ( G3DCURVE * ) calloc ( 0x01, sizeof ( G3DCURVE ) );

    if ( curve == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    /** type is CUBIC or QUADRATIC **/
    curve->type = type;

    return curve;
}

