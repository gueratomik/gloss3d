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
static void solidColorCbk       ( GtkWidget *, gpointer  );
static void solidColorChangeCbk ( GtkWidget *, gpointer  );
static void imageColorCbk       ( GtkWidget *, gpointer  );
static void chooseImageCbk      ( GtkWidget *, gpointer  );

/******************************************************************************/
static void nameCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    const char *name = gtk_entry_get_text ( GTK_ENTRY(widget) );

    common_g3dui_materialNameCbk ( gui, name );
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

    common_g3dui_materialTransparencyStrengthCbk ( gui, strength / 100.0f );
}

/******************************************************************************/
static void refractionStrengthCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float strength = ( float ) gtk_range_get_value ( GTK_RANGE(widget) );

    common_g3dui_materialRefractionStrengthCbk ( gui, strength / 100.0f );
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
                    gtk_range_set_value ( ran, mat->refraction_strength * 100 );
                }

                if ( strcmp ( child_name, EDITREFRACTIONTRANSPARENCY ) == 0x00 ) {
                    gtk_range_set_value ( ran, mat->transparency_strength * 100 );
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

          createSimpleLabel     ( pan, gui, 
                                       EDITREFRACTIONTRANSPARENCY,
                                         0, 24,  96, 18 );

          createHorizontalScale ( pan, gui, 
                                       EDITREFRACTIONTRANSPARENCY,
                                       104, 24, 208, 18,
                                       0.0f, 100.0f, 1.0f,
                                       transparencyStrengthCbk );


    return pan;
}

/******************************************************************************/
static void reflectionStrengthCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float strength = ( float ) gtk_range_get_value ( GTK_RANGE(widget) );

    common_g3dui_materialReflectionStrengthCbk ( gui, strength / 100.0f );
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
                    gtk_range_set_value ( ran, mat->reflection_strength * 100 );
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

    common_g3dui_materialDisplacementToggleCbk ( gui );
}

/******************************************************************************/
static void displacementImageCbk ( GtkWidget *widget, gpointer user_data ) {
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

        common_g3dui_materialDisplacementImageCbk ( gui, filename );

        g_free    ( ( gpointer ) filename );
    }

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
static void displacementStrengthCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float str = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    common_g3dui_materialDisplacementStrengthCbk ( gui, str );
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
                    if ( mat->flags & DISPLACEMENT_USEIMAGECOLOR ) {
                        if ( mat->displacement.image ) {
                            if ( mat->displacement.image->name ) {
                                char *imgpath, *imgname;

                                imgpath = g3dcore_strclone ( mat->displacement.image->name );

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

            if ( GTK_IS_SPIN_BUTTON ( child ) ) {
                GtkSpinButton *spb = GTK_SPIN_BUTTON(child);

                if ( strcmp ( child_name, EDITDISPLACEMENTSTRENGTH ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, mat->displacement_strength );
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
                                0,  24,
                               96,
                               48, displacementStrengthCbk );

    /*** Use image as displacement texture ***/
    createRadioLabel   ( pan, gui, EDITDISPLACEMENTIMAGE, NULL,
                                0, 48, 
                               96, 18, NULL );

    createPushButton   ( pan, gui, EDITDISPLACEMENTIMAGE,
                               96,  48, 
                               96,  18, displacementImageCbk );


    return pan;
}

/******************************************************************************/
static void specularityColorCbk ( GtkWidget *widget,  gpointer user_data ) {
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER(widget);
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( gui->selmat ) {
        G3DMATERIAL *mat = gui->selmat;
        GdkRGBA color;

        gtk_color_chooser_get_rgba ( ccr, &color );

        common_g3dui_materialColorChangeCbk ( gui, &mat->specular,
                                                   color.red   * 255,
                                                   color.green * 255,
                                                   color.blue  * 255 );
    }
}

/******************************************************************************/
static void specularityLevelCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float val = ( float ) gtk_range_get_value ( GTK_RANGE(widget) );

    common_g3dui_materialSpecularityLevelCbk ( gui, val / 255.0f );
}

/******************************************************************************/
static void specularityShininessCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    float val = ( float ) gtk_range_get_value ( GTK_RANGE(widget) );

    common_g3dui_materialSpecularityShininessCbk ( gui, val );
}

/******************************************************************************/
static void updateSpecularityPanel ( GtkWidget *widget, G3DUI *gui ) {
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

                if ( strcmp ( child_name, EDITSPECULARITYSHININESS ) == 0x00 ) {
                    gtk_range_set_value ( ran, mat->shininess );
                }

                if ( strcmp ( child_name, EDITSPECULARITYLEVEL ) == 0x00 ) {
                    gtk_range_set_value ( ran, mat->specular_level * 255.0f );
                }
            }

            if ( GTK_IS_COLOR_BUTTON ( child ) ) {
                GtkColorChooser *ccr = GTK_COLOR_CHOOSER(child);

                if ( strcmp ( child_name, EDITSPECULARITYCOLOR ) == 0x00 ) {
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
static GtkWidget *createSpecularityPanel ( GtkWidget *parent, G3DUI *gui,
                                                              char *name,
                                                              gint x,
                                                              gint y,
                                                              gint width,
                                                              gint height ) {
    GtkWidget *lab, *col, *pan, *btn;

    pan = createPanel ( parent, gui, name, x, y, width, height );

          createSimpleLabel     ( pan, gui, 
                                       EDITSPECULARITYSHININESS,
                                         0,  0,  96, 18 );

          createHorizontalScale ( pan, gui, 
                                       EDITSPECULARITYSHININESS,
                                       104,  0, 208, 18,
                                       0.0f, 128.0f, 1.0f,
                                       specularityShininessCbk );

          createSimpleLabel     ( pan, gui, 
                                       EDITSPECULARITYLEVEL,
                                         0, 24,  96, 18 );

          createHorizontalScale ( pan, gui, 
                                       EDITSPECULARITYLEVEL,
                                       104, 24, 208, 18,
                                       0.0f, 255.0f, 1.0f,
                                       specularityLevelCbk );

          createColorButton     ( pan, gui,
                                       EDITSPECULARITYCOLOR,
                                       104, 48, 32, 18,
                                       specularityColorCbk );

    return pan;
}

/******************************************************************************/
static void chooseImageCbk ( GtkWidget *widget, gpointer user_data ) {
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
        char           *filename = gtk_file_chooser_get_filename ( chooser );

        common_g3dui_materialChooseImageCbk ( gui, filename );

        g_free    ( filename );
    }

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
static void imageColorCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_materialImageColorCbk ( gui );
}

/******************************************************************************/
static void solidColorChangeCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER(widget);

    if ( gui->selmat ) {
        G3DMATERIAL *mat = gui->selmat;
        GdkRGBA color;

        gtk_color_chooser_get_rgba ( ccr, &color );

        common_g3dui_materialColorChangeCbk ( gui, &mat->diffuse,
                                                   color.red   * 255,
                                                   color.green * 255,
                                                   color.blue  * 255 );
    }
}

/******************************************************************************/
static void solidColorCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3dui_materialSolidColorCbk ( gui );
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
                    if ( mat->flags & DIFFUSE_USEIMAGECOLOR ) {
                        if ( mat->diffuse.image ) {
                            if ( mat->diffuse.image->name ) {
                                char *imgpath, *imgname;

                                imgpath = g3dcore_strclone ( mat->diffuse.image->name );

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

            if ( GTK_IS_RADIO_BUTTON ( child ) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITDIFFUSECOLOR ) == 0x00 ) {
                    if ( mat->flags & DIFFUSE_USESOLIDCOLOR ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }

                if ( strcmp ( child_name, EDITDIFFUSEIMAGE ) == 0x00 ) {
                    if ( mat->flags & DIFFUSE_USEIMAGECOLOR ) {
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
                                   solidColorCbk );
          /*** Pick color button ***/
    col = createColorButton ( pan, gui,
                                   EDITDIFFUSECOLOR,
                                   104,  0, 96, 18,
                                   solidColorChangeCbk );

          /*** Use image as texture ***/
          createRadioLabel ( pan, gui,
                                   EDITDIFFUSEIMAGE,
                                   btn,
                                     0, 24, 96, 18,
                                   imageColorCbk );

          /*** Image chooser button **/
          createPushButton  ( pan, gui, 
                                   EDITDIFFUSEIMAGE,
                                   104, 24, 96, 18,
                                   chooseImageCbk );


    return pan;
}

/******************************************************************************/
void updateMaterialChannels ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( strcmp ( child_name, EDITSPECULARITY  ) == 0x00 ) {
            updateSpecularityPanel ( child, gui );
        }

        if ( strcmp ( child_name, EDITDIFFUSE      ) == 0x00 ) {
            updateDiffuseColorPanel ( child, gui );
        }

        if ( strcmp ( child_name, EDITDISPLACEMENT ) == 0x00 ) {
            updateDisplacementPanel ( child, gui );
        }

        if ( strcmp ( child_name, EDITREFLECTION ) == 0x00 ) {
            updateReflectionPanel ( child, gui );
        }

        if ( strcmp ( child_name, EDITREFRACTION ) == 0x00 ) {
            updateRefractionPanel ( child, gui );
        }

        children =  g_list_next ( children );
    }
}

/******************************************************************************/
void updateMaterialEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DMATERIAL *mat = gui->selmat;

    gui->lock = 0x01;

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

    g_signal_connect ( G_OBJECT (tab), "realize", G_CALLBACK (Realize), gui );

    createDiffuseColorPanel ( tab, gui, EDITDIFFUSE     , 0, 0, width, height );
    createSpecularityPanel  ( tab, gui, EDITSPECULARITY , 0, 0, width, height );
    createDisplacementPanel ( tab, gui, EDITDISPLACEMENT, 0, 0, width, height );
    createReflectionPanel   ( tab, gui, EDITREFLECTION  , 0, 0, width, height );
    createRefractionPanel   ( tab, gui, EDITREFRACTION  , 0, 0, width, height );

    gtk_widget_show ( tab );


    gtk_widget_show ( frm );

    list_insert ( &gui->lmatedit, frm );

    return frm;
}
