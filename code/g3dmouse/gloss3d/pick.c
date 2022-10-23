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

static void pick_draw ( G3DMOUSETOOL *mou, 
                        G3DSCENE     *sce, 
                        uint64_t      engine_flags );

/******************************************************************************/
G3DMOUSETOOLPICK *g3dmousetoolpick_new ( ) {
    uint32_t structsize = sizeof ( G3DMOUSETOOLPICK );

    G3DMOUSETOOLPICK *pt =  ( G3DMOUSETOOLPICK * ) calloc ( 0x01, structsize );

    if ( pt == NULL ) {
        fprintf ( stderr, "pickTool_new: Memory allocation failed\n" );
    }

    g3dmousetool_init ( ( G3DMOUSETOOL * ) pt,
                                           PICKTOOL,
                                           's',
                                           NULL,
                                           NULL,
                                           pick_draw,
                                           pick_tool,
                                           0x00 );

    pt->only_visible  = 0x01;

    pt->operation     = 0x01; /*** add vertex weight ***/
    pt->weight        = 1.0f;
    pt->radius        = PICKMINRADIUS;

    return pt;
}

/******************************************************************************/
G3DMOUSETOOLPICKUV *g3dmousetoolpickUV_new ( ) {
    uint32_t structsize = sizeof ( G3DMOUSETOOLPICKUV );
    void *memarea = calloc ( 0x01, structsize );
    G3DMOUSETOOLPICKUV *pt =  ( G3DMOUSETOOLPICKUV * ) memarea;

    if ( pt == NULL ) {
        fprintf ( stderr, "%s: Memory allocation failed\n", __func__ );
    }

    g3dmousetool_init ( ( G3DMOUSETOOL * ) pt,
                                           PICKUVTOOL,
                                           's',
                                           NULL,
                                           NULL,
                                           pick_draw,
                                           pickUV_tool,
                                           0x00 );

    pt->only_visible  = 0x01;

    pt->operation     = 0x01; /*** add vertex weight ***/
    pt->weight        = 1.0f;
    pt->radius        = PICKMINRADIUS;

    return pt;
}

/******************************************************************************/
static void pick_draw ( G3DMOUSETOOL *mou, 
                        G3DSCENE     *sce, 
                        uint64_t      engine_flags ) {
    G3DMOUSETOOLPICK *pt = ( G3DMOUSETOOLPICK * ) mou;
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
void closeSelectionRectangle ( G3DMOUSETOOLPICK *pt, 
                               int              *VPX, 
                               uint64_t          engine_flags ) {
    int x1 = VPX[0x00];
    int y1 = VPX[0x01];
    int x2 = VPX[0x00] + VPX[0x02];
    int y2 = VPX[0x01] + VPX[0x03];

    /*** in face mode, the square is only 1 pixel large. ***/
    if ( ( engine_flags &  VIEWFACE   ) ||
         ( engine_flags &  VIEWFACEUV ) ) {
        if ( pt->coord[0x00] == pt->coord[0x02] ) {
            pt->coord[0x00] -= 0x01;
            pt->coord[0x02] += 0x01;
        }

        if ( pt->coord[0x01] == pt->coord[0x03] ) {
            pt->coord[0x01] -= 0x01;
            pt->coord[0x03] += 0x01;
        }
    } else {
        /*** default selection is a radius X radius square. 
        /*** This is also the one used ***/
        /*** for axis selection ***/
        if ( pt->coord[0x00] == pt->coord[0x02] ) {
            pt->coord[0x00] -= pt->radius;
            pt->coord[0x02] += pt->radius;
        }

        if ( pt->coord[0x01] == pt->coord[0x03] ) {
            pt->coord[0x01] -= pt->radius;
            pt->coord[0x03] += pt->radius;
        }
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
    uint32_t  flags;
} SCENEPICKDATA;

/******************************************************************************/
static uint32_t actionSelectObject ( uint64_t name, SCENEPICKDATA *cpd ) {
    G3DOBJECT *obj = ( G3DOBJECT * ) name;

    if ( ( obj->flags & OBJECTSELECTED ) == 0x00 ) {
        g3dscene_selectObject ( cpd->sce, obj, 0x00 );
    } else {
        if ( cpd->flags & CTRLCLICK ) {
            g3dscene_unselectObject ( cpd->sce, obj, 0x00 );
        }
    }

    return 0x01;
}

/******************************************************************************/
typedef struct _MESHPICKDATA {
    G3DMESH  *mes;
    uint32_t  flags;
    float     weight; /* for vertex painting (skin) */
} MESHPICKDATA;

/******************************************************************************/
static uint32_t actionSelectFace ( uint64_t name, MESHPICKDATA *mpd ) {
    G3DFACE *fac = ( G3DFACE * ) name;

    if ( ( fac->flags & FACESELECTED ) == 0x00 ) {
        g3dmesh_selectFace ( mpd->mes, fac );
    } else {
        if ( mpd->flags & CTRLCLICK ) {
            g3dmesh_unselectFace ( mpd->mes, fac );
        }
    }

    return 0x01;
}

/******************************************************************************/
static uint32_t actionSelectEdge ( uint64_t name, MESHPICKDATA *mpd ) {
    G3DEDGE *edg = ( G3DEDGE * ) name;

    if ( ( edg->flags & EDGESELECTED ) == 0x00 ) {
        g3dmesh_selectEdge ( mpd->mes, edg );
    } else {
        if ( mpd->flags & CTRLCLICK ) {
            g3dmesh_unselectEdge ( mpd->mes, edg );
        }
    }

    return 0x01;
}

/******************************************************************************/
static uint32_t actionSelectVertex ( uint64_t name, MESHPICKDATA *mpd ) {
    G3DVERTEX *ver = ( G3DVERTEX * ) name;

    if ( ( ver->flags & VERTEXSELECTED ) == 0x00 ) {
        g3dmesh_selectVertex ( mpd->mes, ver );
    } else {
        if ( mpd->flags & CTRLCLICK ) {
            g3dmesh_unselectVertex ( mpd->mes, ver );
        }
    }

    return 0x01;
}

/******************************************************************************/
static uint32_t actionSelectVertexUV ( uint64_t name, MESHPICKDATA *mpd ) {
    G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mpd->mes );

    /*** try the first texture in case no texture is selected ***/
    if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mpd->mes );

    if ( tex ) {
        G3DUVMAP *uvmap = tex->map;

        if ( uvmap ) {
            G3DUV *uv = ( G3DUV * ) name;

            if ( ( uv->flags & UVSELECTED ) == 0x00 ) {
                g3duvmap_selectUV ( uvmap, uv );
            } else {
                if ( mpd->flags & CTRLCLICK ) {
                    g3duvmap_unselectUV ( uvmap, uv );
                }
            }
        }
    }

    return 0x01;
}

/******************************************************************************/
static uint32_t actionSelectFaceUV ( uint64_t name, MESHPICKDATA *mpd ) {
    G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mpd->mes );

    /*** try the first texture in case no texture is selected ***/
    if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mpd->mes );

    if ( tex ) {
        G3DUVMAP *uvmap = tex->map;

        if ( uvmap ) {
            G3DUVSET *uvset = ( G3DUVSET * ) name;

            if ( ( uvset->flags & UVSETSELECTED ) == 0x00 ) {
                g3duvmap_selectUVSet ( uvmap, uvset );
            } else {
                if ( mpd->flags & CTRLCLICK ) {
                    g3duvmap_unselectUVSet ( uvmap, uvset );
                }
            }
        }
    }

    return 0x01;
}

/******************************************************************************/
static uint32_t actionPaintVertex ( uint64_t name, MESHPICKDATA *mpd ) {

/* ( G3DMESH *mes, 
                             G3DCAMERA *curcam,
                             float weight,
                             uint32_t visible, uint64_t engine_flags ) {*/
    G3DMESH *mes = mpd->mes;
    G3DVERTEX *ver = ( G3DVERTEX * ) name;
    G3DWEIGHTGROUP *curgrp = mes->curgrp;
    float weight = mpd->weight;

    /*** a Weightgroup must be selected first ***/
    if ( curgrp == NULL ) return 0;

    G3DWEIGHT *wei = g3dvertex_getWeight ( ver, curgrp );

    /*** Remove vertex from weightgroup ***/
    if ( weight == 0.0f ) {
        if ( wei ) {
            g3dvertex_removeWeight ( ver, wei );
        }
    /*** Add vertex to weighgroup ***/
    } else {
        if ( wei ) {
            /* Note: wei->ver->weight is just a storing */
            /* variable for faster display of the weight */
            ver->weight = wei->weight = weight;
        } else {
            G3DWEIGHT *wei = g3dweight_new ( weight, curgrp );

            g3dvertex_addWeight ( ver, wei );
        }

        ver->flags |= VERTEXPAINTED;
    }

    /*g3dmesh_update ( mes, lselver,
                          NULL,
                          NULL,
                          UPDATEMODIFIERS,
                          engine_flags );*/

    return 0x01;
}

/******************************************************************************/
typedef struct _CURVEPICKDATA {
    G3DCURVE  *curve;
    uint32_t   flags;
} CURVEPICKDATA;

/******************************************************************************/
static uint32_t actionSelectPoint ( uint64_t name, CURVEPICKDATA *cpd ) {
    G3DCURVEPOINT *pt = ( G3DCURVEPOINT * ) name;

    if ( pt->flags & CURVEPOINTISPOINT ) {
	    if ( ( pt->flags & CURVEPOINTSELECTED ) == 0x00 ) {
            g3dcurve_selectPoint ( cpd->curve, pt );
	    } else {
            if ( cpd->flags & CTRLCLICK ) {
                g3dcurve_unselectPoint ( cpd->curve, pt );
            }
        }
    }

    if ( pt->flags & CURVEPOINTISHANDLE ) {
        cpd->curve->curhan = pt;
    }

    return 0x01;
}

/******************************************************************************/
typedef struct _MORPHERPICKDATA {
    G3DMORPHER *mpr;
    uint32_t   flags;
} MORPHERPICKDATA;

/******************************************************************************/
static uint32_t actionSelectVertexForPose ( uint64_t         name, 
                                            MORPHERPICKDATA *mpd ) {
    if ( ((G3DOBJECT*)mpd->mpr)->parent->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) ((G3DOBJECT*)mpd->mpr)->parent;
        G3DVERTEX *ver = ( G3DVERTEX * ) name;

        if ( ( ver->flags & VERTEXSELECTED ) == 0x00 ) {
            g3dmesh_selectVertex ( mes, ver );
        } else {
            if ( mpd->flags & CTRLCLICK ) {
                g3dmesh_unselectVertex ( mes, ver );
            }
        }

        if ( g3dmorpher_getVertexPose ( mpd->mpr, ver, NULL, NULL ) == NULL ) {
            g3dmorpher_addVertexPose ( mpd->mpr, ver, NULL, &ver->pos );
        }
    }

    return 0x01;
}


/******************************************************************************/
/*** We basically draw the scene in 2D with pointer values as pixels ***/
void pick_Item ( G3DMOUSETOOLPICK *pt, 
                 G3DSCENE         *sce, 
                 G3DCAMERA        *cam,
                 uint32_t          ctrlClick,
                 uint64_t          engine_flags ) {
    /*static GLint  VPX[0x04];
    static double MVX[0x10];
    static double PJX[0x10];*/

    /*glGetIntegerv ( GL_VIEWPORT, VPX );*/

    closeSelectionRectangle ( pt, cam->vmatrix /*VPX*/, engine_flags );

    /*glMatrixMode ( GL_PROJECTION );
    glPushMatrix ( );
    glLoadIdentity ( );
    g3dcamera_project ( cam, engine_flags );
    glGetDoublev ( GL_PROJECTION_MATRIX, PJX );*/

    glMatrixMode ( GL_MODELVIEW );
    glPushMatrix ( );
    glLoadIdentity ( );
    g3dcamera_view ( cam, 0x00 );

    /*** Note: calling setViewportMatrix before setProjectionMatrix ***/
    /*** is mandatory ***/

    g3dpick_setViewportMatrix   ( /*VPX*/ cam->vmatrix  );
    g3dpick_setProjectionMatrix ( /*PJX*/ cam->pmatrix  );
    g3dpick_setAreaMatrix       ( pt->coord, pt->circular );

    if ( ( ( engine_flags & VIEWVERTEXUV ) == 0x00 ) &&
         ( ( engine_flags & VIEWFACEUV   ) == 0x00 ) ) {
        if ( pt->only_visible ) {
            g3dpick_enableDepthTest  ( );


/*** commented out: we now use glreadpixel to get the depth value ****/
/*
            g3dpick_setAction ( NULL, NULL );

            g3dobject_pick_r ( ( G3DOBJECT * ) sce, 
                                               cam, 
                                               VIEWOBJECT );
*/

            g3dpick_setEpsilon ( 0.00001f );
        }
    }

    /*** clear must be called once the VPX is set and depth test is toggled ***/
    g3dpick_clear ( );

    if ( engine_flags & VIEWOBJECT ) {
        SCENEPICKDATA cpd = { .sce   =  sce,
                              .flags = 0x00 };

        if ( ctrlClick ) {
            cpd.flags |= CTRLCLICK;
        } else {
            g3dscene_unselectAllObjects ( sce, engine_flags );
        }

        g3dpick_setAction ( G3DPICK_ACTIONFUNC( actionSelectObject ), &cpd );

        g3dobject_pick_r ( ( G3DOBJECT * ) sce, 
                                           cam, 
                                           VIEWOBJECT );
    } else {
        G3DOBJECT *obj = g3dscene_getLastSelected ( sce );

	    if ( obj ) {
            if ( pt->only_visible ) g3dpick_enableDepthTest  ( );
            else                    g3dpick_disableDepthTest ( );

		    if ( engine_flags & VIEWPATH ) {
        	    CURVEPICKDATA cpd = { .curve =  obj->curve[0x00],
                        	          .flags = 0x00 };

        	    g3dpick_setAction ( G3DPICK_ACTIONFUNC ( actionSelectPoint ), &cpd );

        	    g3dobject_pick_r ( ( G3DOBJECT * ) sce, 
                                                   cam, 
                                                   VIEWPATH );
		    }

	        if ( ( obj->type == G3DMESHTYPE ) ||
                 ( obj->type == G3DFFDTYPE  ) ) {
        	    G3DMESH *mes = ( G3DMESH * ) obj;
        	    MESHPICKDATA mpd = { .mes    = mes,
                        	         .weight = pt->weight,
                                     .flags = 0x00 };

                if ( ctrlClick ) {
                    mpd.flags |= CTRLCLICK;
                } else {
                    if ( engine_flags & VIEWFACE     ) g3dmesh_unselectAllFaces    ( mes );
                    if ( engine_flags & VIEWEDGE     ) g3dmesh_unselectAllEdges    ( mes );
                    if ( engine_flags & VIEWVERTEX   ) g3dmesh_unselectAllVertices ( mes );
                    if ( engine_flags & VIEWVERTEXUV ) {
                        G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

                        /*** try the first texture in case no texture is selected ***/
                        if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

                        if ( tex ) {
                            G3DUVMAP *uvmap = tex->map;

                            if ( uvmap ) g3duvmap_unselectAllUVs ( uvmap );
                        }
                    }

                    if ( engine_flags & VIEWFACEUV ) {
                        G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

                        /*** try the first texture in case no texture is selected ***/
                        if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

                        if ( tex ) {
                            G3DUVMAP *uvmap = tex->map;

                            if ( uvmap ) g3duvmap_unselectAllUVSets ( uvmap );
                        }
                    }
                }

		        if ( engine_flags & VIEWVERTEXUV ) {
        	        g3dpick_setAction ( G3DPICK_ACTIONFUNC ( actionSelectVertexUV ), &mpd );

                    /*** directly call g3dmesh_pickUVs() to bypass matrix ***/
                    /*** opration. Our drawing call must depend on an ***/
                    /*** identity matrix in the UVMap Editor ***/
                    /*glGetDoublev ( GL_MODELVIEW_MATRIX, MVX );*/
                    g3dpick_setModelviewMatrix ( ((G3DOBJECT*)cam)->iwmatrix /*MVX*/ );
                    g3dmesh_pickVertexUVs ( mes, VIEWVERTEXUV );
                }

		        if ( engine_flags & VIEWFACEUV ) {
        	        g3dpick_setAction ( G3DPICK_ACTIONFUNC ( actionSelectFaceUV ), &mpd );

                    /*** directly call g3dmesh_pickUVs() to bypass matrix ***/
                    /*** opration. Our drawing call must depend on an ***/
                    /*** identity matrix in the UVMap Editor ***/
                    /*glGetDoublev ( GL_MODELVIEW_MATRIX, MVX );*/
                    g3dpick_setModelviewMatrix ( ((G3DOBJECT*)cam)->iwmatrix /*MVX*/ );
                    g3dmesh_pickFaceUVs ( mes, VIEWFACEUV );
                }

		        if ( engine_flags & VIEWFACE ) {
        	        g3dpick_setAction ( G3DPICK_ACTIONFUNC ( actionSelectFace ), &mpd );
        	        g3dobject_pick_r ( ( G3DOBJECT * ) sce, 
                                                       cam, 
                                                       VIEWFACE );
		        }

		        if ( engine_flags & VIEWEDGE ) {
        	        g3dpick_setAction ( G3DPICK_ACTIONFUNC ( actionSelectEdge ), &mpd );
        	        g3dobject_pick_r ( ( G3DOBJECT * ) sce, 
                                                       cam, 
                                                       VIEWEDGE );
		        }

		        if ( engine_flags & VIEWVERTEX ) {
        	        g3dpick_setAction ( G3DPICK_ACTIONFUNC ( actionSelectVertex ), &mpd );
        	        g3dobject_pick_r ( ( G3DOBJECT * ) sce, 
                                                       cam, 
                                                       VIEWVERTEX );
		        }

		        if ( engine_flags & VIEWSKIN ) {
        	        g3dpick_setAction ( G3DPICK_ACTIONFUNC ( actionPaintVertex ), &mpd );
        	        g3dobject_pick_r ( ( G3DOBJECT * ) sce, 
                                                       cam, 
                                                       VIEWVERTEX );
		        }
	        }

	        if ( obj->type == G3DSPLINETYPE ) {
        	    G3DSPLINE *spl = ( G3DSPLINE * ) obj;
        	    CURVEPICKDATA cpd = { .curve =  spl->curve,
                        	          .flags = 0x00 };

                if ( ctrlClick ) {
                    cpd.flags |= CTRLCLICK;
                } else {
                    g3dcurve_unselectAllPoints ( spl->curve );
                }

		        if ( engine_flags & VIEWVERTEX ) {
        	        g3dpick_setAction ( G3DPICK_ACTIONFUNC ( actionSelectPoint ), &cpd );

        	        g3dobject_pick_r ( ( G3DOBJECT * ) sce, 
                                                       cam, 
                                                       VIEWVERTEX );
		        }
	        }

	        if ( obj->type == G3DSUBDIVIDERTYPE ) {
        	    G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) obj;
        	    MESHPICKDATA mpd = { .mes    = g3dobject_getActiveParentByType ( obj, MESH ),
                        	         .weight = pt->weight,
                                     .flags = 0x00 };

                if ( mpd.mes ) {
                    if ( ctrlClick ) {
                        mpd.flags |= CTRLCLICK;
                    } else {
                        if ( engine_flags & VIEWSCULPT ) g3dmesh_unselectAllFaces ( mpd.mes );
                    }

		            if ( engine_flags & VIEWSCULPT ) {
        	            g3dpick_setAction ( G3DPICK_ACTIONFUNC ( actionSelectFace ), &mpd );

        	            g3dobject_pick_r ( ( G3DOBJECT * ) sce, 
                                                           cam, 
                                                           VIEWFACE );

                        /*g3dsubdivider_fillBuffers ( sdr,
                                                    sdr->lsubfac,
                                                    engine_flags );

                        list_free ( &sdr->lsubfac, NULL );*/
		            }
                }
	        }

	        if ( obj->type == G3DMORPHERTYPE ) {
        	    G3DMORPHER *mpr = ( G3DMORPHER * ) obj;

                if ( ((G3DOBJECT*)mpr)->parent ) {
                    if ( ((G3DOBJECT*)mpr)->parent->type == G3DMESHTYPE ) {
                        G3DMESH *mes = ( G3DMESH * ) ((G3DOBJECT*)mpr)->parent;
        	            MORPHERPICKDATA mpd = { .mpr   =  mpr,
                        	                    .flags = 0x00 };

                        if ( ctrlClick ) {
                            mpd.flags |= CTRLCLICK;
                        } else {
                            g3dmesh_unselectAllVertices ( mes );
                        }

		                if ( engine_flags & VIEWVERTEX ) {
        	                g3dpick_setAction ( G3DPICK_ACTIONFUNC ( actionSelectVertexForPose ), &mpd );

        	                g3dobject_pick_r ( ( G3DOBJECT * ) sce, 
                                                               cam, 
                                                               VIEWVERTEX );
		                }
                    }
                }
	        }
	    }
    }

    glPopMatrix ( );

    /*glMatrixMode ( GL_PROJECTION );
    glPopMatrix ( );*/
}

/******************************************************************************/
static uint32_t actionSelectAxis ( uint64_t name, G3DCURSOR *csr ) {
    if ( name == CURSORXAXIS ) csr->axis[0x00].w = 2.0f;
    if ( name == CURSORYAXIS ) csr->axis[0x01].w = 2.0f;
    if ( name == CURSORZAXIS ) csr->axis[0x02].w = 2.0f;

    return 0x01;
}

/******************************************************************************/
/*** We basically draw the scene in 2D with pointer values as pixels ***/
void pick_cursor ( G3DMOUSETOOLPICK *pt, 
                   G3DSCENE         *sce, 
                   G3DCAMERA        *cam,
                   uint64_t          engine_flags ) {
    static GLint  VPX[0x04];
    static double MVX[0x10];
    static double PJX[0x10];
    G3DVECTOR sca;

    /*g3dscene_getSelectionMatrix ( sce, MVX, engine_flags );*/

    /*** Extract scale factor to negate its effect on the cursor size ***/
    /*** by scaling the cursor matrix with the inverse scale factors ***/
    g3dcore_getMatrixScale ( sce->csr.matrix, &sca );

    glGetIntegerv ( GL_VIEWPORT, VPX );

    /*** check boundaries ***/
    /*** engine_flags is 0x00, that way the size ***/
    /*** of the picking region is radius x radius */
    closeSelectionRectangle ( pt, VPX, 0x00 );

    glMatrixMode ( GL_PROJECTION );
    glPushMatrix ( );
    glLoadIdentity ( );
    g3dcamera_project ( cam, engine_flags );
    glGetDoublev ( GL_PROJECTION_MATRIX, PJX );

    glMatrixMode ( GL_MODELVIEW );
    glPushMatrix ( );
    glLoadIdentity ( );
    g3dcamera_view ( cam, 0x00 );

    glMultMatrixd ( sce->csr.matrix );

    glTranslatef ( sce->csr.pivot.x, 
                   sce->csr.pivot.y, 
                   sce->csr.pivot.z );

    /*** scale cursor to object's scale ***/
    glScalef ( 1.0f / sca.x,
               1.0f / sca.y,
               1.0f / sca.z );

    glGetDoublev ( GL_MODELVIEW_MATRIX, MVX );

    g3dpick_setViewportMatrix   ( VPX   );
    g3dpick_setProjectionMatrix ( PJX   );
    g3dpick_setAreaMatrix       ( pt->coord, 0 );

    g3dpick_disableDepthTest  ( );

    /*** clear must be called once the VPX is set ***/
    g3dpick_clear ( );

    /*** this must be called after g3dpick_clear ***/
    g3dpick_setModelviewMatrix  ( MVX   );

    g3dpick_setAction ( G3DPICK_ACTIONFUNC ( actionSelectAxis ), &sce->csr );

    g3dcursor_pick ( &sce->csr, cam, engine_flags );

    if ( ( sce->csr.axis[0].w == 0.0f ) &&
         ( sce->csr.axis[1].w == 0.0f ) &&
         ( sce->csr.axis[2].w == 0.0f ) ) {
        sce->csr.axis[0].w = sce->csr.axis[1].w = sce->csr.axis[2].w = 1.0f;
    }

    glPopMatrix ( );

    glMatrixMode ( GL_PROJECTION );
    glPopMatrix ( );
}

/******************************************************************************/
static void pick_free ( void *data ) {

}

/******************************************************************************/
static int weight_tool ( G3DMOUSETOOL *mou, 
                         G3DSCENE     *sce, 
                         G3DCAMERA    *cam,
                         G3DURMANAGER *urm, 
                         uint64_t      engine_flags, 
                         G3DEvent     *event ) {
    G3DMOUSETOOLPICK *pt = ( G3DMOUSETOOLPICK * ) mou;
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
        	        if ( mes->lweigrp == NULL ) {
                	    G3DWEIGHTGROUP *grp;
                	    char buf[0x20];

                	    snprintf ( buf, 0x20, "WeightGroup%02i", mes->nbweigrp );

                	    grp = g3dweightgroup_new ( mes, buf );

                	    g3dmesh_addWeightGroup ( mes, grp );

                	    mes->curgrp = grp;
        	        }
        	    }
            }

            glGetIntegerv ( GL_VIEWPORT, VPX );
        } return REDRAWVIEW | UPDATECURRENTOBJECT;

        case G3DMotionNotify : {
            G3DMotionEvent *mev = ( G3DMotionEvent * ) event;
            uint32_t ctrlClick = ( mev->state & G3DControlMask ) ? 1 : 0;

            if ( obj ) {
        	    if ( obj->type == G3DMESHTYPE ) {
        	        pt->coord[0x00] = pt->coord[0x02] = mev->x;
        	        pt->coord[0x01] = pt->coord[0x03] = VPX[0x03] - mev->y;

        	        closeSelectionRectangle ( pt, VPX, engine_flags );

                    pick_Item ( pt, sce, cam, ctrlClick, engine_flags );
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
int pickUV_tool ( G3DMOUSETOOL *mou, 
                  G3DSCENE     *sce, 
                  G3DCAMERA    *cam,
                  G3DURMANAGER *urm, 
                  uint64_t      engine_flags, 
                  G3DEvent     *event ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
    G3DMOUSETOOLPICK *pt = ( G3DMOUSETOOLPICK * ) mou;
    static GLint VPX[0x04];

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

        	                glGetIntegerv ( GL_VIEWPORT, VPX );

        	                pt->start = 0x01;

        	                startSelectionRectangle ( bev->x, VPX[0x03] - bev->y, pt->coord );
                        } return UPDATEANDREDRAWALL;

                        case G3DMotionNotify : {
        	            /*** if G3DButtonPress was called ***/
        	                if ( pt->start ) {
                                    G3DMotionEvent *mev = ( G3DMotionEvent * ) event;
                                    glGetIntegerv ( GL_VIEWPORT, VPX );

                                    shapeSelectionRectangle ( mev->x, VPX[0x03] - mev->y, pt->coord );

                                    return REDRAWUVMAPEDITOR;
        	                }
                        } return 0x00;

                        case G3DButtonRelease : {
        	                G3DButtonEvent *bev = ( G3DButtonEvent * ) event;
                            uint32_t ctrlClick = ( bev->state & G3DControlMask ) ? 1 : 0;
                            LIST *lselold = NULL, *lselnew = NULL;

        		            if ( engine_flags & VIEWVERTEXUV ) {
                                lselold = list_copy ( uvmap->lseluv );

                	            pick_Item ( pt, sce, cam, ctrlClick, engine_flags );

                                lselnew = list_copy ( uvmap->lseluv );

                	            /*** remember selection ***/
                	            g3durm_uvmap_pickUVs  ( urm, sce, uvmap,
                                                	         lselold,
                                                	         lselnew,
                                                	         VIEWVERTEXUV,
                                                             REDRAWVIEW |
                                                             REDRAWUVMAPEDITOR );
                            }

        		            if ( engine_flags & VIEWFACEUV ) {
                                lselold = list_copy ( uvmap->lseluvset );

                	            pick_Item ( pt, sce, cam, ctrlClick, engine_flags );

                                lselnew = list_copy ( uvmap->lseluvset );

                	            /*** remember selection ***/
                	            g3durm_uvmap_pickUVSets ( urm, sce, uvmap,
                                                	         lselold,
                                                	         lselnew,
                                                	         VIEWFACEUV,
                                                             REDRAWVIEW |
                                                             REDRAWUVMAPEDITOR );
                            }

        	                pt->start = 0x00;
                        } return UPDATEANDREDRAWALL;

                        default :
                        break;
	                }
                }
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
int pick_tool ( G3DMOUSETOOL *mou, 
                G3DSCENE     *sce, 
                G3DCAMERA    *cam,
                G3DURMANAGER *urm, 
                uint64_t      engine_flags, 
                G3DEvent     *event ) {
    /*G3DURMANAGER *urm = gdt->urm;*/
    /*** selection rectangle coords ***/
    static GLint VPX[0x04];
    G3DMOUSETOOLPICK *pt = ( G3DMOUSETOOLPICK * ) mou;
    uint32_t unselectIfSelected = 0x01;

    if ( engine_flags & VIEWSKIN ) {
        return weight_tool ( mou, sce, cam, urm, engine_flags, event );
    } else {
	switch ( event->type ) {
            case G3DButtonPress : {
        	    G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

        	    glGetIntegerv ( GL_VIEWPORT, VPX );

        	    pt->start = 0x01;

        	    startSelectionRectangle ( bev->x, VPX[0x03] - bev->y, pt->coord );
            } return UPDATEANDREDRAWALL;

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
                uint32_t ctrlClick = ( bev->state & G3DControlMask ) ? 1 : 0;

        	    /*********************************/
        	    if ( engine_flags & VIEWOBJECT ) {
                        /*** copy list for undo/redo manager ***/
                        lselold = list_copy ( sce->lsel );

                        obj = ( G3DOBJECT * ) sce;

                        pick_Item ( pt, sce, cam, ctrlClick, engine_flags );

                        /*** copy list for undo/redo manager ***/
                        lselnew = list_copy ( sce->lsel );

                        if ( lselold || lselnew ) {
                	        /*** remember selection ***/
                            g3durm_scene_pickObject  ( urm, sce,
                                                	        lselold,
                                                	        lselnew,
                                                	        engine_flags,
                                                	        REDRAWVIEW );
                        }
        	    }

        	    if ( obj ) {
        		    if ( engine_flags & VIEWPATH ) {
                	    pick_Item ( pt, sce, cam, ctrlClick, engine_flags );
                	}

                    if ( obj->type == G3DMORPHERTYPE ) {
                	    G3DMORPHER *mpr = ( G3DMORPHER * ) obj;

                        if ( obj->parent->type == G3DMESHTYPE ) {
                            G3DMESH *mes = ( G3DMESH * ) obj->parent;

        		            if ( engine_flags & VIEWVERTEX ) {
                                LIST *lmprver = list_copy ( mpr->lver );

                	            lselold = list_copy ( mes->lselver );

                	            pick_Item ( pt, sce, cam, ctrlClick, engine_flags );

                	            lselnew = list_copy ( mes->lselver );

                	            /*** remember selection ***/
                                if ( lselold || lselnew ) {
                                    g3durm_morpher_selectVertexPose ( urm,
                                                                      sce,
                                                                      mpr,
                                                                      mpr->selmpose,
                                                                      lmprver,
                                                                      lselold,
                                                                      lselnew,
                                                                      REDRAWVIEW );
                                }
                	        }
                        }
                    }

                    if ( obj->type & SPLINE ) {
                	    G3DSPLINE *spl = ( G3DSPLINE * ) obj;

        		        if ( engine_flags & VIEWVERTEX ) {
                	        lselold = list_copy ( spl->curve->lselpt );

                	        pick_Item ( pt, sce, cam, ctrlClick, engine_flags );

                	        lselnew = list_copy ( spl->curve->lselpt );

                            if ( lselold || lselnew ) {
                                g3durm_spline_pickPoints ( urm,
                                                           sce,
                                                           spl,
                                                           lselold,
                                                           lselnew,
                                                           engine_flags,
                                                           REDRAWVIEW );
                            }
                	    }
                    }

                    if ( obj->type == G3DSUBDIVIDERTYPE ) {
                	    G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) obj;
                        G3DMESH *mes = g3dobject_getActiveParentByType ( obj, MESH );

                        if ( mes ) {
        		            if ( engine_flags & VIEWSCULPT ) {
                	            lselold = list_copy ( mes->lselfac );

                	            pick_Item ( pt, 
                                            sce, 
                                            cam, 
                                            ctrlClick, 
                                            engine_flags );

                	            lselnew = list_copy ( mes->lselfac );

                	            /*** remember selection ***/
                                if ( lselold || lselnew ) {
                	                g3durm_mesh_pickFaces  ( urm, 
                                                             sce, 
                                                             mes,
                                                             lselold,
                                                             lselnew,
                                                             engine_flags,
                                                             REDRAWVIEW );

                                    mes->obj.update_flags |= RESETMODIFIERS;

                	                g3dmesh_update ( mes, engine_flags );
                                }
                            }
                        }
                    }

                    if ( ( obj->type == G3DMESHTYPE ) || 
                         ( obj->type == G3DFFDTYPE  ) ) {
                	    G3DMESH *mes = ( G3DMESH * ) obj;

        		        if ( engine_flags & VIEWVERTEX ) {
                	        lselold = list_copy ( mes->lselver );

                	        pick_Item ( pt, sce, cam, ctrlClick, engine_flags );

                	        lselnew = list_copy ( mes->lselver );

                	        /*** remember selection ***/
                            if ( lselold || lselnew ) {
                	            g3durm_mesh_pickVertices  ( urm, 
                                                            sce, 
                                                            mes,
                                                            lselold,
                                                            lselnew,
                                                            engine_flags,
                                                            REDRAWVIEW );
                            }
        		        }

        		        /*********************************/
        		        if ( engine_flags & VIEWEDGE ) {
                	        G3DMESH *mes = ( G3DMESH * ) obj;
                	        lselold = list_copy ( mes->lseledg );

                	        pick_Item ( pt, sce, cam, ctrlClick, engine_flags );

                	        lselnew = list_copy ( mes->lseledg );

                	        /*** remember selection ***/
                            if ( lselold || lselnew ) {
                	            g3durm_mesh_pickEdges  ( urm, 
                                                         sce, 
                                                         mes,
                                                         lselold,
                                                         lselnew,
                                                         engine_flags,
                                                         REDRAWVIEW );
                            }
        		        }

        		        /*********************************/
        	            if ( ( engine_flags & VIEWFACE ) ) {
                	        G3DMESH *mes = ( G3DMESH * ) obj;
                	        lselold = list_copy ( mes->lselfac );

                	        pick_Item ( pt, sce, cam, ctrlClick, engine_flags );

                	        lselnew = list_copy ( mes->lselfac );

                	        /*** remember selection ***/
                            if ( lselold || lselnew ) {
                	            g3durm_mesh_pickFaces  ( urm, 
                                                         sce, 
                                                         mes,
                                                         lselold,
                                                         lselnew,
                                                         engine_flags,
                                                         REDRAWVIEW );

                                mes->obj.update_flags |= RESETMODIFIERS;

                	            g3dmesh_update ( mes, engine_flags );
                            }
        	            }
                    }
        	    }

                g3dscene_updatePivot ( sce, engine_flags );

        	    /*** Push our selected items into the undo/redo stack ***/
        	    /*pick_push ( urm, obj, lselold, lselnew, gdt->flags );*/

        	    pt->start = 0x00;
            } return UPDATEANDREDRAWALL;

            default :
            break;
	    }
    }

    return 0x00;
}
