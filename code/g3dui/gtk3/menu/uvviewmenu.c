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

#define UVVIEWMENU_DIFFUSE      "Diffuse"
#define UVVIEWMENU_SPECULAR     "Specular"
#define UVVIEWMENU_ALPHA        "Alpha"
#define UVVIEWMENU_BUMP         "Bump"
#define UVVIEWMENU_DISPLACEMENT "Displacement"
#define UVVIEWMENU_AMBIENT      "Ambient"
#define UVVIEWMENU_REFLECTION   "Reflection"
#define UVVIEWMENU_REFRACTION   "Refraction"

/******************************************************************************/
static uint32_t toggleRefractionCond ( G3DUIMENU *menu,
                                       void      *data ) {
    M3DUIVIEW *view = ( M3DUIVIEW * ) data;
    uint64_t channel = GETCHANNEL(view->mui->engine_flags);

    return ( channel == REFRACTIONCHANNELID ) ? MENU_CONDITION_SENSITIVE |
                                                MENU_CONDITION_ACTIVE :
                                                MENU_CONDITION_SENSITIVE;
}

/******************************************************************************/
static uint32_t toggleReflectionCond ( G3DUIMENU *menu,
                                       void      *data ) {
    M3DUIVIEW *view = ( M3DUIVIEW * ) data;
    uint64_t channel = GETCHANNEL(view->mui->engine_flags);

    return ( channel == REFLECTIONCHANNELID ) ? MENU_CONDITION_SENSITIVE |
                                                MENU_CONDITION_ACTIVE :
                                                MENU_CONDITION_SENSITIVE;
}

/******************************************************************************/
static uint32_t toggleAmbientCond ( G3DUIMENU *menu,
                                    void      *data ) {
    M3DUIVIEW *view = ( M3DUIVIEW * ) data;
    uint64_t channel = GETCHANNEL(view->mui->engine_flags);

    return ( channel == AMBIENTCHANNELID ) ? MENU_CONDITION_SENSITIVE |
                                             MENU_CONDITION_ACTIVE :
                                             MENU_CONDITION_SENSITIVE;
}

/******************************************************************************/
static uint32_t toggleBumpCond ( G3DUIMENU *menu,
                                 void      *data ) {
    M3DUIVIEW *view = ( M3DUIVIEW * ) data;
    uint64_t channel = GETCHANNEL(view->mui->engine_flags);

    return ( channel == BUMPCHANNELID ) ? MENU_CONDITION_SENSITIVE |
                                          MENU_CONDITION_ACTIVE :
                                          MENU_CONDITION_SENSITIVE;
}

/******************************************************************************/
static uint32_t toggleAlphaCond ( G3DUIMENU *menu,
                                  void      *data ) {
    M3DUIVIEW *view = ( M3DUIVIEW * ) data;
    uint64_t channel = GETCHANNEL(view->mui->engine_flags);

    return ( channel == ALPHACHANNELID ) ? MENU_CONDITION_SENSITIVE |
                                           MENU_CONDITION_ACTIVE :
                                           MENU_CONDITION_SENSITIVE;
}

/******************************************************************************/
static uint32_t toggleDisplacementCond ( G3DUIMENU *menu,
                                         void      *data ) {
    M3DUIVIEW *view = ( M3DUIVIEW * ) data;
    uint64_t channel = GETCHANNEL(view->mui->engine_flags);

    return ( channel == DISPLACEMENTCHANNELID ) ? MENU_CONDITION_SENSITIVE |
                                                  MENU_CONDITION_ACTIVE :
                                                  MENU_CONDITION_SENSITIVE;
}

/******************************************************************************/
static uint32_t toggleSpecularCond ( G3DUIMENU *menu,
                                     void      *data ) {
    M3DUIVIEW *view = ( M3DUIVIEW * ) data;
    uint64_t channel = GETCHANNEL(view->mui->engine_flags);

    return ( channel == SPECULARCHANNELID ) ? MENU_CONDITION_SENSITIVE |
                                              MENU_CONDITION_ACTIVE :
                                              MENU_CONDITION_SENSITIVE;
}

/******************************************************************************/
static uint32_t toggleDiffuseCond ( G3DUIMENU *menu,
                                    void      *data ) {
    M3DUIVIEW *view = ( M3DUIVIEW * ) data;
    uint64_t channel = GETCHANNEL(view->mui->engine_flags);

    return ( channel == DIFFUSECHANNELID ) ? MENU_CONDITION_SENSITIVE |
                                             MENU_CONDITION_ACTIVE :
                                             MENU_CONDITION_SENSITIVE;
}

/******************************************************************************/
static uint64_t toggleChannelCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    M3DUIVIEW *view = ( M3DUIVIEW * ) data;

    if ( strcmp ( menu->name, UVVIEWMENU_DIFFUSE     ) == 0x00 ) {
        SHOWCHANNEL(view->mui->engine_flags,DIFFUSECHANNELID);
    }

    if ( strcmp ( menu->name, UVVIEWMENU_SPECULAR    ) == 0x00 ) {
        SHOWCHANNEL(view->mui->engine_flags,SPECULARCHANNELID);
    }

    if ( strcmp ( menu->name, UVVIEWMENU_DISPLACEMENT) == 0x00 ) {
        SHOWCHANNEL(view->mui->engine_flags,DISPLACEMENTCHANNELID);
    }

    if ( strcmp ( menu->name, UVVIEWMENU_ALPHA       ) == 0x00 ) {
        SHOWCHANNEL(view->mui->engine_flags,ALPHACHANNELID);
    }

    if ( strcmp ( menu->name, UVVIEWMENU_BUMP        ) == 0x00 ) {
        SHOWCHANNEL(view->mui->engine_flags,BUMPCHANNELID);
    }

    if ( strcmp ( menu->name, UVVIEWMENU_AMBIENT     ) == 0x00 ) {
        SHOWCHANNEL(view->mui->engine_flags,AMBIENTCHANNELID);
    }

    if ( strcmp ( menu->name, UVVIEWMENU_REFLECTION  ) == 0x00 ) {
        SHOWCHANNEL(view->mui->engine_flags,REFLECTIONCHANNELID);
    }

    if ( strcmp ( menu->name, UVVIEWMENU_REFRACTION  ) == 0x00 ) {
        SHOWCHANNEL(view->mui->engine_flags,REFRACTIONCHANNELID);
    }

    /*** Note: buffers are resized via m3dui_resizeBuffers()    ***/
    /*** located in : gtk3/menu/uvviewmenu.c, gtk3/m3duiview.c  ***/
    /*** and gtk3/m3duichannelimagecreator.c                    ***/
    m3dui_resizeBuffers ( view->mui );


    return UPDATEUVVIEWMENU | REDRAWUVMAPEDITOR;
}

/******************************************************************************/

static G3DUIMENU uvview_menu_refraction   = { NULL,
                                              UVVIEWMENU_REFRACTION,
                                              CLASS_VIEW_MENU_DEFAULT,
                                              G3DUIMENUTYPE_TOGGLEBUTTON,
                                              toggleRefractionCond,
                                              toggleChannelCbk };

static G3DUIMENU uvview_menu_reflection   = { NULL,
                                              UVVIEWMENU_REFLECTION,
                                              CLASS_VIEW_MENU_DEFAULT,
                                              G3DUIMENUTYPE_TOGGLEBUTTON,
                                              toggleReflectionCond,
                                              toggleChannelCbk };

static G3DUIMENU uvview_menu_ambient      = { NULL,
                                              UVVIEWMENU_AMBIENT,
                                              CLASS_VIEW_MENU_DEFAULT,
                                              G3DUIMENUTYPE_TOGGLEBUTTON,
                                              toggleAmbientCond,
                                              toggleChannelCbk };

static G3DUIMENU uvview_menu_alpha         = { NULL,
                                              UVVIEWMENU_ALPHA,
                                              CLASS_VIEW_MENU_DEFAULT,
                                              G3DUIMENUTYPE_TOGGLEBUTTON,
                                              toggleAlphaCond,
                                              toggleChannelCbk };

static G3DUIMENU uvview_menu_bump         = { NULL,
                                              UVVIEWMENU_BUMP,
                                              CLASS_VIEW_MENU_DEFAULT,
                                              G3DUIMENUTYPE_TOGGLEBUTTON,
                                              toggleBumpCond,
                                              toggleChannelCbk };

static G3DUIMENU uvview_menu_displacement = { NULL,
                                              UVVIEWMENU_DISPLACEMENT,
                                              CLASS_VIEW_MENU_DEFAULT,
                                              G3DUIMENUTYPE_TOGGLEBUTTON,
                                              toggleDisplacementCond,
                                              toggleChannelCbk };

static G3DUIMENU uvview_menu_specular     = { NULL,
                                              UVVIEWMENU_SPECULAR,
                                              CLASS_VIEW_MENU_DEFAULT,
                                              G3DUIMENUTYPE_TOGGLEBUTTON,
                                              toggleSpecularCond,
                                              toggleChannelCbk };

static G3DUIMENU uvview_menu_diffuse      = { NULL,
                                              UVVIEWMENU_DIFFUSE,
                                              CLASS_VIEW_MENU_DEFAULT,
                                              G3DUIMENUTYPE_TOGGLEBUTTON,
                                              toggleDiffuseCond,
                                              toggleChannelCbk };

/******************************************************************************/
static G3DUIMENU *uvviewchildren[] = { &uvview_menu_diffuse,
                                       &uvview_menu_specular,
                                       &uvview_menu_displacement,
                                       &uvview_menu_alpha,
                                       &uvview_menu_bump,
                                      /* &uvview_menu_ambient,*/
                                       &uvview_menu_reflection,
                                       &uvview_menu_refraction,
                                        NULL };

static G3DUIMENU uvview_menu = { NULL,
                                 "_Channel",
                                 CLASS_VIEW_MENU_BAR,
                                 G3DUIMENUTYPE_SUBMENU,
                                 NULL,
                                 NULL,
                                .nodes = uvviewchildren };

/******************************************************************************/
/******************************************************************************/
static G3DUIMENU *uvviewrootchildren[] = { &uvview_menu,
                                            NULL };

static G3DUIMENU uvviewrootnode = { NULL,
                                    OPTIONMENUNAME,
                                    CLASS_VIEW_MENU_BAR,
                                    G3DUIMENUTYPE_MENUBAR,
                                    NULL,
                                    NULL,
                                   .nodes = uvviewrootchildren };

/******************************************************************************/
G3DUIMENU *g3duimenu_getUVViewMenuNode ( ) {
    return &uvviewrootnode;
}
