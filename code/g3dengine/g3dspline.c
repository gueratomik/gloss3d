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
static G3DSPLINE *_default_copy ( G3DSPLINE *spline, 
                                  uint32_t   id, 
                                  char      *name, 
                                  uint64_t   engineFlags ) {
    G3DOBJECT *obj       = ( G3DOBJECT * ) spline;
    G3DSPLINE *newSpline = g3dspline_new ( id, name, obj->flags, engineFlags );

    newSpline->curve = g3dcurve_copy ( spline->curve, engineFlags );

    return newSpline;
}

/******************************************************************************/
static uint32_t _default_pick ( G3DOBJECT *obj, 
                                G3DCAMERA *curcam, 
                                uint64_t   engineFlags ) {
    G3DSPLINE *spline = ( G3DSPLINE * ) obj;

    if ( obj->flags & OBJECTSELECTED ) {
        if ( engineFlags & VIEWEDGE ) {
            g3dcurve_pickSegments ( spline->curve, engineFlags );
        }
        if ( engineFlags & VIEWVERTEX ) {
            g3dcurve_pickPoints  ( spline->curve, engineFlags );
            g3dcurve_pickHandles ( spline->curve, engineFlags );
        }
    } else {
        if ( engineFlags & VIEWOBJECT ) {
            g3dpick_setName ( ( uint64_t ) spline );
            g3dcurve_pick ( spline->curve, engineFlags );
        }
    }

    return 0x00;
}

/******************************************************************************/
void g3dspline_modify ( G3DSPLINE  *spl,
                        G3DMODIFYOP op,
                        uint64_t    engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) spl;
    LIST *ltmpchildren = obj->childList;
/*
    g3dmesh_renumberVertices ( mes );
    g3dmesh_renumberEdges    ( mes );
    g3dmesh_renumberFaces    ( mes );
*/
    spl->lastmod = NULL;

    while ( ltmpchildren ) {
        G3DOBJECT *child = ( G3DOBJECT * ) ltmpchildren->data;

        if ( child->type & MODIFIER ) {
            G3DMODIFIER *mod = ( G3DMODIFIER * ) child;

            spl->lastmod = g3dmodifier_modify_r ( mod,
                                  ( G3DOBJECT * ) spl,
                                                  NULL,
                                                  NULL,
                                                  op,
                                                  engine_flags );
        }

        ltmpchildren = ltmpchildren->next;        
    }
}

/******************************************************************************/
static void _default_update ( G3DSPLINE *spl,
                              LIST      *lpt,
                              uint32_t   update_flags,
                              uint64_t   engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) spl;

    if ( obj->invalidationFlags & INVALIDATE_MODIFIER_STACK_RESET ) {
        g3dspline_modify ( spl,
                           G3DMODIFYOP_MODIFY,
                           engine_flags );
    }

    if ( obj->invalidationFlags & INVALIDATE_MODIFIER_STACK_UPDATE ) {
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
                          float    *PREVMVX, /* previous world matrix */
                          uint64_t engine_flags ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) spl;
    LIST *ltmppt = spl->curve->lpt;
    float DIFFMVX[0x10];

    g3dcore_multMatrixf ( PREVMVX, obj->inverseWorldMatrix, DIFFMVX );

    /*** move object for children ***/
    g3dobject_moveAxis ( obj, PREVMVX, engine_flags );

    while ( ltmppt ) {
        G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmppt->data;
        G3DVECTOR3F  pos = { .x = pt->pos.x,
                             .y = pt->pos.y,
                             .z = pt->pos.z };

        g3dvector3f_matrixf ( &pos, DIFFMVX, &pt->pos );
#ifdef UNUSED
        } else {
            float DIFFROT[0x10];

            /*** spline handles are vectors, they are altered by rotation matrix **/
            g3dcore_extractRotationMatrixf ( DIFFMVX, DIFFROT );

            g3dvector3f_matrixf ( &pos, DIFFROT, &ver->pos );
        }
#endif 

        ltmppt = ltmppt->next;
    }

    g3dobject_invalidate( G3DOBJECTCAST(spl), INVALIDATE_MODIFIER_STACK_RESET );
}

/******************************************************************************/
static uint32_t _default_draw ( G3DOBJECT *obj, 
                                G3DCAMERA *curcam, 
                                uint64_t engine_flags ) {
    G3DSPLINE *spl = ( G3DSPLINE * ) obj;


#ifdef need_refactor
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
#endif

    return 0x00;
}

/******************************************************************************/
static void _default_free ( G3DOBJECT *obj ) {
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

    g3dobject_init ( obj,
                     G3DSPLINETYPE,
                     id,
                     name,
                     type,
       DRAW_CALLBACK(_default_draw),
       FREE_CALLBACK(_default_free),
       PICK_CALLBACK(_default_pick),
       ANIM_CALLBACK(NULL),
     UPDATE_CALLBACK(NULL),
       POSE_CALLBACK(NULL),
       COPY_CALLBACK(_default_copy),
  TRANSFORM_CALLBACK(NULL),
   ACTIVATE_CALLBACK(NULL),
 DEACTIVATE_CALLBACK(NULL),
     COMMIT_CALLBACK(NULL),
   ADDCHILD_CALLBACK(NULL),
REMOVECHILD_CALLBACK(NULL),
  SETPARENT_CALLBACK(NULL) );

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

