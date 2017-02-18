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
#include <g3dui.h>

/******************************************************************************/
void common_g3duisubdivideredit_subdivSyncCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DSUBDIVIDERTYPE ) ) {
        G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) obj;

        if ( obj->flags & SYNCLEVELS ) {
            g3dsubdivider_unsetSyncSubdivision ( sdr );
        } else {
            g3dsubdivider_setSyncSubdivision   ( sdr );
        }
    }
}

/******************************************************************************/
void common_g3duisubdivideredit_useIsoLinesCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DSUBDIVIDERTYPE ) ) {
        G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) obj;

        g3dui_setHourGlass ( gui );

        if ( obj->flags & MESHUSEISOLINES ) {
            obj->flags &= (~MESHUSEISOLINES);
        } else {
            obj->flags |= MESHUSEISOLINES;
        }

        g3dui_unsetHourGlass ( gui );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3duisubdivideredit_subdivRenderCbk ( G3DUI *gui, int level ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DSUBDIVIDERTYPE ) ) {
        G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) obj;

        g3dui_setHourGlass ( gui );

        sdr->subdiv_render = level;

        if ( obj->flags & SYNCLEVELS ) {
            sdr->subdiv_preview = sdr->subdiv_render;
        }

        g3dui_unsetHourGlass ( gui );
    }

    g3dui_redrawGLViews ( gui );
}


/******************************************************************************/
void common_g3duisubdivideredit_subdivPreviewCbk ( G3DUI *gui, int level ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DSUBDIVIDERTYPE ) ) {
        G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) obj;

        g3dui_setHourGlass ( gui );

        sdr->subdiv_preview = level;

        if ( obj->flags & SYNCLEVELS ) {
            sdr->subdiv_render = sdr->subdiv_preview;
        }

        g3dmodifier_modify_r ( sdr, gui->flags );

        g3dui_unsetHourGlass ( gui );
        /*g3dmesh_setSubdivisionLevel ( mes, level, gui->flags );*/
    }

    g3dui_redrawGLViews ( gui );
}
