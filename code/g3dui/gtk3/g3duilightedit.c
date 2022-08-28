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

#define EDITLIGHTGENERAL           "General"
#define EDITLIGHTINTENSITY         "Intensity"
#define EDITLIGHTCASTSHADOWS       "Cast shadows"
#define EDITLIGHTCOLOR             "Light RGB Color"
#define EDITLIGHTSPECULARITY       "Light RGB Specularity"
#define EDITLIGHTSPOTENABLED       "Spot Light"
#define EDITLIGHTSPOTLENGTH        "Spot length"
#define EDITLIGHTSPOTANGLE         "Spot angle"
#define EDITLIGHTSPOTFADEANGLE     "Spot fade angle"
#define EDITLIGHTSHADOWS           "Shadows"
#define EDITLIGHTHARDSHADOWS       "Hard Shadows"
#define EDITLIGHTSOFTSHADOWS       "Area Shadows"
#define EDITLIGHTSOFTSHADOWSRADIUS "Area radius"
#define EDITLIGHTSOFTSHADOWSSAMPLE "Sample count"

/******************************************************************************/
static GTK3G3DUILIGHTEDIT *gtk3_g3duilightedit_new ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUILIGHTEDIT *gtk3led = calloc ( 0x01, sizeof ( GTK3G3DUILIGHTEDIT ) );

    if ( gtk3led == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3led->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3led; 
}

#ifdef unused
/******************************************************************************/
static void updateShadowsPanel ( GTK3G3DUILIGHTEDIT *gtk3led ) {
    gtk3led->core.gui->lock = 0x01;

    if ( gtk3led->editedLight ) {
        if ( ((G3DOBJECT*)gtk3led->editedLight)->flags & LIGHTCASTSHADOWS ) {
            gtk_widget_set_sensitive ( gtk3led->softShadowsToggle, TRUE );

            if ( ((G3DOBJECT*)gtk3led->editedLight)->flags & LIGHTSOFTSHADOWS ) {
                gtk_widget_set_sensitive     ( gtk3led->shadowRadiusEntry, TRUE );
                gtk_widget_set_sensitive     ( gtk3led->shadowSampleEntry, TRUE );

                gtk_toggle_button_set_active ( gtk3led->softShadowsToggle, TRUE  );
            } else {
                gtk_widget_set_sensitive ( gtk3led->shadowRadiusEntry, FALSE );
                gtk_widget_set_sensitive ( gtk3led->shadowSampleEntry, FALSE );

                gtk_toggle_button_set_active ( gtk3led->softShadowsToggle, FALSE  );
            }
        } else {
            gtk_widget_set_sensitive ( gtk3led->softShadowsToggle, FALSE );
            gtk_widget_set_sensitive ( gtk3led->shadowRadiusEntry, FALSE );
            gtk_widget_set_sensitive ( gtk3led->shadowSampleEntry, FALSE );
        }

        gtk_spin_button_set_value  ( gtk3led->shadowRadiusEntry, 
                                     gtk3led->editedLight->shadowRadius );

        gtk_spin_button_set_value  ( gtk3led->shadowSampleEntry, 
                                     gtk3led->editedLight->shadowSample );
    } else {
        gtk_toggle_button_set_active ( gtk3led->softShadowsToggle, FALSE  );

        gtk_widget_set_sensitive ( gtk3led->softShadowsToggle, FALSE );
        gtk_widget_set_sensitive ( gtk3led->shadowRadiusEntry, FALSE );
        gtk_widget_set_sensitive ( gtk3led->shadowSampleEntry, FALSE );
    }


    gtk3led->core.gui->lock = 0x00;
}

/******************************************************************************/
static void setShadowTypeCbk ( GtkWidget *widget,
                               gpointer   user_data ) {
    GTK3G3DUILIGHTEDIT *gtk3led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    G3DUI *gui = gtk3led->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    char *wname = gtk_widget_get_name ( widget );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3led->core.gui->lock ) return;

    if ( gtk3led->editedLight ) {
        common_g3duilightedit_setSoftShadowsCbk ( gtk3led->core.gui,
                                                  gtk3led->editedLight );
    }

    updateShadowsPanel ( gtk3led );
}

/******************************************************************************/
static void shadowRadiusCbk ( GtkWidget *widget,
                              gpointer   user_data ) {
    GTK3G3DUILIGHTEDIT *gtk3led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    G3DUI *gui = gtk3led->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float shadowRadius = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3led->core.gui->lock ) return;

    if ( gtk3led->editedLight ) {
        common_g3duilightedit_shadowRadiusCbk ( gtk3led->core.gui,
                                                gtk3led->editedLight,
                                                shadowRadius );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void shadowSampleCbk ( GtkWidget *widget,
                              gpointer   user_data ) {
    GTK3G3DUILIGHTEDIT *gtk3led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    G3DUI *gui = gtk3led->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    uint32_t shadowSample = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3led->core.gui->lock ) return;

    if ( gtk3led->editedLight ) {
        common_g3duilightedit_shadowSampleCbk ( gtk3led->core.gui,
                                                gtk3led->editedLight,
                                                shadowSample );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static GtkWidget *createShadowsPanel ( GtkWidget      *parent, 
                                       GTK3G3DUILIGHTEDIT *gtk3led,
                                       char           *name,
                                       gint            x,
                                       gint            y,
                                       gint            width,
                                       gint            height ) {
    GtkWidget *pan = createPanel ( parent, gtk3led, name, x, y, width, height );

    gtk3led->softShadowsToggle = createToggleLabel ( pan,
                                                 gtk3led,
                                                 EDITLIGHTSOFTSHADOWS,
                                                 0, 0, 20, 20,
                                                 setShadowTypeCbk );

    gtk3led->shadowRadiusEntry = createFloatText ( pan,
                                               gtk3led,
                                               EDITLIGHTSOFTSHADOWSRADIUS,
                                               0.0f, FLT_MAX,
                                               0, 24, 96, 96,
                                               shadowRadiusCbk );

    gtk3led->shadowSampleEntry = createIntegerText ( pan,
                                                gtk3led,
                                                EDITLIGHTSOFTSHADOWSSAMPLE,
                                                1.0f, 32.0f,
                                                0, 48, 96, 96,
                                                shadowSampleCbk );

    return pan;
}
#endif

/******************************************************************************/
static void specularityChangeCbk ( GtkWidget *widget,
                                   gpointer   user_data ) {
    GTK3G3DUILIGHTEDIT *gtk3led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    G3DUI *gui = gtk3led->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER(widget);
    GdkRGBA color;

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3led->core.gui->lock ) return;

    gtk_color_chooser_get_rgba ( ccr, &color );

    if ( gtk3led->editedLight ) {
        g3duilightedit_setSpecularityCbk ( &gtk3led->core, 
                                            color.red   * 255,
                                            color.green * 255,
                                            color.blue  * 255 );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void updateSpecularityPanel ( GTK3G3DUILIGHTEDIT *gtk3led ) {
    gtk3led->core.gui->lock = 0x01;

    if ( gtk3led->core.multi ) {
        gtk_widget_set_sensitive ( gtk3led->specularColorButton, TRUE );

        gtk_color_chooser_set_rgba ( gtk3led->specularColorButton, &GDKRGBA_MULTI );
    } else {
        GdkRGBA rgba = { .red   = ( float ) gtk3led->editedLight->specularColor.r / 255.0f,
                         .green = ( float ) gtk3led->editedLight->specularColor.g / 255.0f,
                         .blue  = ( float ) gtk3led->editedLight->specularColor.b / 255.0f,
                         .alpha = ( float ) gtk3led->editedLight->specularColor.a / 255.0f };

        gtk_widget_set_sensitive ( gtk3led->specularColorButton, TRUE );

        gtk_color_chooser_set_rgba ( gtk3led->specularColorButton, &rgba );
    }


    gtk3led->core.gui->lock = 0x00;
}

/******************************************************************************/
static void createSpecularityPanel ( GTK3G3DUILIGHTEDIT *gtk3led ) {
    GtkWidget *fixed = gtk_fixed_new ( );
    GtkWidget *label = gtk_label_new ( EDITSPECULAR );

    gtk_widget_set_name ( fixed, EDITSPECULAR );

    gtk_notebook_append_page ( GTK_NOTEBOOK(gtk3led->tab), fixed, label );

    /*gtk_widget_set_size_request ( fixed, 300, 300 );*/

    ui_createSimpleLabel ( fixed, 
                           gtk3led,
                           EDITSPECULARCOLOR,
                           0,  0, 96, 18 );

    gtk3led->specularColorButton = ui_createColorButton ( fixed, 
                                                          gtk3led,
                                                          EDITSPECULARCOLOR,
                                                          104,  0, 96, 18,
                                                          specularityChangeCbk );

    gtk_widget_show ( fixed );
}

#ifdef unused
/******************************************************************************/
static void diffuseChangeCbk ( GtkWidget *widget,
                               gpointer   user_data ) {
    GTK3G3DUILIGHTEDIT *gtk3led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    G3DUI *gui = gtk3led->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER(widget);
    GdkRGBA color;

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3led->core.gui->lock ) return;

    gtk_color_chooser_get_rgba ( ccr, &color );

    if ( gtk3led->editedLight ) {
        common_g3dui_lightDiffuseChangeCbk ( gui,
                                             gtk3led->editedLight,
                                             color.red   * 255,
                                             color.green * 255,
                                             color.blue  * 255 );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void diffuseIntensityCbk ( GtkWidget *widget,
                                  gpointer   user_data ) {
    GTK3G3DUILIGHTEDIT *gtk3led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    G3DUI *gui = gtk3led->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float intensity = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevent useless primitive building when XmTextSetString is calgtk3led ***/
    if ( gtk3led->core.gui->lock ) return;

    if ( gtk3led->editedLight ) {
        gtk3led->editedLight->intensity = intensity;
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void updateLightGeneralPanel ( GTK3G3DUILIGHTEDIT *gtk3led ) {
    gtk3led->core.gui->lock = 0x01;

    if ( gtk3led->editedLight ) {
        gtk_widget_set_sensitive ( gtk3led->spotLengthEntry   , TRUE );
        gtk_widget_set_sensitive ( gtk3led->spotAngleEntry    , TRUE );
        gtk_widget_set_sensitive ( gtk3led->spotFadeAngleEntry, TRUE );
        gtk_widget_set_sensitive ( gtk3led->spotToggle        , TRUE );
        gtk_widget_set_sensitive ( gtk3led->castShadowsToggle , TRUE );

        if ( ((G3DOBJECT*)gtk3led->editedLight)->flags & LIGHTCASTSHADOWS ) {
            gtk_toggle_button_set_active ( gtk3led->castShadowsToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( gtk3led->castShadowsToggle, FALSE );
        }

        if ( ((G3DOBJECT*)gtk3led->editedLight)->flags & SPOTLIGHT ) {
            gtk_toggle_button_set_active ( gtk3led->spotToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( gtk3led->spotToggle, FALSE );
        }

        gtk_spin_button_set_value ( gtk3led->spotLengthEntry   , 
                                    gtk3led->editedLight->spotLength );

        gtk_spin_button_set_value ( gtk3led->spotAngleEntry    , 
                                    gtk3led->editedLight->spotAngle );

        gtk_spin_button_set_value ( gtk3led->spotFadeAngleEntry,
                                    gtk3led->editedLight->spotFadeAngle );
    } else {
        gtk_widget_set_sensitive ( gtk3led->spotLengthEntry   , FALSE );
        gtk_widget_set_sensitive ( gtk3led->spotAngleEntry    , FALSE );
        gtk_widget_set_sensitive ( gtk3led->spotFadeAngleEntry, FALSE );
        gtk_widget_set_sensitive ( gtk3led->spotToggle        , FALSE );
        gtk_widget_set_sensitive ( gtk3led->castShadowsToggle , FALSE );
    }

    gtk3led->core.gui->lock = 0x00;
}

/******************************************************************************/
static void updateDiffuseColorPanel ( GTK3G3DUILIGHTEDIT *gtk3led ) {
    gtk3led->core.gui->lock = 0x01;

    if ( gtk3led->editedLight ) {
        GdkRGBA rgba = { .red   = ( float ) gtk3led->editedLight->diffuseColor.r / 255.0f,
                         .green = ( float ) gtk3led->editedLight->diffuseColor.g / 255.0f,
                         .blue  = ( float ) gtk3led->editedLight->diffuseColor.b / 255.0f,
                         .alpha = ( float ) gtk3led->editedLight->diffuseColor.a / 255.0f };

        gtk_widget_set_sensitive ( gtk3led->intensityEntry    , TRUE );
        gtk_widget_set_sensitive ( gtk3led->diffuseColorButton, TRUE );

        gtk_spin_button_set_value  ( gtk3led->intensityEntry    , 
                                     gtk3led->editedLight->intensity );

        gtk_color_chooser_set_rgba ( gtk3led->diffuseColorButton, &rgba );
    } else {
        gtk_widget_set_sensitive ( gtk3led->intensityEntry    , FALSE );
        gtk_widget_set_sensitive ( gtk3led->diffuseColorButton, FALSE );
    }

    gtk3led->core.gui->lock = 0x00;
}

/******************************************************************************/
static GtkWidget *createDiffuseColorPanel ( GtkWidget      *parent, 
                                            GTK3G3DUILIGHTEDIT *gtk3led,
                                            char           *name,
                                            gint            x,
                                            gint            y,
                                            gint            width,
                                            gint            height ) {
    GtkWidget *pan = createPanel ( parent, gtk3led, name, x, y, width, height );

    createSimpleLabel ( pan,
                        gtk3led,
                        EDITDIFFUSECOLOR,
                        0,  0, 96, 18 );

    gtk3led->diffuseColorButton = createColorButton ( pan, 
                                                  gtk3led,
                                                  EDITDIFFUSECOLOR,
                                                  104,  0, 96, 18,
                                                  diffuseChangeCbk );

    gtk3led->intensityEntry = createFloatText ( pan,
                                            gtk3led,
                                            EDITLIGHTINTENSITY,
                                            0.0f, FLT_MAX,
                                            0, 48, 96, 96,
                                            diffuseIntensityCbk );

    return pan;
}

/******************************************************************************/
static void castShadowsCbk  ( GtkWidget *widget,
                              gpointer   user_data ) {
    GTK3G3DUILIGHTEDIT *gtk3led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    G3DUI *gui = gtk3led->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3led->core.gui->lock ) return;

    if ( gtk3led->editedLight ) {
        common_g3duilightedit_castShadowsCbk ( gtk3led->core.gui, 
                                               gtk3led->editedLight );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );

    updateShadowsPanel ( gtk3led );
}

/******************************************************************************/
static void spotLengthCbk ( GtkWidget *widget,
                            gpointer   user_data ) {
    GTK3G3DUILIGHTEDIT *gtk3led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    G3DUI *gui = gtk3led->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float spotLength = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3led->core.gui->lock ) return;

    if ( gtk3led->editedLight ) {
        common_g3duilightedit_setSpotCbk ( gtk3led->core.gui,
                                           gtk3led->editedLight,
                                           spotLength, 
                                           gtk3led->editedLight->spotAngle, 
                                           gtk3led->editedLight->spotFadeAngle );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void spotFadeAngleCbk ( GtkWidget *widget,
                               gpointer   user_data ) {
    GTK3G3DUILIGHTEDIT *gtk3led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    G3DUI *gui = gtk3led->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float spotFadeAngle = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3led->core.gui->lock ) return;

    if ( gtk3led->editedLight ) {
        common_g3duilightedit_setSpotCbk ( gtk3led->core.gui,
                                           gtk3led->editedLight,
                                           gtk3led->editedLight->spotLength, 
                                           gtk3led->editedLight->spotAngle, 
                                           spotFadeAngle );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void spotAngleCbk ( GtkWidget *widget,
                           gpointer   user_data ) {
    GTK3G3DUILIGHTEDIT *gtk3led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    G3DUI *gui = gtk3led->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float spotAngle = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3led->core.gui->lock ) return;

    if ( gtk3led->editedLight ) {
        common_g3duilightedit_setSpotCbk ( gtk3led->core.gui,
                                           gtk3led->editedLight,
                                           gtk3led->editedLight->spotLength, 
                                           spotAngle, 
                                           gtk3led->editedLight->spotFadeAngle );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void spotToggleCbk ( GtkWidget *widget,
                            gpointer   user_data ) {
    GTK3G3DUILIGHTEDIT *gtk3led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    G3DUI *gui = gtk3led->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3led->core.gui->lock ) return;

    if ( gtk3led->editedLight ) {
        if ( gtk3led->editedLight->obj.flags & SPOTLIGHT ) {
            common_g3duilightedit_unsetSpotCbk ( gtk3led->core.gui,
                                                 gtk3led->editedLight );
        } else {
            common_g3duilightedit_setSpotCbk ( gtk3led->core.gui,
                                               gtk3led->editedLight,
                                               gtk3led->editedLight->spotLength, 
                                               gtk3led->editedLight->spotAngle, 
                                               gtk3led->editedLight->spotFadeAngle );
        }
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static GtkWidget *createLightGeneralPanel ( GtkWidget          *parent, 
                                            GTK3G3DUILIGHTEDIT *gtk3led,
                                            char               *name,
                                            gint                x,
                                            gint                y,
                                            gint                width,
                                            gint                height ) {
    GtkWidget *pan = createPanel ( parent, gtk3led, name, x, y, width, height );

    gtk3led->castShadowsToggle = createToggleLabel ( pan,
                                                 gtk3led,
                                                 EDITLIGHTCASTSHADOWS,
                                                 0, 0, 20, 20,
                                                 castShadowsCbk );

    gtk3led->spotToggle = createToggleLabel ( pan,
                                          gtk3led,
                                          EDITLIGHTSPOTENABLED,
                                          0,  24, 192, 18,
                                          spotToggleCbk );

    gtk3led->spotLengthEntry = createFloatText ( pan,
                                             gtk3led,
                                             EDITLIGHTSPOTLENGTH,
                                             0.0f, FLT_MAX,
                                             0, 48, 96, 96,
                                             spotLengthCbk );

    gtk3led->spotAngleEntry = createFloatText ( pan,
                                            gtk3led,
                                            EDITLIGHTSPOTANGLE,
                                            0.0f, FLT_MAX,
                                            0, 72, 96, 96,
                                            spotAngleCbk );

    gtk3led->spotFadeAngleEntry = createFloatText ( pan,
                                                gtk3led,
                                                EDITLIGHTSPOTFADEANGLE,
                                                0.0f, FLT_MAX,
                                                0, 96, 96, 96,
                                                spotFadeAngleCbk );



    return pan;
}
#endif
/******************************************************************************/
void gtk3_g3duilightedit_update ( GTK3G3DUILIGHTEDIT *gtk3led ) {
    G3DUI *gui = gtk3led->core.gui;

    gui->lock = 0x01;

    if ( gui->sce ) {
        G3DSCENE *sce = gui->sce;
        uint32_t nbsel = list_count ( sce->lsel );

        if ( nbsel ) {
            gtk_widget_set_sensitive ( gtk3led->tab, TRUE );

            if ( g3dobjectlist_checkType ( sce->lsel, G3DLIGHTTYPE ) ) {
                gtk3led->editedLight = g3dscene_getLastSelected ( sce );

                gtk3led->core.multi = ( nbsel > 0x01 ) ? 0x01 : 0x00;

                updateSpecularityPanel  ( gtk3led );
                /*updateDiffuseColorPanel ( gtk3led );
                updateLightGeneralPanel ( gtk3led );
                updateShadowsPanel      ( gtk3led );*/
            }
        } else {
            gtk_widget_set_sensitive ( gtk3led->tab, FALSE );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUILIGHTEDIT *gtk3led = ( GTK3G3DUILIGHTEDIT * ) user_data;

    free ( gtk3led );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUILIGHTEDIT *gtk3led = ( GTK3G3DUILIGHTEDIT * ) user_data;

    gtk3_g3duilightedit_update ( gtk3led );
}

/******************************************************************************/
GTK3G3DUILIGHTEDIT *gtk3_g3duilightedit_create ( GtkWidget *parent,
                                                 GTK3G3DUI *gtk3gui,
                                                 char      *name ) {
    GTK3G3DUILIGHTEDIT *gtk3led = gtk3_g3duilightedit_new ( gtk3gui );
    GtkWidget *tab = gtk_notebook_new ( );

    gtk3led->tab = tab;

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(tab), TRUE );

    gtk_widget_set_name ( tab, name );

    /*gtk_widget_set_size_request ( tab, width, height );*/

    g_signal_connect ( G_OBJECT (tab), "realize", G_CALLBACK (Realize), gtk3led );
    g_signal_connect ( G_OBJECT (tab), "destroy", G_CALLBACK (Destroy), gtk3led );

/*    createLightGeneralPanel ( tab, gtk3led, EDITLIGHTGENERAL, 0, 0, width, height );
    createDiffuseColorPanel ( tab, gtk3led, EDITDIFFUSE     , 0, 0, width, height );*/

    createSpecularityPanel  ( gtk3led );

/*    createShadowsPanel      ( tab, gtk3led, EDITLIGHTSHADOWS, 0, 0, width, height );*/

    gtk_widget_show ( tab );


    return gtk3led;
}
