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
#include <g3dmouse.h>

/******************************************************************************/
/* each function must return FALSE for redrawing the OGL Widget it belongs to */
/* only or TRUE to redraw all OGL Widgets                                     */
/******************************************************************************/

/******************************************************************************/
int move_tool ( G3DMOUSETOOL *mou, G3DSCENE *sce, G3DCAMERA *cam,
                G3DURMANAGER *urm, uint32_t flags, G3DEvent *event ) {
    static GLdouble MVX[0x10], PJX[0x10], TMPX[0x10], FINX[0x10];
    static GLint VPX[0x04];
    static double objx, objy, objz, orix, oriy, oriz,
                  winx, winy, winz;
    static G3DMESH *mes;
    static G3DOBJECT *obj;
    static int32_t xold, yold;
    static LIST *lver, *lfac, *lsub, *ledg, *ffdlsub, *lvtx;
    static G3DVECTOR *oldpos;
    static G3DVECTOR *newpos;
    static G3DVECTOR objpos;
    static G3DMESHFAC **msftab; /*** list of faces to update from skinning ***/
    static uint32_t nbmsf;
    static G3DVECTOR vecx = { .x = 1.0f, .y = 0.0f, .z = 0.0f, .w = 1.0f },
                     vecy = { .x = 0.0f, .y = 1.0f, .z = 0.0f, .w = 1.0f },
                     vecz = { .x = 0.0f, .y = 0.0f, .z = 1.0f, .w = 1.0f };
    static G3DVECTOR movvecx, movvecy, movvecz;
    static float objlocx, objlocy, objlocz;
    uint32_t retflags = REDRAWVIEW | REDRAWCOORDS;

    switch ( event->type ) {
        case G3DButtonPress : {
            /*G3DVECTOR vec = { 0.f, 0.f, 0.f, 1.0f };*/
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            xold = bev->x;
            yold = bev->y;

            obj = g3dscene_getLastSelected ( sce );

            /*** If the selected object is a MESH and we ***/
            /*** are in viewmap mode, we move its uvwmap. ***/
            if ( obj && ( flags & VIEWUVWMAP ) ) {
                if ( obj->type == G3DMESHTYPE ) {
                    G3DMESH *mes = ( G3DMESH * ) obj;

                    obj = ( G3DOBJECT * ) g3dmesh_getLastUVMap ( mes );
                }
            }

            if ( obj ) {
                memcpy ( &objpos, &obj->pos, sizeof ( G3DVECTOR ) );

                memcpy ( FINX, obj->wmatrix, sizeof ( FINX ) );

                glPushMatrix ( );

                /*if ( flags & VIEWOBJECT ) {*/
                    g3dvector_matrix ( &vecx, obj->rmatrix, &movvecx );
                    g3dvector_matrix ( &vecy, obj->rmatrix, &movvecy );
                    g3dvector_matrix ( &vecz, obj->rmatrix, &movvecz );

                    objlocx = obj->pos.x;
                    objlocy = obj->pos.y;
                    objlocz = obj->pos.z;
                /*}*/

                glMultMatrixd ( obj->wmatrix );

                glGetDoublev  ( GL_MODELVIEW_MATRIX, MVX );
                glPopMatrix ( );

                glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
                glGetIntegerv ( GL_VIEWPORT, VPX );

                gluProject ( 0.0f, 0.0f, 0.0f, MVX, PJX, VPX, &winx, &winy, &winz );
                gluUnProject ( ( GLdouble ) bev->x,
                               ( GLdouble ) VPX[0x03] - bev->y,
                               ( GLdouble ) winz,
                               MVX, PJX, VPX,
                               &orix, &oriy, &oriz );

                if ( obj->type & EDITABLE ) { /*** ffd also concerned ***/
                    mes = ( G3DMESH * ) obj;

                    /****** Cursor axis picked ? ******/
                    if ( flags & VIEWVERTEX ) {
                        double TSX[0x10];

                        memcpy ( TMPX, FINX, sizeof ( TMPX ) );
                        g3dmesh_getSelectedVerticesTranslation ( mes, TSX );

                        g3dcore_multmatrix ( TSX, TMPX, FINX );

                        lver = g3dmesh_getVertexListFromSelectedVertices ( mes );

                        /*g3dvertex_copyPositionFromList ( lver, &oldpos );*/
                    }

                    if ( flags & VIEWFACE ) {
                        double TSX[0x10];

                        memcpy ( TMPX, FINX, sizeof ( TMPX ) );
                        g3dmesh_getSelectedFacesTranslation ( mes, TSX );

                        g3dcore_multmatrix ( TSX, TMPX, FINX );

                        lver = g3dmesh_getVertexListFromSelectedFaces ( mes );

                        /*g3dvertex_copyPositionFromList ( lver, &oldpos );*/
                    }
 
                    if ( flags & VIEWEDGE ) {
                        double TSX[0x10];

                        memcpy ( TMPX, FINX, sizeof ( TMPX ) );
                        g3dmesh_getSelectedEdgesTranslation ( mes, TSX );

                        g3dcore_multmatrix ( TSX, TMPX, FINX );

                        lver = g3dmesh_getVertexListFromSelectedEdges ( mes );

                        /*g3dvertex_copyPositionFromList ( lver, &oldpos );*/
                    }

                    lfac = g3dvertex_getFacesFromList  ( lver );
                    ledg = g3dface_getEdgesFromList    ( lfac );
                    lvtx = g3dface_getVerticesFromList ( lfac );

                    g3dvertex_copyPositionFromList ( lver, &oldpos );

                    g3dmesh_startUpdateModifiers_r ( obj, flags );

                    /*if ( mes->subdiv && (((G3DOBJECT*)mes)->flags & BUFFEREDSUBDIVISION) ) {
                        lsub = g3dvertex_getAreaFacesFromList ( lver );
                    }*/
                }

                g3dcursor_pick ( &sce->csr, FINX, cam, bev->x, 
                                                       VPX[0x03] - bev->y,
                                 flags );

            }
        } return REDRAWVIEW | REDRAWCOORDS;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;

            if ( obj ) {
                gluProject ( 0.0f, 0.0f, 0.0f, MVX, PJX, VPX, &winx, &winy, &winz );
                gluUnProject ( ( GLdouble ) mev->x,
                               ( GLdouble ) VPX[0x03] - mev->y,
                               ( GLdouble ) winz,
                               MVX, PJX, VPX,
                               &objx, &objy, &objz );

                if ( obj->type == G3DUVMAPTYPE ) retflags |= NOBUFFEREDSUBDIVISION;

                /*** In VIEWOBJECT mode, just move the object ***/
                if ( ( flags & VIEWOBJECT ) ||
                     ( flags & VIEWAXIS   ) || 
                     ( flags & VIEWSKIN   ) || 
                     ( flags & VIEWUVWMAP ) ) {

                    g3dvector_init ( &obj->pos, objlocx,
                                                objlocy,
                                                objlocz, 1.0f );

                    /*** move object based on its orientation ***/
                    if ( ( flags & XAXIS ) && sce->csr.axis[0x00].w ) {
                        obj->pos.x += ( movvecx.x * ( objx - orix ) );
                        obj->pos.y += ( movvecx.y * ( objx - orix ) );
                        obj->pos.z += ( movvecx.z * ( objx - orix ) );
                    }

                    if ( ( flags & YAXIS ) && sce->csr.axis[0x01].w ) {
                        obj->pos.x += ( movvecy.x * ( objy - oriy ) );
                        obj->pos.y += ( movvecy.y * ( objy - oriy ) );
                        obj->pos.z += ( movvecy.z * ( objy - oriy ) );
                    }

                    if ( ( flags & ZAXIS ) && sce->csr.axis[0x02].w ) {
                        obj->pos.x += ( movvecz.x * ( objz - oriz ) );
                        obj->pos.y += ( movvecz.y * ( objz - oriz ) );
                        obj->pos.z += ( movvecz.z * ( objz - oriz ) );
                    }

                    g3dobject_updateMatrix_r ( obj, flags );

                    /* move vertices */
                    if ( flags & VIEWAXIS ) {
                        if ( obj->type & MESH ) {
                            g3dmesh_moveAxis ( obj, FINX, flags );

                            memcpy ( FINX, obj->wmatrix, sizeof ( FINX ) );
                        }
                    }
                } else {
                    if ( obj->type & EDITABLE ) {
                        LIST *ltmp = lver;

                        while ( ltmp ) {
                            G3DVERTEX *ver = ltmp->data;

                            if ( ( flags & XAXIS ) && sce->csr.axis[0x00].w ) {
                                ver->pos.x += ( objx - orix );
                            }

                            if ( ( flags & YAXIS ) && sce->csr.axis[0x01].w ) {
                                ver->pos.y += ( objy - oriy );
                            }

                            if ( ( flags & ZAXIS ) && sce->csr.axis[0x02].w ) {
                                ver->pos.z += ( objz - oriz );
                            }

                            if ( obj->parent->childvertexchange ) {
                                obj->parent->childvertexchange ( obj->parent,
                                                                 obj, ver );
                            }

                            if ( ver->flags & VERTEXSKINNED ) {
                                g3dvertex_computeSkinnedPosition ( ver );
                            }

                            ltmp = ltmp->next;
                        }

                        g3dmesh_updateModifiers_r ( obj, flags );

                        if ( mes->onGeometryMove ) {
                            mes->onGeometryMove ( mes, lver, 
                                                       ledg, 
                                                       lfac, 
                                                       flags );
                        }
                    }

                    /*** this is only needed in vertex/face/edge mode. ***/
                    /*** do not use those 3 lines in object mode ***/
                    orix = objx;
                    oriy = objy;
                    oriz = objz;
                }

                return retflags;
            }
        } return 0x00;

        case G3DButtonRelease : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            if ( ( bev->x == xold ) && ( bev->y == yold ) ) {
                G3DPICKTOOL pt;
                void *olddata = mou->data;
                GLint polygonMode;
                glGetIntegerv(GL_POLYGON_MODE, &polygonMode );

                /*** when in wireframe, pick all vertices ***/
                if ( polygonMode == GL_LINE ) {
                    pt.only_visible = 0x00;
                } else {
                    pt.only_visible = 0x01;
                }

                mou->data = &pt;

                event->type = G3DButtonPress;
                pick_tool ( mou, sce, cam, urm, flags, event );

                event->type = G3DButtonRelease;
                pick_tool ( mou, sce, cam, urm, flags, event );

                mou->data = olddata;
            } else {
                if ( obj ) {
                    if ( obj->type == G3DUVMAPTYPE ) {
                        G3DMESH *uvwmes = ( G3DMESH * ) obj->parent;

                        if ( uvwmes ) {
                            g3dmesh_update ( uvwmes, NULL,
                                                     NULL,
                                                     NULL,
                                                     RESETMODIFIERS, flags );
                        }
                    }

                    if ( ( flags & VIEWOBJECT ) ||
                         ( flags & VIEWAXIS   ) ) {
                        g3durm_object_move ( urm, obj, &objpos, 
                                                       &obj->rot,
                                                       &obj->sca,
                                                       ( flags & VIEWAXIS ),
                                                       REDRAWVIEW );
                    }

                    if ( ( flags & VIEWVERTEX ) ||
                         ( flags & VIEWEDGE   ) ||
                         ( flags & VIEWFACE   ) ) {
                        if ( obj->type & EDITABLE ) { /*** FFD also concerned ***/
                            g3dvertex_copyPositionFromList ( lver, &newpos );

                            g3durm_mesh_moveVertexList ( urm, mes, lver, ledg, lfac, lsub,
                                                         oldpos, 
                                                         newpos, 
                                                         REDRAWVIEW );

                            g3dmesh_updateBbox ( mes );
                        }
                    }

                    g3dmesh_endUpdateModifiers_r ( obj, flags );

                    /*if ( obj->type == G3DBONETYPE ) {
                        if ( msftab ) {
                            uint32_t i;

                            for ( i = 0x00; i < nbmsf; i++ ) {
                                g3dmeshfac_free ( msftab[i] );
                            }

                            free ( msftab ); msftab = NULL;
                            nbmsf = 0x00;
                        }
                    }*/
                }
            }

            list_free ( &lver, NULL );
            list_free ( &lvtx, NULL );
            list_free ( &lfac, NULL );
            list_free ( &lsub, NULL );
            list_free ( &ledg, NULL );
            list_free ( &ffdlsub, NULL );

            mes  = NULL;
            obj  = NULL;

            oldpos = newpos = NULL;
        } return REDRAWVIEW | REDRAWCOORDS | BUFFEREDSUBDIVISIONOK | REDRAWCURRENTOBJECT;

        default :
        break;
    }


    return FALSE;
}
