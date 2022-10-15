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
#define COLOR1     "Color 1"
#define COLOR2     "Color 2"
#define HORIZONTAL "Horizontal"
#define VERTICAL   "Vertical"

/******************************************************************************/
static void updateProceduralGradientEdit ( GTK3G3DUIPROCEDURALGRADIENTEDIT *gtk3ped ) {
    GdkRGBA color1 = { .red   = ( double ) gtk3ped->core.gradient->color1.r,
                       .green = ( double ) gtk3ped->core.gradient->color1.g,
                       .blue  = ( double ) gtk3ped->core.gradient->color1.b,
                       .alpha = ( double ) 1.0f };
    GdkRGBA color2 = { .red   = ( double ) gtk3ped->core.gradient->color2.r,
                       .green = ( double ) gtk3ped->core.gradient->color2.g,
                       .blue  = ( double ) gtk3ped->core.gradient->color2.b,
                       .alpha = ( double ) 1.0f };

    gtk3ped->core.gui->lock = 0x01;

    gtk_color_chooser_set_rgba ( gtk3ped->color1Button, &color1 );
    gtk_color_chooser_set_rgba ( gtk3ped->color2Button, &color2 );

    if ( gtk3ped->core.gradient->horizontal ) {
        gtk_toggle_button_set_active ( gtk3ped->horizontalToggle, TRUE  );
        gtk_toggle_button_set_active ( gtk3ped->verticalToggle  , FALSE );
    } else {
        gtk_toggle_button_set_active ( gtk3ped->horizontalToggle, FALSE );
        gtk_toggle_button_set_active ( gtk3ped->verticalToggle  , TRUE  );
    }

    gtk3ped->core.gui->lock = 0x00;
}

/******************************************************************************/
static void color2Cbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPROCEDURALGRADIENTEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALGRADIENTEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER ( widget );
    GdkRGBA color;

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    gtk_color_chooser_get_rgba ( ccr, &color );

    gtk3ped->core.gradient->color2.r = color.red;
    gtk3ped->core.gradient->color2.g = color.green;
    gtk3ped->core.gradient->color2.b = color.blue;
    gtk3ped->core.gradient->color2.a = color.alpha;

    g3dproceduralgradient_copySettings ( gtk3ped->core.gradient, &gtk3ped->core.dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &gtk3ped->core.dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( gtk3ped->drawingArea );

    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMATERIALPREVIEW |
                                           REDRAWMATERIALLIST           |
                                           REDRAWVIEW ); 
}

/******************************************************************************/
static void color1Cbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPROCEDURALGRADIENTEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALGRADIENTEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER ( widget );
    GdkRGBA color;

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    gtk_color_chooser_get_rgba ( ccr, &color );

    gtk3ped->core.gradient->color1.r = color.red;
    gtk3ped->core.gradient->color1.g = color.green;
    gtk3ped->core.gradient->color1.b = color.blue;
    gtk3ped->core.gradient->color1.a = color.alpha;

    g3dproceduralgradient_copySettings ( gtk3ped->core.gradient, &gtk3ped->core.dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &gtk3ped->core.dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( gtk3ped->drawingArea );

    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMATERIALPREVIEW |
                                           REDRAWMATERIALLIST           |
                                           REDRAWVIEW ); 
}

/******************************************************************************/
static void horizontalToggleCbk ( GtkWidget *widget, 
                                  gpointer   user_data ) {
    GTK3G3DUIPROCEDURALGRADIENTEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALGRADIENTEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    gtk3ped->core.gradient->horizontal = 0x01;

    g3dproceduralgradient_copySettings ( gtk3ped->core.gradient, &gtk3ped->core.dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &gtk3ped->core.dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( gtk3ped->drawingArea );

    updateProceduralGradientEdit ( gtk3ped );

    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMATERIALPREVIEW |
                                           REDRAWMATERIALLIST           |
                                           REDRAWVIEW ); 
}

/******************************************************************************/
static void verticalToggleCbk ( GtkWidget *widget, 
                                gpointer   user_data ) {
    GTK3G3DUIPROCEDURALGRADIENTEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALGRADIENTEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    gtk3ped->core.gradient->horizontal = 0x00;

    g3dproceduralgradient_copySettings ( gtk3ped->core.gradient, &gtk3ped->core.dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &gtk3ped->core.dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( gtk3ped->drawingArea );

    updateProceduralGradientEdit ( gtk3ped );

    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMATERIALPREVIEW |
                                           REDRAWMATERIALLIST           |
                                           REDRAWVIEW ); 
}

/******************************************************************************/
static gboolean previewCbk ( GtkWidget *widget,  
                             cairo_t   *cr,  
                             gpointer   user_data ) {
    GTK3G3DUIPROCEDURALGRADIENTEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALGRADIENTEDIT * ) user_data;

    gdk_cairo_set_source_pixbuf ( cr, gtk3ped->pix, 0, 0 );

    cairo_paint ( cr );

    return FALSE;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPROCEDURALGRADIENTEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALGRADIENTEDIT * ) user_data;
    void *imageData = gtk3ped->core.dummy.procedural.image.data;

    if ( imageData ) free ( imageData );

    if ( gtk3ped->pix ) g_object_unref ( G_OBJECT ( gtk3ped->pix ) );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) gtk3ped->core.gradient, 256, 256, 24, gtk3ped->core.bindGL );

    free ( gtk3ped );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPROCEDURALGRADIENTEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALGRADIENTEDIT * ) user_data;

    updateProceduralGradientEdit ( gtk3ped );
}

/******************************************************************************/
GTK3G3DUIPROCEDURALGRADIENTEDIT* 
gtk3_g3duiproceduralgradientedit_create ( GtkWidget             *parent,
                                          GTK3G3DUI             *gtk3gui,
                                          G3DPROCEDURALGRADIENT *gradient,
                                          char                  *name,
                                          gint                   x,
                                          gint                   y,
                                          gint                   width,
                                          gint                   height,
                                          uint32_t               bindGL ) {
    GTK3G3DUIPROCEDURALGRADIENTEDIT *gtk3ped = calloc ( 0x01, sizeof ( GTK3G3DUIPROCEDURALGRADIENTEDIT ) );
    GdkRectangle gdkrec = { 0, 0, width, height };
    GtkWidget *fixed = ui_gtk_fixed_new ( CLASS_MAIN );

    gtk_widget_set_name ( fixed, name );

    gtk3ped->fixed = fixed;

    gtk_widget_set_size_request ( fixed, gdkrec.width, gdkrec.height );

    gtk3ped->core.gui    = gtk3gui;
    gtk3ped->core.gradient  = gradient;
    gtk3ped->core.bindGL = bindGL;

    g3dproceduralgradient_copySettings ( gtk3ped->core.gradient, &gtk3ped->core.dummy );

    gtk3ped->core.dummy.procedural.getColor = gradient->procedural.getColor;

    g3dprocedural_fill ( &gtk3ped->core.dummy.procedural, 256, 256, 24, 0x00 );

    gtk3ped->pix = gdk_pixbuf_new_from_data ( gtk3ped->core.dummy.procedural.image.data,
                                              GDK_COLORSPACE_RGB,
                                              FALSE,
                                              0x08,
                                              gtk3ped->core.dummy.procedural.image.width,
                                              gtk3ped->core.dummy.procedural.image.height,
                                              gtk3ped->core.dummy.procedural.image.bytesPerLine,
                                              NULL, NULL );


    gtk3ped->drawingArea      = ui_createDrawingArea ( fixed, gtk3ped, "preview" , CLASS_MAIN, 192,  0,    256, 256, previewCbk );

    gtk3ped->color1Button     = ui_createColorButton ( fixed, gtk3ped, COLOR1    , CLASS_MAIN,   0,  0, 96, 92,  20, color1Cbk  );
    gtk3ped->color2Button     = ui_createColorButton ( fixed, gtk3ped, COLOR2    , CLASS_MAIN,   0, 24, 96, 92,  20, color2Cbk  );
    gtk3ped->horizontalToggle = ui_createToggleLabel ( fixed, gtk3ped, HORIZONTAL, CLASS_MAIN,   0, 48, 96, 20,  20, horizontalToggleCbk );
    gtk3ped->verticalToggle   = ui_createToggleLabel ( fixed, gtk3ped, VERTICAL  , CLASS_MAIN,   0, 72, 96, 20,  20, verticalToggleCbk   );

    g_signal_connect ( G_OBJECT (fixed), "realize", G_CALLBACK (Realize), gtk3ped );
    g_signal_connect ( G_OBJECT (fixed), "destroy", G_CALLBACK (Destroy), gtk3ped );

    gtk_widget_show_all ( fixed );


    return gtk3ped;
}
