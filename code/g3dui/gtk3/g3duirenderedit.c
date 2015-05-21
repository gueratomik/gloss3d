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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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
static void formatCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkComboBoxText *cmbt = GTK_COMBO_BOX_TEXT(widget);
    G3DUI *gui = ( G3DUI * ) user_data;
    const char *str = gtk_combo_box_text_get_active_text ( cmbt );

    common_g3duirenderedit_formatCbk ( gui, str );
}

/******************************************************************************/
void createRenderFormat ( GtkWidget *parent, G3DUI *gui, 
                                             char *name,
                                             gint x, gint y,
                                             gint labwidth,
                                             gint txtwidth,
                                             void (*cbk)( GtkWidget *, 
                                                          gpointer ) ) {
    GtkWidget     *cmb  = gtk_combo_box_text_new ( );
    GdkRectangle   crec = { 0x00, 0x00, txtwidth, 0x12 };

    /*gtk_spin_button_set_numeric ( btn, TRUE );*/

    gtk_widget_set_name ( cmb, name );

    gtk_widget_size_allocate ( cmb, &crec );

    /*gtk_entry_set_width_chars ( GTK_ENTRY(cmb), 18 );*/

    gtk_fixed_put ( GTK_FIXED(parent), cmb, x + labwidth, y );

    if ( labwidth ) {
        GdkRectangle lrec = { 0x00, 0x00, labwidth, 0x12 };
        GtkWidget   *lab  = gtk_label_new ( name );

        gtk_widget_set_name ( lab, name );

        gtk_widget_size_allocate ( lab, &lrec );

        gtk_fixed_put ( GTK_FIXED(parent), lab, x, y );

        gtk_widget_show ( lab );
    }

    if ( cbk ) { 
        g_signal_connect ( cmb, "changed", G_CALLBACK(cbk), gui );
    }

    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, RENDERTOIMAGENAME );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, RENDERTOVIDEONAME );

    gtk_combo_box_set_active ( GTK_COMBO_BOX(cmb), 0x00 );

    gtk_widget_show ( cmb );
}

/******************************************************************************/
/*** This filter is declared in the g3dui layer because of GtkWidget struct***/
R3DFILTER *r3dfilter_toGtkWidget_new ( GtkWidget *widget, uint32_t active_fill ) {
    GdkDisplay *gdkdpy   = gtk_widget_get_display ( widget );
    GdkWindow  *gdkwin   = gtk_widget_get_window  ( widget );
    R3DFILTER *fil;
#ifdef __linux__
    Display    *dis      = gdk_x11_display_get_xdisplay ( gdkdpy );
    Window      win      = gdk_x11_window_get_xid ( gdkwin );


    fil = r3dfilter_new ( FILTERLINE, TOWINDOWFILTERNAME,
                                      filtertowindow_draw,
                                      filtertowindow_free, 
                                      filtertowindow_new ( dis, win, active_fill ) );
#endif

#ifdef __MINGW32__
    HWND hWnd = GDK_WINDOW_HWND ( gdkwin );

    fil = r3dfilter_new ( FILTERLINE, TOWINDOWFILTERNAME,
                                      filtertowindow_draw,
                                      filtertowindow_free, 
                                      filtertowindow_new ( hWnd, active_fill ) );
#endif

    return fil;
}

/******************************************************************************/
static void saveCbk ( GtkWidget *widget, gpointer user_data ) {
    uint32_t save = gtk_toggle_button_get_state ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_saveCbk ( gui, save );

    updateRenderEdit ( parent, gui );
}


/******************************************************************************/
static void previewCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duirenderedit_previewCbk ( gui );
}

/******************************************************************************/
static void startFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float frame = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_startFrameCbk ( gui, frame );

    updateRenderEdit ( parent, gui );
}

/******************************************************************************/
static void fpsCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float fps = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    common_g3duirenderedit_fpsCbk ( gui, fps );
}

/******************************************************************************/
static void endFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float frame = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_endFrameCbk ( gui, frame );

    updateRenderEdit ( parent, gui );
}

/******************************************************************************/
static void ratioCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float ratio = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_ratioCbk ( gui, ratio );

    updateRenderEdit ( parent, gui );
}

/******************************************************************************/
static void widthCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    int width = (int) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    common_g3duirenderedit_widthCbk ( gui, ( uint32_t ) width );
}

/******************************************************************************/
static void heightCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    int height = (int) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    common_g3duirenderedit_heightCbk ( gui, ( uint32_t ) height );
}

/******************************************************************************/
static void outputCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    const char *outfile = gtk_entry_get_text ( GTK_ENTRY(widget) );

    common_g3duirenderedit_outputCbk ( gui, outfile );
}

/******************************************************************************/
static void motionBlurCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    int nbstep = (int) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    common_g3duirenderedit_motionBlurCbk ( gui, ( uint32_t ) nbstep );
}

/******************************************************************************/
static void backgroundCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    GdkRGBA color;

    gtk_color_chooser_get_rgba ( GTK_COLOR_CHOOSER(widget), &color );

    common_g3duirenderedit_backgroundCbk ( gui, ( color.red   * 255 ), 
                                                ( color.green * 255 ),
                                                ( color.blue  * 255 ) );
}

/******************************************************************************/
static void Configure ( GtkWidget *widget, GdkEvent *event, 
                                           gpointer user_data ) {
    GtkWidget *child = gtk_bin_get_child ( GTK_BIN(widget) );
    GdkRectangle gdkrec;

    if ( gtk_widget_get_realized ( widget ) ) {
        gdkrec.x      = 0;
        gdkrec.y      = 0;
        gdkrec.width  = event->configure.width;
        gdkrec.height = event->configure.height;

        gtk_widget_size_allocate ( child, &gdkrec );
    }
}

/******************************************************************************/
void g3dui_runRenderCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIRENDERSETTINGS *rsg = ( G3DUIRENDERSETTINGS * ) gui->currsg;
    float ratio = ( rsg->ratio ) ? rsg->ratio : gui->curcam->ratio;
    uint32_t rsgwidth  = rsg->height * ratio;
    uint32_t rsgheight = rsg->height;

    gtk_widget_add_events(GTK_WIDGET(dial), GDK_CONFIGURE);

    g_signal_connect ( G_OBJECT (dial), "configure-event", G_CALLBACK (Configure), gui );

    createRenderWindow ( dial, gui, "RENDER WINDOW", 0, 0, rsgwidth,
                                                           rsgheight );

    gtk_widget_show ( dial );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
}

/******************************************************************************/
void updateRenderEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    /*** prevents a loop ***/
    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( gui->currsg ) {
            G3DUIRENDERSETTINGS *rsg = gui->currsg;

            if ( GTK_IS_CHECK_BUTTON(child) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITRENDERSAVE ) == 0x00 ) {
                    if ( rsg->flags & RENDERSAVE ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }

                if ( strcmp ( child_name, EDITRENDERPREVIEW ) == 0x00 ) {
                    if ( rsg->flags & RENDERPREVIEW ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }
            }

            if ( GTK_IS_ENTRY(child) ) {
                GtkEntry *ent = GTK_ENTRY(child);

                if ( strcmp ( child_name, EDITRENDEROUTPUT ) == 0x00 ) {
                    if ( rsg->flags & RENDERSAVE ) {
                        gtk_widget_set_sensitive ( child, TRUE );
                    } else {
                        gtk_widget_set_sensitive ( child, FALSE );
                    }

                    gtk_entry_set_text ( ent, rsg->outfile );
                }
            }

            if ( GTK_IS_SPIN_BUTTON(child) ) {
                GtkSpinButton *sbn = GTK_SPIN_BUTTON(child);

                if ( strcmp ( child_name, EDITRENDERSTART ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, rsg->startframe );
                }

                if ( strcmp ( child_name, EDITRENDEREND ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, rsg->endframe );
                }

                if ( strcmp ( child_name, EDITRENDERMBLUR ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, rsg->height );
                }

                if ( strcmp ( child_name, EDITRENDERRATIO ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, rsg->ratio );
                }

                if ( strcmp ( child_name, EDITRENDERFPS ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, rsg->fps );
                }

                if ( strcmp ( child_name, EDITRENDERWIDTH ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, rsg->width );
                }

                if ( strcmp ( child_name, EDITRENDERHEIGHT ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, rsg->height );
                }


            }

            if ( GTK_IS_COMBO_BOX_TEXT(child) ) {
                GtkComboBox *cmb = GTK_COMBO_BOX(child);

                if ( strcmp ( child_name, EDITRENDERFORMAT   ) == 0x00 ) {
                    if ( rsg->flags & RENDERSAVE ) {
                        gtk_widget_set_sensitive ( child, TRUE );
                    } else {
                        gtk_widget_set_sensitive ( child, FALSE );
                    }

                    gtk_combo_box_set_active ( cmb, rsg->format );
                }
            }

            if ( GTK_IS_COLOR_BUTTON(child) ) {
                GtkColorChooser *ccr = GTK_COLOR_CHOOSER(child);

                if ( strcmp ( child_name, EDITRENDERBACKGROUND ) == 0x00 ) {
                    unsigned char R = ( rsg->background & 0x00FF0000 ) >> 0x10,
                                  G = ( rsg->background & 0x0000FF00 ) >> 0x08,
                                  B = ( rsg->background & 0x000000FF );
                    GdkRGBA rgba = { .red   = ( float ) R / 255,
                                     .green = ( float ) G / 255,
                                     .blue  = ( float ) B / 255,
                                     .alpha = 1.0f };

                    gtk_color_chooser_set_rgba ( ccr, &rgba );
                }
            }
        }

        children =  g_list_next ( children );
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    updateRenderEdit ( widget, gui );
}

/******************************************************************************/
GtkWidget* createRenderEdit ( GtkWidget *parent, G3DUI *gui,
                                                 char *name,
                                                 gint x,
                                                 gint y,
                                                 gint width,
                                                 gint height ) {
    GdkRectangle gdkrec = { 0, 0, width, height };
    GtkWidget *frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    /*gtk_widget_size_allocate ( frm, &gdkrec );*/

    gtk_widget_set_size_request ( frm, gdkrec.width, gdkrec.height );

    /*** For some reason, GtkSpinButtons calls its callbacks ***/
    /*** when being realized. With this trick I bypass that. ***/
    /*** Callbacks will return prematurely if gui->lock == 0x01 ***/
    gui->lock = 0x01;

    createToggleLabel ( frm, gui, EDITRENDERSAVE,
                               0,   0, 104, 20, saveCbk );

    /*createToggleLabel ( frm, gui, EDITRENDERPREVIEW,
                               0,  24, 104, 20, previewCbk );*/

    createIntegerText ( frm, gui, EDITRENDERSTART,
                               0,  48, 96,  32, startFrameCbk );

    createIntegerText ( frm, gui, EDITRENDEREND,
                               0,  72, 96,  32, endFrameCbk );

    createIntegerText ( frm, gui, EDITRENDERFPS,
                               0,  96, 96,  32, fpsCbk );

    createIntegerText ( frm, gui, EDITRENDERWIDTH,
                               0, 120, 96,  32, widthCbk );

    createIntegerText ( frm, gui, EDITRENDERHEIGHT,
                               0, 144, 96,  32, heightCbk );

    createFloatText   ( frm, gui, EDITRENDERRATIO,
                               0, 168, 96,  64, ratioCbk );

    createRenderFormat( frm, gui, EDITRENDERFORMAT,
                               0, 192, 96,  64, formatCbk );

    createCharText    ( frm, gui, EDITRENDEROUTPUT,
                               0, 216, 96, 200, outputCbk );

    createIntegerText ( frm, gui, EDITRENDERMBLUR,
                               0, 240, 96,  64, motionBlurCbk );

    createSimpleLabel ( frm, gui, EDITRENDERBACKGROUND,
                               0, 264, 96, 20 );

    createColorButton ( frm, gui, EDITRENDERBACKGROUND,
                              96, 264, 96, 18, backgroundCbk );

    createPushButton  ( frm, gui, EDITRENDERRUN,
                              96, 288, 48, 20, g3dui_runRenderCbk );


    gui->lock = 0x00;

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), gui );
    g_signal_connect ( G_OBJECT (frm), "destroy", G_CALLBACK (Destroy), gui );

    gtk_container_add ( GTK_CONTAINER(parent), frm );

    gtk_widget_show_all ( frm );


    return frm;
}

