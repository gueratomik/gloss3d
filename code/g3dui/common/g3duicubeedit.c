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
uint64_t g3duicubeedit_sliceCbk ( G3DUICUBEEDIT *cubedit,
                                  G3DUIAXIS      axis,
                                  int            slice ) {
    G3DUI *gui = cubedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DCUBETYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;

            CUBEDATASTRUCT *cds = ( CUBEDATASTRUCT * ) pri->data;

            if ( axis == G3DUIXAXIS ) g3dcube_build ( pri,
                                                      slice, 
                                                      cds->nby, 
                                                      cds->nbz,
                                                      cds->radius );

            if ( axis == G3DUIYAXIS ) g3dcube_build ( pri,
                                                      cds->nbx, 
                                                      slice, 
                                                      cds->nbz, 
                                                      cds->radius );

            if ( axis == G3DUIZAXIS ) g3dcube_build ( pri,
                                                      cds->nbx, 
                                                      cds->nby, 
                                                      slice,
                                                      cds->radius );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duicubeedit_radiusCbk ( G3DUICUBEEDIT *cubedit,
                                   float          radius ) {
    G3DUI *gui = cubedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DCUBETYPE ) {
            G3DPRIMITIVE *pri = ( G3DPRIMITIVE * ) sel;

            CUBEDATASTRUCT *cds = ( CUBEDATASTRUCT * ) pri->data;

            g3dcube_build ( pri,
                            cds->nbx,
                            cds->nby,
                            cds->nbz,
                            radius );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}
