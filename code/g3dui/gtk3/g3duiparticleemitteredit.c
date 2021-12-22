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

#define EDITPEMITTERGENERAL                "General"
#define EDITPEMITTERINITIAL                "Initial"
#define EDITPEMITTERFINAL                  "Final"
#define EDITPEMITTERVARIATION              "Variation"

#define EDITPEMITTERSTARTATFRAME           "Start at frame"
#define EDITPEMITTERENDATFRAME             "End at frame"
#define EDITPEMITTERRADIUS                 "Radius"
#define EDITPEMITTERSHAPE                  "Shape"
#define EDITPEMITTERPARTICLELIFETIME       "Lifetime"
#define EDITPEMITTERPARTICLELIFETIMEVAR    "Lifetime variation"
#define EDITPEMITTERPARTICLESPERFRAME      "Particles per frame"
#define EDITPEMITTERMAXPREVIEWS            "Max particles preview"
#define EDITPEMITTERDISPLAYPARTICULES      "Display particles"

#define EDITPEMITTERINITIALACCEL           "Initial acceleration"
#define EDITPEMITTERINITIALSPEED           "Initial speed"
#define EDITPEMITTERINITIALSCALING         "Initial scaling"
#define EDITPEMITTERINITIALROTATION        "Initial rotation"
#define EDITPEMITTERINITIALTRANSPARENCY    "Initial transparency"

#define EDITPEMITTERINITIALANGLEVAR        "Initial angle variation"

#define EDITPEMITTERINITIALACCELVAR        "Initial acceleration variation"
#define EDITPEMITTERINITIALSPEEDVAR        "Initial speed variation"
#define EDITPEMITTERINITIALSCALINGVAR      "Initial scaling variation"
#define EDITPEMITTERINITIALROTATIONVAR     "Initial rotation variation"
#define EDITPEMITTERINITIALTRANSPARENCYVAR "Initial transparency variation"

#define EDITPEMITTERFINALACCEL             "Final acceleration"
#define EDITPEMITTERFINALSPEED             "Final speed"
#define EDITPEMITTERFINALSCALING           "Final scaling"
#define EDITPEMITTERFINALROTATION          "Final roation"
#define EDITPEMITTERFINALTRANSPARENCY      "Final transparency"

/******************************************************************************/
typedef struct _G3DUIPARTICLEEMITTEREDIT {
    G3DUIWIDGETGROUP    grp;

    GtkWidget          *main;

    GtkWidget          *startAtFrameEntry;
    GtkWidget          *endAtFrameEntry;
    GtkWidget          *radiusEntry;
    GtkWidget          *shapeCombo;
    GtkWidget          *lifetimeEntry;
    GtkWidget          *ppfEntry;
    GtkWidget          *maxPreviewsEntry;
    GtkWidget          *displayPartToggle;

    GtkWidget          *initialAccelXEntry;
    GtkWidget          *initialAccelYEntry;
    GtkWidget          *initialAccelZEntry;
    GtkWidget          *initialSpeedXEntry;
    GtkWidget          *initialSpeedYEntry;
    GtkWidget          *initialSpeedZEntry;
    GtkWidget          *initialScaXEntry;
    GtkWidget          *initialScaYEntry;
    GtkWidget          *initialScaZEntry;
    GtkWidget          *initialRotXEntry;
    GtkWidget          *initialRotYEntry;
    GtkWidget          *initialRotZEntry;
    GtkWidget          *initialTranspEntry;

    GtkWidget          *initialAngleVarEntry;
    GtkWidget          *initialLifetimeVarEntry;

    GtkWidget          *initialAccelVarXEntry;
    GtkWidget          *initialAccelVarYEntry;
    GtkWidget          *initialAccelVarZEntry;
    GtkWidget          *initialSpeedVarXEntry;
    GtkWidget          *initialSpeedVarYEntry;
    GtkWidget          *initialSpeedVarZEntry;
    GtkWidget          *initialScaVarXEntry;
    GtkWidget          *initialScaVarYEntry;
    GtkWidget          *initialScaVarZEntry;
    GtkWidget          *initialRotVarXEntry;
    GtkWidget          *initialRotVarYEntry;
    GtkWidget          *initialRotVarZEntry;
    GtkWidget          *initialTranspVarEntry;

    GtkWidget          *finalAccelXEntry;
    GtkWidget          *finalAccelYEntry;
    GtkWidget          *finalAccelZEntry;
    GtkWidget          *finalSpeedXEntry;
    GtkWidget          *finalSpeedYEntry;
    GtkWidget          *finalSpeedZEntry;
    GtkWidget          *finalScalXEntry;
    GtkWidget          *finalScalYEntry;
    GtkWidget          *finalScalZEntry;
    GtkWidget          *finalRotXEntry;
    GtkWidget          *finalRotYEntry;
    GtkWidget          *finalRotZEntry;
    GtkWidget          *finalTranspEntry;

    G3DPARTICLEEMITTER *editedPEmitter;
} G3DUIPARTICLEEMITTEREDIT;

/******************************************************************************/
static G3DUIPARTICLEEMITTEREDIT *g3duiparticleemitteredit_new ( G3DUI *gui ) {
    G3DUIPARTICLEEMITTEREDIT *ped = calloc ( 0x01, sizeof ( G3DUIPARTICLEEMITTEREDIT ) );

    if ( ped == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );
    }

    ped->grp.gui = gui;


    return ped; 
}

#ifdef unused

/******************************************************************************/
static void updateShadowsPanel ( G3DUIPARTICLEEMITTEREDIT *led ) {
    led->grp.gui->lock = 0x01;

    if ( led->editedLight ) {
        if ( ((G3DOBJECT*)led->editedLight)->flags & PARTICLEEMITTERCASTSHADOWS ) {
            gtk_widget_set_sensitive ( led->softShadowsToggle, TRUE );

            if ( ((G3DOBJECT*)led->editedLight)->flags & PARTICLEEMITTERSOFTSHADOWS ) {
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
    G3DUIPARTICLEEMITTEREDIT *led = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    char *wname = gtk_widget_get_name ( widget );

    if ( led->editedLight ) {
        common_g3duilightedit_setSoftShadowsCbk ( led->grp.gui,
                                                  led->editedLight );
    }

    updateShadowsPanel ( led );
}

/******************************************************************************/
static void shadowRadiusCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIPARTICLEEMITTEREDIT *led = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float shadowRadius = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( led->editedLight ) {
        common_g3duilightedit_shadowRadiusCbk ( led->grp.gui,
                                                led->editedLight,
                                                shadowRadius );
    }

    g3dui_redrawGLViews ( led->grp.gui );
}

/******************************************************************************/
static void shadowSampleCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIPARTICLEEMITTEREDIT *led = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    uint32_t shadowSample = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( led->editedLight ) {
        common_g3duilightedit_shadowSampleCbk ( led->grp.gui,
                                                led->editedLight,
                                                shadowSample );
    }

    g3dui_redrawGLViews ( led->grp.gui );
}

/******************************************************************************/
static GtkWidget *createShadowsPanel ( GtkWidget      *parent, 
                                       G3DUIPARTICLEEMITTEREDIT *led,
                                       char           *name,
                                       gint            x,
                                       gint            y,
                                       gint            width,
                                       gint            height ) {
    GtkWidget *pan = createPanel ( parent, led, name, x, y, width, height );

    led->softShadowsToggle = createToggleLabel ( pan,
                                                 led,
                                                 EDITPEMITTERSOFTSHADOWS,
                                                 0, 0, 20, 20,
                                                 setShadowTypeCbk );

    led->shadowRadiusEntry = createFloatText ( pan,
                                               led,
                                               EDITPEMITTERSOFTSHADOWSRADIUS,
                                               0.0f, FLT_MAX,
                                               0, 24, 96, 96,
                                               shadowRadiusCbk );

    led->shadowSampleEntry = createIntegerText ( pan,
                                                led,
                                                EDITPEMITTERSOFTSHADOWSSAMPLE,
                                                1.0f, 32.0f,
                                                0, 48, 96, 96,
                                                shadowSampleCbk );

    return pan;
}

/******************************************************************************/
static void specularityChangeCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIPARTICLEEMITTEREDIT *led = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = ( G3DUI * ) led->grp.gui;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER(widget);
    GdkRGBA color;

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
static void updateSpecularityPanel ( G3DUIPARTICLEEMITTEREDIT *led ) {
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
                                           G3DUIPARTICLEEMITTEREDIT *led,
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
    G3DUIPARTICLEEMITTEREDIT *led = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = ( G3DUI * ) led->grp.gui;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER(widget);
    GdkRGBA color;

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
    G3DUIPARTICLEEMITTEREDIT *led = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    G3DUI *gui = ( G3DUI * ) led->grp.gui;
    float intensity = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevent useless primitive building when XmTextSetString is called ***/
    if ( gui->lock ) return;

    if ( led->editedLight ) {
        led->editedLight->intensity = intensity;
    }

    g3dui_redrawGLViews ( led->grp.gui );
}

/******************************************************************************/
static void updateLightGeneralPanel ( G3DUIPARTICLEEMITTEREDIT *led ) {
    led->grp.gui->lock = 0x01;

    if ( led->editedLight ) {
        gtk_widget_set_sensitive ( led->spotLengthEntry   , TRUE );
        gtk_widget_set_sensitive ( led->spotAngleEntry    , TRUE );
        gtk_widget_set_sensitive ( led->spotFadeAngleEntry, TRUE );
        gtk_widget_set_sensitive ( led->spotToggle        , TRUE );
        gtk_widget_set_sensitive ( led->castShadowsToggle , TRUE );

        if ( ((G3DOBJECT*)led->editedLight)->flags & PARTICLEEMITTERCASTSHADOWS ) {
            gtk_toggle_button_set_active ( led->castShadowsToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( led->castShadowsToggle, FALSE );
        }

        if ( ((G3DOBJECT*)led->editedLight)->flags & SPOTPARTICLEEMITTER ) {
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
static void updateDiffuseColorPanel ( G3DUIPARTICLEEMITTEREDIT *led ) {
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
                                            G3DUIPARTICLEEMITTEREDIT *led,
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
                                            EDITPEMITTERINTENSITY,
                                            0.0f, FLT_MAX,
                                            0, 48, 96, 96,
                                            diffuseIntensityCbk );

    return pan;
}

#endif

/******************************************************************************/
static void updateGeneralPanel ( G3DUIPARTICLEEMITTEREDIT *ped ) {
    ped->grp.gui->lock = 0x01;

    if ( ped->editedPEmitter ) {
        gtk_widget_set_sensitive ( ped->startAtFrameEntry, TRUE );
        gtk_widget_set_sensitive ( ped->endAtFrameEntry  , TRUE );
        /*gtk_widget_set_sensitive ( ped->shapeCombo       , TRUE );*/
        gtk_widget_set_sensitive ( ped->radiusEntry      , TRUE );
        gtk_widget_set_sensitive ( ped->lifetimeEntry    , TRUE );
        gtk_widget_set_sensitive ( ped->ppfEntry         , TRUE );
        gtk_widget_set_sensitive ( ped->maxPreviewsEntry , TRUE );
        gtk_widget_set_sensitive ( ped->displayPartToggle, TRUE );

        gtk_spin_button_set_value  ( ped->startAtFrameEntry, 
                                     ped->editedPEmitter->startAtFrame );

        gtk_spin_button_set_value  ( ped->endAtFrameEntry, 
                                     ped->editedPEmitter->endAtFrame );

        gtk_spin_button_set_value  ( ped->radiusEntry, 
                                     ped->editedPEmitter->radius );

        gtk_spin_button_set_value  ( ped->lifetimeEntry, 
                                     ped->editedPEmitter->particleLifetime );

        gtk_spin_button_set_value  ( ped->ppfEntry, 
                                     ped->editedPEmitter->particlesPerFrame );

        gtk_spin_button_set_value  ( ped->maxPreviewsEntry, 
                                     ped->editedPEmitter->maxPreviewsPerFrame );

        if ( ((G3DOBJECT*)ped->editedPEmitter)->flags & DISPLAYPARTICLES ) {
            gtk_toggle_button_set_active ( ped->displayPartToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( ped->displayPartToggle, FALSE );
        }
    } else {
        gtk_widget_set_sensitive ( ped->startAtFrameEntry, FALSE );
        gtk_widget_set_sensitive ( ped->endAtFrameEntry  , FALSE );
        /*gtk_widget_set_sensitive ( ped->shapeCombo       , FALSE );*/
        gtk_widget_set_sensitive ( ped->radiusEntry      , FALSE );
        gtk_widget_set_sensitive ( ped->lifetimeEntry    , FALSE );
        gtk_widget_set_sensitive ( ped->ppfEntry         , FALSE );
        gtk_widget_set_sensitive ( ped->maxPreviewsEntry , FALSE );
        gtk_widget_set_sensitive ( ped->displayPartToggle, FALSE );
    }

    ped->grp.gui->lock = 0x00;
}

/******************************************************************************/
static void startAtFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float startAtFrame = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_startAtFrameCbk ( ped->grp.gui,
                                                          ped->editedPEmitter,
                                                          startAtFrame );
    }

    g3dui_redrawGLViews ( ped->grp.gui );
}

/******************************************************************************/
static void endAtFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float endAtFrame = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_endAtFrameCbk ( ped->grp.gui,
                                                        ped->editedPEmitter,
                                                        endAtFrame );
    }

    g3dui_redrawGLViews ( ped->grp.gui );
}

/******************************************************************************/
static void radiusCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float radius = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_radiusCbk ( ped->grp.gui,
                                                    ped->editedPEmitter,
                                                    radius );
    }

    g3dui_redrawGLViews ( ped->grp.gui );
}

/******************************************************************************/
static void lifetimeCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float lifetime = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_lifetimeCbk ( ped->grp.gui,
                                                      ped->editedPEmitter,
                                                      lifetime );
    }

    g3dui_redrawGLViews ( ped->grp.gui );
}

/******************************************************************************/
static void ppfCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float ppf = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_ppfCbk ( ped->grp.gui,
                                                 ped->editedPEmitter,
                                                 ppf );
    }

    g3dui_redrawGLViews ( ped->grp.gui );
}

/******************************************************************************/
static void maxPreviewsCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;
    float maxPreviews = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_maxPreviewsCbk ( ped->grp.gui,
                                                         ped->editedPEmitter,
                                                         maxPreviews );
    }

    g3dui_redrawGLViews ( ped->grp.gui );
}

/******************************************************************************/
static void displayPartCbk  ( GtkWidget *widget, 
                              gpointer   user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;

    if ( ped->editedPEmitter ) {
        common_g3duiparticleemitteredit_displayPartCbk ( ped->grp.gui, 
                                                         ped->editedPEmitter );
    }

    g3dui_redrawGLViews ( ped->grp.gui );
}

/******************************************************************************/
static GtkWidget *createGeneralPanel ( GtkWidget                *parent, 
                                       G3DUIPARTICLEEMITTEREDIT *ped,
                                       char                     *name,
                                       gint                      x,
                                       gint                      y,
                                       gint                      width,
                                       gint                      height ) {
    GtkWidget *pan = createPanel ( parent, ped, name, x, y, width, height );

    ped->startAtFrameEntry = createIntegerText ( pan,
                                                 ped,
                                                 EDITPEMITTERSTARTATFRAME,
                                                 INT_MIN, INT_MAX,
                                                 0,   0, 120, 24,
                                                 startAtFrameCbk );

    ped->endAtFrameEntry   = createIntegerText ( pan,
                                                 ped,
                                                 EDITPEMITTERENDATFRAME,
                                                 INT_MIN, INT_MAX,
                                                 0,  24, 120, 24,
                                                 endAtFrameCbk );

    ped->radiusEntry       = createFloatText   ( pan,
                                                 ped,
                                                 EDITPEMITTERRADIUS,
                                                 0.0f, FLT_MAX,
                                                 0, 48, 120, 96,
                                                 radiusCbk );

    /*ped->shapeCombo        = createFloatText   ( pan,
                                                 ped,
                                                 EDITPEMITTERSHAPE,
                                                 0.0f, FLT_MAX,
                                                 0, 72, 96, 96,
                                                 spotAngleCbk );*/

    ped->lifetimeEntry     = createIntegerText ( pan,
                                                 ped,
                                                 EDITPEMITTERPARTICLELIFETIME,
                                                 0x00, INT_MAX,
                                                 0, 96, 120, 96,
                                                 lifetimeCbk );

    ped->ppfEntry          = createFloatText   ( pan,
                                                 ped,
                                                 EDITPEMITTERPARTICLESPERFRAME,
                                                 0.0f, FLT_MAX,
                                                 0, 120, 120, 96,
                                                 ppfCbk );

    ped->maxPreviewsEntry  = createIntegerText ( pan,
                                                 ped,
                                                 EDITPEMITTERMAXPREVIEWS,
                                                 0x00, INT_MAX,
                                                 0, 144, 120, 96,
                                                 maxPreviewsCbk );

    ped->displayPartToggle = createToggleLabel ( pan,
                                                 ped,
                                                 EDITPEMITTERDISPLAYPARTICULES,
                                                 0, 170, 192, 18,
                                                 displayPartCbk );

    return pan;
}

/******************************************************************************/
void updateParticleEmitterEdit ( GtkWidget           *w, 
                                 G3DPARTICLEEMITTER  *pem ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) g_object_get_data ( G_OBJECT(w),
                                                                   "private" );

    ped->editedPEmitter = pem;

    updateGeneralPanel  ( ped );
    /*updateDiffuseColorPanel ( led );
    updateLightGeneralPanel ( led );
    updateShadowsPanel      ( led );*/
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;

    free ( ped );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUIPARTICLEEMITTEREDIT *ped = ( G3DUIPARTICLEEMITTEREDIT * ) user_data;

    updateParticleEmitterEdit ( widget, NULL );
}

/******************************************************************************/
GtkWidget *createParticleEmitterEdit ( GtkWidget *parent,
                                       G3DUI     *gui,
                                       char      *name,
                                       gint       x,
                                       gint       y,
                                       gint       width,
                                       gint       height ) {
    GdkRectangle gdkrec = { 0x00, 0x00, width, height };
    GtkWidget *tab = gtk_notebook_new ( );
    G3DUIPARTICLEEMITTEREDIT *ped = g3duiparticleemitteredit_new ( gui );

    g_object_set_data ( G_OBJECT(tab), "private", (gpointer) ped );

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(tab), TRUE );

    gtk_widget_set_name ( tab, name );

    gtk_widget_size_allocate ( tab, &gdkrec );
    /*gtk_widget_set_size_request ( tab, width, height );*/

    gtk_fixed_put ( GTK_FIXED(parent), tab, x, y );

    g_signal_connect ( G_OBJECT (tab), "realize", G_CALLBACK (Realize), ped );
    g_signal_connect ( G_OBJECT (tab), "destroy", G_CALLBACK (Destroy), ped );

    createGeneralPanel   ( tab, ped, EDITPEMITTERGENERAL, 0, 0, width, height );
/*
    createInitialPanel   ( tab, led, EDITDIFFUSE     , 0, 0, width, height );
    createFinalPanel     ( tab, led, EDITSPECULAR    , 0, 0, width, height );
    createVariationPanel ( tab, led, EDITPEMITTERSHADOWS, 0, 0, width, height );
*/
    /*list_insert ( &gui->lligedit, tab );*/

    gtk_widget_show ( tab );


    return tab;
}
