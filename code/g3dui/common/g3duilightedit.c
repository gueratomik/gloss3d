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
#define FOR_EACH_SELECTED_LIGHT                            \
    G3DUI *gui = ligedit->gui;                             \
    G3DSCENE *sce = gui->sce;                              \
    LIST *ltmpselobj = sce->lsel;                          \
                                                           \
    while ( ltmpselobj ) {                                 \
        G3DOBJECT *sel = ( G3DOBJECT * ) ltmpselobj->data; \
                                                           \
        if ( sel->type == G3DLIGHTTYPE ) {                 \
            G3DLIGHT *lig = ( G3DLIGHT * ) sel;            \


#define END_FOR                                            \
        }                                                  \
                                                           \
        ltmpselobj = ltmpselobj->next;                     \
    }

/******************************************************************************/
uint64_t g3duilightedit_setSpecularity ( G3DUILIGHTEDIT *ligedit,
                                            uint32_t        red,
                                            uint32_t        green,
                                            uint32_t        blue ) {
    if ( ligedit->forKey ) {
        ligedit->editedLight->specularColor.r = red;
        ligedit->editedLight->specularColor.g = green;
        ligedit->editedLight->specularColor.b = blue;
    } else {
FOR_EACH_SELECTED_LIGHT
        lig->specularColor.r = red;
        lig->specularColor.g = green;
        lig->specularColor.b = blue;
END_FOR

        g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duilightedit_unsetSpot ( G3DUILIGHTEDIT *ligedit ) {
    if ( ligedit->forKey ) {
        g3dlight_unsetSpot ( ligedit->editedLight );
    } else {
FOR_EACH_SELECTED_LIGHT
        g3dlight_unsetSpot ( lig );
END_FOR

        g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duilightedit_setSpot ( G3DUILIGHTEDIT *ligedit ) {
    if ( ligedit->forKey ) {
        g3dlight_setSpot ( ligedit->editedLight );
    } else {
FOR_EACH_SELECTED_LIGHT
        g3dlight_setSpot ( lig );
END_FOR
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duilightedit_setSpotAngle ( G3DUILIGHTEDIT *ligedit,
                                       float           spotAngle ) {
    if ( ligedit->forKey ) {
        g3dlight_setSpotAngle ( ligedit->editedLight, spotAngle );
    } else {
FOR_EACH_SELECTED_LIGHT
        g3dlight_setSpotAngle ( lig, spotAngle );
END_FOR

        g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duilightedit_setSpotFadeAngle ( G3DUILIGHTEDIT *ligedit,
                                           float           spotFadeAngle ) {
    if ( ligedit->forKey ) {
        g3dlight_setSpotFadeAngle ( ligedit->editedLight, spotFadeAngle );
    } else {
FOR_EACH_SELECTED_LIGHT
        g3dlight_setSpotFadeAngle ( lig, spotFadeAngle );
END_FOR

        g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duilightedit_setSpotLength ( G3DUILIGHTEDIT *ligedit,
                                        float           spotLength ) {
    if ( ligedit->forKey ) {
        g3dlight_setSpotLength ( ligedit->editedLight, spotLength );
    } else {
FOR_EACH_SELECTED_LIGHT
        g3dlight_setSpotLength ( lig, spotLength );
END_FOR

        g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duilightedit_castShadows ( G3DUILIGHTEDIT *ligedit ) {
    if ( ligedit->forKey ) {
        if ( ligedit->editedLight->obj.flags & LIGHTCASTSHADOWS ) {
            ligedit->editedLight->obj.flags &= (~LIGHTCASTSHADOWS);
        } else {
            ligedit->editedLight->obj.flags |= LIGHTCASTSHADOWS;
        }
    } else {
FOR_EACH_SELECTED_LIGHT
        if ( lig->obj.flags & LIGHTCASTSHADOWS ) {
            lig->obj.flags &= (~LIGHTCASTSHADOWS);
        } else {
            lig->obj.flags |= LIGHTCASTSHADOWS;
        }
END_FOR

        g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duilightedit_setSoftShadows ( G3DUILIGHTEDIT *ligedit ) {
    if ( ligedit->forKey ) {
        if ( ligedit->editedLight->obj.flags & LIGHTHARDSHADOWS ) {
            ligedit->editedLight->obj.flags &= (~LIGHTHARDSHADOWS);

            ligedit->editedLight->obj.flags |= LIGHTSOFTSHADOWS;
        } else {
            ligedit->editedLight->obj.flags &= (~LIGHTSOFTSHADOWS);

            ligedit->editedLight->obj.flags |= LIGHTHARDSHADOWS;
        }
    } else {
FOR_EACH_SELECTED_LIGHT
        if ( lig->obj.flags & LIGHTHARDSHADOWS ) {
            lig->obj.flags &= (~LIGHTHARDSHADOWS);

            lig->obj.flags |= LIGHTSOFTSHADOWS;
        } else {
            lig->obj.flags &= (~LIGHTSOFTSHADOWS);

            lig->obj.flags |= LIGHTHARDSHADOWS;
        }
END_FOR

        g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duilightedit_shadowRadius ( G3DUILIGHTEDIT *ligedit,
                                       float           shadowRadius ) {
    if ( ligedit->forKey ) {
        ligedit->editedLight->shadowRadius = shadowRadius;
    } else {
FOR_EACH_SELECTED_LIGHT
        lig->shadowRadius = shadowRadius;
END_FOR

        g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duilightedit_shadowSample ( G3DUILIGHTEDIT *ligedit,
                                       uint32_t        shadowSample ) {
    if ( ligedit->forKey ) {
        ligedit->editedLight->shadowSample = shadowSample;
    } else {
FOR_EACH_SELECTED_LIGHT
        lig->shadowSample = shadowSample;
END_FOR

        g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );
    }

    return REDRAWVIEW;
}

/******************************************************************************/
uint64_t g3duilightedit_setDiffuse ( G3DUILIGHTEDIT *ligedit,
                                     uint32_t        red,
                                     uint32_t        green,
                                     uint32_t        blue ) {
    if ( ligedit->forKey ) {
        ligedit->editedLight->diffuseColor.r = red;
        ligedit->editedLight->diffuseColor.g = green;
        ligedit->editedLight->diffuseColor.b = blue;

    } else {
FOR_EACH_SELECTED_LIGHT
        lig->diffuseColor.r = red;
        lig->diffuseColor.g = green;
        lig->diffuseColor.b = blue;
END_FOR

        g3dobject_update_r( G3DOBJECTCAST(sce), 0x00, gui->engine_flags );
    }

    return REDRAWVIEW;
}
