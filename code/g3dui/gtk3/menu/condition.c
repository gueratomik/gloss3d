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
#include <g3dui_gtk3.h>

/******************************************************************************/
uint32_t objectModeOnly ( G3DUIMENU *menu,
                          void      *data ) {
    G3DUI *gui = menu->gui;

    return ( gui->engine_flags & VIEWOBJECT ) ? 0x01 : 0x00;
}

/******************************************************************************/
uint32_t vertexModeOnly ( G3DUIMENU *menu,
                          void      *data ) {
    G3DUI *gui = menu->gui;

    return ( gui->engine_flags & VIEWVERTEX ) ? MENU_CONDITION_SENSITIVE : 0x00;
}

/******************************************************************************/
uint32_t faceModeOnly ( G3DUIMENU *menu,
                        void      *data ) {
    G3DUI *gui = menu->gui;

    return ( gui->engine_flags & VIEWFACE   ) ? MENU_CONDITION_SENSITIVE : 0x00;
}

/******************************************************************************/
uint32_t sculptModeOnly ( G3DUIMENU *menu,
                          void      *data ) {
    G3DUI *gui = menu->gui;

    return ( gui->engine_flags & VIEWSCULPT ) ? MENU_CONDITION_SENSITIVE : 0x00;
}

/******************************************************************************/
uint32_t edgeModeOnly ( G3DUIMENU *menu,
                        void      *data ) {
    G3DUI *gui = menu->gui;

    return ( gui->engine_flags & VIEWEDGE   ) ? MENU_CONDITION_SENSITIVE : 0x00;
}

/******************************************************************************/
uint32_t skinModeOnly ( G3DUIMENU *menu,
                        void      *data ) {
    G3DUI *gui = menu->gui;

    return ( gui->engine_flags & VIEWSKIN   ) ? MENU_CONDITION_SENSITIVE : 0x00;
}

/******************************************************************************/
uint32_t objectMode_skinSelected ( G3DUIMENU *menu,
                                   void      *data ) {
    G3DUI *gui = menu->gui;

    if ( gui->sce ) {
        G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

        return  ( ( obj                            ) && 
                  ( obj->type & SKIN               ) &&
                  ( gui->engine_flags & VIEWOBJECT ) ) ? MENU_CONDITION_SENSITIVE :
                                                         0x00;
    }

    return 0x00;
}

/******************************************************************************/
uint32_t object_objectSelectedAndUVFixed ( G3DUIMENU *menu,
                                           void      *data ) {
    G3DUI *gui = menu->gui;

    if ( gui->sce ) {
        G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );
        
        if( obj && obj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            if ( tex ) {
                G3DUVMAP *uvmap = tex->map;

                if ( uvmap ) {
                    if ( ((G3DOBJECT*)uvmap)->flags & UVMAPFIXED ) {
                        return MENU_CONDITION_SENSITIVE;
                    }
                }
            }
        }
    }

    return 0x00;
}

/******************************************************************************/
uint32_t objectMode_objectSelected ( G3DUIMENU *menu,
                                     void      *data ) {
    G3DUI *gui = menu->gui;

    if ( gui->sce ) {
        G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

        return  ( ( obj                            ) && 
                  ( gui->engine_flags & VIEWOBJECT ) ) ? MENU_CONDITION_SENSITIVE :
                                                         0x00;
    }

    return 0x00;
}

/******************************************************************************/
uint32_t objectMode_boneSelected ( G3DUIMENU *menu,
                                   void      *data ) {
    G3DUI *gui = menu->gui;

    if ( gui->sce ) {
        G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

        return  ( ( obj                            ) && 
                  ( obj->type & BONE               ) &&
                  ( gui->engine_flags & VIEWOBJECT ) ) ? MENU_CONDITION_SENSITIVE :
                                                         0x00;
    }

    return 0x00;
}

/******************************************************************************/
uint32_t objectMode_boneOrSkinSelected ( G3DUIMENU *menu,
                                         void      *data ) {
    G3DUI *gui = menu->gui;

    if ( gui->sce ) {
        G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

        return  ( ( obj                            ) && 
                ( ( obj->type & BONE               ) ||
                  ( obj->type & SKIN               ) ) &&
                  ( gui->engine_flags & VIEWOBJECT ) ) ? MENU_CONDITION_SENSITIVE :
                                                         0x00;
    }

    return 0x00;
}
