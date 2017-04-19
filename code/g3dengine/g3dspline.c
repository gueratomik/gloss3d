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
void g3dsplinesegment_free ( G3DSPLINESEGMENT *seg ) {
    free ( seg );
}

/******************************************************************************/
void g3dsplinesegment_drawQuadratic ( G3DSPLINESEGMENT *seg,
                                      G3DSPLINEPOINT   *ori,
                                      G3DSPLINEPOINT   *end,
                                      float from, /* range 0 - 1 */
                                      float to,   /* range 0 - 1 */
                                      float maxAngle,
                                      uint32_t engine_flags) {
    G3DVECTOR       subseg[0x02];
    G3DSPLINEPOINT  han[0x02];
    G3DSPLINEPOINT  mid[0x06];
    G3DSPLINEPOINT *pt = seg->pt;
    float factor = ( to - from ) / 2.0f;
    float incFactor = ( ( from + to ) / 2.0f );
    float decFactor = 1.0f - incFactor;

    /*** compute absolute position of handles ***/
    han[0x00].pos.x = ( pt[0x00].pos.x + pt[0x03].pos.x );
    han[0x00].pos.y = ( pt[0x00].pos.y + pt[0x03].pos.y );
    han[0x00].pos.z = ( pt[0x00].pos.z + pt[0x03].pos.z );

    han[0x01].pos.x = ( pt[0x01].pos.x + pt[0x02].pos.x );
    han[0x01].pos.y = ( pt[0x01].pos.y + pt[0x02].pos.y );
    han[0x01].pos.z = ( pt[0x01].pos.z + pt[0x02].pos.z );

    /*** mid points ***/
    mid[0x00].pos.x = ( pt[0x00].pos.x * decFactor ) + ( han[0x00].pos.x * incFactor );
    mid[0x00].pos.y = ( pt[0x00].pos.y * decFactor ) + ( han[0x00].pos.y * incFactor );
    mid[0x00].pos.z = ( pt[0x00].pos.z * decFactor ) + ( han[0x00].pos.z * incFactor );

    mid[0x01].pos.x = ( pt[0x01].pos.x * incFactor ) + ( han[0x01].pos.x * decFactor );
    mid[0x01].pos.y = ( pt[0x01].pos.y * incFactor ) + ( han[0x01].pos.y * decFactor );
    mid[0x01].pos.z = ( pt[0x01].pos.z * incFactor ) + ( han[0x01].pos.z * decFactor );

    mid[0x02].pos.x = ( han[0x00].pos.x * decFactor ) + ( han[0x01].pos.x * incFactor );
    mid[0x02].pos.y = ( han[0x00].pos.y * decFactor ) + ( han[0x01].pos.y * incFactor );
    mid[0x02].pos.z = ( han[0x00].pos.z * decFactor ) + ( han[0x01].pos.z * incFactor );


    /*** mid mid points ***/
    mid[0x03].pos.x = ( mid[0x00].pos.x * decFactor ) + ( mid[0x02].pos.x * incFactor );
    mid[0x03].pos.y = ( mid[0x00].pos.y * decFactor ) + ( mid[0x02].pos.y * incFactor );
    mid[0x03].pos.z = ( mid[0x00].pos.z * decFactor ) + ( mid[0x02].pos.z * incFactor );

    mid[0x04].pos.x = ( mid[0x01].pos.x * incFactor ) + ( mid[0x02].pos.x * decFactor );
    mid[0x04].pos.y = ( mid[0x01].pos.y * incFactor ) + ( mid[0x02].pos.y * decFactor );
    mid[0x04].pos.z = ( mid[0x01].pos.z * incFactor ) + ( mid[0x02].pos.z * decFactor );


    /*** mid mid mid point ***/
    mid[0x05].pos.x = ( mid[0x03].pos.x * decFactor ) + ( mid[0x04].pos.x * incFactor );
    mid[0x05].pos.y = ( mid[0x03].pos.y * decFactor ) + ( mid[0x04].pos.y * incFactor );
    mid[0x05].pos.z = ( mid[0x03].pos.z * decFactor ) + ( mid[0x04].pos.z * incFactor );

    subseg[0x00].x = ( mid[0x05].pos.x - ori->pos.x );
    subseg[0x00].y = ( mid[0x05].pos.y - ori->pos.y );
    subseg[0x00].z = ( mid[0x05].pos.z - ori->pos.z );

    subseg[0x01].x = ( end->pos.x - mid[0x05].pos.x );
    subseg[0x01].y = ( end->pos.y - mid[0x05].pos.y );
    subseg[0x01].z = ( end->pos.z - mid[0x05].pos.z );

    /*g3dvector_normalize ( &subseg[0x00], NULL );
    g3dvector_normalize ( &subseg[0x01], NULL );*/

    if ( g3dvector_angle ( &subseg[0x00], &subseg[0x01] ) > 0.5f ) {

        g3dsplinesegment_drawQuadratic ( seg,
                                         ori,
                                        &mid[0x05],
                                         from,
                                         from + factor,
                                         maxAngle,
                                         engine_flags | ( 1 << 31 ) );
        g3dsplinesegment_drawQuadratic ( seg,
                                        &mid[0x05],
                                         end,
                                         from + factor,
                                         to,
                                         maxAngle,
                                         engine_flags | ( 1 << 31 ) );
    } else {
        glBegin ( GL_LINES );
        glVertex3f (  ori->pos.x,  ori->pos.y,  ori->pos.z );
        glVertex3f ( mid[0x05].pos.x, mid[0x05].pos.y, mid[0x05].pos.z );


        glVertex3f ( mid[0x05].pos.x, mid[0x05].pos.y, mid[0x05].pos.z );
        glVertex3f (  end->pos.x,  end->pos.y,  end->pos.z );
        glEnd   ( );
    }
}

/******************************************************************************/
G3DSPLINESEGMENT *g3dsplinesegment_new ( float  x1, float  y1, float  z1,
                                         float  x2, float  y2, float  z2,
                                         float hx1, float hy1, float hz1,
                                         float hx2, float hy2, float hz2 ) {
    uint32_t size = sizeof ( G3DSPLINESEGMENT );
    G3DSPLINESEGMENT *seg = ( G3DSPLINESEGMENT * ) calloc ( 0x01, size );

    seg->pt[0x00].pos.x = x1;
    seg->pt[0x00].pos.y = y1;
    seg->pt[0x00].pos.z = z1;

    seg->pt[0x01].pos.x = x2;
    seg->pt[0x01].pos.y = y2;
    seg->pt[0x01].pos.z = z2;

    /*** handles ***/
    seg->pt[0x03].pos.x = hx1;
    seg->pt[0x03].pos.y = hy1;
    seg->pt[0x03].pos.z = hz1;

    seg->pt[0x02].pos.x = hx2;
    seg->pt[0x02].pos.y = hy2;
    seg->pt[0x02].pos.z = hz2;

    return seg;
}

/******************************************************************************/
uint32_t g3dspline_draw ( G3DOBJECT *obj, G3DCAMERA *curcam, 
                                          uint32_t   engine_flags ) {
    G3DSPLINE *spline = ( G3DSPLINE * ) obj;
    LIST *ltmpseg = spline->lseg;

    while ( ltmpseg ) {
        G3DSPLINESEGMENT *seg = ( G3DSPLINESEGMENT * ) ltmpseg->data;

        glDisable ( GL_LIGHTING );
        glColor3ub ( 0xFF, 0x00, 0x00 );
        glBegin ( GL_LINES );
        glVertex3f (  seg->pt[0x00].pos.x, 
                      seg->pt[0x00].pos.y, 
                      seg->pt[0x00].pos.z );
        glVertex3f (  seg->pt[0x00].pos.x + seg->pt[0x03].pos.x, 
                      seg->pt[0x00].pos.y + seg->pt[0x03].pos.y, 
                      seg->pt[0x00].pos.z + seg->pt[0x03].pos.z );
        glVertex3f (  seg->pt[0x01].pos.x, 
                      seg->pt[0x01].pos.y, 
                      seg->pt[0x01].pos.z );
        glVertex3f (  seg->pt[0x01].pos.x + seg->pt[0x02].pos.x, 
                      seg->pt[0x01].pos.y + seg->pt[0x02].pos.y, 
                      seg->pt[0x01].pos.z + seg->pt[0x02].pos.z );
        glEnd   ( );
        glEnable ( GL_LIGHTING );

        g3dsplinesegment_drawQuadratic ( seg, 
                                        &seg->pt[0x00],
                                        &seg->pt[0x01], 
                                         0.0f, 
                                         1.0f,
                                         0.0f,
                                         engine_flags );

        ltmpseg = ltmpseg->next;
    }

    return 0x00;
}

/******************************************************************************/
void g3dspline_free ( G3DOBJECT *obj ) {
    G3DSPLINE *spline = ( G3DSPLINE * ) obj;

    list_free ( &spline->lseg, g3dsplinesegment_free );
}

/******************************************************************************/
void g3dspline_addSegment ( G3DSPLINE *spline, G3DSPLINESEGMENT *seg ) {
    list_insert ( &spline->lseg, seg );
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

