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
                              G3DURMANAGER *urm, 
                              uint64_t      engine_flags );
static void bridge_draw ( G3DMOUSETOOL *mou, 
                          G3DSCENE     *sce, 
                          G3DCAMERA    *cam,
                          G3DENGINE    *engine,
                          uint64_t      engine_flags );
static int bridge_event ( G3DMOUSETOOL *mou, 
                          G3DSCENE     *sce, 
                          G3DCAMERA    *cam,
                          G3DURMANAGER *urm, 
                          uint64_t      engine_flags,
                          G3DEvent     *event );

/******************************************************************************/
G3DMOUSETOOLBRIDGE *g3dmousetoolbridge_new ( ) {
    uint32_t structsize = sizeof ( G3DMOUSETOOLBRIDGE );

    G3DMOUSETOOLBRIDGE *bt =  ( G3DMOUSETOOLBRIDGE * ) calloc ( 0x04, structsize );

    if ( bt == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( ( G3DMOUSETOOL * ) bt,
                        BRIDGETOOL,
                        's',
                        NULL,
                        bridge_init,
                        bridge_draw,
                        bridge_event,
                        0x00 );

    return bt;
}

/******************************************************************************/
static uint32_t bridge_init ( G3DMOUSETOOL *mou, 
                              G3DSCENE     *sce, 
                              G3DURMANAGER *urm, 
                              uint64_t      engine_flags ) {
    G3DMOUSETOOLBRIDGE *bt = ( G3DMOUSETOOLBRIDGE * ) mou;

    bt->pickedVertex[0x00] = 
    bt->pickedVertex[0x01] = 
    bt->pickedVertex[0x02] = 
    bt->pickedVertex[0x03] = NULL;

    bt->pt[0x00]  = 
    bt->pt[0x01]  = NULL;

    bt->draw = 0x00;
    bt->obj  = NULL;

    return 0x00;
}

/******************************************************************************/
static void bridge_draw ( G3DMOUSETOOL *mou, 
                          G3DSCENE     *sce, 
                          G3DCAMERA    *curcam,
                          G3DENGINE    *engine,
                          uint64_t      engine_flags ) {
    G3DMOUSETOOLBRIDGE *bt = ( G3DMOUSETOOLBRIDGE * ) mou;

    /*if ( bt && bt->draw ) {*/
        if ( bt->obj ) {
            int mvpMatrixLocation = glGetUniformLocation( engine->coloredShaderProgram,
                                                          "mvpMatrix" );
            float mvp[0x10];
            float mvw[0x10];
            uint32_t i;

            g3dcore_multMatrixf( curcam->obj.inverseWorldMatrix,
                                 bt->obj->worldMatrix,
                                 mvw );

            /*** the matrix by which vertices coords are transformed ***/
            g3dcore_multMatrixf( curcam->pmatrix, mvw, mvp );

            glUseProgram( engine->coloredShaderProgram );

            glUniformMatrix4fv( mvpMatrixLocation, 1, GL_FALSE, mvp );

            glPushAttrib( GL_ALL_ATTRIB_BITS );
            glDisable   ( GL_DEPTH_TEST );
            glPointSize ( 3.0f );

            if( bt->hoveredVertex ) {
                SHADERVERTEX vertex = { 0 };

                vertex.pos   = bt->hoveredVertex->pos;
                vertex.col.g = 1.0f;

                g3dengine_drawPoint ( engine, &vertex, 0, engine_flags );
            }

            for( i = 0; i < 4; i++ ) {
                if( bt->pickedVertex[i] ) {
                    SHADERVERTEX vertex = { 0 };

                    vertex.pos   = bt->pickedVertex[i]->pos;
                    vertex.col.g = 1.0f;

                    g3dengine_drawPoint ( engine, &vertex, 0, engine_flags );
                }
            }

            if ( bt->obj->type & MESH ) {
        	    if ( bt->pickedVertex[0x02] && bt->pickedVertex[0x03] ) {
                    SHADERVERTEX vertices[0x02] = { 0 };

                    vertices[0x00].pos   = bt->pickedVertex[0x02]->pos;
                    vertices[0x00].col.g = 1.0f;
                    vertices[0x01].pos   = bt->pickedVertex[0x03]->pos;
                    vertices[0x01].col.g = 1.0f;

                    g3dengine_drawLine ( engine, vertices, 0, engine_flags );
        	    }

        	    if ( bt->pickedVertex[0x00] && bt->pickedVertex[0x01] ) {
                    SHADERVERTEX vertices[0x02] = { 0 };

                    vertices[0x00].pos   = bt->pickedVertex[0x00]->pos;
                    vertices[0x00].col.g = 1.0f;
                    vertices[0x01].pos   = bt->pickedVertex[0x01]->pos;
                    vertices[0x01].col.g = 1.0f;

                    g3dengine_drawLine ( engine, vertices, 0, engine_flags );
        	    }

            }

            if ( bt->obj->type & SPLINE ) {
        	    if ( bt->pt[0x00] && bt->pt[0x01] ) {
                    SHADERVERTEX vertices[0x02] = { 0 };

                    vertices[0x00].pos   = bt->pt[0x00]->pos;
                    vertices[0x00].col.g = 1.0f;
                    vertices[0x01].pos   = bt->pt[0x01]->pos;
                    vertices[0x01].col.g = 1.0f;

                    g3dengine_drawLine ( engine, vertices, 0, engine_flags );
        	    }
            }

            glPopAttrib ( );

            glUseProgram( 0 );
        }
    /*}*/
}

/******************************************************************************/
static int bridge_spline  ( G3DSPLINE    *spl,
                            G3DMOUSETOOL *mou, 
                            G3DSCENE     *sce, 
                            G3DCAMERA    *cam,
                            G3DURMANAGER *urm, 
                            uint64_t      engine_flags,
                            G3DEvent     *event ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) spl;
    G3DMOUSETOOLBRIDGE *bt = ( G3DMOUSETOOLBRIDGE * ) mou;
    static GLint VPX[0x04];
    static G3DMOUSETOOLPICK ptool = { .coord = { 0 },
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

            pick_Item ( &ptool, sce, cam, 0x00, engine_flags );

            /*** remember our object for the drawing part ***/
            /*** because we need its world matrix.      ***/
            bt->obj = ( G3DOBJECT * ) spl;

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
                                                   sce,
                                                   spline,
                            ( G3DSPLINESEGMENT * ) seg,
                                                   engine_flags,
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
                          G3DCAMERA    *curcam,
                          G3DURMANAGER *urm, 
                          uint64_t      engine_flags,
                          G3DEvent     *event ) {
    G3DMOUSETOOLBRIDGE *bt = ( G3DMOUSETOOLBRIDGE * ) mou;
    static GLint VPX[0x04];
    static G3DMOUSETOOLPICK ptool = { .coord = { 0 },
                                      .only_visible = 0x01,
                                      .weight = 0.0f,
                                      .radius = 0x08 };

    bt->obj = G3DOBJECTCAST(mes);

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;
            G3DVERTEX *pickedVertex = pick_vertex( sce,
                                                   curcam,
                                                   mes,
                                                   bev->x,
                                                   curcam->vmatrix[0x03] - bev->y,
                                                   0x08 );

            if( pickedVertex && ( pickedVertex != bt->pickedVertex[0x00] ) ) {
                bt->pickedVertex[0x02] = pickedVertex;

                /*** Start drawing ***/
                bt->draw = 0x01;
            }
        } return REDRAWVIEW;

        case G3DMotionNotify : {
            G3DMotionEvent *bev = ( G3DMotionEvent * ) event;

            bt->hoveredVertex = pick_vertex( sce,
                                             curcam,
                                             mes,
                                             bev->x,
                                             curcam->vmatrix[0x03] - bev->y,
                                             0x08 );

            if ( bt->hoveredVertex ) {
                if ( bev->state & G3DButton1Mask ) {
                    bt->pickedVertex[0x03] = bt->hoveredVertex;
                }
            }
        } return REDRAWVIEW;

        case G3DButtonRelease : {
            if ( bt->pickedVertex[0x00] && bt->pickedVertex[0x01] && 
                 bt->pickedVertex[0x02] && bt->pickedVertex[0x03] ) {
                G3DFACE *fac = g3durm_mesh_createFace ( urm,
                                                        sce,
                                                        mes,
                                                        bt->pickedVertex[0x00],
                                                        bt->pickedVertex[0x01],
                                                        bt->pickedVertex[0x02],
                                                        bt->pickedVertex[0x03],
                                                        engine_flags,
                                                        REDRAWVIEW );
            }

            bt->pickedVertex[0x00] = bt->pickedVertex[0x02];
            bt->pickedVertex[0x01] = bt->pickedVertex[0x03];
            bt->pickedVertex[0x02] = NULL;
            bt->pickedVertex[0x03] = NULL;
            bt->hoveredVertex = NULL;

            /*** end drawing ***/
            bt->draw = 0x00;
        } return REDRAWVIEW;

        default :
        break;
    }

    return FALSE;
}

/******************************************************************************/
static int bridge_event ( G3DMOUSETOOL *mou,
                          G3DSCENE     *sce,
                          G3DCAMERA    *cam,
                          G3DURMANAGER *urm,
                          uint64_t      engine_flags,
                          G3DEvent     *event ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    if ( obj ) {
        if ( engine_flags & VIEWVERTEX ) {
            if ( obj->type == G3DMESHTYPE ) {
                G3DMESH *mes = ( G3DMESH * ) obj;

                return bridge_mesh ( mes, 
                                     mou, 
                                     sce, 
                                     cam, 
                                     urm, 
                                     engine_flags, 
                                     event );
            }

            if ( obj->type & SPLINE ) {
                G3DSPLINE *spl = ( G3DSPLINE * ) obj;

                return bridge_spline ( ( G3DSPLINE * ) obj, 
                                                       mou, 
                                                       sce, 
                                                       cam, 
                                                       urm, 
                                                       engine_flags, 
                                                       event );
            }
        }
    }

    return FALSE;
}
