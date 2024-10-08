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
static uint32_t gouraudActiveCond ( G3DUIMENU *menu,
                                    void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;

    return ( view->engine_flags == FILLINGGOURAUD ) ? MENU_CONDITION_SENSITIVE |
                                                      MENU_CONDITION_ACTIVE :
                                                      MENU_CONDITION_SENSITIVE;
}

/******************************************************************************/
static uint32_t flatActiveCond ( G3DUIMENU *menu,
                                 void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;

    return ( view->engine_flags == FILLINGFLAT ) ? MENU_CONDITION_SENSITIVE |
                                                   MENU_CONDITION_ACTIVE : 
                                                   MENU_CONDITION_SENSITIVE;
}

/******************************************************************************/
static uint32_t wireframeActiveCond ( G3DUIMENU *menu,
                                      void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;

    return ( view->engine_flags == FILLINGWIREFRAME ) ? MENU_CONDITION_SENSITIVE |
                                                        MENU_CONDITION_ACTIVE :
                                                        MENU_CONDITION_SENSITIVE;
}

/******************************************************************************/
static uint64_t setShadingCbk ( G3DUIMENU *menu, 
                                void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;
    GTK3G3DUIVIEW *gtk3view = ( GTK3G3DUIVIEW * ) view;
    
    gtk_gl_area_make_current ( gtk3view->glarea );

    return g3duiview_setShading ( view, menu->name );
}

/******************************************************************************/
static G3DUIMENU shading_menu_gouraud   = { NULL,
                                            SHADINGMENU_GOURAUD,
                                            CLASS_VIEW_MENU_DEFAULT,
                                            G3DUIMENUTYPE_TOGGLEBUTTON,
                                            gouraudActiveCond,
                                            setShadingCbk };

static G3DUIMENU shading_menu_flat      = { NULL,
                                            SHADINGMENU_FLAT,
                                            CLASS_VIEW_MENU_DEFAULT,
                                            G3DUIMENUTYPE_TOGGLEBUTTON,
                                            flatActiveCond,
                                            setShadingCbk };
 
static G3DUIMENU shading_menu_wireframe = { NULL,
                                            SHADINGMENU_WIREFRAME,
                                            CLASS_VIEW_MENU_DEFAULT,
                                            G3DUIMENUTYPE_TOGGLEBUTTON,
                                            wireframeActiveCond,
                                            setShadingCbk };

/*static G3DUIMENU shading_menu_phong     = { NULL,
                                            SHADINGMENU_PHONG,
                                            CLASS_VIEW_MENU_DEFAULT,
                                            G3DUIMENUTYPE_PUSHBUTTON,
                                            NULL,
                                            setShadingCbk };*/

/******************************************************************************/
static G3DUIMENU *shadingchildren[] = { &shading_menu_gouraud,
                                     &shading_menu_flat,
                                     &shading_menu_wireframe,
                                     /*&shading_menu_phong,*/
                                      NULL };

static G3DUIMENU shading_menu = { NULL,
                                  "_Shading",
                                  CLASS_VIEW_MENU_BAR,
                                  G3DUIMENUTYPE_SUBMENU,
                                  NULL,
                                  NULL,
                                 .nodes = shadingchildren };

/******************************************************************************/
static uint32_t useDefaultCameraCond ( G3DUIMENU *menu,
                                       void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;

    return ( view->cam == &view->defcam ) ? MENU_CONDITION_SENSITIVE |
                                            MENU_CONDITION_ACTIVE :
                                            MENU_CONDITION_SENSITIVE;
}

/******************************************************************************/
static uint64_t useDefaultCameraCbk ( G3DUIMENU *menu, 
                                      void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;

    view->cam = &view->defcam;


    return UPDATEVIEWMENU | REDRAWVIEW;
}

/******************************************************************************/
static uint64_t useSelectedCameraCbk ( G3DUIMENU *menu, 
                                       void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;
    G3DUI *gui = menu->gui;
    G3DSCENE  *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj && obj->type == G3DCAMERATYPE ) {
        G3DCAMERA *cam = ( G3DCAMERA * ) obj;

        g3duiview_useSelectedCamera ( view, cam );
    }


    return UPDATEVIEWMENU | REDRAWVIEW;
}

/******************************************************************************/
static uint32_t toggleNormalsCond ( G3DUIMENU *menu,
                                    void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;

    return ( view->engine_flags & VIEWNORMALS ) ? MENU_CONDITION_SENSITIVE |
                                                  MENU_CONDITION_ACTIVE :
                                                  MENU_CONDITION_SENSITIVE;
}

/******************************************************************************/
static uint64_t toggleNormalsCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;
    G3DUI *gui = menu->gui;

    if ( view->engine_flags & VIEWNORMALS ) {
        view->engine_flags &= (~VIEWNORMALS);
    } else {
        view->engine_flags |= VIEWNORMALS;
    }


    return UPDATEVIEWMENU | REDRAWVIEW;
}

/******************************************************************************/
static uint32_t toggleBonesCond ( G3DUIMENU *menu,
                                  void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;

    return ( view->engine_flags & HIDEBONES ) ? MENU_CONDITION_SENSITIVE :
                                                MENU_CONDITION_SENSITIVE |
                                                MENU_CONDITION_ACTIVE;
}

/******************************************************************************/
static uint64_t toggleBonesCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;
    G3DUI *gui = menu->gui;

    if ( view->engine_flags & HIDEBONES ) {
        view->engine_flags &= (~HIDEBONES);
    } else {
        view->engine_flags |= HIDEBONES;
    }


    return UPDATEVIEWMENU | REDRAWVIEW;
}

/******************************************************************************/
static uint32_t toggleLightingCond ( G3DUIMENU *menu,
                                     void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;

    return ( view->engine_flags & NOLIGHTING ) ? MENU_CONDITION_SENSITIVE :
                                                 MENU_CONDITION_SENSITIVE | 
                                                 MENU_CONDITION_ACTIVE;
}

/******************************************************************************/
static uint64_t toggleLightingCbk ( G3DUIMENU *menu, 
                                    void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;
    G3DUI *gui = menu->gui;

    /*** Note: gui->sce is NULL when this widget is initialized ***/
    /*** because g3dengine needs an initialised widget first ***/
    if ( gui->sce ) {
        if ( view->engine_flags & NOLIGHTING ) {
            view->engine_flags &= (~NOLIGHTING);
        } else {
            view->engine_flags |= NOLIGHTING;
        }
    }

#ifdef unused
    gtk_widget_queue_draw ( ggt->curogl );
    /*** we call it twice because the lighting status is returned by ***/
    /*** g3dobject_draw(), which means it must at least be called once ***/
    /*** and then called again to tell the engine to turn on the default ***/
    /*** light or not. EDIT: does not work very well but I leave it here ***/
    /*** just so that we  dont forget about what has just been described ***/
    gtk_widget_queue_draw ( ggt->curogl );

    gtk_view_updateMenu ( gvw );
#endif


    return UPDATEVIEWMENU | REDRAWVIEW;
}

/******************************************************************************/
static uint32_t toggleGridCond ( G3DUIMENU *menu,
                                 void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;

    return ( view->engine_flags & HIDEGRID ) ? MENU_CONDITION_SENSITIVE :
                                               MENU_CONDITION_SENSITIVE |
                                               MENU_CONDITION_ACTIVE;
}

/******************************************************************************/
static uint64_t toggleGridCbk ( G3DUIMENU *menu, 
                                void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;
    G3DUI *gui = menu->gui;

    if ( view->engine_flags & HIDEGRID ) {
        view->engine_flags &= (~HIDEGRID);
    } else {
        view->engine_flags |= HIDEGRID;
    }


    return UPDATEVIEWMENU | REDRAWVIEW;
}

/******************************************************************************/
static uint32_t toggleTexturesCond ( G3DUIMENU *menu,
                                     void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;

    return ( view->engine_flags & NOTEXTURE ) ? MENU_CONDITION_SENSITIVE :
                                                MENU_CONDITION_ACTIVE |
                                                MENU_CONDITION_SENSITIVE;
}

/******************************************************************************/
static uint64_t toggleTexturesCbk ( G3DUIMENU *menu, 
                                    void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;
    G3DUI *gui = menu->gui;

    if ( view->engine_flags & NOTEXTURE ) {
        view->engine_flags &= (~NOTEXTURE);
    } else {
        view->engine_flags |= NOTEXTURE;
    }


    return UPDATEVIEWMENU | REDRAWVIEW;
}

/******************************************************************************/
static uint32_t toggleBackgroundImageCond ( G3DUIMENU *menu,
                                            void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;

    return ( view->engine_flags & NOBACKGROUNDIMAGE ) ? MENU_CONDITION_SENSITIVE :
                                                        MENU_CONDITION_ACTIVE |
                                                        MENU_CONDITION_SENSITIVE;
}

/******************************************************************************/
static uint64_t toggleBackgroundImageCbk ( G3DUIMENU *menu, 
                                           void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;
    G3DUI *gui = menu->gui;

    if ( view->engine_flags & NOBACKGROUNDIMAGE ) {
        view->engine_flags &= (~NOBACKGROUNDIMAGE);
    } else {
        view->engine_flags |= NOBACKGROUNDIMAGE;
    }


    return UPDATEVIEWMENU | REDRAWVIEW;
}

/******************************************************************************/
static uint64_t resetCameraCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;
    G3DUI *gui = menu->gui;
    G3DOBJECT *objcam = ( G3DOBJECT * ) view->cam;

    memcpy ( &objcam->pos, &view->defcampos, sizeof ( G3DVECTOR3F ) );
    memcpy ( &objcam->rot, &view->defcamrot, sizeof ( G3DVECTOR3F ) );
    memcpy ( &objcam->sca, &view->defcamsca, sizeof ( G3DVECTOR3F ) );

    view->cam->focal = view->defcamfoc;


    g3dobject_updateMatrix_r ( objcam, gui->engine_flags );


    return REDRAWVIEW;
}

/******************************************************************************/
static G3DUIMENU view_menu_defcam   = { NULL,
                                        VIEWMENU_DEFAULTCAMERA,
                                        CLASS_VIEW_MENU_DEFAULT,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        useDefaultCameraCond,
                                        useDefaultCameraCbk };

static G3DUIMENU view_menu_selcam   = { NULL,
                                        VIEWMENU_SELECTEDCAMERA,
                                        CLASS_VIEW_MENU_DEFAULT,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        useSelectedCameraCbk };

static G3DUIMENU view_menu_normals  = { NULL,
                                        VIEWMENU_NORMALS,
                                        CLASS_VIEW_MENU_DEFAULT,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        toggleNormalsCond,
                                        toggleNormalsCbk };

static G3DUIMENU view_menu_bones    = { NULL,
                                        VIEWMENU_BONES,
                                        CLASS_VIEW_MENU_DEFAULT,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        toggleBonesCond,
                                        toggleBonesCbk };

static G3DUIMENU view_menu_grid     = { NULL,
                                        VIEWMENU_GRID,
                                        CLASS_VIEW_MENU_DEFAULT,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        toggleGridCond,
                                        toggleGridCbk };

static G3DUIMENU view_menu_textures = { NULL,
                                        VIEWMENU_TEXTURES,
                                        CLASS_VIEW_MENU_DEFAULT,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        toggleTexturesCond,
                                        toggleTexturesCbk };

static G3DUIMENU view_menu_bg       = { NULL,
                                        VIEWMENU_BACKGROUND,
                                        CLASS_VIEW_MENU_DEFAULT,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        toggleBackgroundImageCond,
                                        toggleBackgroundImageCbk };

static G3DUIMENU view_menu_lighting = { NULL,
                                        VIEWMENU_LIGHTING,
                                        CLASS_VIEW_MENU_DEFAULT,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        toggleLightingCond,
                                        toggleLightingCbk };

static G3DUIMENU view_menu_reset    = { NULL,
                                        VIEWMENU_RESET,
                                        CLASS_VIEW_MENU_DEFAULT,
                                        G3DUIMENUTYPE_PUSHBUTTON,
                                        NULL,
                                        resetCameraCbk };

/******************************************************************************/
static G3DUIMENU *viewchildren[] = { &view_menu_defcam,
                                     &view_menu_selcam,
                                     &view_menu_normals,
                                     &view_menu_bones,
                                     &view_menu_grid,
                                     &view_menu_textures,
                                     &view_menu_bg,
                                     &view_menu_lighting,
                                     &view_menu_reset,
                                      NULL };

static G3DUIMENU view_menu = { NULL,
                               "_Options",
                               CLASS_VIEW_MENU_BAR,
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = viewchildren };

/******************************************************************************/
/******************************************************************************/
static G3DUIMENU *viewrootchildren[] = { &view_menu,
                                         &shading_menu,
                                          NULL };

static G3DUIMENU viewrootnode = { NULL,
                                  OPTIONMENUNAME,
                                  CLASS_VIEW_MENU_BAR,
                                  G3DUIMENUTYPE_MENUBAR,
                                  NULL,
                                  NULL,
                                 .nodes = viewrootchildren };

/******************************************************************************/
G3DUIMENU *g3duimenu_getViewMenuNode ( ) {
    return &viewrootnode;
}
