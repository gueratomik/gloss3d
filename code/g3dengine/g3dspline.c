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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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
void g3dsplinepoint_free ( G3DSPLINEPOINT *pt ) {
    free ( pt );
}

/******************************************************************************/
void g3dsplinepoint_roundCubicSegments ( G3DSPLINEPOINT *pt ) {
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
void g3dsplinepoint_addSegment ( G3DSPLINEPOINT   *pt, 
                                 G3DSPLINESEGMENT *seg ) {
    list_insert ( &pt->lseg, seg );

    pt->nbseg++;
}

/******************************************************************************/
void g3dsplinepoint_removeSegment ( G3DSPLINEPOINT   *pt, 
                                    G3DSPLINESEGMENT *seg ) {
    list_remove ( &pt->lseg, seg );

    pt->nbseg--;
}

/******************************************************************************/
G3DSPLINEPOINT *g3dsplinepoint_new ( float x, float y, float z ) {
    uint32_t size = sizeof ( G3DSPLINEPOINT );
    G3DSPLINEPOINT *pt = ( G3DSPLINEPOINT * ) calloc ( 0x01, size );

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
    G3DSPLINEPOINT mid[0x05];
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
void g3dsplinesegment_draw ( G3DSPLINESEGMENT *seg,
                             float             from, /* range 0 - 1 */
                             float             to,   /* range 0 - 1 */
                             float             nbSteps,
                             GLUtesselator    *tobj,
                             double          (*coords)[0x03],
                             uint32_t          spline_flags,
                             uint32_t          engine_flags ) {
    float factor = ( to - from ) / nbSteps;
    float incFactor = ( ( from + to ) / 2.0f );
    uint32_t i;

    if ( spline_flags & DRAW_FOR_TESSELLATION ) {
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
G3DCUBICSEGMENT *g3dcubicsegment_new ( G3DSPLINEPOINT *pt0,
                                       G3DSPLINEPOINT *pt1,
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

    csg->seg.pt[0x03] = ( G3DSPLINEPOINT * ) &csg->handle[0x00];
    csg->seg.pt[0x03]->pos.x  = hx1;
    csg->seg.pt[0x03]->pos.y  = hy1;
    csg->seg.pt[0x03]->pos.z  = hz1;
    csg->seg.pt[0x03]->flags |= VERTEXHANDLE;

    csg->seg.pt[0x02] = ( G3DSPLINEPOINT * ) &csg->handle[0x01];
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
    G3DSPLINEPOINT mid[0x03];
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
                                G3DSPLINEPOINT      *pt0,
                                G3DSPLINEPOINT      *pt1,
                                float                hx, 
                                float                hy,
                                float                hz ) {
    qsg->seg.getPoint = g3dquadraticsegment_getPoint;

    qsg->seg.pt[0x00] = pt0;
    qsg->seg.pt[0x01] = pt1;

    /*** handles ***/
    qsg->handle.pt[0x00] = pt0;
    qsg->handle.pt[0x01] = pt1;

    qsg->seg.pt[0x02] = ( G3DSPLINEPOINT * ) &qsg->handle;
    qsg->seg.pt[0x02]->pos.x  = hx;
    qsg->seg.pt[0x02]->pos.y  = hy;
    qsg->seg.pt[0x02]->pos.z  = hz;
    qsg->seg.pt[0x02]->flags |= VERTEXHANDLE;
}

/******************************************************************************/
G3DQUADRATICSEGMENT *g3dquadraticsegment_new ( G3DSPLINEPOINT *pt0,
                                               G3DSPLINEPOINT *pt1,
                                               float           hx, 
                                               float           hy,
                                               float           hz ) {
    uint32_t size = sizeof ( G3DQUADRATICSEGMENT );
    G3DQUADRATICSEGMENT *qsg = ( G3DQUADRATICSEGMENT * ) calloc ( 0x01, size );

    g3dquadraticsegment_init ( qsg, pt0, pt1, hx, hy, hz );

    return qsg;
}

/******************************************************************************/
uint32_t g3dspline_draw ( G3DOBJECT *obj, 
                          G3DCAMERA *curcam, 
                          uint32_t   engine_flags ) {
    G3DSPLINE *spline = ( G3DSPLINE * ) obj;
    G3DMESH   *mesh   = ( G3DMESH   * ) obj;
    LIST *ltmpseg = spline->lseg;
    LIST *ltmpver = mesh->lver;

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glLineWidth ( 2.0f );
    glPointSize ( 3.0f );

    glDisable ( GL_LIGHTING );

    while ( ltmpseg ) {
        G3DSPLINESEGMENT *seg = ( G3DSPLINESEGMENT * ) ltmpseg->data;

        if ( obj->flags & CUBIC ) {
            G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) seg;

            if ( engine_flags & VIEWVERTEX ) {
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
            }

            if ( ( engine_flags & SELECTMODE ) == 0x00 ) {
                glColor3ub ( 0xFF, 0x7F, 0x7F );
                g3dsplinesegment_draw ( seg,
                                        0.0f, 
                                        1.0f,
                                        spline->nbStepsPerSegment,
                                        NULL, /* to tessellation object */
                                        NULL, /* no tessellation data **/
                                        0x00,
                                        engine_flags );
            }
        }

        ltmpseg = ltmpseg->next;
    }

    if ( engine_flags & VIEWVERTEX ) {

        while ( ltmpver ) {
            G3DVERTEX * ver = ( G3DVERTEX * ) ltmpver->data;

            if ( engine_flags & SELECTMODE ) {
                glLoadName ( ( GLint ) ver->id );
            }

            glBegin ( GL_POINTS );
            if ( ver->flags & VERTEXHANDLE ) {
                if ( obj->flags & CUBIC ) {
                    G3DCUBICHANDLE *han = ( G3DCUBICHANDLE * ) ver;

                    glColor3ub ( 0x00, 0xFF, 0x00 );
                    glVertex3f (  han->ver.pos.x + han->pt->pos.x, 
                                  han->ver.pos.y + han->pt->pos.y, 
                                  han->ver.pos.z + han->pt->pos.z );
                }
            } else {
                glColor3ub ( 0x00, 0x00, 0xFF );
                glVertex3f (  ver->pos.x, ver->pos.y, ver->pos.z );
            }
            glEnd ( );

            ltmpver = ltmpver->next;
        }
    }

    glPopAttrib ( );

    g3dobject_drawModifiers ( obj, curcam, engine_flags );

    return 0x00;
}

/******************************************************************************/
void g3dspline_free ( G3DOBJECT *obj ) {
    G3DSPLINE *spline = ( G3DSPLINE * ) obj;
    G3DMESH   *mes    = ( G3DMESH   * ) obj;

    list_free ( &spline->lseg, g3dcubicsegment_free );

    g3dmesh_free ( mes );
}

/******************************************************************************/
void g3dspline_addSegment ( G3DSPLINE        *spline, 
                            G3DSPLINESEGMENT *seg ) {
    list_insert ( &spline->lseg, seg );

    if ( ((G3DOBJECT*)spline)->flags & CUBIC ) {
        G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) seg;

        g3dmesh_addVertex ( spline, &csg->handle[0x00] );
        g3dmesh_addVertex ( spline, &csg->handle[0x01] );

        g3dsplinepoint_addSegment ( csg->seg.pt[0x00], csg );
        g3dsplinepoint_addSegment ( csg->seg.pt[0x01], csg );
    }

    spline->nbseg++;
}

/******************************************************************************/
void g3dspline_removeSegment ( G3DSPLINE        *spline, 
                               G3DSPLINESEGMENT *seg ) {
    list_remove ( &spline->lseg, seg );

    if ( ((G3DOBJECT*)spline)->flags & CUBIC ) {
        G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) seg;

        g3dmesh_removeVertex ( spline, &csg->handle[0x00] );
        g3dmesh_removeVertex ( spline, &csg->handle[0x01] );

        g3dsplinepoint_removeSegment ( csg->seg.pt[0x00], csg );
        g3dsplinepoint_removeSegment ( csg->seg.pt[0x01], csg );
    }

    spline->nbseg--;
}

/******************************************************************************/
void g3dspline_init ( G3DSPLINE *spline, 
                      uint32_t   id, 
                      char      *name,
                      uint32_t   type,
                      uint32_t   engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) spline;

    g3dobject_init ( obj, G3DSPLINETYPE, id, name, DRAWBEFORECHILDREN | type,
                                     DRAW_CALLBACK(g3dspline_draw),
                                     FREE_CALLBACK(g3dspline_free),
                                                   NULL,
                                                   NULL,
                                     COPY_CALLBACK(NULL),
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                 ADDCHILD_CALLBACK(NULL),
                                                   NULL );

    spline->nbStepsPerSegment = 12;

}

/******************************************************************************/
G3DSPLINE *g3dspline_new ( uint32_t id, 
                           char    *name, 
                           uint32_t type, 
                           uint32_t engine_flags ) {
    G3DSPLINE *spline = ( G3DSPLINE * ) calloc ( 0x01, sizeof ( G3DSPLINE ) );

    if ( spline == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    /** type is CUBIC or QUADRATIC **/
    g3dspline_init ( spline, id, name, type, engine_flags );

    return spline;
}

