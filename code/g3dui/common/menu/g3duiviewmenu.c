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
static uint64_t useDefaultCameraCbk ( G3DUIMENU *menu, 
                                      void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;

    view->cam = view->defcam;


    return REDRAWVIEWMENU | REDRAWVIEW;
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


    return REDRAWVIEWMENU | REDRAWVIEW;
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


    return REDRAWVIEWMENU | REDRAWVIEW;
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


    return REDRAWVIEWMENU | REDRAWVIEW;
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


    return REDRAWVIEWMENU | REDRAWVIEW;
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


    return REDRAWVIEWMENU | REDRAWVIEW;
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


    return REDRAWVIEWMENU | REDRAWVIEW;
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


    return REDRAWVIEWMENU | REDRAWVIEW;
}

/******************************************************************************/
static uint64_t resetCameraCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    G3DUIVIEW *view = ( G3DUIVIEW * ) data;
    G3DUI *gui = menu->gui;
    G3DOBJECT *objcam = ( G3DOBJECT * ) view->cam;

    memcpy ( &objcam->pos, &view->defcampos, sizeof ( G3DVECTOR ) );
    memcpy ( &objcam->rot, &view->defcamrot, sizeof ( G3DVECTOR ) );
    memcpy ( &objcam->sca, &view->defcamsca, sizeof ( G3DVECTOR ) );

    view->cam->focal = view->defcamfoc;


    g3dobject_updateMatrix_r ( objcam, gui->engine_flags );


    return REDRAWVIEW;
}

/******************************************************************************/
static G3DUIMENU view_menu_defcam   = { NULL,
                                        VIEWMENU_DEFAULTCAMERA,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        useDefaultCameraCbk };

static G3DUIMENU view_menu_selcam   = { NULL,
                                        VIEWMENU_SELECTEDCAMERA,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        useSelectedCameraCbk };

static G3DUIMENU view_menu_normals  = { NULL,
                                        VIEWMENU_NORMALS,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        toggleNormalsCbk };

static G3DUIMENU view_menu_bones    = { NULL,
                                        VIEWMENU_BONES,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        toggleBonesCbk };

static G3DUIMENU view_menu_grid     = { NULL,
                                        VIEWMENU_GRID,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        toggleGridCbk };

static G3DUIMENU view_menu_textures = { NULL,
                                        VIEWMENU_TEXTURES,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        toggleTexturesCbk };

static G3DUIMENU view_menu_bg       = { NULL,
                                        VIEWMENU_BACKGROUND,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        toggleBackgroundImageCbk };

static G3DUIMENU view_menu_lighting = { NULL,
                                        VIEWMENU_LIGHTING,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        toggleLightingCbk };

static G3DUIMENU view_menu_reset    = { NULL,
                                        VIEWMENU_RESET,
                                        G3DUIMENUTYPE_PUSHBUTTON,
                                        NULL,
                                        resetCameraCbk };

/******************************************************************************/
static G3DUIMENU view_menu = { NULL,
                               "_Options",
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = { &view_menu_defcam,
                                         &view_menu_selcam,
                                         &view_menu_normals,
                                         &view_menu_bones,
                                         &view_menu_grid,
                                         &view_menu_textures,
                                         &view_menu_bg,
                                         &view_menu_lighting,
                                         &view_menu_reset,
                                          NULL } };

/******************************************************************************/
/******************************************************************************/
static G3DUIMENU viewrootnode = { NULL,
                                  OPTIONMENUNAME,
                                  G3DUIMENUTYPE_MENUBAR,
                                  NULL,
                                  NULL,
                                 .nodes = { &view_menu,
                                             NULL } };

/******************************************************************************/
G3DUIMENU *g3duimenu_getViewMenuNode ( ) {
    return &viewrootnode;
}
