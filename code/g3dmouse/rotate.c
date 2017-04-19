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
G3DMESHFAC *g3dmeshfac_new ( G3DMESH *mes, LIST *lfac ) {
    G3DMESHFAC *msf = ( G3DMESHFAC * ) calloc ( 0x01, sizeof ( G3DMESHFAC ) );

    if ( msf == NULL ) {
        fprintf ( stderr, "g3dmeshfac_new: memory allocation failed\n" );

        return NULL;
    }

    msf->mes  = mes;
    msf->lfac = lfac;


    return msf;
}

/******************************************************************************/
G3DMESHFAC *g3dmeshfac_free ( G3DMESHFAC *msf ) {
    list_free ( &msf->lfac, NULL );

    free ( msf );
}

/******************************************************************************/
int rotate_tool  ( G3DMOUSETOOL *mou, G3DSCENE *sce, G3DCAMERA *cam,
                   G3DURMANAGER *urm, uint32_t flags, G3DEvent *event ) {
    static int32_t xold, yold, xori, yori;
    static GLdouble MVX[0x10], PJX[0x10], TMPX[0x10], FINX[0x10];
    static GLint VPX[0x04];
    static double objx, objy, objz, orix, oriy, oriz, rotx, roty, rotz,
                  winx, winy, winz, locrotx, locroty, locrotz;
    static G3DMESH *mes = NULL;
    static G3DOBJECT *obj = NULL;
    static G3DVECTOR *vec = NULL;
    static uint32_t nbver;
    static LIST *lver, *lfac, *lsub, *ledg, *ffdlsub;
    static G3DVECTOR center;
    static G3DVECTOR *oldpos;
    static G3DVECTOR *newpos;
    static G3DVECTOR objrot;

    static G3DMESHFAC **msftab; /*** list of faces to update from skinning ***/
    static uint32_t nbmsf;
    static float objlocx, objlocy, objlocz;
    uint32_t retflags = REDRAWVIEW | REDRAWCOORDS;

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            glGetIntegerv ( GL_VIEWPORT, VPX );

            xori = xold = bev->x;
            yori = yold = bev->y;

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
                memcpy ( &objrot, &obj->rot, sizeof ( G3DVECTOR ) );

                rotx = obj->rot.x;
                roty = obj->rot.y;
                rotz = obj->rot.z;

                locrotx = 0.0f;
                locroty = 0.0f;
                locrotz = 0.0f;

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

                    mes = ( G3DMESH * ) obj;

                    /*** In other modes, move vertices ***/
                    if ( flags & VIEWVERTEX ) {
                        lver = g3dmesh_getVertexListFromSelectedVertices ( mes );
                    }

                    if ( flags & VIEWFACE ) {
                        lver = g3dmesh_getVertexListFromSelectedFaces ( mes );
                    }

                    if ( flags & VIEWEDGE ) {
                        lver = g3dmesh_getVertexListFromSelectedEdges ( mes );
                    }

                    lfac = g3dvertex_getFacesFromList ( lver );
                    ledg = g3dface_getEdgesFromList   ( lfac );

                    /*if ( mes->subdiv && (objmes->flags & BUFFEREDSUBDIVISION)){
                        lsub = g3dvertex_getAreaFacesFromList ( lver );
                    }*/

                    /*** for the undo/redo manager ***/
                    nbver = g3dvertex_copyPositionFromList ( lver, &oldpos );

                    g3dmesh_startUpdateModifiers_r ( obj, flags );

                    vec = oldpos;

                    /*** get scaling center. It is the average pos of vertices ***/
                    /*** we are going to move ***/
                    g3dmesh_getCenterFromVertexList ( lver, &center );
                }
            }

        } return  REDRAWVIEW | REDRAWCOORDS;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;

            if ( obj ) {

                if ( obj->type == G3DUVMAPTYPE ) retflags |= NOBUFFEREDSUBDIVISION;

                if ( ( flags & VIEWOBJECT ) ||
                     ( flags & VIEWAXIS   ) || 
                     ( flags & VIEWSKIN   ) || 
                     ( flags & VIEWUVWMAP ) ) {
                    if ( ( flags & XAXIS ) && sce->csr.axis[0x00].w ) {
                        rotx = ( xold - mev->x );

                        glLoadMatrixd ( obj->rmatrix );
                        glRotatef ( rotx, 1.0f, 0.0f, 0.0f );
                        glGetDoublev ( GL_MODELVIEW_MATRIX, obj->rmatrix );
                    }

                    if ( ( flags & YAXIS ) && sce->csr.axis[0x01].w ) {
                        roty = ( xold - mev->x );

                        glLoadMatrixd ( obj->rmatrix );
                        glRotatef ( roty, 0.0f, 1.0f, 0.0f );
                        glGetDoublev ( GL_MODELVIEW_MATRIX, obj->rmatrix );
                    }

                    if ( ( flags & ZAXIS ) && sce->csr.axis[0x02].w ) {
                        rotz = ( xold - mev->x );

                        glLoadMatrixd ( obj->rmatrix );
                        glRotatef ( rotz, 0.0f, 0.0f, 1.0f );
                        glGetDoublev ( GL_MODELVIEW_MATRIX, obj->rmatrix );
                    }

                    g3dcore_getMatrixRotation ( obj->rmatrix, &obj->rot );

                    g3dobject_updateMatrix_r ( obj, flags );

                    /* move vertices */
                    if ( flags & VIEWAXIS ) {
                        if ( ( obj->type & MESH ) ||
                             ( obj->type & SPLINE ) ) {
                            g3dmesh_moveAxis ( obj, FINX, flags );

                            memcpy ( FINX, obj->wmatrix, sizeof ( FINX ) );
                        }
                    }
                } else {
                    if ( obj->type & EDITABLE ) {
                        double rmatrix[0x10];
                        LIST *ltmp = lver;
                        mes = ( G3DMESH * ) obj;
                        int i = 0x00;

                        if ( ( flags & XAXIS ) && sce->csr.axis[0x00].w ) {
                                locrotx += ( xold - mev->x );
                        }

                        if ( ( flags & YAXIS ) && sce->csr.axis[0x01].w ) {
                                locroty += ( xold - mev->x );
                        }

                        if ( ( flags & ZAXIS ) && sce->csr.axis[0x02].w ) {
                                locrotz += ( xold - mev->x );
                        }

                        glPushMatrix ( );
                        glLoadIdentity ( );
                        glTranslatef ( center.x, center.y, center.z );
                        glRotatef ( locrotx, 1.0f, 0.0f, 0.0f );
                        glRotatef ( locroty, 0.0f, 1.0f, 0.0f );
                        glRotatef ( locrotz, 0.0f, 0.0f, 1.0f );
                        glTranslatef ( -center.x, -center.y, -center.z );
                        glGetDoublev ( GL_MODELVIEW_MATRIX, rmatrix );
                        glPopMatrix ( );

                        while ( ltmp ) {
                            G3DVERTEX *ver = ltmp->data;
                            G3DVECTOR vout;

                            g3dvector_matrix ( &oldpos[i++], rmatrix, &vout );

                            ver->pos.x = vout.x;
                            ver->pos.y = vout.y;
                            ver->pos.z = vout.z;

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
                }

                xold = mev->x;
                yold = mev->y;

                orix = objx;
                oriy = objy;
                oriz = objz;

                return  retflags;
            }
        } return 0x00;

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
                                                       &objrot,
                                                       &obj->sca,
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
            list_free ( &lfac, NULL );
            list_free ( &lsub, NULL );

            mes  = NULL;
            obj  = NULL;

            oldpos = newpos = NULL;
        } return REDRAWVIEW | REDRAWCOORDS | BUFFEREDSUBDIVISIONOK | REDRAWCURRENTOBJECT;

        default :
        break;
    }

    return FALSE;
}
