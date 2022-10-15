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
#define NOISEPAIRCOLORSTR "%s%d%s%d"
#define NOISEPAIR         "Pair"
#define NOISECOLOR        "Color"
#define UGRADIENTS        "U Gradients"
#define VGRADIENTS        "V Gradients"
#define OCTAVES           "Octaves"
#define NBPAIRS           "Nb of pairs"
#define THRESHOLD         "Threshold"

#define NOISETHRESHOLDSTR "%s%d"
#define NOISETHRESHOLD    "Threshold "

/******************************************************************************/
static void updateProceduralNoisePair ( GTK3G3DUIPROCEDURALNOISEEDIT *gtk3ped,
                                        uint32_t                      pid ) {
    GdkRGBA rgba1 = { .red   = ( double ) gtk3ped->core.noise->colorPair[pid][0x00].r,
                      .green = ( double ) gtk3ped->core.noise->colorPair[pid][0x00].g,
                      .blue  = ( double ) gtk3ped->core.noise->colorPair[pid][0x00].b,
                      .alpha = ( double ) 1.0f };
    GdkRGBA rgba2 = { .red   = ( double ) gtk3ped->core.noise->colorPair[pid][0x01].r,
                      .green = ( double ) gtk3ped->core.noise->colorPair[pid][0x01].g,
                      .blue  = ( double ) gtk3ped->core.noise->colorPair[pid][0x01].b,
                      .alpha = ( double ) 1.0f };

    gtk3ped->core.gui->lock = 0x01;

    gtk_color_chooser_set_rgba ( gtk3ped->pair[pid].color1Button, &rgba1 );
    gtk_color_chooser_set_rgba ( gtk3ped->pair[pid].color2Button, &rgba2 );

    gtk_spin_button_set_value ( gtk3ped->pair[pid].thresholdEntry,
                                gtk3ped->core.noise->threshold[pid] );

    if ( pid < gtk3ped->core.noise->nbColors ) {
        gtk_widget_show ( gtk3ped->pair[pid].fixed );
    } else {
        gtk_widget_hide ( gtk3ped->pair[pid].fixed );
    }

    gtk3ped->core.gui->lock = 0x00;
}

/******************************************************************************/
static void updateProceduralNoiseEdit ( GTK3G3DUIPROCEDURALNOISEEDIT *gtk3ped ) {
    uint32_t i;

    gtk3ped->core.gui->lock = 0x01;

    for ( i = 0x00; i < MAXNOISECOLORS; i++ ) {
        updateProceduralNoisePair ( gtk3ped, i );
    }

    gtk_spin_button_set_value ( gtk3ped->uGradientsEntry, gtk3ped->core.noise->nbGradientX );
    gtk_spin_button_set_value ( gtk3ped->vGradientsEntry, gtk3ped->core.noise->nbGradientY );
    gtk_spin_button_set_value ( gtk3ped->octavesEntry   , gtk3ped->core.noise->nbOctaves   );
    gtk_spin_button_set_value ( gtk3ped->nbpairsEntry   , gtk3ped->core.noise->nbColors    );

    gtk3ped->core.gui->lock = 0x00;
}

/******************************************************************************/
static uint32_t getPairID ( GTK3G3DUIPROCEDURALNOISEEDIT *gtk3ped,
                            GtkWidget                    *widget ) {
    uint32_t i;

    for ( i = 0x00; i < MAXNOISECOLORS; i++ ) {
        if ( widget == gtk3ped->pair[i].color1Button   ) return i;
        if ( widget == gtk3ped->pair[i].color2Button   ) return i;
        if ( widget == gtk3ped->pair[i].thresholdEntry ) return i;
    }

    return 0x00;
}

/******************************************************************************/
static void thresholdCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPROCEDURALNOISEEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALNOISEEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;
    float str = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    uint32_t pid = getPairID ( gtk3ped, widget );

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    gtk3ped->core.noise->threshold[pid] = str;

    g3dproceduralnoise_copySettings ( gtk3ped->core.noise, &gtk3ped->core.dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &gtk3ped->core.dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( gtk3ped->drawingArea );

    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMATERIALPREVIEW |
                                           REDRAWMATERIALLIST           |
                                           REDRAWVIEW );
}

/******************************************************************************/
static void color1Cbk ( GtkWidget *widget, gpointer user_data ) {
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER ( widget );
    GTK3G3DUIPROCEDURALNOISEEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALNOISEEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;
    uint32_t pid = getPairID ( gtk3ped, widget );
    GdkRGBA color;

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    gtk_color_chooser_get_rgba ( ccr, &color );

    gtk3ped->core.noise->colorPair[pid][0x00].r = color.red;
    gtk3ped->core.noise->colorPair[pid][0x00].g = color.green;
    gtk3ped->core.noise->colorPair[pid][0x00].b = color.blue;
    gtk3ped->core.noise->colorPair[pid][0x00].a = color.alpha;

    g3dproceduralnoise_copySettings ( gtk3ped->core.noise, &gtk3ped->core.dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &gtk3ped->core.dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( gtk3ped->drawingArea );

    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMATERIALPREVIEW |
                                           REDRAWMATERIALLIST           |
                                           REDRAWVIEW );
}

/******************************************************************************/
static void color2Cbk ( GtkWidget *widget, gpointer user_data ) {
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER ( widget );
    GTK3G3DUIPROCEDURALNOISEEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALNOISEEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;
    uint32_t pid = getPairID ( gtk3ped, widget );
    GdkRGBA color;

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    gtk_color_chooser_get_rgba ( ccr, &color );

    gtk3ped->core.noise->colorPair[pid][0x01].r = color.red;
    gtk3ped->core.noise->colorPair[pid][0x01].g = color.green;
    gtk3ped->core.noise->colorPair[pid][0x01].b = color.blue;
    gtk3ped->core.noise->colorPair[pid][0x01].a = color.alpha;

    g3dproceduralnoise_copySettings ( gtk3ped->core.noise, &gtk3ped->core.dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &gtk3ped->core.dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( gtk3ped->drawingArea );

    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMATERIALPREVIEW |
                                           REDRAWMATERIALLIST           |
                                           REDRAWVIEW );
}

/******************************************************************************/
static void nbpairsCbk ( GtkWidget *widget, gpointer user_data ) {
    int nbPairs = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON ( widget ) );
    GTK3G3DUIPROCEDURALNOISEEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALNOISEEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    gtk3ped->core.noise->nbColors = nbPairs;

    g3dproceduralnoise_copySettings ( gtk3ped->core.noise, &gtk3ped->core.dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &gtk3ped->core.dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( gtk3ped->drawingArea );

    updateProceduralNoiseEdit ( gtk3ped );

    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMATERIALPREVIEW |
                                           REDRAWMATERIALLIST           |
                                           REDRAWVIEW );
}

/******************************************************************************/
static void octavesCbk ( GtkWidget *widget, gpointer user_data ) {
    int nbOctaves = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON ( widget ) );
    GTK3G3DUIPROCEDURALNOISEEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALNOISEEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    gtk3ped->core.noise->nbOctaves = nbOctaves;

    g3dproceduralnoise_copySettings ( gtk3ped->core.noise, &gtk3ped->core.dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &gtk3ped->core.dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( gtk3ped->drawingArea );

    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMATERIALPREVIEW |
                                           REDRAWMATERIALLIST           |
                                           REDRAWVIEW ); 
}

/******************************************************************************/
static void xGradientsCbk ( GtkWidget *widget, gpointer user_data ) {
    int nbGradientX = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON ( widget ) );
    GTK3G3DUIPROCEDURALNOISEEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALNOISEEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;
    int nbGradientY = gtk3ped->core.noise->nbGradientY;

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3dproceduralnoise_buildGradients ( gtk3ped->core.noise, nbGradientX, nbGradientY );

    g3dproceduralnoise_copySettings ( gtk3ped->core.noise, &gtk3ped->core.dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &gtk3ped->core.dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( gtk3ped->drawingArea );

    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMATERIALPREVIEW |
                                           REDRAWMATERIALLIST           |
                                           REDRAWVIEW ); 
}

/******************************************************************************/
static void yGradientsCbk ( GtkWidget *widget, gpointer user_data ) {
    int nbGradientY = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON ( widget ) );
    GTK3G3DUIPROCEDURALNOISEEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALNOISEEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;
    int nbGradientX = gtk3ped->core.noise->nbGradientX;

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    g3dproceduralnoise_buildGradients ( gtk3ped->core.noise, nbGradientX, nbGradientY );

    g3dproceduralnoise_copySettings ( gtk3ped->core.noise, &gtk3ped->core.dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &gtk3ped->core.dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( gtk3ped->drawingArea );

    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMATERIALPREVIEW |
                                           REDRAWMATERIALLIST           |
                                           REDRAWVIEW ); 
}

/******************************************************************************/
static gboolean previewCbk ( GtkWidget *widget,  
                             cairo_t   *cr,  
                             gpointer   user_data ) {
    GTK3G3DUIPROCEDURALNOISEEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALNOISEEDIT * ) user_data;

    gdk_cairo_set_source_pixbuf ( cr, gtk3ped->pix, 0, 0 );

    cairo_paint ( cr );

    return FALSE;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPROCEDURALNOISEEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALNOISEEDIT * ) user_data;
    void *imageData = gtk3ped->core.dummy.procedural.image.data;

    if ( imageData ) free ( imageData );

    if ( gtk3ped->pix ) g_object_unref ( G_OBJECT ( gtk3ped->pix ) );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) gtk3ped->core.noise, 256, 256, 24, gtk3ped->core.bindGL );

    free ( gtk3ped );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPROCEDURALNOISEEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALNOISEEDIT * ) user_data;

    updateProceduralNoiseEdit ( gtk3ped );
}

/******************************************************************************/
GTK3G3DUIPROCEDURALNOISEEDIT* 
gtk3_g3duiproceduralnoiseedit_create ( GtkWidget          *parent,
                                       GTK3G3DUI          *gtk3gui,
                                       G3DPROCEDURALNOISE *noise,
                                       char               *name,
                                       gint                x,
                                       gint                y,
                                       gint                width,
                                       gint                height,
                                       uint32_t            bindGL ) {
    GTK3G3DUIPROCEDURALNOISEEDIT *gtk3ped = calloc ( 0x01, sizeof ( GTK3G3DUIPROCEDURALNOISEEDIT ) );
    GdkRectangle gdkrec = { 0, 0, width, height };
    GtkFixed *fixed = ui_gtk_fixed_new ( CLASS_MAIN );
    uint32_t pcy = 96;
    uint32_t i;

    gtk_widget_set_name ( fixed, name );

    gtk3ped->fixed = fixed;

    gtk_widget_set_size_request ( fixed, gdkrec.width, gdkrec.height );

    gtk3ped->core.gui    = gtk3gui;
    gtk3ped->core.noise  = noise;
    gtk3ped->core.bindGL = bindGL;

    g3dproceduralnoise_copySettings ( gtk3ped->core.noise, &gtk3ped->core.dummy );

    gtk3ped->core.dummy.procedural.getColor = noise->procedural.getColor;

    g3dprocedural_fill ( &gtk3ped->core.dummy.procedural, 256, 256, 24, 0x00 );

    gtk3ped->pix = gdk_pixbuf_new_from_data ( gtk3ped->core.dummy.procedural.image.data,
                                              GDK_COLORSPACE_RGB,
                                              FALSE,
                                              0x08,
                                              gtk3ped->core.dummy.procedural.image.width,
                                              gtk3ped->core.dummy.procedural.image.height,
                                              gtk3ped->core.dummy.procedural.image.bytesPerLine,
                                              NULL, NULL );


    gtk3ped->drawingArea     = ui_createDrawingArea ( fixed, gtk3ped, "preview" , CLASS_MAIN, 224,              0,      256,    256, previewCbk    );

    gtk3ped->uGradientsEntry = ui_createIntegerText ( fixed, gtk3ped, UGRADIENTS, CLASS_MAIN,   1,            255,   0,   0, 96, 96, 20, xGradientsCbk );
    gtk3ped->vGradientsEntry = ui_createIntegerText ( fixed, gtk3ped, VGRADIENTS, CLASS_MAIN,   1,            255,   0,  24, 96, 96, 20, yGradientsCbk );

    gtk3ped->octavesEntry    = ui_createIntegerText ( fixed, gtk3ped, OCTAVES   , CLASS_MAIN,   1,             10,   0,  48, 96, 96, 20, octavesCbk    );
    gtk3ped->nbpairsEntry    = ui_createIntegerText ( fixed, gtk3ped, NBPAIRS   , CLASS_MAIN,   1, MAXNOISECOLORS,   0,  72, 96, 96, 20, nbpairsCbk    );

    for ( i = 0x00; i < MAXNOISECOLORS; i++ ) {
        char labelstr[0x10] = { 0 };

        gtk3ped->pair[i].fixed = ui_gtk_fixed_new ( CLASS_MAIN );

        gtk_fixed_put ( fixed, GTK_WIDGET(gtk3ped->pair[i].fixed), 0, pcy );

        snprintf ( labelstr, 0x10, "#%d", i + 1 );

                                          ui_createSimpleLabel ( gtk3ped->pair[i].fixed, gtk3ped, labelstr, CLASS_MAIN,   0, 0,     20, 20            );
        gtk3ped->pair[i].color1Button   = ui_createColorButton ( gtk3ped->pair[i].fixed, gtk3ped,  "Col 1", CLASS_MAIN,  48, 0, 32, 32, 20, color1Cbk );
        gtk3ped->pair[i].color2Button   = ui_createColorButton ( gtk3ped->pair[i].fixed, gtk3ped,  "Col 2", CLASS_MAIN, 120, 0, 32, 32, 20, color2Cbk );

        gtk3ped->pair[i].thresholdEntry = ui_createFloatText   ( gtk3ped->pair[i].fixed, gtk3ped, "Threshold", CLASS_MAIN, 0.0f, 1.0f, 0, 24, 96, 64, 20, thresholdCbk );


        pcy += 48;
    }

    g_signal_connect ( G_OBJECT (fixed), "realize", G_CALLBACK (Realize), gtk3ped );
    g_signal_connect ( G_OBJECT (fixed), "destroy", G_CALLBACK (Destroy), gtk3ped );

    gtk_widget_show_all ( fixed );


    return gtk3ped;
}
