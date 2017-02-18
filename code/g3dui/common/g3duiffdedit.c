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
void common_g3duiffdedit_radiusCbk ( G3DUI *gui, G3DUIAXIS axis,
                                                 float radius ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DFFDTYPE ) ) {
        G3DFFD *ffd = ( G3DFFD * ) obj;

        g3dobject_deactivate ( ffd, gui->flags );

        if ( axis == G3DUIXAXIS ) g3dffd_shape ( ffd, ffd->nbx,
                                                      ffd->nby,
                                                      ffd->nbz,
                                                      radius,
                                                      ffd->locmax.y,
                                                      ffd->locmax.z );

        if ( axis == G3DUIYAXIS ) g3dffd_shape ( ffd, ffd->nbx,
                                                      ffd->nby,
                                                      ffd->nbz,
                                                      ffd->locmax.x,
                                                      radius,
                                                      ffd->locmax.z );

        if ( axis == G3DUIZAXIS ) g3dffd_shape ( ffd, ffd->nbx,
                                                      ffd->nby,
                                                      ffd->nbz,
                                                      ffd->locmax.x,
                                                      ffd->locmax.y,
                                                      radius );

        g3dui_redrawGLViews ( gui );
    }
}

/******************************************************************************/
void common_g3duiffdedit_sliceCbk ( G3DUI *gui, G3DUIAXIS axis, int slice ) {
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( obj && ( obj->type == G3DFFDTYPE ) ) {
        G3DFFD *ffd = ( G3DFFD * ) obj;

        g3dobject_deactivate ( ffd, gui->flags );

        if ( axis == G3DUIXAXIS ) g3dffd_shape ( ffd, slice, 
                                                      ffd->nby,
                                                      ffd->nbz,
                                                      ffd->locmax.x,
                                                      ffd->locmax.y,
                                                      ffd->locmax.z );

        if ( axis == G3DUIYAXIS ) g3dffd_shape ( ffd, ffd->nbx, 
                                                      slice,
                                                      ffd->nbz,
                                                      ffd->locmax.x,
                                                      ffd->locmax.y,
                                                      ffd->locmax.z );

        if ( axis == G3DUIZAXIS ) g3dffd_shape ( ffd, ffd->nbx, 
                                                      ffd->nby,
                                                      slice,
                                                      ffd->locmax.x,
                                                      ffd->locmax.y,
                                                      ffd->locmax.z );

        g3dui_redrawGLViews ( gui );
    }
}
