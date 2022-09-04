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
#define EDITLIGHTSPOTLENGTH        "Length"
#define EDITLIGHTSPOTANGLE         "Angle"
#define EDITLIGHTSPOTFADEANGLE     "Fade angle"
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

/******************************************************************************/
static void updateShadowsPanel ( GTK3G3DUILIGHTEDIT *gtk3led ) {
    gtk3led->core.gui->lock = 0x01;

    if ( gtk3led->core.editedLight ) {
        if ( ((G3DOBJECT*)gtk3led->core.editedLight)->flags & LIGHTCASTSHADOWS ) {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3led->softShadowsToggle), TRUE );

            if ( ((G3DOBJECT*)gtk3led->core.editedLight)->flags & LIGHTSOFTSHADOWS ) {
                gtk_widget_set_sensitive     ( GTK_WIDGET(gtk3led->shadowRadiusEntry), TRUE );
                gtk_widget_set_sensitive     ( GTK_WIDGET(gtk3led->shadowSampleEntry), TRUE );

                gtk_toggle_button_set_active ( gtk3led->softShadowsToggle, TRUE  );
            } else {
                gtk_widget_set_sensitive ( GTK_WIDGET(gtk3led->shadowRadiusEntry), FALSE );
                gtk_widget_set_sensitive ( GTK_WIDGET(gtk3led->shadowSampleEntry), FALSE );

                gtk_toggle_button_set_active ( gtk3led->softShadowsToggle, FALSE  );
            }
        } else {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3led->softShadowsToggle), FALSE );
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3led->shadowRadiusEntry), FALSE );
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3led->shadowSampleEntry), FALSE );
        }

        gtk_spin_button_set_value  ( gtk3led->shadowRadiusEntry, 
                                     gtk3led->core.editedLight->shadowRadius );

        gtk_spin_button_set_value  ( gtk3led->shadowSampleEntry, 
                                     gtk3led->core.editedLight->shadowSample );
    }
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

    if ( gtk3led->core.editedLight ) {
        g3duilightedit_setSoftShadowsCbk ( &gtk3led->core );
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

    if ( gtk3led->core.editedLight ) {
        g3duilightedit_shadowRadiusCbk ( &gtk3led->core, shadowRadius );
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

    if ( gtk3led->core.editedLight ) {
        g3duilightedit_shadowSampleCbk ( &gtk3led->core, shadowSample );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void createShadowsPanel ( GTK3G3DUILIGHTEDIT *gtk3led,
                                 uint32_t            x,
                                 uint32_t            y,
                                 uint32_t            width,
                                 uint32_t            height ) {

    GtkFixed *pan = ui_createTab ( gtk3led->notebook,
                                   gtk3led,
                                   EDITLIGHTSHADOWS,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3led->core.gui->lock = 0x01;

    gtk3led->softShadowsToggle = ui_createToggleLabel ( pan,
                                                        gtk3led,
                                                        EDITLIGHTSOFTSHADOWS,
                                                        CLASS_MAIN,
                                                        0, 0, 96, 20, 20,
                                                        setShadowTypeCbk );

    gtk3led->shadowRadiusEntry = ui_createFloatText ( pan,
                                                      gtk3led,
                                                      EDITLIGHTSOFTSHADOWSRADIUS,
                                                      CLASS_MAIN,
                                                      0.0f, FLT_MAX,
                                                      0, 24, 96, 96, 20,
                                                      shadowRadiusCbk );

    gtk3led->shadowSampleEntry = ui_createIntegerText ( pan,
                                                        gtk3led,
                                                        EDITLIGHTSOFTSHADOWSSAMPLE,
                                                        CLASS_MAIN,
                                                        1.0f, 32.0f,
                                                        0, 48, 96, 96, 20,
                                                        shadowSampleCbk );
    gtk3led->core.gui->lock = 0x00;
}


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

    if ( gtk3led->core.editedLight ) {
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

    if ( gtk3led->core.editedLight ) {
        if ( gtk3led->core.multi ) {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3led->specularColorButton), TRUE );

            gtk_color_chooser_set_rgba ( GTK_COLOR_CHOOSER(gtk3led->specularColorButton), &GDKRGBA_MULTI );
        } else {
            GdkRGBA rgba = { .red   = ( gdouble ) gtk3led->core.editedLight->specularColor.r / 255.0f,
                             .green = ( gdouble ) gtk3led->core.editedLight->specularColor.g / 255.0f,
                             .blue  = ( gdouble ) gtk3led->core.editedLight->specularColor.b / 255.0f,
                             .alpha = ( gdouble ) gtk3led->core.editedLight->specularColor.a / 255.0f };

            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3led->specularColorButton), TRUE );

            gtk_color_chooser_set_rgba ( GTK_COLOR_CHOOSER(gtk3led->specularColorButton), &rgba );
        }
    }

    gtk3led->core.gui->lock = 0x00;
}

/******************************************************************************/
static void createSpecularityPanel ( GTK3G3DUILIGHTEDIT *gtk3led,
                                     uint32_t            x,
                                     uint32_t            y,
                                     uint32_t            width,
                                     uint32_t            height ) {

    GtkFixed *pan = ui_createTab ( gtk3led->notebook,
                                   gtk3led,
                                   EDITSPECULAR,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3led->core.gui->lock = 0x01;

    gtk3led->specularColorButton = ui_createColorButton ( pan, 
                                                          gtk3led,
                                                          EDITSPECULARCOLOR,
                                                          CLASS_MAIN,
                                                          0,  0, 96, 20, 20,
                                                          specularityChangeCbk );
    gtk3led->core.gui->lock = 0x00;
}

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

    if ( gtk3led->core.editedLight ) {
        g3duilightedit_setDiffuseCbk ( &gtk3led->core,
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

    if ( gtk3led->core.editedLight ) {
        gtk3led->core.editedLight->intensity = intensity;
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void updateDiffuseColorPanel ( GTK3G3DUILIGHTEDIT *gtk3led ) {
    GdkRGBA rgba = { .red   = ( float ) gtk3led->core.editedLight->diffuseColor.r / 255.0f,
                     .green = ( float ) gtk3led->core.editedLight->diffuseColor.g / 255.0f,
                     .blue  = ( float ) gtk3led->core.editedLight->diffuseColor.b / 255.0f,
                     .alpha = ( float ) gtk3led->core.editedLight->diffuseColor.a / 255.0f };

    gtk3led->core.gui->lock = 0x01;

    if ( gtk3led->core.editedLight ) {
        gtk_widget_set_sensitive ( GTK_WIDGET(gtk3led->intensityEntry)    , TRUE );
        gtk_widget_set_sensitive ( GTK_WIDGET(gtk3led->diffuseColorButton), TRUE );

        gtk_spin_button_set_value  ( gtk3led->intensityEntry    , 
                                     gtk3led->core.editedLight->intensity );

        gtk_color_chooser_set_rgba ( GTK_COLOR_CHOOSER(gtk3led->diffuseColorButton), &rgba );
    }

    gtk3led->core.gui->lock = 0x00;
}

/******************************************************************************/
static void createDiffuseColorPanel( GTK3G3DUILIGHTEDIT *gtk3led,
                                     uint32_t            x,
                                     uint32_t            y,
                                     uint32_t            width,
                                     uint32_t            height ) {
    GtkFixed *pan = ui_createTab ( gtk3led->notebook,
                                   gtk3led,
                                   EDITDIFFUSE,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3led->diffuseColorButton = ui_createColorButton ( pan, 
                                                         gtk3led,
                                                         EDITDIFFUSECOLOR,
                                                         CLASS_MAIN,
                                                         0,  0, 96, 20, 20,
                                                         diffuseChangeCbk );
}

/******************************************************************************/
static void castShadowsCbk  ( GtkWidget *widget,
                              gpointer   user_data ) {
    GTK3G3DUILIGHTEDIT *gtk3led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    G3DUI *gui = gtk3led->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3led->core.gui->lock ) return;

    if ( gtk3led->core.editedLight ) {
        g3duilightedit_castShadowsCbk ( &gtk3led->core );
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

    if ( gtk3led->core.editedLight ) {
        g3duilightedit_setSpotLengthCbk ( &gtk3led->core, spotLength );
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

    if ( gtk3led->core.editedLight ) {
        g3duilightedit_setSpotFadeAngleCbk ( &gtk3led->core, spotFadeAngle );
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

    if ( gtk3led->core.editedLight ) {
        g3duilightedit_setSpotAngleCbk ( &gtk3led->core, spotAngle );
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

    if ( gtk3led->core.editedLight ) {
        if ( gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON(widget) ) ) {
            g3duilightedit_unsetSpotCbk ( &gtk3led->core );
        } else {
            g3duilightedit_setSpotCbk ( &gtk3led->core );
        }
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void updateLightGeneralPanel ( GTK3G3DUILIGHTEDIT *gtk3led ) {
    gtk3led->core.gui->lock = 0x01;

    if ( gtk3led->core.editedLight ) {
        if ( ((G3DOBJECT*)gtk3led->core.editedLight)->flags & LIGHTCASTSHADOWS ) {
            gtk_toggle_button_set_active ( gtk3led->castShadowsToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( gtk3led->castShadowsToggle, FALSE );
        }

        if ( ((G3DOBJECT*)gtk3led->core.editedLight)->flags & SPOTLIGHT ) {
            gtk_toggle_button_set_active ( gtk3led->spotToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( gtk3led->spotToggle, FALSE );
        }

        gtk_spin_button_set_value ( gtk3led->spotLengthEntry   , 
                                    gtk3led->core.editedLight->spotLength );

        gtk_spin_button_set_value ( gtk3led->spotAngleEntry    , 
                                    gtk3led->core.editedLight->spotAngle );

        gtk_spin_button_set_value ( gtk3led->spotFadeAngleEntry,
                                    gtk3led->core.editedLight->spotFadeAngle );
    }

    gtk3led->core.gui->lock = 0x00;
}


/******************************************************************************/
static void createLightGeneralPanel ( GTK3G3DUILIGHTEDIT *gtk3led,
                                      gint                x,
                                      gint                y,
                                      gint                width,
                                      gint                height ) {
    GtkFixed *pan = ui_createTab ( gtk3led->notebook,
                                   gtk3led,
                                   EDITLIGHTGENERAL,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );


    gtk3led->castShadowsToggle  = ui_createToggleLabel ( pan,
                                                         gtk3led,
                                                         EDITLIGHTCASTSHADOWS,
                                                         CLASS_MAIN,
                                                         0, 0, 96, 20, 20,
                                                         castShadowsCbk );

    gtk3led->intensityEntry = ui_createFloatText ( pan,
                                                   gtk3led,
                                                   EDITLIGHTINTENSITY,
                                                   CLASS_MAIN,
                                                   0.0f, FLT_MAX,
                                                   0, 24, 96, 96, 20,
                                                   diffuseIntensityCbk );

    gtk3led->spotToggle         = ui_createToggleLabel ( pan,
                                                         gtk3led,
                                                         EDITLIGHTSPOTENABLED,
                                                         CLASS_MAIN,
                                                         0,  48, 96, 20, 20,
                                                         spotToggleCbk );

    gtk3led->spotLengthEntry    = ui_createFloatText  ( pan,
                                                        gtk3led,
                                                        EDITLIGHTSPOTLENGTH,
                                                        CLASS_MAIN,
                                                        0.0f, FLT_MAX,
                                                        0, 72, 96, 96, 20,
                                                        spotLengthCbk );

    gtk3led->spotAngleEntry     = ui_createFloatText  ( pan,
                                                        gtk3led,
                                                        EDITLIGHTSPOTANGLE,
                                                        CLASS_MAIN,
                                                        0.0f, FLT_MAX,
                                                        0, 96, 96, 96, 20,
                                                        spotAngleCbk );

    gtk3led->spotFadeAngleEntry = ui_createFloatText  ( pan,
                                                        gtk3led,
                                                        EDITLIGHTSPOTFADEANGLE,
                                                        CLASS_MAIN,
                                                        0.0f, FLT_MAX,
                                                        0, 120, 96, 96, 20,
                                                        spotFadeAngleCbk );
}

/******************************************************************************/
void gtk3_g3duilightedit_update ( GTK3G3DUILIGHTEDIT *gtk3led ) {
    G3DUI *gui = gtk3led->core.gui;

    gui->lock = 0x01;

    if ( gui->sce ) {
        G3DSCENE *sce = gui->sce;
        uint32_t nbsel = list_count ( sce->lsel );

        if ( nbsel ) {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3led->notebook), TRUE );

            if ( g3dobjectlist_checkType ( sce->lsel, G3DLIGHTTYPE ) ) {
                gtk3led->core.multi = ( nbsel > 0x01 ) ? 0x01 : 0x00;

                gtk3led->core.editedLight = ( G3DLIGHT * ) g3dscene_getLastSelected ( sce );

                if ( gtk3led->core.editedLight ) {
                    updateSpecularityPanel  ( gtk3led );
                    updateDiffuseColorPanel ( gtk3led );
                    updateLightGeneralPanel ( gtk3led );
                    updateShadowsPanel      ( gtk3led );
                }
            }
        } else {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3led->notebook), FALSE );
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
    GtkWidget *notebook = gtk_notebook_new ( );

    gtk3led->notebook = GTK_NOTEBOOK(notebook);

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(notebook), TRUE );

    gtk_widget_set_name ( notebook, name );

    /*gtk_widget_set_size_request ( tab, width, height );*/

    g_signal_connect ( G_OBJECT (notebook), "realize", G_CALLBACK (Realize), gtk3led );
    g_signal_connect ( G_OBJECT (notebook), "destroy", G_CALLBACK (Destroy), gtk3led );

    createLightGeneralPanel ( gtk3led, 0, 0, 310, 150 );
    createDiffuseColorPanel ( gtk3led, 0, 0, 310, 150 );
    createSpecularityPanel  ( gtk3led, 0, 0, 310, 150 );
    createShadowsPanel      ( gtk3led, 0, 0, 310, 150 );


    gtk_widget_show ( notebook );


    return gtk3led;
}
