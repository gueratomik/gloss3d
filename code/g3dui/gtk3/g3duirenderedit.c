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
#define EDITRENDERMOTIONBLURFRAME  "Motion Blur Settings"
#define EDITRENDERVECTORMOTIONBLUR "Vector Motion Blur"
#define EDITRENDERVECTORMOTIONBLURSAMPLES "Samples"
#define EDITRENDERVECTORMOTIONBLURSUBSAMPLINGRATE "Sub-Sampling %"
#define EDITRENDERMOTIONBLURSTRENGTH "Strength"
#define EDITRENDERSCENEMOTIONBLUR  "Scene Motion Blur"
#define EDITRENDERSCENEMOTIONBLURITERATION "Iterations:"
#define EDITRENDERENABLEMOTIONBLUR "Enable Motion Blur"
#define EDITRENDERBACKGROUNDFRAME        "Background settings"
#define EDITRENDERBACKGROUNDCOLOR        "Background Color"
#define EDITRENDERBACKGROUNDIMAGE        "Background Image"
#define EDITRENDERBACKGROUNDSTRETCHIMAGE "Stretch Image"

#define EDITRENDERPREVIEW          "Make preview"
#define EDITRENDERRUN              "Run render"
#define EDITRENDERSAVEOUTPUTFRAME  "Output"
#define EDITRENDERSAVE             "Save result"


#define EDITRENDERALIASINGFRAME      "Anti-Aliasing"
#define EDITRENDERALIASING           "Enable Anti-Aliasing"
#define EDITRENDERALIASINGMODE       "Mode"
#define EDITRENDERALIASINGEDGE       "Edge"
#define EDITRENDERALIASINGFULL       "Full"
#define EDITRENDERALIASINGSAMPLES_5  "5 samples"
#define EDITRENDERALIASINGSAMPLES_9  "9 samples"

#define EDITRENDERWIREFRAMEFRAME     "Wireframe Settings"
#define EDITRENDERWIREFRAME          "Wireframe"
#define EDITRENDERWIREFRAMELIGHTING  "Affected by Lighting"
#define EDITRENDERWIREFRAMETHICKNESS "Thickness"
#define EDITRENDERWIREFRAMECOLOR     "Color"

#define EDITRENDERFOGFRAME             "Fog Settings"
#define EDITRENDERFOGAFFECTSBACKGROUND "Affects background"
#define EDITRENDERFOG                  "Enable Fog"
#define EDITRENDERFOGNEAR              "Near distance"
#define EDITRENDERFOGFAR               "Far distance"
#define EDITRENDERFOGCOLOR             "Color"
#define EDITRENDERFOGSTRENGTH          "Strength"

#define EDITRENDERTEXTURINGFRAME       "Texturing"
#define EDITRENDERTEXTURINGCOLOR       "Mesh default color"
#define EDITRENDERTEXTURING            "Disable texturing"

/******************************************************************************/
typedef struct _G3DUIRENDEREDIT {
    G3DUIWIDGETGROUP grp;

    GtkWidget       *main;

    GtkWidget       *renderPreviewToggle;
    GtkWidget       *fromFrameEntry;
    GtkWidget       *toFrameEntry;
    GtkWidget       *framerateEntry;


    GtkWidget       *renderWidthEntry;
    GtkWidget       *renderHeightEntry;
    GtkWidget       *renderRatioEntry;

    GtkWidget       *blurEnabledToggle;
    GtkWidget       *blurVectorEnabledRadio;
    GtkWidget       *blurSceneEnabledRadio;
    GtkWidget       *blurSamplesEntry;
    GtkWidget       *blurSubSamplingEntry;
    GtkWidget       *blurStrengthEntry;
    GtkWidget       *blurIterationsEntry;

    GtkWidget       *backgroundColorRadio;
    GtkWidget       *backgroundColorButton;
    GtkWidget       *backgroundImageRadio;
    GtkWidget       *backgroundImageButton;

    GtkWidget       *outputEnabledToggle;
    GtkWidget       *outputCodecButton;
    GtkWidget       *outputFileEntry;
    GtkWidget       *outputFormatSelector;

    GtkWidget       *wireframeEnabledToggle;
    GtkWidget       *wireframeAffectedToggle;
    GtkWidget       *wireframeThicknessEntry;
    GtkWidget       *wireframeColorButton;

    GtkWidget       *fogAffectsBackgroundToggle;
    GtkWidget       *fogEnabledToggle;
    GtkWidget       *fogNearEntry;
    GtkWidget       *fogFarEntry;
    GtkWidget       *fogColorButton;
    GtkWidget       *fogStrengthEntry;

    GtkWidget       *texturingDisabledToggle;
    GtkWidget       *texturingColorButton;

    GtkWidget       *aliasingEnabledToggle;
    GtkWidget       *aliasingEdgeRadio;
    GtkWidget       *aliasingFullRadio;
    GtkWidget       *aliasingS5Radio;
    GtkWidget       *aliasingS9Radio;

    Q3DSETTINGS     *editedRsg; 
} G3DUIRENDEREDIT;

/******************************************************************************/
static G3DUIRENDEREDIT *g3duirenderedit_new ( G3DUI *gui ) {
    G3DUIRENDEREDIT *red = calloc ( 0x01, sizeof ( G3DUIRENDEREDIT ) );

    if ( red == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );
    }

    red->grp.gui = gui;

    return red; 
}

static void updateGeneralPanel    ( G3DUIRENDEREDIT *red );
static void updateSaveOutputFrame ( G3DUIRENDEREDIT *red );

/******************************************************************************/
static void formatCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    GtkComboBoxText *cmbt = GTK_COMBO_BOX_TEXT(widget);
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
    const char *str = gtk_combo_box_text_get_active_text ( cmbt );

    common_g3duirenderedit_formatCbk ( gui, str );
    
    updateSaveOutputFrame ( red );
}

/******************************************************************************/
GtkWidget *createRenderFormat ( GtkWidget       *parent, 
                                G3DUIRENDEREDIT *red, 
                                char            *name,
                                gint             x, 
                                gint             y,
                                gint             labwidth,
                                gint             txtwidth,
                                void             (*cbk)( GtkWidget *, 
                                                         gpointer ) ) {
    GtkWidget     *cmb  = gtk_combo_box_text_new ( );
    GdkRectangle   crec = { 0x00, 0x00, txtwidth, 0x12 };
    G3DUI *gui = red->grp.gui;
    Q3DSETTINGS *rsg = ( red->editedRsg ) ? red->editedRsg : gui->currsg;

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
        g_signal_connect ( cmb, "changed", G_CALLBACK(cbk), red );
    }

    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, RENDERTOIMAGENAME );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, RENDERTOVIDEONAME );

    gtk_combo_box_set_active ( GTK_COMBO_BOX(cmb), rsg->output.format );

    gtk_widget_show ( cmb );


    return cmb;
}

/******************************************************************************/
/*** This filter is declared in the g3dui layer because of GtkWidget struct***/
Q3DFILTER *q3dfilter_toGtkWidget_new ( GtkWidget *widget, uint32_t active_fill ) {
    GdkDisplay *gdkdpy   = gtk_widget_get_display ( widget );
    GdkWindow  *gdkwin   = gtk_widget_get_window  ( widget );
    Q3DFILTER *fil;
    uint32_t filterMode =  FILTERLINE | FILTERIMAGE;
#ifdef __linux__
/*
    Display    *dis      = gdk_x11_display_get_xdisplay ( gdkdpy );
    Window      win      = gdk_x11_window_get_xid ( gdkwin );


    fil = q3dfilter_new ( filterMode, TOWINDOWFILTERNAME,
                                      filtertowindow_draw,
                                      filtertowindow_free, 
                                      filtertowindow_new ( dis, win, active_fill ) );
*/
#endif

#ifdef __MINGW32__
    HWND hWnd = GDK_WINDOW_HWND ( gdkwin );

    fil = q3dfilter_new ( filterMode, TOWINDOWFILTERNAME,
                                      filtertowindow_draw,
                                      filtertowindow_free, 
                                      filtertowindow_new ( hWnd, active_fill ) );
#endif

    return fil;
}

/******************************************************************************/
static void saveCbk ( GtkWidget *widget, gpointer user_data ) {
    uint32_t save = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget));
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_saveCbk ( gui, save );

    updateSaveOutputFrame ( red );
}


/******************************************************************************/
static void previewCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;

    common_g3duirenderedit_previewCbk ( gui );
}

/******************************************************************************/
static void startFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
    float frame = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_startFrameCbk ( gui, frame );

    updateGeneralPanel ( red );
}

/******************************************************************************/
static void fpsCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
    float fps = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    common_g3duirenderedit_fpsCbk ( gui, fps );
}

/******************************************************************************/
static void endFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
    float frame = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_endFrameCbk ( gui, frame );

    updateGeneralPanel ( red );
}

/******************************************************************************/
static void ratioCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
    float ratio = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_ratioCbk ( gui, ratio );

    updateGeneralPanel ( red );
}

/******************************************************************************/
static void widthCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
    int width = (int) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_widthCbk ( gui, ( uint32_t ) width );

    updateGeneralPanel ( red );
}

/******************************************************************************/
static void heightCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
    int height = (int) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_heightCbk ( gui, ( uint32_t ) height );

    updateGeneralPanel ( red );
}

/******************************************************************************/
static void outputCbk ( GtkWidget *widget, GdkEvent *event, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
    const char *outfile = gtk_entry_get_text ( GTK_ENTRY(widget) );

    common_g3duirenderedit_outputCbk ( gui, outfile );
}

/******************************************************************************/
static void backgroundCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
    GdkRGBA color;

    gtk_color_chooser_get_rgba ( GTK_COLOR_CHOOSER(widget), &color );

    common_g3duirenderedit_backgroundCbk ( gui, ( color.red   * 255 ), 
                                                ( color.green * 255 ),
                                                ( color.blue  * 255 ) );
}

/******************************************************************************/
static void wireframeColorCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
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
    Q3DSETTINGS *rsg = gui->currsg;
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
static void setMotionBlurCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;

    common_g3duirenderedit_setMotionBlurCbk ( gui );

    updateMotionBlurFrame ( red );
}

/******************************************************************************/
static void sceneMotionBlurCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;

    common_g3duirenderedit_sceneMotionBlurCbk ( gui );

    updateMotionBlurFrame ( red );
}

/******************************************************************************/
static void sceneMotionBlurIterationCbk ( GtkWidget *widget, gpointer user_data ) {
    uint32_t iteration = gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;

    common_g3duirenderedit_sceneMotionBlurIterationCbk ( gui, iteration );
}

/******************************************************************************/
static void vectorMotionBlurCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;

    common_g3duirenderedit_vectorMotionBlurCbk ( gui );

    updateMotionBlurFrame ( red );
}

/******************************************************************************/
static void motionBlurStrengthCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
    float strength = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_motionBlurStrengthCbk ( gui, strength );
}

/******************************************************************************/
static void vectorMotionBlurSamplesCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
    uint32_t samples = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_vectorMotionBlurSamplesCbk ( gui, samples );
}

/******************************************************************************/
static void vectorMotionBlurSubSamplingRateCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
    float rate = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_vectorMotionBlurSubSamplingRateCbk ( gui, rate / 100.0f );
}

/******************************************************************************/
static void chooseCodecCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
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
static void updateSaveOutputFrame ( G3DUIRENDEREDIT *red ) {
    G3DUI *gui = red->grp.gui;
    Q3DSETTINGS *rsg = ( red->editedRsg ) ? red->editedRsg : gui->currsg;

    gui->lock = 0x01;

    if ( rsg->flags & RENDERSAVE ) {
        uint32_t tovid = ( rsg->output.format == RENDERTOVIDEO );

        gtk_toggle_button_set_active ( red->outputEnabledToggle, TRUE  );

        gtk_widget_set_sensitive ( red->outputCodecButton, TRUE );
        gtk_widget_set_sensitive ( red->outputCodecButton, TRUE );
        gtk_widget_set_sensitive ( red->outputCodecButton, TRUE );

#ifdef __MINGW32__
        gtk_widget_set_sensitive ( red->outputCodecButton, tovid );
#endif
    } else {
        gtk_toggle_button_set_active ( red->outputEnabledToggle, FALSE );

        gtk_widget_set_sensitive ( red->outputCodecButton, FALSE );
        gtk_widget_set_sensitive ( red->outputCodecButton, FALSE );
        gtk_widget_set_sensitive ( red->outputCodecButton, FALSE );

#ifdef __MINGW32__
        gtk_widget_set_sensitive ( red->outputCodecButton, FALSE );
#endif
    }

    gtk_entry_set_text ( red->outputFileEntry, rsg->output.outfile );
    gtk_combo_box_set_active ( red->outputFormatSelector, rsg->output.format );

    gui->lock = 0x00;
}

/******************************************************************************/
static GtkWidget *createSaveOutputForm ( GtkWidget       *parent, 
                                         G3DUIRENDEREDIT *red,
                                         char            *name,
                                         gint             x,
                                         gint             y,
                                         gint             width,
                                         gint             height ) {
    G3DUI *gui = red->grp.gui;
    GtkWidget *frm;

    frm = createFrame ( parent, red, name, x, y, width, height );

    gui->lock = 0x01;

    red->outputEnabledToggle = createToggleLabel ( frm,
                                                   red,
                                                   EDITRENDERSAVE,
                                                   0,  0, 104, 20, saveCbk );

    red->outputFormatSelector = createRenderFormat( frm,
                                                    red,
                                                    EDITRENDERFORMAT,
                                                    0, 24, 96,  64, formatCbk );

#ifdef __MINGW32__
    red->outputCodecButton = createPushButton ( frm,
                                                red,
                                                EDITRENDERCODEC,
                                                96, 48,
                                                96, 18, chooseCodecCbk );
#endif
    red->outputFileEntry = createCharText ( frm, 
                                            red,
                                            EDITRENDEROUTPUT,
                                            0, 72, 96, 200, outputCbk );

    gui->lock = 0x00;

    return frm;
}

/******************************************************************************/
static void updateAliasingFrame ( G3DUIRENDEREDIT *red ) {
    G3DUI *gui = red->grp.gui;
    Q3DSETTINGS *rsg = ( red->editedRsg ) ? red->editedRsg : gui->currsg;

    gui->lock = 0x01;

    GtkWidget       *aliasingEnabledToggle;
    GtkWidget       *aliasingEdgeRadio;
    GtkWidget       *aliasingFullRadio;
    GtkWidget       *aliasingS5Radio;
    GtkWidget       *aliasingS9Radio;

    if ( rsg->flags & ENABLEAA ) {
        gtk_toggle_button_set_active ( red->aliasingEnabledToggle, TRUE  );

        gtk_widget_set_sensitive ( red->aliasingEdgeRadio, TRUE );
        gtk_widget_set_sensitive ( red->aliasingFullRadio, TRUE );
        gtk_widget_set_sensitive ( red->aliasingS5Radio  , TRUE );
        gtk_widget_set_sensitive ( red->aliasingS9Radio  , TRUE );
    } else {
        gtk_toggle_button_set_active ( red->aliasingEnabledToggle, FALSE );

        gtk_widget_set_sensitive ( red->aliasingEdgeRadio, FALSE );
        gtk_widget_set_sensitive ( red->aliasingFullRadio, FALSE );
        gtk_widget_set_sensitive ( red->aliasingS5Radio  , FALSE );
        gtk_widget_set_sensitive ( red->aliasingS9Radio  , FALSE );
    }

    gtk_toggle_button_set_active ( red->aliasingEdgeRadio, ( rsg->aa.mode == AAEDGEMODE ) );
    gtk_toggle_button_set_active ( red->aliasingFullRadio, ( rsg->aa.mode == AAFULLMODE ) );
    gtk_toggle_button_set_active ( red->aliasingS5Radio  , ( rsg->aa.nbsamples == 0x05 ) );
    gtk_toggle_button_set_active ( red->aliasingS9Radio  , ( rsg->aa.nbsamples == 0x09 ) );

    gui->lock = 0x00;
}

/******************************************************************************/
static void aliasingCbk ( GtkWidget *widget, gpointer user_data ) {
    uint32_t aa = gtk_toggle_button_get_active (GTK_TOGGLE_BUTTON(widget));
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;

    common_g3duirenderedit_aaCbk ( gui );

    updateAliasingFrame ( red );
}

/******************************************************************************/
static void aliasingEdgeCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;

    common_g3duirenderedit_aaEdgeCbk ( gui );
}

/******************************************************************************/
static void aliasingFullCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;

    common_g3duirenderedit_aaFullCbk ( gui );
}

/******************************************************************************/
static void aliasingS5Cbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;

    common_g3duirenderedit_aaSamplesCbk ( gui, 5 );
}

/******************************************************************************/
static void aliasingS9Cbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;

    common_g3duirenderedit_aaSamplesCbk ( gui, 9 );
}

/******************************************************************************/
static GtkWidget *createAliasingForm ( GtkWidget       *parent, 
                                       G3DUIRENDEREDIT *red,
                                       char            *name,
                                       gint             x,
                                       gint             y,
                                       gint             width,
                                       gint             height ) {
    G3DUI *gui = red->grp.gui;
    GtkWidget *frm;

    frm = createFrame ( parent, red, name, x, y, width, height );

    gui->lock = 0x01;

    red->aliasingEnabledToggle = createToggleLabel ( frm,
                                                     red,
                                                     EDITRENDERALIASING,
                                                     0,  0, 104, 20, 
                                                     aliasingCbk );

    red->aliasingEdgeRadio = createRadioLabel ( frm,
                                                red,
                                                EDITRENDERALIASINGEDGE,
                                                NULL,
                                                0, 24, 96,  24, 
                                                aliasingEdgeCbk );

    red->aliasingFullRadio = createRadioLabel ( frm,
                                                red,
                                                EDITRENDERALIASINGFULL,
                                                red->aliasingEdgeRadio,
                                                0, 48, 96,  24, 
                                                aliasingFullCbk );

    red->aliasingS5Radio = createRadioLabel ( frm,
                                              red,
                                              EDITRENDERALIASINGSAMPLES_5,
                                              NULL,
                                              60, 24, 96,  24, 
                                              aliasingS5Cbk );

    red->aliasingS9Radio = createRadioLabel ( frm,
                                              red,
                                              EDITRENDERALIASINGSAMPLES_9,
                                              red->aliasingS5Radio,
                                              60, 48, 96,  24,
                                              aliasingS9Cbk );


    gui->lock = 0x00;

    return frm;
}

/******************************************************************************/
void updateMotionBlurFrame ( G3DUIRENDEREDIT *red ) {
    G3DUI *gui = red->grp.gui;
    Q3DSETTINGS *rsg = ( red->editedRsg ) ? red->editedRsg : gui->currsg;

    gui->lock = 0x01;

    if ( rsg->flags & ENABLEMOTIONBLUR ) {
        uint32_t vblur = ( rsg->flags & VECTORMOTIONBLUR );
        uint32_t sblur = ( rsg->flags & SCENEMOTIONBLUR  );

        gtk_toggle_button_set_active ( red->blurEnabledToggle, TRUE  );

        gtk_widget_set_sensitive ( red->blurVectorEnabledRadio, TRUE );
        gtk_widget_set_sensitive ( red->blurSceneEnabledRadio , TRUE );
        gtk_widget_set_sensitive ( red->blurStrengthEntry     , TRUE );

        gtk_widget_set_sensitive ( red->blurSamplesEntry    , vblur );
        gtk_widget_set_sensitive ( red->blurSubSamplingEntry, vblur );

        gtk_widget_set_sensitive ( red->blurIterationsEntry, sblur );
    } else {
        gtk_toggle_button_set_active ( red->blurEnabledToggle, FALSE );

        gtk_widget_set_sensitive ( red->blurVectorEnabledRadio, FALSE );
        gtk_widget_set_sensitive ( red->blurSceneEnabledRadio , FALSE );
        gtk_widget_set_sensitive ( red->blurStrengthEntry     , FALSE );

        gtk_widget_set_sensitive ( red->blurSamplesEntry    , FALSE );
        gtk_widget_set_sensitive ( red->blurSubSamplingEntry, FALSE );

        gtk_widget_set_sensitive ( red->blurIterationsEntry, FALSE );
    }

    if ( rsg->flags & VECTORMOTIONBLUR ) {
        gtk_toggle_button_set_active ( red->blurVectorEnabledRadio, TRUE  );
    } else {
        gtk_toggle_button_set_active ( red->blurVectorEnabledRadio, FALSE );
    }

    if ( rsg->flags & SCENEMOTIONBLUR ) {
        gtk_toggle_button_set_active ( red->blurSceneEnabledRadio, TRUE  );
    } else {
        gtk_toggle_button_set_active ( red->blurSceneEnabledRadio, FALSE );
    }

    gtk_spin_button_set_value ( red->blurIterationsEntry,
                                rsg->motionBlur.iterations );

    gtk_spin_button_set_value ( red->blurStrengthEntry,
                                rsg->motionBlur.strength * 100.0f );

    gtk_spin_button_set_value ( red->blurSamplesEntry, 
                                rsg->motionBlur.vMotionBlurSamples );

    gtk_spin_button_set_value ( red->blurSubSamplingEntry, 
                                rsg->motionBlur.vMotionBlurSubSamplingRate * 100 );

    gui->lock = 0x00;
}

/******************************************************************************/
static GtkWidget *createMotionBlurForm ( GtkWidget       *parent,
                                         G3DUIRENDEREDIT *red,
                                         char            *name,
                                         gint             x,
                                         gint             y,
                                         gint             width,
                                         gint             height ) {
    G3DUI *gui = red->grp.gui;
    GtkWidget *frm;

    gui->lock = 0x01;

    frm = createFrame ( parent, red, name, x, y, width, height );

    red->blurEnabledToggle = createToggleLabel ( frm,
                                                 red,
                                                 EDITRENDERENABLEMOTIONBLUR,
                                                 0,  0, 96, 18,
                                                 setMotionBlurCbk );

    red->blurStrengthEntry = createIntegerText ( frm,
                                                 red,
                                                 EDITRENDERMOTIONBLURSTRENGTH,
                                                 0, 100,
                                                 160, 0, 96,  32,
                                                 motionBlurStrengthCbk );

    red->blurVectorEnabledRadio = createRadioLabel ( frm,
                                                     red,
                                                     EDITRENDERVECTORMOTIONBLUR,
                                                     NULL,
                                                     0,  24, 96, 18,
                                                     vectorMotionBlurCbk );

    red->blurSamplesEntry = createIntegerText ( frm,
                                                red,
                                                EDITRENDERVECTORMOTIONBLURSAMPLES,
                                                0, 100,
                                                160, 24, 96,  32,
                                                vectorMotionBlurSamplesCbk );

    red->blurSubSamplingEntry = createIntegerText ( frm,
                                                    red,
                                                    EDITRENDERVECTORMOTIONBLURSUBSAMPLINGRATE,
                                                    0, 100,
                                                    160, 48, 96,  32,
                                                    vectorMotionBlurSubSamplingRateCbk );

    red->blurSceneEnabledRadio = createRadioLabel ( frm,
                                                    red,
                                                    EDITRENDERSCENEMOTIONBLUR,
                                                    red->blurVectorEnabledRadio,
                                                    0, 72, 96, 18,
                                                    sceneMotionBlurCbk );

    red->blurIterationsEntry = createIntegerText ( frm,
                                                   red,
                                                   EDITRENDERSCENEMOTIONBLURITERATION,
                                                   2, 31,
                                                   160, 72, 96,  32,
                                                   sceneMotionBlurIterationCbk );


    gui->lock = 0x00;

    return frm;
}

/******************************************************************************/
static void updateWireframeFrame ( G3DUIRENDEREDIT *red ) {
    G3DUI *gui = red->grp.gui;
    Q3DSETTINGS *rsg = ( red->editedRsg ) ? red->editedRsg : gui->currsg;
    unsigned char R = ( rsg->wireframe.color & 0x00FF0000 ) >> 0x10,
                  G = ( rsg->wireframe.color & 0x0000FF00 ) >> 0x08,
                  B = ( rsg->wireframe.color & 0x000000FF );
    GdkRGBA rgba = { .red   = ( float ) R / 255,
                     .green = ( float ) G / 255,
                     .blue  = ( float ) B / 255,
                     .alpha = 1.0f };


    gui->lock = 0x01;

    if ( ( rsg->flags & RENDERWIREFRAME ) ) {
        gtk_toggle_button_set_active ( red->wireframeEnabledToggle, TRUE  );

        gtk_widget_set_sensitive ( red->wireframeAffectedToggle, TRUE );
        gtk_widget_set_sensitive ( red->wireframeThicknessEntry, TRUE );
        gtk_widget_set_sensitive ( red->wireframeColorButton   , TRUE );
    } else {
        gtk_toggle_button_set_active ( red->wireframeEnabledToggle, FALSE );

        gtk_widget_set_sensitive ( red->wireframeAffectedToggle, FALSE );
        gtk_widget_set_sensitive ( red->wireframeThicknessEntry, FALSE );
        gtk_widget_set_sensitive ( red->wireframeColorButton   , FALSE );
    }

    if ( rsg->flags & WIREFRAMELIGHTING ) {
        gtk_toggle_button_set_active ( red->wireframeAffectedToggle, TRUE  );
    } else {
        gtk_toggle_button_set_active ( red->wireframeAffectedToggle, FALSE );
    }

    gtk_spin_button_set_value ( red->wireframeThicknessEntry, rsg->wireframe.thickness );
    gtk_color_chooser_set_rgba ( red->wireframeColorButton, &rgba );

    gui->lock = 0x00;
}

/******************************************************************************/
static void setWireframeCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;

    common_g3duirenderedit_setWireframeCbk ( gui );

    updateWireframeFrame( red );
}

/******************************************************************************/
static void setWireframeLightingCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;

    common_g3duirenderedit_setWireframeLightingCbk ( gui );

    updateWireframeFrame ( red );
}

/******************************************************************************/
static void wireframeThicknessCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
    float thickness = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_wireframeThicknessCbk ( gui, thickness );
}

/******************************************************************************/
static GtkWidget *createWireframeForm ( GtkWidget       *parent,
                                        G3DUIRENDEREDIT *red,
                                        char            *name,
                                        gint             x,
                                        gint             y,
                                        gint             width,
                                        gint             height ) {
    G3DUI *gui = red->grp.gui;
    GtkWidget *frm;

    frm = createFrame ( parent, red, name, x, y, width, height );

    gui->lock = 0x01;

    red->wireframeEnabledToggle = createToggleLabel ( frm, 
                                                      red,
                                                      EDITRENDERWIREFRAME,
                                                      0,  0, 96, 18,
                                                      setWireframeCbk );

    red->wireframeAffectedToggle = createToggleLabel ( frm,
                                                       red,
                                                       EDITRENDERWIREFRAMELIGHTING,
                                                       0, 24, 96, 18,
                                                       setWireframeLightingCbk );

    red->wireframeThicknessEntry = createFloatText ( frm,
                                                     red,
                                                     EDITRENDERWIREFRAMETHICKNESS,
                                                     0.0f, FLT_MAX,
                                                     0, 48, 96,  48,
                                                     wireframeThicknessCbk );

    createSimpleLabel ( frm, 
                        red,
                        EDITRENDERWIREFRAMECOLOR,
                        0, 72, 96, 20 );

    red->wireframeColorButton = createColorButton ( frm,
                                                    red,
                                                    EDITRENDERWIREFRAMECOLOR,
                                                    96, 72, 96, 18, wireframeColorCbk );

    gui->lock = 0x00;

    return frm;
}

/******************************************************************************/
static void updateTexturingFrame ( G3DUIRENDEREDIT *red ) {
    G3DUI *gui = red->grp.gui;
    Q3DSETTINGS *rsg = ( red->editedRsg ) ? red->editedRsg : gui->currsg;
    GdkRGBA rgba = { .red   = ( float ) rsg->defaultColor.r / 255,
                     .green = ( float ) rsg->defaultColor.g / 255,
                     .blue  = ( float ) rsg->defaultColor.b / 255,
                     .alpha = 1.0f };


    gui->lock = 0x01;

    if ( rsg->flags & DISABLETEXTURING ) {
        gtk_toggle_button_set_active ( red->texturingDisabledToggle, TRUE  );
    } else {
        gtk_toggle_button_set_active ( red->texturingDisabledToggle, FALSE );
    }

    gtk_color_chooser_set_rgba ( red->texturingColorButton, &rgba );


    gui->lock = 0x00;
}

/******************************************************************************/
static void texturingColorCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
    GdkRGBA color;

    gtk_color_chooser_get_rgba ( GTK_COLOR_CHOOSER(widget), &color );

    common_g3duirenderedit_texturingColorCbk ( gui, ( color.red   * 255 ), 
                                                    ( color.green * 255 ),
                                                    ( color.blue  * 255 ) );
}

/******************************************************************************/
static void setTexturingCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;

    common_g3duirenderedit_setTexturingCbk ( gui );
}

/******************************************************************************/
static GtkWidget *createTexturingForm ( GtkWidget       *parent, 
                                        G3DUIRENDEREDIT *red,
                                        char            *name,
                                        gint             x,
                                        gint             y,
                                        gint             width,
                                        gint             height ) {
    G3DUI *gui = red->grp.gui;
    GtkWidget *frm;

    frm = createFrame ( parent, red, name, x, y, width, height );

    gui->lock = 0x01;

    red->texturingDisabledToggle = createToggleLabel ( frm,
                                                       red,
                                                       EDITRENDERTEXTURING,
                                                       0,  0, 96, 18,
                                                       setTexturingCbk );

          createSimpleLabel ( frm,
                              red,
                              EDITRENDERTEXTURINGCOLOR,
                              0, 24, 96, 20 );

    red->texturingColorButton = createColorButton ( frm, 
                                                    red,
                                                    EDITRENDERTEXTURINGCOLOR,
                                                    96, 24, 96, 18, 
                                                    texturingColorCbk );

    gui->lock = 0x00;

    return frm;
}


/******************************************************************************/
static void updateFogFrame ( G3DUIRENDEREDIT *red ) {
    G3DUI *gui = red->grp.gui;
    Q3DSETTINGS *rsg = ( red->editedRsg ) ? red->editedRsg : gui->currsg;
    unsigned char R = ( rsg->fog.color & 0x00FF0000 ) >> 0x10,
                  G = ( rsg->fog.color & 0x0000FF00 ) >> 0x08,
                  B = ( rsg->fog.color & 0x000000FF );
    GdkRGBA rgba = { .red   = ( float ) R / 255,
                     .green = ( float ) G / 255,
                     .blue  = ( float ) B / 255,
                     .alpha = 1.0f };


    gui->lock = 0x01;

    if ( rsg->flags & RENDERFOG ) {
        gtk_toggle_button_set_active ( red->fogEnabledToggle, TRUE  );

        gtk_widget_set_sensitive ( red->fogAffectsBackgroundToggle, TRUE );
        gtk_widget_set_sensitive ( red->fogStrengthEntry          , TRUE );
        gtk_widget_set_sensitive ( red->fogNearEntry              , TRUE );
        gtk_widget_set_sensitive ( red->fogFarEntry               , TRUE );
        gtk_widget_set_sensitive ( red->fogColorButton            , TRUE );
    } else {
        gtk_toggle_button_set_active ( red->fogEnabledToggle, FALSE );

        gtk_widget_set_sensitive ( red->fogAffectsBackgroundToggle, FALSE );
        gtk_widget_set_sensitive ( red->fogStrengthEntry          , FALSE );
        gtk_widget_set_sensitive ( red->fogNearEntry              , FALSE );
        gtk_widget_set_sensitive ( red->fogFarEntry               , FALSE );
        gtk_widget_set_sensitive ( red->fogColorButton            , FALSE );
    }

    if ( rsg->fog.flags & FOGAFFECTSBACKGROUND ) {
        gtk_toggle_button_set_active ( red->fogAffectsBackgroundToggle, TRUE  );
    } else {
        gtk_toggle_button_set_active ( red->fogAffectsBackgroundToggle, FALSE );
    }

    gtk_spin_button_set_value  ( red->fogStrengthEntry, rsg->fog.strength * 100.0f );
    gtk_spin_button_set_value  ( red->fogNearEntry, rsg->fog.fnear );
    gtk_spin_button_set_value  ( red->fogFarEntry, rsg->fog.ffar );
    gtk_color_chooser_set_rgba ( red->fogColorButton, &rgba );


    gui->lock = 0x00;
}

/******************************************************************************/
static void setFogAffectsBackgroundCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;

    common_g3duirenderedit_setFogAffectsBackgroundCbk ( gui );
}

/******************************************************************************/
static void setFogCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;

    common_g3duirenderedit_setFogCbk ( gui );

    updateFogFrame ( red );
}

/******************************************************************************/
static void fogStrengthCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
    float strength = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_setFogStrengthCbk ( gui, strength );
}

/******************************************************************************/
static void fogNearCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
    float fnear = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_fogNearCbk ( gui, fnear );
}

/******************************************************************************/
static void fogFarCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
    float ffar = (float) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );

    common_g3duirenderedit_fogFarCbk ( gui, ffar );
}

/******************************************************************************/
static void fogColorCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
    GdkRGBA color;

    gtk_color_chooser_get_rgba ( GTK_COLOR_CHOOSER(widget), &color );

    common_g3duirenderedit_fogColorCbk ( gui, ( color.red   * 255 ), 
                                              ( color.green * 255 ),
                                              ( color.blue  * 255 ) );
}

/******************************************************************************/
static GtkWidget *createFogForm ( GtkWidget       *parent, 
                                  G3DUIRENDEREDIT *red,
                                  char            *name,
                                  gint             x,
                                  gint             y,
                                  gint             width,
                                  gint             height ) {
    G3DUI *gui = red->grp.gui;
    GtkWidget *frm;

    frm = createFrame ( parent, red, name, x, y, width, height );

    gui->lock = 0x01;

    red->fogEnabledToggle = createToggleLabel ( frm,
                                                red,
                                                EDITRENDERFOG,
                                                0,  0, 96, 18,
                                                setFogCbk );

    red->fogAffectsBackgroundToggle = createToggleLabel ( frm,
                                                          red,
                                                          EDITRENDERFOGAFFECTSBACKGROUND,
                                                          0, 24, 96, 18,
                                                          setFogAffectsBackgroundCbk );

    red->fogStrengthEntry = createFloatText ( frm,
                                              red,
                                              EDITRENDERFOGSTRENGTH,
                                              0.0f, 100.0f,
                                              0, 48, 96, 48,
                                              fogStrengthCbk );

    red->fogNearEntry = createFloatText ( frm,
                                          red,
                                          EDITRENDERFOGNEAR,
                                          0.0f,
                                          FLT_MAX,
                                          0, 72, 96, 48,
                                          fogNearCbk );

    red->fogFarEntry = createFloatText ( frm,
                                         red,
                                         EDITRENDERFOGFAR,
                                         0.0f,
                                         FLT_MAX,
                                         0, 96, 96, 48,
                                         fogFarCbk );

          createSimpleLabel ( frm,
                              red,
                              EDITRENDERFOGCOLOR,
                              0, 120, 96, 20 );

    red->fogColorButton = createColorButton ( frm, 
                                              red,
                                              EDITRENDERFOGCOLOR,
                                              96, 120, 96, 18, 
                                              fogColorCbk );

    gui->lock = 0x00;

    return frm;
}

/******************************************************************************/
static void updateBackgroundFrame ( G3DUIRENDEREDIT *red ) {
    G3DUI *gui = red->grp.gui;
    Q3DSETTINGS *rsg = ( red->editedRsg ) ? red->editedRsg : gui->currsg;
    unsigned char R = ( rsg->background.color & 0x00FF0000 ) >> 0x10,
                  G = ( rsg->background.color & 0x0000FF00 ) >> 0x08,
                  B = ( rsg->background.color & 0x000000FF );
    GdkRGBA rgba = { .red   = ( float ) R / 255,
                     .green = ( float ) G / 255,
                     .blue  = ( float ) B / 255,
                     .alpha = 1.0f };


    gui->lock = 0x01;

    if ( ( rsg->background.mode & BACKGROUND_IMAGE ) ) {
        gtk_toggle_button_set_active ( red->backgroundImageRadio, TRUE );
        gtk_toggle_button_set_active ( red->backgroundColorRadio, FALSE  );
    } else {
        gtk_toggle_button_set_active ( red->backgroundImageRadio, FALSE );
        gtk_toggle_button_set_active ( red->backgroundColorRadio, TRUE );
    }

    if ( rsg->background.image && 
         rsg->background.image->filename ) {
        char *imgpath, *imgname;

        imgpath = strdup ( rsg->background.image->filename );

        /*** We just keep the image name, not the whole ***/
        /*** path and display it as the button label.   ***/
        imgname = basename ( imgpath );

        gtk_button_set_label ( red->backgroundImageButton, imgname );

        free ( imgpath );
    }

    gtk_color_chooser_set_rgba ( red->backgroundColorButton, &rgba );

    gui->lock = 0x00;
}

/******************************************************************************/
static void setBackgroundImageCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;
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

    updateBackgroundFrame ( red );

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
static void setBackgroundColorModeCbk ( GtkWidget *widget, 
                                        gpointer   user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;

    common_g3duirenderedit_setBackgroundColorModeCbk ( gui );
}

/******************************************************************************/
static void setBackgroundImageModeCbk ( GtkWidget *widget, 
                                        gpointer   user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;
    G3DUI *gui = red->grp.gui;

    common_g3duirenderedit_setBackgroundImageModeCbk ( gui );
}

/******************************************************************************/
static GtkWidget *createBackgroundForm ( GtkWidget       *parent, 
                                         G3DUIRENDEREDIT *red,
                                         char            *name,
                                         gint             x,
                                         gint             y,
                                         gint             width,
                                         gint             height ) {
    G3DUI *gui = red->grp.gui;
    GtkWidget *frm;

    frm = createFrame ( parent, red, name, x, y, width, height );

    gui->lock = 0x01;

    red->backgroundColorRadio = createRadioLabel ( frm, 
                                                   red,
                                                   EDITRENDERBACKGROUNDCOLOR,
                                                   NULL,
                                                   0,  0, 96, 18,
                                                   setBackgroundColorModeCbk );

    red->backgroundImageRadio = createRadioLabel ( frm,
                                                   red,
                                                   EDITRENDERBACKGROUNDIMAGE,
                                                   red->backgroundColorRadio,
                                                   0, 24, 96, 18,
                                                   setBackgroundImageModeCbk );

    red->backgroundColorButton = createColorButton ( frm,
                                                     red,
                                                     EDITRENDERBACKGROUNDCOLOR,
                                                     160, 0, 96, 18,
                                                     backgroundCbk );

    red->backgroundImageButton = createPushButton  ( frm,
                                                     red,
                                                     EDITRENDERBACKGROUNDIMAGE,
                                                     160, 24, 96,  18,
                                                     setBackgroundImageCbk );

    gui->lock = 0x00;

    return frm;
}

/******************************************************************************/
static void updateGeneralPanel ( G3DUIRENDEREDIT *red ) {
    G3DUI *gui = red->grp.gui;
    Q3DSETTINGS *rsg = ( red->editedRsg ) ? red->editedRsg : gui->currsg;

    updateSaveOutputFrame ( red );
    updateBackgroundFrame ( red );
    updateAliasingFrame   ( red );

    gui->lock = 0x01;

    if ( rsg->flags & RENDERPREVIEW ) {
        gtk_toggle_button_set_active ( red->renderPreviewToggle, TRUE  );
    } else {
        gtk_toggle_button_set_active ( red->renderPreviewToggle, FALSE );
    }

    gtk_spin_button_set_value ( red->fromFrameEntry   , rsg->output.startframe );
    gtk_spin_button_set_value ( red->toFrameEntry     , rsg->output.endframe   );
    gtk_spin_button_set_value ( red->renderRatioEntry , rsg->output.ratio      );
    gtk_spin_button_set_value ( red->framerateEntry   , rsg->output.fps        );
    gtk_spin_button_set_value ( red->renderWidthEntry , rsg->output.width      );
    gtk_spin_button_set_value ( red->renderHeightEntry, rsg->output.height     );

    gui->lock = 0x00;
}

/******************************************************************************/
void createGeneralPanel ( GtkWidget       *parent, 
                          G3DUIRENDEREDIT *red,
                          char            *name,
                          gint             x,
                          gint             y,
                          gint             width,
                          gint             height ) {
    G3DUI *gui = red->grp.gui;

    GtkWidget *pan = createPanel ( parent, red, name, x, y, width, height );

    gui->lock = 0x01;

    red->renderPreviewToggle = createToggleLabel ( pan,
                                                   red,
                                                   EDITRENDERPREVIEW,
                                                   0,  0, 104, 20, previewCbk );

    red->fromFrameEntry = createIntegerText   ( pan, 
                                                red, 
                                                EDITRENDERSTART,
                                                INT_MIN,
                                                INT_MAX,
                                                0,  24, 96,  32, startFrameCbk );

    red->toFrameEntry = createIntegerText     ( pan,
                                                red,
                                                EDITRENDEREND,
                                                INT_MIN,
                                                INT_MAX,
                                                0,  48, 96,  32, endFrameCbk );

    red->framerateEntry = createIntegerText   ( pan,
                                                red,
                                                EDITRENDERFPS,
                                                0,
                                                INT_MAX,
                                                0,  72, 96,  32, fpsCbk );

    red->renderWidthEntry = createIntegerText ( pan,
                                                red,
                                                EDITRENDERWIDTH,
                                                0,
                                                INT_MAX,
                                                0,  96, 96,  32, widthCbk );

    red->renderHeightEntry = createIntegerText ( pan,
                                                 red,
                                                 EDITRENDERHEIGHT,
                                                 0,
                                                 INT_MAX,
                                                 0, 120, 96,  32, heightCbk );

    red->renderRatioEntry = createFloatText   ( pan,
                                                red,
                                                EDITRENDERRATIO,
                                                0.0f,
                                                FLT_MAX,
                                                0, 144, 96,  64, ratioCbk );

    gui->lock = 0x00;

    createBackgroundForm ( pan,
                           red,
                           EDITRENDERBACKGROUNDFRAME,
                           0, 168, 256,  96 );

    createSaveOutputForm ( pan,
                           red,
                           EDITRENDERSAVEOUTPUTFRAME,
                           0, 264, 256,  96 );

    createAliasingForm ( pan,
                         red,
                         EDITRENDERALIASINGFRAME,
                         0, 364, 256,  96 );
}

/******************************************************************************/
void updateEffectsPanel ( G3DUIRENDEREDIT *red ) {
    updateMotionBlurFrame ( red );
    updateWireframeFrame  ( red );
    updateFogFrame        ( red );
    updateTexturingFrame  ( red );
}

/******************************************************************************/
void createEffectsPanel ( GtkWidget       *parent, 
                          G3DUIRENDEREDIT *red,
                          char            *name,
                          gint             x,
                          gint             y,
                          gint             width,
                          gint             height ) {
    GtkWidget *pan;

    pan = createPanel ( parent,
                        red,
                        name,
                        x,
                        y,
                        width,
                        height );

    createMotionBlurForm ( pan, 
                           red,
                           EDITRENDERMOTIONBLURFRAME,
                           0,   0, 256,  96 );

    createWireframeForm ( pan, 
                          red,
                          EDITRENDERWIREFRAMEFRAME,
                          0, 108, 256,  96 );

    createFogForm ( pan,
                    red,
                    EDITRENDERFOGFRAME,
                    0, 220, 256,  96 );

    createTexturingForm ( pan,
                          red,
                          EDITRENDERTEXTURINGFRAME,
                          0, 376, 256,  96 );
}

/******************************************************************************/
void updateRenderEdit ( GtkWidget *w, Q3DSETTINGS *rsg ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) g_object_get_data ( G_OBJECT(w),
                                                                     "private" );

    red->editedRsg = rsg;

    updateEffectsPanel ( red );
    updateGeneralPanel ( red );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    G3DUIRENDEREDIT *red = ( G3DUIRENDEREDIT * ) user_data;

    free ( red );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    updateRenderEdit ( widget, NULL );
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
    G3DUIRENDEREDIT *red = g3duirenderedit_new ( gui );

    gtk_widget_set_name ( frm, name );

    gtk_widget_set_size_request ( frm, frmrec.width, frmrec.height );

    /*** Because this is an independent window ***/
    gtk_container_add (GTK_CONTAINER (parent), frm);

    createPushButton  ( frm, gui, EDITRENDERRUN,
                         (width / 2 ) - 48, 4, 48, 20, g3dui_runRenderCbk );

    /********************/
    tab = gtk_notebook_new ( );

    g_object_set_data ( G_OBJECT(tab), "private", (gpointer) red );

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(tab), TRUE );

    gtk_widget_set_name ( tab, EDITRENDERSETTINGS );

    gtk_widget_size_allocate ( tab, &gdkrec );
    /*gtk_widget_set_size_request ( tab, width, height );*/

    gtk_fixed_put ( GTK_FIXED(frm), tab, gdkrec.x, gdkrec.y );

    g_signal_connect ( G_OBJECT (tab), "realize", G_CALLBACK (Realize), red );
    g_signal_connect ( G_OBJECT (tab), "destroy", G_CALLBACK (Destroy), red );

    createGeneralPanel ( tab, red, EDITRENDERGENERAL, 0, 0, width, height );
    createEffectsPanel ( tab, red, EDITRENDEREFFECTS, 0, 0, width, height );

    gtk_widget_show ( tab );


    gtk_widget_show ( frm );

    /*list_insert ( &gui->lmatedit, frm );*/

    return frm;
}
