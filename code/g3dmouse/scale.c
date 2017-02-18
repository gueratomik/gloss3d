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
int scale_tool ( G3DMOUSETOOL *mou, G3DSCENE *sce, G3DCAMERA *cam,
                 G3DURMANAGER *urm, uint32_t flags, G3DEvent *event ) {
    static GLdouble MVX[0x10], PJX[0x10], TMPX[0x10], FINX[0x10];
    static GLint VPX[0x04];
    static int32_t xold, yold, xori, yori;
    static double objx, objy, objz,
                  winx, winy, winz;
    static G3DVECTOR center;
    static G3DOBJECT *obj;
    static G3DMESH *mes;
    static LIST *lori, *lver, *lfac, *lsub, *ledg, *ffdlsub, *lvtx; /*** list of rail vectors ***/
    static G3DVECTOR *oldpos;
    static G3DVECTOR *newpos;
    static G3DVECTOR objsca;
    uint32_t retflags = REDRAWVIEW | REDRAWCOORDS;

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            xori = bev->x;
            yori = bev->y;

            glGetIntegerv ( GL_VIEWPORT, VPX );

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
                memcpy ( FINX, obj->wmatrix, sizeof ( FINX ) );

                /*** save status ***/
                memcpy ( &objsca, &obj->sca, sizeof ( G3DVECTOR ) );

                if ( obj->type & EDITABLE ) {
                    mes = ( G3DMESH * ) obj;

                    /****** Cursor axis picked ? ******/
                    if ( flags & VIEWVERTEX ) {
                        double TSX[0x10];

                        memcpy ( TMPX, FINX, sizeof ( TMPX ) );
                        g3dmesh_getSelectedVerticesTranslation ( mes, TSX );

                        g3dcore_multmatrix ( TSX, TMPX, FINX );
                    }

                    if ( flags & VIEWFACE ) {
                        double TSX[0x10];

                        memcpy ( TMPX, FINX, sizeof ( TMPX ) );
                        g3dmesh_getSelectedFacesTranslation ( mes, TSX );

                        g3dcore_multmatrix ( TSX, TMPX, FINX );
                    }
 
                    if ( flags & VIEWEDGE ) {
                        double TSX[0x10];

                        memcpy ( TMPX, FINX, sizeof ( TMPX ) );
                        g3dmesh_getSelectedEdgesTranslation ( mes, TSX );

                        g3dcore_multmatrix ( TSX, TMPX, FINX );
                    }
                }

                /****** Cursor axis picked ? ******/
                g3dcursor_pick ( &sce->csr, FINX, cam, bev->x, 
                                                       VPX[0x03] - bev->y,
                                 flags );

                if ( obj->type & EDITABLE ) {
                    G3DOBJECT *objmes = obj;
                    LIST *ltmp;
                    mes = ( G3DMESH * ) obj;

                    if ( flags & VIEWVERTEX ) {
                        lver = g3dmesh_getVertexListFromSelectedVertices ( mes );
                    }

                    if ( flags & VIEWFACE ) {
                        lver = g3dmesh_getVertexListFromSelectedFaces ( mes );
                    }

                    if ( flags & VIEWEDGE ) {
                        lver = g3dmesh_getVertexListFromSelectedEdges ( mes );
                    }
 
                    lfac = g3dvertex_getFacesFromList  ( lver );
                    ledg = g3dface_getEdgesFromList    ( lfac );
                    lvtx = g3dface_getVerticesFromList ( lfac );

                    /*if ( mes->subdiv && (objmes->flags & BUFFEREDSUBDIVISION) ) {
                        lsub = g3dvertex_getAreaFacesFromList ( lver );
                    }*/

                    /*** for the undo/redo manager ***/
                    g3dvertex_copyPositionFromList ( lver, &oldpos );

                    g3dmesh_startUpdateModifiers_r ( obj, flags );

                    /*** get scaling center. It is the average pos of vertices ***/
                    /*** we are going to move ***/
                    g3dmesh_getCenterFromVertexList ( lver, &center );

                    /*** for each vertex, add a rail vector ***/
                    ltmp = lver;
                    while ( ltmp ) {
                        G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;
                        G3DVECTOR *ori = g3dvector_new ( ( ver->pos.x - center.x ),
                                                         ( ver->pos.y - center.y ),
                                                         ( ver->pos.z - center.z ),
                                                         ( 1.0f ) );

                        list_append ( &lori, ori );

                        ltmp = ltmp->next;
                    }
                }
            }

            xold = bev->x;
            yold = bev->y;
        } return REDRAWVIEW | REDRAWCOORDS;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;

            if ( obj ) {

                if ( obj->type == G3DUVMAPTYPE ) retflags |= NOBUFFEREDSUBDIVISION;

                if ( ( flags & VIEWOBJECT ) ||
                     ( flags & VIEWSKIN   ) ||
                     ( flags & VIEWUVWMAP ) ) {
                    if ( ( flags & XAXIS ) && sce->csr.axis[0x00].w ) {
                        obj->sca.x += ((float) ( xold - mev->x ) / 10.0f);
                    }

                    if ( ( flags & YAXIS ) && sce->csr.axis[0x01].w ) {
                        obj->sca.y += ((float) ( xold - mev->x ) / 10.0f);
                    }

                    if ( ( flags & ZAXIS ) && sce->csr.axis[0x02].w ) {
                        obj->sca.z += ((float) ( xold - mev->x ) / 10.0f);
                    }

                    g3dobject_updateMatrix_r ( obj, 0x00 );

                    xold = mev->x;
                    yold = mev->y;
                } else {

                    LIST *ltmp = lver;
                    LIST *lbuf = lori;

                    if ( obj->type & EDITABLE ) {
                        while ( ltmp ) {
                            G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;
                            G3DVECTOR *ori = ( G3DVECTOR * ) lbuf->data;

                            float sca = ( mev->x ) ? ( float ) xold / mev->x : 1.0f;


                            if ( ( flags & XAXIS ) && sce->csr.axis[0x00].w ) {
                                ver->pos.x = center.x + ( ori->x * ( sca ) );
                            }

                            if ( ( flags & YAXIS ) && sce->csr.axis[0x01].w ) {
                                ver->pos.y = center.y + ( ori->y * ( sca ) );
                            }

                            if ( ( flags & ZAXIS ) && sce->csr.axis[0x02].w ) {
                                ver->pos.z = center.z + ( ori->z * ( sca ) );
                            }

                            if ( ver->flags & VERTEXSKINNED ) {
                                g3dvertex_computeSkinnedPosition ( ver );
                            }

                            lbuf = lbuf->next;
                            ltmp = ltmp->next;

                            /*g3dvertex_updateFacesPosition ( ver );*/
                        }

                        g3dmesh_updateModifiers_r ( obj, flags );

                        if ( mes->onGeometryMove ) {
                            mes->onGeometryMove ( mes, lver, 
                                                       ledg, 
                                                       lfac, 
                                                       flags );
                        }
                    }
                }
            }
        } return retflags;

        case G3DButtonRelease : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            if ( ( bev->x == xori ) && ( bev->y == yori ) ) {
                G3DPICKTOOL pt;
                void *olddata = mou->data;
                GLint polygonMode;
                glGetIntegerv(GL_POLYGON_MODE, &polygonMode);

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
                        g3durm_object_move ( urm, obj, &obj->pos, 
                                                       &obj->rot,
                                                       &objsca,
                                                        ( flags & VIEWAXIS ),
                                                        REDRAWVIEW );
                    }

                    if ( ( flags & VIEWVERTEX ) ||
                         ( flags & VIEWEDGE   ) ||
                         ( flags & VIEWFACE   ) ) {
                        if ( obj->type & MESH ) { /*** FFD also concerned ***/

                            g3dvertex_copyPositionFromList ( lver, &newpos );

                            g3durm_mesh_moveVertexList ( urm, mes, lver, ledg, lfac, lsub,
                                                         oldpos, 
                                                         newpos, 
                                                         REDRAWVIEW );

                            g3dmesh_updateBbox ( mes );
                        }
                    }
                }
            }

            list_free ( &lver, NULL );
            list_free ( &lvtx, NULL );
            list_free ( &lfac, NULL );
            list_free ( &lsub, NULL );

            mes  = NULL;
            obj  = NULL;

            oldpos = newpos = NULL;

            /*** free the temporary list and vectors ***/
            if ( lori ) {
                list_free ( &lori, g3dvector_free );
            }
        } return REDRAWVIEW | REDRAWCOORDS | BUFFEREDSUBDIVISIONOK | REDRAWCURRENTOBJECT;

        default : {

        } break;
    }

    return FALSE;
}
