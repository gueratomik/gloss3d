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
uint64_t g3duilightedit_setSpecularityCbk ( G3DUILIGHTEDIT *ligedit,
                                            uint32_t        red,
                                            uint32_t        green,
                                            uint32_t        blue ) {
    G3DUI *gui = ligedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DLIGHTTYPE ) {
            G3DLIGHT *lig = ( G3DLIGHT * ) sel;

            lig->specularColor.r = red;
            lig->specularColor.g = green;
            lig->specularColor.b = blue;
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duilightedit_unsetSpotCbk ( G3DUILIGHTEDIT *ligedit ) {
    G3DUI *gui = ligedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DLIGHTTYPE ) {
            G3DLIGHT *lig = ( G3DLIGHT * ) sel;

            g3dlight_unsetSpot ( lig );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duilightedit_setSpotCbk ( G3DUILIGHTEDIT *ligedit,
                                     float           spotLength,
                                     float           spotAngle,
                                     float           spotFadeAngle ) {
    G3DUI *gui = ligedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DLIGHTTYPE ) {
            G3DLIGHT *lig = ( G3DLIGHT * ) sel;

            g3dlight_setSpot ( lig, spotLength, spotAngle, spotFadeAngle );
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duilightedit_castShadowsCbk ( G3DUILIGHTEDIT *ligedit ) {
    G3DUI *gui = ligedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DLIGHTTYPE ) {
            G3DLIGHT *lig = ( G3DLIGHT * ) sel;

            if ( lig->obj.flags & LIGHTCASTSHADOWS ) {
                lig->obj.flags &= (~LIGHTCASTSHADOWS);
            } else {
                lig->obj.flags |= LIGHTCASTSHADOWS;
            }
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duilightedit_setSoftShadowsCbk ( G3DUILIGHTEDIT *ligedit ) {
    G3DUI *gui = ligedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DLIGHTTYPE ) {
            G3DLIGHT *lig = ( G3DLIGHT * ) sel;

            if ( lig->obj.flags & LIGHTHARDSHADOWS ) {
                lig->obj.flags &= (~LIGHTHARDSHADOWS);

                lig->obj.flags |= LIGHTSOFTSHADOWS;
            } else {
                lig->obj.flags &= (~LIGHTSOFTSHADOWS);

                lig->obj.flags |= LIGHTHARDSHADOWS;
            }
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duilightedit_shadowRadiusCbk ( G3DUILIGHTEDIT *ligedit,
                                          float           shadowRadius ) {
    G3DUI *gui = ligedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DLIGHTTYPE ) {
            G3DLIGHT *lig = ( G3DLIGHT * ) sel;

            lig->shadowRadius = shadowRadius;
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duilightedit_shadowSampleCbk ( G3DUILIGHTEDIT *ligedit,
                                          uint32_t        shadowSample ) {
    G3DUI *gui = ligedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DLIGHTTYPE ) {
            G3DLIGHT *lig = ( G3DLIGHT * ) sel;

            lig->shadowSample = shadowSample;
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duilightedit_setDiffuseCbk ( G3DUILIGHTEDIT *ligedit
                                        uint32_t        red,
                                        uint32_t        green,
                                        uint32_t        blue ) {
    G3DUI *gui = ligedit->gui;
    G3DSCENE *sce = gui->sce;
    LIST *ltmpselobj = sce->lsel;

    while ( ltmpselobj ) {
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data;

        if ( sel->type == G3DLIGHTTYPE ) {
            G3DLIGHT *lig = ( G3DLIGHT * ) sel;

            lig->diffuseColor.r = red;
            lig->diffuseColor.g = green;
            lig->diffuseColor.b = blue;
        }

        ltmpselobj = ltmpselobj->next;
    }


    return REDRAWVIEW;
}
