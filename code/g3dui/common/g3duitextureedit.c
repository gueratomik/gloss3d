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
void common_g3duitextureedit_toggleRestrictCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( gui->lock ) return;

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes  = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            if ( tex ) {
                if ( tex->flags & TEXTURERESTRICTED ) {
                    g3dtexture_unrestrict (tex );
                } else {
                    g3dtexture_restrict (tex );
                }
            }
        }
    }

    g3dui_redrawGLViews ( gui );
}

/******************************************************************************/
void common_g3duitextureedit_toggleRepeatCbk ( G3DUI *gui ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( gui->lock ) return;

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes  = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            if ( tex ) {
                if ( tex->flags & TEXTUREREPEATED ) {
                    tex->flags &= (~TEXTUREREPEATED);
                } else {
                    tex->flags |= TEXTUREREPEATED;
                }
            }
        }
    }

    g3dui_redrawGLViews ( gui );
}

/******************************************************************************/
void common_g3duitextureedit_setUVMapCbk ( G3DUI *gui, uint32_t mapID ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( gui->lock ) return;

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes  = ( G3DMESH * ) obj;
            LIST *ltmpseltex = mes->lseltex;

            while ( ltmpseltex ) {
                G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmpseltex->data;
                uint32_t curmap = 0x00;
                LIST *ltmpuvmap = mes->luvmap;

                while ( ltmpuvmap ) {
                    G3DUVMAP *uvmap = ( G3DUVMAP * ) ltmpuvmap->data;

                    if ( curmap == mapID ) {
                        tex->map = uvmap;
                    }

                    curmap++;

                    ltmpuvmap = ltmpuvmap->next;
                }

                ltmpseltex = ltmpseltex->next;
            }
        }
    }
}
