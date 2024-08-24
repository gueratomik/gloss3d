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

static int scale_tool ( G3DMOUSETOOL *mou, 
                        G3DSCENE     *sce,
                        G3DCAMERA    *cam,
                        G3DURMANAGER *urm, 
                        uint64_t engine_flags, 
                        G3DEvent    *event );
static int scaleUV_tool ( G3DMOUSETOOL *mou, 
                          G3DSCENE     *sce,
                          G3DCAMERA    *cam,
                          G3DURMANAGER *urm, 
                          uint64_t engine_flags, 
                          G3DEvent    *event );

/******************************************************************************/
G3DMOUSETOOLSCALE *g3dmousetoolscale_new ( ) {
    uint32_t structsize = sizeof ( G3DMOUSETOOLSCALE );
    void *memarea = calloc ( 0x01, structsize );
    G3DMOUSETOOLSCALE *sc =  ( G3DMOUSETOOLSCALE * ) memarea;

    if ( sc == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( ( G3DMOUSETOOL * ) sc,
                                           SCALETOOL,
                                           's',
                                           NULL,
                                           NULL,
                                           NULL,
                                           scale_tool,
                                           0x00 );

    sc->only_visible  = 0x01;
    sc->radius        = PICKMINRADIUS;


    return sc;
}

/******************************************************************************/
G3DMOUSETOOLSCALEUV *g3dmousetoolscaleUV_new ( ) {
    uint32_t structsize = sizeof ( G3DMOUSETOOLSCALEUV );
    void *memarea = calloc ( 0x01, structsize );
    G3DMOUSETOOLSCALEUV *sc =  ( G3DMOUSETOOLSCALEUV * ) memarea;

    if ( sc == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( ( G3DMOUSETOOL * ) sc,
                                           SCALEUVTOOL,
                                           's',
                                           NULL,
                                           NULL,
                                           NULL,
                                           scaleUV_tool,
                                           0x00 );

    sc->only_visible  = 0x00;
    sc->radius        = PICKMINRADIUS;

    return sc;
}

/******************************************************************************/
static int scale_spline ( G3DSPLINE    *spl,
                           G3DMOUSETOOL *mou, 
                           G3DSCENE     *sce, 
                           G3DCAMERA    *cam,
                           G3DURMANAGER *urm,
                           uint64_t engine_flags, 
                           G3DEvent     *event ) {
    static double orix, oriy, oriz, newx, newy, newz,
                  winx, winy, winz;
    static double mouseXpress, mouseYpress;
    static float MVX[0x10], PJX[0x10];
    static GLint VPX[0x04];
    G3DOBJECT *obj = ( G3DOBJECT * ) spl;
    static URMMOVEPOINT *ump;

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;


            g3dcore_multMatrixf ( obj->worldMatrix,
                                  cam->obj.inverseWorldMatrix,
                                  MVX );

            glGetIntegerv ( GL_VIEWPORT, VPX );

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

                g3dcore_projectf ( 0.0f,
                                   0.0f,
                                   0.0f,
                                   MVX,
                                   cam->pmatrix,
                                   VPX,
                                  &winx,
                                  &winy,
                                  &winz );

                g3dcore_unprojectf ( ( GLdouble ) bev->x,
                                     ( GLdouble ) VPX[0x03] - bev->y,
                                     ( GLdouble ) winz,
                                                  MVX,
                                                  cam->pmatrix,
                                                  VPX,
                                                 &orix,
                                                 &oriy,
                                                 &oriz );
            }
        } return REDRAWVIEW;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;

            if ( mev->state & G3DButton1Mask ) {
                if ( engine_flags & VIEWVERTEX ) {
                    LIST *ltmppt = spl->curve->lselpt;
                    double difx, dify, difz;

                    g3dcore_unprojectf ( ( GLdouble ) mev->x,
                                         ( GLdouble ) VPX[0x03] - mev->y,
                                         ( GLdouble ) winz,
                                                      MVX,
                                                      cam->pmatrix,
                                                      VPX,
                                                     &newx,
                                                     &newy,
                                                     &newz );

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
int scaleUV_tool ( G3DMOUSETOOL *mou, 
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
                                    float udiff = ( float ) ( mev->x - xold ) * 0.005f,
                                          vdiff = ( float ) ( mev->y - yold ) * 0.005f;
                                    LIST *ltmpuv = lseluv;
                                    int i = 0x00;

                                    while ( ltmpuv ) {
                                        G3DUV *uv = ( G3DUV * ) ltmpuv->data;
                                        G3DUV vec = { .u = olduv[i].u - cenuv.u,
                                                      .v = olduv[i].v - cenuv.v };

                                        uv->u = olduv[i].u - ( vec.u * udiff );
                                        uv->v = olduv[i].v - ( vec.v * vdiff );

                                        i++;

                                        ltmpuv = ltmpuv->next;
                                    }
                                }
                            }
                        } return REDRAWUVMAPEDITOR;

                        case G3DButtonRelease : {
                            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;
                            G3DOBJECT *parobj = ((G3DOBJECT*)uvmap)->parent;
                            G3DMESH   *parmes = ( G3DMESH * ) parobj;

                            /*** simulate click and release ***/
                            if ( ( ( int ) bev->x == ( int ) mouseXpress ) && 
                                 ( ( int ) bev->y == ( int ) mouseYpress ) ) {
                                G3DMOUSETOOLSCALE *sc = ( G3DMOUSETOOLSCALE * ) mou;

                                sc->coord[0x00] = sc->coord[0x02] = bev->x;
                                sc->coord[0x01] = sc->coord[0x03] = VPX[0x03] - bev->y;

                                pickUV_tool ( ( G3DMOUSETOOL * ) sc, 
                                                                 sce, 
                                                                 cam, 
                                                                 urm, 
                                                                 engine_flags, 
                                                                 event );

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

                            parmes->obj.update_flags |= RESETMODIFIERS;

                            /** TODO: do this only for subdivided meshes ***/
                            g3dmesh_update ( parmes, engine_flags );

                            olduv = newuv = NULL;
                        } return REDRAWVIEW            | 
                                 UPDATECOORDS          | 
                                 BUFFEREDSUBDIVISIONOK | 
                                 UPDATECURRENTOBJECT   | 
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
static int scale_morpher ( G3DMORPHER       *mpr,
                           G3DMOUSETOOL     *mou, 
                           G3DSCENE         *sce, 
                           G3DCAMERA        *cam,
                           G3DURMANAGER     *urm,
                           uint64_t engine_flags, 
                           G3DEvent         *event ) {
    static double orix, oriy, oriz, newx, newy, newz,
                  winx, winy, winz;
    static double mouseXpress, mouseYpress;
    static GLint VPX[0x04];
    G3DOBJECT *obj = ( G3DOBJECT * ) mpr;
    static LIST *lver, *lfac, *ledg;
    static G3DVECTOR *oldpos;
    static G3DVECTOR *newpos;
    static G3DVECTOR localPivot;

    if ( obj->parent->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) obj->parent;

        if ( mpr->selmpose ) {
            switch ( event->type ) {
                case G3DButtonPress : {
                    if ( engine_flags & VIEWVERTEX ) {
                        G3DButtonEvent *bev = ( G3DButtonEvent * ) event;
                        G3DVECTOR avgpos;

                        glGetIntegerv ( GL_VIEWPORT, VPX );

                        mouseXpress = orix = bev->x;
                        mouseYpress = bev->y;

                        lver = g3dmorpher_getMeshPoseSelectedVertices ( mpr,
                                                                        NULL );

                        g3dvertex_getAveragePositionFromList ( lver, 
                                                              &localPivot );

                        if ( lver ) {
                            oldpos = g3dmorpher_getMeshPoseArrayFromList ( mpr, 
                                                                           NULL, 
                                                                           lver );
                        }
                    }
                } return REDRAWVIEW;

                case G3DMotionNotify : {
                    G3DMotionEvent *mev = ( G3DMotionEvent * ) event;

                    newx = mev->x;

                    if ( mev->state & G3DButton1Mask ) {
                        if ( engine_flags & VIEWVERTEX ) {
                            G3DVECTOR *axis = sce->csr.axis;
                            LIST *ltmpver = lver;
                            double difx, dify, difz;
                            uint32_t verID = 0x00;

                            if ( ( engine_flags & XAXIS ) && axis[0].w ) difx = ( newx - orix );
                            if ( ( engine_flags & YAXIS ) && axis[1].w ) dify = ( newx - orix );
                            if ( ( engine_flags & ZAXIS ) && axis[2].w ) difz = ( newx - orix );

                            while ( ltmpver ) {
                                G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;
                                G3DMORPHERVERTEXPOSE *vpose;

                                vpose = g3dmorpher_getVertexPose ( mpr, ver, NULL, NULL );

                                vpose->pos.x = localPivot.x + ( ( oldpos[verID].x - localPivot.x ) * ( 1.0f + ( difx * 0.01f ) ) );
                                vpose->pos.y = localPivot.y + ( ( oldpos[verID].y - localPivot.y ) * ( 1.0f + ( dify * 0.01f ) ) );
                                vpose->pos.z = localPivot.z + ( ( oldpos[verID].z - localPivot.z ) * ( 1.0f + ( difz * 0.01f ) ) );

                                verID++;

                                ltmpver = ltmpver->next;
                            }
                        }
                    }
                } return REDRAWVIEW;

                case G3DButtonRelease : {
                    if ( engine_flags & VIEWVERTEX ) {
                        G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

                        /*** simulate click and release ***/
                        if ( ( ( int ) bev->x == ( int ) mouseXpress ) && 
                             ( ( int ) bev->y == ( int ) mouseYpress ) ) {
                            G3DMOUSETOOLSCALE *sc = ( G3DMOUSETOOLSCALE * ) mou;

                            sc->coord[0x00] = sc->coord[0x02] = bev->x;
                            sc->coord[0x01] = sc->coord[0x03] = VPX[0x03] - bev->y;

                            pick_tool ( ( G3DMOUSETOOL * ) sc, 
                                                           sce, 
                                                           cam, 
                                                           urm, 
                                                           engine_flags, 
                                                           event );
                        } else {
                            if ( lver ) {
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
                            }
                        }

                        list_free ( &lver, NULL );

                        oldpos = newpos = NULL;
                    }
                } return REDRAWVIEW            | 
                         UPDATECOORDS          | 
                         BUFFEREDSUBDIVISIONOK | 
                         UPDATECURRENTOBJECT   | 
                         REDRAWUVMAPEDITOR;

                default :
                break;
            }
        }
    }

    return FALSE;
}

/******************************************************************************/
static int scale_mesh ( G3DMESH          *mes,
                        G3DMOUSETOOL     *mou, 
                        G3DSCENE         *sce, 
                        G3DCAMERA        *cam,
                        G3DURMANAGER     *urm,
                        uint64_t engine_flags, 
                        G3DEvent         *event ) {
    static double orix, oriy, oriz, newx, newy, newz,
                  winx, winy, winz;
    static double mouseXpress, mouseYpress;
    static GLint VPX[0x04];
    G3DOBJECT *obj = ( G3DOBJECT * ) mes;
    static LIST *lver, *lfac, *ledg;
    static G3DVECTOR *oldpos;
    static G3DVECTOR *newpos;
    static G3DVECTOR localPivot;

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;
            G3DVECTOR avgpos;

            glGetIntegerv ( GL_VIEWPORT, VPX );

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

            g3dvertex_getAveragePositionFromList ( lver, &localPivot );

            g3dvertex_copyPositionFromList       ( lver, &oldpos );
 
            lfac = g3dvertex_getFacesFromList  ( lver );
            ledg = g3dface_getEdgesFromList    ( lfac );

            /*g3dmesh_modify ( mes,
                             G3DMODIFYOP_STARTUPDATE,
                             engine_flags );*/

            if ( mes->onGeometryMove ) {
                mes->onGeometryMove ( mes, lver, 
                                           ledg, 
                                           lfac, 
                                           G3DMODIFYOP_STARTUPDATE,
                                           engine_flags );
            }

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
                    uint32_t verID = 0x00;

                    if ( ( engine_flags & XAXIS ) && axis[0].w ) difx = ( newx - orix );
                    if ( ( engine_flags & YAXIS ) && axis[1].w ) dify = ( newx - orix );
                    if ( ( engine_flags & ZAXIS ) && axis[2].w ) difz = ( newx - orix );

                    while ( ltmpver ) {
                        G3DVERTEX *ver = ( G3DVERTEX * ) ltmpver->data;

                        ver->pos.x = localPivot.x + ( ( oldpos[verID].x - localPivot.x ) * ( 1.0f + ( difx * 0.01f ) ) );
                        ver->pos.y = localPivot.y + ( ( oldpos[verID].y - localPivot.y ) * ( 1.0f + ( dify * 0.01f ) ) );
                        ver->pos.z = localPivot.z + ( ( oldpos[verID].z - localPivot.z ) * ( 1.0f + ( difz * 0.01f ) ) );

                        if ( obj->parent->childvertexchange ) {
                            obj->parent->childvertexchange ( obj->parent,
                                                             obj, ver );
                        }

                        verID++;

                        ltmpver = ltmpver->next;
                    }

                    /*g3dmesh_modify ( mes,
                                     G3DMODIFYOP_UPDATE,
                                     engine_flags );*/

                    if ( mes->onGeometryMove ) {
                        mes->onGeometryMove ( mes, lver, 
                                                   ledg, 
                                                   lfac, 
                                                   G3DMODIFYOP_UPDATE,
                                                   engine_flags );
                    }
                }
            }
        } return REDRAWVIEW;

        case G3DButtonRelease : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            /*** simulate click and release ***/
            if ( ( ( int ) bev->x == ( int ) mouseXpress ) && 
                 ( ( int ) bev->y == ( int ) mouseYpress ) ) {
                G3DMOUSETOOLSCALE *sc = ( G3DMOUSETOOLSCALE * ) mou;

                sc->coord[0x00] = sc->coord[0x02] = bev->x;
                sc->coord[0x01] = sc->coord[0x03] = VPX[0x03] - bev->y;

                pick_tool ( ( G3DMOUSETOOL * ) sc, 
                                               sce, 
                                               cam, 
                                               urm, 
                                               engine_flags, 
                                               event );
            }

            g3dvertex_copyPositionFromList ( lver, &newpos );

            g3durm_mesh_moveVertexList ( urm, 
                                         mes, 
                                         lver, 
                                         ledg, 
                                         lfac, 
                                         NULL,
                                         oldpos, 
                                         newpos, 
                                         REDRAWVIEW );

            g3dmesh_updateBbox ( mes );

            if ( mes->onGeometryMove ) {
                mes->onGeometryMove ( mes, 
                                      lver, 
                                      ledg, 
                                      lfac, 
                                      G3DMODIFYOP_ENDUPDATE,
                                      engine_flags );
            }

            /*g3dmesh_modify ( mes,
                             G3DMODIFYOP_ENDUPDATE,
                             engine_flags );*/

            list_free ( &lver, NULL );
            list_free ( &lfac, NULL );
            list_free ( &ledg, NULL );

            oldpos = newpos = NULL;
        } return REDRAWVIEW            | 
                 UPDATECOORDS          | 
                 BUFFEREDSUBDIVISIONOK | 
                 UPDATECURRENTOBJECT   | 
                 REDRAWUVMAPEDITOR;

        default :
        break;
    }

    return FALSE;
}

/******************************************************************************/
static int scale_object ( LIST        *lobj, 
                          G3DMOUSETOOL *mou, 
                          G3DSCENE     *sce, 
                          G3DCAMERA    *cam,
                          G3DURMANAGER *urm,
                          uint64_t engine_flags, 
                          G3DEvent     *event ) {
    static double orix, oriy, oriz, newx, newy, newz,
                  winx, winy, winz;
    static double mouseXpress, mouseYpress;
    static float MVX[0x10];
    static GLint VPX[0x04];
    static LIST *lver, *lfac, *ledg;
    static G3DVECTOR lvecx, lvecy, lvecz;
    static uint32_t nbobj;
    static URMTRANSFORMOBJECT *uto;
    static float PREVWMVX[0x10];

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            /*** Record and undo procedure and record the current state ***/
            uto = g3durm_object_transform ( urm,
                                            sce,
                                            lobj,
                                            REDRAWVIEW );

            urmtransform_saveState ( uto, UTOSAVESTATEBEFORE );
            /***/

            mouseXpress = orix = bev->x;
            mouseYpress = bev->y;

            memcpy( MVX,
                    cam->obj.inverseWorldMatrix,
                    sizeof( cam->obj.inverseWorldMatrix ) );

            glGetIntegerv ( GL_VIEWPORT, VPX );

            nbobj = list_count ( lobj );

            if ( nbobj == 0x01 ) {
                G3DOBJECT *obj = ( G3DOBJECT * ) lobj->data;
                /*** PREVWMX is used in VIEWAXIS mode to move vertices back ***/
                memcpy ( PREVWMVX, obj->worldMatrix, sizeof ( obj->worldMatrix ) );
            }
        } return REDRAWVIEW;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;
            LIST *ltmpobj = lobj;

            if ( mev->state & G3DButton1Mask ) {
                G3DVECTOR dif = { 0.0f, 0.0f, 0.0f }; /** local pivot ***/
                G3DVECTOR *axis = ( G3DVECTOR * ) &sce->csr.axis;

                if ( ( engine_flags & XAXIS ) && axis[0x00].w ) dif.x = ( mev->x - orix );
                if ( ( engine_flags & YAXIS ) && axis[0x01].w ) dif.y = ( mev->x - orix );
                if ( ( engine_flags & ZAXIS ) && axis[0x02].w ) dif.z = ( mev->x - orix );

                while ( ltmpobj ) {
                    G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

                    if ( ( obj->flags & OBJECTNOSCALING ) == 0x00 ) {
                        obj->sca.x += ( dif.x * 0.02f );
                        obj->sca.y += ( dif.y * 0.02f );
                        obj->sca.z += ( dif.z * 0.02f );

                        if ( obj->sca.x < 0.0f ) obj->sca.x = 0.0001f;
                        if ( obj->sca.y < 0.0f ) obj->sca.y = 0.0001f;
                        if ( obj->sca.z < 0.0f ) obj->sca.z = 0.0001f;

                        g3dobject_updateMatrix_r ( obj, engine_flags );

                        if ( engine_flags & VIEWAXIS ) {
                            /*** in case this was in VIEWAXIS mode, we move ***/
                            /*** back the vertices were they were before,   ***/
                            /*** in world coord ***/
                            if ( nbobj == 0x01 ) {
                                if ( obj->type == G3DMESHTYPE ) {
                                    G3DMESH *mes = ( G3DMESH * ) obj;

                                    g3dmesh_moveAxis ( mes, PREVWMVX, engine_flags );
                                    memcpy ( PREVWMVX, obj->worldMatrix, sizeof ( obj->worldMatrix ) );
                                }

                                if ( obj->type & SPLINE ) {
                                    G3DSPLINE *spl = ( G3DSPLINE * ) obj;

                                    g3dspline_moveAxis ( spl, PREVWMVX, engine_flags );
                                    memcpy ( PREVWMVX, obj->worldMatrix, sizeof ( obj->worldMatrix ) );
                                }
                            }
                        }
                    }

                    ltmpobj = ltmpobj->next;
                }
            }

            orix = mev->x;
        } return REDRAWVIEW | UPDATECOORDS;

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
            if ( ( ( int ) bev->x == ( int ) mouseXpress ) && 
                 ( ( int ) bev->y == ( int ) mouseYpress ) ) {
                G3DMOUSETOOLSCALE *sc = ( G3DMOUSETOOLSCALE * ) mou;

                sc->coord[0x00] = sc->coord[0x02] = bev->x;
                sc->coord[0x01] = sc->coord[0x03] = VPX[0x03] - bev->y;

                /*** FIRST UNDO the TRANSFORM that we saved at buttonPress ***/
                /*** and that was not used at all ***/
                g3durmanager_undo ( urm, engine_flags );

                pick_tool ( ( G3DMOUSETOOL * ) sc, 
                                               sce, 
                                               cam, 
                                               urm, 
                                               engine_flags, 
                                               event );
            } else {
                urmtransform_saveState ( uto, UTOSAVESTATEAFTER );
            }

        } return UPDATEANDREDRAWALL;

        default :
        break;
    }

    return FALSE;
}

/******************************************************************************/
static int scale_tool ( G3DMOUSETOOL *mou, 
                        G3DSCENE     *sce,
                        G3DCAMERA    *cam,
                        G3DURMANAGER *urm, 
                        uint64_t      engine_flags, 
                        G3DEvent     *event ) {
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
                                    .only_visible = 0x00,
                                    .weight = 0.0f,
                                    .radius = PICKMINRADIUS };

            pick_cursor ( &pt, sce, cam, engine_flags );
        } break;

        default :
        break;
    }

    if ( engine_flags & VIEWOBJECT ) {
        return scale_object ( sce->lsel, mou, sce, cam, urm, engine_flags, event );
    }


    if ( obj ) {
        if ( engine_flags & VIEWAXIS ) {
            LIST ldummy = { .next = NULL, 
                            .data = obj,
                            .prev = NULL };

            return scale_object ( &ldummy, mou, sce, cam, urm, engine_flags, event );
        }

        if ( engine_flags & VIEWUVWMAP ) {
            if ( obj->type & MESH ) {
                G3DMESH  *mes = ( G3DMESH * ) obj;
                G3DUVMAP * map = g3dmesh_getSelectedUVMap ( mes );

                if ( map && ( g3duvmap_isFixed ( map ) == 0x00 ) ) {
                    LIST ldummy = { .next = NULL, 
                                    .data = map,
                                    .prev = NULL };
                    return scale_object ( &ldummy, mou, sce, cam, urm, engine_flags, event );
                }
            }
        }

        if ( ( engine_flags & VIEWVERTEX ) ||
             ( engine_flags & VIEWEDGE   ) ||
             ( engine_flags & VIEWFACE   ) ) {
            if ( obj->type & MORPHER ) {
                G3DMORPHER *mpr = ( G3DMORPHER * ) obj;

                return scale_morpher ( mpr, mou, sce, cam, urm, engine_flags, event );
            }

            if ( obj->type & SPLINE ) {
                G3DSPLINE *spl = ( G3DSPLINE * ) obj;

                return scale_spline ( spl, mou, sce, cam, urm, engine_flags, event );
            }

            if ( ( obj->type & MESH ) && ( obj->type & EDITABLE ) ) {
                G3DMESH *mes = ( G3DMESH * ) obj;

                return scale_mesh ( mes, mou, sce, cam, urm, engine_flags, event );
            }
        }
    }

    return FALSE;
}
