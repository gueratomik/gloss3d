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
G3DSPLINE *g3dspline_copy ( G3DSPLINE     *spline, 
                            uint32_t       id, 
                            char          *name, 
                            uint64_t engine_flags ) {
    G3DOBJECT *obj       = ( G3DOBJECT * ) spline;
    G3DSPLINE *newSpline = g3dspline_new ( id, name, obj->flags, engine_flags );

    newSpline->curve = g3dcurve_copy ( spline->curve, engine_flags );

    return newSpline;
}

/******************************************************************************/
uint32_t g3dspline_pick ( G3DOBJECT *obj, 
                          G3DCAMERA *curcam, 
                          uint64_t engine_flags ) {
    G3DSPLINE *spline = ( G3DSPLINE * ) obj;

    if ( obj->flags & OBJECTSELECTED ) {
        if ( engine_flags & VIEWEDGE ) {
            g3dcurve_pickSegments ( spline->curve, engine_flags );
        }
        if ( engine_flags & VIEWVERTEX ) {
            g3dcurve_pickPoints  ( spline->curve, engine_flags );
            g3dcurve_pickHandles ( spline->curve, engine_flags );
        }
    } else {
        if ( engine_flags & VIEWOBJECT ) {
            g3dpick_setName ( spline );
            g3dcurve_pick ( spline->curve, engine_flags );
        }
    }

    return 0x00;
}

/******************************************************************************/
void g3dspline_modify ( G3DSPLINE  *spl,
                        G3DMODIFYOP op,
                        uint64_t    engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) spl;
    LIST *ltmpchildren = obj->lchildren;
/*
    g3dmesh_renumberVertices ( mes );
    g3dmesh_renumberEdges    ( mes );
    g3dmesh_renumberFaces    ( mes );
*/
    spl->lastmod = NULL;

    while ( ltmpchildren ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;

        if ( child->type & MODIFIER ) {
            spl->lastmod = g3dmodifier_modify_r ( child,
                                                  spl,
                                                  NULL,
                                                  NULL,
                                                  op,
                                                  engine_flags );
        }

        ltmpchildren = ltmpchildren->next;        
    }

    if ( spl->lastmod ) {
        if ( spl->lastmod->mes.obj.flags & MODIFIERNEEDSNORMALUPDATE ) {
            if ( ( spl->lastmod->mes.obj.type & MESH ) == 0x00 ) {
                g3dmesh_update ( spl->lastmod, 
                                 NULL, /*** update vertices    ***/
                                 NULL, /*** update edges       ***/
                                 NULL, /*** update faces       ***/
                                 UPDATEFACENORMAL   |
                                 UPDATEVERTEXNORMAL,
                                 engine_flags );
            }
        }
    }
}

/******************************************************************************/
void g3dspline_update ( G3DSPLINE *spl,
                        LIST      *lpt,
                        uint32_t   update_flags,
                        uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) spl;

    if ( update_flags & RESETMODIFIERS ) {
        g3dspline_modify ( spl,
                           G3DMODIFYOP_MODIFY,
                           engine_flags );
    }

    if ( update_flags & UPDATEMODIFIERS ) {
        /*** usually modifier update is based on selected vertices/faces/edges ***/
        LIST *ltmpselpt = spl->curve->lselpt;

        spl->curve->lselpt = ( lpt == NULL ) ? spl->curve->lpt : lpt;

        g3dspline_modify ( spl,
                           G3DMODIFYOP_UPDATE,
                           engine_flags );

        /*** restore selected items ***/
        spl->curve->lselpt = ltmpselpt;
    }
}

/******************************************************************************/
void g3dspline_moveAxis ( G3DSPLINE *spl, 
                          double    *PREVMVX, /* previous world matrix */
                          uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) spl;
    LIST *ltmppt = spl->curve->lpt;
    double DIFFMVX[0x10];

    g3dcore_multmatrix ( PREVMVX, obj->iwmatrix, DIFFMVX );

    /*** move object for children ***/
    g3dobject_moveAxis ( obj, PREVMVX, engine_flags );

    while ( ltmppt ) {
        G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmppt->data;
        G3DVECTOR  pos = { .x = pt->pos.x,
                           .y = pt->pos.y,
                           .z = pt->pos.z,
                           .w = 1.0f };

        g3dvector_matrix ( &pos, DIFFMVX, &pt->pos );
#ifdef UNUSED
        } else {
            double DIFFROT[0x10];

            /*** spline handles are vectors, they are altered by rotation matrix **/
            g3dcore_extractRotationMatrix ( DIFFMVX, DIFFROT );

            g3dvector_matrix ( &pos, DIFFROT, &ver->pos );
        }
#endif 

        ltmppt = ltmppt->next;
    }

    /*g3dmesh_updateBbox ( mes );*/
    g3dspline_update ( spl, NULL, RESETMODIFIERS, engine_flags );
}

/******************************************************************************/
uint32_t g3dspline_draw ( G3DOBJECT *obj, 
                          G3DCAMERA *curcam, 
                          uint64_t engine_flags ) {
    G3DSPLINE *spl = ( G3DSPLINE * ) obj;



    /*** this means a modifier has taken over ***/
    if ( spl->lastmod ) {
        g3dmodifier_moddraw ( spl->lastmod, curcam, engine_flags );

        glPushAttrib ( GL_ALL_ATTRIB_BITS );
        glLineWidth ( 2.0f );
        glPointSize ( 4.0f );

        if ( obj->flags & OBJECTSELECTED ) {
            glDisable ( GL_LIGHTING );

            if ( engine_flags & VIEWEDGE ) {
                g3dcurve_drawSegments ( spl->curve, engine_flags );
            }

            if ( engine_flags & VIEWVERTEX ) {
                g3dcurve_drawSegments ( spl->curve, engine_flags );
                g3dcurve_drawHandles  ( spl->curve, engine_flags );
                g3dcurve_drawPoints   ( spl->curve, engine_flags );
            }
        }

        glPopAttrib ( );
    } else {
        glPushAttrib ( GL_ALL_ATTRIB_BITS );
        glLineWidth ( 2.0f );
        glPointSize ( 4.0f );

        glDisable ( GL_LIGHTING );

        if ( obj->flags & OBJECTSELECTED ) {
            if ( engine_flags & VIEWOBJECT ) {
                g3dcurve_draw ( spl->curve, engine_flags );
            }

            if ( engine_flags & VIEWAXIS ) {
                g3dcurve_draw ( spl->curve, engine_flags );
            }

            if ( engine_flags & VIEWEDGE ) {
                g3dcurve_drawSegments ( spl->curve, engine_flags );
            }

            if ( engine_flags & VIEWVERTEX ) {
                g3dcurve_drawSegments ( spl->curve, engine_flags );
                g3dcurve_drawHandles  ( spl->curve, engine_flags );
                g3dcurve_drawPoints   ( spl->curve, engine_flags );
            }
        } else {
            g3dcurve_draw ( spl->curve, engine_flags );
        }

        glPopAttrib ( );
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
                      uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) spline;

    g3dobject_init ( obj, G3DSPLINETYPE, id, name, type,
                                     DRAW_CALLBACK(g3dspline_draw),
                                     FREE_CALLBACK(g3dspline_free),
                                     PICK_CALLBACK(g3dspline_pick),
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
                           uint64_t engine_flags ) {
    G3DSPLINE *spline = ( G3DSPLINE * ) calloc ( 0x01, sizeof ( G3DSPLINE ) );

    if ( spline == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    /** type is CUBIC or QUADRATIC **/
    g3dspline_init ( spline, id, name, type, engine_flags );

    return spline;
}

