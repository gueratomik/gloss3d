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

#define EDITRENDERGENERAL          "General"
#define EDITRENDEREFFECTS          "Effects"
#define EDITRENDERSETTINGS         "Settings"
#define EDITRENDERSTART            "From Frame"
#define EDITRENDEREND              "To Frame"
#define EDITRENDERFPS              "Framerate"
#define EDITRENDEROUTPUT           "Output file"
#define EDITRENDERFORMAT           "Format"
#define EDITRENDERCODEC            "Codec"
#define EDITRENDERWIDTH            "Render width"
#define EDITRENDERHEIGHT           "Render height"
#define EDITRENDERRATIO            "W/H ratio"

#define EDITRENDERMOTIONBLURFRAME                 "Motion Blur Settings"
#define EDITRENDERENABLEMOTIONBLUR                "Enabled"
#define EDITRENDERVECTORMOTIONBLUR                "Vector"
#define EDITRENDERSCENEMOTIONBLUR                 "Scene"
#define EDITRENDERMOTIONBLURSTRENGTH              "Strength"
#define EDITRENDERVECTORMOTIONBLURSAMPLES         "Samples"
#define EDITRENDERVECTORMOTIONBLURSUBSAMPLINGRATE "Sub-Sampl. %"
#define EDITRENDERSCENEMOTIONBLURITERATION        "Iterations"


#define EDITRENDERBACKGROUNDFRAME        "Background settings"
#define EDITRENDERBACKGROUNDCOLOR        "Color"
#define EDITRENDERBACKGROUNDIMAGE        "Image"
#define EDITRENDERBACKGROUNDSTRETCHIMAGE "Stretch Image"

#define EDITRENDERPREVIEW          "Make preview"
#define EDITRENDERRUN              "Run render"
#define EDITRENDERSAVEOUTPUTFRAME  "Output"
#define EDITRENDERSAVE             "Save result"


#define EDITRENDERALIASINGFRAME      "Anti-Aliasing"
#define EDITRENDERALIASING           "Enabled"
#define EDITRENDERALIASINGMODE       "Mode"
#define EDITRENDERALIASINGEDGE       "Edge"
#define EDITRENDERALIASINGFULL       "Full"
#define EDITRENDERALIASINGSAMPLES_5  "5 samples"
#define EDITRENDERALIASINGSAMPLES_9  "9 samples"

#define EDITRENDERWIREFRAMEFRAME     "Wireframe Settings"
#define EDITRENDERWIREFRAME          "Enabled"
#define EDITRENDERWIREFRAMELIGHTING  "Shaded"
#define EDITRENDERWIREFRAMETHICKNESS "Thickness"
#define EDITRENDERWIREFRAMECOLOR     "Color"

#define EDITRENDERFOGFRAME             "Fog Settings"
#define EDITRENDERFOGAFFECTSBACKGROUND "Affects background"
#define EDITRENDERFOG                  "Enabled"
#define EDITRENDERFOGNEAR              "Near distance"
#define EDITRENDERFOGFAR               "Far distance"
#define EDITRENDERFOGCOLOR             "Color"
#define EDITRENDERFOGSTRENGTH          "Strength"

#define EDITRENDERTEXTURINGFRAME       "Texturing"
#define EDITRENDERTEXTURINGCOLOR       "Mesh default color"
#define EDITRENDERTEXTURING            "Disable"

#define WINWIDTH 320

/******************************************************************************/
static GTK3G3DUIRENDEREDIT *gtk3_g3duirenderedit_new ( GTK3G3DUI   *gtk3gui,
                                                       Q3DSETTINGS *rsg ) {
    uint32_t memSize = sizeof ( GTK3G3DUIRENDEREDIT );
    GTK3G3DUIRENDEREDIT *gtk3red = calloc ( 0x01, memSize );

    if ( gtk3red == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );
    }

    gtk3red->core.gui = ( G3DUI * ) gtk3gui;
    gtk3red->core.editedRsg = rsg;


    return gtk3red; 
}

static void updateGeneralPanel    ( GTK3G3DUIRENDEREDIT *gtk3red );
static void updateSaveOutputFrame ( GTK3G3DUIRENDEREDIT *gtk3red );
static void updateMotionBlurFrame ( GTK3G3DUIRENDEREDIT *gtk3red );

/******************************************************************************/
static void formatCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    GtkComboBoxText *cmbt = GTK_COMBO_BOX_TEXT(widget);
    const char *str = gtk_combo_box_text_get_active_text ( cmbt );

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_format ( &gtk3red->core, str );

    updateSaveOutputFrame ( gtk3red );
}

/******************************************************************************/
static GtkComboBoxText *ui_createRenderFormat ( GtkWidget *parent, 
                                                void      *data,
                                                gint       x, 
                                                gint       y,
                                                gint       labwidth,
                                                gint       cmbwidth,
                                                gint       cmbheight,
                                                void     (*cbk)( GtkWidget *, 
                                                                 gpointer ) ) {
    GtkComboBoxText *cmb = ui_gtk_combo_box_text_new ( CLASS_MAIN );

    gtk_widget_set_name         ( GTK_WIDGET(cmb), EDITRENDERFORMAT );
    gtk_widget_set_size_request ( GTK_WIDGET(cmb), cmbwidth, cmbheight );

    gtk_fixed_put ( GTK_FIXED(parent), GTK_WIDGET(cmb), x + labwidth, y );

    if ( labwidth ) {
        GtkLabel *lab = ui_gtk_label_new ( CLASS_MAIN, EDITRENDERFORMAT );

        gtk_widget_set_size_request ( GTK_WIDGET(lab), labwidth, cmbheight );

        gtk_fixed_put ( GTK_FIXED(parent), GTK_WIDGET(lab), x, y );

        gtk_widget_show ( lab );
    }

    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, RENDERTOIMAGENAME );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, RENDERTOVIDEONAME );

    if ( cbk ) { 
        g_signal_connect ( cmb, "changed", G_CALLBACK(cbk), data );
    }

    gtk_widget_show ( cmb );


    return cmb;
}

/******************************************************************************/
static void saveCbk ( GtkWidget *widget, gpointer user_data ) {
    uint32_t save = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget));
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_save ( &gtk3red->core, save );

    updateSaveOutputFrame ( gtk3red );
}


/******************************************************************************/
static void previewCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_preview ( &gtk3red->core );
}

/******************************************************************************/
static void startFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    float frame = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_startFrame ( &gtk3red->core, frame );

    updateGeneralPanel ( gtk3red );
}

/******************************************************************************/
static void fpsCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    float fps = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_fps ( &gtk3red->core, fps );
}

/******************************************************************************/
static void endFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    float frame = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_endFrame ( &gtk3red->core, frame );

    updateGeneralPanel ( gtk3red );
}

/******************************************************************************/
static void ratioCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    float ratio = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_ratio ( &gtk3red->core, ratio );

    updateGeneralPanel ( gtk3red );
}

/******************************************************************************/
static void widthCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    int width = (int) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_width ( &gtk3red->core, ( uint32_t ) width );

    updateGeneralPanel ( gtk3red );
}

/******************************************************************************/
static void heightCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    int height = (int) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_height ( &gtk3red->core, ( uint32_t ) height );

    updateGeneralPanel ( gtk3red );
}

/******************************************************************************/
static void outputCbk ( GtkWidget *widget, 
                        GdkEvent  *event,
                        gpointer   user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    const char *outfile = gtk_entry_get_text ( GTK_ENTRY(widget) );

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_output ( &gtk3red->core, outfile );
}

/******************************************************************************/
static void backgroundCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    GdkRGBA color;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    gtk_color_chooser_get_rgba ( GTK_COLOR_CHOOSER(widget), &color );

    g3duirenderedit_background ( &gtk3red->core, ( color.red   * 255 ), 
                                                    ( color.green * 255 ),
                                                    ( color.blue  * 255 ) );
}

/******************************************************************************/
static void wireframeColorCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    GdkRGBA color;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    gtk_color_chooser_get_rgba ( GTK_COLOR_CHOOSER(widget), &color );

    g3duirenderedit_wireframeColor ( &gtk3red->core, ( color.red   * 255 ), 
                                                        ( color.green * 255 ),
                                                        ( color.blue  * 255 ) );
}

/******************************************************************************/
static void runRenderCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) user_data;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    gtk3_runRender ( gtk3gui );
}

/******************************************************************************/
static void setMotionBlurCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_setMotionBlur ( &gtk3red->core );

    updateMotionBlurFrame ( gtk3red );
}

/******************************************************************************/
static void sceneMotionBlurCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_sceneMotionBlur ( &gtk3red->core );

    updateMotionBlurFrame ( gtk3red );
}

/******************************************************************************/
static void sceneMotionBlurIterationCbk ( GtkWidget *widget, gpointer user_data ) {
    uint32_t iteration = gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_sceneMotionBlurIteration ( &gtk3red->core, iteration );
}

/******************************************************************************/
static void vectorMotionBlurCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_vectorMotionBlur ( &gtk3red->core );

    updateMotionBlurFrame ( gtk3red );
}

/******************************************************************************/
static void motionBlurStrengthCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = gtk3red->core.gui;
    float strength = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_motionBlurStrength ( &gtk3red->core, strength );
}

/******************************************************************************/
static void vectorMotionBlurSamplesCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    uint32_t samples = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_vectorMotionBlurSamples ( &gtk3red->core, samples );
}

/******************************************************************************/
static void vectorMotionBlurSubSamplingRateCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    float rate = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_vectorMotionBlurSubSamplingRate ( &gtk3red->core, rate / 100.0f );
}

/******************************************************************************/
static void chooseCodecCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = gtk3red->core.gui;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

#ifdef __MINGW32__
    gui->cvars.cbSize = sizeof ( COMPVARS );

    ICCompressorChoose( gdk_win32_window_get_handle  ( gtk_widget_get_window ( widget ) ),
                        ICMF_CHOOSE_ALLCOMPRESSORS | 
                        ICMF_CHOOSE_DATARATE |
                        ICMF_CHOOSE_KEYFRAME,
                        NULL,
                        NULL,
                       &gui->cvars,
                        "Choose output video format" );
#endif
}

/******************************************************************************/
static void updateSaveOutputFrame ( GTK3G3DUIRENDEREDIT *gtk3red ) {
    G3DUI *gui = gtk3red->core.gui;
    Q3DSETTINGS *rsg = gtk3red->core.editedRsg;

    gui->lock = 0x01;

    if ( rsg->flags & RENDERSAVE ) {
        uint32_t tovid = ( rsg->output.format == RENDERTOVIDEO );

        gtk_toggle_button_set_active ( gtk3red->outputEnabledToggle, TRUE  );

        gtk_widget_set_sensitive ( gtk3red->outputFileEntry     , TRUE );
        gtk_widget_set_sensitive ( gtk3red->outputFormatSelector, TRUE );

#ifdef __MINGW32__
        gtk_widget_set_sensitive ( gtk3red->outputCodecButton, tovid );
#endif
    } else {
        gtk_toggle_button_set_active ( gtk3red->outputEnabledToggle, FALSE );

        gtk_widget_set_sensitive ( gtk3red->outputFileEntry     , FALSE );
        gtk_widget_set_sensitive ( gtk3red->outputFormatSelector, FALSE );

#ifdef __MINGW32__
        gtk_widget_set_sensitive ( gtk3red->outputCodecButton, FALSE );
#endif
    }

    gtk_entry_set_text ( gtk3red->outputFileEntry, rsg->output.outfile );
    gtk_combo_box_set_active ( gtk3red->outputFormatSelector,
                                rsg->output.format );

    gui->lock = 0x00;
}

/******************************************************************************/
static void createSaveOutputFrame ( GTK3G3DUIRENDEREDIT *gtk3red,
                                    gint                 x,
                                    gint                 y,
                                    gint                 width,
                                    gint                 height ) {
    G3DUI *gui = gtk3red->core.gui;
    GtkFixed *fix = ui_gtk_fixed_new ( CLASS_MAIN );
    GtkFrame *frm = ui_gtk_frame_new ( CLASS_MAIN, EDITRENDERSAVEOUTPUTFRAME );

    gtk_container_add ( GTK_CONTAINER(frm), fix );

    gtk_widget_set_size_request ( GTK_WIDGET(frm), width, height );

    gtk_fixed_put ( gtk3red->generalTab, frm, x, y );

    gtk3red->outputEnabledToggle = 
                          ui_createToggleLabel  ( fix,
                                                  gtk3red,
                                                  EDITRENDERSAVE,
                                                  CLASS_MAIN,
                                                  0,  0, 120, 20, 20,
                                                  saveCbk );

    gtk3red->outputFormatSelector =
                          ui_createRenderFormat ( fix,
                                                  gtk3red,
                                                  0, 24, 120, 96, 20,
                                                  formatCbk );

#ifdef __MINGW32__
    gtk3red->outputCodecButton = 
                          ui_createPushButton   ( fix,
                                                  gtk3red,
                                                  EDITRENDERCODEC,
                                                  CLASS_MAIN,
                                                  96, 48, 96, 20, 20,
                                                  chooseCodecCbk );
#endif
    gtk3red->outputFileEntry = 
                          ui_createCharText     ( fix, 
                                                  gtk3red,
                                                  EDITRENDEROUTPUT,
                                                  CLASS_ENTRY,
                                                  0, 72, 120, 200, 20,
                                                  outputCbk );

    gtk_widget_show ( fix );
    gtk_widget_show ( frm );
}

/******************************************************************************/
static void updateAliasingFrame ( GTK3G3DUIRENDEREDIT *gtk3red ) {
    G3DUI *gui = gtk3red->core.gui;
    Q3DSETTINGS *rsg = gtk3red->core.editedRsg;

    gui->lock = 0x01;

    if ( rsg->flags & ENABLEAA ) {
        gtk_toggle_button_set_active ( gtk3red->aliasingEnabledToggle, TRUE  );

        gtk_widget_set_sensitive ( gtk3red->aliasingEdgeToggle, TRUE );
        gtk_widget_set_sensitive ( gtk3red->aliasingFullToggle, TRUE );
        gtk_widget_set_sensitive ( gtk3red->aliasingS5Toggle  , TRUE );
        gtk_widget_set_sensitive ( gtk3red->aliasingS9Toggle  , TRUE );
    } else {
        gtk_toggle_button_set_active ( gtk3red->aliasingEnabledToggle, FALSE );

        gtk_widget_set_sensitive ( gtk3red->aliasingEdgeToggle, FALSE );
        gtk_widget_set_sensitive ( gtk3red->aliasingFullToggle, FALSE );
        gtk_widget_set_sensitive ( gtk3red->aliasingS5Toggle  , FALSE );
        gtk_widget_set_sensitive ( gtk3red->aliasingS9Toggle  , FALSE );
    }

    gtk_toggle_button_set_active ( gtk3red->aliasingEdgeToggle, ( rsg->aa.mode == AAEDGEMODE ) );
    gtk_toggle_button_set_active ( gtk3red->aliasingFullToggle, ( rsg->aa.mode == AAFULLMODE ) );
    gtk_toggle_button_set_active ( gtk3red->aliasingS5Toggle  , ( rsg->aa.nbsamples == 0x05 ) );
    gtk_toggle_button_set_active ( gtk3red->aliasingS9Toggle  , ( rsg->aa.nbsamples == 0x09 ) );

    gui->lock = 0x00;
}

/******************************************************************************/
static void aliasingCbk ( GtkWidget *widget, gpointer user_data ) {
    uint32_t aa = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget));
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_aa ( &gtk3red->core );

    updateAliasingFrame ( gtk3red );
}

/******************************************************************************/
static void aliasingEdgeCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_aaEdge ( &gtk3red->core );
}

/******************************************************************************/
static void aliasingFullCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_aaFull ( &gtk3red->core );
}

/******************************************************************************/
static void aliasingS5Cbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_aaSamples ( &gtk3red->core, 5 );
}

/******************************************************************************/
static void aliasingS9Cbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_aaSamples ( &gtk3red->core, 9 );
}

/******************************************************************************/
static GtkWidget *createAliasingFrame ( GTK3G3DUIRENDEREDIT *gtk3red,
                                        gint                 x,
                                        gint                 y,
                                        gint                 width,
                                        gint                 height ) {
    G3DUI *gui = gtk3red->core.gui;
    GtkFixed *fix = ui_gtk_fixed_new ( CLASS_MAIN );
    GtkFrame *frm = ui_gtk_frame_new ( CLASS_MAIN, EDITRENDERALIASINGFRAME );

    gtk_container_add ( GTK_CONTAINER(frm), fix );

    gtk_widget_set_size_request ( GTK_WIDGET(frm), width, height );

    gtk_fixed_put ( gtk3red->generalTab, frm, x, y );

    gtk3red->aliasingEnabledToggle = 
                        ui_createToggleLabel ( fix,
                                               gtk3red,
                                               EDITRENDERALIASING,
                                               CLASS_MAIN,
                                               0,  0, 120, 20, 20,
                                               aliasingCbk );

    gtk3red->aliasingEdgeToggle = 
                        ui_createToggleLabel ( fix,
                                               gtk3red,
                                               EDITRENDERALIASINGEDGE,
                                               CLASS_MAIN,
                                               0, 24, 120, 20, 20,
                                               aliasingEdgeCbk );

    gtk3red->aliasingFullToggle = 
                        ui_createToggleLabel ( fix,
                                               gtk3red,
                                               EDITRENDERALIASINGFULL,
                                               CLASS_MAIN,
                                               0, 48, 120, 20, 20,
                                               aliasingFullCbk );

    gtk3red->aliasingS5Toggle = 
                        ui_createToggleLabel ( fix,
                                               gtk3red,
                                               EDITRENDERALIASINGSAMPLES_5,
                                               CLASS_MAIN,
                                               120, 24, 96, 20, 20,
                                               aliasingS5Cbk );

    gtk3red->aliasingS9Toggle = 
                        ui_createToggleLabel ( fix,
                                               gtk3red,
                                               EDITRENDERALIASINGSAMPLES_9,
                                               CLASS_MAIN,
                                               120, 48, 96, 20, 20,
                                               aliasingS9Cbk );

    gtk_widget_show ( fix );
    gtk_widget_show ( frm );
}

/******************************************************************************/
static void updateMotionBlurFrame ( GTK3G3DUIRENDEREDIT *gtk3red ) {
    G3DUI *gui = gtk3red->core.gui;
    Q3DSETTINGS *rsg = gtk3red->core.editedRsg;

    gui->lock = 0x01;

    if ( rsg->flags & ENABLEMOTIONBLUR ) {
        uint32_t vblur = ( rsg->flags & VECTORMOTIONBLUR );
        uint32_t sblur = ( rsg->flags & SCENEMOTIONBLUR  );

        gtk_toggle_button_set_active ( gtk3red->blurEnabledToggle, TRUE  );

        gtk_widget_set_sensitive ( gtk3red->blurVectorEnabledToggle, TRUE );
        gtk_widget_set_sensitive ( gtk3red->blurSceneEnabledToggle , TRUE );
        gtk_widget_set_sensitive ( gtk3red->blurStrengthEntry     , TRUE );

        gtk_widget_set_sensitive ( gtk3red->blurSamplesEntry    , vblur );
        gtk_widget_set_sensitive ( gtk3red->blurSubSamplingEntry, vblur );

        gtk_widget_set_sensitive ( gtk3red->blurIterationsEntry, sblur );
    } else {
        gtk_toggle_button_set_active ( gtk3red->blurEnabledToggle, FALSE );

        gtk_widget_set_sensitive ( gtk3red->blurVectorEnabledToggle, FALSE );
        gtk_widget_set_sensitive ( gtk3red->blurSceneEnabledToggle , FALSE );
        gtk_widget_set_sensitive ( gtk3red->blurStrengthEntry     , FALSE );

        gtk_widget_set_sensitive ( gtk3red->blurSamplesEntry    , FALSE );
        gtk_widget_set_sensitive ( gtk3red->blurSubSamplingEntry, FALSE );

        gtk_widget_set_sensitive ( gtk3red->blurIterationsEntry, FALSE );
    }

    if ( rsg->flags & VECTORMOTIONBLUR ) {
        gtk_toggle_button_set_active ( gtk3red->blurVectorEnabledToggle, TRUE  );
    } else {
        gtk_toggle_button_set_active ( gtk3red->blurVectorEnabledToggle, FALSE );
    }

    if ( rsg->flags & SCENEMOTIONBLUR ) {
        gtk_toggle_button_set_active ( gtk3red->blurSceneEnabledToggle, TRUE  );
    } else {
        gtk_toggle_button_set_active ( gtk3red->blurSceneEnabledToggle, FALSE );
    }

    gtk_spin_button_set_value ( gtk3red->blurIterationsEntry,
                                rsg->motionBlur.iterations );

    gtk_spin_button_set_value ( gtk3red->blurStrengthEntry,
                                rsg->motionBlur.strength * 100.0f );

    gtk_spin_button_set_value ( gtk3red->blurSamplesEntry, 
                                rsg->motionBlur.vMotionBlurSamples );

    gtk_spin_button_set_value ( gtk3red->blurSubSamplingEntry, 
                                rsg->motionBlur.vMotionBlurSubSamplingRate * 100 );

    gui->lock = 0x00;
}

/******************************************************************************/
static void createMotionBlurFrame ( GTK3G3DUIRENDEREDIT *gtk3red,
                                    gint                 x,
                                    gint                 y,
                                    gint                 width,
                                    gint                 height ) {
    G3DUI *gui = gtk3red->core.gui;
    GtkFixed *fix = ui_gtk_fixed_new ( CLASS_MAIN );
    GtkFrame *frm = ui_gtk_frame_new ( CLASS_MAIN, EDITRENDERMOTIONBLURFRAME );


    gtk_container_add (GTK_CONTAINER(frm), fix );

    gtk_widget_set_size_request ( GTK_WIDGET(frm), width, height );

    gtk_fixed_put ( gtk3red->effectsTab, frm, x, y );

    gtk3red->blurEnabledToggle = 
               ui_createToggleLabel ( fix,
                                      gtk3red,
                                      EDITRENDERENABLEMOTIONBLUR,
                                      CLASS_MAIN,
                                      0,  0, 120, 20, 20,
                                      setMotionBlurCbk );

    gtk3red->blurStrengthEntry = 
               ui_createIntegerText ( fix,
                                      gtk3red,
                                      EDITRENDERMOTIONBLURSTRENGTH,
                                      CLASS_MAIN,
                                      0, 100,
                                      160, 0, 96, 96, 20,
                                      motionBlurStrengthCbk );

    gtk3red->blurVectorEnabledToggle = 
               ui_createToggleLabel ( fix,
                                      gtk3red,
                                      EDITRENDERVECTORMOTIONBLUR,
                                      CLASS_MAIN,
                                      0,  24, 120, 20, 20,
                                      vectorMotionBlurCbk );

    gtk3red->blurSamplesEntry = 
               ui_createIntegerText ( fix,
                                      gtk3red,
                                      EDITRENDERVECTORMOTIONBLURSAMPLES,
                                      CLASS_MAIN,
                                      0, 100,
                                      160, 24, 96, 96, 20,
                                      vectorMotionBlurSamplesCbk );

    gtk3red->blurSubSamplingEntry = 
               ui_createIntegerText ( fix,
                                      gtk3red,
                                      EDITRENDERVECTORMOTIONBLURSUBSAMPLINGRATE,
                                      CLASS_MAIN,
                                      0, 100,
                                      160, 48, 96, 96, 20,
                                      vectorMotionBlurSubSamplingRateCbk );

    gtk3red->blurSceneEnabledToggle =
               ui_createToggleLabel ( fix,
                                      gtk3red,
                                      EDITRENDERSCENEMOTIONBLUR,
                                      CLASS_MAIN,
                                      0, 72, 120, 20, 20,
                                      sceneMotionBlurCbk );

    gtk3red->blurIterationsEntry = 
               ui_createIntegerText ( fix,
                                      gtk3red,
                                      EDITRENDERSCENEMOTIONBLURITERATION,
                                      CLASS_MAIN,
                                      2, 31,
                                      160, 72, 96, 96, 20,
                                      sceneMotionBlurIterationCbk );

    gtk_widget_show ( fix );
    gtk_widget_show ( frm );
}

/******************************************************************************/
static void updateWireframeFrame ( GTK3G3DUIRENDEREDIT *gtk3red ) {
    G3DUI *gui = gtk3red->core.gui;
    Q3DSETTINGS *rsg = gtk3red->core.editedRsg;
    unsigned char R = ( rsg->wireframe.color & 0x00FF0000 ) >> 0x10,
                  G = ( rsg->wireframe.color & 0x0000FF00 ) >> 0x08,
                  B = ( rsg->wireframe.color & 0x000000FF );
    GdkRGBA rgba = { .red   = ( float ) R / 255,
                     .green = ( float ) G / 255,
                     .blue  = ( float ) B / 255,
                     .alpha = 1.0f };

    gui->lock = 0x01;

    if ( ( rsg->flags & RENDERWIREFRAME ) ) {
        gtk_toggle_button_set_active ( gtk3red->wireframeEnabledToggle, TRUE  );

        gtk_widget_set_sensitive ( gtk3red->wireframeAffectedToggle, TRUE );
        gtk_widget_set_sensitive ( gtk3red->wireframeThicknessEntry, TRUE );
        gtk_widget_set_sensitive ( gtk3red->wireframeColorButton   , TRUE );
    } else {
        gtk_toggle_button_set_active ( gtk3red->wireframeEnabledToggle, FALSE );

        gtk_widget_set_sensitive ( gtk3red->wireframeAffectedToggle, FALSE );
        gtk_widget_set_sensitive ( gtk3red->wireframeThicknessEntry, FALSE );
        gtk_widget_set_sensitive ( gtk3red->wireframeColorButton   , FALSE );
    }

    if ( rsg->flags & WIREFRAMELIGHTING ) {
        gtk_toggle_button_set_active ( gtk3red->wireframeAffectedToggle, TRUE  );
    } else {
        gtk_toggle_button_set_active ( gtk3red->wireframeAffectedToggle, FALSE );
    }

    gtk_spin_button_set_value ( gtk3red->wireframeThicknessEntry,
                                    rsg->wireframe.thickness );
    gtk_color_chooser_set_rgba ( gtk3red->wireframeColorButton, &rgba );

    gui->lock = 0x00;
}

/******************************************************************************/
static void setWireframeCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_setWireframe ( &gtk3red->core );

    updateWireframeFrame( gtk3red );
}

/******************************************************************************/
static void setWireframeLightingCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_setWireframeLighting ( &gtk3red->core );

    updateWireframeFrame ( gtk3red );
}

/******************************************************************************/
static void wireframeThicknessCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    float thickness = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_wireframeThickness ( &gtk3red->core, thickness );
}

/******************************************************************************/
static GtkWidget *createWireframeFrame ( GTK3G3DUIRENDEREDIT *gtk3red,
                                         gint                 x,
                                         gint                 y,
                                         gint                 width,
                                         gint                 height ) {
    G3DUI *gui = gtk3red->core.gui;
    GtkFixed *fix = ui_gtk_fixed_new ( CLASS_MAIN );
    GtkFrame *frm = ui_gtk_frame_new ( CLASS_MAIN, EDITRENDERWIREFRAMEFRAME );

    gtk_container_add ( GTK_CONTAINER(frm), fix );

    gtk_widget_set_size_request ( GTK_WIDGET(frm), width, height );

    gtk_fixed_put ( gtk3red->effectsTab, frm, x, y );

    gtk3red->wireframeEnabledToggle =
                            ui_createToggleLabel ( fix, 
                                                   gtk3red,
                                                   EDITRENDERWIREFRAME,
                                                   CLASS_MAIN,
                                                   0,  0, 120, 20, 20,
                                                   setWireframeCbk );

    gtk3red->wireframeAffectedToggle = 
                            ui_createToggleLabel ( fix,
                                                   gtk3red,
                                                   EDITRENDERWIREFRAMELIGHTING,
                                                   CLASS_MAIN,
                                                   0, 24, 120, 20, 20,
                                                   setWireframeLightingCbk );

    gtk3red->wireframeThicknessEntry = 
                            ui_createFloatText   ( fix,
                                                   gtk3red,
                                                   EDITRENDERWIREFRAMETHICKNESS,
                                                   CLASS_MAIN,
                                                   0.0f, FLT_MAX,
                                                   0, 48, 120,  96, 20,
                                                   wireframeThicknessCbk );

    gtk3red->wireframeColorButton =
                            ui_createColorButton ( fix,
                                                   gtk3red,
                                                   EDITRENDERWIREFRAMECOLOR,
                                                   CLASS_MAIN,
                                                   0, 72, 120, 32, 20,
                                                   wireframeColorCbk );

    gtk_widget_show ( fix );
    gtk_widget_show ( frm );
}

/******************************************************************************/
static void updateTexturingFrame ( GTK3G3DUIRENDEREDIT *gtk3red ) {
    G3DUI *gui = gtk3red->core.gui;
    Q3DSETTINGS *rsg = gtk3red->core.editedRsg;
    GdkRGBA rgba = { .red   = ( float ) rsg->defaultColor.r / 255,
                     .green = ( float ) rsg->defaultColor.g / 255,
                     .blue  = ( float ) rsg->defaultColor.b / 255,
                     .alpha = 1.0f };

    gui->lock = 0x01;

    if ( rsg->flags & DISABLETEXTURING ) {
        gtk_toggle_button_set_active ( gtk3red->texturingDisabledToggle, TRUE  );
    } else {
        gtk_toggle_button_set_active ( gtk3red->texturingDisabledToggle, FALSE );
    }

    gtk_color_chooser_set_rgba ( gtk3red->texturingColorButton, &rgba );


    gui->lock = 0x00;
}

/******************************************************************************/
static void texturingColorCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = gtk3red->core.gui;
    GdkRGBA color;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    gtk_color_chooser_get_rgba ( GTK_COLOR_CHOOSER(widget), &color );

    g3duirenderedit_texturingColor ( &gtk3red->core, ( color.red   * 255 ), 
                                                        ( color.green * 255 ),
                                                        ( color.blue  * 255 ) );
}

/******************************************************************************/
static void setTexturingCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_setTexturing ( &gtk3red->core );
}

/******************************************************************************/
static void createTexturingFrame ( GTK3G3DUIRENDEREDIT *gtk3red,
                                   gint                 x,
                                   gint                 y,
                                   gint                 width,
                                   gint                 height ) {
    G3DUI *gui = gtk3red->core.gui;
    GtkFixed *fix = ui_gtk_fixed_new ( CLASS_MAIN );
    GtkFrame *frm = ui_gtk_frame_new ( CLASS_MAIN, EDITRENDERTEXTURINGFRAME );

    gtk_container_add (GTK_CONTAINER(frm), fix );

    gtk_fixed_put ( gtk3red->effectsTab, frm, x, y );

    gtk_widget_set_size_request ( GTK_WIDGET(frm), width, height );

    gtk3red->texturingDisabledToggle =
                              ui_createToggleLabel ( fix,
                                                     gtk3red,
                                                     EDITRENDERTEXTURING,
                                                     CLASS_MAIN,
                                                     0,  0, 120, 20, 20,
                                                     setTexturingCbk );

    gtk3red->texturingColorButton =
                              ui_createColorButton ( fix, 
                                                     gtk3red,
                                                     EDITRENDERTEXTURINGCOLOR,
                                                     CLASS_MAIN,
                                                     0, 24, 120, 32, 20, 
                                                     texturingColorCbk );

    gtk_widget_show ( fix );
    gtk_widget_show ( frm );
}


/******************************************************************************/
static void updateFogFrame ( GTK3G3DUIRENDEREDIT *gtk3red ) {
    G3DUI *gui = gtk3red->core.gui;
    Q3DSETTINGS *rsg = gtk3red->core.editedRsg;
    unsigned char R = ( rsg->fog.color & 0x00FF0000 ) >> 0x10,
                  G = ( rsg->fog.color & 0x0000FF00 ) >> 0x08,
                  B = ( rsg->fog.color & 0x000000FF );
    GdkRGBA rgba = { .red   = ( float ) R / 255,
                     .green = ( float ) G / 255,
                     .blue  = ( float ) B / 255,
                     .alpha = 1.0f };


    gui->lock = 0x01;

    if ( rsg->flags & RENDERFOG ) {
        gtk_toggle_button_set_active ( gtk3red->fogEnabledToggle, TRUE  );

        gtk_widget_set_sensitive ( gtk3red->fogAffectsBackgroundToggle, TRUE );
        gtk_widget_set_sensitive ( gtk3red->fogStrengthEntry          , TRUE );
        gtk_widget_set_sensitive ( gtk3red->fogNearEntry              , TRUE );
        gtk_widget_set_sensitive ( gtk3red->fogFarEntry               , TRUE );
        gtk_widget_set_sensitive ( gtk3red->fogColorButton            , TRUE );
    } else {
        gtk_toggle_button_set_active ( gtk3red->fogEnabledToggle, FALSE );

        gtk_widget_set_sensitive ( gtk3red->fogAffectsBackgroundToggle, FALSE );
        gtk_widget_set_sensitive ( gtk3red->fogStrengthEntry          , FALSE );
        gtk_widget_set_sensitive ( gtk3red->fogNearEntry              , FALSE );
        gtk_widget_set_sensitive ( gtk3red->fogFarEntry               , FALSE );
        gtk_widget_set_sensitive ( gtk3red->fogColorButton            , FALSE );
    }

    if ( rsg->fog.flags & FOGAFFECTSBACKGROUND ) {
        gtk_toggle_button_set_active ( gtk3red->fogAffectsBackgroundToggle, TRUE  );
    } else {
        gtk_toggle_button_set_active ( gtk3red->fogAffectsBackgroundToggle, FALSE );
    }

    gtk_spin_button_set_value  ( gtk3red->fogStrengthEntry, rsg->fog.strength * 100.0f );
    gtk_spin_button_set_value  ( gtk3red->fogNearEntry, rsg->fog.fnear );
    gtk_spin_button_set_value  ( gtk3red->fogFarEntry, rsg->fog.ffar );
    gtk_color_chooser_set_rgba ( gtk3red->fogColorButton, &rgba );


    gui->lock = 0x00;
}

/******************************************************************************/
static void setFogAffectsBackgroundCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;

    g3duirenderedit_setFogAffectsBackground ( &gtk3red->core );
}

/******************************************************************************/
static void setFogCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_setFog ( &gtk3red->core );

    updateFogFrame ( gtk3red );
}

/******************************************************************************/
static void fogStrengthCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    float strength = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_setFogStrength ( &gtk3red->core, strength );
}

/******************************************************************************/
static void fogNearCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    float fnear = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_fogNear ( &gtk3red->core, fnear );
}

/******************************************************************************/
static void fogFarCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    float ffar = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_fogFar ( &gtk3red->core, ffar );
}

/******************************************************************************/
static void fogColorCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    GdkRGBA color;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    gtk_color_chooser_get_rgba ( GTK_COLOR_CHOOSER(widget), &color );

    g3duirenderedit_fogColor ( &gtk3red->core, ( color.red   * 255 ), 
                                                  ( color.green * 255 ),
                                                  ( color.blue  * 255 ) );
}

/******************************************************************************/
static void createFogFrame ( GTK3G3DUIRENDEREDIT *gtk3red,
                             gint                 x,
                             gint                 y,
                             gint                 width,
                             gint                 height ) {
    G3DUI *gui = gtk3red->core.gui;
    GtkFixed *fix = ui_gtk_fixed_new ( CLASS_MAIN );
    GtkFrame *frm = ui_gtk_frame_new ( CLASS_MAIN, EDITRENDERFOGFRAME );

    gtk_container_add (GTK_CONTAINER(frm), fix );

    gtk_widget_set_size_request ( GTK_WIDGET(frm), width, height );

    gtk_fixed_put ( gtk3red->effectsTab, frm, x, y );

    gtk3red->fogEnabledToggle = 
                         ui_createToggleLabel ( fix,
                                                gtk3red,
                                                EDITRENDERFOG,
                                                CLASS_MAIN,
                                                0,  0, 120, 20, 20,
                                                setFogCbk );

    gtk3red->fogAffectsBackgroundToggle = 
                         ui_createToggleLabel ( fix,
                                                gtk3red,
                                                EDITRENDERFOGAFFECTSBACKGROUND,
                                                CLASS_MAIN,
                                                0, 24, 120, 20, 20,
                                                setFogAffectsBackgroundCbk );

    gtk3red->fogStrengthEntry = 
                         ui_createFloatText   ( fix,
                                                gtk3red,
                                                EDITRENDERFOGSTRENGTH,
                                                CLASS_MAIN,
                                                0.0f, 100.0f,
                                                0, 48, 120, 96, 20,
                                                fogStrengthCbk );

    gtk3red->fogNearEntry =
                         ui_createFloatText   ( fix,
                                                gtk3red,
                                                EDITRENDERFOGNEAR,
                                                CLASS_MAIN,
                                                0.0f,
                                                FLT_MAX,
                                                0, 72, 120, 96, 20,
                                                fogNearCbk );

    gtk3red->fogFarEntry = 
                         ui_createFloatText   ( fix,
                                                gtk3red,
                                                EDITRENDERFOGFAR,
                                                CLASS_MAIN,
                                                0.0f,
                                                FLT_MAX,
                                                0, 96, 120, 96, 20,
                                                fogFarCbk );

    gtk3red->fogColorButton = 
                         ui_createColorButton ( fix, 
                                                gtk3red,
                                                EDITRENDERFOGCOLOR,
                                                CLASS_MAIN,
                                                0, 120, 120, 32, 20, 
                                                fogColorCbk );

    gtk_widget_show ( fix );
    gtk_widget_show ( frm );
}

/******************************************************************************/
static void updateBackgroundFrame ( GTK3G3DUIRENDEREDIT *gtk3red ) {
    G3DUI *gui = gtk3red->core.gui;
    Q3DSETTINGS *rsg = gtk3red->core.editedRsg;
    unsigned char R = ( rsg->background.color & 0x00FF0000 ) >> 0x10,
                  G = ( rsg->background.color & 0x0000FF00 ) >> 0x08,
                  B = ( rsg->background.color & 0x000000FF );
    GdkRGBA rgba = { .red   = ( float ) R / 255,
                     .green = ( float ) G / 255,
                     .blue  = ( float ) B / 255,
                     .alpha = 1.0f };

    gui->lock = 0x01;

    if ( ( rsg->background.mode & BACKGROUND_IMAGE ) ) {
        gtk_toggle_button_set_active ( gtk3red->backgroundImageToggle, TRUE   );
        gtk_toggle_button_set_active ( gtk3red->backgroundColorToggle, FALSE  );
    } else {
        gtk_toggle_button_set_active ( gtk3red->backgroundImageToggle, FALSE );
        gtk_toggle_button_set_active ( gtk3red->backgroundColorToggle, TRUE  );
    }

    if ( rsg->background.image && 
         rsg->background.image->filename ) {
        char *imgpath, *imgname;

        imgpath = strdup ( rsg->background.image->filename );

        /*** We just keep the image name, not the whole ***/
        /*** path and display it as the button label.   ***/
        imgname = basename ( imgpath );

        gtk_button_set_label ( gtk3red->backgroundImageButton, imgname );

        free ( imgpath );
    }

    gtk_color_chooser_set_rgba ( gtk3red->backgroundColorButton, &rgba );

    gui->lock = 0x00;
}

/******************************************************************************/
static void setBackgroundImageCbk ( GtkWidget *widget,
                                    gpointer   user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3red->core.gui;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Open File",
                                           GTK_WINDOW(gtk3gui->topWin),
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

        g3duirenderedit_setBackgroundImage ( &gtk3red->core, filename );

        g_free ( ( gpointer ) filename );
    }

    updateBackgroundFrame ( gtk3red );

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
static void setBackgroundColorModeCbk ( GtkWidget *widget, 
                                        gpointer   user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_setBackgroundColorMode ( &gtk3red->core );

    updateBackgroundFrame ( gtk3red );
}

/******************************************************************************/
static void setBackgroundImageModeCbk ( GtkWidget *widget, 
                                        gpointer   user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;

    /*** prevents loop ***/
    if ( gtk3red->core.gui->lock ) return;

    g3duirenderedit_setBackgroundImageMode ( &gtk3red->core );

    updateBackgroundFrame ( gtk3red );
}

/******************************************************************************/
static void createBackgroundFrame ( GTK3G3DUIRENDEREDIT *gtk3red,
                                    gint                 x,
                                    gint                 y,
                                    gint                 width,
                                    gint                 height ) {
    GtkFixed *fix = ui_gtk_fixed_new ( CLASS_MAIN );
    GtkFrame *frm = ui_gtk_frame_new ( CLASS_MAIN, EDITRENDERBACKGROUNDFRAME );

    gtk_widget_set_size_request ( GTK_WIDGET(fix), width, height );

    gtk_container_add (GTK_CONTAINER(frm), fix );

    gtk_fixed_put ( gtk3red->generalTab, frm, x, y );

    gtk3red->backgroundColorToggle  =
                          ui_createToggleLabel ( fix, 
                                                 gtk3red,
                                                 EDITRENDERBACKGROUNDCOLOR,
                                                 CLASS_MAIN,
                                                 0,  0, 120, 20, 20,
                                                 setBackgroundColorModeCbk );

    gtk3red->backgroundImageToggle  = 
                          ui_createToggleLabel ( fix,
                                                 gtk3red,
                                                 EDITRENDERBACKGROUNDIMAGE,
                                                 CLASS_MAIN,
                                                 0, 24, 120, 20, 20,
                                                 setBackgroundImageModeCbk );

    gtk3red->backgroundColorButton =
                          ui_createColorButton ( fix,
                                                 gtk3red,
                                                 EDITRENDERBACKGROUNDCOLOR,
                                                 CLASS_MAIN,
                                                 160, 0, 0, 32, 20,
                                                 backgroundCbk );

    gtk3red->backgroundImageButton = 
                          ui_createPushButton  ( fix,
                                                 gtk3red,
                                                 EDITRENDERBACKGROUNDIMAGE,
                                                 CLASS_MAIN,
                                                 160, 24, 96, /*96,*/ 20,
                                                 setBackgroundImageCbk );

    gtk_widget_show ( fix );
    gtk_widget_show ( frm );

}

/******************************************************************************/
static void updateGeneralPanel ( GTK3G3DUIRENDEREDIT *gtk3red ) {
    G3DUI *gui = gtk3red->core.gui;
    Q3DSETTINGS *rsg = gtk3red->core.editedRsg;

    updateSaveOutputFrame ( gtk3red );
    updateBackgroundFrame ( gtk3red );
    updateAliasingFrame   ( gtk3red );

    gui->lock = 0x01;

    if ( rsg->flags & RENDERPREVIEW ) {
        gtk_toggle_button_set_active ( gtk3red->renderPreviewToggle, TRUE  );
    } else {
        gtk_toggle_button_set_active ( gtk3red->renderPreviewToggle, FALSE );
    }

    gtk_spin_button_set_value ( gtk3red->fromFrameEntry   , rsg->output.startframe );
    gtk_spin_button_set_value ( gtk3red->toFrameEntry     , rsg->output.endframe   );
    gtk_spin_button_set_value ( gtk3red->renderRatioEntry , rsg->output.ratio      );
    gtk_spin_button_set_value ( gtk3red->framerateEntry   , rsg->output.fps        );
    gtk_spin_button_set_value ( gtk3red->renderWidthEntry , rsg->output.width      );
    gtk_spin_button_set_value ( gtk3red->renderHeightEntry, rsg->output.height     );

    gui->lock = 0x00;
}

/******************************************************************************/
static void createGeneralPanel ( GTK3G3DUIRENDEREDIT *gtk3red,
                                 gint                 x,
                                 gint                 y,
                                 gint                 width,
                                 gint                 height ) {
    gtk3red->generalTab = ui_createTab ( gtk3red->notebook,
                                         gtk3red,
                                         EDITRENDERGENERAL,
                                         CLASS_MAIN,
                                         x,
                                         y,
                                         width,
                                         height );

    gtk3red->renderPreviewToggle = ui_createToggleLabel ( gtk3red->generalTab,
                                                          gtk3red,
                                                          EDITRENDERPREVIEW,
                                                          CLASS_MAIN,
                                                          0,  0, 120, 20, 20,
                                                          previewCbk );

    gtk3red->fromFrameEntry      = ui_createIntegerText ( gtk3red->generalTab, 
                                                          gtk3red, 
                                                          EDITRENDERSTART,
                                                          CLASS_MAIN,
                                                          INT_MIN,
                                                          INT_MAX,
                                                          0,  24, 120, 96, 20,
                                                          startFrameCbk );

    gtk3red->toFrameEntry        = ui_createIntegerText ( gtk3red->generalTab,
                                                          gtk3red,
                                                          EDITRENDEREND,
                                                          CLASS_MAIN,
                                                          INT_MIN,
                                                          INT_MAX,
                                                          0,  48, 120, 96, 20,
                                                          endFrameCbk );

    gtk3red->framerateEntry      = ui_createIntegerText ( gtk3red->generalTab,
                                                          gtk3red,
                                                          EDITRENDERFPS,
                                                          CLASS_MAIN,
                                                          0,
                                                          INT_MAX,
                                                          0,  72, 120, 96, 20,
                                                          fpsCbk );

    gtk3red->renderWidthEntry    = ui_createIntegerText ( gtk3red->generalTab,
                                                          gtk3red,
                                                          EDITRENDERWIDTH,
                                                          CLASS_MAIN,
                                                          0,
                                                          INT_MAX,
                                                          0,  96, 120, 96, 20,
                                                          widthCbk );

    gtk3red->renderHeightEntry   = ui_createIntegerText ( gtk3red->generalTab,
                                                          gtk3red,
                                                          EDITRENDERHEIGHT,
                                                          CLASS_MAIN,
                                                          0,
                                                          INT_MAX,
                                                          0, 120, 120, 96, 20,
                                                          heightCbk );

    gtk3red->renderRatioEntry    = ui_createFloatText   ( gtk3red->generalTab,
                                                          gtk3red,
                                                          EDITRENDERRATIO,
                                                          CLASS_MAIN,
                                                          0.0f,
                                                          FLT_MAX,
                                                          0, 144, 120, 96, 20,
                                                          ratioCbk );


    createBackgroundFrame ( gtk3red, 0, 176, WINWIDTH, 48 );
    createSaveOutputFrame ( gtk3red, 0, 248, WINWIDTH, 112 );
    createAliasingFrame   ( gtk3red, 0, 364, WINWIDTH, 96 );
}

/******************************************************************************/
void updateEffectsPanel ( GTK3G3DUIRENDEREDIT *gtk3red ) {
    updateMotionBlurFrame ( gtk3red );
    updateWireframeFrame  ( gtk3red );
    updateFogFrame        ( gtk3red );
    updateTexturingFrame  ( gtk3red );
}

/******************************************************************************/
static void createEffectsPanel ( GTK3G3DUIRENDEREDIT *gtk3red,
                                 gint                 x,
                                 gint                 y,
                                 gint                 width,
                                 gint                 height ) {
    gtk3red->effectsTab = ui_createTab ( gtk3red->notebook,
                                         gtk3red,
                                         EDITRENDEREFFECTS,
                                         CLASS_MAIN,
                                         x,
                                         y,
                                         width,
                                         height );

    createMotionBlurFrame ( gtk3red, 0,   0, WINWIDTH,  112 );
    createWireframeFrame  ( gtk3red, 0, 120, WINWIDTH,  96 );
    createFogFrame        ( gtk3red, 0, 240, WINWIDTH,  96 );
    createTexturingFrame  ( gtk3red, 0, 376, WINWIDTH,  96 );
}

/******************************************************************************/
static void updateRenderEdit ( GTK3G3DUIRENDEREDIT *gtk3red ) {
    updateEffectsPanel ( gtk3red );
    updateGeneralPanel ( gtk3red );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;

    free ( gtk3red );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIRENDEREDIT *gtk3red = ( GTK3G3DUIRENDEREDIT * ) user_data;

    updateRenderEdit ( gtk3red );
}

/******************************************************************************/
GTK3G3DUIRENDEREDIT* gtk3_g3duirenderedit_create ( GtkWidget   *parent, 
                                                   GTK3G3DUI   *gtk3gui,
                                                   char        *name,
                                                   Q3DSETTINGS *rsg ) {

    GTK3G3DUIRENDEREDIT *gtk3red = gtk3_g3duirenderedit_new ( gtk3gui, rsg );
    GtkFixed *fixed = ui_gtk_fixed_new ( CLASS_MAIN );
    GtkNotebook *notebook = ui_gtk_notebook_new ( CLASS_MAIN );

    gtk3red->fixed = fixed;
    gtk3red->notebook = notebook;

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(notebook), TRUE );

    gtk_fixed_put ( fixed, notebook, 0, 0 );

    g_signal_connect ( G_OBJECT (notebook), "realize", G_CALLBACK (Realize), gtk3red );
    g_signal_connect ( G_OBJECT (notebook), "destroy", G_CALLBACK (Destroy), gtk3red );

    /*ui_createImageButton ( fixed,
                           gtk3red,
                           EDITRENDERRUN,
                           CLASS_MAIN,
                           prevframe_xpm,
                           0,  0, 24, 24,
                           runRenderCbk );*/

    createGeneralPanel ( gtk3red, 0, 24, WINWIDTH, 360 );
    createEffectsPanel ( gtk3red, 0, 24, WINWIDTH, 360 );

    gtk_widget_show ( notebook );
    gtk_widget_show ( fixed );

    return gtk3red;
}
