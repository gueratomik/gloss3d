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
static void solidColorCbk       ( GtkWidget *, gpointer  );
static void solidColorChangeCbk ( GtkWidget *, gpointer  );
static void imageColorCbk       ( GtkWidget *, gpointer  );
static void proceduralCbk       ( GtkWidget *, gpointer  );
static void chooseImageCbk      ( GtkWidget *, gpointer  );

static void editChannelProceduralCbk ( GtkWidget *widget, 
                                       gpointer   user_data );

/******************************************************************************/
static void nameCbk ( GtkWidget *widget, GdkEvent *event, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    const char *name = gtk_entry_get_text ( GTK_ENTRY(widget) );

    common_g3dui_materialSetNameCbk ( gui, name );
}


/******************************************************************************/
const char *getProdeduralTypeSelection ( GtkWidget *widget ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    const char *procTypeStr = NULL;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( strcmp ( child_name, EDITCHANNELPROCEDURALTYPE ) == 0x00 ) {
            if ( GTK_IS_COMBO_BOX_TEXT(child) ) {
                GtkComboBoxText *cbt = GTK_COMBO_BOX_TEXT(child);

                return gtk_combo_box_text_get_active_text ( cbt );
            }
        }

        children =  g_list_next ( children );
    }

    return procTypeStr;
}

/******************************************************************************/
const char *getProdeduralResSelection ( GtkWidget *widget ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    const char *procTypeStr = NULL;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( strcmp ( child_name, EDITCHANNELPROCEDURALRES ) == 0x00 ) {
            if ( GTK_IS_COMBO_BOX_TEXT(child) ) {
                GtkComboBoxText *cbt = GTK_COMBO_BOX_TEXT(child);

                return gtk_combo_box_text_get_active_text ( cbt );
            }
        }

        children =  g_list_next ( children );
    }

    return procTypeStr;
}

/******************************************************************************/
GtkWidget *createPanel ( GtkWidget *parent, G3DUI *gui,
                                            char *name,
                                            gint x,
                                            gint y,
                                            gint width,
                                            gint height ) {
    GdkRectangle prec = { 0x00, 0x00, width, height };
    GtkWidget *lab = gtk_label_new ( name );
    GtkWidget *pan;

    pan = gtk_fixed_new ( );

    gtk_widget_set_name ( pan, name );

    gtk_widget_set_size_request ( pan, width, height );
    /*gtk_widget_size_allocate ( pan, &prec );*/

    /*gtk_fixed_put ( parent, pan, x, y );*/
    gtk_notebook_append_page ( GTK_NOTEBOOK(parent), pan, lab );

    /*gtk_widget_show ( lab );*/
    gtk_widget_show ( pan );


    return pan;
}

/******************************************************************************/
static void transparencyStrengthCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float strength = ( float ) gtk_range_get_value ( GTK_RANGE(widget) );

    common_g3dui_materialSetAlphaStrengthCbk ( gui, strength / 100.0f );
}

/******************************************************************************/
static void refractionStrengthCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float strength = ( float ) gtk_range_get_value ( GTK_RANGE(widget) );

    common_g3dui_materialSetRefractionStrengthCbk ( gui, strength / 100.0f );
}

/******************************************************************************/
static void updateRefractionPanel ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( gui->selmat == NULL ) gtk_widget_set_sensitive ( child, FALSE );
        else                       gtk_widget_set_sensitive ( child, TRUE  );

        if ( gui->selmat ) {
            G3DMATERIAL *mat = gui->selmat;

            if ( GTK_IS_SCALE ( child ) ) {
                GtkRange *ran = GTK_RANGE(child);

                if ( strcmp ( child_name, EDITREFRACTIONSTRENGTH ) == 0x00 ) {
                    float refractionStrength = ( mat->refraction.solid.r + 
                                                 mat->refraction.solid.g +
                                                 mat->refraction.solid.b ) / 3.0f;

                    gtk_range_set_value ( ran, refractionStrength * 100 );
                }

                if ( strcmp ( child_name, EDITREFRACTIONTRANSPARENCY ) == 0x00 ) {
                    gtk_range_set_value ( ran, mat->alpha.solid.a * 100 );
                }
            }
        }

        children =  g_list_next ( children );
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static GtkWidget *createRefractionPanel ( GtkWidget *parent, G3DUI *gui,
                                                              char *name,
                                                              gint x,
                                                              gint y,
                                                              gint width,
                                                              gint height ) {
    GtkWidget *lab, *col, *pan, *btn;

    pan = createPanel ( parent, gui, name, x, y, width, height );

          createSimpleLabel     ( pan, gui, 
                                       EDITREFRACTIONSTRENGTH,
                                         0,  0,  96, 18 );

          createHorizontalScale ( pan, gui, 
                                       EDITREFRACTIONSTRENGTH,
                                       104,  0, 208, 18,
                                       100.0f, 200.0f, 1.0f,
                                       refractionStrengthCbk );

          /*createSimpleLabel     ( pan, gui, 
                                       EDITREFRACTIONTRANSPARENCY,
                                         0, 24,  96, 18 );

          createHorizontalScale ( pan, gui, 
                                       EDITREFRACTIONTRANSPARENCY,
                                       104, 24, 208, 18,
                                       0.0f, 100.0f, 1.0f,
                                       transparencyStrengthCbk );*/


    return pan;
}

/******************************************************************************/
static void reflectionStrengthCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float strength = ( float ) gtk_range_get_value ( GTK_RANGE(widget) );

    common_g3dui_materialSetReflectionStrengthCbk ( gui, strength / 100.0f );
}

/******************************************************************************/
static void updateReflectionPanel ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( gui->selmat == NULL ) gtk_widget_set_sensitive ( child, FALSE );
        else                       gtk_widget_set_sensitive ( child, TRUE  );

        if ( gui->selmat ) {
            G3DMATERIAL *mat = gui->selmat;

            if ( GTK_IS_SCALE ( child ) ) {
                GtkRange *ran = GTK_RANGE(child);

                if ( strcmp ( child_name, EDITREFLECTIONSTRENGTH ) == 0x00 ) {
                    float reflectionStrength = ( mat->reflection.solid.r + 
                                                 mat->reflection.solid.g +
                                                 mat->reflection.solid.b ) / 3.0f;
                    gtk_range_set_value ( ran, reflectionStrength * 100 );
                }
            }
        }

        children =  g_list_next ( children );
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static GtkWidget *createReflectionPanel ( GtkWidget *parent, G3DUI *gui,
                                                              char *name,
                                                              gint x,
                                                              gint y,
                                                              gint width,
                                                              gint height ) {
    GtkWidget *lab, *col, *pan, *btn;

    pan = createPanel ( parent, gui, name, x, y, width, height );

          createSimpleLabel     ( pan, gui, 
                                       EDITREFLECTIONSTRENGTH,
                                         0,  0,  96, 18 );

          createHorizontalScale ( pan, gui, 
                                       EDITREFLECTIONSTRENGTH,
                                       104,  0, 208, 18,
                                       0.0f, 100.0f, 1.0f,
                                       reflectionStrengthCbk );


    return pan;
}

/******************************************************************************/
static void displacementToggleCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_materialToggleDisplacementCbk ( gui );
}

/******************************************************************************/
void g3dui_loadImageForChannel ( G3DUI      *gui,
                                 G3DCHANNEL *chan ) {
    G3DUIGTK3 *ggt = gui->toolkit_data;

    if ( chan ) {
        GtkFileFilter *filter = gtk_file_filter_new ();
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

        /* extension filters */
        gtk_file_filter_add_pattern ( filter, "*.jpg" );
        gtk_file_filter_add_pattern ( filter, "*.png" );
        gtk_file_filter_add_pattern ( filter, "*.avi" );
        gtk_file_filter_add_pattern ( filter, "*.mkv" );
        gtk_file_filter_add_pattern ( filter, "*.flv" );
        gtk_file_filter_add_pattern ( filter, "*.gif" );
        gtk_file_filter_add_pattern ( filter, "*.mp4" );
        gtk_file_chooser_set_filter ( GTK_FILE_CHOOSER ( dialog ), filter );

        res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

        if ( res == GTK_RESPONSE_OK ) {
            GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
            const char     *filename = gtk_file_chooser_get_filename ( chooser );

            common_g3dui_channelChooseImageCbk ( gui,
                                        	     chan,
                                        	     filename,
                                        	     0x00 );

            g_free    ( ( gpointer ) filename );
        }

        gtk_widget_destroy ( dialog );
    }

    g3dui_updateMaterialEdit ( gui );
}

/******************************************************************************/
static void displacementImageCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( gui->selmat ) {
        g3dui_loadImageForChannel ( gui, &gui->selmat->displacement );
    }
}

/******************************************************************************/
static void displacementStrengthCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float str = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    g3dui_setHourGlass ( gui );

    common_g3dui_materialSetDisplacementStrengthCbk ( gui, str );

    g3dui_unsetHourGlass ( gui );
}

/******************************************************************************/
static void displacementProceduralCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;
    gchar *procType = getProdeduralTypeSelection ( parent ),
          *procRes  = getProdeduralResSelection ( parent );

    g3dui_setHourGlass ( gui );

    if ( gui->selmat ) {
        common_g3dui_materialEnableProceduralCbk ( gui, &gui->selmat->displacement );
	    common_g3dui_materialChooseProceduralCbk ( gui,
                                        	      &gui->selmat->displacement,
                                        	       procType, 
                                        	       procRes,
                                                   0x00 );
    }

    g3dui_unsetHourGlass ( gui );

    g_free ( procType );
    g_free ( procRes  );
}

/******************************************************************************/
static void updateDisplacementPanel ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( gui->selmat == NULL ) gtk_widget_set_sensitive ( child, FALSE );
        else                       gtk_widget_set_sensitive ( child, TRUE  );

        if ( gui->selmat ) {
            G3DMATERIAL *mat = gui->selmat;

            if ( GTK_IS_BUTTON ( child ) && ( GTK_IS_RADIO_BUTTON ( child ) == 0 ) ) {
                GtkButton *btn = GTK_BUTTON(child);

                if ( strcmp ( child_name, EDITDISPLACEMENTIMAGE ) == 0x00 ) {
                    if ( mat->displacement.flags & USEIMAGECOLOR ) {
                        if ( mat->displacement.image ) {
                            if ( mat->displacement.image->filename ) {
                                char *imgpath, *imgname;

                                imgpath = strdup ( mat->displacement.image->filename );

                                /*** We just keep the image name, not the whole ***/
                                /*** path and display it as the button label.   ***/
                                imgname = basename ( imgpath );

                                gtk_button_set_label ( btn, imgname );

                                free ( imgpath );
                            }
                        }
                    }
                }
            }

            if ( GTK_IS_COMBO_BOX_TEXT(child) ) {
                GtkComboBoxText *cbt = GTK_COMBO_BOX_TEXT(child);

                if ( strcmp ( child_name, EDITCHANNELPROCEDURALTYPE ) == 0x00 ) {
                    if ( mat->displacement.flags & USEPROCEDURAL ) {
                        if ( mat->displacement.proc ) {
                            G3DPROCEDURAL *proc = mat->displacement.proc;

                            gtk_combo_box_set_active ( cbt, proc->type );
                        }
                    }
                }
            }

            if ( GTK_IS_RADIO_BUTTON ( child ) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITDISPLACEMENTIMAGE ) == 0x00 ) {
                    if ( mat->alpha.flags & USEIMAGECOLOR ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }

                if ( strcmp ( child_name, EDITDISPLACEMENTPROCEDURAL ) == 0x00 ) {
                    if ( mat->alpha.flags & USEPROCEDURAL ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }
            }

            if ( GTK_IS_SPIN_BUTTON ( child ) ) {
                GtkSpinButton *spb = GTK_SPIN_BUTTON(child);

                if ( strcmp ( child_name, EDITDISPLACEMENTSTRENGTH ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, mat->displacement.solid.a );
                }
            }

            if ( GTK_IS_CHECK_BUTTON ( child ) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITDISPLACEMENTENABLED ) == 0x00 ) {
                    if ( mat->flags & DISPLACEMENT_ENABLED ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }
            }
        }

        children =  g_list_next ( children );
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void radioDisplacementImageColorCbk ( GtkWidget *widget, 
                                      gpointer   user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( gui->selmat ) {
        common_g3dui_materialEnableImageCbk ( gui, &gui->selmat->displacement );
    }
}

/******************************************************************************/
static void displacementProceduralTypeCbk ( GtkWidget *widget, gpointer user_data ) {
    displacementProceduralCbk ( widget, user_data );
}

/******************************************************************************/
static void displacementProceduralResCbk  ( GtkWidget *widget, gpointer user_data ) {
    displacementProceduralCbk ( widget, user_data );
}

/******************************************************************************/
static GtkWidget *createDisplacementPanel ( GtkWidget *parent, G3DUI *gui,
                                                               char *name,
                                                               gint x,
                                                               gint y,
                                                               gint width,
                                                               gint height ) {
    GtkWidget *lab, *col, *pan, *btn;

    pan = createPanel ( parent, gui, name, x, y, width, height );

    createToggleLabel  ( pan, gui, EDITDISPLACEMENTENABLED,   
                                0,  0,
                              192, 18, displacementToggleCbk );


    createFloatText    ( pan, gui, EDITDISPLACEMENTSTRENGTH,
                               0.0f, FLT_MAX,
                                0,  24,
                               96,
                               48, displacementStrengthCbk );

    /*** Use image as displacement texture ***/
    btn = createRadioLabel   ( pan, gui, EDITDISPLACEMENTIMAGE, NULL,
                                0, 48, 
                               96, 18, radioDisplacementImageColorCbk );

    createPushButton   ( pan, gui, EDITDISPLACEMENTIMAGE,
                               96,  48, 
                               96,  18, displacementImageCbk );

          /*** Use image as texture ***/
          createRadioLabel ( pan, gui, EDITDISPLACEMENTPROCEDURAL,
                                   btn,
                                     0, 72, 96, 18,
                                   displacementProceduralCbk );

          createProceduralTypeSelection ( pan, gui, 
                                               EDITDIFFUSEPROCEDURALTYPE,
                                               0, 96,
                                               104,
                                               64,
                                               displacementProceduralTypeCbk );

          createProceduralResSelection  ( pan, gui, 
                                               EDITDIFFUSEPROCEDURALRES,
                                               0, 120,
                                               104,
                                               64,
                                               displacementProceduralResCbk );
          /*** Procedural settings **/
          createPushButton  ( pan, gui, 
                                   EDITDISPLACEMENTPROCEDURALEDIT,
                                   104, 144, 96, 18,
                                   editChannelProceduralCbk );


    return pan;
}

/******************************************************************************/
static void alphaToggleCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_materialToggleAlphaCbk ( gui );
}

/******************************************************************************/
static void alphaImageCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( gui->selmat ) {
        g3dui_loadImageForChannel ( gui, &gui->selmat->alpha );
    }
}

/******************************************************************************/
static void alphaStrengthCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float opacity = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    g3dui_setHourGlass ( gui );

    common_g3dui_materialSetAlphaStrengthCbk ( gui, opacity / 100.0f );

    g3dui_unsetHourGlass ( gui );
}

/******************************************************************************/
static void alphaProceduralCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;
    gchar *procType = getProdeduralTypeSelection ( parent ),
          *procRes  = getProdeduralResSelection ( parent );

    g3dui_setHourGlass ( gui );

    if ( gui->selmat ) {
        common_g3dui_materialEnableProceduralCbk ( gui, &gui->selmat->alpha );
	    common_g3dui_materialChooseProceduralCbk ( gui,
                                        	      &gui->selmat->alpha,
                                        	       procType, 
                                        	       procRes,
                                                       0x01 );
    }

    g3dui_unsetHourGlass ( gui );

    g_free ( procType );
    g_free ( procRes  );
}

/******************************************************************************/
static void updateAlphaPanel ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER ( widget ) );

    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( gui->selmat == NULL ) gtk_widget_set_sensitive ( child, FALSE );
        else                       gtk_widget_set_sensitive ( child, TRUE  );

        if ( gui->selmat ) {
            G3DMATERIAL *mat = gui->selmat;

            if ( GTK_IS_BUTTON ( child ) && ( GTK_IS_RADIO_BUTTON ( child ) == 0 ) ) {
                GtkButton *btn = GTK_BUTTON(child);

                if ( strcmp ( child_name, EDITALPHAIMAGE ) == 0x00 ) {
                    if ( mat->alpha.flags & USEIMAGECOLOR ) {
                        if ( mat->alpha.image ) {
                            if ( mat->alpha.image->filename ) {
                                char *imgpath, *imgname;

                                imgpath = strdup ( mat->alpha.image->filename );

                                /*** We just keep the image name, not the whole ***/
                                /*** path and display it as the button label.   ***/
                                imgname = basename ( imgpath );

                                gtk_button_set_label ( btn, imgname );

                                free ( imgpath );
                            }
                        }
                    }
                }
            }

            if ( GTK_IS_COMBO_BOX_TEXT(child) ) {
                GtkComboBoxText *cbt = GTK_COMBO_BOX_TEXT(child);

                if ( strcmp ( child_name, EDITCHANNELPROCEDURALTYPE ) == 0x00 ) {
                    if ( mat->alpha.flags & USEPROCEDURAL ) {
                        if ( mat->alpha.proc ) {
                            G3DPROCEDURAL *proc = mat->alpha.proc;

                            gtk_combo_box_set_active ( cbt, proc->type );
                        }
                    }
                }
            }

            if ( GTK_IS_RADIO_BUTTON ( child ) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITALPHAIMAGE ) == 0x00 ) {
                    if ( mat->alpha.flags & USEIMAGECOLOR ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }

                if ( strcmp ( child_name, EDITALPHAPROCEDURAL ) == 0x00 ) {
                    if ( mat->alpha.flags & USEPROCEDURAL ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }
            }

            if ( GTK_IS_SPIN_BUTTON ( child ) ) {
                GtkSpinButton *spb = GTK_SPIN_BUTTON(child);

                if ( strcmp ( child_name, EDITALPHASTRENGTH ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, mat->alpha.solid.a * 100.0f );
                }
            }

            if ( GTK_IS_CHECK_BUTTON ( child ) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITALPHAENABLED ) == 0x00 ) {
                    if ( mat->flags & ALPHA_ENABLED ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }
            }
        }

        children =  g_list_next ( children );
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void radioAlphaImageColorCbk ( GtkWidget *widget, 
                                      gpointer   user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( gui->selmat ) {
        common_g3dui_materialEnableImageCbk ( gui, &gui->selmat->alpha );
    }
}

/******************************************************************************/
static void alphaProceduralTypeCbk ( GtkWidget *widget, gpointer user_data ) {
    alphaProceduralCbk ( widget, user_data );
}

/******************************************************************************/
static void alphaProceduralResCbk  ( GtkWidget *widget, gpointer user_data ) {
    alphaProceduralCbk ( widget, user_data );
}

/******************************************************************************/
static GtkWidget *createAlphaPanel ( GtkWidget *parent, G3DUI *gui,
                                                               char *name,
                                                               gint x,
                                                               gint y,
                                                               gint width,
                                                               gint height ) {
    GtkWidget *lab, *col, *pan, *btn;

    pan = createPanel ( parent, gui, name, x, y, width, height );

    createToggleLabel  ( pan, gui, EDITALPHAENABLED,   
                                0,  0,
                              192, 18, alphaToggleCbk );


    createFloatText    ( pan, gui, EDITALPHASTRENGTH,
                               0.0f, 100.0f,
                                0,  24,
                               96,
                               48, alphaStrengthCbk );

    /*** Use image as displacement texture ***/
    btn = createRadioLabel   ( pan, gui, EDITALPHAIMAGE, NULL,
                                0, 48, 
                               96, 18, radioAlphaImageColorCbk );

    createPushButton   ( pan, gui, EDITALPHAIMAGE,
                               96,  48, 
                               96,  18, alphaImageCbk );

          /*** Use image as texture ***/
          createRadioLabel ( pan, gui, EDITALPHAPROCEDURAL,
                                   btn,
                                     0, 72, 96, 18,
                                   alphaProceduralCbk );

          createProceduralTypeSelection ( pan, gui, 
                                               EDITDIFFUSEPROCEDURALTYPE,
                                               0, 96,
                                               104,
                                               64,
                                               alphaProceduralTypeCbk );

          createProceduralResSelection  ( pan, gui, 
                                               EDITDIFFUSEPROCEDURALRES,
                                               0, 120,
                                               104,
                                               64,
                                               alphaProceduralResCbk );

          /*** Procedural settings **/
          createPushButton  ( pan, gui, 
                                   EDITALPHAPROCEDURALEDIT,
                                   104, 144, 96, 18,
                                   editChannelProceduralCbk );


    return pan;
}

/******************************************************************************/
static void bumpToggleCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_materialToggleBumpCbk ( gui );
}

/******************************************************************************/
static void bumpImageCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( gui->selmat ) {
        g3dui_loadImageForChannel ( gui, &gui->selmat->bump );
    }
}

/******************************************************************************/
static void bumpStrengthCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float str = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    g3dui_setHourGlass ( gui );

    common_g3dui_materialSetBumpStrengthCbk ( gui, str );

    g3dui_unsetHourGlass ( gui );
}

/******************************************************************************/
static void bumpProceduralCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;
    gchar *procType = getProdeduralTypeSelection ( parent ),
          *procRes  = getProdeduralResSelection ( parent );

    g3dui_setHourGlass ( gui );

    if ( gui->selmat ) {
        common_g3dui_materialEnableProceduralCbk ( gui, &gui->selmat->bump );
	    common_g3dui_materialChooseProceduralCbk ( gui,
                                        	      &gui->selmat->bump,
                                        	       procType, 
                                        	       procRes,
                                                       0x01 );
    }

    g3dui_unsetHourGlass ( gui );

    g_free ( procType );
    g_free ( procRes  );
}

/******************************************************************************/
static void updateBumpPanel ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( gui->selmat == NULL ) gtk_widget_set_sensitive ( child, FALSE );
        else                       gtk_widget_set_sensitive ( child, TRUE  );

        if ( gui->selmat ) {
            G3DMATERIAL *mat = gui->selmat;

            if ( GTK_IS_BUTTON ( child ) && ( GTK_IS_RADIO_BUTTON ( child ) == 0 ) ) {
                GtkButton *btn = GTK_BUTTON(child);

                if ( strcmp ( child_name, EDITBUMPIMAGE ) == 0x00 ) {
                    if ( mat->bump.flags & USEIMAGECOLOR ) {
                        if ( mat->bump.image ) {
                            if ( mat->bump.image->filename ) {
                                char *imgpath, *imgname;

                                imgpath = strdup ( mat->bump.image->filename );

                                /*** We just keep the image name, not the whole ***/
                                /*** path and display it as the button label.   ***/
                                imgname = basename ( imgpath );

                                gtk_button_set_label ( btn, imgname );

                                free ( imgpath );
                            }
                        }
                    }
                }
            }

            if ( GTK_IS_COMBO_BOX_TEXT(child) ) {
                GtkComboBoxText *cbt = GTK_COMBO_BOX_TEXT(child);

                if ( strcmp ( child_name, EDITCHANNELPROCEDURALTYPE ) == 0x00 ) {
                    if ( mat->bump.flags & USEPROCEDURAL ) {
                        if ( mat->bump.proc ) {
                            G3DPROCEDURAL *proc = mat->bump.proc;

                            gtk_combo_box_set_active ( cbt, proc->type );
                        }
                    }
                }
            }

            if ( GTK_IS_RADIO_BUTTON ( child ) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITBUMPIMAGE ) == 0x00 ) {
                    if ( mat->bump.flags & USEIMAGECOLOR ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }

                if ( strcmp ( child_name, EDITBUMPPROCEDURAL ) == 0x00 ) {
                    if ( mat->bump.flags & USEPROCEDURAL ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }
            }

            if ( GTK_IS_SPIN_BUTTON ( child ) ) {
                GtkSpinButton *spb = GTK_SPIN_BUTTON(child);

                if ( strcmp ( child_name, EDITBUMPSTRENGTH ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, mat->bump.solid.a );
                }
            }

            if ( GTK_IS_CHECK_BUTTON ( child ) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITBUMPENABLED ) == 0x00 ) {
                    if ( mat->flags & BUMP_ENABLED ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }
            }
        }

        children =  g_list_next ( children );
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void bumpProceduralTypeCbk ( GtkWidget *widget, gpointer user_data ) {
    bumpProceduralCbk ( widget, user_data );
}

/******************************************************************************/
static void bumpProceduralResCbk  ( GtkWidget *widget, gpointer user_data ) {
    bumpProceduralCbk ( widget, user_data );
}

/******************************************************************************/
static void radioBumpImageColorCbk ( GtkWidget *widget, 
                                      gpointer   user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( gui->selmat ) {
        common_g3dui_materialEnableImageCbk ( gui, &gui->selmat->bump );
    }
}

/******************************************************************************/
static GtkWidget *createBumpPanel ( GtkWidget *parent, G3DUI *gui,
                                                               char *name,
                                                               gint x,
                                                               gint y,
                                                               gint width,
                                                               gint height ) {
    GtkWidget *lab, *col, *pan, *btn;

    pan = createPanel ( parent, gui, name, x, y, width, height );

    createToggleLabel  ( pan, gui, EDITBUMPENABLED,   
                                0,  0,
                              192, 18, bumpToggleCbk );


    createFloatText    ( pan, gui, EDITBUMPSTRENGTH,
                               0.0f, FLT_MAX,
                                0,  24,
                               96,
                               48, bumpStrengthCbk );

    /*** Use image as displacement texture ***/
    btn = createRadioLabel   ( pan, gui, EDITBUMPIMAGE, NULL,
                                0, 48, 
                               96, 18, radioBumpImageColorCbk );

    createPushButton   ( pan, gui, EDITBUMPIMAGE,
                               96,  48, 
                               96,  18, bumpImageCbk );

          /*** Use image as texture ***/
          createRadioLabel ( pan, gui, EDITBUMPPROCEDURAL,
                                   btn,
                                     0, 72, 96, 18,
                                   bumpProceduralCbk );

          createProceduralTypeSelection ( pan, gui, 
                                               EDITCHANNELPROCEDURALTYPE,
                                               0, 96,
                                               104,
                                               64,
                                               bumpProceduralTypeCbk );

          createProceduralResSelection  ( pan, gui, 
                                               EDITCHANNELPROCEDURALRES,
                                               0, 120,
                                               104,
                                               64,
                                               bumpProceduralResCbk );

          /*** Procedural settings **/
          createPushButton  ( pan, gui, 
                                   EDITBUMPPROCEDURALEDIT,
                                   104, 144, 96, 18,
                                   editChannelProceduralCbk );


    return pan;
}

/******************************************************************************/
static void specularColorCbk ( GtkWidget *widget,  gpointer user_data ) {
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER(widget);
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( gui->selmat ) {
        GdkRGBA color;

        gtk_color_chooser_get_rgba ( ccr, &color );

        common_g3dui_materialSetSpecularColorCbk ( gui,
                                                   color.red,
                                                   color.green,
                                                   color.blue,
                                                   color.alpha );
    }
}

/******************************************************************************/
static void specularLevelCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float val = ( float ) gtk_range_get_value ( GTK_RANGE(widget) );

    common_g3dui_materialSetSpecularLevelCbk ( gui, val / 255.0f );
}

/******************************************************************************/
static void specularShininessCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float val = ( float ) gtk_range_get_value ( GTK_RANGE(widget) );

    common_g3dui_materialSetSpecularShininessCbk ( gui, val );
}

/******************************************************************************/
static void updateSpecularPanel ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( gui->selmat == NULL ) gtk_widget_set_sensitive ( child, FALSE );
        else                       gtk_widget_set_sensitive ( child, TRUE  );

        if ( gui->selmat ) {
            G3DMATERIAL *mat = gui->selmat;

            if ( GTK_IS_SCALE ( child ) ) {
                GtkRange *ran = GTK_RANGE(child);

                if ( strcmp ( child_name, EDITSPECULARSHININESS ) == 0x00 ) {
                    gtk_range_set_value ( ran, mat->shininess );
                }

                if ( strcmp ( child_name, EDITSPECULARLEVEL ) == 0x00 ) {
                    gtk_range_set_value ( ran, mat->specular_level * 255.0f );
                }
            }

            if ( GTK_IS_COLOR_BUTTON ( child ) ) {
                GtkColorChooser *ccr = GTK_COLOR_CHOOSER(child);

                if ( strcmp ( child_name, EDITSPECULARCOLOR ) == 0x00 ) {
                    GdkRGBA rgba = { .red   = mat->specular.solid.r,
                                     .green = mat->specular.solid.g,
                                     .blue  = mat->specular.solid.b,
                                     .alpha = mat->specular.solid.a };

                    gtk_color_chooser_set_rgba ( ccr, &rgba );
                }
            }
        }

        children =  g_list_next ( children );
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static GtkWidget *createSpecularPanel ( GtkWidget *parent, G3DUI *gui,
                                                              char *name,
                                                              gint x,
                                                              gint y,
                                                              gint width,
                                                              gint height ) {
    GtkWidget *lab, *col, *pan, *btn;

    pan = createPanel ( parent, gui, name, x, y, width, height );

          createSimpleLabel     ( pan, gui, 
                                       EDITSPECULARSHININESS,
                                         0,  0,  96, 18 );

          createHorizontalScale ( pan, gui, 
                                       EDITSPECULARSHININESS,
                                       104,  0, 208, 18,
                                       0.0f, 128.0f, 1.0f,
                                       specularShininessCbk );

          createSimpleLabel     ( pan, gui, 
                                       EDITSPECULARLEVEL,
                                         0, 24,  96, 18 );

          createHorizontalScale ( pan, gui, 
                                       EDITSPECULARLEVEL,
                                       104, 24, 208, 18,
                                       0.0f, 255.0f, 1.0f,
                                       specularLevelCbk );

          createColorButton     ( pan, gui,
                                       EDITSPECULARCOLOR,
                                       104, 48, 32, 18,
                                       specularColorCbk );

    return pan;
}

/******************************************************************************/
static void diffuseImageColorCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( gui->selmat ) {
        g3dui_loadImageForChannel ( gui, &gui->selmat->diffuse );
    }
}

/******************************************************************************/
static void editChannelProceduralCbk ( GtkWidget *widget, 
                                       gpointer   user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );
    G3DUI *gui = ( G3DUI * ) user_data;
    char *wname = gtk_widget_get_name ( widget );

    if ( gui->selmat ) {
        G3DMATERIAL *mat = gui->selmat;
        G3DCHANNEL *cha = NULL;

        if ( strcmp ( wname, EDITDIFFUSEPROCEDURALEDIT ) == 0x00 ) {
            cha = &mat->diffuse;
        }

        if ( strcmp ( wname, EDITDISPLACEMENTPROCEDURALEDIT ) == 0x00 ) {
            cha = &mat->displacement;
        }

        if ( strcmp ( wname, EDITBUMPPROCEDURALEDIT ) == 0x00 ) {
            cha = &mat->bump;
        }

        if ( strcmp ( wname, EDITALPHAPROCEDURALEDIT ) == 0x00 ) {
            cha = &mat->alpha;
        }

        if ( cha ) {
            if (cha->proc ) {
                G3DPROCEDURAL *proc = cha->proc;

                switch ( proc->type ) {
                    case PROCEDURALBRICK :
                        createProceduralBrickEdit ( dial,
                                                    gui,
                                                    cha->proc, 
                                                    "Procedural Brick",
                                                    0, 0, 350, 35, 0x01 );
                    break;

                    case PROCEDURALCHESS :
                        createProceduralChessEdit ( dial,
                                                    gui,
                                                    cha->proc, 
                                                    "Procedural Chess",
                                                    0, 0, 350, 35, 0x01 );
                    break;

                    case PROCEDURALNOISE :
                        createProceduralNoiseEdit ( dial,
                                                    gui,
                                                    cha->proc, 
                                                    "Procedural Noise",
                                                    0, 0, 350, 35, 0x01 );
                    break;

                    default :
                    break;
                }

            }
        }

        gtk_widget_show ( dial );
    }
}

/******************************************************************************/
static void diffuseProceduralCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;
    gchar *procType = getProdeduralTypeSelection ( parent ),
          *procRes  = getProdeduralResSelection ( parent );

    g3dui_setHourGlass ( gui );

    if ( gui->selmat ) {
        common_g3dui_materialEnableProceduralCbk ( gui, &gui->selmat->diffuse );
	common_g3dui_materialChooseProceduralCbk ( gui,
                                        	  &gui->selmat->diffuse,
                                        	   procType, 
                                        	   procRes,
                                                   0x01 );
    }

    g3dui_unsetHourGlass ( gui );

    g_free ( procType );
    g_free ( procRes  );
}

/******************************************************************************/
static void diffuseSolidColorCbk ( GtkWidget *widget,  gpointer user_data ) {
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER(widget);
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( gui->selmat ) {
        GdkRGBA color;

        gtk_color_chooser_get_rgba ( ccr, &color );

        common_g3dui_materialSetDiffuseColorCbk ( gui,
                                                  color.red,
                                                  color.green,
                                                  color.blue,
                                                  color.alpha );
    }
}

/******************************************************************************/
static void radioDiffuseImageColorCbk ( GtkWidget *widget, 
                                        gpointer   user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( gui->selmat ) {
        common_g3dui_materialEnableImageCbk ( gui, &gui->selmat->diffuse );
    }
}

/******************************************************************************/
static void radioDiffuseSolidColorCbk ( GtkWidget *widget, 
                                        gpointer   user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( gui->selmat ) {
        common_g3dui_materialEnableSolidColorCbk ( gui, &gui->selmat->diffuse );
    }
}

/******************************************************************************/
static void diffuseProceduralTypeCbk ( GtkWidget *widget, gpointer user_data ) {
    diffuseProceduralCbk ( widget, user_data );
}

/******************************************************************************/
static void diffuseProceduralResCbk  ( GtkWidget *widget, gpointer user_data ) {
    diffuseProceduralCbk ( widget, user_data );
}

/******************************************************************************/
static void updateDiffuseColorPanel ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( gui->selmat == NULL ) gtk_widget_set_sensitive ( child, FALSE );
        else                       gtk_widget_set_sensitive ( child, TRUE  );

        if ( gui->selmat ) {
            G3DMATERIAL *mat = gui->selmat;

            if ( GTK_IS_BUTTON ( child ) && ( GTK_IS_RADIO_BUTTON ( child ) == 0 ) ) {
                GtkButton *btn = GTK_BUTTON(child);

                if ( strcmp ( child_name, EDITDIFFUSEIMAGE ) == 0x00 ) {
                    if ( mat->diffuse.flags & USEIMAGECOLOR ) {
                        if ( mat->diffuse.image ) {
                            if ( mat->diffuse.image->filename ) {
                                char *imgpath, *imgname;

                                imgpath = g3dcore_strclone ( mat->diffuse.image->filename );

                                /*** We just keep the image name, not the whole ***/
                               /*** path and display it as the button label.   ***/
                                imgname = basename ( imgpath );

                                gtk_button_set_label ( btn, imgname );

                                free ( imgpath );
                            }
                        }
                    }
                }
            }

            if ( GTK_IS_COMBO_BOX_TEXT(child) ) {
                GtkComboBoxText *cbt = GTK_COMBO_BOX_TEXT(child);

                if ( strcmp ( child_name, EDITCHANNELPROCEDURALTYPE ) == 0x00 ) {
                    if ( mat->diffuse.flags & USEPROCEDURAL ) {
                        if ( mat->diffuse.proc ) {
                            G3DPROCEDURAL *proc = mat->diffuse.proc;

                            gtk_combo_box_set_active ( cbt, proc->type );
                        }
                    }
                }
            }

            if ( GTK_IS_RADIO_BUTTON ( child ) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITDIFFUSECOLOR ) == 0x00 ) {
                    if ( mat->diffuse.flags & USESOLIDCOLOR ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }

                if ( strcmp ( child_name, EDITDIFFUSEIMAGE ) == 0x00 ) {
                    if ( mat->diffuse.flags & USEIMAGECOLOR ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }

                if ( strcmp ( child_name, EDITDIFFUSEPROCEDURAL ) == 0x00 ) {
                    if ( mat->diffuse.flags & USEPROCEDURAL ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }
            }

            if ( GTK_IS_COLOR_BUTTON ( child ) ) {
                GtkColorChooser *ccr = GTK_COLOR_CHOOSER(child);

                if ( strcmp ( child_name, EDITDIFFUSECOLOR ) == 0x00 ) {
                    GdkRGBA rgba = { .red   = mat->diffuse.solid.r,
                                     .green = mat->diffuse.solid.g,
                                     .blue  = mat->diffuse.solid.b,
                                     .alpha = mat->diffuse.solid.a };

                    gtk_color_chooser_set_rgba ( ccr, &rgba );
                }
            }
        }

        children =  g_list_next ( children );
    }

    gui->lock = 0x00;
}



/******************************************************************************/
static GtkWidget *createDiffuseColorPanel ( GtkWidget *parent, G3DUI *gui,
                                                               char *name,
                                                               gint x,
                                                               gint y,
                                                               gint width,
                                                               gint height ) {
    GtkWidget *lab, *col, *pan, *btn;

    pan = createPanel ( parent, gui, name, x, y, width, height );

          /*** Use Solid color as texture ***/
    btn = createRadioLabel ( pan, gui,
                                   EDITDIFFUSECOLOR,
                                   NULL,
                                     0,  0, 96, 18,
                                   radioDiffuseSolidColorCbk );
          /*** Pick color button ***/
    col = createColorButton ( pan, gui,
                                   EDITDIFFUSECOLOR,
                                   104,  0, 96, 18,
                                   diffuseSolidColorCbk );

          /*** Use image as texture ***/
          createRadioLabel ( pan, gui,
                                   EDITDIFFUSEIMAGE,
                                   btn,
                                     0, 24, 96, 18,
                                   radioDiffuseImageColorCbk );

          /*** Image chooser button **/
          createPushButton  ( pan, gui, 
                                   EDITDIFFUSEIMAGE,
                                   104, 24, 96, 18,
                                   diffuseImageColorCbk );

          /*** Use image as texture ***/
          createRadioLabel ( pan, gui,
                                   EDITDIFFUSEPROCEDURAL,
                                   btn,
                                     0, 48, 96, 18,
                                   diffuseProceduralCbk );

          createProceduralTypeSelection ( pan, gui, 
                                               EDITDIFFUSEPROCEDURALTYPE,
                                               0, 72,
                                               104,
                                               64,
                                               diffuseProceduralTypeCbk );

          /*** Procedural settings **/
          createPushButton  ( pan, gui, 
                                   EDITDIFFUSEPROCEDURALEDIT,
                                   104, 96, 96, 18,
                                   editChannelProceduralCbk );

          /*createProceduralResSelection  ( pan, gui, 
                                               EDITDIFFUSEPROCEDURALRES,
                                               0, 96,
                                               104,
                                               64,
                                               diffuseProceduralResCbk );*/


    return pan;
}

/******************************************************************************/
static void updatePage ( GtkWidget *widget, G3DUI *gui ) {
    const char *name = gtk_widget_get_name ( widget );

    if ( strcmp ( name, EDITSPECULAR     ) == 0x00 ) {
        updateSpecularPanel ( widget, gui );
    }

    if ( strcmp ( name, EDITDIFFUSE      ) == 0x00 ) {
        updateDiffuseColorPanel ( widget, gui );
    }

    if ( strcmp ( name, EDITDISPLACEMENT ) == 0x00 ) {
        updateDisplacementPanel ( widget, gui );
    }

    if ( strcmp ( name, EDITREFLECTION   ) == 0x00 ) {
        updateReflectionPanel ( widget, gui );
    }

    if ( strcmp ( name, EDITREFRACTION   ) == 0x00 ) {
        updateRefractionPanel ( widget, gui );
    }

    if ( strcmp ( name, EDITALPHA        ) == 0x00 ) {
        updateAlphaPanel ( widget, gui );
    }

    if ( strcmp ( name, EDITBUMP         ) == 0x00 ) {
        updateBumpPanel ( widget, gui );
    }
}

/******************************************************************************/
void updateMaterialChannels ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;

        updatePage ( child, gui );

        children =  g_list_next ( children );
    }
}

/******************************************************************************/
void updateMaterialEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DMATERIAL *mat = gui->selmat;

    gui->lock = 0x01;

    if ( gui->selmat == NULL ) gtk_widget_set_sensitive ( widget, FALSE );
    else                       gtk_widget_set_sensitive ( widget, TRUE  );

    if ( mat ) {
        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *child_name = gtk_widget_get_name ( child );

            if ( strcmp ( child_name, EDITMATERIALCHANNELS  ) == 0x00 ) {
                updateMaterialChannels ( child, gui );
            }

            if ( strcmp ( child_name, EDITMATERIALNAME      ) == 0x00 ) {
                if ( GTK_IS_ENTRY(child) ) {
                    GtkEntry *ent = GTK_ENTRY(child);

                    gtk_entry_set_text ( ent, mat->name );
                }
            }

            children =  g_list_next ( children );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void pageAdded ( GtkNotebook *notebook,
                        GtkWidget   *child,
                        guint        page_num,
                        gpointer     user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    updatePage ( child, gui );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    updateMaterialEdit ( widget, gui );
}

/******************************************************************************/
GtkWidget *createMaterialEdit ( GtkWidget *parent, G3DUI *gui,
                                                   char *name,
                                                   gint x,
                                                   gint y,
                                                   gint width,
                                                   gint height ) {
    GdkRectangle frmrec = { 0x00, 0x00, width, height };
    GdkRectangle gdkrec = { 0x00, 0x20, width, height - 0x20 };
    GtkWidget * frm = gtk_fixed_new ( );
    GtkWidget *tab;

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &frmrec );

    gtk_fixed_put ( GTK_FIXED(parent), frm, x, y );


    createCharText ( frm, gui, EDITMATERIALNAME, 0, 0, 96, 96, nameCbk );


    /********************/
    tab = gtk_notebook_new ( );

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(tab), TRUE );

    gtk_widget_set_name ( tab, EDITMATERIALCHANNELS );

    gtk_widget_size_allocate ( tab, &gdkrec );
    /*gtk_widget_set_size_request ( tab, width, height );*/

    gtk_fixed_put ( GTK_FIXED(frm), tab, gdkrec.x, gdkrec.y );

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), gui );
    /*g_signal_connect ( G_OBJECT (tab), "page-added", G_CALLBACK (pageAdded), gui );*/

    createDiffuseColorPanel ( tab, gui, EDITDIFFUSE     , 0, 0, width, height );
    createSpecularPanel     ( tab, gui, EDITSPECULAR    , 0, 0, width, height );
    createBumpPanel         ( tab, gui, EDITBUMP        , 0, 0, width, height );
    createDisplacementPanel ( tab, gui, EDITDISPLACEMENT, 0, 0, width, height );
    createAlphaPanel        ( tab, gui, EDITALPHA       , 0, 0, width, height );
    createReflectionPanel   ( tab, gui, EDITREFLECTION  , 0, 0, width, height );
    createRefractionPanel   ( tab, gui, EDITREFRACTION  , 0, 0, width, height );

    gtk_widget_show ( tab );


    gtk_widget_show ( frm );

    list_insert ( &gui->lmatedit, frm );

    return frm;
}
