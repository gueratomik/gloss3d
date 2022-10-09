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
#define EDITMATERIALNAME                           "Material Name"

#define EDITMATERIALDIFFUSE                        "Diffuse"
#define EDITDIFFUSECHANNELPROCEDURALSETTINGS       "Diffuse procedural settings"

#define EDITMATERIALSPECULAR                       "Specularity"
#define EDITMATERIALSPECULARSHININESS              "Shininess"
#define EDITMATERIALSPECULARLEVEL                  "Level"

#define EDITMATERIALDISPLACEMENT                   "Displacement"
#define EDITMATERIALDISPLACEMENTCHANNEL            "Displacement Channel"
#define EDITMATERIALDISPLACEMENTSTRENGTH           "Strength"

#define EDITMATERIALBUMP                           "Bump"
#define EDITMATERIALBUMPCHANNEL                    "Bump Channel"
#define EDITMATERIALBUMPSTRENGTH                   "Strength"

#define EDITMATERIALALPHA                          "Alpha"
#define EDITMATERIALALPHACHANNEL                   "Alpha Channel"
#define EDITMATERIALALPHASTRENGTH                  "Opacity"

#define EDITMATERIALREFLECTION                     "Reflection"
#define EDITMATERIALREFLECTIONSTRENGTH             "Strength"

#define EDITMATERIALREFRACTION                     "Refraction"
#define EDITMATERIALREFRACTIONTRANSPARENCY         "Transparency"
#define EDITMATERIALREFRACTIONSTRENGTH             "Strength"

/******************************************************************************/
static GTK3G3DUIMATERIALEDIT *gtk3_g3duimaterialedit_new ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUIMATERIALEDIT *gtk3med = calloc ( 0x01, sizeof ( GTK3G3DUIMATERIALEDIT ) );

    if ( gtk3med == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3med->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3med; 
}

/******************************************************************************/
uint32_t getChannelID ( GtkFixed *fixed ) {
    char *wname = gtk_widget_get_name ( fixed );
    uint32_t channelID = 0x00;

    if ( strcmp ( wname, EDITMATERIALDIFFUSE      ) == 0x00 ) {
        channelID = DIFFUSECHANNELID;
    }

    if ( strcmp ( wname, EDITMATERIALSPECULAR     ) == 0x00 ) {
        channelID = SPECULARCHANNELID;
    }

    if ( strcmp ( wname, EDITMATERIALDISPLACEMENT ) == 0x00 ) {
        channelID = DISPLACEMENTCHANNELID;
    }

    if ( strcmp ( wname, EDITMATERIALBUMP         ) == 0x00 ) {
        channelID = BUMPCHANNELID;
    }

    if ( strcmp ( wname, EDITMATERIALALPHA        ) == 0x00 ) {
        channelID = ALPHACHANNELID;
    }

    if ( strcmp ( wname, EDITMATERIALREFLECTION   ) == 0x00 ) {
        channelID = REFLECTIONCHANNELID;
    }

    if ( strcmp ( wname, EDITMATERIALREFRACTION   ) == 0x00 ) {
        channelID = REFRACTIONCHANNELID;
    }

    return channelID;
}

/******************************************************************************/
static void channelProceduralSettingsCbk ( GtkWidget *widget, 
                                           gpointer   user_data ) {
    GTK3G3DUIMATERIALEDIT *gtk3med = ( GTK3G3DUIMATERIALEDIT * ) user_data;
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    char *wname = gtk_widget_get_name ( parent );
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3med->core.gui;
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    uint32_t channelID = getChannelID ( gtk_widget_get_parent ( widget ) );

    if ( gtk3med->core.gui->lock ) return;

    if ( gui->lselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) gui->lselmat->data;
        G3DCHANNEL *cha = g3dmaterial_getChannelByID ( mat, channelID );

        if ( cha ) {
            if (cha->proc ) {
                GtkWidget *dial = gtk_dialog_new ( );
                GtkWidget *box = gtk_dialog_get_content_area ( dial );
                G3DPROCEDURAL *proc = cha->proc;

                switch ( proc->type ) {
                    case PROCEDURALBRICK :
#ifdef TODO
                        createProceduralBrickEdit ( box,
                                                    gui,
                                                    cha->proc, 
                                                    "Procedural Brick",
                                                    0, 0, 350, 35, 0x01 );
#endif
                    break;

                    case PROCEDURALCHESS :
#ifdef TODO
                        createProceduralChessEdit ( box,
                                                    gui,
                                                    cha->proc, 
                                                    "Procedural Chess",
                                                    0, 0, 350, 35, 0x01 );
#endif
                    break;

                    case PROCEDURALNOISE :
#ifdef TODO
                        createProceduralNoiseEdit ( box,
                                                    gui,
                                                    cha->proc, 
                                                    "Procedural Noise",
                                                    0, 0, 350, 35, 0x01 );
#endif
                    break;

                    case PROCEDURALGRADIENT :
#ifdef TODO
                        createProceduralGradientEdit ( box,
                                                       gui,
                                                       cha->proc, 
                                                       "Procedural Gradient",
                                                       0, 0, 350, 35, 0x01 );
#endif
                    break;

                    default :
                    break;
                }


                g_signal_connect_swapped ( dial,
                                           "response",
                                           G_CALLBACK (gtk_widget_destroy),
                                           dial);

                gtk_dialog_run ( dial );

                if ( strcmp ( wname, EDITMATERIALDISPLACEMENT ) == 0x00 ) {
                    g3dmaterial_updateMeshes ( mat, 
                                               gui->sce, 
                                               gui->engine_flags );
                }
            }
        }
    }
}

/******************************************************************************/
static void channelImageChooseCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMATERIALEDIT *gtk3med = ( GTK3G3DUIMATERIALEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3med->core.gui;
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    uint32_t channelID = getChannelID ( gtk_widget_get_parent ( widget ) );
    uint64_t ret = 0x00;

    if ( gtk3med->core.gui->lock ) return;

    ret = gtk3_loadImageForChannel ( gtk3gui, channelID );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret ); 
}

/******************************************************************************/
static void channelProceduralCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMATERIALEDIT *gtk3med = ( GTK3G3DUIMATERIALEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3med->core.gui;
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    gchar *procType = gtk_combo_box_text_get_active_text ( gtk3med->diffuseProcTypeCombo ),
          *procRes  = PROCRES512/*gtk_combo_box_text_get_active_text ( gtk3med->diffuseProcResCombo  )*/;
    uint32_t channelID = getChannelID ( gtk_widget_get_parent ( widget ) );
    uint64_t ret = 0x00;

    if ( gtk3med->core.gui->lock ) return;

     if ( procType ) {
        ret = g3duimaterialedit_enableProcedural ( &gtk3med->core,
                                                    channelID );

	    g3duimaterialedit_chooseProcedural ( &gtk3med->core,
                                              channelID,
                                              procType, 
                                              procRes,
                                              0x01 );

        g_free ( procType );
        /*g_free ( procRes  );*/
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, ret | UPDATECURRENTMATERIAL );
}

/******************************************************************************/
static void channelColorCbk ( GtkWidget *widget,  gpointer user_data ) {
    GTK3G3DUIMATERIALEDIT *gtk3med = ( GTK3G3DUIMATERIALEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3med->core.gui;
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER(widget);
    GdkRGBA color;
    uint32_t channelID = getChannelID ( gtk_widget_get_parent ( widget ) );
    uint64_t ret;

    /*** prevents a loop ***/
    if ( gtk3med->core.gui->lock ) return;

    gtk_color_chooser_get_rgba ( ccr, &color );


    ret = g3duimaterialedit_setChannelColor ( &gtk3med->core,
                                               channelID,
                                               color.red,
                                               color.green,
                                               color.blue,
                                               color.alpha );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );
}

/******************************************************************************/
static void channelEnabledCbk ( GtkWidget *widget, 
                                gpointer   user_data ) {
    GTK3G3DUIMATERIALEDIT *gtk3med = ( GTK3G3DUIMATERIALEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3med->core.gui;
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    uint32_t channelID = getChannelID ( gtk_widget_get_parent ( widget ) );
    uint64_t ret;

    /*** prevents a loop ***/
    if ( gtk3med->core.gui->lock ) return;

    ret = g3duimaterialedit_toggleChannel ( gtk3med, channelID );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret | UPDATECURRENTMATERIAL );
}

/******************************************************************************/
static void channelImageCbk ( GtkWidget *widget, 
                              gpointer   user_data ) {
    GTK3G3DUIMATERIALEDIT *gtk3med = ( GTK3G3DUIMATERIALEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3med->core.gui;
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    uint32_t channelID = getChannelID ( gtk_widget_get_parent ( widget ) );
    uint64_t ret;

    /*** prevents a loop ***/
    if ( gtk3med->core.gui->lock ) return;

    ret = g3duimaterialedit_enableImage ( gtk3med, channelID );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret | UPDATECURRENTMATERIAL );
}

/******************************************************************************/
static void channelSolidCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMATERIALEDIT *gtk3med = ( GTK3G3DUIMATERIALEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3med->core.gui;
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    uint32_t channelID = getChannelID ( gtk_widget_get_parent ( widget ) );
    uint64_t ret;

    /*** prevents a loop ***/
    if ( gtk3med->core.gui->lock ) return;

    ret = g3duimaterialedit_enableSolidColor ( gtk3med, channelID );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret | UPDATECURRENTMATERIAL );
}

/******************************************************************************/
static void channelProceduralTypeCbk ( GtkWidget *widget, gpointer user_data ) {
    channelProceduralCbk ( widget, user_data );
}

/******************************************************************************/
static void channelProceduralResCbk  ( GtkWidget *widget, gpointer user_data ) {
    channelProceduralCbk ( widget, user_data );
}

/******************************************************************************/
static void updateDisplacementPanel ( GTK3G3DUIMATERIALEDIT *gtk3med ) {
    gtk3med->core.gui->lock = 0x01;

    gtk_widget_set_sensitive ( gtk3med->displacementImageButton       , FALSE );
    gtk_widget_set_sensitive ( gtk3med->displacementProcTypeCombo     , FALSE );
    gtk_widget_set_sensitive ( gtk3med->displacementProcSettingsButton, FALSE );
    gtk_combo_box_set_active ( gtk3med->displacementProcTypeCombo     , 0x00 );

    if ( gtk3med->core.editedMaterial ) {
        G3DMATERIAL *mat = gtk3med->core.editedMaterial;
        G3DPROCEDURAL *proc = mat->displacement.proc;
        GdkRGBA rgba = { .red   = mat->displacement.solid.r,
                         .green = mat->displacement.solid.g,
                         .blue  = mat->displacement.solid.b,
                         .alpha = mat->displacement.solid.a };

        if ( mat->displacement.flags & USECHANNEL ) {
            gtk_toggle_button_set_active ( gtk3med->displacementEnabledToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( gtk3med->displacementEnabledToggle, FALSE );
        }

        if ( mat->displacement.flags & USEIMAGECOLOR ) {
            gtk_toggle_button_set_active ( gtk3med->displacementImageToggle, TRUE  );

            gtk_widget_set_sensitive ( gtk3med->displacementImageButton, TRUE );
        } else {
            gtk_toggle_button_set_active ( gtk3med->displacementImageToggle, FALSE );
        }

        if ( mat->displacement.flags & USEPROCEDURAL ) {
            gtk_toggle_button_set_active ( gtk3med->displacementProcToggle, TRUE  );

            gtk_widget_set_sensitive ( gtk3med->displacementProcTypeCombo     , TRUE );
            gtk_widget_set_sensitive ( gtk3med->displacementProcSettingsButton, TRUE );
        } else {
            gtk_toggle_button_set_active ( gtk3med->displacementProcToggle, FALSE );
        }

        if ( proc ) {
            gtk_combo_box_set_active ( gtk3med->displacementProcTypeCombo, proc->type );
        }

        gtk_spin_button_set_value ( gtk3med->displacementStrengthEntry, mat->displacement.solid.a );

        if ( mat->displacement.image ) {
            if ( mat->displacement.image->filename ) {
                char *imgpath, *imgname;

                imgpath = g3dcore_strclone ( mat->displacement.image->filename );

                /*** We just keep the image name, not the whole ***/
               /*** path and display it as the button label.   ***/
                imgname = basename ( imgpath );

                gtk_button_set_label ( gtk3med->displacementImageButton, imgname );

                free ( imgpath );
            }
        } else {
            gtk_button_set_label ( gtk3med->displacementImageButton, EDITCHANNELIMAGE );
        }
    }

    gtk3med->core.gui->lock = 0x00;
}

/******************************************************************************/
static void displacementStrengthCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMATERIALEDIT *gtk3med = ( GTK3G3DUIMATERIALEDIT * ) user_data;
    float val = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    g3duimaterialedit_setDisplacementStrength ( gtk3med, val );
}

/******************************************************************************/
static GtkWidget *createDisplacementPanel ( GTK3G3DUIMATERIALEDIT *gtk3med,
                                            uint32_t               x,
                                            uint32_t               y,
                                            uint32_t               width,
                                            uint32_t               height ) {
    GtkFixed *pan = ui_createTab ( gtk3med->notebook,
                                   gtk3med,
                                   EDITMATERIALDISPLACEMENT,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3med->displacementEnabledToggle  = ui_createToggleLabel ( pan, 
                                                         gtk3med,
                                                         EDITCHANNELENABLED,
                                                         CLASS_MAIN,
                                                         0,  0, 96, 20, 20,
                                                         channelEnabledCbk );

    gtk3med->displacementStrengthEntry  = ui_createFloatText   ( pan, 
                                                         gtk3med,
                                                         EDITMATERIALBUMPSTRENGTH,
                                                         CLASS_MAIN,
                                                         0.0f, FLT_MAX,
                                                         0, 24, 96, 96, 20,
                                                         displacementStrengthCbk );

          /*** Use image as texture ***/
    gtk3med->displacementImageToggle    = ui_createToggleLabel ( pan,
                                                         gtk3med,
                                                         EDITCHANNELIMAGE,
                                                         CLASS_MAIN,
                                                         0, 48, 96, 20, 20,
                                                         channelImageCbk );

          /*** Image chooser button **/
    gtk3med->displacementImageButton    = ui_createPushButton  ( pan,
                                                         gtk3med, 
                                                         EDITCHANNELIMAGE,
                                                         CLASS_MAIN,
                                                         120, 48, 96, 20,
                                                         channelImageChooseCbk );

          /*** Use image as texture ***/
    gtk3med->displacementProcToggle     = ui_createToggleLabel ( pan,
                                                         gtk3med,
                                                         EDITCHANNELPROCEDURAL,
                                                         CLASS_MAIN,
                                                         0, 72, 96, 20, 20,
                                                         channelProceduralCbk );

    gtk3med->displacementProcTypeCombo 
                                = ui_createProceduralSelector
                                                       ( pan,
                                                         gtk3med, 
                                                         EDITCHANNELPROCEDURALTYPE,
                                                         CLASS_MAIN,
                                                         0, 96, 96, 64, 20,
                                                         channelProceduralTypeCbk );

    /*** Procedural settings **/
    gtk3med->displacementProcSettingsButton 
                                = ui_createPushButton  ( pan,
                                                         gtk3med, 
                                                         EDITCHANNELPROCEDURALSETTINGS,
                                                         CLASS_MAIN,
                                                         96, 120, 96, 18,
                                                         channelProceduralSettingsCbk );

    return pan;
}

/******************************************************************************/
static void updateBumpPanel ( GTK3G3DUIMATERIALEDIT *gtk3med ) {
    gtk3med->core.gui->lock = 0x01;

    gtk_widget_set_sensitive ( gtk3med->bumpImageButton       , FALSE );
    gtk_widget_set_sensitive ( gtk3med->bumpProcTypeCombo     , FALSE );
    gtk_widget_set_sensitive ( gtk3med->bumpProcSettingsButton, FALSE );
    gtk_combo_box_set_active ( gtk3med->bumpProcTypeCombo     , 0x00 );

    if ( gtk3med->core.editedMaterial ) {
        G3DMATERIAL *mat = gtk3med->core.editedMaterial;
        G3DPROCEDURAL *proc = mat->bump.proc;
        GdkRGBA rgba = { .red   = mat->bump.solid.r,
                         .green = mat->bump.solid.g,
                         .blue  = mat->bump.solid.b,
                         .alpha = mat->bump.solid.a };

        if ( mat->bump.flags & USECHANNEL ) {
            gtk_toggle_button_set_active ( gtk3med->bumpEnabledToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( gtk3med->bumpEnabledToggle, FALSE );
        }

        if ( mat->bump.flags & USEIMAGECOLOR ) {
            gtk_toggle_button_set_active ( gtk3med->bumpImageToggle, TRUE  );

            gtk_widget_set_sensitive ( gtk3med->bumpImageButton, TRUE );
        } else {
            gtk_toggle_button_set_active ( gtk3med->bumpImageToggle, FALSE );
        }

        if ( mat->bump.flags & USEPROCEDURAL ) {
            gtk_toggle_button_set_active ( gtk3med->bumpProcToggle, TRUE  );

            gtk_widget_set_sensitive ( gtk3med->bumpProcTypeCombo     , TRUE );
            gtk_widget_set_sensitive ( gtk3med->bumpProcSettingsButton, TRUE );
        } else {
            gtk_toggle_button_set_active ( gtk3med->bumpProcToggle, FALSE );
        }

        if ( proc ) {
            gtk_combo_box_set_active ( gtk3med->bumpProcTypeCombo, proc->type );
        }

        gtk_spin_button_set_value ( gtk3med->bumpStrengthEntry, mat->bump.solid.a );

        if ( mat->bump.image ) {
            if ( mat->bump.image->filename ) {
                char *imgpath, *imgname;

                imgpath = g3dcore_strclone ( mat->bump.image->filename );

                /*** We just keep the image name, not the whole ***/
               /*** path and display it as the button label.   ***/
                imgname = basename ( imgpath );

                gtk_button_set_label ( gtk3med->bumpImageButton, imgname );

                free ( imgpath );
            }
        } else {
            gtk_button_set_label ( gtk3med->bumpImageButton, EDITCHANNELIMAGE );
        }
    }

    gtk3med->core.gui->lock = 0x00;
}

/******************************************************************************/
static void bumpStrengthCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMATERIALEDIT *gtk3med = ( GTK3G3DUIMATERIALEDIT * ) user_data;
    float val = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    g3duimaterialedit_setBumpStrength ( gtk3med, val );
}

/******************************************************************************/
static GtkWidget *createBumpPanel ( GTK3G3DUIMATERIALEDIT *gtk3med,
                                    uint32_t               x,
                                    uint32_t               y,
                                    uint32_t               width,
                                    uint32_t               height ) {
    GtkFixed *pan = ui_createTab ( gtk3med->notebook,
                                   gtk3med,
                                   EDITMATERIALBUMP,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3med->bumpEnabledToggle  = ui_createToggleLabel ( pan, 
                                                         gtk3med,
                                                         EDITCHANNELENABLED,
                                                         CLASS_MAIN,
                                                         0,  0, 96, 20, 20,
                                                         channelEnabledCbk );

    gtk3med->bumpStrengthEntry  = ui_createFloatText   ( pan, 
                                                         gtk3med,
                                                         EDITMATERIALBUMPSTRENGTH,
                                                         CLASS_MAIN,
                                                         0.0f, FLT_MAX,
                                                         0, 24, 96, 96, 20,
                                                         bumpStrengthCbk );

          /*** Use image as texture ***/
    gtk3med->bumpImageToggle    = ui_createToggleLabel ( pan,
                                                         gtk3med,
                                                         EDITCHANNELIMAGE,
                                                         CLASS_MAIN,
                                                         0, 48, 96, 20, 20,
                                                         channelImageCbk );

          /*** Image chooser button **/
    gtk3med->bumpImageButton    = ui_createPushButton  ( pan,
                                                         gtk3med, 
                                                         EDITCHANNELIMAGE,
                                                         CLASS_MAIN,
                                                         120, 48, 96, 20,
                                                         channelImageChooseCbk );

          /*** Use image as texture ***/
    gtk3med->bumpProcToggle     = ui_createToggleLabel ( pan,
                                                         gtk3med,
                                                         EDITCHANNELPROCEDURAL,
                                                         CLASS_MAIN,
                                                         0, 72, 96, 20, 20,
                                                         channelProceduralCbk );

    gtk3med->bumpProcTypeCombo 
                                = ui_createProceduralSelector
                                                       ( pan,
                                                         gtk3med, 
                                                         EDITCHANNELPROCEDURALTYPE,
                                                         CLASS_MAIN,
                                                         0, 96, 96, 64, 20,
                                                         channelProceduralTypeCbk );

    /*** Procedural settings **/
    gtk3med->bumpProcSettingsButton 
                                = ui_createPushButton  ( pan,
                                                         gtk3med, 
                                                         EDITCHANNELPROCEDURALSETTINGS,
                                                         CLASS_MAIN,
                                                         96, 120, 96, 18,
                                                         channelProceduralSettingsCbk );

    return pan;
}

/******************************************************************************/
static void updateSpecularPanel ( GTK3G3DUIMATERIALEDIT *gtk3med ) {
    gtk3med->core.gui->lock = 0x01;

    gtk_widget_set_sensitive ( gtk3med->specularColorButton       , FALSE );
    gtk_widget_set_sensitive ( gtk3med->specularImageButton       , FALSE );
    gtk_widget_set_sensitive ( gtk3med->specularProcTypeCombo     , FALSE );
    gtk_widget_set_sensitive ( gtk3med->specularProcSettingsButton, FALSE );
    gtk_combo_box_set_active ( gtk3med->specularProcTypeCombo     , 0x00 );

    if ( gtk3med->core.editedMaterial ) {
        G3DMATERIAL *mat = gtk3med->core.editedMaterial;
        G3DPROCEDURAL *proc = mat->specular.proc;
        GdkRGBA rgba = { .red   = mat->specular.solid.r,
                         .green = mat->specular.solid.g,
                         .blue  = mat->specular.solid.b,
                         .alpha = mat->specular.solid.a };

        if ( mat->specular.flags & USECHANNEL ) {
            gtk_toggle_button_set_active ( gtk3med->specularEnabledToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( gtk3med->specularEnabledToggle, FALSE );
        }

        if ( mat->specular.flags & USESOLIDCOLOR ) {
            gtk_toggle_button_set_active ( gtk3med->specularSolidToggle, TRUE  );

            gtk_widget_set_sensitive ( gtk3med->specularColorButton, TRUE );
        } else {
            gtk_toggle_button_set_active ( gtk3med->specularSolidToggle, FALSE );
        }

        if ( mat->specular.flags & USEIMAGECOLOR ) {
            gtk_toggle_button_set_active ( gtk3med->specularImageToggle, TRUE  );

            gtk_widget_set_sensitive ( gtk3med->specularImageButton, TRUE );
        } else {
            gtk_toggle_button_set_active ( gtk3med->specularImageToggle, FALSE );
        }

        if ( mat->specular.flags & USEPROCEDURAL ) {
            gtk_toggle_button_set_active ( gtk3med->specularProcToggle, TRUE  );

            gtk_widget_set_sensitive ( gtk3med->specularProcTypeCombo     , TRUE );
            gtk_widget_set_sensitive ( gtk3med->specularProcSettingsButton, TRUE );
        } else {
            gtk_toggle_button_set_active ( gtk3med->specularProcToggle, FALSE );
        }

        if ( proc ) {
            gtk_combo_box_set_active ( gtk3med->specularProcTypeCombo, proc->type );
        }

        gtk_color_chooser_set_rgba ( gtk3med->specularColorButton, &rgba );

        gtk_range_set_value ( gtk3med->specularShininessScale, mat->shininess );  
        gtk_range_set_value ( gtk3med->specularLevelScale    , mat->specular_level * 255.0f );

        if ( mat->specular.image ) {
            if ( mat->specular.image->filename ) {
                char *imgpath, *imgname;

                imgpath = g3dcore_strclone ( mat->specular.image->filename );

                /*** We just keep the image name, not the whole ***/
               /*** path and display it as the button label.   ***/
                imgname = basename ( imgpath );

                gtk_button_set_label ( gtk3med->specularImageButton, imgname );

                free ( imgpath );
            }
        } else {
            gtk_button_set_label ( gtk3med->specularImageButton, EDITCHANNELIMAGE );
        }
    }

    gtk3med->core.gui->lock = 0x00;
}

/******************************************************************************/
static void specularLevelCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMATERIALEDIT *gtk3med = ( GTK3G3DUIMATERIALEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3med->core.gui;
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    uint64_t ret = 0x00;
    float val = ( float ) gtk_range_get_value ( GTK_RANGE(widget) );

    ret = g3duimaterialedit_setSpecularLevel ( gtk3med, val / 255.0f );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret ); 
}

/******************************************************************************/
static void specularShininessCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMATERIALEDIT *gtk3med = ( GTK3G3DUIMATERIALEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3med->core.gui;
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    uint64_t ret = 0x00;
    float val = ( float ) gtk_range_get_value ( GTK_RANGE(widget) );

    ret = g3duimaterialedit_setSpecularShininess ( gtk3med, val );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret ); 
}


/******************************************************************************/
static GtkWidget *createSpecularPanel ( GTK3G3DUIMATERIALEDIT *gtk3med,
                                        uint32_t               x,
                                        uint32_t               y,
                                        uint32_t               width,
                                        uint32_t               height ) {
    GtkFixed *pan = ui_createTab ( gtk3med->notebook,
                                   gtk3med,
                                   EDITMATERIALSPECULAR,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3med->specularEnabledToggle = ui_createToggleLabel ( pan, 
                                                           gtk3med,
                                                           EDITCHANNELENABLED,
                                                           CLASS_MAIN,
                                                           0,  0, 96, 20, 20,
                                                           channelEnabledCbk );

          /*** Use Solid color as texture ***/
    gtk3med->specularSolidToggle = ui_createToggleLabel ( pan, 
                                                         gtk3med,
                                                         EDITCHANNELSOLID,
                                                         CLASS_MAIN,
                                                         0,  24, 96, 20, 20,
                                                         channelSolidCbk );
          /*** Pick color button ***/
    gtk3med->specularColorButton = ui_createColorButton ( pan,
                                                         gtk3med,
                                                         EDITCHANNELSOLID,
                                                         CLASS_MAIN,
                                                         120,  24, 0, 32, 20,
                                                         channelColorCbk );

          /*** Use image as texture ***/
    gtk3med->specularImageToggle = ui_createToggleLabel ( pan,
                                                         gtk3med,
                                                         EDITCHANNELIMAGE,
                                                         CLASS_MAIN,
                                                         0, 48, 96, 20, 20,
                                                         channelImageCbk );

          /*** Image chooser button **/
    gtk3med->specularImageButton = ui_createPushButton  ( pan,
                                                         gtk3med, 
                                                         EDITCHANNELIMAGE,
                                                         CLASS_MAIN,
                                                         120, 48, 96, 20,
                                                         channelImageChooseCbk );

          /*** Use image as texture ***/
    gtk3med->specularProcToggle  = ui_createToggleLabel ( pan,
                                                         gtk3med,
                                                         EDITCHANNELPROCEDURAL,
                                                         CLASS_MAIN,
                                                         0, 72, 96, 20, 20,
                                                         channelProceduralCbk );

    gtk3med->specularProcTypeCombo 
                                = ui_createProceduralSelector
                                                       ( pan,
                                                         gtk3med, 
                                                         EDITCHANNELPROCEDURALTYPE,
                                                         CLASS_MAIN,
                                                         0, 96, 96, 64, 20,
                                                         channelProceduralTypeCbk );

    /*** Procedural settings **/
    gtk3med->specularProcSettingsButton 
                                = ui_createPushButton  ( pan,
                                                         gtk3med, 
                                                         EDITCHANNELPROCEDURALSETTINGS,
                                                         CLASS_MAIN,
                                                         96, 120, 96, 18,
                                                         channelProceduralSettingsCbk );

    gtk3med->specularShininessScale = ui_createHorizontalScale
                                                       ( pan, 
                                                         gtk3med,
                                                         EDITMATERIALSPECULARSHININESS,
                                                         CLASS_MAIN,
                                                         0, 144, 96, 208, 20,
                                                         0.0f, 128.0f, 1.0f,
                                                         specularShininessCbk );

    gtk3med->specularLevelScale     = ui_createHorizontalScale
                                                       ( pan,
                                                         gtk3med,
                                                         EDITMATERIALSPECULARLEVEL,
                                                         CLASS_MAIN,
                                                         0, 168, 96, 208, 20,
                                                         0.0f, 255.0f, 1.0f,
                                                         specularLevelCbk );

    return pan;
}

/******************************************************************************/
static void updateDiffusePanel ( GTK3G3DUIMATERIALEDIT *gtk3med ) {
    gtk3med->core.gui->lock = 0x01;

    gtk_widget_set_sensitive ( gtk3med->diffuseColorButton       , FALSE );
    gtk_widget_set_sensitive ( gtk3med->diffuseImageButton       , FALSE );
    gtk_widget_set_sensitive ( gtk3med->diffuseProcTypeCombo     , FALSE );
    gtk_widget_set_sensitive ( gtk3med->diffuseProcSettingsButton, FALSE );
    gtk_combo_box_set_active ( gtk3med->diffuseProcTypeCombo     , 0x00 );

    if ( gtk3med->core.editedMaterial ) {
        G3DMATERIAL *mat = gtk3med->core.editedMaterial;
        G3DPROCEDURAL *proc = mat->diffuse.proc;
        GdkRGBA rgba = { .red   = mat->diffuse.solid.r,
                         .green = mat->diffuse.solid.g,
                         .blue  = mat->diffuse.solid.b,
                         .alpha = mat->diffuse.solid.a };

        if ( mat->diffuse.flags & USECHANNEL ) {
            gtk_toggle_button_set_active ( gtk3med->diffuseEnabledToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( gtk3med->diffuseEnabledToggle, FALSE );
        }

        if ( mat->diffuse.flags & USESOLIDCOLOR ) {
            gtk_toggle_button_set_active ( gtk3med->diffuseSolidToggle, TRUE  );

            gtk_widget_set_sensitive ( gtk3med->diffuseColorButton, TRUE );
        } else {
            gtk_toggle_button_set_active ( gtk3med->diffuseSolidToggle, FALSE );
        }

        if ( mat->diffuse.flags & USEIMAGECOLOR ) {
            gtk_toggle_button_set_active ( gtk3med->diffuseImageToggle, TRUE  );

            gtk_widget_set_sensitive ( gtk3med->diffuseImageButton, TRUE );
        } else {
            gtk_toggle_button_set_active ( gtk3med->diffuseImageToggle, FALSE );
        }

        if ( mat->diffuse.flags & USEPROCEDURAL ) {
            gtk_toggle_button_set_active ( gtk3med->diffuseProcToggle, TRUE  );

            gtk_widget_set_sensitive ( gtk3med->diffuseProcTypeCombo     , TRUE );
            gtk_widget_set_sensitive ( gtk3med->diffuseProcSettingsButton, TRUE );
        } else {
            gtk_toggle_button_set_active ( gtk3med->diffuseProcToggle, FALSE );
        }

        if ( proc ) {
            gtk_combo_box_set_active ( gtk3med->diffuseProcTypeCombo, proc->type );
        }

        gtk_color_chooser_set_rgba ( gtk3med->diffuseColorButton, &rgba );

        if ( mat->diffuse.image ) {
            if ( mat->diffuse.image->filename ) {
                char *imgpath, *imgname;

                imgpath = g3dcore_strclone ( mat->diffuse.image->filename );

                /*** We just keep the image name, not the whole ***/
               /*** path and display it as the button label.   ***/
                imgname = basename ( imgpath );

                gtk_button_set_label ( gtk3med->diffuseImageButton, imgname );

                free ( imgpath );
            }
        } else {
            gtk_button_set_label ( gtk3med->diffuseImageButton, EDITCHANNELIMAGE );
        }
    }

    gtk3med->core.gui->lock = 0x00;
}

/******************************************************************************/
static GtkWidget *createDiffusePanel ( GTK3G3DUIMATERIALEDIT *gtk3med,
                                       uint32_t               x,
                                       uint32_t               y,
                                       uint32_t               width,
                                       uint32_t               height ) {
    GtkFixed *pan = ui_createTab ( gtk3med->notebook,
                                   gtk3med,
                                   EDITMATERIALDIFFUSE,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3med->diffuseEnabledToggle = ui_createToggleLabel ( pan, 
                                                           gtk3med,
                                                           EDITCHANNELENABLED,
                                                           CLASS_MAIN,
                                                           0,  0, 96, 20, 20,
                                                           channelEnabledCbk );

          /*** Use Solid color as texture ***/
    gtk3med->diffuseSolidToggle = ui_createToggleLabel ( pan, 
                                                         gtk3med,
                                                         EDITCHANNELSOLID,
                                                         CLASS_MAIN,
                                                         0,  24, 96, 20, 20,
                                                         channelSolidCbk );
          /*** Pick color button ***/
    gtk3med->diffuseColorButton = ui_createColorButton ( pan,
                                                         gtk3med,
                                                         EDITCHANNELSOLID,
                                                         CLASS_MAIN,
                                                         120,  24, 0, 32, 20,
                                                         channelColorCbk );

          /*** Use image as texture ***/
    gtk3med->diffuseImageToggle = ui_createToggleLabel ( pan,
                                                         gtk3med,
                                                         EDITCHANNELIMAGE,
                                                         CLASS_MAIN,
                                                         0, 48, 96, 20, 20,
                                                         channelImageCbk );

          /*** Image chooser button **/
    gtk3med->diffuseImageButton = ui_createPushButton  ( pan,
                                                         gtk3med, 
                                                         EDITCHANNELIMAGE,
                                                         CLASS_MAIN,
                                                         120, 48, 96, 20,
                                                         channelImageChooseCbk );

          /*** Use image as texture ***/
    gtk3med->diffuseProcToggle  = ui_createToggleLabel ( pan,
                                                         gtk3med,
                                                         EDITCHANNELPROCEDURAL,
                                                         CLASS_MAIN,
                                                         0, 72, 96, 20, 20,
                                                         channelProceduralCbk );

    gtk3med->diffuseProcTypeCombo 
                                = ui_createProceduralSelector
                                                       ( pan,
                                                         gtk3med, 
                                                         EDITCHANNELPROCEDURALTYPE,
                                                         CLASS_MAIN,
                                                         0, 96, 96, 64, 20,
                                                         channelProceduralTypeCbk );

    /*** Procedural settings **/
    gtk3med->diffuseProcSettingsButton 
                                = ui_createPushButton  ( pan,
                                                         gtk3med, 
                                                         EDITCHANNELPROCEDURALSETTINGS,
                                                         CLASS_MAIN,
                                                         96, 120, 96, 18,
                                                         channelProceduralSettingsCbk );

    return pan;
}

/******************************************************************************/
void gtk3_g3duimaterialedit_update ( GTK3G3DUIMATERIALEDIT *gtk3med ) {
    G3DUI *gui = gtk3med->core.gui;

    gui->lock = 0x01;

    if ( gui->sce ) {
        G3DSCENE *sce = gui->sce;

        if ( gui->lselmat ) {
            gtk3med->core.editedMaterial = ( G3DMATERIAL * ) gui->lselmat->data;

            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3med->notebook), TRUE );

            updateDiffusePanel      ( gtk3med );
            updateSpecularPanel     ( gtk3med );
            updateBumpPanel         ( gtk3med );
            updateDisplacementPanel ( gtk3med );
            /*gtk3med->core.multi = ( nbsel > 0x01 ) ? 0x01 : 0x00;*/

/*
            updateSpecularityPanel  ( gtk3med );
            updateDiffuseColorPanel ( gtk3med );
            updateLightGeneralPanel ( gtk3med );
            updateShadowsPanel      ( gtk3med );
*/
        } else {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3med->notebook), FALSE );
        }
    }

    gui->lock = 0x00;
}


/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIMATERIALEDIT *gtk3med = ( GTK3G3DUIMATERIALEDIT * ) user_data;

    free ( gtk3med );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIMATERIALEDIT *gtk3med = ( GTK3G3DUIMATERIALEDIT * ) user_data;

    gtk3_g3duimaterialedit_update ( gtk3med );
}

/******************************************************************************/
GTK3G3DUIMATERIALEDIT *gtk3_g3duimaterialedit_create ( GtkWidget *parent,
                                                       GTK3G3DUI *gtk3gui,
                                                       char      *name ) {
    GTK3G3DUIMATERIALEDIT *gtk3med = gtk3_g3duimaterialedit_new ( gtk3gui );
    GtkWidget *scrolled = ui_gtk_scrolled_window_new ( CLASS_MAIN, NULL, NULL );
    GtkWidget *notebook = ui_gtk_notebook_new ( CLASS_MAIN );

    gtk3med->scrolled = scrolled;
    gtk3med->notebook = GTK_NOTEBOOK(notebook);

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(notebook), TRUE );

    gtk_widget_set_name ( notebook, name );

#if GTK_CHECK_VERSION(3,8,0)
    gtk_container_add ( GTK_CONTAINER(scrolled), notebook );
#else
    gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW(scrolled), notebook );
#endif

    /*gtk_widget_set_size_request ( tab, width, height );*/

    g_signal_connect ( G_OBJECT (notebook), "realize", G_CALLBACK (Realize), gtk3med );
    g_signal_connect ( G_OBJECT (notebook), "destroy", G_CALLBACK (Destroy), gtk3med );

    createDiffusePanel      ( gtk3med, 0, 0, 310, 250 );
    createSpecularPanel     ( gtk3med, 0, 0, 310, 250 );
    createBumpPanel         ( gtk3med, 0, 0, 310, 250 );
    createDisplacementPanel ( gtk3med, 0, 0, 310, 250 );
/*
    createLightGeneralPanel ( gtk3med, 0, 0, 310, 150 );
    createDiffuseColorPanel ( gtk3med, 0, 0, 310, 150 );
    createSpecularityPanel  ( gtk3med, 0, 0, 310, 150 );
    createShadowsPanel      ( gtk3med, 0, 0, 310, 150 );
*/

    gtk_widget_show ( notebook );
    gtk_widget_show ( scrolled );

    return gtk3med;
}
