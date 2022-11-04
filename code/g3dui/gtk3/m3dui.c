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
uint64_t gtk3_m3dui_setMouseTool ( GTK3M3DUI *gtk3mui,
                                   GtkWidget *button,
                                   char      *toolName ) {
    G3DUI *gui = ( G3DUI * ) gtk3mui->core.gui;
    G3DUIMOUSETOOL *mou = g3dui_getMouseTool ( gui, toolName );
    G3DCAMERA *cam = &gtk3mui->core.main->view->cam;

    if ( mou ) {
        m3dui_setMouseTool ( &gtk3mui->core, cam, mou );

        if ( ( mou->tool->flags & MOUSETOOLNOCURRENT ) == 0x00 ) {
            /*** Remember that widget ID, for example to be unset when a toggle button 
            from another parent widget is called (because XmNradioBehavior won't talk
            to other parent widget ***/
            if ( gtk3mui->currentMouseToolButton ) {
                if ( button != gtk3mui->currentMouseToolButton ) {
                     /*** disengage ***/
                    if ( GTK_IS_TOGGLE_TOOL_BUTTON ( gtk3mui->currentMouseToolButton ) ) {
                        GtkToggleToolButton *ttb = GTK_TOGGLE_TOOL_BUTTON(gtk3mui->currentMouseToolButton);

                        gui->lock = 0x01;

                        gtk_toggle_tool_button_set_active ( ttb, FALSE );

                        gui->lock = 0x00;
                    }
                }
            }

            gtk3mui->currentMouseToolButton = button;
        }
    } else {
        fprintf ( stderr, "No such mousetool %s\n", toolName );
    }

    return UPDATECURRENTMOUSETOOL;
}

/******************************************************************************/
uint64_t gtk3_m3dui_loadImageByChannelIDCbk ( GTK3M3DUI *gtk3mui ) {
    M3DUI *mui = ( M3DUI * ) gtk3mui;
    G3DUI *gui = ( G3DUI * ) gtk3mui->core.gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );
    uint64_t ret = 0x00;

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

                ret = gtk3_loadImageForChannel ( ( GTK3G3DUI * ) gui, chn );

                /*** resize selection mask and zbuffer ***/
                m3dui_resizeBuffers ( mui );
            }
        }
    }

    return ret;
}

/******************************************************************************/
void gtk3_m3dui_saveimage ( GTK3M3DUI *gtk3mui ) {
    M3DUI *mui = ( M3DUI * ) gtk3mui;
    G3DUI *gui = ( G3DUI * ) gtk3mui->core.gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( obj ) {
        if ( obj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            /*** try the first texture in case no texture is selected ***/
            if ( tex == NULL ) tex = g3dmesh_getDefaultTexture ( mes );

            if ( tex ) {
                G3DMATERIAL *mat = tex->mat;
                uint32_t chnID = GETCHANNEL( mui->engine_flags );
                G3DCHANNEL  *chn = g3dmaterial_getChannelByID ( mat, chnID );

                gtk3_g3dui_saveChannelAlteredImage ( gui,
                                                     mat->name,
                                                     chn,
                                                     0x00,
                                                     0x00 );
            }
        }
    }
}

/******************************************************************************/
GTK3M3DUI *gtk3_m3dui_create ( GTK3G3DUI *gtk3gui) {
    GTK3M3DUI* gtk3mui = ( GTK3M3DUI * ) calloc ( 0x01, sizeof ( GTK3M3DUI ) );

    if ( gtk3mui == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    m3dui_init ( &gtk3mui->core, &gtk3gui->core );


    return gtk3mui;
}

/******************************************************************************/
static gboolean Configure ( GtkWindow *window, 
                            GdkEvent  *event,
                            gpointer   data ) {
    GTK3M3DUI *gtk3mui = ( GTK3M3DUI * ) data;
    GTK3M3DUIMAIN *gtk3main = gtk3mui->core.main;

    if ( gtk_widget_get_realized ( GTK_WIDGET ( window ) ) ) {
        gtk3_m3duimain_resize ( &gtk3main->core, 
                                 event->configure.width, 
                                 event->configure.height );
    }


    return TRUE;
}

/******************************************************************************/
void gtk3_m3dui_display ( GTK3M3DUI *gtk3mui ) {
    GtkWidget *win = ui_gtk_window_new ( CLASS_MAIN, GTK_WINDOW_TOPLEVEL );
    GTK3M3DUIMAIN *gtk3main = gtk3_m3duimain_create ( win,
                                                      gtk3mui,
                                                      "MakeUp\n" );

    gtk3mui->topWin = win;

    g_signal_connect ( win, "configure-event", G_CALLBACK (Configure), gtk3mui );

    if ( gtk3mui->core.main == NULL ) {
        gtk3mui->core.main = ( M3DUIMAIN * ) gtk3main;

        gtk_container_add ( win, gtk3main->layout );

        gtk_window_resize ( win, 1024, 576 );

        gtk_widget_show ( win );
    }
}
