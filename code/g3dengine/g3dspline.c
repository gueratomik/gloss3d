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
G3DSPLINEPOINT *g3dsplinepoint_new ( float x, float y, float z ) {
    uint32_t size = sizeof ( G3DSPLINEPOINT );
    G3DSPLINEPOINT *pt = ( G3DSPLINEPOINT * ) calloc ( 0x01, size );

    if ( pt == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    pt->pos.x = x;
    pt->pos.y = y;
    pt->pos.z = z;

    return pt;
}

/******************************************************************************/
void g3dsplinesegment_free ( G3DSPLINESEGMENT *seg ) {
    free ( seg );
}

/******************************************************************************/
void g3dsplinesegment_getPoint ( G3DSPLINESEGMENT *seg, 
                                 float             factor, /* range 0 - 1 */
                                 G3DSPLINEPOINT   *pout ) {
    G3DSPLINEPOINT  han[0x02];
    G3DSPLINEPOINT  mid[0x05];
    float incFac = factor;
    float decFac = 1.0f - incFac;

    /*** compute absolute position of handles ***/
    han[0].pos.x = ( seg->pt[0]->pos.x + seg->pt[3]->pos.x );
    han[0].pos.y = ( seg->pt[0]->pos.y + seg->pt[3]->pos.y );
    han[0].pos.z = ( seg->pt[0]->pos.z + seg->pt[3]->pos.z );

    han[1].pos.x = ( seg->pt[1]->pos.x + seg->pt[2]->pos.x );
    han[1].pos.y = ( seg->pt[1]->pos.y + seg->pt[2]->pos.y );
    han[1].pos.z = ( seg->pt[1]->pos.z + seg->pt[2]->pos.z );


    /*** mid points ***/
    mid[0].pos.x = ( seg->pt[0]->pos.x * decFac ) + ( han[0].pos.x * incFac );
    mid[0].pos.y = ( seg->pt[0]->pos.y * decFac ) + ( han[0].pos.y * incFac );
    mid[0].pos.z = ( seg->pt[0]->pos.z * decFac ) + ( han[0].pos.z * incFac );

    mid[1].pos.x = ( seg->pt[1]->pos.x * incFac ) + ( han[1].pos.x * decFac );
    mid[1].pos.y = ( seg->pt[1]->pos.y * incFac ) + ( han[1].pos.y * decFac );
    mid[1].pos.z = ( seg->pt[1]->pos.z * incFac ) + ( han[1].pos.z * decFac );

    mid[2].pos.x = ( han[0].pos.x * decFac ) + ( han[1].pos.x * incFac );
    mid[2].pos.y = ( han[0].pos.y * decFac ) + ( han[1].pos.y * incFac );
    mid[2].pos.z = ( han[0].pos.z * decFac ) + ( han[1].pos.z * incFac );


    /*** mid mid points ***/
    mid[3].pos.x = ( mid[0].pos.x * decFac ) + ( mid[2].pos.x * incFac );
    mid[3].pos.y = ( mid[0].pos.y * decFac ) + ( mid[2].pos.y * incFac );
    mid[3].pos.z = ( mid[0].pos.z * decFac ) + ( mid[2].pos.z * incFac );

    mid[4].pos.x = ( mid[1].pos.x * incFac ) + ( mid[2].pos.x * decFac );
    mid[4].pos.y = ( mid[1].pos.y * incFac ) + ( mid[2].pos.y * decFac );
    mid[4].pos.z = ( mid[1].pos.z * incFac ) + ( mid[2].pos.z * decFac );


    /*** mid mid mid point ***/
    pout->pos.x  = ( mid[3].pos.x * decFac ) + ( mid[4].pos.x * incFac );
    pout->pos.y  = ( mid[3].pos.y * decFac ) + ( mid[4].pos.y * incFac );
    pout->pos.z  = ( mid[3].pos.z * decFac ) + ( mid[4].pos.z * incFac );
}

/******************************************************************************/
void g3dsplinesegment_drawQuadratic ( G3DSPLINESEGMENT *seg,
                                      G3DSPLINEPOINT   *pori,
                                      G3DSPLINEPOINT   *pend,
                                      float from, /* range 0 - 1 */
                                      float to,   /* range 0 - 1 */
                                      float maxAngle,
                                      uint32_t engine_flags) {
    G3DVECTOR       subseg[0x02];
    float factor = ( to - from ) / 2.0f;
    float incFactor = ( ( from + to ) / 2.0f );
    G3DSPLINEPOINT pout;
    float len0, len1;

    g3dsplinesegment_getPoint ( seg, incFactor, &pout );

    subseg[0x00].x = ( pout.pos.x - pori->pos.x );
    subseg[0x00].y = ( pout.pos.y - pori->pos.y );
    subseg[0x00].z = ( pout.pos.z - pori->pos.z );

    subseg[0x01].x = ( pend->pos.x - pout.pos.x );
    subseg[0x01].y = ( pend->pos.y - pout.pos.y );
    subseg[0x01].z = ( pend->pos.z - pout.pos.z );

    g3dvector_normalize ( &subseg[0x00], &len0 );
    g3dvector_normalize ( &subseg[0x01], &len1 );

    if ( len0 && len1 ) {
        if ( g3dvector_angle ( &subseg[0x00], &subseg[0x01] ) > 0.1f ) {

            g3dsplinesegment_drawQuadratic ( seg,
                                             pori,
                                            &pout,
                                             from,
                                             from + factor,
                                             maxAngle,
                                             engine_flags );
            g3dsplinesegment_drawQuadratic ( seg,
                                            &pout,
                                             pend,
                                             from + factor,
                                             to,
                                             maxAngle,
                                             engine_flags );
        } else {
            glBegin ( GL_LINES );
            glVertex3f ( pori->pos.x, pori->pos.y, pori->pos.z );
            glVertex3f (  pout.pos.x,  pout.pos.y, pout.pos.z  );

            glVertex3f (  pout.pos.x,  pout.pos.y, pout.pos.z  );
            glVertex3f ( pend->pos.x, pend->pos.y, pend->pos.z );
            glEnd   ( );
        }
    }
}

/******************************************************************************/
G3DSPLINESEGMENT *g3dsplinesegment_new ( G3DSPLINEPOINT *pt0,
                                         G3DSPLINEPOINT *pt1,
                                         float hx1, float hy1, float hz1,
                                         float hx2, float hy2, float hz2 ) {
    uint32_t size = sizeof ( G3DSPLINESEGMENT );
    G3DSPLINESEGMENT *seg = ( G3DSPLINESEGMENT * ) calloc ( 0x01, size );

    seg->pt[0x00] = pt0;
    seg->pt[0x01] = pt1;

    /*** handles ***/
    seg->handle[0x00].pt = pt0;
    seg->handle[0x01].pt = pt1;

    seg->pt[0x03] = ( G3DSPLINEPOINT * ) &seg->handle[0x00];
    seg->pt[0x03]->pos.x  = hx1;
    seg->pt[0x03]->pos.y  = hy1;
    seg->pt[0x03]->pos.z  = hz1;
    seg->pt[0x03]->flags |= VERTEXHANDLE;

    seg->pt[0x02] = ( G3DSPLINEPOINT * ) &seg->handle[0x01];
    seg->pt[0x02]->pos.x  = hx2;
    seg->pt[0x02]->pos.y  = hy2;
    seg->pt[0x02]->pos.z  = hz2;
    seg->pt[0x02]->flags |= VERTEXHANDLE;

    return seg;
}

/******************************************************************************/
uint32_t g3dspline_draw ( G3DOBJECT *obj, G3DCAMERA *curcam, 
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

        if ( engine_flags & VIEWVERTEX ) {
            glColor3ub ( 0xFF, 0x7F, 0x00 );
            glBegin ( GL_LINES );
            glVertex3f (  seg->pt[0x00]->pos.x, 
                          seg->pt[0x00]->pos.y, 
                          seg->pt[0x00]->pos.z );
            glVertex3f (  seg->pt[0x00]->pos.x + seg->pt[0x03]->pos.x, 
                          seg->pt[0x00]->pos.y + seg->pt[0x03]->pos.y, 
                          seg->pt[0x00]->pos.z + seg->pt[0x03]->pos.z );
            glVertex3f (  seg->pt[0x01]->pos.x, 
                          seg->pt[0x01]->pos.y, 
                          seg->pt[0x01]->pos.z );
            glVertex3f (  seg->pt[0x01]->pos.x + seg->pt[0x02]->pos.x, 
                          seg->pt[0x01]->pos.y + seg->pt[0x02]->pos.y, 
                          seg->pt[0x01]->pos.z + seg->pt[0x02]->pos.z );
            glEnd   ( );
        }
        
        if ( ( engine_flags & SELECTMODE ) == 0x00 ) {
            glColor3ub ( 0xFF, 0x7F, 0x7F );
            g3dsplinesegment_drawQuadratic ( seg, 
                                             seg->pt[0x00],
                                             seg->pt[0x01], 
                                             0.0f, 
                                             1.0f,
                                             0.0f,
                                             engine_flags );
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
                G3DCUBICSPLINEHANDLE *han = ( G3DCUBICSPLINEHANDLE * ) ver;

                glColor3ub ( 0x00, 0xFF, 0x00 );
                glVertex3f (  han->ver.pos.x + han->pt->pos.x, 
                              han->ver.pos.y + han->pt->pos.y, 
                              han->ver.pos.z + han->pt->pos.z );
            } else {
                glColor3ub ( 0x00, 0x00, 0xFF );
                glVertex3f (  ver->pos.x, ver->pos.y, ver->pos.z );
            }
            glEnd ( );

            ltmpver = ltmpver->next;
        }
    }

    glPopAttrib ( );

    return 0x00;
}

/******************************************************************************/
void g3dspline_free ( G3DOBJECT *obj ) {
    G3DSPLINE *spline = ( G3DSPLINE * ) obj;
    G3DMESH   *mes    = ( G3DMESH   * ) obj;

    list_free ( &spline->lseg, g3dsplinesegment_free );

    g3dmesh_free ( mes );
}

/******************************************************************************/
void g3dspline_addSegment ( G3DSPLINE *spline, G3DSPLINESEGMENT *seg ) {
    list_insert ( &spline->lseg, seg );

    g3dmesh_addVertex ( spline, &seg->handle[0x00] );
    g3dmesh_addVertex ( spline, &seg->handle[0x01] );

    spline->nbseg++;
}

/******************************************************************************/
void g3dspline_removeSegment ( G3DSPLINE *spline, G3DSPLINESEGMENT *seg ) {
    list_remove ( &spline->lseg, seg );

    g3dmesh_removeVertex ( spline, &seg->handle[0x00] );
    g3dmesh_removeVertex ( spline, &seg->handle[0x01] );

    spline->nbseg--;
}

/******************************************************************************/
void g3dspline_init ( G3DSPLINE *spline, uint32_t id, 
                                         char    *name,
                                         uint32_t type,
                                         uint32_t engine_flags ) {
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

