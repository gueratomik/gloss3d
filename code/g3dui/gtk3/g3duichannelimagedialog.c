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

#define WIDTHBUTTONNAME  "Width"
#define HEIGHTBUTTONNAME "Height"

/******************************************************************************/
typedef struct _CHANNELIMAGEDATA {
    G3DUIUVMAPEDITOR *uvme;
    G3DCHANNEL       *chn;
    uint32_t          width;
    uint32_t          height;
    uint32_t          resize;
} CHANNELIMAGEDATA;

/******************************************************************************/
static void updateChannelImageDialog ( GtkWidget        *widget, 
                                       CHANNELIMAGEDATA *cid ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DUI *gui = cid->uvme->gui;

    /*** prevents a loop ***/
    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( GTK_IS_LABEL ( child ) ) {
            GtkLabel *lab = GTK_LABEL (child);
            uint32_t width  = g3dcore_getNextPowerOfTwo ( cid->width  ),
                     height = g3dcore_getNextPowerOfTwo ( cid->height );
            char text[0x100];

            snprintf ( text, 0x100, "The final size will be power-of-2 compliant: %dx%d\n", width, height );

            gtk_label_set_text ( lab, text );
        }

        children =  g_list_next ( children );
    }

    gui->lock = 0x00;
}

/******************************************************************************/
CHANNELIMAGEDATA *channelimagedata_new ( G3DUIUVMAPEDITOR *uvme, 
                                         G3DCHANNEL       *chn,
                                         uint32_t          resize ) {
    void *memarea = calloc ( 0x01, sizeof ( CHANNELIMAGEDATA ) );
    CHANNELIMAGEDATA *cid = ( CHANNELIMAGEDATA * ) memarea;

    if ( cid == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }
 
    cid->uvme   = uvme;
    cid->chn    = chn;
    cid->resize = resize;

    cid->width  = 512;
    cid->height = 512;

    return cid;
}

/******************************************************************************/
void channelimagedata_free ( CHANNELIMAGEDATA *cid ) {
    free ( cid );
}

/******************************************************************************/
static void widthCbk ( GtkWidget *widget, 
                       gpointer   user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    uint32_t width = ( uint32_t ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    CHANNELIMAGEDATA *cid = ( CHANNELIMAGEDATA * ) user_data;
    G3DUI *gui = cid->uvme->gui;

    if ( gui->lock ) return;

    cid->width = width;

    updateChannelImageDialog ( parent, cid );
}

/******************************************************************************/
static void heightCbk ( GtkWidget *widget, 
                        gpointer   user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    uint32_t height = ( uint32_t ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    CHANNELIMAGEDATA *cid = ( CHANNELIMAGEDATA * ) user_data;
    G3DUI *gui = cid->uvme->gui;

    if ( gui->lock ) return;

    cid->height = height;

    updateChannelImageDialog ( parent, cid );
}

/******************************************************************************/
static void okCbk ( GtkWidget *widget, 
                    gpointer   user_data ) {
    CHANNELIMAGEDATA *cid = ( CHANNELIMAGEDATA * ) user_data;
    G3DCHANNEL *chn = ( G3DCHANNEL * ) cid->chn;
    G3DUI *gui = cid->uvme->gui;

    if ( cid->resize ) {
        if ( cid->chn->flags & USEIMAGECOLOR ) {
            if ( chn->image ) {
                g3dimage_resize ( chn->image, 
                                  g3dcore_getNextPowerOfTwo ( cid->width  ), 
                                  g3dcore_getNextPowerOfTwo ( cid->height ) );
            }
        }
    } else {
        if ( chn->image ) {
            /*** Todo: allow undo / redo ? ***/
            g3dimage_free ( chn->image );
        }
    /*** note: image is binded to GL lib in order to view it in the editor ***/
        chn->image = g3dimage_new ( g3dcore_getNextPowerOfTwo ( cid->width ), 
                                    g3dcore_getNextPowerOfTwo ( cid->height ), 
                                    24, 
                                    0x01 );
    }

    g3dchannel_enableImageColor ( chn );

    g3dui_updateMaterialEdit ( gui );

   /*** resize selection mask and zbuffer ***/
   common_g3duiuvmapeditor_resizeBuffers ( cid->uvme );

    gtk_widget_destroy ( gtk_widget_get_ancestor ( widget, GTK_TYPE_WINDOW ) );
}

#ifdef unused
/******************************************************************************/
void g3dui_saveChannelImageAs ( G3DUI      *gui, 
                                G3DCHANNEL *chn ) {
    G3DUIGTK3 *ggt = gui->toolkit_data;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Save image ...",
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
        char           *filename = gtk_file_chooser_get_filename ( chooser );

        /** Bind GL is true because we need to display ***/
        chn->image = g3dimage_load ( filename, 0x01 );

        g_free    ( filename );
    }

    gtk_widget_destroy ( dialog );
}
#endif

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    CHANNELIMAGEDATA *cid = ( CHANNELIMAGEDATA * ) user_data;

    channelimagedata_free ( cid );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    CHANNELIMAGEDATA *cid = ( CHANNELIMAGEDATA * ) user_data;

    updateChannelImageDialog ( widget, cid );
}

/******************************************************************************/
GtkWidget* createChannelImage ( GtkWidget        *parent,
                                G3DUIUVMAPEDITOR *uvme,
                                G3DCHANNEL       *chn,
                                uint32_t          resize,
                                char             *name,
                                gint              x,
                                gint              y,
                                gint              width,
                                gint              height ) {
    CHANNELIMAGEDATA *cid = channelimagedata_new ( uvme, chn, resize );
    GdkRectangle gdkrec = { 0, 0, width, height };
    GtkWidget *frm = gtk_fixed_new ( ), *wbtn, *hbtn;

    gtk_widget_set_name ( frm, name );

    /*gtk_widget_size_allocate ( frm, &gdkrec );*/

    gtk_widget_set_size_request ( frm, gdkrec.width, gdkrec.height );


    wbtn = createIntegerText ( frm, cid, WIDTHBUTTONNAME , 0, 8192, 8,  0,   0, 96, widthCbk  );
    hbtn = createIntegerText ( frm, cid, HEIGHTBUTTONNAME, 0, 8192, 8, 24,   0, 96, heightCbk );

    createSimpleLabel ( frm, uvme->gui, "", 0, 48, 128, 24 );

    createPushButton ( frm, cid, "OK", 60, 72, 32, 24, okCbk );

    if ( cid->resize ) {
        if ( cid->chn->flags & USEIMAGECOLOR ) {
            if ( cid->chn->image ) {
                gtk_spin_button_set_value ( wbtn, cid->chn->image->width  );
                gtk_spin_button_set_value ( hbtn, cid->chn->image->height );
            }
        }
    } else {
        gtk_spin_button_set_value ( wbtn, cid->width  );
        gtk_spin_button_set_value ( hbtn, cid->height );
    }

    uvme->gui->lock = 0x00;

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), cid );
    g_signal_connect ( G_OBJECT (frm), "destroy", G_CALLBACK (Destroy), cid );


    gtk_container_add ( GTK_CONTAINER(parent), frm );

    gtk_widget_show_all ( frm );


    return frm;
}
