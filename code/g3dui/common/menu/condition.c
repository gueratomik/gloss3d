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
uint32_t objectModeOnly ( G3DUI *gui ) {
    return ( gui->engine_flags & VIEWOBJECT ) ? 0x01 : 0x00;
}

/******************************************************************************/
uint32_t vertexModeOnly ( G3DUI *gui ) {
    return ( gui->engine_flags & VIEWVERTEX ) ? 0x01 : 0x00;
}

/******************************************************************************/
uint32_t faceModeOnly ( G3DUI *gui ) {
    return ( gui->engine_flags & VIEWFACE ) ? 0x01 : 0x00;
}

/******************************************************************************/
uint32_t sculptModeOnly ( G3DUI *gui ) {
    return ( gui->engine_flags & VIEWSCULPT ) ? 0x01 : 0x00;
}

/******************************************************************************/
uint32_t edgeModeOnly ( G3DUI *gui ) {
    return ( gui->engine_flags & VIEWEDGE ) ? 0x01 : 0x00;
}

/******************************************************************************/
uint32_t skinModeOnly ( G3DUI *gui ) {
    return ( gui->engine_flags & VIEWSKIN ) ? 0x01 : 0x00;
}

/******************************************************************************/
uint32_t objectMode_skinSelected ( G3DUI *gui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    return  ( ( obj                            ) && 
              ( obj->type & SKIN               ) &&
              ( gui->engine_flags & VIEWOBJECT ) ) ? 0x01 : 0x00;
}

/******************************************************************************/
uint32_t objectMode_objectSelected ( G3DUI *gui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    return  ( ( obj                            ) && 
              ( gui->engine_flags & VIEWOBJECT ) ) ? 0x01 : 0x00;
}

/******************************************************************************/
uint32_t objectMode_boneSelected ( G3DUI *gui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    return  ( ( obj                            ) && 
              ( obj->type & BONE               ) &&
              ( gui->engine_flags & VIEWOBJECT ) ) ? 0x01 : 0x00;
}

/******************************************************************************/
uint32_t objectMode_boneOrSkinSelected ( G3DUI *gui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    return  ( ( obj                            ) && 
            ( ( obj->type & BONE               ) ||
              ( obj->type & SKIN               ) ) &&
              ( gui->engine_flags & VIEWOBJECT ) ) ? 0x01 : 0x00;
}
