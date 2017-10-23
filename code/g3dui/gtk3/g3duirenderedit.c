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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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

    gtk_combo_box_set_active ( GTK_COMBO_BOX(cmb), gui->currsg->output.format );

    gtk_widget_show ( cmb );
}

/******************************************************************************/
/*** This filter is declared in the g3dui layer because of GtkWidget struct***/
R3DFILTER *r3dfilter_toGtkWidget_new ( GtkWidget *widget, uint32_t active_fill ) {
    GdkDisplay *gdkdpy   = gtk_widget_get_display ( widget );
    GdkWindow  *gdkwin   = gtk_widget_get_window  ( widget );
    R3DFILTER *fil;
    uint32_t filterMode =  FILTERLINE | FILTERIMAGE;
#ifdef __linux__
    Display    *dis      = gdk_x11_display_get_xdisplay ( gdkdpy );
    Window      win      = gdk_x11_window_get_xid ( gdkwin );


    fil = r3dfilter_new ( filterMode, TOWINDOWFILTERNAME,
                                      filtertowindow_draw,
                                      filtertowindow_free, 
                                      filtertowindow_new ( dis, win, active_fill ) );
#endif

#ifdef __MINGW32__
    HWND hWnd = GDK_WINDOW_HWND ( gdkwin );

    fil = r3dfilter_new ( filterMode, TOWINDOWFILTERNAME,
                                      filtertowindow_draw,
                                      filtertowindow_free, 
                                      filtertowindow_new ( hWnd, active_fill ) );
#endif

    return fil;
}

/******************************************************************************/
static void saveCbk ( GtkWidget *widget, gpointer user_data ) {
    uint32_t save = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget));
    G3DUI *gui = ( G3DUI * ) user_data;
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_saveCbk ( gui, save );

    gui->lock = 0x01;
    updateSaveOutputForm ( parent, gui );
    gui->lock = 0x00;
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

    gui->lock = 1;
    updateGeneralPanel ( parent, gui );
    gui->lock = 0;
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

    gui->lock = 1;
    updateGeneralPanel ( parent, gui );
    gui->lock = 0;
}

/******************************************************************************/
static void ratioCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float ratio = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_ratioCbk ( gui, ratio );

    gui->lock = 1;
    updateGeneralPanel ( parent, gui );
    gui->lock = 0;
}

/******************************************************************************/
static void widthCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    int width = (int) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_widthCbk ( gui, ( uint32_t ) width );

    gui->lock = 1;
    updateGeneralPanel ( parent, gui );
    gui->lock = 0;
}

/******************************************************************************/
static void heightCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    int height = (int) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_heightCbk ( gui, ( uint32_t ) height );

    gui->lock = 1;
    updateGeneralPanel ( parent, gui );
    gui->lock = 0;
}

/******************************************************************************/
static void outputCbk ( GtkWidget *widget, GdkEvent *event, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    const char *outfile = gtk_entry_get_text ( GTK_ENTRY(widget) );

    common_g3duirenderedit_outputCbk ( gui, outfile );
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
static void wireframeColorCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    GdkRGBA color;

    gtk_color_chooser_get_rgba ( GTK_COLOR_CHOOSER(widget), &color );

    common_g3duirenderedit_wireframeColorCbk ( gui, ( color.red   * 255 ), 
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
    G3DUI *gui = ( G3DUI * ) user_data;
    R3DRENDERSETTINGS *rsg = ( R3DRENDERSETTINGS * ) gui->currsg;
    G3DCAMERA *mainCamera = g3dui_getMainViewCamera ( gui );

    if ( mainCamera ) {
        GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );

        gtk_widget_add_events(GTK_WIDGET(dial), GDK_CONFIGURE);

        g_signal_connect ( G_OBJECT (dial), "configure-event", G_CALLBACK (Configure), gui );

        createRenderWindow ( dial, gui, "RENDER WINDOW", 0, 0, rsg->output.width,
                                                               rsg->output.height );

        gtk_widget_show ( dial );
    }
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
}

/******************************************************************************/
static void setMotionBlurCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duirenderedit_setMotionBlurCbk ( gui );

    gui->lock = 0x01;
    updateMotionBlurForm ( gtk_widget_get_parent ( widget ), gui );
    gui->lock = 0x00;
}

/******************************************************************************/
static void sceneMotionBlurCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duirenderedit_sceneMotionBlurCbk ( gui );

    gui->lock = 0x01;
    updateMotionBlurForm ( gtk_widget_get_parent ( widget ), gui );
    gui->lock = 0x00;
}

/******************************************************************************/
static void sceneMotionBlurIterationCbk ( GtkWidget *widget, gpointer user_data ) {
    uint32_t iteration = gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duirenderedit_sceneMotionBlurIterationCbk ( gui, iteration );
}

/******************************************************************************/
static void vectorMotionBlurCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duirenderedit_vectorMotionBlurCbk ( gui );

    gui->lock = 0x01;
    updateMotionBlurForm ( gtk_widget_get_parent ( widget ), gui );
    gui->lock = 0x00;
}

/******************************************************************************/
static void motionBlurStrengthCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float strength = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_motionBlurStrengthCbk ( gui, strength );
}

/******************************************************************************/
static void vectorMotionBlurSamplesCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    uint32_t samples = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_vectorMotionBlurSamplesCbk ( gui, samples );
}

/******************************************************************************/
static void vectorMotionBlurSubSamplingRateCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float rate = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_vectorMotionBlurSubSamplingRateCbk ( gui, rate / 100.0f );
}

/******************************************************************************/
void updateSaveOutputForm ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( gui->currsg ) {
            R3DRENDERSETTINGS *rsg = gui->currsg;

            if ( GTK_IS_TOGGLE_BUTTON(child) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITRENDERSAVE ) == 0x00 ) {
                    if ( rsg->flags & RENDERSAVE ) {
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

                    gtk_entry_set_text ( ent, rsg->output.outfile );
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

                    gtk_combo_box_set_active ( cmb, rsg->output.format );
                }
            }
        }

        children =  g_list_next ( children );
    }
}

/******************************************************************************/
void updateSaveOutputFrame ( GtkWidget *widget, G3DUI *gui ) {
    GtkWidget *frm = gtk_bin_get_child ( GTK_BIN(widget) );

    gui->lock = 0x01;
    if ( frm ) updateSaveOutputForm ( frm, gui );
    gui->lock = 0x00;
}

/******************************************************************************/
static GtkWidget *createSaveOutputForm ( GtkWidget *parent, G3DUI *gui,
                                                             char *name,
                                                             gint x,
                                                             gint y,
                                                             gint width,
                                                             gint height ) {
    GtkWidget *vbr, *col, *frm, *btn;

    frm = createFrame ( parent, gui, name, x, y, width, height );

    createToggleLabel ( frm, gui, EDITRENDERSAVE,
                               0,  0, 104, 20, saveCbk );

    createRenderFormat( frm, gui, EDITRENDERFORMAT,
                               0, 24, 96,  64, formatCbk );

    createCharText    ( frm, gui, EDITRENDEROUTPUT,
                               0, 48, 96, 200, outputCbk );


    return frm;
}


/******************************************************************************/
void updateMotionBlurForm ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( gui->currsg ) {
            R3DRENDERSETTINGS *rsg = gui->currsg;

            if ( GTK_IS_CHECK_BUTTON(child) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITRENDERENABLEMOTIONBLUR ) == 0x00 ) {
                    if ( rsg->flags & ENABLEMOTIONBLUR ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }
            }

            if ( GTK_IS_RADIO_BUTTON(child) ) {
                GtkEntry *ent = GTK_ENTRY(child);

                if ( strcmp ( child_name, EDITRENDERVECTORMOTIONBLUR ) == 0x00 ) {
                    if ( rsg->flags & VECTORMOTIONBLUR ) {
                        gtk_toggle_button_set_active ( child, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( child, FALSE );
                    }

                    if ( ( rsg->flags & ENABLEMOTIONBLUR ) ) {
                        gtk_widget_set_sensitive ( child, TRUE );
                    } else {
                        gtk_widget_set_sensitive ( child, FALSE );
                    }
                }

                if ( strcmp ( child_name, EDITRENDERSCENEMOTIONBLUR ) == 0x00 ) {
                    if ( rsg->flags & SCENEMOTIONBLUR ) {
                        gtk_toggle_button_set_active ( child, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( child, FALSE );
                    }

                    if ( ( rsg->flags & ENABLEMOTIONBLUR ) ) {
                        gtk_widget_set_sensitive ( child, TRUE );
                    } else {
                        gtk_widget_set_sensitive ( child, FALSE );
                    }
                }
            }

            if ( GTK_IS_SPIN_BUTTON(child) ) {
                GtkSpinButton *sbn = GTK_SPIN_BUTTON(child);

                if ( strcmp ( child_name, EDITRENDERSCENEMOTIONBLURITERATION ) == 0x00 ) {
                    if ( ( rsg->flags & ENABLEMOTIONBLUR ) && 
                         ( rsg->flags & SCENEMOTIONBLUR  ) ) {
                        gtk_widget_set_sensitive ( child, TRUE );
                    } else {
                        gtk_widget_set_sensitive ( child, FALSE );
                    }

                    gtk_spin_button_set_value ( sbn, rsg->motionBlur.iterations );
                }

                if ( strcmp ( child_name, EDITRENDERMOTIONBLURSTRENGTH ) == 0x00 ) {
                    if ( ( rsg->flags & ENABLEMOTIONBLUR ) ) {
                        gtk_widget_set_sensitive ( child, TRUE );
                    } else {
                        gtk_widget_set_sensitive ( child, FALSE );
                    }

                    gtk_spin_button_set_value ( sbn, rsg->motionBlur.strength );
                }

                if ( strcmp ( child_name, EDITRENDERVECTORMOTIONBLURSAMPLES ) == 0x00 ) {
                    if ( ( rsg->flags & ENABLEMOTIONBLUR ) &&
                         ( rsg->flags & VECTORMOTIONBLUR ) ) {
                        gtk_widget_set_sensitive ( child, TRUE );
                    } else {
                        gtk_widget_set_sensitive ( child, FALSE );
                    }

                    gtk_spin_button_set_value ( sbn, rsg->motionBlur.vMotionBlurSamples );
                }

                if ( strcmp ( child_name, EDITRENDERVECTORMOTIONBLURSUBSAMPLINGRATE ) == 0x00 ) {
                    if ( ( rsg->flags & ENABLEMOTIONBLUR ) &&
                         ( rsg->flags & VECTORMOTIONBLUR ) ) {
                        gtk_widget_set_sensitive ( child, TRUE );
                    } else {
                        gtk_widget_set_sensitive ( child, FALSE );
                    }

                    gtk_spin_button_set_value ( sbn, rsg->motionBlur.vMotionBlurSubSamplingRate * 100 );
                }
            }
        }

        children =  g_list_next ( children );
    }
}

/******************************************************************************/
void updateMotionBlurFrame ( GtkWidget *widget, G3DUI *gui ) {
    GtkWidget *frm = gtk_bin_get_child ( GTK_BIN(widget) );

    gui->lock = 0x01;
    if ( frm ) updateMotionBlurForm ( frm, gui );
    gui->lock = 0x00;
}

/******************************************************************************/
static GtkWidget *createMotionBlurForm ( GtkWidget *parent, G3DUI *gui,
                                                             char *name,
                                                             gint x,
                                                             gint y,
                                                             gint width,
                                                             gint height ) {
    GtkWidget *vbr, *col, *frm, *btn;

    frm = createFrame ( parent, gui, name, x, y, width, height );

          createToggleLabel ( frm, gui,
                                   EDITRENDERENABLEMOTIONBLUR,
                                     0,  0, 96, 18,
                                   setMotionBlurCbk );

          createIntegerText ( frm, gui, EDITRENDERMOTIONBLURSTRENGTH,
                                      0, 100,
                                    160, 0, 96,  32,
                                   motionBlurStrengthCbk );

    btn = createRadioLabel ( frm, gui,
                                   EDITRENDERVECTORMOTIONBLUR,
                                   NULL,
                                     0,  24, 96, 18,
                                   vectorMotionBlurCbk );

          createIntegerText ( frm, gui, EDITRENDERVECTORMOTIONBLURSAMPLES,
                                      0, 100,
                                    160, 24, 96,  32,
                                   vectorMotionBlurSamplesCbk );

          createIntegerText ( frm, gui, EDITRENDERVECTORMOTIONBLURSUBSAMPLINGRATE,
                                      0, 100,
                                    160, 48, 96,  32,
                                   vectorMotionBlurSubSamplingRateCbk );

          createRadioLabel ( frm, gui,
                                   EDITRENDERSCENEMOTIONBLUR,
                                   btn,
                                     0, 72, 96, 18,
                                   sceneMotionBlurCbk );

          createIntegerText ( frm, gui, EDITRENDERSCENEMOTIONBLURITERATION,
                                      5, 31,
                                    160, 72, 96,  32,
                                   sceneMotionBlurIterationCbk );


    return frm;
}

/******************************************************************************/
static void updateWireframeForm ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( gui->currsg ) {
            R3DRENDERSETTINGS *rsg = gui->currsg;

            if ( GTK_IS_CHECK_BUTTON(child) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITRENDERWIREFRAME ) == 0x00 ) {
                    if ( rsg->flags & RENDERWIREFRAME ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }

                if ( strcmp ( child_name, EDITRENDERWIREFRAMELIGHTING ) == 0x00 ) {
                    if ( ( rsg->flags & RENDERWIREFRAME ) ) {
                        gtk_widget_set_sensitive ( child, TRUE );
                    } else {
                        gtk_widget_set_sensitive ( child, FALSE );
                    }

                    if ( rsg->flags & WIREFRAMELIGHTING ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }
            }

            if ( GTK_IS_SPIN_BUTTON(child) ) {
                GtkSpinButton *sbn = GTK_SPIN_BUTTON(child);

                if ( strcmp ( child_name, EDITRENDERWIREFRAMETHICKNESS ) == 0x00 ) {
                    if ( ( rsg->flags & RENDERWIREFRAME ) ) {
                        gtk_widget_set_sensitive ( child, TRUE );
                    } else {
                        gtk_widget_set_sensitive ( child, FALSE );
                    }

                    gtk_spin_button_set_value ( sbn, rsg->wireframe.thickness );
                }
            }

            if ( GTK_IS_COLOR_BUTTON(child) ) {
                GtkColorChooser *ccr = GTK_COLOR_CHOOSER(child);

                if ( ( rsg->flags & RENDERWIREFRAME ) ) {
                    gtk_widget_set_sensitive ( child, TRUE );
                } else {
                    gtk_widget_set_sensitive ( child, FALSE );
                }

                if ( strcmp ( child_name, EDITRENDERWIREFRAMECOLOR ) == 0x00 ) {
                    unsigned char R = ( rsg->wireframe.color & 0x00FF0000 ) >> 0x10,
                                  G = ( rsg->wireframe.color & 0x0000FF00 ) >> 0x08,
                                  B = ( rsg->wireframe.color & 0x000000FF );
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
}

/******************************************************************************/
static void updateWireframeFrame ( GtkWidget *widget, G3DUI *gui ) {
    GtkWidget *frm = gtk_bin_get_child ( GTK_BIN(widget) );

    gui->lock = 0x01;
    if ( frm ) updateWireframeForm ( frm, gui );
    gui->lock = 0x00;
}

/******************************************************************************/
static void setWireframeCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duirenderedit_setWireframeCbk ( gui );

    gui->lock = 0x01;
    updateWireframeForm ( gtk_widget_get_parent ( widget ), gui );
    gui->lock = 0x00;
}

/******************************************************************************/
static void setWireframeLightingCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duirenderedit_setWireframeLightingCbk ( gui );

    gui->lock = 0x01;
    updateWireframeForm ( gtk_widget_get_parent ( widget ), gui );
    gui->lock = 0x00;
}

/******************************************************************************/
static void wireframeThicknessCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float thickness = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_wireframeThicknessCbk ( gui, thickness );
}

/******************************************************************************/
static GtkWidget *createWireframeForm ( GtkWidget *parent, G3DUI *gui,
                                                         char *name,
                                                         gint x,
                                                         gint y,
                                                         gint width,
                                                         gint height ) {
    GtkWidget *vbr, *col, *frm, *btn;

    frm = createFrame ( parent, gui, name, x, y, width, height );

    gui->lock = 0x01;

          createToggleLabel ( frm, gui,
                                   EDITRENDERWIREFRAME,
                                     0,  0, 96, 18,
                                   setWireframeCbk );

          createToggleLabel ( frm, gui,
                                   EDITRENDERWIREFRAMELIGHTING,
                                     0, 24, 96, 18,
                                   setWireframeLightingCbk );

          createFloatText ( frm, gui, EDITRENDERWIREFRAMETHICKNESS,
                                     0.0f, FLT_MAX,
                                     0, 48, 96,  48,
                                   wireframeThicknessCbk );

          createSimpleLabel ( frm, gui, EDITRENDERWIREFRAMECOLOR,
                                     0, 72, 96, 20 );

          createColorButton ( frm, gui, EDITRENDERWIREFRAMECOLOR,
                                     96, 72, 96, 18, wireframeColorCbk );

    gui->lock = 0x00;

    return frm;
}

/******************************************************************************/
static void updateFogForm ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( gui->currsg ) {
            R3DRENDERSETTINGS *rsg = gui->currsg;

            if ( GTK_IS_CHECK_BUTTON(child) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITRENDERFOG ) == 0x00 ) {
                    if ( rsg->flags & RENDERFOG ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }

                if ( strcmp ( child_name, EDITRENDERFOGAFFECTSBACKGROUND ) == 0x00 ) {
                    if ( ( rsg->flags & RENDERFOG ) ) {
                        gtk_widget_set_sensitive ( child, TRUE );
                    } else {
                        gtk_widget_set_sensitive ( child, FALSE );
                    }

                    if ( rsg->fog.flags & FOGAFFECTSBACKGROUND ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }
            }

            if ( GTK_IS_SPIN_BUTTON(child) ) {
                GtkSpinButton *sbn = GTK_SPIN_BUTTON(child);

                if ( strcmp ( child_name, EDITRENDERFOGNEAR ) == 0x00 ) {
                    if ( ( rsg->flags & RENDERFOG ) ) {
                        gtk_widget_set_sensitive ( child, TRUE );
                    } else {
                        gtk_widget_set_sensitive ( child, FALSE );
                    }

                    gtk_spin_button_set_value ( sbn, rsg->fog.fnear );
                }

                if ( strcmp ( child_name, EDITRENDERFOGFAR ) == 0x00 ) {
                    if ( ( rsg->flags & RENDERFOG ) ) {
                        gtk_widget_set_sensitive ( child, TRUE );
                    } else {
                        gtk_widget_set_sensitive ( child, FALSE );
                    }

                    gtk_spin_button_set_value ( sbn, rsg->fog.ffar );
                }
            }

            if ( GTK_IS_COLOR_BUTTON(child) ) {
                GtkColorChooser *ccr = GTK_COLOR_CHOOSER(child);

                if ( ( rsg->flags & RENDERFOG ) ) {
                    gtk_widget_set_sensitive ( child, TRUE );
                } else {
                    gtk_widget_set_sensitive ( child, FALSE );
                }

                if ( strcmp ( child_name, EDITRENDERFOGCOLOR ) == 0x00 ) {
                    unsigned char R = ( rsg->fog.color & 0x00FF0000 ) >> 0x10,
                                  G = ( rsg->fog.color & 0x0000FF00 ) >> 0x08,
                                  B = ( rsg->fog.color & 0x000000FF );
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
}

/******************************************************************************/
static void updateFogFrame ( GtkWidget *widget, G3DUI *gui ) {
    GtkWidget *frm = gtk_bin_get_child ( GTK_BIN(widget) );

    gui->lock = 0x01;
    if ( frm ) updateFogForm ( frm, gui );
    gui->lock = 0x00;
}

/******************************************************************************/
static void setFogAffectsBackgroundCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duirenderedit_setFogAffectsBackgroundCbk ( gui );
}

/******************************************************************************/
static void setFogCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duirenderedit_setFogCbk ( gui );

    gui->lock = 0x01;
    updateFogForm ( gtk_widget_get_parent ( widget ), gui );
    gui->lock = 0x00;
}

/******************************************************************************/
static void fogNearCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float fnear = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_fogNearCbk ( gui, fnear );
}

/******************************************************************************/
static void fogFarCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float ffar = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_fogFarCbk ( gui, ffar );
}

/******************************************************************************/
static void fogColorCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    GdkRGBA color;

    gtk_color_chooser_get_rgba ( GTK_COLOR_CHOOSER(widget), &color );

    common_g3duirenderedit_fogColorCbk ( gui, ( color.red   * 255 ), 
                                              ( color.green * 255 ),
                                              ( color.blue  * 255 ) );
}

/******************************************************************************/
static GtkWidget *createFogForm ( GtkWidget *parent, 
                                  G3DUI     *gui,
                                  char      *name,
                                  gint       x,
                                  gint       y,
                                  gint       width,
                                  gint       height ) {
    GtkWidget *vbr, *col, *frm, *btn;

    frm = createFrame ( parent, gui, name, x, y, width, height );

    gui->lock = 0x01;

          createToggleLabel ( frm, gui,
                                   EDITRENDERFOG,
                                     0,  0, 96, 18,
                                   setFogCbk );

          createToggleLabel ( frm, gui,
                                   EDITRENDERFOGAFFECTSBACKGROUND,
                                     0, 24, 96, 18,
                                   setFogAffectsBackgroundCbk );

          createFloatText ( frm, gui, EDITRENDERFOGNEAR,
                                     0.0f, FLT_MAX,
                                     0, 48, 96,  48,
                                   fogNearCbk );

          createFloatText ( frm, gui, EDITRENDERFOGFAR,
                                     0.0f, FLT_MAX,
                                     0, 72, 96,  48,
                                   fogFarCbk );

          createSimpleLabel ( frm, gui, EDITRENDERFOGCOLOR,
                                     0, 96, 96, 20 );

          createColorButton ( frm, gui, EDITRENDERFOGCOLOR,
                                    96, 96, 96, 18, fogColorCbk );

    gui->lock = 0x00;

    return frm;
}


/******************************************************************************/
static void updateBackgroundForm ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( gui->currsg ) {
            R3DRENDERSETTINGS *rsg = gui->currsg;
            G3DSYSINFO *sysinfo = g3dsysinfo_get ( );

            if ( GTK_IS_RADIO_BUTTON(child) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITRENDERBACKGROUNDCOLOR ) == 0x00 ) {
                    if ( rsg->background.mode & BACKGROUND_IMAGE ) {
                        gtk_toggle_button_set_active ( tbn, FALSE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, TRUE );
                    }
                }

                if ( strcmp ( child_name, EDITRENDERBACKGROUNDIMAGE ) == 0x00 ) {
                    if ( ( rsg->background.mode & BACKGROUND_IMAGE ) ) {
                        gtk_toggle_button_set_active ( tbn, TRUE );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }
            }

            if ( GTK_IS_BUTTON(child) && (GTK_IS_RADIO_BUTTON(child) == FALSE) ) {
                if ( strcmp ( child_name, EDITRENDERBACKGROUNDIMAGE ) == 0x00 ) {
                    if ( rsg->background.image && 
                         rsg->background.image->filename ) {
                        char *imgpath, *imgname;

                        imgpath = strdup ( rsg->background.image->filename );

                        /*** We just keep the image name, not the whole ***/
                        /*** path and display it as the button label.   ***/
                        imgname = basename ( imgpath );

                        gtk_button_set_label ( child, imgname );

                        free ( imgpath );
                    }
                }
            }

            if ( GTK_IS_COLOR_BUTTON(child) ) {
                GtkColorChooser *ccr = GTK_COLOR_CHOOSER(child);

                if ( strcmp ( child_name, EDITRENDERBACKGROUNDCOLOR ) == 0x00 ) {
                    unsigned char R = ( rsg->background.color & 0x00FF0000 ) >> 0x10,
                                  G = ( rsg->background.color & 0x0000FF00 ) >> 0x08,
                                  B = ( rsg->background.color & 0x000000FF );
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
}

/******************************************************************************/
static void updateBackgroundFrame ( GtkWidget *widget, G3DUI *gui ) {
    GtkWidget *frm = gtk_bin_get_child ( GTK_BIN(widget) );

    gui->lock = 0x01;
    if ( frm ) updateBackgroundForm ( frm, gui );
    gui->lock = 0x00;
}

/******************************************************************************/
static void setBackgroundImageCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt = gui->toolkit_data;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Open File",
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
        const char     *filename = gtk_file_chooser_get_filename ( chooser );

        common_g3duirenderedit_setBackgroundImageCbk ( gui, filename );

        g_free    ( ( gpointer ) filename );
    }

    gui->lock = 0x01;
    updateBackgroundForm ( gtk_widget_get_parent ( widget ), gui );
    gui->lock = 0x00;

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
static void setBackgroundColorModeCbk ( GtkWidget *widget, 
                                        gpointer   user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duirenderedit_setBackgroundColorModeCbk ( gui );
}

/******************************************************************************/
static void setBackgroundImageModeCbk ( GtkWidget *widget, 
                                        gpointer   user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duirenderedit_setBackgroundImageModeCbk ( gui );
}

/******************************************************************************/
static GtkWidget *createBackgroundForm ( GtkWidget *parent, G3DUI *gui,
                                                            char *name,
                                                            gint x,
                                                            gint y,
                                                            gint width,
                                                            gint height ) {
    GtkWidget *vbr, *col, *frm, *btn;

    frm = createFrame ( parent, gui, name, x, y, width, height );

    gui->lock = 0x01;

    btn = createRadioLabel ( frm, gui,
                             EDITRENDERBACKGROUNDCOLOR,
                             NULL,
                               0,  0, 96, 18,
                             setBackgroundColorModeCbk );

          createRadioLabel ( frm, gui,
                                  EDITRENDERBACKGROUNDIMAGE,
                                  btn,
                                    0, 24, 96, 18,
                                  setBackgroundImageModeCbk );

          createColorButton ( frm, gui, EDITRENDERBACKGROUNDCOLOR,
                                   160, 0, 96, 18, backgroundCbk );

          createPushButton   ( frm, gui, EDITRENDERBACKGROUNDIMAGE,
                                   160, 24, 
                                     96,  18, setBackgroundImageCbk );

    gui->lock = 0x00;

    return frm;
}

/******************************************************************************/
void updateGeneralPanel ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( gui->currsg ) {
            R3DRENDERSETTINGS *rsg = gui->currsg;

            if ( strcmp ( child_name, EDITRENDERSAVEOUTPUTFRAME ) == 0x00 ) {
                updateSaveOutputFrame ( child, gui );
            }

            if ( strcmp ( child_name, EDITRENDERBACKGROUNDFRAME ) == 0x00 ) {
                updateBackgroundFrame ( child, gui );
            }

            if ( GTK_IS_CHECK_BUTTON(child) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITRENDERPREVIEW ) == 0x00 ) {
                    if ( rsg->flags & RENDERPREVIEW ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }
            }

            if ( GTK_IS_SPIN_BUTTON(child) ) {
                GtkSpinButton *sbn = GTK_SPIN_BUTTON(child);

                if ( strcmp ( child_name, EDITRENDERSTART ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, rsg->output.startframe );
                }

                if ( strcmp ( child_name, EDITRENDEREND ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, rsg->output.endframe );
                }

                if ( strcmp ( child_name, EDITRENDERRATIO ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, rsg->output.ratio );
                }

                if ( strcmp ( child_name, EDITRENDERFPS ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, rsg->output.fps );
                }

                if ( strcmp ( child_name, EDITRENDERWIDTH ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, rsg->output.width );
                }

                if ( strcmp ( child_name, EDITRENDERHEIGHT ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, rsg->output.height );
                }
            }
        }

        children =  g_list_next ( children );
    }
}

/******************************************************************************/
void createGeneralPanel ( GtkWidget *parent, 
                          G3DUI     *gui,
                          char      *name,
                          gint       x,
                          gint       y,
                          gint       width,
                          gint       height ) {
    GtkWidget *pan;

    pan = createPanel ( parent, gui, name, x, y, width, height );

    createToggleLabel ( pan, gui, EDITRENDERPREVIEW,
                               0,  0, 104, 20, previewCbk );

    createIntegerText ( pan, gui, EDITRENDERSTART,
                               INT_MIN, INT_MAX,
                               0,  24, 96,  32, startFrameCbk );

    createIntegerText ( pan, gui, EDITRENDEREND,
                               INT_MIN, INT_MAX,
                               0,  48, 96,  32, endFrameCbk );

    createIntegerText ( pan, gui, EDITRENDERFPS,
                               0, INT_MAX,
                               0,  72, 96,  32, fpsCbk );

    createIntegerText ( pan, gui, EDITRENDERWIDTH,
                               0, INT_MAX,
                               0,  96, 96,  32, widthCbk );

    createIntegerText ( pan, gui, EDITRENDERHEIGHT,
                               0, INT_MAX,
                               0, 120, 96,  32, heightCbk );

    createFloatText   ( pan, gui, EDITRENDERRATIO,
                               0.0f, FLT_MAX,
                               0, 144, 96,  64, ratioCbk );

    createSaveOutputForm ( pan, gui, EDITRENDERSAVEOUTPUTFRAME,
                               0, 168, 256,  96 );

    createBackgroundForm ( pan, gui, EDITRENDERBACKGROUNDFRAME,
                               0, 264, 256,  96 );
}

/******************************************************************************/
void updateEffectsPanel ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( gui->currsg ) {
            if ( strcmp ( child_name, EDITRENDERMOTIONBLURFRAME ) == 0x00 ) {
                updateMotionBlurFrame ( child, gui );
            }

            if ( strcmp ( child_name, EDITRENDERWIREFRAMEFRAME ) == 0x00 ) {
                updateWireframeFrame ( child, gui );
            }

            if ( strcmp ( child_name, EDITRENDERFOGFRAME ) == 0x00 ) {
                updateFogFrame ( child, gui );
            }
        }

        children =  g_list_next ( children );
    }
}

/******************************************************************************/
void createEffectsPanel ( GtkWidget *parent, 
                          G3DUI     *gui,
                          char      *name,
                          gint       x,
                          gint       y,
                          gint       width,
                          gint       height ) {
    GtkWidget *pan;

    pan = createPanel ( parent, gui, name, x, y, width, height );

    createMotionBlurForm ( pan, gui, EDITRENDERMOTIONBLURFRAME,
                               0,   0, 256,  96 );

    createWireframeForm ( pan, gui, EDITRENDERWIREFRAMEFRAME,
                               0, 128, 256,  96 );

    createFogForm ( pan, gui, EDITRENDERFOGFRAME,
                               0, 256, 256,  96 );
}

/******************************************************************************/
void updateRenderEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( gui->currsg ) {
            if ( strcmp ( child_name, EDITRENDEREFFECTS ) == 0x00 ) {
                updateEffectsPanel ( child, gui );
            }

            if ( strcmp ( child_name, EDITRENDERGENERAL ) == 0x00 ) {
                updateGeneralPanel ( child, gui );
            }
        }

        children =  g_list_next ( children );
    }
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    gui->lock = 1;
    updateRenderEdit ( widget, gui );
    gui->lock = 0;
}

/******************************************************************************/
GtkWidget* createRenderEdit ( GtkWidget *parent, 
                              G3DUI     *gui,
                              char      *name,
                              gint       x,
                              gint       y,
                              gint       width,
                              gint       height ) {
    GdkRectangle frmrec = { 0x00, 0x00, width, height };
    GdkRectangle gdkrec = { 0x00, 0x20, width, height - 0x20 };
    GtkWidget * frm = gtk_fixed_new ( );
    GtkWidget *tab;

    gtk_widget_set_name ( frm, name );

    gtk_widget_set_size_request ( frm, frmrec.width, frmrec.height );

    /*** Because this is an independent window ***/
    gtk_container_add (GTK_CONTAINER (parent), frm);

    createPushButton  ( frm, gui, EDITRENDERRUN,
                         (width / 2 ) - 48, 4, 48, 20, g3dui_runRenderCbk );

    /********************/
    tab = gtk_notebook_new ( );

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(tab), TRUE );

    gtk_widget_set_name ( tab, EDITRENDERSETTINGS );

    gtk_widget_size_allocate ( tab, &gdkrec );
    /*gtk_widget_set_size_request ( tab, width, height );*/

    gtk_fixed_put ( GTK_FIXED(frm), tab, gdkrec.x, gdkrec.y );

    g_signal_connect ( G_OBJECT (tab), "realize", G_CALLBACK (Realize), gui );
    g_signal_connect ( G_OBJECT (tab), "destroy", G_CALLBACK (Destroy), gui );

    createGeneralPanel ( tab, gui, EDITRENDERGENERAL, 0, 0, width, height );
    createEffectsPanel ( tab, gui, EDITRENDEREFFECTS, 0, 0, width, height );

    gtk_widget_show ( tab );


    gtk_widget_show ( frm );

    /*list_insert ( &gui->lmatedit, frm );*/

    return frm;
}
