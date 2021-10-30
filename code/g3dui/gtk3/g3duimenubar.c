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

#define G3DUIMENUTYPE_PUSHBUTTON   0x00
#define G3DUIMENUTYPE_TOGGLEBUTTON 0x01
#define G3DUIMENUTYPE_SEPARATOR    0x02
#define G3DUIMENUTYPE_SUBMENU      0x03
#define G3DUIMENUTYPE_MENUBAR      0x04

/******************************************************************************/
static uint32_t objectModeOnly ( G3DUI *gui ) {
    return ( gui->engine_flags & VIEWOBJECT ) ? 0x01 : 0x00;
}

/******************************************************************************/
static uint32_t vertexModeOnly ( G3DUI *gui ) {
    return ( gui->engine_flags & VIEWVERTEX ) ? 0x01 : 0x00;
}

/******************************************************************************/
static uint32_t faceModeOnly ( G3DUI *gui ) {
    return ( gui->engine_flags & VIEWFACE ) ? 0x01 : 0x00;
}

/******************************************************************************/
static uint32_t edgeModeOnly ( G3DUI *gui ) {
    return ( gui->engine_flags & VIEWEDGE ) ? 0x01 : 0x00;
}

/******************************************************************************/
static uint32_t skinModeOnly ( G3DUI *gui ) {
    return ( gui->engine_flags & VIEWSKIN ) ? 0x01 : 0x00;
}

/******************************************************************************/
static uint32_t objectMode_skinSelected ( G3DUI *gui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    return  ( ( obj                            ) && 
              ( obj->type & SKIN               ) &&
              ( gui->engine_flags & VIEWOBJECT ) ) ? 0x01 : 0x00;
}

/******************************************************************************/
static uint32_t objectMode_objectSelected ( G3DUI *gui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    return  ( ( obj                            ) && 
              ( gui->engine_flags & VIEWOBJECT ) ) ? 0x01 : 0x00;
}

/******************************************************************************/
static uint32_t objectMode_boneSelected ( G3DUI *gui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    return  ( ( obj                            ) && 
              ( obj->type & BONE               ) &&
              ( gui->engine_flags & VIEWOBJECT ) ) ? 0x01 : 0x00;
}

/******************************************************************************/
static uint32_t objectMode_boneOrSkinSelected ( G3DUI *gui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    return  ( ( obj                            ) && 
            ( ( obj->type & BONE               ) ||
              ( obj->type & SKIN               ) ) &&
              ( gui->engine_flags & VIEWOBJECT ) ) ? 0x01 : 0x00;
}

/******************************************************************************/
static void parseMenu_r ( G3DUIMENU *node, 
                          G3DUIMENU *parent, 
                          G3DUI     *gui ) {
    switch ( node->type ) {
        case G3DUIMENUTYPE_SEPARATOR :
            node->item = gtk_separator_menu_item_new ( );
        break;

        case G3DUIMENUTYPE_TOGGLEBUTTON :
            node->item = gtk_check_menu_item_new_with_mnemonic ( node->name );

            gtk_widget_set_name ( node->item, node->name );

            if ( node->callback ) {
                g_signal_connect ( G_OBJECT ( node->item ), 
                                   "toggled", 
                                   node->callback, 
                                   gui );
            }
        break;

        case G3DUIMENUTYPE_PUSHBUTTON :
            node->item = gtk_menu_item_new_with_mnemonic ( node->name );

            gtk_widget_set_name ( node->item, node->name );

            if ( node->callback ) {
                g_signal_connect ( G_OBJECT ( node->item ), 
                                   "activate", 
                                   node->callback, 
                                   gui );
            }
        break;

        case G3DUIMENUTYPE_MENUBAR : {
            uint32_t i = 0x00;

            node->menu = gtk_menu_bar_new ( );

            gtk_widget_set_name ( node->menu, node->name );

            while ( node->nodes[i] != NULL ) {
                parseMenu_r ( node->nodes[i], node, gui );

                i++;
            }

            gtk_widget_show ( node->menu );
        } break;

        case G3DUIMENUTYPE_SUBMENU : {
            uint32_t i = 0x00;

            node->menu = gtk_menu_new ( );

            gtk_widget_set_name ( node->menu, node->name );

            node->item = gtk_menu_item_new_with_mnemonic ( node->name );

            gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( node->item ), node->menu );

            while ( node->nodes[i] != NULL ) {
                parseMenu_r ( node->nodes[i], node, gui );

                i++;
            }

            gtk_widget_show ( node->menu );

            gtk_widget_set_size_request ( node->menu, 0x60, 24 );
        } break;

        default :
        break;
    }

    if ( node->type == G3DUIMENUTYPE_SUBMENU  ) {
        int height = gtk_widget_get_allocated_height ( node->item );
        GdkRectangle gdkrec = { 0, 0, 0x60, height };

        /*gtk_widget_set_halign ( node->item, GTK_ALIGN_CENTER );*/

    /*gtk_widget_size_allocate ( node->item, &gdkrec );*/

        gtk_widget_set_size_request ( node->item, 0x60, height );
    }

    if ( parent ) {
        gtk_menu_shell_append ( GTK_MENU_SHELL ( parent->menu ), 
                                node->item );
    }

    gtk_widget_set_name ( node->item, node->name );
    gtk_widget_show     ( node->item );
}

/******************************************************************************/
static void updateMenu_r ( G3DUIMENU *node, 
                           G3DUI     *gui ) {
    switch ( node->type ) {
        case G3DUIMENUTYPE_SEPARATOR :
        break;

        case G3DUIMENUTYPE_TOGGLEBUTTON :

        break;

        case G3DUIMENUTYPE_PUSHBUTTON :
        break;

        case G3DUIMENUTYPE_MENUBAR :
        case G3DUIMENUTYPE_SUBMENU : {
            uint32_t i = 0x00;

            while ( node->nodes[i] != NULL ) {
                updateMenu_r ( node->nodes[i], gui );

                i++;
            }
        } break;

        default :
        break;
    }

    if ( node->item ) {
        if ( node->condition ) {
            if ( node->condition ( gui ) == 0x00 ) {
                gtk_widget_set_sensitive ( node->item, FALSE );
            } else {
                gtk_widget_set_sensitive ( node->item, TRUE  );
            }
        } else {
            gtk_widget_set_sensitive ( node->item, TRUE  );
        }
    }
}

/******************************************************************************/
void g3duitimeline_deleteKeysCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUITIMELINE *tim = ( G3DUITIMELINE * ) user_data;

    common_g3duitimeline_deleteSelectedKeys ( tim->grp.gui );
}

/******************************************************************************/
void g3duitimeline_scaleKeysCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUITIMELINE *tim = ( G3DUITIMELINE * ) user_data;

    tim->tdata->tool = TIME_SCALE_TOOL;
}

/******************************************************************************/
void g3duitimeline_selectKeysCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUITIMELINE *tim = ( G3DUITIMELINE * ) user_data;

    tim->tdata->tool = TIME_MOVE_TOOL;
}

/******************************************************************************/
static G3DUIMENU time_menu_delete = { TIMEMENU_DELETE,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      g3duitimeline_deleteKeysCbk };

static G3DUIMENU time_menu_scale = { TIMEMENU_SCALE,
                                     G3DUIMENUTYPE_PUSHBUTTON,
                                     NULL,
                                     g3duitimeline_scaleKeysCbk };

static G3DUIMENU time_menu_select = { TIMEMENU_SELECT,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      g3duitimeline_selectKeysCbk };

/******************************************************************************/
static G3DUIMENU time_menu = { "_Options",
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = { /*&time_menu_scale,
                                         &time_menu_select,*/
                                         &time_menu_delete,
                                          NULL } };

/******************************************************************************/
/******************************************************************************/
GtkWidget *createTimeContextMenu ( GtkWidget     *parent,
                                   G3DUITIMELINE *tim,
                                   char          *name,
                                   gint           width,
                                   gint           height ) {
    GdkRectangle gdkrec = { 0, 0, width, height };

    parseMenu_r ( &time_menu, NULL, tim );

    gtk_widget_size_allocate ( time_menu.menu, &gdkrec );

    gtk_widget_show ( time_menu.menu );


    return time_menu.menu;
}

/******************************************************************************/
void g3duiview_useDefaultCameraCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkView *gvw = ( GtkView * ) user_data;
    G3DUIVIEW *view = &gvw->view;
    G3DUI *gui = view->gui;
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    view->cam = view->defcam;

    gtk_widget_queue_draw ( ggt->curogl );

    gtk_view_updateMenu ( gvw );
}

/******************************************************************************/
void g3duiview_useSelectedCameraCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkView *gvw = ( GtkView * ) user_data;
    G3DUIVIEW *view = &gvw->view;
    G3DUI *gui = view->gui;
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;
    G3DSCENE  *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( obj && obj->type == G3DCAMERATYPE ) {
        G3DCAMERA *cam = ( G3DCAMERA * ) obj;

        common_g3duiview_useSelectedCamera ( view, cam );
    }

    gtk_widget_queue_draw ( ggt->curogl );

    gtk_view_updateMenu ( gvw );
}

/******************************************************************************/
void g3duiview_toggleNormalsCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkView *gvw = ( GtkView * ) user_data;
    G3DUIVIEW *view = &gvw->view;
    G3DUI *gui = view->gui;
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( view->engine_flags & VIEWNORMALS ) {
        view->engine_flags &= (~VIEWNORMALS);
    } else {
        view->engine_flags |= VIEWNORMALS;
    }

    gtk_widget_queue_draw ( ggt->curogl );

    gtk_view_updateMenu ( gvw );
}

/******************************************************************************/
void g3duiview_toggleBonesCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkView *gvw = ( GtkView * ) user_data;
    G3DUIVIEW *view = &gvw->view;
    G3DUI *gui = view->gui;
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( view->engine_flags & HIDEBONES ) {
        view->engine_flags &= (~HIDEBONES);
    } else {
        view->engine_flags |= HIDEBONES;
    }

    gtk_widget_queue_draw ( ggt->curogl );

    gtk_view_updateMenu ( gvw );
}

/******************************************************************************/
void g3duiview_toggleLightingCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkView *gvw = ( GtkView * ) user_data;
    G3DUIVIEW *view = &gvw->view;
    G3DUI *gui = view->gui;
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    /*** Note: gui->sce is NULL when this widget is initialized ***/
    /*** because g3dengine needs an initialised widget first ***/
    if ( gui->sce ) {
        if ( view->engine_flags & NOLIGHTING ) {
            view->engine_flags &= (~NOLIGHTING);

            g3dscene_turnLightsOn  ( gui->sce );
        } else {
            view->engine_flags |= NOLIGHTING;

            g3dscene_turnLightsOff ( gui->sce );
        }
    }

    gtk_widget_queue_draw ( ggt->curogl );

    gtk_view_updateMenu ( gvw );
}

/******************************************************************************/
void g3duiview_toggleGridCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkView *gvw = ( GtkView * ) user_data;
    G3DUIVIEW *view = &gvw->view;
    G3DUI *gui = view->gui;
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( view->engine_flags & HIDEGRID ) {
        view->engine_flags &= (~HIDEGRID);
    } else {
        view->engine_flags |= HIDEGRID;
    }

    gtk_widget_queue_draw ( ggt->curogl );

    gtk_view_updateMenu ( gvw );
}

/******************************************************************************/
void g3duiview_toggleTexturesCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkView *gvw = ( GtkView * ) user_data;
    G3DUIVIEW *view = &gvw->view;
    G3DUI *gui = view->gui;
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( view->engine_flags & NOTEXTURE ) {
        view->engine_flags &= (~NOTEXTURE);
    } else {
        view->engine_flags |= NOTEXTURE;
    }

    gtk_widget_queue_draw ( ggt->curogl );

    gtk_view_updateMenu ( gvw );
}

/******************************************************************************/
void g3duiview_toggleBackgroundImageCbk ( GtkWidget *widget, 
                                          gpointer user_data ) {
    GtkView *gvw = ( GtkView * ) user_data;
    G3DUIVIEW *view = &gvw->view;
    G3DUI *gui = view->gui;
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    if ( view->engine_flags & NOBACKGROUNDIMAGE ) {
        view->engine_flags &= (~NOBACKGROUNDIMAGE);
    } else {
        view->engine_flags |= NOBACKGROUNDIMAGE;
    }

    gtk_widget_queue_draw ( ggt->curogl );

    gtk_view_updateMenu ( gvw );
}

/******************************************************************************/
void g3duiview_resetCameraCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkView *gvw = ( GtkView * ) user_data;
    G3DUIVIEW *view = &gvw->view;
    G3DUI *gui = view->gui;
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;
    G3DOBJECT *objcam = ( G3DOBJECT * ) view->cam;

    memcpy ( &objcam->pos, &view->defcampos, sizeof ( G3DVECTOR ) );
    memcpy ( &objcam->rot, &view->defcamrot, sizeof ( G3DVECTOR ) );
    memcpy ( &objcam->sca, &view->defcamsca, sizeof ( G3DVECTOR ) );

    view->cam->focal = view->defcamfoc;


    g3dobject_updateMatrix_r ( objcam, gui->engine_flags );

    gtk_widget_queue_draw ( ggt->curogl );
}

/******************************************************************************/
static G3DUIMENU view_menu_defcam   = { VIEWMENU_DEFAULTCAMERA,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        g3duiview_useDefaultCameraCbk };

static G3DUIMENU view_menu_selcam   = { VIEWMENU_SELECTEDCAMERA,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        g3duiview_useSelectedCameraCbk };

static G3DUIMENU view_menu_normals  = { VIEWMENU_NORMALS,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        g3duiview_toggleNormalsCbk };

static G3DUIMENU view_menu_bones    = { VIEWMENU_BONES,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        g3duiview_toggleBonesCbk };

static G3DUIMENU view_menu_grid     = { VIEWMENU_GRID,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        g3duiview_toggleGridCbk };

static G3DUIMENU view_menu_textures = { VIEWMENU_TEXTURES,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        g3duiview_toggleTexturesCbk };

static G3DUIMENU view_menu_bg       = { VIEWMENU_BACKGROUND,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        g3duiview_toggleBackgroundImageCbk };

static G3DUIMENU view_menu_lighting = { VIEWMENU_LIGHTING,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        g3duiview_toggleLightingCbk };

static G3DUIMENU view_menu_reset    = { VIEWMENU_RESET,
                                        G3DUIMENUTYPE_PUSHBUTTON,
                                        NULL,
                                        g3duiview_resetCameraCbk };

/******************************************************************************/
static G3DUIMENU view_menu = { "_Options",
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
static G3DUIMENU viewrootnode = { OPTIONMENUNAME,
                                  G3DUIMENUTYPE_MENUBAR,
                                  NULL,
                                  NULL,
                                 .nodes = { &view_menu,
                                             NULL } };

/******************************************************************************/
/******************************************************************************/
GtkWidget *createOptionMenu ( GtkWidget *parent,
                              GtkWidget *gvw,
                              char      *name,
                              gint       x,
                              gint       y,
                              gint       width,
                              gint       height ) {
    GdkRectangle gdkrec = { x, y, width, height };

    parseMenu_r ( &viewrootnode, NULL, gvw );

    gtk_widget_size_allocate ( viewrootnode.menu, &gdkrec );
    gtk_fixed_put ( GTK_FIXED(parent), viewrootnode.menu, x, y );

    gtk_widget_show ( viewrootnode.menu );


    return viewrootnode.menu;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
static void setMaterialCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_setMaterialCbk ( gui );
}

/******************************************************************************/
static void removeMaterialCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    LIST *ltmpmatlist = gui->lmatlist;

    if ( gui->selmat ) {
        while ( ltmpmatlist ) {
            GtkWidget *matlst = ( GtkWidget * ) ltmpmatlist->data;

            g3duimateriallist_removeMaterial ( matlst, gui->sce, 
                                                       gui->urm,
                                                       gui->selmat );

            gui->selmat = NULL;


            ltmpmatlist = ltmpmatlist->next;
        }
    }

    g3dui_redrawMaterialList ( gui );
    g3dui_updateMaterialEdit ( gui );
    g3dui_redrawGLViews      ( gui );
}

/******************************************************************************/
static void addMaterialCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DMATERIAL *mat = g3dmaterial_new ( "Material" );
    G3DUI *gui = ( G3DUI * ) user_data;
    LIST *ltmpmatlist = gui->lmatlist;

    g3durm_scene_addMaterial ( gui->urm,
                               gui->sce, 
                               mat,
                               gui->engine_flags,
                               REBUILDMATERIALLIST );

    while ( ltmpmatlist ) {
        GtkWidget *matlst = ( GtkWidget * ) ltmpmatlist->data;

        g3duimateriallist_addPreview ( matlst, mat );

        ltmpmatlist = ltmpmatlist->next;
    }

    gui->selmat = mat;

    g3dui_redrawMaterialList ( gui );
    g3dui_updateMaterialEdit ( gui );
}

/******************************************************************************/
static G3DUIMENU matfile_menu_add    = { "Add Material",
                                         G3DUIMENUTYPE_PUSHBUTTON,
                                         NULL,
                                         addMaterialCbk };

static G3DUIMENU matfile_menu_remove = { "Remove Material",
                                         G3DUIMENUTYPE_PUSHBUTTON,
                                         NULL,
                                         removeMaterialCbk };

static G3DUIMENU matfile_menu_set    = { "Set Material",
                                         G3DUIMENUTYPE_PUSHBUTTON,
                                         NULL,
                                         setMaterialCbk };

/******************************************************************************/
static G3DUIMENU matfile_menu = { "File",
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = { &matfile_menu_add,
                                         &matfile_menu_remove,
                                         &matfile_menu_set,
                                          NULL } };

/******************************************************************************/
/******************************************************************************/
static G3DUIMENU matrootnode = { "Bar",
                                 G3DUIMENUTYPE_MENUBAR,
                                 NULL,
                                 NULL,
                                .nodes = { &matfile_menu,
                                            NULL } };

/******************************************************************************/
GtkWidget *createMaterialMenuBar ( GtkWidget *parent, 
                                   G3DUI     *gui,
                                   char      *name,
                                   gint       x,
                                   gint       y,
                                   gint       width,
                                   gint       height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *bar = gtk_menu_bar_new ( );

    gtk_widget_set_name ( bar, name );

    /*gtk_widget_size_allocate ( bar, &gdkrec );*/
    gtk_widget_set_size_request ( bar, width, height );

    parseMenu_r ( &matrootnode, NULL, gui );

    gtk_widget_size_allocate ( matrootnode.menu, &gdkrec );
    gtk_fixed_put ( GTK_FIXED(parent), matrootnode.menu, x, y );

    gtk_widget_show ( bar );


    return bar;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void g3dui_addVibratorCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addVibratorCbk ( gui );
}

/******************************************************************************/
void g3dui_addTrackerCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addTrackerCbk ( gui );
}


/******************************************************************************/
void g3dui_removeSelectedTagCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_removeSelectedTagCbk ( gui );
}

/******************************************************************************/
/****************************** Tags MENU **********************************/
static G3DUIMENU tags_menu_addVibrator     = { MENU_ADDVIBRATOR,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectMode_objectSelected,
                                               g3dui_addVibratorCbk };

static G3DUIMENU tags_menu_addTracker      = { MENU_ADDTRACKER,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectMode_objectSelected,
                                               g3dui_addTrackerCbk };

static G3DUIMENU tags_menu_removeSelTag    = { MENU_REMOVESELTAG,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectMode_objectSelected,
                                               g3dui_removeSelectedTagCbk };

/******************************************************************************/
static G3DUIMENU tags_menu = { "Tags",
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = { /*&tags_menu_addVibrator,*/
                                         &tags_menu_addTracker,
                                         &tags_menu_removeSelTag,
                                          NULL } };

/******************************************************************************/
/******************************************************************************/
static G3DUIMENU objrootnode = { "Bar",
                                 G3DUIMENUTYPE_MENUBAR,
                                 NULL,
                                 NULL,
                                .nodes = { &tags_menu,
                                            NULL } };

/******************************************************************************/
GtkWidget *createObjectsMenuBar ( GtkWidget *parent, 
                                  G3DUI     *gui,
                                  char      *name,
                                  gint       x,
                                  gint       y,
                                  gint       width,
                                  gint       height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *bar = gtk_menu_bar_new ( );

    gtk_widget_set_name ( bar, name );

    /*gtk_widget_size_allocate ( bar, &gdkrec );*/
    gtk_widget_set_size_request ( bar, width, height );

    parseMenu_r ( &objrootnode, NULL, gui );

    gtk_widget_size_allocate ( objrootnode.menu, &gdkrec );
    gtk_fixed_put ( GTK_FIXED(parent), objrootnode.menu, x, y );

    gtk_widget_show ( bar );


    return bar;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void g3dui_aboutCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    gchar *authors[] = { "Gary GABRIEL", NULL };

    gtk_show_about_dialog ( NULL,
                           "authors", authors,
                           "program-name", "Gloss3D",
                           "license", GLOSS3DLICENSE,
                           "comments", "Graphics by a Lightweight Open-Source Software",
                           "version", VERSION,
                           "copyright", "2012-2021 The Gloss3D Team",
                           "website", "http://www.gloss3d.net",
                           "title", ("About Gloss3D"),
                           NULL );
}

/******************************************************************************/
static G3DUIMENU uvhelp_menu_about = { MENU_ABOUT,
                                       G3DUIMENUTYPE_PUSHBUTTON,
                                       NULL,
                                       g3dui_aboutCbk };

/******************************************************************************/
static G3DUIMENU uvhelp_menu = { "Help",
                                  G3DUIMENUTYPE_SUBMENU,
                                  NULL,
                                  NULL,
                                 .nodes = { &uvhelp_menu_about,
                                             NULL } };

/******************************************************************************/
/******************************************************************************/
void l3dui_resizeChannelImageCbk ( GtkWidget *widget, 
                                              gpointer   user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;
    G3DCHANNEL *chn = common_l3dui_getWorkingChannel ( lui );

    if ( chn ) {
        GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );

        createChannelImage ( dial, 
                             lui, 
                             chn,
                             0x01,
                             "Channel Image", 0, 0, 200, 96 );

        gtk_widget_show ( dial );
    }
}

/******************************************************************************/
void l3dui_fgfillCbk ( GtkWidget *widget, gpointer user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;
    L3DSYSINFO *sysinfo = l3dsysinfo_get ( );

    common_l3dui_fillWithColor ( lui, sysinfo->fgcolor );
}

/******************************************************************************/
void l3dui_bgfillCbk ( GtkWidget *widget, gpointer user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;
    L3DSYSINFO *sysinfo = l3dsysinfo_get ( );

    common_l3dui_fillWithColor ( lui, sysinfo->bgcolor );
}

/******************************************************************************/
static G3DUIMENU uvimage_menu_resize = { MENU_RESIZEIMAGE,
                                         G3DUIMENUTYPE_PUSHBUTTON,
                                         NULL,
                                         l3dui_resizeChannelImageCbk };

static G3DUIMENU uvimage_menu_fgfill = { MENU_FGFILL,
                                         G3DUIMENUTYPE_PUSHBUTTON,
                                         NULL,
                                         l3dui_fgfillCbk };

static G3DUIMENU uvimage_menu_bgfill = { MENU_BGFILL,
                                         G3DUIMENUTYPE_PUSHBUTTON,
                                         NULL,
                                         l3dui_bgfillCbk };

/******************************************************************************/
static G3DUIMENU uvimage_menu = { "Image",
                                  G3DUIMENUTYPE_SUBMENU,
                                  NULL,
                                  NULL,
                                 .nodes = { &uvimage_menu_resize,
                                            &uvimage_menu_fgfill,
                                            &uvimage_menu_bgfill,
                                             NULL } };

/******************************************************************************/
/******************************************************************************/
void l3dui_fac2uvsetCbk ( GtkWidget *widget, gpointer user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;

    common_l3dui_fac2uvsetCbk ( lui );

    g3dui_redrawUVMapEditors ( lui->gui );
}

/******************************************************************************/
void l3dui_uvset2facCbk ( GtkWidget *widget, gpointer user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;

    common_l3dui_uvset2facCbk ( lui );

    g3dui_redrawGLViews ( lui->gui );
}

/******************************************************************************/
void l3dui_ver2uvCbk ( GtkWidget *widget, gpointer user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;

    common_l3dui_ver2uvCbk ( lui );

    g3dui_redrawUVMapEditors ( lui->gui );
}

/******************************************************************************/
void l3dui_uv2verCbk ( GtkWidget *widget, gpointer user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;

    common_l3dui_uv2verCbk ( lui );

    g3dui_redrawGLViews ( lui->gui );
}

/******************************************************************************/
static G3DUIMENU uvedit_menu_undo      = { MENU_UNDO,
                                           G3DUIMENUTYPE_PUSHBUTTON,
                                           NULL,
                                           l3dui_undoCbk };

static G3DUIMENU uvedit_menu_redo      = { MENU_REDO,
                                           G3DUIMENUTYPE_PUSHBUTTON,
                                           NULL,
                                           l3dui_redoCbk };

static G3DUIMENU uvedit_menu_uv2ver    = { MENU_VERTEXFROMUV,
                                           G3DUIMENUTYPE_PUSHBUTTON,
                                           NULL,
                                           l3dui_uv2verCbk };

static G3DUIMENU uvedit_menu_ver2uv    = { MENU_UVFROMVERTEX,
                                           G3DUIMENUTYPE_PUSHBUTTON,
                                           NULL,
                                           l3dui_ver2uvCbk };

static G3DUIMENU uvedit_menu_uvset2fac = { MENU_FACEFROMUVSET,
                                           G3DUIMENUTYPE_PUSHBUTTON,
                                           NULL,
                                           l3dui_uvset2facCbk };

static G3DUIMENU uvedit_menu_fac2uvset = { MENU_UVSETFROMFACE,
                                           G3DUIMENUTYPE_PUSHBUTTON,
                                           NULL,
                                           l3dui_fac2uvsetCbk };

/******************************************************************************/
static G3DUIMENU uvedit_menu = { "Edit",
                                 G3DUIMENUTYPE_SUBMENU,
                                 NULL,
                                 NULL,
                                .nodes = { &uvedit_menu_undo,
                                           &uvedit_menu_redo,
                                           &uvedit_menu_uv2ver,
                                           &uvedit_menu_ver2uv,
                                           &uvedit_menu_uvset2fac,
                                           &uvedit_menu_fac2uvset,
                                            NULL } };

/******************************************************************************/
void l3dui_loadImageByChannelIDCbk ( GtkWidget *widget, 
                                     gpointer   user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;
    G3DUI *gui = ( G3DUI * ) lui->gui;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( gui->sce );

    if ( obj ) {
        if ( obj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DUVMAP *uvmap = g3dmesh_getSelectedUVMap ( mes );
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            /*** try the first texture in case no texture is selected ***/
            if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

            if ( tex ) {
                G3DMATERIAL *mat = tex->mat;
                uint32_t chnID = GETCHANNEL(lui->engine_flags);
                G3DCHANNEL  *chn = g3dmaterial_getChannelByID ( mat, chnID );

                g3dui_loadImageForChannel ( gui, chn );

                /*** resize selection mask and zbuffer ***/
                common_l3dui_resizeBuffers ( lui );
            }
        }
    }
}

/******************************************************************************/
void l3dui_createChannelImageCbk ( GtkWidget *widget, 
                                   gpointer   user_data ) {
    L3DUI *lui = ( L3DUI * ) user_data;
    G3DCHANNEL *chn = common_l3dui_getWorkingChannel ( lui );

    if ( chn ) {
        GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );

        createChannelImage ( dial, 
                             lui, 
                             chn,
                             0x00,
                             "Channel Image", 0, 0, 200, 96 );

        gtk_widget_show ( dial );
    }
}

/******************************************************************************/
static G3DUIMENU uvfile_menu_createChannelImage = { MENU_CREATECHANNELIMAGE,
                                                    G3DUIMENUTYPE_PUSHBUTTON,
                                                    NULL,
                                                    l3dui_createChannelImageCbk };

static G3DUIMENU uvfile_menu_openChannelImage   = { MENU_OPENCHANNELIMAGE,
                                                    G3DUIMENUTYPE_PUSHBUTTON,
                                                    NULL,
                                                   l3dui_loadImageByChannelIDCbk };

/******************************************************************************/
static G3DUIMENU uvfile_menu = { "File",
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = { &uvfile_menu_createChannelImage,
                                         &uvfile_menu_openChannelImage,
                                          NULL } };

/******************************************************************************/
/******************************************************************************/
static G3DUIMENU uvrootnode = { "Bar",
                                G3DUIMENUTYPE_MENUBAR,
                                NULL,
                                NULL,
                               .nodes = { &uvfile_menu,
                                          &uvedit_menu,
                                          &uvimage_menu,
                                          &uvhelp_menu,
                                           NULL } };

/******************************************************************************/
GtkWidget *createUVMenuBar ( GtkWidget *parent,  
                             L3DUI      *lui,
                             char      *name,
                             gint       x,
                             gint       y,
                             gint       width,
                             gint       height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *bar = gtk_menu_bar_new ( );

    gtk_widget_set_name ( bar, name );

    gtk_widget_size_allocate ( bar, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), bar, x, y );

    parseMenu_r ( &uvrootnode, NULL, lui );

    gtk_widget_size_allocate ( uvrootnode.menu, &gdkrec );
    gtk_fixed_put ( GTK_FIXED(parent), uvrootnode.menu, x, y );

    /*createUVHelpMenu ( bar, gui, "HelpMenu" , 90 );*/

    gtk_widget_show ( bar );


    return bar;
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
static void addUVMapCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duimenubar_addUVMapCbk ( gui );
}

/******************************************************************************/
static void fitUVMapCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duimenubar_fitUVMapCbk ( gui );
}

/******************************************************************************/
static void alignUVMapCbk ( GtkWidget *widget, gpointer user_data ) {
    const gchar *option = gtk_widget_get_name  ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duimenubar_alignUVMapCbk ( gui, option );
}



/******************************************************************************/
static void renderSettingsCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );

    /*** For some reason, GtkSpinButtons calls its callbacks ***/
    /*** when being realized. With this trick I bypass that. ***/
    /*** Callbacks will return prematurely if gui->lock == 0x01 ***/
    gui->lock = 0x01;

    createRenderEdit ( dial, gui, "RENDEREDIT", 0, 0, 480, 340 );

    gtk_widget_show ( dial );

    gui->lock = 0x00;
}

/******************************************************************************/
void g3dui_mergeMeshCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_mergeMeshCbk ( gui );
}

/******************************************************************************/
void g3dui_splitMeshCbk ( GtkWidget *widget, gpointer user_data ) {
    const gchar *option = gtk_widget_get_name  ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_splitMeshCbk ( gui, option );
}

/******************************************************************************/
void g3dui_mirrorWeightGroupCbk ( GtkWidget *widget, gpointer user_data ) {
    const gchar *option = gtk_widget_get_name  ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_mirrorWeightGroupCbk ( gui, option );
}

/******************************************************************************/
void g3dui_resetBoneTreeCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_resetBoneTreeCbk ( gui );
}

/******************************************************************************/
void g3dui_resetBoneCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_resetBoneCbk ( gui );
}

/******************************************************************************/
void g3dui_fixBoneTreeCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_fixBoneTreeCbk ( gui );
}

/******************************************************************************/
void g3dui_fixBoneCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_fixBoneCbk ( gui );
}

/******************************************************************************/
void g3dui_addSymmetryCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addSymmetryCbk ( gui );
}

/******************************************************************************/
void g3dui_addInstanceCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addInstanceCbk ( gui );
}

/******************************************************************************/
void g3dui_addWireframeCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addWireframeCbk ( gui );
}

/******************************************************************************/
void g3dui_addSubdividerCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addSubdividerCbk ( gui );
}

/******************************************************************************/
void g3dui_addSplineRevolverCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addSplineRevolverCbk ( gui );
}

/******************************************************************************/
void g3dui_addFFDBoxCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addFFDBoxCbk ( gui );
}

/******************************************************************************/
void g3dui_addMorpherCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addMorpherCbk ( gui );
}

/******************************************************************************/
void g3dui_addSkinCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addSkinCbk ( gui );
}

/******************************************************************************/
void g3dui_addBoneCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addBoneCbk ( gui );
}

/******************************************************************************/
void g3dui_addPlaneCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addPlaneCbk ( gui );
}

/******************************************************************************/
void g3dui_addCubeCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addCubeCbk ( gui );
}

/******************************************************************************/
void g3dui_addSphereCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addSphereCbk ( gui );
}

/******************************************************************************/
void g3dui_addTorusCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addTorusCbk ( gui );
}

/******************************************************************************/
void g3dui_addCylinderCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addCylinderCbk ( gui );
}

/******************************************************************************/
void g3dui_addTubeCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addTubeCbk ( gui );
}

/******************************************************************************/
void g3dui_addLightCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addLightCbk ( gui );
}

/******************************************************************************/
void g3dui_addCameraCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addCameraCbk ( gui, g3dui_getCurrentViewCamera ( gui ) );
}

/******************************************************************************/
void g3dui_addEmptyMeshCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addEmptyMeshCbk ( gui );
}

/******************************************************************************/
void g3dui_addSplineCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addSplineCbk ( gui );
}

/******************************************************************************/
void g3dui_addTextCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addTextCbk ( gui );
}

/******************************************************************************/
void g3dui_addNullCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_addNullCbk ( gui );
}

/******************************************************************************/
void g3dui_invertNormalCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_invertNormalCbk ( gui );
}

/******************************************************************************/
void g3dui_alignNormalsCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_alignNormalsCbk ( gui );
}

/******************************************************************************/
void g3dui_triangulateCbk ( GtkWidget *widget, gpointer user_data ) {
    const gchar *option = gtk_widget_get_name  ( widget );
    /*const gchar *option = gtk_menu_item_get_label  ( GTK_MENU_ITEM(widget) );*/
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_triangulateCbk ( gui, option );
}

/******************************************************************************/
void g3dui_untriangulateCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_untriangulateCbk ( gui );
}

/******************************************************************************/
void g3dui_weldVerticesCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_weldVerticesCbk ( gui );
}

/******************************************************************************/
void g3dui_deleteLoneVerticesCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_deleteLoneVerticesCbk ( gui );
}

/******************************************************************************/
void g3dui_selectAllCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_selectAllCbk ( gui );
}

/******************************************************************************/
void g3dui_invertSelectionCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_invertSelectionCbk ( gui );
}

/******************************************************************************/
void g3dui_getObjectStatsCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    char buffer[0x200] = { 0 };
    GtkWidget *dialog;

    common_g3dui_getObjectStatsCbk ( gui, buffer, 0x200 );

    if ( strlen ( buffer ) ) {
        dialog = gtk_message_dialog_new ( NULL,
                                          GTK_DIALOG_MODAL,
                                          GTK_MESSAGE_INFO,
                                          GTK_BUTTONS_NONE,
                                          buffer,
                                          NULL );

        gtk_dialog_run ( GTK_DIALOG ( dialog ) );
        gtk_widget_destroy ( dialog );
    }
}

/******************************************************************************/
void g3dui_selectTreeCbk ( GtkWidget *widget, gpointer user_data ) {
    /*const gchar *option = gtk_menu_item_get_label  ( widget );*/
    const gchar *option = gtk_widget_get_name  ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_selectTreeCbk ( gui, option );
}

/******************************************************************************/
gboolean g3dui_exitEventCbk ( GtkWidget *widget, 
                              GdkEvent  *event, 
                              gpointer   user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    GtkWidget *dialog;
    gint       res;

    if ( g3dui_saveAlteredImages ( gui ) ) {
        return TRUE;
    }

    dialog = gtk_message_dialog_new ( NULL,
                                      GTK_DIALOG_MODAL,
                                      GTK_MESSAGE_QUESTION,
                                      GTK_BUTTONS_YES_NO,
                                      "Leave Gloss3D ?" );


    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_YES ) {
        common_g3dui_exitokcbk ( gui );

        return FALSE;
    }

    gtk_widget_destroy ( dialog );

    return TRUE;
}

/******************************************************************************/
void g3dui_exitcbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_message_dialog_new ( NULL,
                                      GTK_DIALOG_MODAL,
                                      GTK_MESSAGE_QUESTION,
                                      GTK_BUTTONS_YES_NO,
                                      "Leave Gloss3D ?" );


    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_YES ) {
        common_g3dui_exitokcbk ( gui );
    }

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
void g3dui_exportfilecbk ( GtkWidget *widget, gpointer user_data ) {
    const gchar *filedesc = gtk_widget_get_name  ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Export file ...",
                                           GTK_WINDOW(ggt->top),
                        /*** from ristretto-0.3.5/src/main_window.c ***/
                                           GTK_FILE_CHOOSER_ACTION_SAVE,
                                           "_Cancel", 
                                           GTK_RESPONSE_CANCEL,
                                           "_Open", 
                                           GTK_RESPONSE_OK,
                                           NULL );

    gtk_file_chooser_set_do_overwrite_confirmation ( GTK_FILE_CHOOSER(dialog),
                                                     TRUE );

    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_OK ) {
        GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
        const char *filename = gtk_file_chooser_get_filename ( chooser );

        common_g3dui_exportfileokcbk ( gui, filedesc, filename );

        g_free    ( ( gpointer ) filename );
    }

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
void g3dui_importfilecbk ( GtkWidget *widget, gpointer user_data ) {
    const gchar *filedesc = gtk_widget_get_name  ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Import file ...",
                                           GTK_WINDOW(ggt->top),
                        /*** from ristretto-0.3.5/src/main_window.c ***/
                                           GTK_FILE_CHOOSER_ACTION_OPEN,
                                           "_Cancel", 
                                           GTK_RESPONSE_CANCEL,
                                           "_Open", 
                                           GTK_RESPONSE_OK,
                                           NULL );


    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_OK ) {
        GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
        const char *filename = gtk_file_chooser_get_filename ( chooser );

        common_g3dui_importfileokcbk ( gui, filedesc, filename );

        g_free    ( ( gpointer ) filename );
    }

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
static G3DUIMENU menu_separator = { "SEPARATOR",
                                    G3DUIMENUTYPE_SEPARATOR,
                                    NULL,
                                    NULL };

/******************************************************************************/
/********************************* Help MENU **********************************/
static G3DUIMENU help_menu_about = { MENU_ABOUT,
                                     G3DUIMENUTYPE_PUSHBUTTON,
                                     NULL,
                                     g3dui_aboutCbk };

/******************************************************************************/
static G3DUIMENU help_menu = { "Help",
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = { &help_menu_about,
                                          NULL } };

/******************************************************************************/
/******************************* UVMapping MENU *******************************/
static G3DUIMENU uvmapping_menu_align_xy = { MENU_ALIGNUVMAPXY,
                                             G3DUIMENUTYPE_PUSHBUTTON,
                                             NULL,
                                             alignUVMapCbk };

static G3DUIMENU uvmapping_menu_align_yz = { MENU_ALIGNUVMAPYZ,
                                             G3DUIMENUTYPE_PUSHBUTTON,
                                             NULL,
                                             alignUVMapCbk };

static G3DUIMENU uvmapping_menu_align_zx = { MENU_ALIGNUVMAPZX,
                                             G3DUIMENUTYPE_PUSHBUTTON,
                                             NULL,
                                             alignUVMapCbk };

/******************************************************************************/
static G3DUIMENU uvmapping_menu_add   = { MENU_ADDUVMAP,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          addUVMapCbk };

static G3DUIMENU uvmapping_menu_fit   = { MENU_FITUVMAP,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          fitUVMapCbk };

static G3DUIMENU uvmapping_menu_align = { MENU_ALIGNUVMAP,
                                          G3DUIMENUTYPE_SUBMENU,
                                          NULL,
                                          NULL,
                                         .nodes = { &uvmapping_menu_align_xy,
                                                    &uvmapping_menu_align_yz,
                                                    &uvmapping_menu_align_zx,
                                                     NULL } };

/******************************************************************************/
static G3DUIMENU uvmapping_menu = { "UV Mapping",
                                    G3DUIMENUTYPE_SUBMENU,
                                    NULL,
                                    NULL,
                                   .nodes = { &uvmapping_menu_add,
                                              &uvmapping_menu_fit,
                                              &uvmapping_menu_align,
                                               NULL } };

/******************************************************************************/
/******************************* Render MENU **********************************/
static G3DUIMENU render_menu_view     = { MENU_RENDERVIEW,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          g3dui_renderViewCbk };

static G3DUIMENU render_menu_settings = { MENU_RENDERSETTINGS,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          renderSettingsCbk };

/******************************************************************************/
static G3DUIMENU render_menu = { "Render",
                                 G3DUIMENUTYPE_SUBMENU,
                                 NULL,
                                 NULL,
                                .nodes = { &render_menu_view,
                                           &render_menu_settings,
                                            NULL } };

/******************************************************************************/
/****************************** Functions MENU ********************************/
static G3DUIMENU functions_menu_mirrorWeightgroup_xy = { MENU_MIRRORXY,
                                                         G3DUIMENUTYPE_PUSHBUTTON,
                                                         skinModeOnly,
                                                         g3dui_mirrorWeightGroupCbk };

static G3DUIMENU functions_menu_mirrorWeightgroup_yz = { MENU_MIRRORYZ,
                                                         G3DUIMENUTYPE_PUSHBUTTON,
                                                         skinModeOnly,
                                                         g3dui_mirrorWeightGroupCbk };

static G3DUIMENU functions_menu_mirrorWeightgroup_zx = { MENU_MIRRORZX,
                                                         G3DUIMENUTYPE_PUSHBUTTON,
                                                         skinModeOnly,
                                                         g3dui_mirrorWeightGroupCbk };

/******************************************************************************/
static G3DUIMENU functions_menu_splitMesh_keep   = { MENU_SPLITANDKEEP,
                                                     G3DUIMENUTYPE_PUSHBUTTON,
                                                     faceModeOnly,
                                                     g3dui_splitMeshCbk };

static G3DUIMENU functions_menu_splitMesh_remove = { MENU_SPLITANDREMOVE,
                                                     G3DUIMENUTYPE_PUSHBUTTON,
                                                     faceModeOnly,
                                                     g3dui_splitMeshCbk };

/******************************************************************************/
static G3DUIMENU functions_menu_mirrorWG     = { MENU_MIRRORWEIGHTGROUP,
                                                 G3DUIMENUTYPE_SUBMENU,
                                                 skinModeOnly,
                                                 NULL,
                                                .nodes = { &functions_menu_mirrorWeightgroup_xy,
                                                           &functions_menu_mirrorWeightgroup_yz,
                                                           &functions_menu_mirrorWeightgroup_zx,
                                                            NULL } };

static G3DUIMENU functions_menu_splitMesh    = { MENU_SPLITMESH,
                                                 G3DUIMENUTYPE_SUBMENU,
                                                 faceModeOnly,
                                                 NULL,
                                                .nodes = { &functions_menu_splitMesh_keep,
                                                           &functions_menu_splitMesh_remove,
                                                            NULL } };

static G3DUIMENU functions_menu_mergeMesh    = { MENU_MERGEMESH,
                                                 G3DUIMENUTYPE_PUSHBUTTON,
                                                 objectModeOnly,
                                                 g3dui_mergeMeshCbk };

static G3DUIMENU functions_menu_makeEditable = { MENU_MAKEEDITABLE,
                                                 G3DUIMENUTYPE_PUSHBUTTON,
                                                 objectModeOnly,
                                                 g3dui_makeEditableCbk };

/******************************************************************************/
static G3DUIMENU functions_menu = { "Functions",
                                    G3DUIMENUTYPE_SUBMENU,
                                    NULL,
                                    NULL,
                                   .nodes = { &functions_menu_mirrorWG,
                                              &functions_menu_splitMesh,
                                              &functions_menu_mergeMesh,
                                              &functions_menu_makeEditable,
                                               NULL } };


/******************************************************************************/
/***************************** Multipliers MENU *******************************/
static G3DUIMENU multipliers_menu_addInstance = { MENU_ADDINSTANCE,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  g3dui_addInstanceCbk };

static G3DUIMENU multipliers_menu_addSymmetry = { MENU_ADDSYMMETRY,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  g3dui_addSymmetryCbk };

/******************************************************************************/
static G3DUIMENU multipliers_menu = { "Multipliers",
                                      G3DUIMENUTYPE_SUBMENU,
                                      NULL,
                                      NULL,
                                     .nodes = { &multipliers_menu_addInstance,
                                                &multipliers_menu_addSymmetry,
                                                 NULL } };

/******************************************************************************/
/****************************** Modifiers MENU ********************************/
static G3DUIMENU modifiers_menu_resetBone_tree = { MENU_RESETBONETREE,
                                                   G3DUIMENUTYPE_PUSHBUTTON,
                                                   objectMode_boneSelected,
                                                   g3dui_resetBoneTreeCbk };

static G3DUIMENU modifiers_menu_resetBone_only = { MENU_RESETBONEONLY,
                                                   G3DUIMENUTYPE_PUSHBUTTON,
                                                   objectMode_boneSelected,
                                                   g3dui_resetBoneCbk };
/******************************************************************************/
static G3DUIMENU modifiers_menu_fixBone_tree = { MENU_FIXBONETREE,
                                                 G3DUIMENUTYPE_PUSHBUTTON,
                                                 objectMode_boneSelected,
                                                 g3dui_fixBoneTreeCbk };

static G3DUIMENU modifiers_menu_fixBone_only = { MENU_FIXBONEONLY,
                                                 G3DUIMENUTYPE_PUSHBUTTON,
                                                 objectMode_boneSelected,
                                                 g3dui_fixBoneCbk };

/******************************************************************************/
static G3DUIMENU modifiers_menu_addWireframe  = { MENU_ADDWIREFRAME,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  g3dui_addWireframeCbk };

static G3DUIMENU modifiers_menu_addSubdivider = { MENU_ADDSUBDIVIDER,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  g3dui_addSubdividerCbk };

static G3DUIMENU modifiers_menu_addFFDBox     = { MENU_ADDFFDBOX,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  g3dui_addFFDBoxCbk };

static G3DUIMENU modifiers_menu_addMorpher    = { MENU_ADDMORPHER,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  g3dui_addMorpherCbk };

static G3DUIMENU modifiers_menu_addSkin       = { MENU_ADDSKIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  g3dui_addSkinCbk };

static G3DUIMENU modifiers_menu_addBone       = { MENU_ADDBONE,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectMode_boneOrSkinSelected,
                                                  g3dui_addBoneCbk };

static G3DUIMENU modifiers_menu_fixBone       = { MENU_FIXBONE,
                                                  G3DUIMENUTYPE_SUBMENU,
                                                  objectMode_boneSelected,
                                                  NULL,
                                                 .nodes = { &modifiers_menu_fixBone_tree,
                                                            &modifiers_menu_fixBone_only,
                                                             NULL } };

static G3DUIMENU modifiers_menu_resetBone     = { MENU_RESETBONE,
                                                  G3DUIMENUTYPE_SUBMENU,
                                                  objectMode_boneSelected,
                                                  NULL,
                                                 .nodes = { &modifiers_menu_resetBone_tree,
                                                            &modifiers_menu_resetBone_only,
                                                             NULL } };

static G3DUIMENU modifiers_menu_addSRevolver  = { MENU_ADDSPLINEREVOLVER,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  g3dui_addSplineRevolverCbk };

/******************************************************************************/
static G3DUIMENU modifiers_menu = { "Modifiers",
                                     G3DUIMENUTYPE_SUBMENU,
                                     NULL,
                                     NULL,
                                    .nodes = { &modifiers_menu_addWireframe,
                                               &modifiers_menu_addSubdivider,
                                               &modifiers_menu_addFFDBox,
                                               &modifiers_menu_addMorpher,
                                               &modifiers_menu_addSkin,
                                               &modifiers_menu_addSRevolver,
                                               &menu_separator,
                                               &modifiers_menu_addBone,
                                               &modifiers_menu_fixBone,
                                               &modifiers_menu_resetBone,
                                                NULL } };

/******************************************************************************/
/****************************** Objects MENU **********************************/
static G3DUIMENU objects_menu_addPlane     = { MENU_ADDPLANE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               g3dui_addPlaneCbk };

static G3DUIMENU objects_menu_addCube      = { MENU_ADDCUBE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               g3dui_addCubeCbk };

static G3DUIMENU objects_menu_addSphere    = { MENU_ADDSPHERE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               g3dui_addSphereCbk };

static G3DUIMENU objects_menu_addTorus     = { MENU_ADDTORUS,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               g3dui_addTorusCbk };

static G3DUIMENU objects_menu_addCylinder  = { MENU_ADDCYLINDER,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               g3dui_addCylinderCbk };

static G3DUIMENU objects_menu_addTube      = { MENU_ADDTUBE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               g3dui_addTubeCbk };

static G3DUIMENU objects_menu_addText      = { MENU_ADDTEXT,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               g3dui_addTextCbk };

static G3DUIMENU objects_menu_addNull      = { MENU_ADDNULL,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               g3dui_addNullCbk };

static G3DUIMENU objects_menu_addLight     = { MENU_ADDLIGHT,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               g3dui_addLightCbk };

static G3DUIMENU objects_menu_addCamera    = { MENU_ADDCAMERA,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               g3dui_addCameraCbk };

static G3DUIMENU objects_menu_addEmptyMesh = { MENU_ADDEMPTYMESH,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               g3dui_addEmptyMeshCbk };

static G3DUIMENU objects_menu_addSpline    = { MENU_ADDSPLINE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               g3dui_addSplineCbk };

/******************************************************************************/
static G3DUIMENU objects_menu = { "Objects",
                                  G3DUIMENUTYPE_SUBMENU,
                                  NULL,
                                  NULL,
                                 .nodes = { &objects_menu_addPlane,
                                            &objects_menu_addCube,
                                            &objects_menu_addSphere,
                                            &objects_menu_addTorus,
                                            &objects_menu_addCylinder,
                                            &objects_menu_addTube,
                                            &objects_menu_addText,
                                            &objects_menu_addNull,
                                            &menu_separator,
                                            &objects_menu_addLight,
                                            &menu_separator,
                                            &objects_menu_addCamera,
                                            &menu_separator,
                                            &objects_menu_addEmptyMesh,
                                            &menu_separator,
                                            &objects_menu_addSpline,
                                             NULL } };

/******************************************************************************/
/****************************** Edit MENU *************************************/
static G3DUIMENU edit_menu_selectTree_all  = { MENU_OPTION_ALLTYPES,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               NULL,
                                               g3dui_selectTreeCbk };

static G3DUIMENU edit_menu_selectTree_same = { MENU_OPTION_SAMETYPE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               NULL,
                                               g3dui_selectTreeCbk };

/******************************************************************************/
static G3DUIMENU edit_menu_triangulate_cw  = { MENU_OPTION_CLOCKWISE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               NULL,
                                               g3dui_triangulateCbk };

static G3DUIMENU edit_menu_triangulate_ccw = { MENU_OPTION_ANTICLOCKWISE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               NULL,
                                               g3dui_triangulateCbk };

/******************************************************************************/
static G3DUIMENU edit_menu_undo               = { MENU_UNDO,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  NULL,
                                                  g3dui_undoCbk };

static G3DUIMENU edit_menu_redo               = { MENU_REDO,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  NULL,
                                                  g3dui_undoCbk };

static G3DUIMENU edit_menu_invertNormals      = { MENU_INVERTNORMALS,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  faceModeOnly,
                                                  g3dui_invertNormalCbk };

static G3DUIMENU edit_menu_alignNormals       = { MENU_ALIGNNORMALS,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  faceModeOnly,
                                                  g3dui_alignNormalsCbk };

static G3DUIMENU edit_menu_triangulate        = { MENU_TRIANGULATE,
                                                  G3DUIMENUTYPE_SUBMENU,
                                                  faceModeOnly,
                                                  NULL,
                                                 .nodes = { &edit_menu_triangulate_cw,
                                                            &edit_menu_triangulate_ccw,
                                                             NULL } };

static G3DUIMENU edit_menu_untriangulate      = { MENU_UNTRIANGULATE,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  faceModeOnly,
                                                  g3dui_untriangulateCbk };

static G3DUIMENU edit_menu_weldVertices       = { MENU_WELDVERTICES,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  vertexModeOnly,
                                                  g3dui_weldVerticesCbk };

static G3DUIMENU edit_menu_deleteLoneVertices = { MENU_DELETELONEVERTICES,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  vertexModeOnly,
                                                  g3dui_deleteLoneVerticesCbk };

static G3DUIMENU edit_menu_selectAll          = { MENU_SELECTALL,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  NULL,
                                                  g3dui_selectAllCbk };

static G3DUIMENU edit_menu_selectTree         = { MENU_SELECTTREE,
                                                  G3DUIMENUTYPE_SUBMENU,
                                                  objectModeOnly,
                                                  NULL,
                                                  .nodes = { &edit_menu_selectTree_all,
                                                             &edit_menu_selectTree_same,
                                                              NULL } };

static G3DUIMENU edit_menu_invertSelection    = { MENU_INVERTSELECTION,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  NULL,
                                                  g3dui_invertSelectionCbk };

static G3DUIMENU edit_menu_getObjectStats     = { MENU_GETOBJECTSTATS,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectMode_objectSelected,
                                                  g3dui_getObjectStatsCbk };

/******************************************************************************/
static G3DUIMENU edit_menu = { "Edit",
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = { &edit_menu_undo,
                                         &edit_menu_redo,
                                         &edit_menu_invertNormals,
                                         &edit_menu_alignNormals,
                                         &edit_menu_triangulate,
                                         &edit_menu_untriangulate,
                                         &edit_menu_weldVertices,
                                         &edit_menu_deleteLoneVertices,
                                         &edit_menu_selectAll,
                                         &edit_menu_selectTree,
                                         &edit_menu_invertSelection,
                                         &edit_menu_getObjectStats,
                                          NULL } };

/******************************************************************************/
/****************************** File MENU *************************************/
static G3DUIMENU file_menu_export_obj = { FILEDESC_OBJ,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          g3dui_exportfilecbk };

static G3DUIMENU file_menu_export_pov = { FILEDESC_POV,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          g3dui_exportfilecbk };

static G3DUIMENU file_menu_export_sta = { FILEDESC_STA,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          g3dui_exportfilecbk };

/******************************************************************************/
static G3DUIMENU file_menu_import_3ds = { FILEDESC_3DS,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          g3dui_importfilecbk };

static G3DUIMENU file_menu_import_obj = { FILEDESC_OBJ,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          g3dui_importfilecbk };

static G3DUIMENU file_menu_import_v2  = { FILEDESC_V2,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          g3dui_importfilecbk };

#ifdef HAVE_EXPAT_H
static G3DUIMENU file_menu_import_dae = { FILEDESC_DAE,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          g3dui_importfilecbk };
#endif
#ifdef HAVE_C4D_H
static G3DUIMENU file_menu_import_c4d = { FILEDESC_C4D,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          g3dui_importfilecbk };
#endif

/******************************************************************************/
static G3DUIMENU file_menu_new    = { MENU_NEWSCENE,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      g3dui_newscenecbk };

static G3DUIMENU file_menu_open   = { MENU_OPENFILE,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      g3dui_openfilecbk };

static G3DUIMENU file_menu_merge  = { MENU_MERGESCENE,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      g3dui_mergeSceneCbk };

static G3DUIMENU file_menu_import = { MENU_IMPORTFILE,
                                     G3DUIMENUTYPE_SUBMENU,
                                     NULL,
                                     NULL,
                                     .nodes = { &file_menu_import_3ds,
                                                &file_menu_import_obj,
                                                &file_menu_import_v2,
#ifdef HAVE_EXPAT_H
                                                &file_menu_import_dae,
#endif
#ifdef HAVE_C4D_H
                                                &file_menu_import_c4d,
#endif
                                                 NULL                 } };

static G3DUIMENU file_menu_save   = { MENU_SAVEFILE,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      g3dui_savefilecbk };

static G3DUIMENU file_menu_saveas = { MENU_SAVEFILEAS,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      g3dui_saveascbk };

static G3DUIMENU file_menu_export = { MENU_EXPORTSCENE,
                                      G3DUIMENUTYPE_SUBMENU,
                                      NULL,
                                      g3dui_exportfilecbk,
                                     .nodes = { &file_menu_export_obj,
                                                &file_menu_export_pov,
                                                &file_menu_export_sta,
                                                 NULL                 } };

static G3DUIMENU file_menu_exit   = { MENU_EXIT,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      g3dui_exitcbk };

/******************************************************************************/
static G3DUIMENU file_menu = { "File",
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = { &file_menu_new,
                                         &file_menu_open,
                                         &file_menu_merge,
                                         &file_menu_import,
                                         &menu_separator,
                                         &file_menu_save,
                                         &file_menu_saveas,
                                         &file_menu_export,
                                         &menu_separator,
                                         &file_menu_exit,
                                          NULL } };

/******************************************************************************/
/******************************************************************************/
static G3DUIMENU rootnode = { "Bar",
                              G3DUIMENUTYPE_MENUBAR,
                              NULL,
                              NULL,
                             .nodes = { &file_menu,
                                        &edit_menu,
                                        &objects_menu,
                                        &modifiers_menu,
                                        &multipliers_menu,
                                        &functions_menu,
                                        &uvmapping_menu,
                                        &render_menu,
                                        &help_menu,
                                         NULL } };



/******************************************************************************/
void g3dui_updateMenuBar ( G3DUI *gui ) {
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;

    updateMenu_r ( ggt->menuBar, gui );
}

/******************************************************************************/
void createMenuBar ( G3DUI *gui,
                     char  *name,
                     gint   x,
                     gint   y,
                     gint   width,
                     gint   height ) {
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;

    GdkRectangle gdkrec = { x, y, width, height };
    /*GtkWidget *bar = gtk_menu_bar_new ( );*/

    parseMenu_r ( &rootnode, NULL, gui );

    gtk_widget_size_allocate ( rootnode.menu, &gdkrec );
    gtk_fixed_put ( GTK_FIXED(ggt->main), rootnode.menu, x, y );

    gtk_widget_show ( rootnode.menu );


    ggt->menuBar = &rootnode;
}
