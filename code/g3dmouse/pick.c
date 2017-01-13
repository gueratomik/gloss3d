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
G3DPICKTOOL *pickTool_new ( ) {
    uint32_t structsize = sizeof ( G3DPICKTOOL );

    G3DPICKTOOL *pt =  ( G3DPICKTOOL * ) calloc ( 0x01, structsize );

    if ( pt == NULL ) {
        fprintf ( stderr, "pickTool_new: Memory allocation failed\n" );
    }

    pt->only_visible = 0x01;

    pt->operation    = 0x01; /*** add vertex weight ***/
    pt->weight       = 1.0f;
    pt->paint_radius = 0x08;

    return pt;
}

/******************************************************************************/
uint32_t pickTool_init ( G3DMOUSETOOL *mou, G3DSCENE *sce, 
                                            G3DCAMERA *cam,
                                            G3DURMANAGER *urm, 
                                            uint32_t engine_flags ) {
    if ( mou->data ) {
        G3DPICKTOOL *pt = mou->data;

    } else {
        mou->data = pickTool_new ( );
    }

    return 0x00;
}

/******************************************************************************/
void pick_draw ( G3DMOUSETOOL *mou, G3DSCENE *sce, uint32_t flags ) {
    G3DPICKTOOL *pt = ( G3DPICKTOOL * ) mou->data;
    int32_t *coord = pt->coord;
    static GLdouble MVX[0x10], PJX[0x10];
    static GLint VPX[0x04];
    static double winx, winy, winz, rec[0x04][0x03];
    int i;

    if ( pt->start ) {
        uint32_t index[0x04][0x02] = { { coord[0x00], coord[0x01] },   /*x1,y1*/
                                       { coord[0x02], coord[0x01] },   /*x2,y1*/
                                       { coord[0x02], coord[0x03] },   /*x1,y2*/
                                       { coord[0x00], coord[0x03] } }; /*x2,y2*/

        glGetDoublev  ( GL_MODELVIEW_MATRIX, MVX );
        glGetDoublev  ( GL_PROJECTION_MATRIX, PJX );
        glGetIntegerv ( GL_VIEWPORT, VPX );

        gluProject ( 0.0f, 0.0f, 0.0f, MVX, PJX, VPX, &winx, &winy, &winz );
        for ( i = 0x00; i < 0x04; i++ ) {
            gluUnProject ( ( GLdouble ) index[i][0x00],
                           ( GLdouble ) index[i][0x01],
                           ( GLdouble ) winz,
                           MVX, PJX, VPX,
                           &rec[i][0x00], &rec[i][0x01], &rec[i][0x02] );
        }

        glPushAttrib( GL_ALL_ATTRIB_BITS );
        glDisable   ( GL_DEPTH_TEST );
        glDisable   ( GL_LIGHTING );
        glColor3ub  ( 0xFF, 0x00, 0x00 );

        glBegin ( GL_LINES );
        for ( i = 0x00; i < 0x04; i++ ) {
            int n = ( i + 0x01 ) % 0x04;

            glVertex3dv ( rec[i] );
            glVertex3dv ( rec[n] );
        }
        glEnd ( );

        glPopAttrib ( );
    }
}

/******************************************************************************/
/************* Rectangle array is { 0=>x1, 1=>y1, 2=>x2, 3=>y2 } **************/
static void startSelectionRectangle ( int x, int y, int32_t *coord ) {
    coord[0x00] = coord[0x02] = x;
    coord[0x01] = coord[0x03] = y;
}

/******************************************************************************/
static void shapeSelectionRectangle ( int x, int y, int32_t *coord ) {
    coord[0x02] = ( x < 0x00 ) ? 0x00 : x;
    coord[0x03] = ( y < 0x00 ) ? 0x00 : y;
}

/******************************************************************************/
/*** Var radius is the selection rectangle size in case x1 = x2 or y1 = y2  ***/
static void closeSelectionRectangle ( int32_t *coord ) {
    if ( coord[0x00] > coord[0x02] ) {
        int32_t tmp = coord[0x00];

        coord[0x00] = coord[0x02];
        coord[0x02] = tmp;
    }

    if ( coord[0x01] > coord[0x03] ) {
        int32_t tmp = coord[0x01];

        coord[0x01] = coord[0x03];
        coord[0x03] = tmp;
    }
}

/******************************************************************************/
void pick_Item ( G3DPICKTOOL *pt, G3DOBJECT *obj, G3DCAMERA *cam,
                 int32_t *coord, uint32_t unselectIfSelected, uint32_t flags ) {
    int MX = coord[0x00] + ( ( coord[0x02] - coord[0x00] ) >> 0x01 ),
        MY = coord[0x01] + ( ( coord[0x03] - coord[0x01] ) >> 0x01 ),
        MW = ( coord[0x02] - coord[0x00] ), /*** width ***/
        MH = ( coord[0x03] - coord[0x01] ); /*** height ***/
    G3DVECTOR vec = { 0.f, 0.f, 0.f, 1.0f };
    static GLint VPX[0x04];
    static double MVX[0x10];
    static double PJX[0x10];
    int middle = 0x00;

    if ( MW == 0x00 ) { MW = 0x10; middle = 0x01; };
    if ( MH == 0x00 ) { MH = 0x10; middle = 0x01; };

    glGetIntegerv ( GL_VIEWPORT, VPX );

    glMatrixMode ( GL_PROJECTION );
    glPushMatrix ( );
    glLoadIdentity ( );
    gluPickMatrix ( MX, MY, MW, MH, VPX );
    g3dcamera_project ( cam, flags );
    glGetDoublev ( GL_PROJECTION_MATRIX, PJX );

    glMatrixMode ( GL_MODELVIEW );
    glPushMatrix ( );
    glLoadIdentity ( );
    g3dcamera_view ( cam, 0x00 );

    if ( ( ( flags & VIEWOBJECT ) == 0x00 ) && ( obj->type & EDITABLE ) ) {
        glMultMatrixd ( obj->wmatrix );
        glGetDoublev ( GL_MODELVIEW_MATRIX, MVX );

        if ( flags & VIEWVERTEX ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            LIST *lfac;
            LIST *loldselver = list_copy ( mes->lselver );

            LIST *lnewselver = g3dmesh_pickVertices ( mes, cam, pt->only_visible, flags );

            LIST *ltmp = lnewselver;

            while ( ltmp ) {
                G3DVERTEX *ver = ( G3DVERTEX * ) ltmp->data;

                if ( list_seek ( mes->lselver, ver ) == NULL ) {
                    g3dmesh_selectVertex   ( mes, ver );
                } else {
                    if ( unselectIfSelected ) {
                        g3dmesh_unselectVertex ( mes, ver );
                    }
                }

                ltmp = ltmp->next;
            }

            list_free ( &loldselver, NULL );
            list_free ( &lnewselver, NULL );
        }

        if ( flags & VIEWFACE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            LIST *loldselfac = list_copy ( mes->lselfac );
            LIST *lnewselfac = g3dmesh_pickFace ( mes, cam, MX, MY, pt->only_visible, flags | SELECTMODE );
            LIST *ltmpfac = lnewselfac;

            while ( ltmpfac ) {
                G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

                if ( list_seek ( mes->lselfac, fac ) == NULL ) {
                    g3dmesh_selectFace   ( mes, fac );
                } else {
                    if ( unselectIfSelected ) {
                        g3dmesh_unselectFace ( mes, fac );
                    }
                }

                ltmpfac = ltmpfac->next;
            }

            list_free ( &loldselfac, NULL );
            list_free ( &lnewselfac, NULL );
        }

/*printf ( "Selected face:%d\n", list_count(mes->lselfac) );*/

        if ( flags & VIEWEDGE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            LIST *loldseledg = list_copy ( mes->lseledg );
            LIST *lnewseledg = g3dmesh_pickEdge ( mes, cam, MX, MY, pt->only_visible, flags | SELECTMODE );
            LIST *ltmpedg = lnewseledg;

            while ( ltmpedg ) {
                G3DEDGE *edg = ( G3DEDGE * ) ltmpedg->data;

                if ( list_seek ( mes->lseledg, edg ) == NULL ) {
                    g3dmesh_selectEdge   ( mes, edg );
                } else {
                    if ( unselectIfSelected ) {
                        g3dmesh_unselectEdge ( mes, edg );
                    }
                }

                ltmpedg = ltmpedg->next;
            }

            list_free ( &loldseledg, NULL );
            list_free ( &lnewseledg, NULL );
        }
    }

    glPopMatrix ( );

    glPushMatrix ( );
    glLoadIdentity ( );
    g3dcamera_view ( cam, 0x00 );

    if ( flags & VIEWOBJECT ) {
        G3DSCENE *sce = ( G3DSCENE * ) obj;
        LIST *loldselobj = list_copy ( sce->lsel );
        LIST *lnewselobj = g3dscene_pick ( ( G3DOBJECT * ) sce, cam, flags | SELECTMODE );
        LIST *ltmobj = lnewselobj;

        while ( ltmobj ) {
            G3DOBJECT *sel = ( G3DOBJECT * ) ltmobj->data;

            if ( list_seek ( sce->lsel, sel ) == NULL ) {
                g3dscene_selectObject   ( sce, sel, flags );
            } else {
                if ( unselectIfSelected ) {
                    g3dscene_unselectObject ( sce, sel, flags );
                }
            }

            ltmobj = ltmobj->next;
        }
        
    }
    glPopMatrix ( );

    glMatrixMode ( GL_PROJECTION );
    glPopMatrix ( );
}

/******************************************************************************/
/*static void pick_undo ( void *data ) {
    URMPICK *urp = ( URMPICK * ) data;
    G3DOBJECT *obj = urp->obj;

    if ( urp->flags & VIEWOBJECT ) {
        G3DSCENE *sce = ( G3DSCENE * ) urp->obj;

        if ( sce ) {
            sce->lsel = urp->lselold;
        }
    }

    if ( urp->flags & VIEWFACE ) {
        G3DMESH *mes = ( G3DMESH * ) urp->obj;

        if ( mes ) {
            mes->lselfac = urp->lselold;
        }
    }

    if ( urp->flags & VIEWVERTEX ) {
        G3DMESH *mes = ( G3DMESH * ) urp->obj;

        if ( mes ) {
            mes->lselver = urp->lselold;
        }
    }
}*/

/******************************************************************************/
/*static void pick_redo ( void *data ) {
    URMPICK *urp = ( URMPICK * ) data;
    G3DOBJECT *obj = urp->obj;

    if ( urp->flags & VIEWOBJECT ) {
        G3DSCENE *sce = ( G3DSCENE * ) urp->obj;

        if ( sce ) {
            sce->lsel = urp->lselnew;
        }
    }

    if ( urp->flags & VIEWFACE ) {
        G3DMESH *mes = ( G3DMESH * ) urp->obj;

        if ( mes ) {
            mes->lselfac = urp->lselnew;
        }
    }

    if ( urp->flags & VIEWVERTEX ) {
        G3DMESH *mes = ( G3DMESH * ) urp->obj;

        if ( mes ) {
            mes->lselver = urp->lselnew;
        }
    }
}*/

/******************************************************************************/
static void pick_free ( void *data ) {

}

/******************************************************************************/
/*static void pick_push ( G3DURMANAGER *urm, G3DOBJECT *obj,
                        LIST *lselold, LIST *lselnew, uint32_t flags ) {
    URMPICK *urp = ( URMPICK * ) calloc ( 0x01, sizeof ( URMPICK ) );

    if ( urp == NULL ) {
        fprintf ( stderr, "pick_push: memory allocation failed\n" );

        return NULL;
    }

    urp->obj = obj;
    urp->flags = flags;
    urp->lselold = lselold;
    urp->lselnew = lselnew;

    g3durmanager_push ( urm, pick_undo,
                             pick_redo,
                             pick_free, urp );
}*/

/******************************************************************************/
int pick_tool ( G3DMOUSETOOL *mou, G3DSCENE *sce, G3DCAMERA *cam,
                G3DURMANAGER *urm, uint32_t flags, G3DEvent *event ) {
    /*G3DURMANAGER *urm = gdt->urm;*/
    /*** selection rectangle coords ***/
    static GLint VPX[0x04];
    G3DPICKTOOL *pt = mou->data;
    uint32_t unselectIfSelected = 0x01;

    if ( flags & VIEWSKIN ) {
        return paintWeight_tool ( mou, sce, cam, urm, flags, event );
    }

    switch ( event->type ) {
        case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            glGetIntegerv ( GL_VIEWPORT, VPX );

            pt->start = 0x01;

            startSelectionRectangle ( bev->x, VPX[0x03] - bev->y, pt->coord );
        } return REDRAWALL;

        case G3DMotionNotify : {
            /*** if G3DButtonPress was called ***/
            if ( pt->start ) {
                G3DMotionEvent *mev = ( G3DMotionEvent * ) event;
                glGetIntegerv ( GL_VIEWPORT, VPX );

                shapeSelectionRectangle ( mev->x, VPX[0x03] - mev->y, pt->coord );

                return REDRAWVIEW;
            }
        } return 0x00;

        case G3DButtonRelease : {
            G3DVECTOR vec = { 0.f, 0.f, 0.f, 1.0f };
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;
            G3DOBJECT *obj = NULL;
            LIST *lselold = NULL;
            LIST *lselnew = NULL;

            closeSelectionRectangle ( pt->coord );

            /*********************************/
            if ( ( flags & VIEWVERTEX ) ) {
                obj = g3dscene_getLastSelected ( sce );

                if ( obj && ( obj->type & EDITABLE ) ) {
                    G3DMESH *mes = ( G3DMESH * ) obj;
                    lselold = list_copy ( mes->lselver );

                    if ( ( bev->state & G3DControlMask ) == 0x00 ) {
                        g3dmesh_unselectAllVertices ( mes );
                    }

                    pick_Item ( pt, obj, cam, pt->coord, unselectIfSelected, flags );

                    lselnew = list_copy ( mes->lselver );

                    /*** remember selection ***/
                    g3durm_mesh_pickVertices  ( urm, mes,
                                                     lselold,
                                                     lselnew,
                                                     flags,
                                                     REDRAWVIEW );
                }
            }

            /*********************************/
            if ( ( flags & VIEWEDGE ) ) {
                obj = g3dscene_getLastSelected ( sce );

                if ( obj && ( obj->type & EDITABLE ) ) {
                    G3DMESH *mes = ( G3DMESH * ) obj;
                    lselold = list_copy ( mes->lseledg );

                    if ( ( bev->state & G3DControlMask ) == 0x00 ) {
                        g3dmesh_unselectAllEdges ( mes );
                    }

                    pick_Item ( pt, obj, cam, pt->coord, unselectIfSelected, flags );


                    lselnew = list_copy ( mes->lseledg );
                }
            }

            /*********************************/
            if ( ( flags & VIEWFACE ) ) {
                obj = g3dscene_getLastSelected ( sce );

                if ( obj && ( obj->type & EDITABLE ) ) {
                    G3DMESH *mes = ( G3DMESH * ) obj;
                    lselold = list_copy ( mes->lselfac );

                    if ( ( bev->state & G3DControlMask ) == 0x00 ) {
                        g3dmesh_unselectAllFaces ( mes );
                    }

                    pick_Item ( pt, obj, cam, pt->coord, unselectIfSelected, flags );

                    lselnew = list_copy ( mes->lselfac );

                    /*** remember selection ***/
                    g3durm_mesh_pickFaces  ( urm, mes,
                                                  lselold,
                                                  lselnew,
                                                  flags,
                                                  REDRAWVIEW );

                    g3dmesh_update ( mes, NULL,
                                          NULL,
                                          NULL,
                                          RESETMODIFIERS, flags );
                }
            }

            /*********************************/
            if ( ( flags & VIEWOBJECT ) ) {
                /*** copy list for undo/redo manager ***/
                lselold = list_copy ( sce->lsel );
                obj = ( G3DOBJECT * ) sce;

                if ( ( bev->state & G3DControlMask ) == 0x00 ) {
                    g3dscene_unselectAllObjects ( sce, flags );
                }

                pick_Item ( pt, obj, cam, pt->coord, unselectIfSelected, flags );

                /*** copy list for undo/redo manager ***/
                lselnew = list_copy ( sce->lsel );

                /*** remember selection ***/
                g3durm_scene_pickObject  ( urm, sce,
                                                lselold,
                                                lselnew,
                                                flags,
                                                REDRAWVIEW );
            }

            /*** Push our selected items into the undo/redo stack ***/
            /*pick_push ( urm, obj, lselold, lselnew, gdt->flags );*/

            pt->start = 0x00;
        } return REDRAWALL;

        default :
        break;
    }

    return 0x00;
}
