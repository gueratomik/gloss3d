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
#include <g3dmouse.h>

/******************************************************************************/
/* each function must return FALSE for redrawing the OGL Widget it belongs to */
/* only or TRUE to redraw all OGL Widgets                                     */
/******************************************************************************/

static uint32_t createVertex_init ( G3DMOUSETOOL *mou, 
                                    G3DSCENE     *sce, 
                                    G3DCAMERA    *cam,
                                    G3DURMANAGER *urm, 
                                    uint64_t      engine_flags );
static int createVertex_tool ( G3DMOUSETOOL *mou, 
                               G3DSCENE     *sce, 
                               G3DCAMERA    *cam,
                               G3DURMANAGER *urm, 
                               uint64_t      engine_flags, 
                               G3DEvent     *event );

/******************************************************************************/
G3DMOUSETOOLCREATEVERTEX *g3dmousetoolcreatevertex_new ( ) {
    uint32_t structsize = sizeof ( G3DMOUSETOOLCREATEVERTEX );

    G3DMOUSETOOLCREATEVERTEX *cv = ( G3DMOUSETOOLCREATEVERTEX * ) calloc ( 0x01, structsize );

    if ( cv == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( cv,
                        CREATEVERTEXTOOL,
                        's',
                        NULL,
                        createVertex_init,
                        NULL,
                        createVertex_tool,
                        0x00 );

    return cv;
}

/******************************************************************************/
static uint32_t createVertex_init ( G3DMOUSETOOL *mou, 
                                    G3DSCENE     *sce, 
                                    G3DCAMERA    *cam,
                                    G3DURMANAGER *urm, 
                                    uint64_t      engine_flags ) {

    return 0x00;
}

/******************************************************************************/
static int createVertex_tool ( G3DMOUSETOOL *mou, 
                               G3DSCENE     *sce, 
                               G3DCAMERA    *cam,
                               G3DURMANAGER *urm, 
                               uint64_t      engine_flags, 
                               G3DEvent     *event ) {
    /*G3DURMANAGER *urm = gdt->urm;*/
    static GLdouble MVX[0x10], PJX[0x10];
    static GLint VPX[0x04];
    /*static uint32_t xold, yold;*/
    static double objx, objy, objz,
                  winx, winy, winz;
    static G3DMESH *mes;

    switch ( event->type ) {
        case G3DButtonPress : {

        } break;

        case G3DButtonRelease : {
            G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

            if ( obj ) {
                G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

                /*** We need the selected object matrix in order to create ***/
                /*** the cutting plan and find its coords, but do not ***/
                /*** forget the current matrix is the camera transformations **/
                glPushMatrix ( );
                glMultMatrixd ( obj->wmatrix );

                glGetDoublev  ( GL_MODELVIEW_MATRIX, MVX );
                glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
                glGetIntegerv ( GL_VIEWPORT, VPX );

                gluProject ( 0.0f, 0.0f, 0.0f, MVX, PJX, VPX, &winx, &winy, &winz );
                gluUnProject ( ( GLdouble ) bev->x,
                               ( GLdouble ) VPX[0x03] - bev->y,
                               ( GLdouble ) winz,
                               MVX, PJX, VPX,
                               &objx, &objy, &objz );

                /*** hitting CTRL + click creates a vertex ***/
                if ( /*( bev->state & G3DControlMask ) &&*/
                     ( bev->state & G3DControlMask )  ) {

                    if ( obj->type == G3DSPLINETYPE ) {
                        G3DSPLINE *spline = ( G3DSPLINE * ) obj;
                        G3DSPLINESEGMENT *seg = NULL;
                        G3DCURVEPOINT *pt = g3dcurvepoint_new ( objx, 
                                                                objy,
                                                                objz );

                        G3DCURVEPOINT *lastpt = ( spline->curve->lselpt ) ? spline->curve->lselpt->data : NULL;

                        g3dcurve_unselectAllPoints ( spline->curve );

                        /* 
                         * check if the point can be connected to a segment 
                         * i.e does not belong to more than 1 segment.
                         */
                        if ( lastpt == NULL ) {
                            lastpt = g3dcurve_getConnectablePoint ( spline->curve );
                        } else {
                            if ( ((G3DSPLINEPOINT*)lastpt)->nbseg > 0x01 ) {
                                lastpt = g3dcurve_getConnectablePoint ( spline->curve );
                            }
                        }

                        if ( lastpt ) {
                            seg = ( G3DCURVESEGMENT * ) 
                                      g3dcubicsegment_new ( pt,
                                                            lastpt,
                                                            0.0f, 0.0f, 0.0f,
                                                            0.0f, 0.0f, 0.0f );
                        }

                        g3durm_spline_addPoint ( urm,
                                                 spline,
                                                 pt,
                                                 seg,
                                                 engine_flags,
                                                 REDRAWVIEW );
                    }

                    if ( obj->type == G3DMESHTYPE ) {
                        G3DMESH *mes = ( G3DMESH * ) obj;
                        G3DVERTEX *ver = g3dvertex_new ( objx, objy, objz );
 
                        G3DVERTEX *lastver = ( mes->lselver ) ? mes->lselver->data : NULL;

                        g3dmesh_unselectAllVertices ( mes );


                        g3dmesh_addSelectedVertex ( mes, ver );

                        mes->obj.update_flags |= ( UPDATEFACEPOSITION |
                                                   UPDATEFACENORMAL   |
                                                   UPDATEVERTEXNORMAL |
                                                   RESETMODIFIERS );

                        /*** Rebuild the mesh or spline ***/
                        g3dmesh_update ( mes, engine_flags );

                        /*** add this action to the undo redo stack ***/
                        /*createVertex_push ( urm, mes, ver );*/

                        g3dmesh_updateBbox ( mes );

                        /*** record for the undo-redo management ***/
                        g3durm_mesh_addVertex ( urm, mes, ver, REDRAWVIEW );
                    }
                }

                glPopMatrix ( );
            }
        } return REDRAWVIEW;

        default :
        break;
    }

    /*xold = event->xmotion.x;
    yold = event->xmotion.y;*/

    return FALSE;
}
