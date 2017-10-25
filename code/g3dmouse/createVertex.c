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
#include <g3dmouse.h>

/******************************************************************************/
/* each function must return FALSE for redrawing the OGL Widget it belongs to */
/* only or TRUE to redraw all OGL Widgets                                     */
/******************************************************************************/

/******************************************************************************/
/*static void createVertex_undo ( void *data ) {
    URMCREATEVERTEX *ucv = ( URMCREATEVERTEX * ) data;
    G3DVERTEX *ver = ucv->ver;
    G3DMESH *mes = ucv->mes;

    list_remove ( &mes->lver, ver );
}*/

/******************************************************************************/
/*static void createVertex_redo ( void *data ) {
    URMCREATEVERTEX *ucv = ( URMCREATEVERTEX * ) data;
    G3DVERTEX *ver = ucv->ver;
    G3DMESH *mes = ucv->mes;

    list_append ( &mes->lver, ver );
}*/

/******************************************************************************/
static void createVertex_free ( void *data ) {
    /*URMCREATEVERTEX *ucv = ( URMCREATEVERTEX * ) data;*/
}

/******************************************************************************/
/*static void createVertex_push ( G3DURMANAGER *urm, G3DMESH *mes,
                                                   G3DVERTEX *ver ) {
    URMCREATEVERTEX *ucv;

    ucv = ( URMCREATEVERTEX * ) calloc ( 0x01, sizeof ( URMCREATEVERTEX ) );

    if ( ucv == NULL ) {
        fprintf ( stderr, "createVertex_push: calloc failed\n" );

        return;
    }

    ucv->mes = mes;
    ucv->ver = ver;

    g3durmanager_push ( urm, createVertex_undo,
                             createVertex_redo,
                             createVertex_free, ucv );
}*/

/******************************************************************************/
G3DCREATEVERTEX *createVertex_new ( ) {
    uint32_t structsize = sizeof ( G3DCREATEVERTEX );

    G3DCREATEVERTEX *cv =  ( G3DCREATEVERTEX * ) calloc ( 0x04, structsize );

    if ( cv == NULL ) {
        fprintf ( stderr, "createVertex_new: Memory allocation failed\n" );
    }

    return cv;
}

/******************************************************************************/
uint32_t createVertex_init ( G3DMOUSETOOL *mou, G3DSCENE *sce, 
                                                G3DCAMERA *cam,
                                                G3DURMANAGER *urm, 
                                                uint32_t engine_flags ) {
    /*** reset the vertex array ***/
    if ( mou->data ) {
        G3DCREATEVERTEX *cv = mou->data;

        /*cv->lastAddedVertex = NULL;*/
    } else {
        mou->data = createVertex_new ( );
    }

    return 0x00;
}

/******************************************************************************/
int createVertex ( G3DMOUSETOOL *mou, G3DSCENE *sce, G3DCAMERA *cam,
                   G3DURMANAGER *urm, uint32_t flags, G3DEvent *event ) {
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
                G3DMESH *mes = ( G3DMESH * ) obj;
                G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

                if ( ( obj->type & EDITABLE ) == 0x00 ) {
                    fprintf ( stderr, "Not an editable object\n" );

                    return FALSE;
                }

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
                    G3DVERTEX *lastver = ( mes->lselver ) ? mes->lselver->data : NULL;

                    g3dmesh_unselectAllVertices ( mes );

                    if ( obj->type == G3DSPLINETYPE ) {
                        G3DSPLINE *spline = ( G3DSPLINE * ) obj;
                        G3DSPLINESEGMENT *seg = NULL;
                        G3DSPLINEPOINT *pt = g3dcurvepoint_new ( objx, 
                                                                  objy,
                                                                  objz );

                        /* 
                         * check if the point can be connected to a segment 
                         * i.e does not belong to more than 1 segment.
                         */
                        if ( lastver == NULL ) {
                            lastver = g3dcurve_getConnectablePoint ( spline->curve );
                        } else {
                            if ( ((G3DSPLINEPOINT*)lastver)->nbseg > 0x01 ) {
                                lastver = g3dcurve_getConnectablePoint ( spline->curve );
                            }
                        }

                        if ( lastver ) {
                            seg = g3dcubicsegment_new ( pt,
                                                        lastver,
                                                        0.0f, 0.0f, 0.0f,
                                                        0.0f, 0.0f, 0.0f );
                        }

                        g3durm_spline_addPoint ( urm,
                                                 spline,
                                                 pt,
                                                 seg,
                                                 flags,
                                                 REDRAWVIEW );
                    }

                    if ( obj->type == G3DMESHTYPE ) {
                        G3DVERTEX *ver = g3dvertex_new ( objx, objy, objz );
 
                        g3dmesh_addSelectedVertex ( mes, ver );

                        /*** Rebuild the mesh or spline ***/
                        g3dmesh_update ( mes, NULL,
                                              NULL,
                                              NULL,
                                              UPDATEFACEPOSITION |
                                              UPDATEFACENORMAL   |
                                              UPDATEVERTEXNORMAL |
                                              RESETMODIFIERS, flags );

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
