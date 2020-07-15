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
static uint32_t bridge_init ( G3DMOUSETOOL *mou, 
                              G3DSCENE     *sce, 
                              G3DCAMERA    *cam,
                              G3DURMANAGER *urm, 
                              uint32_t      engine_flags );
static void bridge_draw ( G3DMOUSETOOL *mou, 
                          G3DSCENE     *sce, 
                          uint32_t      flags );
static int bridge_tool  ( G3DMOUSETOOL *mou, 
                          G3DSCENE     *sce, 
                          G3DCAMERA    *cam,
                          G3DURMANAGER *urm, 
                          uint32_t      eflags,
                          G3DEvent     *event );

/******************************************************************************/
G3DMOUSETOOLBRIDGE *g3dmousetoolbridge_new ( ) {
    uint32_t structsize = sizeof ( G3DMOUSETOOLBRIDGE );

    G3DMOUSETOOLBRIDGE *bt =  ( G3DMOUSETOOLBRIDGE * ) calloc ( 0x04, structsize );

    if ( bt == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( bt,
                        BRIDGETOOL,
                        's',
                        NULL,
                        bridge_init,
                        bridge_draw,
                        bridge_tool,
                        0x00 );

    return bt;
}

/******************************************************************************/
static uint32_t bridge_init ( G3DMOUSETOOL *mou, 
                              G3DSCENE     *sce, 
                              G3DCAMERA    *cam,
                              G3DURMANAGER *urm, 
                              uint32_t      engine_flags ) {
    G3DMOUSETOOLBRIDGE *bt = ( G3DMOUSETOOLBRIDGE * ) mou;

    bt->ver[0x00] = 
    bt->ver[0x01] = 
    bt->ver[0x02] = 
    bt->ver[0x03] =
    bt->pt[0x00]  = 
    bt->pt[0x01]  = NULL;
    bt->draw = 0x00;
    bt->obj  = NULL;

    return 0x00;
}

/******************************************************************************/
static void bridge_draw ( G3DMOUSETOOL *mou, 
                          G3DSCENE     *sce, 
                          uint32_t      flags ) {
    G3DMOUSETOOLBRIDGE *bt = ( G3DMOUSETOOLBRIDGE * ) mou;

    if ( bt && bt->draw ) {
        if ( bt->obj ) {
            glPushAttrib( GL_ALL_ATTRIB_BITS );
            glDisable   ( GL_DEPTH_TEST );
            glDisable   ( GL_LIGHTING );
            glColor3ub  ( 0xFF, 0x00, 0x00 );

            /*** We need the selected object matrix in order to create ***/
            /*** the cutting plan and find its coords, but do not ***/
            /*** forget the current matrix is the camera transformations **/
            glPushMatrix ( );
            glMultMatrixd ( bt->obj->wmatrix );

            glBegin ( GL_LINES );
            if ( bt->obj->type & MESH ) {
        	if ( bt->ver[0x02] && bt->ver[0x03] ) {
                    glVertex3fv ( ( GLfloat * ) &bt->ver[0x02]->pos );
                    glVertex3fv ( ( GLfloat * ) &bt->ver[0x03]->pos );
        	}

        	if ( bt->ver[0x00] && bt->ver[0x01] ) {
                    glVertex3fv ( ( GLfloat * ) &bt->ver[0x00]->pos );
                    glVertex3fv ( ( GLfloat * ) &bt->ver[0x01]->pos );
        	}
            }
            if ( bt->obj->type & SPLINE ) {
        	if ( bt->pt[0x00] && bt->pt[0x01] ) {
                    glVertex3fv ( ( GLfloat * ) &bt->pt[0x00]->pos );
                    glVertex3fv ( ( GLfloat * ) &bt->pt[0x01]->pos );
        	}
            }
            glEnd ( );

            glPopMatrix ( );

            glPopAttrib ( );
        }
    }
}

/******************************************************************************/
static int bridge_spline  ( G3DSPLINE    *spl,
                            G3DMOUSETOOL *mou, 
                            G3DSCENE     *sce, 
                            G3DCAMERA    *cam,
                            G3DURMANAGER *urm, 
                            uint32_t      flags,
                            G3DEvent     *event ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) spl;
    G3DMOUSETOOLBRIDGE *bt = ( G3DMOUSETOOLBRIDGE * ) mou;
    static GLint VPX[0x04];
    static G3DPICKTOOL ptool = { .coord = { 0 },
                                 .only_visible = 0x01,
                                 .weight = 0.0f,
                                 .radius = 0x08 };
 
    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            /*** Start drawing ***/
            bt->draw = 0x01;

            glGetIntegerv ( GL_VIEWPORT, VPX );

            /*** Selection rectangle ***/
            /*** Selection rectangle ***/
            ptool.coord[0x00] = bev->x;
            ptool.coord[0x01] = VPX[0x03] - bev->y;
            ptool.coord[0x02] = ptool.coord[0x00];
            ptool.coord[0x03] = ptool.coord[0x01];

            g3dcurve_unselectAllPoints ( spl->curve );

            pick_Item ( &ptool, sce, cam, 0x00, flags );

            /*** remember our object for the drawing part ***/
            /*** because we need its world matrix.      ***/
            bt->obj = spl;

            /*** if any selected point ***/
            if ( spl->curve->lselpt ) {
                bt->pt[0x00] = g3dcurve_getLastSelectedPoint ( spl->curve );
            } else {
            /*** reset if click fails to pick a vertex ***/
                bt->pt[0x00] = bt->pt[0x01] = NULL;
            }
        } return REDRAWVIEW;

        case G3DMotionNotify : {
            G3DMotionEvent *bev = ( G3DMotionEvent * ) event;

            if ( bev->state & G3DButton1Mask ) {
                G3DMESH *mes = ( G3DMESH * ) obj;
                /*** Selection rectangle ***/
                ptool.coord[0x00] = bev->x;
                ptool.coord[0x01] = VPX[0x03] - bev->y;
                ptool.coord[0x02] = ptool.coord[0x00];
                ptool.coord[0x03] = ptool.coord[0x01];

                pick_Item ( &ptool, sce, cam, 0x00, VIEWVERTEX );

                if ( spl->curve->lselpt ) {
                    bt->pt[0x01] = g3dcurve_getLastSelectedPoint ( spl->curve );
                }
            }
        } return REDRAWVIEW;

        case G3DButtonRelease : {
            if ( bt->pt[0x00] && bt->pt[0x01] ) {
                /*** Must not be the same vertices ***/
                if ( bt->pt[0x00] != bt->pt[0x01] ) {
                    /*** must not already belong to 2 segment either ***/
                    if ( ( bt->pt[0x00]->nbseg != 0x02 ) &&
                         ( bt->pt[0x01]->nbseg != 0x02 ) ) { 
                        G3DSPLINE *spline = ( G3DSPLINE * ) obj;
                        G3DCUBICSEGMENT *seg;

                        seg = g3dcubicsegment_new ( bt->pt[0x00],
                                                    bt->pt[0x01],
                                                    0.0f, 0.0f, 0.0f,
                                                    0.0f, 0.0f, 0.0f );

                        g3durm_spline_addSegment ( urm,
                                                   spline,
                                                   seg,
                                                   flags,
                                                   REDRAWVIEW );
                    }
                }

                bt->pt[0x00] = NULL;
                bt->pt[0x01] = NULL;
            }

            /*** end drawing ***/
            bt->draw = 0x00;
            bt->obj = NULL;
        } return REDRAWVIEW;

        default :
        break;
    }

    return FALSE;
}

/******************************************************************************/
static int bridge_mesh  ( G3DMESH      *mes,
                          G3DMOUSETOOL *mou, 
                          G3DSCENE     *sce, 
                          G3DCAMERA    *cam,
                          G3DURMANAGER *urm, 
                          uint32_t      flags,
                          G3DEvent     *event ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    G3DMOUSETOOLBRIDGE *bt = ( G3DMOUSETOOLBRIDGE * ) mou;
    G3DVERTEX **ver = bt->ver;
    static GLint VPX[0x04];
    static G3DPICKTOOL ptool = { .coord = { 0 },
                                 .only_visible = 0x01,
                                 .weight = 0.0f,
                                 .radius = 0x08 };

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            /*** Start drawing ***/
            bt->draw = 0x01;

            glGetIntegerv ( GL_VIEWPORT, VPX );

            /*** Selection rectangle ***/
            /*** Selection rectangle ***/
            ptool.coord[0x00] = bev->x;
            ptool.coord[0x01] = VPX[0x03] - bev->y;
            ptool.coord[0x02] = ptool.coord[0x00];
            ptool.coord[0x03] = ptool.coord[0x01];

            if ( !ver[0x00] && !ver[0x01] && !ver[0x02] && !ver[0x03] ) {
                g3dmesh_unselectAllVertices ( mes );
            }

            pick_Item ( &ptool, sce, cam, 0x00, flags );

            /*** remember our Mesh for the drawing part ***/
            /*** because we need its world matrix.      ***/
            bt->obj = obj;

            /*** if any selected vertex ***/
            if ( mes->lselver ) {
                if ( ver[0x00] ) {
                    ver[0x02] = g3dmesh_getLastSelectedVertex ( mes );
                } else {
                    ver[0x00] = g3dmesh_getLastSelectedVertex ( mes );
                }
            } else {
            /*** reset if click fails to pick a vertex ***/
                ver[0x00] = ver[0x01] = ver[0x02] = ver[0x03] = NULL;
            }
        } return REDRAWVIEW;

        case G3DMotionNotify : {
            G3DMotionEvent *bev = ( G3DMotionEvent * ) event;

            if ( bev->state & G3DButton1Mask ) {
                /*** Selection rectangle ***/
                ptool.coord[0x00] = bev->x;
                ptool.coord[0x01] = VPX[0x03] - bev->y;
                ptool.coord[0x02] = ptool.coord[0x00];
                ptool.coord[0x03] = ptool.coord[0x01];

                pick_Item ( &ptool, sce, cam, 0x00, VIEWVERTEX );

                if ( mes->lselver ) {
                    if ( ver[0x02] ) {
                        ver[0x03] = g3dmesh_getLastSelectedVertex ( mes );

                        g3dmesh_unselectAllVertices ( mes );
                        g3dmesh_selectVertex ( mes, ver[0x02] );
                        g3dmesh_selectVertex ( mes, ver[0x03] );
                    } else {
                        if ( ver[0x00] ) {
                            ver[0x01] = g3dmesh_getLastSelectedVertex ( mes );

                            g3dmesh_unselectAllVertices ( mes );
                            g3dmesh_selectVertex ( mes, ver[0x00] );
                            g3dmesh_selectVertex ( mes, ver[0x01] );
                        }
                    }
                }
            }
        } return REDRAWVIEW;

        case G3DButtonRelease : {
            if ( ver[0x00] && ver[0x01] && 
                 ver[0x02] && ver[0x03] ) {
                /*** Must not be the same vertices ***/
                if ( ( ( ver[0x00] != ver[0x02] ) && 
                       ( ver[0x01] != ver[0x03] ) ) ||
                     ( ( ver[0x00] != ver[0x03] ) && 
                       ( ver[0x01] != ver[0x02] ) ) ) {
                    G3DMESH *mes = ( G3DMESH * ) obj;
                    G3DFACE *fac = NULL;
                    G3DVERTEX *tmpver2 = ver[0x02],
                              *tmpver3 = ver[0x03];

                    /*** Triangle - ver[3] differs from the first 2 vertices ***/
                    if ( ( ver[0x02] == ver[0x00] ) ||
                         ( ver[0x02] == ver[0x01] ) ) {
                        ver[0x02] = ver[0x03];
                        ver[0x03] = NULL;
                    }

                    /*** Triangle - ver[2] differs from the first 2 vertices ***/
                    if ( ( ver[0x03] == ver[0x00] ) ||
                         ( ver[0x03] == ver[0x01] ) ) {
                        ver[0x03] = NULL;
                    }

                    /*** if ver[0x03] != NULL, then we did not create any ***/ 
                    /*** triangle. Then, create a QUAD ***/
                    if ( ver[0x03] ) {
                        fac = g3dquad_new ( ver[0x00], ver[0x01],
                                            ver[0x03], ver[0x02] );
                    } else {
                        fac = g3dtriangle_new ( ver[0x00], ver[0x01],
                                                           ver[0x02] );
                    }

                    g3dmesh_addFace ( mes, fac );

                    if ( g3dface_checkOrientation ( fac ) ) {
                        g3dface_invertNormal ( fac );
                    }

                    g3durm_mesh_createFace ( urm, mes, fac, REDRAWVIEW );

                    /*** regenerate subdivision buffer ***/
                    g3dmesh_update ( mes, NULL,
                                          NULL,
                                          NULL,
                                          UPDATEFACEPOSITION |
                                          UPDATEFACENORMAL   |
                                          UPDATEVERTEXNORMAL |
                                          RESETMODIFIERS, flags );

                    /*** be ready for another bridging ***/
                    ver[0x00] = tmpver2;
                    ver[0x01] = tmpver3;

                    ver[0x02] = NULL;
                    ver[0x03] = NULL;
                }
            }

            /*** end drawing ***/
            bt->draw = 0x00;
            bt->obj = NULL;
        } return REDRAWVIEW;

        default :
        break;
    }

    return FALSE;
}

/******************************************************************************/
static int bridge_tool  ( G3DMOUSETOOL *mou, 
                          G3DSCENE     *sce, 
                          G3DCAMERA    *cam,
                          G3DURMANAGER *urm, 
                          uint32_t      eflags,
                          G3DEvent     *event ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        if ( eflags & VIEWVERTEX ) {
            if ( obj->type == G3DMESHTYPE ) {
                G3DMESH *mes = ( G3DMESH * ) obj;

                return bridge_mesh ( mes, mou, sce, cam, urm, eflags, event );
            }

            if ( obj->type & SPLINE ) {
                G3DSPLINE *spl = ( G3DSPLINE * ) obj;

                return bridge_spline ( obj, mou, sce, cam, urm, eflags, event );
            }
        }
    }

    return FALSE;
}
