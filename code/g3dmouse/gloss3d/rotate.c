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
#include <g3dmouse.h>

/******************************************************************************/
/* each function must return FALSE for redrawing the OGL Widget it belongs to */
/* only or TRUE to redraw all OGL Widgets                                     */
/******************************************************************************/

static int rotateUV_tool ( G3DMOUSETOOL *mou, 
                           G3DSCENE     *sce, 
                           G3DCAMERA    *cam,
                           G3DURMANAGER *urm,
                           uint64_t engine_flags, 
                           G3DEvent     *event );
static int rotate_tool ( G3DMOUSETOOL *mou, 
                         G3DSCENE     *sce,
                         G3DCAMERA    *cam,
                         G3DURMANAGER *urm, 
                         uint64_t engine_flags, 
                         G3DEvent    *event );

/******************************************************************************/
G3DMOUSETOOLROTATE *g3dmousetoolrotate_new ( ) {
    uint32_t structsize = sizeof ( G3DMOUSETOOLROTATE );
    void *memarea = calloc ( 0x01, structsize );
    G3DMOUSETOOLROTATE *rt =  ( G3DMOUSETOOLROTATE * ) memarea;

    if ( rt == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( rt,
                        ROTATETOOL,
                        's',
                        NULL,
                        NULL,
                        NULL,
                        rotate_tool,
                        0x00 );

    return rt;
}

/******************************************************************************/
G3DMOUSETOOLROTATEUV *g3dmousetoolrotateUV_new ( ) {
    uint32_t structsize = sizeof ( G3DMOUSETOOLROTATEUV );
    void *memarea = calloc ( 0x01, structsize );
    G3DMOUSETOOLROTATEUV *rt =  ( G3DMOUSETOOLROTATEUV * ) memarea;

    if ( rt == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( rt,
                        ROTATEUVTOOL,
                        's',
                        NULL,
                        NULL,
                        NULL,
                        rotateUV_tool,
                        0x00 );

    return rt;
}

/******************************************************************************/
static int rotate_spline ( G3DSPLINE    *spl,
                           G3DMOUSETOOL *mou, 
                           G3DSCENE     *sce, 
                           G3DCAMERA    *cam,
                           G3DURMANAGER *urm,
                           uint64_t engine_flags, 
                           G3DEvent     *event ) {
    static double orix, oriy, oriz, newx, newy, newz,
                  winx, winy, winz;
    static double mouseXpress, mouseYpress;
    static GLdouble MVX[0x10], PJX[0x10];
    static GLint VPX[0x04];
    G3DOBJECT *obj = ( G3DOBJECT * ) spl;
    static URMMOVEPOINT *ump;

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            glMatrixMode ( GL_PROJECTION );
            glPushMatrix ( );
            glLoadIdentity ( );
            g3dcamera_project ( cam, engine_flags );

            glMatrixMode ( GL_MODELVIEW );
            glPushMatrix ( );
            g3dcamera_view ( cam, HIDEGRID ); 
            glMultMatrixd ( obj->wmatrix );

            glGetDoublev  ( GL_MODELVIEW_MATRIX,  MVX );
            glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
            glGetIntegerv ( GL_VIEWPORT, VPX );

            glPopMatrix ( );
            glMatrixMode ( GL_PROJECTION );
            glPopMatrix ( );

            mouseXpress = bev->x;
            mouseYpress = bev->y;

            if ( engine_flags & VIEWVERTEX ) {
                G3DMOUSETOOLPICK pt = { .coord = { bev->x, VPX[0x03] - bev->y,
                                                   bev->x, VPX[0x03] - bev->y },
                                        .only_visible = 0x01,
                                        .weight = 0.0f,
                                        .radius = PICKMINRADIUS };
                uint32_t ctrlClick = ( bev->state & G3DControlMask ) ? 1 : 0;

                /*** simulate click and release ***/
                pick_Item ( &pt, sce, cam, ctrlClick, engine_flags );

                /*** MUST be called after pick_item because based on ***/
                /*** currently selected points ***/
                ump = g3durm_spline_movePoint ( urm, 
                                                spl, 
                                                UMPSAVESELECTEDPOINTS |
                                                UMPSAVECURRENTHANDLE,
                                                REDRAWVIEW );
                urmmovepoint_saveState ( ump, UMPSAVESTATEBEFORE );

                gluProject ( 0.0f, 0.0f, 0.0f, MVX, PJX, VPX, &winx, &winy, &winz );
                gluUnProject ( ( GLdouble ) bev->x,
                               ( GLdouble ) VPX[0x03] - bev->y,
                               ( GLdouble ) winz,
                               MVX, PJX, VPX,
                               &orix, &oriy, &oriz );
            }
        } return REDRAWVIEW;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;

            if ( mev->state & G3DButton1Mask ) {
                if ( engine_flags & VIEWVERTEX ) {
                    LIST *ltmppt = spl->curve->lselpt;
                    double difx, dify, difz;

                    gluUnProject ( ( GLdouble ) mev->x,
                               ( GLdouble ) VPX[0x03] - mev->y,
                               ( GLdouble ) winz,
                               MVX, PJX, VPX,
                               &newx, &newy, &newz );

                    difx = ( newx - orix );
                    dify = ( newy - oriy );
                    difz = ( newz - oriz );

                    if ( spl->curve->curhan ) {
                        spl->curve->curhan->pos.x += difx;
                        spl->curve->curhan->pos.y += dify;
                        spl->curve->curhan->pos.z += difz;
                    }

                    while ( ltmppt ) {
                        G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) ltmppt->data;

                        pt->pos.x += difx;
                        pt->pos.y += dify;
                        pt->pos.z += difz;

                        ltmppt = ltmppt->next;
                    }

                	g3dspline_update ( spl,
                                       NULL,
                                       UPDATEMODIFIERS, engine_flags );

                    orix = newx;
                    oriy = newy;
                    oriz = newz;
                }
            }
        } return REDRAWVIEW;

        case G3DButtonRelease : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            if ( engine_flags & VIEWVERTEX ) {
                urmmovepoint_saveState ( ump, UMPSAVESTATEAFTER );
            }

            spl->curve->curhan = NULL;
        } return REDRAWVIEW;

        default :
        break;
    }

    return FALSE;
}

/******************************************************************************/
static int rotateUV_tool ( G3DMOUSETOOL *mou, 
                           G3DSCENE     *sce, 
                           G3DCAMERA    *cam,
                           G3DURMANAGER *urm,
                           uint64_t engine_flags, 
                           G3DEvent     *event ) {
    static double orix, oriy, oriz, newx, newy, newz,
                  winx, winy, winz;
    static double mouseXpress, mouseYpress;
    G3DOBJECT *obj = ( G3DOBJECT * ) g3dscene_getLastSelected ( sce );
    static int VPX[0x04];
    static float widthFactor;
    static float heightFactor;
    static int xold, yold;
    static G3DUV *olduv, *newuv;
    static uint32_t nbuv;
    static G3DUV cenuv;
    static LIST *lseluv;

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            /*** try the first texture in case no texture is selected ***/
            if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

            if ( tex ) {
                G3DUVMAP *uvmap = tex->map;

                if ( uvmap ) {
                    switch ( event->type ) {
                        case G3DButtonPress : {
                            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

                            mouseXpress = xold = bev->x;
                            mouseYpress = yold = bev->y;

                            if ( engine_flags & VIEWVERTEXUV ) lseluv = list_copy ( uvmap->lseluv );
                            if ( engine_flags & VIEWFACEUV   ) lseluv = g3duvset_getUVsFromList ( uvmap->lseluvset );

                            g3duv_copyUVFromList     ( lseluv, &olduv );
                            g3duv_getAverageFromList ( lseluv, &cenuv );

                            glGetIntegerv ( GL_VIEWPORT, VPX );

                            /* Note: cam->obj.sca.z = cam->obj.sca.x = cam->obj.sca.y */
                        } return REDRAWVIEW;

                        case G3DMotionNotify : {
                            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;

                            if ( mev->state & G3DButton1Mask ) {
                                if ( ( engine_flags & VIEWVERTEXUV ) ||
                                     ( engine_flags & VIEWFACEUV   ) ) {
                                    float angle = ( float ) ( mev->x - xold ) * 0.005f;
                                    /*** Build the 2D rotation matrix ***/
                                    float RTX[0x04] = { cos ( angle ), - sin ( angle ),
                                                        sin ( angle ),   cos ( angle ) };
                                    LIST *ltmpuv = lseluv;
                                    int i = 0x00;

                                    while ( ltmpuv ) {
                                        G3DUV *uv = ( G3DUV * ) ltmpuv->data;
                                        G3DUV vec = { .u = olduv[i].u - cenuv.u,
                                                      .v = olduv[i].v - cenuv.v };
                                        /** Perform Matrix multiplication ***/
                                        G3DUV vrt = { .u = ( vec.u * RTX[0x00] ) + 
                                                           ( vec.v * RTX[0x01] ),
                                                      .v = ( vec.u * RTX[0x02] ) +
                                                           ( vec.v * RTX[0x03] ) };

                                        uv->u = cenuv.u + vrt.u;
                                        uv->v = cenuv.v + vrt.v;

                                        i++;

                                        ltmpuv = ltmpuv->next;
                                    }
                                }
                            }
                        } return REDRAWUVMAPEDITOR;

                        case G3DButtonRelease : {
                            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

                            /*** simulate click and release ***/
                            if ( ( bev->x == mouseXpress ) && 
                                 ( bev->y == mouseYpress ) ) {
                                G3DMOUSETOOLPICKUV pt = { .coord = { bev->x, VPX[0x03] - bev->y,
                                                                     bev->x, VPX[0x03] - bev->y },
                                                          .only_visible = 0x00,
                                                          .weight = 0.0f,
                                                          .radius = PICKMINRADIUS };

                                pickUV_tool ( &pt, sce, cam, urm, engine_flags, event );

                                /*** cancel arrays allocated for undo-redo ***/
                                if ( olduv ) free ( olduv );
                            } else {
                                g3duv_copyUVFromList ( lseluv, &newuv );

                                g3durm_uvmap_moveUVList ( urm,
                                                          uvmap, 
                                                          lseluv,
                                                          olduv, 
                                                          newuv, 
                                                          REDRAWVIEW |
                                                          REDRAWUVMAPEDITOR );
                            }

                            list_free ( &lseluv, NULL );

                            /** TODO: do this only for subdivided meshes ***/
                            g3dmesh_update ( ((G3DOBJECT*)uvmap)->parent, 
                                             NULL,
                                             NULL,
                                             NULL,
                                             RESETMODIFIERS, 
                                             engine_flags );

                            olduv = newuv = NULL;
                        } return REDRAWVIEW            | 
                                 REDRAWCOORDS          | 
                                 BUFFEREDSUBDIVISIONOK | 
                                 REDRAWCURRENTOBJECT   | 
                                 REDRAWUVMAPEDITOR;

                        default :
                        break;
                    }
                }
            }
        }
    }

    return FALSE;
}

/******************************************************************************/
static int rotate_morpher ( G3DMORPHER       *mpr,
                            G3DMOUSETOOL     *mou, 
                            G3DSCENE         *sce, 
                            G3DCAMERA        *cam,
                            G3DURMANAGER     *urm,
                            uint64_t engine_flags, 
                            G3DEvent         *event ) {
    static double orix, oriy, oriz, newx, newy, newz,
                  winx, winy, winz;
    static double mouseXpress, mouseYpress;
    static GLdouble MVX[0x10], PJX[0x10];
    static GLint VPX[0x04];
    G3DOBJECT *obj = ( G3DOBJECT * ) mpr;
    static LIST *lver, *lfac, *ledg;
    static G3DVECTOR *oldpos;
    static G3DVECTOR *newpos;
    static G3DVECTOR pivot;

    if ( obj->parent->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) obj->parent;

        if ( ( mpr->selmpose ) && 
             ( mes->lselver  ) ) {
            switch ( event->type ) {
                case G3DButtonPress : {
                    G3DButtonEvent *bev = ( G3DButtonEvent * ) event;
                    G3DVECTOR avgpos;

                    glMatrixMode ( GL_PROJECTION );
                    glPushMatrix ( );
                    glLoadIdentity ( );
                    g3dcamera_project ( cam, engine_flags );

                    glMatrixMode ( GL_MODELVIEW );
                    glPushMatrix ( );
                    g3dcamera_view ( cam, HIDEGRID );
                    glMultMatrixd ( obj->wmatrix );

                    glGetDoublev  ( GL_MODELVIEW_MATRIX,  MVX );
                    glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
                    glGetIntegerv ( GL_VIEWPORT, VPX );

                    glPopMatrix ( );
                    glMatrixMode ( GL_PROJECTION );
                    glPopMatrix ( );

                    mouseXpress = orix = bev->x;
                    mouseYpress = bev->y;

                    if ( engine_flags & VIEWVERTEX ) {
                        lver = g3dmesh_getVertexListFromSelectedVertices ( mes );
                    }

                    g3dvertex_getAveragePositionFromList ( lver, &pivot );

                    oldpos = g3dmorpher_getMeshPoseArrayFromList ( mpr, 
                                                                   NULL, 
                                                                   lver );

                    /*g3dvertex_copyPositionFromList       ( lver, &oldpos );

                    lfac = g3dvertex_getFacesFromList  ( lver );
                    ledg = g3dface_getEdgesFromList    ( lfac );

                    g3dobject_startUpdateModifiers_r ( mes, engine_flags );*/
                } return REDRAWVIEW;

                case G3DMotionNotify : {
                    G3DMotionEvent *mev = ( G3DMotionEvent * ) event;

                    newx = mev->x;

                    if ( mev->state & G3DButton1Mask ) {
                        if ( ( engine_flags & VIEWVERTEX ) ||
                             ( engine_flags & VIEWEDGE   ) ||
                             ( engine_flags & VIEWFACE   ) ) {
                            G3DVECTOR *axis = sce->csr.axis;
                            LIST *ltmpver = lver;
                            double difx, dify, difz;
                            double ROTX[0x10];

                            if ( ( engine_flags & XAXIS ) && axis[0].w ) difx = ( newx - orix );
                            if ( ( engine_flags & YAXIS ) && axis[1].w ) dify = ( newx - orix );
                            if ( ( engine_flags & ZAXIS ) && axis[2].w ) difz = ( newx - orix );

                            glMatrixMode ( GL_MODELVIEW );
                            glPushMatrix ( );
                            glLoadIdentity ( );
                            glTranslatef ( pivot.x, pivot.y, pivot.z );
                            glRotatef ( difx, 1.0f, 0.0f, 0.0f );
                            glRotatef ( dify, 0.0f, 1.0f, 0.0f );
                            glRotatef ( difz, 0.0f, 0.0f, 1.0f );
                            glTranslatef ( -pivot.x, -pivot.y, -pivot.z );
                            glGetDoublev ( GL_MODELVIEW_MATRIX, ROTX );
                            glPopMatrix ( );

                            while ( ltmpver ) {
                                G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                                G3DMORPHERVERTEXPOSE *vpose;

                                vpose = g3dmorpher_getVertexPose ( mpr, ver, NULL, NULL );

                                if ( vpose ) {
                                    G3DVECTOR verpos = { .x = vpose->pos.x,
                                                         .y = vpose->pos.y,
                                                         .z = vpose->pos.z, 1.0f };
                                    G3DVECTOR newpos;

                                    g3dvector_matrix ( &verpos, ROTX, &newpos );

                                    vpose->pos.x = newpos.x;
                                    vpose->pos.y = newpos.y;
                                    vpose->pos.z = newpos.z;

                                    /*if ( ver->flags & VERTEXSKINNED ) {
                                        g3dvertex_computeSkinnedPosition ( ver );
                                    }*/
                                }

                                ltmpver = ltmpver->next;
                            }

                            orix = newx;
                        }
                    }
                } return REDRAWVIEW;

                case G3DButtonRelease : {
                    G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

                    /*** simulate click and release ***/
                    if ( ( bev->x == mouseXpress ) && 
                         ( bev->y == mouseYpress ) ) {
                        G3DMOUSETOOLPICK pt = { .coord = { bev->x, VPX[0x03] - bev->y,
                                                           bev->x, VPX[0x03] - bev->y },
                                                .only_visible = 0x01,
                                                .weight = 0.0f,
                                                .radius = PICKMINRADIUS };

                        pick_tool ( &pt, sce, cam, urm, engine_flags, event );
                    }

                    newpos = g3dmorpher_getMeshPoseArrayFromList ( mpr, 
                                                                   NULL, 
                                                                   lver );

                    g3durm_morpher_moveVertices ( urm,
                                                  mpr,
                                                  mpr->selmpose,
                                                  lver,
                                                  oldpos, 
                                                  newpos, 
                                                  REDRAWVIEW );

                    list_free ( &lver, NULL );

                    oldpos = newpos = NULL;
                } return REDRAWVIEW            | 
                         REDRAWCOORDS          | 
                         BUFFEREDSUBDIVISIONOK | 
                         REDRAWCURRENTOBJECT   | 
                         REDRAWUVMAPEDITOR;

                default :
                break;
            }
        }
    }

    return FALSE;
}

/******************************************************************************/
static int rotate_mesh ( G3DMESH          *mes,
                         G3DMOUSETOOL     *mou, 
                         G3DSCENE         *sce, 
                         G3DCAMERA        *cam,
                         G3DURMANAGER     *urm,
                         uint64_t engine_flags, 
                         G3DEvent         *event ) {
    static double orix, oriy, oriz, newx, newy, newz,
                  winx, winy, winz;
    static double mouseXpress, mouseYpress;
    static GLdouble MVX[0x10], PJX[0x10];
    static GLint VPX[0x04];
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    static LIST *lver, *lfac, *ledg;
    static G3DVECTOR *oldpos;
    static G3DVECTOR *newpos;
    static G3DVECTOR pivot;

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;
            G3DVECTOR avgpos;

            glMatrixMode ( GL_PROJECTION );
            glPushMatrix ( );
            glLoadIdentity ( );
            g3dcamera_project ( cam, engine_flags );

            glMatrixMode ( GL_MODELVIEW );
            glPushMatrix ( );
            g3dcamera_view ( cam, HIDEGRID );
            glMultMatrixd ( obj->wmatrix );

            glGetDoublev  ( GL_MODELVIEW_MATRIX,  MVX );
            glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
            glGetIntegerv ( GL_VIEWPORT, VPX );

            glPopMatrix ( );
            glMatrixMode ( GL_PROJECTION );
            glPopMatrix ( );

            mouseXpress = orix = bev->x;
            mouseYpress = bev->y;

            if ( engine_flags & VIEWVERTEX ) {
                lver = g3dmesh_getVertexListFromSelectedVertices ( mes );
            }

            if ( engine_flags & VIEWEDGE ) {
                lver = g3dmesh_getVertexListFromSelectedEdges ( mes );
            }

            if ( engine_flags & VIEWFACE ) {
                lver = g3dmesh_getVertexListFromSelectedFaces ( mes );
            }

            g3dvertex_getAveragePositionFromList ( lver, &pivot );
            g3dvertex_copyPositionFromList       ( lver, &oldpos );
 
            lfac = g3dvertex_getFacesFromList  ( lver );
            ledg = g3dface_getEdgesFromList    ( lfac );

            g3dobject_startUpdateModifiers_r ( mes, engine_flags );
        } return REDRAWVIEW;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;

            newx = mev->x;

            if ( mev->state & G3DButton1Mask ) {
                if ( ( engine_flags & VIEWVERTEX ) ||
                     ( engine_flags & VIEWEDGE   ) ||
                     ( engine_flags & VIEWFACE   ) ) {
                    G3DVECTOR *axis = sce->csr.axis;
                    LIST *ltmpver = lver;
                    double difx, dify, difz;
                    double ROTX[0x10];

                    if ( ( engine_flags & XAXIS ) && axis[0].w ) difx = ( newx - orix );
                    if ( ( engine_flags & YAXIS ) && axis[1].w ) dify = ( newx - orix );
                    if ( ( engine_flags & ZAXIS ) && axis[2].w ) difz = ( newx - orix );

                    glMatrixMode ( GL_MODELVIEW );
                    glPushMatrix ( );
                    glLoadIdentity ( );
                    glTranslatef ( pivot.x, pivot.y, pivot.z );
                    glRotatef ( difx, 1.0f, 0.0f, 0.0f );
                    glRotatef ( dify, 0.0f, 1.0f, 0.0f );
                    glRotatef ( difz, 0.0f, 0.0f, 1.0f );
                    glTranslatef ( -pivot.x, -pivot.y, -pivot.z );
                    glGetDoublev ( GL_MODELVIEW_MATRIX, ROTX );
                    glPopMatrix ( );

                    while ( ltmpver ) {
                        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                        G3DVECTOR verpos = { .x = ver->pos.x,
                                             .y = ver->pos.y,
                                             .z = ver->pos.z, 1.0f };
                        G3DVECTOR newpos;

                        g3dvector_matrix ( &verpos, ROTX, &newpos );

                        ver->pos.x = newpos.x;
                        ver->pos.y = newpos.y;
                        ver->pos.z = newpos.z;

                        if ( obj->parent->childvertexchange ) {
                            obj->parent->childvertexchange ( obj->parent,
                                                             obj, ver );
                        }

                        if ( ver->flags & VERTEXSKINNED ) {
                            g3dvertex_computeSkinnedPosition ( ver );
                        }

                        ltmpver = ltmpver->next;
                    }

                    g3dobject_updateModifiers_r ( mes, engine_flags );

                    if ( mes->onGeometryMove ) {
                        mes->onGeometryMove ( mes, lver, 
                                                   ledg, 
                                                   lfac, 
                                                   engine_flags );
                    }

                    orix = newx;
                }
            }
        } return REDRAWVIEW;

        case G3DButtonRelease : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            /*** simulate click and release ***/
            if ( ( bev->x == mouseXpress ) && 
                 ( bev->y == mouseYpress ) ) {
                G3DMOUSETOOLPICK pt = { .coord = { bev->x, VPX[0x03] - bev->y,
                                                   bev->x, VPX[0x03] - bev->y },
                                        .only_visible = 0x01,
                                        .weight = 0.0f,
                                        .radius = PICKMINRADIUS };

                pick_tool ( &pt, sce, cam, urm, engine_flags, event );
            }

            g3dvertex_copyPositionFromList ( lver, &newpos );

            g3durm_mesh_moveVertexList ( urm, mes, lver, ledg, lfac, NULL,
                                         oldpos, 
                                         newpos, 
                                         REDRAWVIEW );

            g3dmesh_updateBbox ( mes );

            g3dobject_endUpdateModifiers_r ( mes, engine_flags );

            list_free ( &lver, NULL );
            list_free ( &lfac, NULL );
            list_free ( &ledg, NULL );

            oldpos = newpos = NULL;
        } return REDRAWVIEW            | 
                 REDRAWCOORDS          | 
                 BUFFEREDSUBDIVISIONOK | 
                 REDRAWCURRENTOBJECT   | 
                 REDRAWUVMAPEDITOR;

        default :
        break;
    }

    return FALSE;
}

/******************************************************************************/
static int rotate_object ( LIST        *lobj,
                           G3DMOUSETOOL *mou, 
                           G3DSCENE     *sce, 
                           G3DCAMERA    *cam,
                           G3DURMANAGER *urm,
                           uint64_t engine_flags, 
                           G3DEvent     *event ) {
    static double orix, oriy, oriz, newx, newy, newz,
                  winx, winy, winz;
    static double mouseXpress, mouseYpress;
    static GLdouble MVX[0x10], PJX[0x10];
    static GLint VPX[0x04];
    static LIST *lver, *lfac, *ledg;
    static G3DVECTOR lvecx, lvecy, lvecz, pivot;
    static G3DDOUBLEVECTOR startpos; /*** world original pivot ***/
    static uint32_t nbobj;
    static URMTRANSFORMOBJECT *uto;
    static double PREVWMVX[0x10];

    static G3DVECTOR xaxis, yaxis, zaxis;

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            /*** Record and undo procedure and record the current state ***/
            uto = g3durm_object_transform ( urm,
                                            lobj,
                                            UTOSAVEROTATION, 
                                            REDRAWVIEW );

            urmtransform_saveState ( uto, UTOSAVESTATEBEFORE );
            /***/

            g3dscene_getSelectionPosition ( sce, &pivot );

            mouseXpress = orix = bev->x;
            mouseYpress = bev->y;

            glMatrixMode ( GL_PROJECTION );
            glPushMatrix ( );
            glLoadIdentity ( );
            g3dcamera_project ( cam, engine_flags );

            glMatrixMode ( GL_MODELVIEW );
            glPushMatrix ( );
            g3dcamera_view ( cam, HIDEGRID );

            glGetDoublev  ( GL_MODELVIEW_MATRIX,  MVX );
            glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
            glGetIntegerv ( GL_VIEWPORT, VPX );

            glPopMatrix ( );
            glMatrixMode ( GL_PROJECTION );
            glPopMatrix ( );

            nbobj = list_count ( lobj );

            if ( nbobj == 0x01 ) {
                G3DOBJECT *obj = ( G3DOBJECT * ) lobj->data;
                /*** PREVWMX is used in VIEWAXIS mode to move vertices back ***/
                memcpy ( PREVWMVX, obj->wmatrix, sizeof ( double ) * 0x10 );
            }
        } return REDRAWVIEW;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;
            LIST *ltmpobj = lobj;

            if ( mev->state & G3DButton1Mask ) {
                G3DVECTOR dif = { 0.0f, 0.0f, 0.0f }; /** local pivot ***/
                G3DVECTOR *axis = &sce->csr.axis;
                G3DDOUBLEVECTOR endpos;
                double ROTX[0x10];

                if ( ( engine_flags & XAXIS ) && axis[0x00].w ) dif.x = ( mev->x - orix );
                if ( ( engine_flags & YAXIS ) && axis[0x01].w ) dif.y = ( mev->x - orix );
                if ( ( engine_flags & ZAXIS ) && axis[0x02].w ) dif.z = ( mev->x - orix );

                if ( nbobj > 0x01 ) {
                    glMatrixMode ( GL_MODELVIEW );
                    glPushMatrix ( );
                    glLoadIdentity ( );
                    glTranslatef ( pivot.x, pivot.y, pivot.z );
                    glRotatef ( dif.x, 1.0f, 0.0f, 0.0f );
                    glRotatef ( dif.y, 0.0f, 1.0f, 0.0f );
                    glRotatef ( dif.z, 0.0f, 0.0f, 1.0f );
                    glTranslatef ( -pivot.x, -pivot.y, -pivot.z );
                    glGetDoublev ( GL_MODELVIEW_MATRIX, ROTX );
                    glPopMatrix ( );
                }

                while ( ltmpobj ) {
                    G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;
                    G3DDOUBLEVECTOR lstartpos, lendpos;

                    if ( ( obj->flags & OBJECTNOROTATION ) == 0x00 ) {
                        if ( nbobj == 0x01 ) {
                            glPushMatrix ( );
                            glLoadMatrixd ( obj->rmatrix );

                            glRotatef     ( dif.x, 1.0f, 0.0f, 0.0f );
                            glRotatef     ( dif.y, 0.0f, 1.0f, 0.0f );
                            glRotatef     ( dif.z, 0.0f, 0.0f, 1.0f );

                            glGetDoublev  ( GL_MODELVIEW_MATRIX, obj->rmatrix );

                            g3dcore_getMatrixRotation ( obj->rmatrix, &obj->rot );
/*g3dvector_print ( &dif );*/
                            glPopMatrix ( );
                        } else {
                            G3DVECTOR cenpos = { 0.0f, 0.0f, 0.0f };
                            G3DVECTOR objpos;
                            G3DVECTOR rotpos;
                            G3DVECTOR newpos;

                            g3dvector_matrix ( &cenpos, obj->wmatrix, &objpos );
                            g3dvector_matrix ( &objpos, ROTX, &rotpos );
                            g3dvector_matrix ( &rotpos, obj->parent->iwmatrix, &newpos );

                            obj->pos.x = newpos.x;
                            obj->pos.y = newpos.y;
                            obj->pos.z = newpos.z;
                        }

                        g3dobject_updateMatrix_r ( obj, engine_flags );

                        if ( engine_flags & VIEWAXIS ) {
                            /*** in case this was in VIEWAXIS mode, we move ***/
                            /*** back the vertices were they were before,   ***/
                            /*** in world coord ***/
                            if ( nbobj == 0x01 ) {
                                if ( obj->type == G3DMESHTYPE ) {
                                    G3DMESH *mes = ( G3DMESH * ) obj;

                                    g3dmesh_moveAxis ( mes, PREVWMVX, engine_flags );
                                    memcpy ( PREVWMVX, obj->wmatrix, sizeof ( double ) * 0x10 );
                                }

                                if ( obj->type & SPLINE ) {
                                    G3DSPLINE *spl = ( G3DSPLINE * ) obj;

                                    g3dspline_moveAxis ( spl, PREVWMVX, engine_flags );
                                    memcpy ( PREVWMVX, obj->wmatrix, sizeof ( double ) * 0x10 );
                                }
                            }
                        }
                    }

                    ltmpobj = ltmpobj->next;
                }

                memcpy ( &startpos, &endpos, sizeof ( G3DDOUBLEVECTOR ) );
            }

            orix = mev->x;
        } return REDRAWVIEW | REDRAWCOORDS;

        case G3DButtonRelease : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;
            LIST *ltmpobj = lobj;

            while ( ltmpobj ) {
                G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

                if ( obj->type == G3DMESHTYPE ) {
                    G3DMESH *mes = ( G3DMESH * ) obj;

                    g3dmesh_updateBbox ( mes );
                }

                ltmpobj = ltmpobj->next;
            }

            /*** simulate click and release ***/
            if ( ( bev->x == mouseXpress ) && 
                 ( bev->y == mouseYpress ) ) {
                G3DMOUSETOOLPICK pt = { .coord = { bev->x, VPX[0x03] - bev->y,
                                                   bev->x, VPX[0x03] - bev->y },
                                        .only_visible = 0x01,
                                        .weight = 0.0f,
                                        .radius = PICKMINRADIUS };

                /*** FIRST UNDO the TRANSFORM that we saved at buttonPress ***/
                /*** and that was not used at all ***/
                g3durmanager_undo ( urm, engine_flags );

                pick_tool ( &pt, sce, cam, urm, engine_flags, event );
            } else {
                urmtransform_saveState ( uto, UTOSAVESTATEAFTER );
            }

        } return REDRAWALL;

        default :
        break;
    }

    return FALSE;
}

/******************************************************************************/
static int rotate_tool ( G3DMOUSETOOL *mou, 
                         G3DSCENE     *sce,
                         G3DCAMERA    *cam,
                         G3DURMANAGER *urm, 
                         uint64_t engine_flags, 
                         G3DEvent    *event ) {
    static GLint VPX[0x04];
    static LIST *lver, *lfac, *lsub, *ledg, *ffdlsub, *lvtx;
    static G3DMESHFAC **msftab; /*** list of faces to update from skinning ***/
    static uint32_t nbmsf;
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

    glGetIntegerv ( GL_VIEWPORT, VPX );

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;
            G3DMOUSETOOLPICK pt = { .coord = { bev->x, VPX[0x03] - bev->y,
                                               bev->x, VPX[0x03] - bev->y },
                                    .only_visible = 0x01,
                                    .weight = 0.0f,
                                    .radius = PICKMINRADIUS };

            pick_cursor ( &pt, sce, cam, engine_flags );
        } break;

        default :
        break;
    }

    if ( engine_flags & VIEWOBJECT ) {
        return rotate_object ( sce->lsel, mou, sce, cam, urm, engine_flags, event );
    }

    if ( obj ) {
        if ( engine_flags & VIEWAXIS ) {
            LIST ldummy = { .next = NULL, 
                            .data = obj,
                            .prev = NULL };

            return rotate_object ( &ldummy, mou, sce, cam, urm, engine_flags, event );
        }

        if ( engine_flags & VIEWUVWMAP ) {
            if ( obj->type & MESH ) {
                G3DMESH  *mes = ( G3DMESH * ) obj;
                G3DUVMAP * map = g3dmesh_getSelectedUVMap ( mes );

                if ( map && ( g3duvmap_isFixed ( map ) == 0x00 ) ) {
                    LIST ldummy = { .next = NULL, 
                                    .data = map,
                                    .prev = NULL };

                    return rotate_object ( &ldummy, mou, sce, cam, urm, engine_flags, event );
                }
            }
        }

        if ( ( engine_flags & VIEWVERTEX ) ||
             ( engine_flags & VIEWEDGE   ) ||
             ( engine_flags & VIEWFACE   ) ) {
            if ( obj->type & MORPHER ) {
                G3DMORPHER *mpr = ( G3DMORPHER * ) obj;

                return rotate_morpher ( mpr, mou, sce, cam, urm, engine_flags, event );
            }

            if ( obj->type & SPLINE ) {
                G3DSPLINE *spl = ( G3DSPLINE * ) obj;

                return rotate_spline ( spl, mou, sce, cam, urm, engine_flags, event );
            }

            if ( ( obj->type & MESH ) && ( obj->type & EDITABLE ) ) {
                G3DMESH *mes = ( G3DMESH * ) obj;

                return rotate_mesh ( mes, mou, sce, cam, urm, engine_flags, event );
            }
        }
    }

    return FALSE;
}
