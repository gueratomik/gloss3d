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
/*static G3DUIMENU uvhelp_menu_about = { MENU_ABOUT,
                                       G3DUIMENUTYPE_PUSHBUTTON,
                                       NULL,
                                       aboutCbk };*/

/******************************************************************************/
/*static G3DUIMENU uvhelp_menu = { "Help",
                                  G3DUIMENUTYPE_SUBMENU,
                                  NULL,
                                  NULL,
                                 .nodes = { &uvhelp_menu_about,
                                             NULL } };*/

/******************************************************************************/
/******************************************************************************/
static uint64_t resizeChannelImageCbk ( G3DUIMENU *menu, 
                                        void      *data ) {
#ifdef TODO
    M3DUI *mui = ( M3DUI * ) data;
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
    M3DUI *mui = ( M3DUI * ) data;
    M3DSYSINFO *sysinfo = m3dsysinfo_get ( );

    m3dui_fillWithColorCbk ( mui, sysinfo->fgcolor );


    return REDRAWUVMAPEDITOR;
}

/******************************************************************************/
static uint64_t bgfillCbk ( G3DUIMENU *menu, 
                            void      *data ) {
    M3DUI *mui = ( M3DUI * ) data;
    M3DSYSINFO *sysinfo = m3dsysinfo_get ( );

    m3dui_fillWithColorCbk ( mui, sysinfo->bgcolor );


    return REDRAWUVMAPEDITOR;
}

/******************************************************************************/
static G3DUIMENU uvimage_menu_resize = { NULL,
                                         MENU_RESIZEIMAGE,
                                         G3DUIMENUTYPE_PUSHBUTTON,
                                         NULL,
                                         resizeChannelImageCbk };

static G3DUIMENU uvimage_menu_fgfill = { NULL,
                                         MENU_FGFILL,
                                         G3DUIMENUTYPE_PUSHBUTTON,
                                         NULL,
                                         fgfillCbk };

static G3DUIMENU uvimage_menu_bgfill = { NULL,
                                         MENU_BGFILL,
                                         G3DUIMENUTYPE_PUSHBUTTON,
                                         NULL,
                                         bgfillCbk };

/******************************************************************************/
static G3DUIMENU uvimage_menu = { NULL,
                                  "Image",
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
    M3DUI *mui = ( M3DUI * ) data;

    m3dui_fac2uvsetCbk ( mui );


    return REDRAWUVMAPEDITOR;
}

/******************************************************************************/
static uint64_t uvset2facCbk ( G3DUIMENU *menu, 
                               void      *data ) {
    M3DUI *mui = ( M3DUI * ) data;

    m3dui_uvset2facCbk ( mui );


    return REDRAWUVMAPEDITOR;
}

/******************************************************************************/
static uint64_t ver2uvCbk ( G3DUIMENU *menu, 
                            void      *data ) {
    M3DUI *mui = ( M3DUI * ) data;

    m3dui_ver2uvCbk ( mui );


    return REDRAWUVMAPEDITOR;
}

/******************************************************************************/
static uint64_t uv2verCbk ( G3DUIMENU *menu, 
                            void      *data ) {
    M3DUI *mui = ( M3DUI * ) data;

    m3dui_uv2verCbk ( mui );


    return REDRAWUVMAPEDITOR;
}

/******************************************************************************/
static uint64_t uvUndoCbk ( G3DUIMENU *menu, 
                            void      *data ) {
    M3DUI *mui = ( M3DUI * ) data;

    m3dui_undoCbk ( mui );


    return REDRAWUVMAPEDITOR;
}

/******************************************************************************/
static uint64_t uvRedoCbk ( G3DUIMENU *menu, 
                            void      *data ) {
    M3DUI *mui = ( M3DUI * ) data;

    m3dui_redoCbk ( mui );


    return REDRAWUVMAPEDITOR;
}

/******************************************************************************/
static G3DUIMENU uvedit_menu_undo      = { NULL,
                                           MENU_UNDO,
                                           G3DUIMENUTYPE_PUSHBUTTON,
                                           NULL,
                                           uvUndoCbk };
static G3DUIMENU uvedit_menu_redo      = { NULL,
                                           MENU_REDO,
                                           G3DUIMENUTYPE_PUSHBUTTON,
                                           NULL,
                                           uvRedoCbk };

static G3DUIMENU uvedit_menu_uv2ver    = { NULL,
                                           MENU_VERTEXFROMUV,
                                           G3DUIMENUTYPE_PUSHBUTTON,
                                           NULL,
                                           uv2verCbk };

static G3DUIMENU uvedit_menu_ver2uv    = { NULL,
                                           MENU_UVFROMVERTEX,
                                           G3DUIMENUTYPE_PUSHBUTTON,
                                           NULL,
                                           ver2uvCbk };

static G3DUIMENU uvedit_menu_uvset2fac = { NULL,
                                           MENU_FACEFROMUVSET,
                                           G3DUIMENUTYPE_PUSHBUTTON,
                                           NULL,
                                           uvset2facCbk };

static G3DUIMENU uvedit_menu_fac2uvset = { NULL,
                                           MENU_UVSETFROMFACE,
                                           G3DUIMENUTYPE_PUSHBUTTON,
                                           NULL,
                                           fac2uvsetCbk };

/******************************************************************************/
static G3DUIMENU uvedit_menu = { NULL,
                                 "Edit",
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
#ifdef TODO
    M3DUI *mui = ( M3DUI * ) data;
    G3DUI *gui = ( G3DUI * ) mui->gui;
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
                uint32_t chnID = GETCHANNEL(mui->engine_flags);
                G3DCHANNEL  *chn = g3dmaterial_getChannelByID ( mat, chnID );

                g3dui_loadImageForChannel ( gui, chn );

                /*** resize selection mask and zbuffer ***/
                m3dui_resizeBuffers ( mui );
            }
        }
    }
#endif

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
static G3DUIMENU uvfile_menu_createChannelImage = { NULL,
                                                    MENU_CREATECHANNELIMAGE,
                                                    G3DUIMENUTYPE_PUSHBUTTON,
                                                    NULL,
                                                    createChannelImageCbk };

static G3DUIMENU uvfile_menu_openChannelImage   = { NULL,
                                                    MENU_OPENCHANNELIMAGE,
                                                    G3DUIMENUTYPE_PUSHBUTTON,
                                                    NULL,
                                                    loadImageByChannelIDCbk };

/******************************************************************************/
static G3DUIMENU uvfile_menu = { NULL,
                                 "File",
                                 G3DUIMENUTYPE_SUBMENU,
                                 NULL,
                                 NULL,
                                .nodes = { &uvfile_menu_createChannelImage,
                                           &uvfile_menu_openChannelImage,
                                            NULL } };

/******************************************************************************/
/******************************************************************************/
static G3DUIMENU uvrootnode = { NULL,
                                "Bar",
                                G3DUIMENUTYPE_MENUBAR,
                                NULL,
                                NULL,
                               .nodes = { &uvfile_menu,
                                          &uvedit_menu,
                                          &uvimage_menu,
                                          /*&uvhelp_menu,*/
                                           NULL } };

/******************************************************************************/
G3DUIMENU *g3duimenu_getUVEditorMenuNode ( ) {
    return &uvrootnode;
}
