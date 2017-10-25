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
G3DSPLINE *g3dspline_copy ( G3DSPLINE     *spline, 
                            uint32_t       id, 
                            char          *name, 
                            uint32_t       engine_flags ) {
    G3DOBJECT *obj       = ( G3DOBJECT * ) spline;
    G3DSPLINE *newSpline = g3dspline_new ( id, name, obj->flags, engine_flags );

    newSpline->curve = g3dcurve_copy ( spline->curve, engine_flags );

    return newSpline;
}

/******************************************************************************/
uint32_t g3dspline_draw ( G3DOBJECT *obj, 
                          G3DCAMERA *curcam, 
                          uint32_t   engine_flags ) {
    G3DSPLINE *spline = ( G3DSPLINE * ) obj;
    LIST *ltmpseg = spline->curve->lseg;
    LIST *ltmpver = spline->curve->lpt;

    glPushAttrib ( GL_ALL_ATTRIB_BITS );
    glLineWidth ( 2.0f );
    glPointSize ( 3.0f );

    glDisable ( GL_LIGHTING );

    if ( ( engine_flags & SELECTMODE ) == 0x00 ) {
        while ( ltmpseg ) {
            G3DSPLINESEGMENT *seg = ( G3DSPLINESEGMENT * ) ltmpseg->data;

            if ( obj->flags & CUBIC ) {
                G3DCUBICSEGMENT *csg = ( G3DCUBICSEGMENT * ) seg;

                /* draw line to the handle */
                if ( ( obj->flags   & OBJECTSELECTED ) && ( engine_flags & VIEWVERTEX ) ) {
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
                    g3dcurvesegment_draw ( seg,
                                            0.0f, 
                                            1.0f,
                                            spline->curve->nbStepsPerSegment,
                                            NULL, /* to tessellation object */
                                            NULL, /* no tessellation data **/
                                            0x00,
                                            engine_flags );
                }
            }

            ltmpseg = ltmpseg->next;
        }
    }

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

    glPopAttrib ( );

    if ( ( ( obj->flags   & OBJECTSELECTED ) == 0x00 ) ||
         ( ( obj->flags   & OBJECTSELECTED ) &&
           ( engine_flags & SELECTMODE     ) == 0x00 ) ) {
        g3dobject_drawModifiers ( obj, curcam, engine_flags );
    }

    return 0x00;
}

/******************************************************************************/
void g3dspline_free ( G3DOBJECT *obj ) {
    G3DSPLINE *spline = ( G3DSPLINE * ) obj;

    g3dcurve_free ( spline->curve );
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
                                     COPY_CALLBACK(g3dspline_copy),
                                                   NULL,
                                                   NULL,
                                                   NULL,
                                 ADDCHILD_CALLBACK(NULL),
                                                   NULL );

    spline->curve = g3dcurve_new ( type, engine_flags );
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

