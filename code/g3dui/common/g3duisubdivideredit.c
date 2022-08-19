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
#include <g3dui.h>

/******************************************************************************/
uint64_t g3duisubdivideredit_displacementHeightCbk ( G3DUISUBDIVIDEREDIT *subedit ) {
    G3DUI *gui = subedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DSUBDIVIDERTYPE ) {
            G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) sel;

            g3dsubdivider_setSculptMode ( sdr, 
                                          SCULPTMODE_HEIGHT,
                                          gui->engine_flags );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duisubdivideredit_displacementSculptCbk ( G3DUISUBDIVIDEREDIT *subedit ) {
    G3DUI *gui = subedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DSUBDIVIDERTYPE ) {
            G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) sel;

            g3dsubdivider_setSculptMode ( sdr,
                                          SCULPTMODE_SCULPT,
                                          gui->engine_flags );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duisubdivideredit_subdivSyncCbk ( G3DUISUBDIVIDEREDIT *subedit ) {
    G3DUI *gui = subedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DSUBDIVIDERTYPE ) {
            G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) sel;

            if ( sel->flags & SYNCLEVELS ) {
                g3dsubdivider_unsetSyncSubdivision ( sdr );
            } else {
                g3dsubdivider_setSyncSubdivision   ( sdr );
            }
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
uint64_t g3duisubdivideredit_useIsoLinesCbk ( G3DUISUBDIVIDEREDIT *subedit ) {
    G3DUI *gui = subedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DSUBDIVIDERTYPE ) {
            G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) sel;

            if ( sel->flags & MESHUSEISOLINES ) {
                sel->flags &= (~MESHUSEISOLINES);
            } else {
                sel->flags |= MESHUSEISOLINES;
            }
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duisubdivideredit_subdivRenderCbk ( G3DUISUBDIVIDEREDIT *subedit,
                                               uint32_t             level ) {
    G3DUI *gui = subedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DSUBDIVIDERTYPE ) {
            G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) sel;

            sdr->subdiv_render = level;

            if ( sel->flags & SYNCLEVELS ) {
                sdr->subdiv_preview = sdr->subdiv_render;
            }
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWCURRENTOBJECT;
}

/******************************************************************************/
uint64_t g3duisubdivideredit_subdivPreviewCbk ( G3DUISUBDIVIDEREDIT *subedit,
                                                uint32_t             level ) {
    G3DUI *gui = subedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DSUBDIVIDERTYPE ) {
            G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) sel;
            uint32_t preview = level,
                     render  = ( sel->flags & SYNCLEVELS ) ? preview :
                                                             sdr->subdiv_render; 

            g3dsubdivider_setLevels ( sdr,
                                      preview, 
                                      render, 
                                      gui->engine_flags );

            /*g3dmodifier_modify_r ( sdr, gui->engine_flags );*/


            /*g3dmesh_setSubdivisionLevel ( mes, level, gui->engine_flags );*/
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWCURRENTOBJECT;
}
