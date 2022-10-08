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
uint64_t g3duiffdedit_radius ( G3DUIFFDEDIT *ffdedit,
                                  G3DUIAXIS     axis,
                                  float         radius ) {
    G3DUI *gui = ffdedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DFFDTYPE ) {
            G3DFFD *ffd = ( G3DFFD * ) sel;

            g3dobject_deactivate ( ( G3DOBJECT * ) ffd, gui->engine_flags );

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
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW | REDRAWOBJECTLIST;
}

/******************************************************************************/
uint64_t g3duiffdedit_slice ( G3DUIFFDEDIT *ffdedit,
                                 G3DUIAXIS     axis,
                                 int           slice ) {
    G3DUI *gui = ffdedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DFFDTYPE ) {
            G3DFFD *ffd = ( G3DFFD * ) sel;

            g3dobject_deactivate ( ( G3DOBJECT * ) ffd, gui->engine_flags );

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
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW | REDRAWOBJECTLIST;
}
