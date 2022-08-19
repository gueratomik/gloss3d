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
typedef struct _GTK3G3DUILIGHTEDIT {
    G3DUILIGHTEDIT grp;

    GtkWidget       *main;
    GtkWidget       *diffuseColorButton;
    GtkWidget       *specularColorButton;
    GtkWidget       *intensityEntry;
    GtkWidget       *castShadowsToggle;
    GtkWidget       *spotToggle;
    GtkWidget       *spotLengthEntry;
    GtkWidget       *spotAngleEntry;
    GtkWidget       *spotFadeAngleEntry;
    GtkWidget       *softShadowsToggle;
    GtkWidget       *shadowRadiusEntry;
    GtkWidget       *shadowSampleEntry;

    G3DLIGHT        *editedLight;
} GTK3G3DUILIGHTEDIT;

/******************************************************************************/
static GTK3G3DUILIGHTEDIT *g3duilightedit_new ( G3DUI *gui ) {
    GTK3G3DUILIGHTEDIT *led = calloc ( 0x01, sizeof ( GTK3G3DUILIGHTEDIT ) );

    if ( led == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );
    }

    led->grp.gui = gui;

    return led; 
}

/******************************************************************************/
static void updateShadowsPanel ( GTK3G3DUILIGHTEDIT *led ) {
    led->grp.gui->lock = 0x01;

    if ( led->editedLight ) {
        if ( ((G3DOBJECT*)led->editedLight)->flags & LIGHTCASTSHADOWS ) {
            gtk_widget_set_sensitive ( led->softShadowsToggle, TRUE );

            if ( ((G3DOBJECT*)led->editedLight)->flags & LIGHTSOFTSHADOWS ) {
                gtk_widget_set_sensitive     ( led->shadowRadiusEntry, TRUE );
                gtk_widget_set_sensitive     ( led->shadowSampleEntry, TRUE );

                gtk_toggle_button_set_active ( led->softShadowsToggle, TRUE  );
            } else {
                gtk_widget_set_sensitive ( led->shadowRadiusEntry, FALSE );
                gtk_widget_set_sensitive ( led->shadowSampleEntry, FALSE );

                gtk_toggle_button_set_active ( led->softShadowsToggle, FALSE  );
            }
        } else {
            gtk_widget_set_sensitive ( led->softShadowsToggle, FALSE );
            gtk_widget_set_sensitive ( led->shadowRadiusEntry, FALSE );
            gtk_widget_set_sensitive ( led->shadowSampleEntry, FALSE );
        }

        gtk_spin_button_set_value  ( led->shadowRadiusEntry, 
                                     led->editedLight->shadowRadius );

        gtk_spin_button_set_value  ( led->shadowSampleEntry, 
                                     led->editedLight->shadowSample );
    } else {
        gtk_toggle_button_set_active ( led->softShadowsToggle, FALSE  );

        gtk_widget_set_sensitive ( led->softShadowsToggle, FALSE );
        gtk_widget_set_sensitive ( led->shadowRadiusEntry, FALSE );
        gtk_widget_set_sensitive ( led->shadowSampleEntry, FALSE );
    }


    led->grp.gui->lock = 0x00;
}

/******************************************************************************/
static void setShadowTypeCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUILIGHTEDIT *led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    char *wname = gtk_widget_get_name ( widget );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( led->grp.gui->lock ) return;

    if ( led->editedLight ) {
        common_g3duilightedit_setSoftShadowsCbk ( led->grp.gui,
                                                  led->editedLight );
    }

    updateShadowsPanel ( led );
}

/******************************************************************************/
static void shadowRadiusCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUILIGHTEDIT *led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    float shadowRadius = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( led->grp.gui->lock ) return;

    if ( led->editedLight ) {
        common_g3duilightedit_shadowRadiusCbk ( led->grp.gui,
                                                led->editedLight,
                                                shadowRadius );
    }

    g3dui_redrawGLViews ( led->grp.gui );
}

/******************************************************************************/
static void shadowSampleCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUILIGHTEDIT *led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    uint32_t shadowSample = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( led->grp.gui->lock ) return;

    if ( led->editedLight ) {
        common_g3duilightedit_shadowSampleCbk ( led->grp.gui,
                                                led->editedLight,
                                                shadowSample );
    }

    g3dui_redrawGLViews ( led->grp.gui );
}

/******************************************************************************/
static GtkWidget *createShadowsPanel ( GtkWidget      *parent, 
                                       GTK3G3DUILIGHTEDIT *led,
                                       char           *name,
                                       gint            x,
                                       gint            y,
                                       gint            width,
                                       gint            height ) {
    GtkWidget *pan = createPanel ( parent, led, name, x, y, width, height );

    led->softShadowsToggle = createToggleLabel ( pan,
                                                 led,
                                                 EDITLIGHTSOFTSHADOWS,
                                                 0, 0, 20, 20,
                                                 setShadowTypeCbk );

    led->shadowRadiusEntry = createFloatText ( pan,
                                               led,
                                               EDITLIGHTSOFTSHADOWSRADIUS,
                                               0.0f, FLT_MAX,
                                               0, 24, 96, 96,
                                               shadowRadiusCbk );

    led->shadowSampleEntry = createIntegerText ( pan,
                                                led,
                                                EDITLIGHTSOFTSHADOWSSAMPLE,
                                                1.0f, 32.0f,
                                                0, 48, 96, 96,
                                                shadowSampleCbk );

    return pan;
}

/******************************************************************************/
static void specularityChangeCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUILIGHTEDIT *led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    G3DUI *gui = ( G3DUI * ) led->grp.gui;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER(widget);
    GdkRGBA color;

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( led->grp.gui->lock ) return;

    gtk_color_chooser_get_rgba ( ccr, &color );

    if ( led->editedLight ) {
        common_g3dui_lightSpecularityChangeCbk ( gui, 
                                                 led->editedLight,
                                                 color.red   * 255,
                                                 color.green * 255,
                                                 color.blue  * 255 );
    }

    g3dui_redrawGLViews ( led->grp.gui );
}

/******************************************************************************/
static void updateSpecularityPanel ( GTK3G3DUILIGHTEDIT *led ) {
    led->grp.gui->lock = 0x01;

    if ( led->editedLight ) {
        GdkRGBA rgba = { .red   = ( float ) led->editedLight->specularColor.r / 255.0f,
                         .green = ( float ) led->editedLight->specularColor.g / 255.0f,
                         .blue  = ( float ) led->editedLight->specularColor.b / 255.0f,
                         .alpha = ( float ) led->editedLight->specularColor.a / 255.0f };

        gtk_widget_set_sensitive ( led->specularColorButton, TRUE );

        gtk_color_chooser_set_rgba ( led->specularColorButton, &rgba );
    } else {
        gtk_widget_set_sensitive ( led->specularColorButton, FALSE );
    }

    led->grp.gui->lock = 0x00;
}

/******************************************************************************/
static GtkWidget *createSpecularityPanel ( GtkWidget      *parent, 
                                           GTK3G3DUILIGHTEDIT *led,
                                           char           *name,
                                           gint            x,
                                           gint            y,
                                           gint            width,
                                           gint            height ) {
    GtkWidget *pan = createPanel ( parent, led, name, x, y, width, height );

    createSimpleLabel ( pan, 
                        led,
                        EDITSPECULARCOLOR,
                        0,  0, 96, 18 );

    led->specularColorButton = createColorButton ( pan, 
                                                   led,
                                                   EDITSPECULARCOLOR,
                                                   104,  0, 96, 18,
                                                   specularityChangeCbk );

    return pan;
}

/******************************************************************************/
static void diffuseChangeCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUILIGHTEDIT *led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    G3DUI *gui = ( G3DUI * ) led->grp.gui;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER(widget);
    GdkRGBA color;

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( led->grp.gui->lock ) return;

    gtk_color_chooser_get_rgba ( ccr, &color );

    if ( led->editedLight ) {
        common_g3dui_lightDiffuseChangeCbk ( gui,
                                             led->editedLight,
                                             color.red   * 255,
                                             color.green * 255,
                                             color.blue  * 255 );
    }

    g3dui_redrawGLViews ( led->grp.gui );
}

/******************************************************************************/
static void diffuseIntensityCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUILIGHTEDIT *led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    G3DUI *gui = ( G3DUI * ) led->grp.gui;
    float intensity = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( led->grp.gui->lock ) return;

    if ( led->editedLight ) {
        led->editedLight->intensity = intensity;
    }

    g3dui_redrawGLViews ( led->grp.gui );
}

/******************************************************************************/
static void updateLightGeneralPanel ( GTK3G3DUILIGHTEDIT *led ) {
    led->grp.gui->lock = 0x01;

    if ( led->editedLight ) {
        gtk_widget_set_sensitive ( led->spotLengthEntry   , TRUE );
        gtk_widget_set_sensitive ( led->spotAngleEntry    , TRUE );
        gtk_widget_set_sensitive ( led->spotFadeAngleEntry, TRUE );
        gtk_widget_set_sensitive ( led->spotToggle        , TRUE );
        gtk_widget_set_sensitive ( led->castShadowsToggle , TRUE );

        if ( ((G3DOBJECT*)led->editedLight)->flags & LIGHTCASTSHADOWS ) {
            gtk_toggle_button_set_active ( led->castShadowsToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( led->castShadowsToggle, FALSE );
        }

        if ( ((G3DOBJECT*)led->editedLight)->flags & SPOTLIGHT ) {
            gtk_toggle_button_set_active ( led->spotToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( led->spotToggle, FALSE );
        }

        gtk_spin_button_set_value ( led->spotLengthEntry   , 
                                    led->editedLight->spotLength );

        gtk_spin_button_set_value ( led->spotAngleEntry    , 
                                    led->editedLight->spotAngle );

        gtk_spin_button_set_value ( led->spotFadeAngleEntry,
                                    led->editedLight->spotFadeAngle );
    } else {
        gtk_widget_set_sensitive ( led->spotLengthEntry   , FALSE );
        gtk_widget_set_sensitive ( led->spotAngleEntry    , FALSE );
        gtk_widget_set_sensitive ( led->spotFadeAngleEntry, FALSE );
        gtk_widget_set_sensitive ( led->spotToggle        , FALSE );
        gtk_widget_set_sensitive ( led->castShadowsToggle , FALSE );
    }

    led->grp.gui->lock = 0x00;
}

/******************************************************************************/
static void updateDiffuseColorPanel ( GTK3G3DUILIGHTEDIT *led ) {
    led->grp.gui->lock = 0x01;

    if ( led->editedLight ) {
        GdkRGBA rgba = { .red   = ( float ) led->editedLight->diffuseColor.r / 255.0f,
                         .green = ( float ) led->editedLight->diffuseColor.g / 255.0f,
                         .blue  = ( float ) led->editedLight->diffuseColor.b / 255.0f,
                         .alpha = ( float ) led->editedLight->diffuseColor.a / 255.0f };

        gtk_widget_set_sensitive ( led->intensityEntry    , TRUE );
        gtk_widget_set_sensitive ( led->diffuseColorButton, TRUE );

        gtk_spin_button_set_value  ( led->intensityEntry    , 
                                     led->editedLight->intensity );

        gtk_color_chooser_set_rgba ( led->diffuseColorButton, &rgba );
    } else {
        gtk_widget_set_sensitive ( led->intensityEntry    , FALSE );
        gtk_widget_set_sensitive ( led->diffuseColorButton, FALSE );
    }

    led->grp.gui->lock = 0x00;
}

/******************************************************************************/
static GtkWidget *createDiffuseColorPanel ( GtkWidget      *parent, 
                                            GTK3G3DUILIGHTEDIT *led,
                                            char           *name,
                                            gint            x,
                                            gint            y,
                                            gint            width,
                                            gint            height ) {
    GtkWidget *pan = createPanel ( parent, led, name, x, y, width, height );

    createSimpleLabel ( pan,
                        led,
                        EDITDIFFUSECOLOR,
                        0,  0, 96, 18 );

    led->diffuseColorButton = createColorButton ( pan, 
                                                  led,
                                                  EDITDIFFUSECOLOR,
                                                  104,  0, 96, 18,
                                                  diffuseChangeCbk );

    led->intensityEntry = createFloatText ( pan,
                                            led,
                                            EDITLIGHTINTENSITY,
                                            0.0f, FLT_MAX,
                                            0, 48, 96, 96,
                                            diffuseIntensityCbk );

    return pan;
}

/******************************************************************************/
static void castShadowsCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUILIGHTEDIT *led = ( GTK3G3DUILIGHTEDIT * ) user_data;

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( led->grp.gui->lock ) return;

    if ( led->editedLight ) {
        common_g3duilightedit_castShadowsCbk ( led->grp.gui, 
                                               led->editedLight );
    }

    g3dui_redrawGLViews ( led->grp.gui );

    updateShadowsPanel ( led );
}

/******************************************************************************/
static void spotLengthCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUILIGHTEDIT *led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    float spotLength = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( led->grp.gui->lock ) return;

    if ( led->editedLight ) {
        common_g3duilightedit_setSpotCbk ( led->grp.gui,
                                           led->editedLight,
                                           spotLength, 
                                           led->editedLight->spotAngle, 
                                           led->editedLight->spotFadeAngle );
    }

    g3dui_redrawGLViews ( led->grp.gui );
}

/******************************************************************************/
static void spotFadeAngleCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUILIGHTEDIT *led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    float spotFadeAngle = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( led->grp.gui->lock ) return;

    if ( led->editedLight ) {
        common_g3duilightedit_setSpotCbk ( led->grp.gui,
                                           led->editedLight,
                                           led->editedLight->spotLength, 
                                           led->editedLight->spotAngle, 
                                           spotFadeAngle );
    }

    g3dui_redrawGLViews ( led->grp.gui );
}

/******************************************************************************/
static void spotAngleCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUILIGHTEDIT *led = ( GTK3G3DUILIGHTEDIT * ) user_data;
    float spotAngle = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( led->grp.gui->lock ) return;

    if ( led->editedLight ) {
        common_g3duilightedit_setSpotCbk ( led->grp.gui,
                                           led->editedLight,
                                           led->editedLight->spotLength, 
                                           spotAngle, 
                                           led->editedLight->spotFadeAngle );
    }

    g3dui_redrawGLViews ( led->grp.gui );
}

/******************************************************************************/
static void spotToggleCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUILIGHTEDIT *led = ( GTK3G3DUILIGHTEDIT * ) user_data;

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( led->grp.gui->lock ) return;

    if ( led->editedLight ) {
        if ( led->editedLight->obj.flags & SPOTLIGHT ) {
            common_g3duilightedit_unsetSpotCbk ( led->grp.gui,
                                                 led->editedLight );
        } else {
            common_g3duilightedit_setSpotCbk ( led->grp.gui,
                                               led->editedLight,
                                               led->editedLight->spotLength, 
                                               led->editedLight->spotAngle, 
                                               led->editedLight->spotFadeAngle );
        }
    }

    g3dui_redrawGLViews ( led->grp.gui );
}

/******************************************************************************/
static GtkWidget *createLightGeneralPanel ( GtkWidget      *parent, 
                                            GTK3G3DUILIGHTEDIT *led,
                                            char           *name,
                                            gint            x,
                                            gint            y,
                                            gint            width,
                                            gint            height ) {
    GtkWidget *pan = createPanel ( parent, led, name, x, y, width, height );

    led->castShadowsToggle = createToggleLabel ( pan,
                                                 led,
                                                 EDITLIGHTCASTSHADOWS,
                                                 0, 0, 20, 20,
                                                 castShadowsCbk );

    led->spotToggle = createToggleLabel ( pan,
                                          led,
                                          EDITLIGHTSPOTENABLED,
                                          0,  24, 192, 18,
                                          spotToggleCbk );

    led->spotLengthEntry = createFloatText ( pan,
                                             led,
                                             EDITLIGHTSPOTLENGTH,
                                             0.0f, FLT_MAX,
                                             0, 48, 96, 96,
                                             spotLengthCbk );

    led->spotAngleEntry = createFloatText ( pan,
                                            led,
                                            EDITLIGHTSPOTANGLE,
                                            0.0f, FLT_MAX,
                                            0, 72, 96, 96,
                                            spotAngleCbk );

    led->spotFadeAngleEntry = createFloatText ( pan,
                                                led,
                                                EDITLIGHTSPOTFADEANGLE,
                                                0.0f, FLT_MAX,
                                                0, 96, 96, 96,
                                                spotFadeAngleCbk );



    return pan;
}

/******************************************************************************/
void updateLightEdit ( GtkWidget *w, 
                       G3DLIGHT  *lig ) {
    GTK3G3DUILIGHTEDIT *led = ( GTK3G3DUILIGHTEDIT * ) g_object_get_data ( G_OBJECT(w),
                                                                   "private" );

    led->editedLight = lig;

    updateSpecularityPanel  ( led );
    updateDiffuseColorPanel ( led );
    updateLightGeneralPanel ( led );
    updateShadowsPanel      ( led );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUILIGHTEDIT *led = ( GTK3G3DUILIGHTEDIT * ) user_data;

    free ( led );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUILIGHTEDIT *led = ( GTK3G3DUILIGHTEDIT * ) user_data;

    updateLightEdit ( widget, NULL );
}

/******************************************************************************/
GtkWidget *createLightEdit ( GtkWidget *parent,
                             G3DUI     *gui,
                             char      *name,
                             gint       x,
                             gint       y,
                             gint       width,
                             gint       height ) {
    GdkRectangle gdkrec = { 0x00, 0x00, width, height };
    GtkWidget *tab = gtk_notebook_new ( );
    GTK3G3DUILIGHTEDIT *led = g3duilightedit_new ( gui );

    g_object_set_data ( G_OBJECT(tab), "private", (gpointer) led );

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(tab), TRUE );

    gtk_widget_set_name ( tab, name );

    gtk_widget_size_allocate ( tab, &gdkrec );
    /*gtk_widget_set_size_request ( tab, width, height );*/

    gtk_fixed_put ( GTK_FIXED(parent), tab, x, y );

    g_signal_connect ( G_OBJECT (tab), "realize", G_CALLBACK (Realize), led );
    g_signal_connect ( G_OBJECT (tab), "destroy", G_CALLBACK (Destroy), led );

    createLightGeneralPanel ( tab, led, EDITLIGHTGENERAL, 0, 0, width, height );
    createDiffuseColorPanel ( tab, led, EDITDIFFUSE     , 0, 0, width, height );
    createSpecularityPanel  ( tab, led, EDITSPECULAR    , 0, 0, width, height );
    createShadowsPanel      ( tab, led, EDITLIGHTSHADOWS, 0, 0, width, height );

    /*list_insert ( &gui->lligedit, tab );*/

    gtk_widget_show ( tab );


    return tab;
}
