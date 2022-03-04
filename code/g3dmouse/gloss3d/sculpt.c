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

                        pick_Item ( mts, sce, cam, ctrlClick, engine_flags );
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
