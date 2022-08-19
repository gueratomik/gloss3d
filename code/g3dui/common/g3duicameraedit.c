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
uint64_t g3duicameraedit_dofRadiusCbk ( G3DUICAMERAEDIT *camedit,
                                        uint32_t         radius ) {
    G3DUI *gui = camedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DCAMERATYPE ) {
            G3DCAMERA *cam = ( G3DCAMERA * ) sel;

            cam->dof.radius = radius;
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duicameraedit_dofNearBlurCbk ( G3DUICAMERAEDIT *camedit,
                                          float            nearBlur ) {
    G3DUI *gui = camedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DCAMERATYPE ) {
            G3DCAMERA *cam = ( G3DCAMERA * ) sel;

            cam->dof.nearBlur = nearBlur;
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duicameraedit_dofNoBlurCbk ( G3DUICAMERAEDIT *camedit,
                                        float            noBlur ) {
    G3DUI *gui = camedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DCAMERATYPE ) {
            G3DCAMERA *cam = ( G3DCAMERA * ) sel;

            cam->dof.noBlur = noBlur;
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duicameraedit_dofFarBlurCbk ( G3DUICAMERAEDIT *camedit,
                                         float            farBlur ) {
    G3DUI *gui = camedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DCAMERATYPE ) {
            G3DCAMERA *cam = ( G3DCAMERA * ) sel;

            cam->dof.farBlur = farBlur;
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}


/******************************************************************************/
uint64_t g3duicameraedit_dofEnableCbk ( G3DUICAMERAEDIT *camedit ) {
    G3DUI *gui = camedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DCAMERATYPE ) {
            G3DCAMERA *cam = ( G3DCAMERA * ) sel;

            if ( sel->flags & CAMERADOF ) {
                sel->flags &= (~CAMERADOF);
            } else {
                sel->flags |= CAMERADOF;
            }
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}
