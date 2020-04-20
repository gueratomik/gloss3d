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
G3DPICKTOOL *pickTool_new ( ) {
    uint32_t structsize = sizeof ( G3DPICKTOOL );

    G3DPICKTOOL *pt =  ( G3DPICKTOOL * ) calloc ( 0x01, structsize );

    if ( pt == NULL ) {
        fprintf ( stderr, "pickTool_new: Memory allocation failed\n" );
    }

    pt->only_visible  = 0x01;

    pt->operation     = 0x01; /*** add vertex weight ***/
    pt->weight        = 1.0f;
    pt->radius        = 0x08;
    pt->unselectFirst = 0x01;

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
    int i;

    if ( pt->start ) {
        uint32_t index[0x04][0x02] = { { coord[0x00], coord[0x01] },   /*x1,y1*/
                                       { coord[0x02], coord[0x01] },   /*x2,y1*/
                                       { coord[0x02], coord[0x03] },   /*x1,y2*/
                                       { coord[0x00], coord[0x03] } }; /*x2,y2*/

        glGetIntegerv ( GL_VIEWPORT, VPX );

        glPushAttrib( GL_ALL_ATTRIB_BITS );
        glDisable   ( GL_DEPTH_TEST );
        glDisable   ( GL_LIGHTING );
        glColor3ub  ( 0xFF, 0x00, 0x00 );

        glMatrixMode ( GL_PROJECTION );
        glPushMatrix ( );
        glLoadIdentity ( );
        gluOrtho2D (VPX[0x00],  VPX[0x02],  VPX[0x01],  VPX[0x03] );

        glMatrixMode ( GL_MODELVIEW );
        glPushMatrix ( );
        glLoadIdentity ( );

        glBegin ( GL_LINES );
        for ( i = 0x00; i < 0x04; i++ ) {
            int n = ( i + 0x01 ) % 0x04;

            glVertex2f ( index[i][0x00], index[i][0x01] );
            glVertex2f ( index[n][0x00], index[n][0x01] );
        }
        glEnd ( );

        glPopAttrib ( );

        glPopMatrix ( );

        glMatrixMode ( GL_PROJECTION );
        glPopMatrix ( );
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
static void closeSelectionRectangle ( G3DPICKTOOL *pt, int *VPX ) {
    int x1 = VPX[0x00];
    int y1 = VPX[0x01];
    int x2 = VPX[0x00] + VPX[0x02];
    int y2 = VPX[0x01] + VPX[0x03];

    if ( pt->coord[0x00] == pt->coord[0x02] ) {
        pt->coord[0x00] -= pt->radius;
        pt->coord[0x02] += pt->radius;
    }

    if ( pt->coord[0x01] == pt->coord[0x03] ) {
        pt->coord[0x01] -= pt->radius;
        pt->coord[0x03] += pt->radius;
    }

    if ( pt->coord[0x00] > pt->coord[0x02] ) {
        int32_t tmp = pt->coord[0x00];

        pt->coord[0x00] = pt->coord[0x02];
        pt->coord[0x02] = tmp;
    }

    if ( pt->coord[0x01] > pt->coord[0x03] ) {
        int32_t tmp = pt->coord[0x01];

        pt->coord[0x01] = pt->coord[0x03];
        pt->coord[0x03] = tmp;
    }

    /*** check boundaries ***/
    if ( pt->coord[0x00] < x1 ) pt->coord[0x00] = x1;
    if ( pt->coord[0x01] < y1 ) pt->coord[0x01] = y1;

    if ( pt->coord[0x00] > x2 ) pt->coord[0x00] = x2;
    if ( pt->coord[0x01] > y2 ) pt->coord[0x01] = y2;

    if ( pt->coord[0x02] < x1 ) pt->coord[0x02] = x1;
    if ( pt->coord[0x03] < y1 ) pt->coord[0x03] = y1;

    if ( pt->coord[0x02] > x2 ) pt->coord[0x02] = x2;
    if ( pt->coord[0x03] > y2 ) pt->coord[0x03] = y2;
}

/******************************************************************************/
typedef struct _SCENEPICKDATA {
    G3DSCENE *sce;
    uint64_t  flags;
} SCENEPICKDATA;

/******************************************************************************/
uint32_t actionSelectObject ( uint64_t name, SCENEPICKDATA *spd ) {
    if ( list_seek ( spd->sce->lsel, ( G3DOBJECT * ) name ) == NULL ) {
        g3dscene_selectObject ( spd->sce, ( G3DOBJECT * ) name, spd->flags );
    }

    return 0x01;
}

/******************************************************************************/
typedef struct _MESHPICKDATA {
    G3DMESH  *mes;
    uint64_t  flags;
    float     weight; /* for vertex painting (skin) */
} MESHPICKDATA;

/******************************************************************************/
uint32_t actionSelectFace ( uint64_t name, MESHPICKDATA *mpd ) {
    G3DFACE *fac = ( G3DFACE * ) name;

    if ( list_seek ( mpd->mes->lselver, fac ) == NULL ) {
        g3dmesh_selectFace ( mpd->mes, fac );
    }

    return 0x01;
}

/******************************************************************************/
uint32_t actionSelectEdge ( uint64_t name, MESHPICKDATA *mpd ) {
    G3DEDGE *edg = ( G3DEDGE * ) name;

    if ( list_seek ( mpd->mes->lseledg, edg ) == NULL ) {
        g3dmesh_selectFace ( mpd->mes, edg );
    }

    return 0x01;
}

/******************************************************************************/
uint32_t actionSelectVertex ( uint64_t name, MESHPICKDATA *mpd ) {
    G3DVERTEX *ver = ( G3DVERTEX * ) name;

    if ( list_seek ( mpd->mes->lselver, ver ) == NULL ) {
        g3dmesh_selectVertex ( mpd->mes, ver );
    }

    return 0x01;
}

/******************************************************************************/
uint32_t actionPaintVertex ( uint64_t name, MESHPICKDATA *mpd ) {

/* ( G3DMESH *mes, 
                             G3DCAMERA *curcam,
                             float weight,
                             uint32_t visible, uint32_t eflags ) {*/
    G3DMESH *mes = mpd->mes;
    G3DVERTEX *ver = ( G3DVERTEX * ) name;
    G3DWEIGHTGROUP *curgrp = mes->curgrp;
    float weight = mpd->weight;

    /*** a Weightgroup must be selected first ***/
    if ( curgrp == NULL ) return;

    G3DWEIGHT *wei = g3dweightgroup_seekVertex ( curgrp, ver );

    /*** Remove vertex from weightgroup ***/
    if ( weight == 0.0f ) {
        if ( wei ) {
            g3dweightgroup_removeWeight ( curgrp, wei );
        }
    /*** Add vertex to weighgroup ***/
    } else {
        if ( wei ) {
            /* Note: wei->ver->weight is just a storing */
            /* variable for faster display of the weight */
            wei->ver->weight = wei->weight = weight;
        } else {
            wei = g3dweightgroup_addVertex ( curgrp, ver, 1.0f );
        }

        ver->flags |= VERTEXPAINTED;
    }

    /*g3dmesh_update ( mes, lselver,
                          NULL,
                          NULL,
                          UPDATEMODIFIERS,
                          eflags );*/

    return 0x01;
}

/******************************************************************************/
typedef struct _SPLINEPICKDATA {
    G3DSPLINE *spl;
    uint64_t  flags;
} SPLINEPICKDATA;

/******************************************************************************/
uint32_t actionSelectPoint ( uint64_t name, SPLINEPICKDATA *spd ) {
    G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) name;

    if ( pt->flags & CURVEPOINTISPOINT ) {
	if ( list_seek ( spd->spl->curve->lselpt, pt ) == NULL ) {
            g3dcurve_selectPoint ( spd->spl->curve, pt );
	}
    }

    if ( pt->flags & CURVEPOINTISHANDLE ) {
        spd->spl->curve->curhan = pt;
    }

    return 0x01;
}

/******************************************************************************/
/*** We basically draw the scene in 2D with pointer values as pixels ***/
void pick_Item ( G3DPICKTOOL *pt, 
                 G3DSCENE   *sce, 
                 G3DCAMERA  *cam,
                 uint32_t    eflags ) {
    static GLint  VPX[0x04];
    static double MVX[0x10];
    static double PJX[0x10];


    glGetIntegerv ( GL_VIEWPORT, VPX );

    closeSelectionRectangle ( pt, VPX );

    glMatrixMode ( GL_PROJECTION );
    glPushMatrix ( );
    glLoadIdentity ( );

    g3dcamera_project ( cam, eflags );
    glGetDoublev ( GL_PROJECTION_MATRIX, PJX );

    glMatrixMode ( GL_MODELVIEW );
    glPushMatrix ( );
    glLoadIdentity ( );
    g3dcamera_view ( cam, 0x00 );

    g3dpick_setProjectionMatrix ( PJX   );
    g3dpick_setViewportMatrix   ( VPX   );
    g3dpick_setAreaMatrix       ( pt->coord );

    g3dpick_clear ( );

    if ( pt->only_visible ) {
        g3dpick_setAction ( NULL, NULL );
        g3dobject_pick ( sce, cam, VIEWOBJECT );
        g3dpick_setEpsilon ( 0.00001f );
    }

    if ( eflags & VIEWOBJECT ) {
        SCENEPICKDATA spd = { .sce   =  sce,
                              .flags = eflags };

        if ( pt->unselectFirst ) g3dscene_unselectAllObjects ( sce, eflags );
        g3dpick_setAction ( actionSelectObject, &spd );
        g3dobject_pick ( sce, cam, VIEWOBJECT );
    } else {
        G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

	if ( obj ) {
	    if ( obj->type & MESH ) {
        	G3DMESH *mes = ( G3DMESH * ) obj;
        	MESHPICKDATA mpd = { .mes    = mes,
                        	     .weight = pt->weight,
                        	     .flags  = eflags };

		if ( eflags & VIEWFACE ) {
                    if ( pt->unselectFirst ) g3dmesh_unselectAllFaces ( mes );
        	    g3dpick_setAction ( actionSelectFace, &mpd );
        	    g3dobject_pick ( sce, cam, VIEWFACE );
		}

		if ( eflags & VIEWEDGE ) {
                    if ( pt->unselectFirst ) g3dmesh_unselectAllEdges ( mes );
        	    g3dpick_setAction ( actionSelectEdge, &mpd );
        	    g3dobject_pick ( sce, cam, VIEWEDGE );
		}

		if ( eflags & VIEWVERTEX ) {
                    if ( pt->unselectFirst ) g3dmesh_unselectAllVertices ( mes );
        	    g3dpick_setAction ( actionSelectVertex, &mpd );
        	    g3dobject_pick ( sce, cam, VIEWVERTEX );
		}

		if ( eflags & VIEWSKIN ) {
        	    g3dpick_setAction ( actionPaintVertex, &mpd );
        	    g3dobject_pick ( sce, cam, VIEWVERTEX );
		}
	    }

	    if ( obj->type & SPLINE ) {
        	G3DSPLINE *spl = ( G3DSPLINE * ) obj;
        	SPLINEPICKDATA spd = { .spl   =  ( G3DSPLINE * ) obj,
                        	       .flags = eflags };

		if ( eflags & VIEWVERTEX ) {
                    if ( pt->unselectFirst ) g3dcurve_unselectAllPoints ( spl->curve );
        	    g3dpick_setAction ( actionSelectPoint, &spd );
        	    g3dobject_pick ( sce, cam, VIEWVERTEX );
		}
	    }
	}
    }

    glPopMatrix ( );

    glMatrixMode ( GL_PROJECTION );
    glPopMatrix ( );
}

/******************************************************************************/
uint32_t actionSelectAxis ( uint64_t name, G3DCURSOR *csr ) {
    if ( name == CURSORXAXIS ) csr->axis[0x00].w = 2.0f;
    if ( name == CURSORYAXIS ) csr->axis[0x01].w = 2.0f;
    if ( name == CURSORZAXIS ) csr->axis[0x02].w = 2.0f;

    return 0x01;
}

/******************************************************************************/
/*** We basically draw the scene in 2D with pointer values as pixels ***/
void pick_cursor ( G3DPICKTOOL *pt, 
                   G3DSCENE   *sce, 
                   G3DCAMERA  *cam,
                   uint32_t    eflags ) {
    static GLint  VPX[0x04];
    static double MVX[0x10];
    static double PJX[0x10];

    g3dscene_getSelectionMatrix ( sce, MVX, eflags );

    glGetIntegerv ( GL_VIEWPORT, VPX );

    /*** check boundaries ***/
    closeSelectionRectangle ( pt, VPX );

    glMatrixMode ( GL_PROJECTION );
    glPushMatrix ( );
    glLoadIdentity ( );
    g3dcamera_project ( cam, eflags );
    glGetDoublev ( GL_PROJECTION_MATRIX, PJX );

    glMatrixMode ( GL_MODELVIEW );
    glPushMatrix ( );
    glLoadIdentity ( );
    g3dcamera_view ( cam, 0x00 );
    glMultMatrixd ( MVX );
    glGetDoublev ( GL_MODELVIEW_MATRIX, MVX );

    g3dpick_setModelviewMatrix  ( MVX   );
    g3dpick_setProjectionMatrix ( PJX   );
    g3dpick_setViewportMatrix   ( VPX   );
    g3dpick_setAreaMatrix       ( pt->coord );

    g3dpick_clear ( );

    g3dpick_setAction ( actionSelectAxis, &sce->csr );
    g3dcursor_pick ( &sce->csr, cam, eflags );

    glPopMatrix ( );

    glMatrixMode ( GL_PROJECTION );
    glPopMatrix ( );
}

/******************************************************************************/
static void pick_free ( void *data ) {

}

/******************************************************************************/
int weight_tool ( G3DMOUSETOOL *mou, G3DSCENE *sce, G3DCAMERA *cam,
                  G3DURMANAGER *urm, uint32_t flags, G3DEvent *event ) {
    G3DPICKTOOL *pt = mou->data;
    static G3DOBJECT *obj = NULL;
    static int VPX[0x04];

    switch ( event->type ) {
	case G3DButtonPress : {
            G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

            obj = g3dscene_getLastSelected ( sce );

            if ( obj ) {
        	if ( obj->type == G3DMESHTYPE ) {
        	    G3DMESH *mes = ( G3DMESH * ) obj;

        	    /*** If there is no weightGroup, add one ***/
        	    if ( mes->lgrp == NULL ) {
                	G3DWEIGHTGROUP *grp;
                	char buf[0x20];

                	snprintf ( buf, 0x20, "VertexWeightGroup%02i", mes->nbgrp );

                	grp = g3dweightgroup_new ( mes, buf );

                	g3dmesh_addWeightGroup ( mes, grp );

                	mes->curgrp = grp;
        	    }
        	}
            }

            glGetIntegerv ( GL_VIEWPORT, VPX );
	} return REDRAWVIEW | REDRAWCURRENTOBJECT;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;

            if ( obj ) {
        	if ( obj->type == G3DMESHTYPE ) {
        	    pt->coord[0x00] = pt->coord[0x02] = mev->x;
        	    pt->coord[0x01] = pt->coord[0x03] = VPX[0x03] - mev->y;

        	    closeSelectionRectangle ( pt, VPX );

                    pick_Item ( pt, sce, cam, flags );
        	}
            }
        } return REDRAWVIEW;

        case G3DButtonRelease : {
            obj = NULL;
        } break;

        default :
        break;
    }

    return 0x00;
}

/******************************************************************************/
int pick_tool ( G3DMOUSETOOL *mou, G3DSCENE *sce, G3DCAMERA *cam,
                G3DURMANAGER *urm, uint32_t flags, G3DEvent *event ) {
    /*G3DURMANAGER *urm = gdt->urm;*/
    /*** selection rectangle coords ***/
    static GLint VPX[0x04];
    G3DPICKTOOL *pt = mou->data;
    uint32_t unselectIfSelected = 0x01;

    if ( flags & VIEWSKIN ) {
        return weight_tool ( mou, sce, cam, urm, flags, event );
    } else {
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

                    return REDRAWVIEW | REDRAWUVMAPEDITOR;
        	}
            } return 0x00;

            case G3DButtonRelease : {
        	G3DVECTOR vec = { 0.f, 0.f, 0.f, 1.0f };
        	G3DButtonEvent *bev = ( G3DButtonEvent * ) event;
        	G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
        	LIST *lselold = NULL;
        	LIST *lselnew = NULL;

                pt->unselectFirst = ( bev->state & G3DControlMask ) ? 0 : 1;

        	/*********************************/
        	if ( flags & VIEWOBJECT ) {
                    /*** copy list for undo/redo manager ***/
                    lselold = list_copy ( sce->lsel );

                    obj = ( G3DOBJECT * ) sce;

                    pick_Item ( pt, sce, cam, flags );

                    /*** copy list for undo/redo manager ***/
                    lselnew = list_copy ( sce->lsel );

                    if ( lselold || lselnew ) {
                	/*** remember selection ***/
                	g3durm_scene_pickObject  ( urm, sce,
                                                	lselold,
                                                	lselnew,
                                                	flags,
                                                	REDRAWVIEW );
                    }
        	}

        	if ( obj ) {
                    if ( obj->type & SPLINE ) {
                	G3DSPLINE *spl = ( G3DSPLINE * ) obj;

        		if ( flags & VIEWVERTEX ) {
                	    lselold = list_copy ( spl->curve->lselpt );

                	    pick_Item ( pt, sce, cam, flags );

                	    lselnew = list_copy ( spl->curve->lselpt );
                	}
                    }

                    if ( obj->type & MESH ) {
                	G3DMESH *mes = ( G3DMESH * ) obj;

        		if ( flags & VIEWVERTEX ) {
                	    lselold = list_copy ( mes->lselver );

                	    pick_Item ( pt, sce, cam, flags );

                	    lselnew = list_copy ( mes->lselver );

                	    /*** remember selection ***/
                	    g3durm_mesh_pickVertices  ( urm, mes,
                                                	     lselold,
                                                	     lselnew,
                                                	     flags,
                                                	     REDRAWVIEW );
        		}

        		/*********************************/
        		/*if ( flags & VIEWEDGE ) {
                	    G3DMESH *mes = ( G3DMESH * ) obj;
                	    lselold = list_copy ( mes->lseledg );

                	    if ( ( bev->state & G3DControlMask ) == 0x00 ) {
                        	g3dmesh_unselectAllEdges ( mes );
                	    }

                	    pick_Item ( pt, sce, cam, pt->coord, unselectIfSelected, flags );

                	    lselnew = list_copy ( mes->lseledg );
        		}*/

        		/*********************************/
        		if ( ( flags & VIEWFACE ) ) {
                	    G3DMESH *mes = ( G3DMESH * ) obj;
                	    lselold = list_copy ( mes->lselfac );

                	    pick_Item ( pt, sce, cam, flags );

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
        	}

        	/*** Push our selected items into the undo/redo stack ***/
        	/*pick_push ( urm, obj, lselold, lselnew, gdt->flags );*/

        	pt->start = 0x00;
            } return REDRAWALL;

            default :
            break;
	}
    }

    return 0x00;
}
