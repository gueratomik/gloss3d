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

static int sculpt_tool ( G3DMOUSETOOL *mou, 
                         G3DSCENE     *sce, 
                         G3DCAMERA    *cam,
                         G3DURMANAGER *urm, 
                         uint64_t      engine_flags, 
                         G3DEvent     *event );

/******************************************************************************/
uint32_t directionChange ( uint32_t x, uint32_t y ) {
    static uint32_t oldx, oldy;
    static G2DVECTOR olddir;
    static int32_t oldres;
    G2DVECTOR dir = { x - oldx, y - oldy };
    int32_t res;
    uint32_t change = 0;

    res = g2dvector_scalar ( &dir, &olddir );

    if ( res * oldres < 0 ) change = 0x01;

    oldres = res;

    olddir.x = dir.x;
    olddir.y = dir.y;

    oldx = x;
    oldy = y;

    return change;
}

/******************************************************************************/
G3DMOUSETOOLSCULPT *g3dmousetoolsculpt_new ( SCULPTFUNCENUM efc ) {
    uint32_t structsize = sizeof ( G3DMOUSETOOLSCULPT );

    G3DMOUSETOOLSCULPT *mts =  ( G3DMOUSETOOLSCULPT * ) calloc ( 0x01, structsize );
    char *toolname;

    if ( mts == NULL ) {
        fprintf ( stderr, "sculptTool_new: Memory allocation failed\n" );

        return NULL;
    }

    switch ( efc ) {
        case SCULPTINFLATE :
            toolname = INFLATETOOL;
        break;

        case SCULPTCREASE :
            toolname = CREASETOOL;
        break;

        case SCULPTFLATTEN :
            toolname = FLATTENTOOL;
        break;

        case SCULPTSMOOTH :
            toolname = SMOOTHTOOL;
        break;

        case SCULPTUNSCULPT :
            toolname = UNSCULPTTOOL;
        break;

        default :
            toolname = "NOTOOL";
        break;
    }

    g3dmousetool_init ( ( G3DMOUSETOOL * ) mts,
                                           toolname,
                                           's',
                                           NULL,
                                           NULL,
                                           NULL,
                                           sculpt_tool,
                                           0x00 );

    mts->only_visible = 0x01;
    mts->pressure     = 0.5f;
    mts->radius       = 0x20;
    mts->type         = efc;

    return mts;
}

/******************************************************************************/
static void sculpt_free ( void *data ) {

}

/******************************************************************************/
typedef struct _SUBDIVIDERPICKDATA {
    G3DSUBDIVIDER      *sdr;
    uint32_t            flags;
    uint32_t            round;
    G3DMOUSETOOLSCULPT *mts;
} SUBDIVIDERPICKDATA;

/******************************************************************************/
static uint32_t actionSculptVertex ( uint64_t name, SUBDIVIDERPICKDATA *spd ) {
/* ( G3DMESH *mes, 
                             G3DCAMERA *curcam,
                             float weight,
                             uint32_t visible, uint64_t engine_flags ) {*/
    G3DSUBDIVIDER *sdr = spd->sdr;
    uint64_t facID = ( name >> 0x20 );
    uint64_t rtverID = name & 0xFFFFFFFF;
    G3DFACE *fac = sdr->factab[facID];
    G3DFACESCULPTEXTENSION *fse = g3dface_getExtension ( fac,
                                            ( uint64_t ) sdr );
    URMSCULPTFACEEXTENSION *usfe = NULL;
    G3DMOUSETOOLSCULPT *mts = spd->mts;

    /*** adjust resolution for existing maps ***/
    if ( sdr->subdiv_preview > sdr->sculptResolution ) {
        g3dsubdivider_setSculptResolution ( sdr, sdr->subdiv_preview );
    }

    if ( fse == NULL ) {
        fse = g3dfacesculptextension_new ( ( uint64_t ) sdr,
                                                        fac,
                                                        sdr->sculptResolution,
                                                        sdr->sculptMode );

        g3dface_addExtension ( fac, fse );
    }

    /*printf ("%d %d\n", facID, rtverID );*/

    usfe = urmsculptfaceextension_seek ( mts->lusfe, fse );

    if ( usfe == NULL ) {
        uint32_t i;

        usfe = urmsculptfaceextension_new ( fse, fac );

        for ( i = 0x00; i < fse->nbver; i++ ) {
            switch ( sdr->sculptMode ) {
                case SCULPTMODE_SCULPT :
                    memcpy ( &usfe->pos[i],
                             &fse->pos[i], sizeof ( G3DVECTOR ) );

                    memcpy ( &usfe->nor[i],
                             &fac->rtvermem[i].nor, sizeof ( G3DVECTOR ) );
                break;

                default :
                    memcpy ( &usfe->hei[i],
                             &fse->hei[i], sizeof ( G3DHEIGHT ) );
                break;
            }
        }

        list_insert ( &mts->lusfe, usfe ); /*** for undo / redo ***/
    }

    if ( list_seek ( mts->lfse, fse ) == NULL ) {
        list_insert ( &mts->lfse, fse );
    }

    if ( mts->type == SCULPTINFLATE ) {
        if ( ( fse->flags[rtverID] & 0x01 ) == 0x00 ) {
            switch ( sdr->sculptMode ) {
                case SCULPTMODE_SCULPT :
                    fse->pos[rtverID].x += ( usfe->nor[rtverID].x * 0.01f * mts->pressure );
                    fse->pos[rtverID].y += ( usfe->nor[rtverID].y * 0.01f * mts->pressure );
                    fse->pos[rtverID].z += ( usfe->nor[rtverID].z * 0.01f * mts->pressure );
                    fse->pos[rtverID].w  = 1.0f;
                break;

                default :
                    fse->hei[rtverID].s += mts->pressure * 0.01f;
                    fse->hei[rtverID].w  = 1.0f;
                break;
            }

            fse->flags[rtverID] |= 0x01;
        }
    }

    if ( mts->type == SCULPTCREASE ) {
        if ( ( fse->flags[rtverID] & 0x01 ) == 0x00 ) {
            switch ( sdr->sculptMode ) {
                case SCULPTMODE_SCULPT :
                    fse->pos[rtverID].x -= ( usfe->nor[rtverID].x * 0.01f * mts->pressure );
                    fse->pos[rtverID].y -= ( usfe->nor[rtverID].y * 0.01f * mts->pressure );
                    fse->pos[rtverID].z -= ( usfe->nor[rtverID].z * 0.01f * mts->pressure );
                    fse->pos[rtverID].w  = 1.0f;
                break;

                default :
                    fse->hei[rtverID].s -= mts->pressure * 0.01f;
                    fse->hei[rtverID].w  = 1.0f;
                break;
            }

            fse->flags[rtverID] |= 0x01;
        }
    }

    if ( mts->type == SCULPTUNSCULPT ) {
        if ( ( fse->flags[rtverID] & 0x01 ) == 0x00 ) {
            switch ( sdr->sculptMode ) {
                case SCULPTMODE_SCULPT :
                    fse->pos[rtverID].x = 0.0f;
                    fse->pos[rtverID].y = 0.0f;
                    fse->pos[rtverID].z = 0.0f;
                    fse->pos[rtverID].w = 0.0f;
                break;

                default :
                    fse->hei[rtverID].s = 0.0f;
                    fse->hei[rtverID].w = 0.0f;
                break;
            }

            fse->flags[rtverID] |= 0x01;
        }
    }


    /*** update this face only ***/
    if ( list_seek ( sdr->lsubfac, fac ) == NULL ) {
        list_insert ( &sdr->lsubfac, fac );
    }

    return 0x01;
}

/******************************************************************************/
/*** We basically draw the scene in 2D with pointer values as pixels ***/
void sculpt_Item ( G3DMOUSETOOLSCULPT *pt, 
                   G3DSCENE           *sce, 
                   G3DCAMERA          *cam,
                   uint32_t            ctrlClick,
                   uint64_t            engine_flags ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( sce );
    static GLint  VPX[0x04];
    static double MVX[0x10];
    static double PJX[0x10];

    glGetIntegerv ( GL_VIEWPORT, VPX );

    closeSelectionRectangle ( pt, VPX, engine_flags );

    glMatrixMode ( GL_PROJECTION );
    glPushMatrix ( );
    glLoadIdentity ( );
    g3dcamera_project ( cam, engine_flags );
    glGetDoublev ( GL_PROJECTION_MATRIX, PJX );

    glMatrixMode ( GL_MODELVIEW );
    glPushMatrix ( );
    glLoadIdentity ( );
    g3dcamera_view ( cam, 0x00 );

    /*** Note: calling setViewportMatrix before setProjectionMatrix ***/
    /*** is mandatory ***/
    g3dpick_setViewportMatrix   ( VPX   );
    g3dpick_setProjectionMatrix ( PJX   );
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

	if ( obj ) {
        if ( pt->only_visible ) g3dpick_enableDepthTest  ( );
        else                    g3dpick_disableDepthTest ( );

	    if ( obj->type == G3DSUBDIVIDERTYPE ) {
        	G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) obj;
        	SUBDIVIDERPICKDATA spd = { .sdr   = sdr,
                        	           .flags = 0x00,
                                       .mts   = pt };

		    if ( engine_flags & VIEWSCULPT ) {
        	    g3dpick_setAction ( G3DPICK_ACTIONFUNC ( actionSculptVertex ), &spd );

        	    g3dobject_pick_r ( ( G3DOBJECT * ) sce, 
                                                   cam, 
                                                   VIEWSCULPT );

                g3dsubdivider_fillBuffers ( sdr,
                                            sdr->lsubfac,
                                            engine_flags );

                list_free ( &sdr->lsubfac, NULL );
		    }
	    }
	}

    glPopMatrix ( );

    glMatrixMode ( GL_PROJECTION );
    glPopMatrix ( );
}

/******************************************************************************/
int sculpt_tool ( G3DMOUSETOOL *mou, 
                  G3DSCENE     *sce, 
                  G3DCAMERA    *cam,
                  G3DURMANAGER *urm, 
                  uint64_t      engine_flags, 
                  G3DEvent     *event ) {
    G3DMOUSETOOLSCULPT *mts = ( G3DMOUSETOOLSCULPT * ) mou;
    static G3DOBJECT *obj = NULL;
    static int VPX[0x04];
    static URMSCULPTFACE *usf;

    if ( engine_flags & VIEWSCULPT ) {
        switch ( event->type ) {
	        case G3DButtonPress : {
                G3DButtonEvent *bev = ( G3DButtonEvent * ) event;

                obj = g3dscene_getLastSelected ( sce );

                if ( obj ) {
                    if ( obj->type == G3DSUBDIVIDERTYPE ) {
                        G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) obj;

                        usf = g3durm_mesh_sculptFace ( urm,
                                                       sdr,
                                                       engine_flags,
                                                       REDRAWVIEW );
                    }
                }

                /*** init ***/
                directionChange ( bev->x, bev->y );

                /*** also done in release event ***/
                list_free ( &mts->lfse, NULL );

                glGetIntegerv ( GL_VIEWPORT, VPX );

            } return REDRAWVIEW | REDRAWCURRENTOBJECT;

            case G3DMotionNotify : {
                G3DMotionEvent *mev = ( G3DMotionEvent * ) event;
                uint32_t ctrlClick = ( mev->state & G3DControlMask ) ? 1 : 0;

                if ( obj ) {
                    if ( directionChange ( mev->x, mev->y ) ) {
                        list_exec ( mts->lfse, g3dfacesculptextension_clearFlags );
                    } 

        	        if ( obj->type == G3DSUBDIVIDERTYPE ) {
        	            mts->coord[0x00] = mts->coord[0x02] = mev->x;
        	            mts->coord[0x01] = mts->coord[0x03] = VPX[0x03] - mev->y;

        	            closeSelectionRectangle ( mts, VPX, engine_flags );

                        sculpt_Item ( mts, sce, cam, ctrlClick, engine_flags );
        	        }
                }
            } return REDRAWVIEW;

            case G3DButtonRelease : {
                obj = NULL;

                /*** also done in press event ***/
                list_free ( &mts->lfse, NULL );

                if ( usf ) {
                    usf->lusfe = mts->lusfe;
                }

                usf = NULL;

                mts->lusfe = NULL;

                /*list_free ( &mts->lusf, NULL );*/
            } break;

            default :
            break;
        }
    }

    return 0x00;
}
