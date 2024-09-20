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
uint64_t g3duitextureedit_toggleRestrict ( G3DUITEXTUREEDIT *texedit ) {
    G3DUI *gui = texedit->gui;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = texedit->obj;

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


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duitextureedit_toggleRepeat ( G3DUITEXTUREEDIT *texedit ) {
    G3DUI *gui = texedit->gui;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = texedit->obj;

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


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duitextureedit_setUVMap ( G3DUITEXTUREEDIT *texedit,
                                        uint32_t         rank ) {
    G3DUI *gui = texedit->gui;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = texedit->obj;

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes  = ( G3DMESH * ) obj;
            G3DUVMAP *map = g3dmesh_getUVMapByRank ( mes, rank );
            LIST *ltmpseltex = mes->selectedTextureList;

            while ( ltmpseltex ) {
                G3DTEXTURE *tex = ( G3DTEXTURE * ) ltmpseltex->data;

                tex->map = ( map ) ? map : NULL;

                ltmpseltex = ltmpseltex->next;
            }
        }
    }


    return REDRAWVIEW;
}
