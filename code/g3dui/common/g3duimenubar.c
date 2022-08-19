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
static uint32_t sculptModeOnly ( G3DUI *gui ) {
    return ( gui->engine_flags & VIEWSCULPT ) ? 0x01 : 0x00;
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
static uint64_t exitRenderWindowCbk ( G3DUIMENU *menu, void *data ) {
/***
    G3DUIRENDERWINDOW *grw = ( G3DUIRENDERWINDOW * ) user_data;

    gtk_widget_destroy ( grw->topLevel );
*/
    return 0x00;
}

/******************************************************************************/
static G3DUIMENU rw_menu_close   = { RWMENU_CLOSE,
                                     G3DUIMENUTYPE_PUSHBUTTON,
                                     NULL,
                                     exitRenderWindowCbk };

/*static G3DUIMENU rw_menu_savepng = { RWMENU_SAVEPNG,
                                     G3DUIMENUTYPE_PUSHBUTTON,
                                     NULL,
                                     g3duirenderwindow_savePNGCbk };*/

static G3DUIMENU rw_menu_savejpg = { RWMENU_SAVEJPG,
                                     G3DUIMENUTYPE_PUSHBUTTON,
                                     NULL,
                                     g3duirenderwindow_saveJPGCbk };

/******************************************************************************/
static G3DUIMENU rw_file_menu = { "_File",
                                  G3DUIMENUTYPE_SUBMENU,
                                  NULL,
                                  NULL,
                                 .nodes = { /*&rw_menu_savepng,*/
                                            &rw_menu_savejpg,
                                            &rw_menu_close,
                                             NULL } };

/******************************************************************************/
/******************************************************************************/
static G3DUIMENU renderwindowrootnode = { "Bar",
                                          G3DUIMENUTYPE_MENUBAR,
                                          NULL,
                                          NULL,
                                         .nodes = { &rw_file_menu,
                                                     NULL } };

/******************************************************************************/
static uint64_t deleteKeysCbk ( G3DUIMENU *menu, 
                                void      *data ) {
    g3duitimeline_deleteSelectedKeys ( menu->gui );


    return REDRAWTIMELINE;
}

/******************************************************************************/
static uint64_t scaleKeysDialogCbk ( G3DUIMENU *menu, 
                                     void      *data ) {
/***
    G3DUITIMELINE *tim = ( G3DUITIMELINE * ) user_data;
    GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );

    createScaleKeysDialog ( dial,
                            tim->grp.gui,
                            "Scale Keys",
                            0,
                            0,
                            150,
                            96 );

    gtk_widget_show ( dial );
*/
    return 0x00;
}

/******************************************************************************/
static uint64_t scaleKeysCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    G3DUITIMELINE *tim = ( G3DUITIMELINE * ) user_data;

    tim->tdata->tool = TIME_SCALE_TOOL;


    return 0x00;
}

/******************************************************************************/
static uint64_t selectKeysCbk ( G3DUIMENU *menu, 
                                void      *data ) {
    G3DUITIMELINE *tim = ( G3DUITIMELINE * ) user_data;

    tim->tdata->tool = TIME_MOVE_TOOL;


    return 0x00;
}

/******************************************************************************/
static G3DUIMENU time_menu_delete = { TIMEMENU_DELETE,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      deleteKeysCbk };

static G3DUIMENU time_menu_scale = { TIMEMENU_SCALE,
                                     G3DUIMENUTYPE_PUSHBUTTON,
                                     NULL,
                                     scaleKeysDialogCbk };

static G3DUIMENU time_menu_select = { TIMEMENU_SELECT,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      selectKeysCbk };

/******************************************************************************/
static G3DUIMENU time_menu = { "_Options",
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = { /*&time_menu_select,*/
                                         &time_menu_scale,
                                         &time_menu_delete,
                                          NULL } };

/******************************************************************************/
/******************************************************************************/
static uint64_t useDefaultCameraCbk ( G3DUIMENU *menu, 
                                      void      *data ) {
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
static G3DUIMENU view_menu_defcam   = { VIEWMENU_DEFAULTCAMERA,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        useDefaultCameraCbk };

static G3DUIMENU view_menu_selcam   = { VIEWMENU_SELECTEDCAMERA,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        useSelectedCameraCbk };

static G3DUIMENU view_menu_normals  = { VIEWMENU_NORMALS,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        toggleNormalsCbk };

static G3DUIMENU view_menu_bones    = { VIEWMENU_BONES,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        toggleBonesCbk };

static G3DUIMENU view_menu_grid     = { VIEWMENU_GRID,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        toggleGridCbk };

static G3DUIMENU view_menu_textures = { VIEWMENU_TEXTURES,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        toggleTexturesCbk };

static G3DUIMENU view_menu_bg       = { VIEWMENU_BACKGROUND,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        toggleBackgroundImageCbk };

static G3DUIMENU view_menu_lighting = { VIEWMENU_LIGHTING,
                                        G3DUIMENUTYPE_TOGGLEBUTTON,
                                        NULL,
                                        toggleLightingCbk };

static G3DUIMENU view_menu_reset    = { VIEWMENU_RESET,
                                        G3DUIMENUTYPE_PUSHBUTTON,
                                        NULL,
                                        resetCameraCbk };

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
/******************************************************************************/
static uint64_t setMaterialCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    g3dui_setMaterialCbk ( menu->gui );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t removeMaterialCbk ( G3DUIMENU *menu, 
                                    void      *data ) {
#ifdef TODO
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
#endif

    return REDRAWVIEW | REDRAWMATERIALLIST | REDRAWCURRENTMATERIAL;
}

/******************************************************************************/
static uint64_t addMaterialCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
#ifdef TODO
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
#endif


    return REDRAWMATERIALLIST | REDRAWCURRENTMATERIAL;
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
/******************************************************************************/
/******************************************************************************/
static uint64_t addVibratorTagCbk ( G3DUIMENU *menu, 
                                    void      *data ) {
    g3dui_addVibratorTagCbk ( menu->gui );
}

/******************************************************************************/
static uint64_t addTrackerTagCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_addTrackerTagCbk ( menu->gui );
}


/******************************************************************************/
static uint64_t removeSelectedTagCbk ( G3DUIMENU *menu, 
                                       void      *data ) {
    g3dui_removeSelectedTagCbk ( menu->gui );
}

/******************************************************************************/
/****************************** Tags MENU **********************************/
static G3DUIMENU tags_menu_addVibrator     = { MENU_ADDVIBRATORTAG,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectMode_objectSelected,
                                               addVibratorTagCbk };

static G3DUIMENU tags_menu_addTrackerTag   = { MENU_ADDTRACKERTAG,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectMode_objectSelected,
                                               addTrackerTagCbk };

static G3DUIMENU tags_menu_removeSelTag    = { MENU_REMOVESELTAG,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectMode_objectSelected,
                                               removeSelectedTagCbk };

/******************************************************************************/
static G3DUIMENU tags_menu = { "Tags",
                               G3DUIMENUTYPE_SUBMENU,
                               NULL,
                               NULL,
                              .nodes = { /*&tags_menu_addVibrator,*/
                                         &tags_menu_addTrackerTag,
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
/******************************************************************************/
/******************************************************************************/
static uint64_t aboutCbk ( G3DUIMENU *menu, 
                           void      *data ) {
#ifdef TODO
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
#endif

    return 0x00;
}

/******************************************************************************/
static G3DUIMENU uvhelp_menu_about = { MENU_ABOUT,
                                       G3DUIMENUTYPE_PUSHBUTTON,
                                       NULL,
                                       aboutCbk };

/******************************************************************************/
static G3DUIMENU uvhelp_menu = { "Help",
                                  G3DUIMENUTYPE_SUBMENU,
                                  NULL,
                                  NULL,
                                 .nodes = { &uvhelp_menu_about,
                                             NULL } };

/******************************************************************************/
/******************************************************************************/
static uint64_t resizeChannelImageCbk ( G3DUIMENU *menu, 
                                        void      *data ) {
#ifdef TODO
    M3DUI *mui = ( M3DUI * ) user_data;
    G3DCHANNEL *chn = common_m3dui_getWorkingChannel ( mui );

    if ( chn ) {
        GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );

        createChannelImage ( dial, 
                             mui, 
                             chn,
                             0x01,
                             "Channel Image", 0, 0, 200, 96 );

        gtk_widget_show ( dial );
    }
#endif

    return 0x00;
}

/******************************************************************************/
static uint64_t fgfillCbk ( G3DUIMENU *menu, 
                            void      *data ) {
    M3DUI *mui = ( M3DUI * ) user_data;
    M3DSYSINFO *sysinfo = m3dsysinfo_get ( );

    m3dui_fillWithColor ( mui, sysinfo->fgcolor );


    return REDRAWUVMAPEDITOR;
}

/******************************************************************************/
static uint64_t bgfillCbk ( G3DUIMENU *menu, 
                            void      *data ) {
    M3DUI *mui = ( M3DUI * ) user_data;
    M3DSYSINFO *sysinfo = m3dsysinfo_get ( );

    m3dui_fillWithColor ( mui, sysinfo->bgcolor );


    return REDRAWUVMAPEDITOR;
}

/******************************************************************************/
static G3DUIMENU uvimage_menu_resize = { MENU_RESIZEIMAGE,
                                         G3DUIMENUTYPE_PUSHBUTTON,
                                         NULL,
                                         resizeChannelImageCbk };

static G3DUIMENU uvimage_menu_fgfill = { MENU_FGFILL,
                                         G3DUIMENUTYPE_PUSHBUTTON,
                                         NULL,
                                         fgfillCbk };

static G3DUIMENU uvimage_menu_bgfill = { MENU_BGFILL,
                                         G3DUIMENUTYPE_PUSHBUTTON,
                                         NULL,
                                         bgfillCbk };

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
static uint64_t fac2uvsetCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    M3DUI *mui = ( M3DUI * ) user_data;

    m3dui_fac2uvsetCbk ( mui );


    return REDRAWUVMAPEDITOR;
}

/******************************************************************************/
static uint64_t uvset2facCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    M3DUI *mui = ( M3DUI * ) user_data;

    uvset2facCbk ( mui );


    return REDRAWUVMAPEDITOR;
}

/******************************************************************************/
static uint64_t ver2uvCbk ( G3DUIMENU *menu, 
                            void      *data ) {
    M3DUI *mui = ( M3DUI * ) user_data;

    m3dui_ver2uvCbk ( mui );


    return REDRAWUVMAPEDITOR;
}

/******************************************************************************/
static uint64_t uv2verCbk ( G3DUIMENU *menu, 
                            void      *data ) {
    M3DUI *mui = ( M3DUI * ) user_data;

    m3dui_uv2verCbk ( lui );


    return REDRAWUVMAPEDITOR;
}

/******************************************************************************/
static G3DUIMENU uvedit_menu_undo      = { MENU_UNDO,
                                           G3DUIMENUTYPE_PUSHBUTTON,
                                           NULL,
                                           m3dui_undoCbk };

static G3DUIMENU uvedit_menu_redo      = { MENU_REDO,
                                           G3DUIMENUTYPE_PUSHBUTTON,
                                           NULL,
                                           m3dui_redoCbk };

static G3DUIMENU uvedit_menu_uv2ver    = { MENU_VERTEXFROMUV,
                                           G3DUIMENUTYPE_PUSHBUTTON,
                                           NULL,
                                           uv2verCbk };

static G3DUIMENU uvedit_menu_ver2uv    = { MENU_UVFROMVERTEX,
                                           G3DUIMENUTYPE_PUSHBUTTON,
                                           NULL,
                                           ver2uvCbk };

static G3DUIMENU uvedit_menu_uvset2fac = { MENU_FACEFROMUVSET,
                                           G3DUIMENUTYPE_PUSHBUTTON,
                                           NULL,
                                           uvset2facCbk };

static G3DUIMENU uvedit_menu_fac2uvset = { MENU_UVSETFROMFACE,
                                           G3DUIMENUTYPE_PUSHBUTTON,
                                           NULL,
                                           fac2uvsetCbk };

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
static uint64_t loadImageByChannelIDCbk ( G3DUIMENU *menu, 
                                          void      *data ) {
    M3DUI *mui = ( M3DUI * ) user_data;
    G3DUI *gui = ( G3DUI * ) mui->gui;
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
                m3dui_resizeBuffers ( mui );
            }
        }
    }


    return REDRAWUVMAPEDITOR;
}

/******************************************************************************/
static uint64_t createChannelImageCbk ( G3DUIMENU *menu, 
                                        void      *data ) {
#ifdef TODO
    M3DUI *mui = ( M3DUI * ) user_data;
    G3DCHANNEL *chn = common_m3dui_getWorkingChannel ( mui );

    if ( chn ) {
        GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );

        createChannelImage ( dial, 
                             lui, 
                             chn,
                             0x00,
                             "Channel Image", 0, 0, 200, 96 );

        gtk_widget_show ( dial );
    }
#endif

    return 0x00;
}

/******************************************************************************/
static G3DUIMENU uvfile_menu_createChannelImage = { MENU_CREATECHANNELIMAGE,
                                                    G3DUIMENUTYPE_PUSHBUTTON,
                                                    NULL,
                                                    createChannelImageCbk };

static G3DUIMENU uvfile_menu_openChannelImage   = { MENU_OPENCHANNELIMAGE,
                                                    G3DUIMENUTYPE_PUSHBUTTON,
                                                    NULL,
                                                    loadImageByChannelIDCbk };

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
/******************************************************************************/
/******************************************************************************/
static uint64_t addUVMapCbk ( G3DUIMENU *menu, 
                              void      *data ) {
    g3duimenubar_addUVMapCbk ( menu->gui );


    return REDRAWLIST;
}

/******************************************************************************/
static uint64_t fitUVMapCbk ( G3DUIMENU *menu, 
                              void      *data ) {
    g3duimenubar_fitUVMapCbk ( menu->gui );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t alignUVMapCbk ( G3DUIMENU *menu, 
                                void      *data ) {
    g3duimenubar_alignUVMapCbk ( menu->gui, menu->name );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t renderViewCbk ( G3DUIMENU *menu, 
                                void      *data ) {
    return 0x00;
}

/******************************************************************************/
static uint64_t renderSettingsCbk ( G3DUIMENU *menu, 
                                    void      *data ) {
#ifdef TODO
    G3DUI *gui = ( G3DUI * ) user_data;
    GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );

    /*** For some reason, GtkSpinButtons calls its callbacks ***/
    /*** when being realized. With this trick I bypass that. ***/
    /*** Callbacks will return prematurely if gui->lock == 0x01 ***/
    gui->lock = 0x01;

    createRenderEdit ( dial, gui, "RENDEREDIT", 0, 0, 480, 340 );

    gtk_widget_show ( dial );

    gui->lock = 0x00;
#endif
    return 0x00;
}

/******************************************************************************/
static uint64_t mergeMeshCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    g3dui_mergeMeshCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t splitMeshCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    g3dui_splitMeshCbk ( menu->gui, menu->name );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t mirrorHeightmapCbk ( G3DUIMENU *menu, 
                                     void      *data ) {
    g3dui_mirrorHeightmapCbk ( menu->gui, menu->name );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t mirrorWeightGroupCbk ( G3DUIMENU *menu, 
                                       void      *data ) {
    g3dui_mirrorWeightGroupCbk ( menu->gui, menu->name );


    return REDRAWVIEW | REDRAWLIST;
}

/******************************************************************************/
static uint64_t resetBoneTreeCbk ( G3DUIMENU *menu, 
                                       void      *data ) {
    g3dui_resetBoneTreeCbk ( gui );


    return REDRAWVIEW | REDRAWLIST;
}

/******************************************************************************/
static uint64_t resetBoneCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    g3dui_resetBoneCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST;
}

/******************************************************************************/
static uint64_t fixBoneTreeCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    g3dui_fixBoneTreeCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST;
}

/******************************************************************************/
static uint64_t fixBoneCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    g3dui_fixBoneCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST;
}

/******************************************************************************/
static uint64_t addSymmetryCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    g3dui_addSymmetryCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addInstanceCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    g3dui_addInstanceCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addEmitterCbk ( G3DUIMENU *menu, 
                                void      *data ) {
    g3dui_addEmitterCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addWireframeCbk ( G3DUIMENU *menu, 
                                  void      *data ) {

    g3dui_addWireframeCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addSubdividerCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_addSubdividerCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addSplineRevolverCbk ( G3DUIMENU *menu, 
                                       void      *data ) {
    g3dui_addSplineRevolverCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addFFDBoxCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    g3dui_addFFDBoxCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addMorpherCbk ( G3DUIMENU *menu, 
                                void      *data ) {
    g3dui_addMorpherCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addSkinCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    g3dui_addSkinCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addBoneCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    g3dui_addBoneCbk ( gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addPlaneCbk ( G3DUIMENU *menu, 
                              void      *data ) {
    g3dui_addPlaneCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addCubeCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    g3dui_addCubeCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addSphereCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    g3dui_addSphereCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addTorusCbk ( G3DUIMENU *menu, 
                              void      *data ) {
    g3dui_addTorusCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addCylinderCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    g3dui_addCylinderCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addTubeCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    g3dui_addTubeCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addLightCbk ( G3DUIMENU *menu, 
                              void      *data ) {
    g3dui_addLightCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addCameraCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    g3dui_addCameraCbk ( menu->gui, g3dui_getCurrentViewCamera ( menu->gui ) );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addEmptyMeshCbk  ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_addEmptyMeshCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addSplineCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_addSplineCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addTextCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    g3dui_addTextCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t addNullCbk ( G3DUIMENU *menu, 
                             void      *data ) {
    g3dui_addNullCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t invertNormalCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_invertNormalCbk ( menu->gui );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t alignNormalsCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_alignNormalsCbk ( menu->gui );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t triangulateCbk ( G3DUIMENU *menu, 
                                 void      *data ) {
    g3dui_triangulateCbk ( menu->gui, menu->name );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t untriangulateCbk ( G3DUIMENU *menu, 
                                   void      *data ) {
    g3dui_untriangulateCbk ( menu->gui );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t weldVerticesCbk ( G3DUIMENU *menu, 
                                  void      *data ) {
    g3dui_weldVerticesCbk ( menu->gui );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t deleteLoneVerticesCbk ( G3DUIMENU *menu, 
                                        void      *data ) {
    g3dui_deleteLoneVerticesCbk ( menu->gui );


    return REDRAWVIEW;
}

/******************************************************************************/
static uint64_t selectAllCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    g3dui_selectAllCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t invertSelectionCbk ( G3DUIMENU *menu, 
                                     void      *data ) {
    g3dui_invertSelectionCbk ( menu->gui );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t getObjectStatsCbk ( G3DUIMENU *menu, 
                                    void      *data ) {
#ifdef TODO
    G3DUI *gui = ( G3DUI * ) user_data;
    char buffer[0x200] = { 0 };
    GtkWidget *dialog;

    g3dui_getObjectStatsCbk ( menu->gui, buffer, 0x200 );

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
#endif

    return 0x00;
}

/******************************************************************************/
static uint64_t selectTreeCbk ( G3DUIMENU *menu, 
                                void      *data ) {
    g3dui_selectTreeCbk ( menu->gui, menu->name );


    return REDRAWVIEW | REDRAWLIST | REDRAWCURRENTOBJECT;
}

/******************************************************************************/
static uint64_t exitCbk ( G3DUIMENU *menu, 
                          void      *data ) {
#ifdef TODO
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
#endif

    return 0x00;
}

/******************************************************************************/
static uint64_t exportFileCbk ( G3DUIMENU *menu, 
                                void      *data ) {
#ifdef TODO
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

        g3dui_exportFileOkCbk ( gui, filedesc, filename );

        g_free    ( ( gpointer ) filename );
    }

    gtk_widget_destroy ( dialog );
#endif

    return 0x00;
}

/******************************************************************************/
static uint64_t importFileCbk ( G3DUIMENU *menu, 
                                void      *data ) {
#ifdef TODO
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

        g3dui_importFileOkCbk ( gui, filedesc, filename );

        g_free    ( ( gpointer ) filename );
    }

    gtk_widget_destroy ( dialog );
#endif

    return 0x00;
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
                                     aboutCbk };

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
                                          renderViewCbk };

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
static G3DUIMENU functions_menu_mirrorHeightmap_xy = { MENU_MIRRORXY,
                                                       G3DUIMENUTYPE_PUSHBUTTON,
                                                       sculptModeOnly,
                                                       mirrorHeightmapCbk };

static G3DUIMENU functions_menu_mirrorHeightmap_yz = { MENU_MIRRORYZ,
                                                       G3DUIMENUTYPE_PUSHBUTTON,
                                                       sculptModeOnly,
                                                       mirrorHeightmapCbk };

static G3DUIMENU functions_menu_mirrorHeightmap_zx = { MENU_MIRRORZX,
                                                       G3DUIMENUTYPE_PUSHBUTTON,
                                                       sculptModeOnly,
                                                       mirrorHeightmapCbk };

/******************************************************************************/
static G3DUIMENU functions_menu_mirrorWeightgroup_xy = { MENU_MIRRORXY,
                                                         G3DUIMENUTYPE_PUSHBUTTON,
                                                         skinModeOnly,
                                                         mirrorWeightGroupCbk };

static G3DUIMENU functions_menu_mirrorWeightgroup_yz = { MENU_MIRRORYZ,
                                                         G3DUIMENUTYPE_PUSHBUTTON,
                                                         skinModeOnly,
                                                         mirrorWeightGroupCbk };

static G3DUIMENU functions_menu_mirrorWeightgroup_zx = { MENU_MIRRORZX,
                                                         G3DUIMENUTYPE_PUSHBUTTON,
                                                         skinModeOnly,
                                                         mirrorWeightGroupCbk };

/******************************************************************************/
static G3DUIMENU functions_menu_splitMesh_keep   = { MENU_SPLITANDKEEP,
                                                     G3DUIMENUTYPE_PUSHBUTTON,
                                                     faceModeOnly,
                                                     splitMeshCbk };

static G3DUIMENU functions_menu_splitMesh_remove = { MENU_SPLITANDREMOVE,
                                                     G3DUIMENUTYPE_PUSHBUTTON,
                                                     faceModeOnly,
                                                     splitMeshCbk };

/******************************************************************************/
static G3DUIMENU functions_menu_mirrorHM     = { MENU_MIRRORHEIGHTMAP,
                                                 G3DUIMENUTYPE_SUBMENU,
                                                 sculptModeOnly,
                                                 NULL,
                                                .nodes = { &functions_menu_mirrorHeightmap_xy,
                                                           &functions_menu_mirrorHeightmap_yz,
                                                           &functions_menu_mirrorHeightmap_zx,
                                                            NULL } };

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
                                                 mergeMeshCbk };

static G3DUIMENU functions_menu_makeEditable = { MENU_MAKEEDITABLE,
                                                 G3DUIMENUTYPE_PUSHBUTTON,
                                                 objectModeOnly,
                                                 makeEditableCbk };

/******************************************************************************/
static G3DUIMENU functions_menu = { "Functions",
                                    G3DUIMENUTYPE_SUBMENU,
                                    NULL,
                                    NULL,
                                   .nodes = { &functions_menu_mirrorWG,
                                              &functions_menu_mirrorHM,
                                              &functions_menu_splitMesh,
                                              &functions_menu_mergeMesh,
                                              &functions_menu_makeEditable,
                                               NULL } };


/******************************************************************************/
/***************************** Multipliers MENU *******************************/
static G3DUIMENU multipliers_menu_addEmitter = { MENU_ADDEMITTER,
                                                 G3DUIMENUTYPE_PUSHBUTTON,
                                                 objectModeOnly,
                                                 addEmitterCbk };

static G3DUIMENU multipliers_menu_addInstance = { MENU_ADDINSTANCE,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addInstanceCbk };

static G3DUIMENU multipliers_menu_addSymmetry = { MENU_ADDSYMMETRY,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addSymmetryCbk };

/******************************************************************************/
static G3DUIMENU multipliers_menu = { "Multipliers",
                                      G3DUIMENUTYPE_SUBMENU,
                                      NULL,
                                      NULL,
                                     .nodes = { &multipliers_menu_addInstance,
                                                &multipliers_menu_addSymmetry,
                                                &multipliers_menu_addEmitter,
                                                 NULL } };

/******************************************************************************/
/****************************** Modifiers MENU ********************************/
static G3DUIMENU modifiers_menu_resetBone_tree = { MENU_RESETBONETREE,
                                                   G3DUIMENUTYPE_PUSHBUTTON,
                                                   objectMode_boneSelected,
                                                   resetBoneTreeCbk };

static G3DUIMENU modifiers_menu_resetBone_only = { MENU_RESETBONEONLY,
                                                   G3DUIMENUTYPE_PUSHBUTTON,
                                                   objectMode_boneSelected,
                                                   resetBoneCbk };
/******************************************************************************/
static G3DUIMENU modifiers_menu_fixBone_tree = { MENU_FIXBONETREE,
                                                 G3DUIMENUTYPE_PUSHBUTTON,
                                                 objectMode_boneSelected,
                                                 fixBoneTreeCbk };

static G3DUIMENU modifiers_menu_fixBone_only = { MENU_FIXBONEONLY,
                                                 G3DUIMENUTYPE_PUSHBUTTON,
                                                 objectMode_boneSelected,
                                                 fixBoneCbk };

/******************************************************************************/
static G3DUIMENU modifiers_menu_addWireframe  = { MENU_ADDWIREFRAME,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addWireframeCbk };

static G3DUIMENU modifiers_menu_addSubdivider = { MENU_ADDSUBDIVIDER,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addSubdividerCbk };

static G3DUIMENU modifiers_menu_addFFDBox     = { MENU_ADDFFDBOX,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addFFDBoxCbk };

static G3DUIMENU modifiers_menu_addMorpher    = { MENU_ADDMORPHER,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addMorpherCbk };

static G3DUIMENU modifiers_menu_addSkin       = { MENU_ADDSKIN,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectModeOnly,
                                                  addSkinCbk };

static G3DUIMENU modifiers_menu_addBone       = { MENU_ADDBONE,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectMode_boneOrSkinSelected,
                                                  addBoneCbk };

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
                                                  addSplineRevolverCbk };

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
                                               addPlaneCbk };

static G3DUIMENU objects_menu_addCube      = { MENU_ADDCUBE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addCubeCbk };

static G3DUIMENU objects_menu_addSphere    = { MENU_ADDSPHERE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addSphereCbk };

static G3DUIMENU objects_menu_addTorus     = { MENU_ADDTORUS,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addTorusCbk };

static G3DUIMENU objects_menu_addCylinder  = { MENU_ADDCYLINDER,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addCylinderCbk };

static G3DUIMENU objects_menu_addTube      = { MENU_ADDTUBE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addTubeCbk };

static G3DUIMENU objects_menu_addText      = { MENU_ADDTEXT,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addTextCbk };

static G3DUIMENU objects_menu_addNull      = { MENU_ADDNULL,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addNullCbk };

static G3DUIMENU objects_menu_addLight     = { MENU_ADDLIGHT,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addLightCbk };

static G3DUIMENU objects_menu_addCamera    = { MENU_ADDCAMERA,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addCameraCbk };

static G3DUIMENU objects_menu_addEmptyMesh = { MENU_ADDEMPTYMESH,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addEmptyMeshCbk };

static G3DUIMENU objects_menu_addSpline    = { MENU_ADDSPLINE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               objectModeOnly,
                                               addSplineCbk };

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
                                               selectTreeCbk };

static G3DUIMENU edit_menu_selectTree_same = { MENU_OPTION_SAMETYPE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               NULL,
                                               selectTreeCbk };

/******************************************************************************/
static G3DUIMENU edit_menu_triangulate_cw  = { MENU_OPTION_CLOCKWISE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               NULL,
                                               triangulateCbk };

static G3DUIMENU edit_menu_triangulate_ccw = { MENU_OPTION_ANTICLOCKWISE,
                                               G3DUIMENUTYPE_PUSHBUTTON,
                                               NULL,
                                               triangulateCbk };

/******************************************************************************/
static G3DUIMENU edit_menu_undo               = { MENU_UNDO,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  NULL,
                                                  undoCbk };

static G3DUIMENU edit_menu_redo               = { MENU_REDO,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  NULL,
                                                  undoCbk };

static G3DUIMENU edit_menu_invertNormals      = { MENU_INVERTNORMALS,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  faceModeOnly,
                                                  invertNormalCbk };

static G3DUIMENU edit_menu_alignNormals       = { MENU_ALIGNNORMALS,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  faceModeOnly,
                                                  alignNormalsCbk };

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
                                                  untriangulateCbk };

static G3DUIMENU edit_menu_weldVertices       = { MENU_WELDVERTICES,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  vertexModeOnly,
                                                  weldVerticesCbk };

static G3DUIMENU edit_menu_deleteLoneVertices = { MENU_DELETELONEVERTICES,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  vertexModeOnly,
                                                  deleteLoneVerticesCbk };

static G3DUIMENU edit_menu_selectAll          = { MENU_SELECTALL,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  NULL,
                                                  selectAllCbk };

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
                                                  invertSelectionCbk };

static G3DUIMENU edit_menu_getObjectStats     = { MENU_GETOBJECTSTATS,
                                                  G3DUIMENUTYPE_PUSHBUTTON,
                                                  objectMode_objectSelected,
                                                  getObjectStatsCbk };

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
                                          exportFileCbk };

static G3DUIMENU file_menu_export_pov = { FILEDESC_POV,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          exportFileCbk };

static G3DUIMENU file_menu_export_sta = { FILEDESC_STA,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          exportFileCbk };

/******************************************************************************/
static G3DUIMENU file_menu_import_3ds = { FILEDESC_3DS,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          importFileCbk };

static G3DUIMENU file_menu_import_obj = { FILEDESC_OBJ,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          importFileCbk };

static G3DUIMENU file_menu_import_v2  = { FILEDESC_V2,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          importFileCbk };

#ifdef HAVE_EXPAT_H
static G3DUIMENU file_menu_import_dae = { FILEDESC_DAE,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          importFileCbk };
#endif
#ifdef HAVE_C4D_H
static G3DUIMENU file_menu_import_c4d = { FILEDESC_C4D,
                                          G3DUIMENUTYPE_PUSHBUTTON,
                                          NULL,
                                          importFileCbk };
#endif

/******************************************************************************/
static G3DUIMENU file_menu_new    = { MENU_NEWSCENE,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      newSceneCbk };

static G3DUIMENU file_menu_open   = { MENU_OPENFILE,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      openFileCbk };

static G3DUIMENU file_menu_merge  = { MENU_MERGESCENE,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      mergeSceneCbk };

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
                                      saveFileCbk };

static G3DUIMENU file_menu_saveas = { MENU_SAVEFILEAS,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      saveAsCbk };

static G3DUIMENU file_menu_export = { MENU_EXPORTSCENE,
                                      G3DUIMENUTYPE_SUBMENU,
                                      NULL,
                                      exportFileCbk,
                                     .nodes = { &file_menu_export_obj,
                                                &file_menu_export_pov,
                                                &file_menu_export_sta,
                                                 NULL                 } };

static G3DUIMENU file_menu_exit   = { MENU_EXIT,
                                      G3DUIMENUTYPE_PUSHBUTTON,
                                      NULL,
                                      exitCbk };

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
