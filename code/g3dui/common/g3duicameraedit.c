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
#define FOR_EACH_SELECTED_CAMERA                            \
    G3DUI *gui = camedit->gui;                             \
    G3DSCENE *sce = gui->sce;                              \
    LIST *ltmpselobj = sce->lsel;                          \
                                                           \
    while ( ltmpselobj ) {                                 \
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data; \
                                                           \
        if ( sel->type == G3DCAMERATYPE ) {                \
            G3DCAMERA *cam = ( G3DCAMERA * ) sel;          \


#define END_FOR                                            \
        }                                                  \
                                                           \
        ltmpselobj = ltmpselobj->next;                     \
    }


/******************************************************************************/
uint64_t g3duicameraedit_focal ( G3DUICAMERAEDIT *camedit,
                                 float            focal ) {
    if ( camedit->forKey ) {
        camedit->editedCamera->focal = focal;
    } else {
FOR_EACH_SELECTED_CAMERA
        cam->focal = focal;
END_FOR
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duicameraedit_dofRadius ( G3DUICAMERAEDIT *camedit,
                                     uint32_t         radius ) {
    if ( camedit->forKey ) {
        camedit->editedCamera->dof.radius = radius;
    } else {
FOR_EACH_SELECTED_CAMERA
        cam->dof.radius = radius;
END_FOR
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duicameraedit_dofNearBlur ( G3DUICAMERAEDIT *camedit,
                                       float            nearBlur ) {
    if ( camedit->forKey ) {
        camedit->editedCamera->dof.nearBlur = nearBlur;
    } else {
FOR_EACH_SELECTED_CAMERA
        cam->dof.nearBlur = nearBlur;
END_FOR
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duicameraedit_dofNoBlur ( G3DUICAMERAEDIT *camedit,
                                     float            noBlur ) {
    if ( camedit->forKey ) {
        camedit->editedCamera->dof.noBlur = noBlur;
    } else {
FOR_EACH_SELECTED_CAMERA
        cam->dof.noBlur = noBlur;
END_FOR
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duicameraedit_dofFarBlur ( G3DUICAMERAEDIT *camedit,
                                      float            farBlur ) {
    if ( camedit->forKey ) {
        camedit->editedCamera->dof.farBlur = farBlur;
    } else {
FOR_EACH_SELECTED_CAMERA
        cam->dof.farBlur = farBlur;
END_FOR
    }


    return REDRAWVIEW;
}


/******************************************************************************/
uint64_t g3duicameraedit_dofEnable ( G3DUICAMERAEDIT *camedit ) {
    if ( camedit->forKey ) {
        if ( camedit->editedCamera->obj.flags & CAMERADOF ) {
            camedit->editedCamera->obj.flags &= (~CAMERADOF);
        } else {
            camedit->editedCamera->obj.flags |= CAMERADOF;
        }
    } else {
FOR_EACH_SELECTED_CAMERA
        if ( cam->obj.flags & CAMERADOF ) {
            cam->obj.flags &= (~CAMERADOF);
        } else {
            cam->obj.flags |= CAMERADOF;
        }
END_FOR
    }


    return REDRAWVIEW;
}
