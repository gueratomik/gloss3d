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
#define UDIVISIONS "U Divisions"
#define VDIVISIONS "V Divisions"

/******************************************************************************/
static void color2Cbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPROCEDURALCHESSEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALCHESSEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER ( widget );
    GdkRGBA color;

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    gtk_color_chooser_get_rgba ( ccr, &color );

    gtk3ped->core.chess->color2.r = color.red;
    gtk3ped->core.chess->color2.g = color.green;
    gtk3ped->core.chess->color2.b = color.blue;
    gtk3ped->core.chess->color2.a = color.alpha;

    g3dproceduralchess_copySettings ( gtk3ped->core.chess, &gtk3ped->core.dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &gtk3ped->core.dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( gtk3ped->drawingArea );

    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMATERIALPREVIEW |
                                           REDRAWMATERIALLIST           |
                                           REDRAWVIEW ); 
}

/******************************************************************************/
static void color1Cbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPROCEDURALCHESSEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALCHESSEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER ( widget );
    GdkRGBA color;

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    gtk_color_chooser_get_rgba ( ccr, &color );

    gtk3ped->core.chess->color1.r = color.red;
    gtk3ped->core.chess->color1.g = color.green;
    gtk3ped->core.chess->color1.b = color.blue;
    gtk3ped->core.chess->color1.a = color.alpha;

    g3dproceduralchess_copySettings ( gtk3ped->core.chess, &gtk3ped->core.dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &gtk3ped->core.dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( gtk3ped->drawingArea );

    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMATERIALPREVIEW |
                                           REDRAWMATERIALLIST           |
                                           REDRAWVIEW ); 
}

/******************************************************************************/
static void uDivCbk ( GtkWidget *widget, gpointer user_data ) {
    float udiv = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON ( widget ) );
    GTK3G3DUIPROCEDURALCHESSEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALCHESSEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    gtk3ped->core.chess->udiv = udiv;

    g3dproceduralchess_copySettings ( gtk3ped->core.chess, &gtk3ped->core.dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &gtk3ped->core.dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( gtk3ped->drawingArea );

    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMATERIALPREVIEW |
                                           REDRAWMATERIALLIST           |
                                           REDRAWVIEW ); 
}

/******************************************************************************/
static void vDivCbk ( GtkWidget *widget, gpointer user_data ) {
    float vdiv = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON ( widget ) );
    GTK3G3DUIPROCEDURALCHESSEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALCHESSEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    gtk3ped->core.chess->vdiv = vdiv;

    g3dproceduralchess_copySettings ( gtk3ped->core.chess, &gtk3ped->core.dummy );

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
    GTK3G3DUIPROCEDURALCHESSEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALCHESSEDIT * ) user_data;

    gdk_cairo_set_source_pixbuf ( cr, gtk3ped->pix, 0, 0 );

    cairo_paint ( cr );

    return FALSE;
}

/******************************************************************************/
static void updateProceduralChessEdit ( GTK3G3DUIPROCEDURALCHESSEDIT *gtk3ped ) {
    GdkRGBA color1 = { .red   = ( double ) gtk3ped->core.chess->color1.r,
                       .green = ( double ) gtk3ped->core.chess->color1.g,
                       .blue  = ( double ) gtk3ped->core.chess->color1.b,
                       .alpha = ( double ) 1.0f };
    GdkRGBA color2 = { .red   = ( double ) gtk3ped->core.chess->color2.r,
                       .green = ( double ) gtk3ped->core.chess->color2.g,
                       .blue  = ( double ) gtk3ped->core.chess->color2.b,
                       .alpha = ( double ) 1.0f };

    gtk3ped->core.gui->lock = 0x01;

    gtk_color_chooser_set_rgba ( gtk3ped->color1Button, &color1 );
    gtk_color_chooser_set_rgba ( gtk3ped->color2Button, &color2 );
    gtk_spin_button_set_value  ( gtk3ped->uDivEntry,
                                 gtk3ped->core.chess->udiv );
    gtk_spin_button_set_value  ( gtk3ped->vDivEntry,
                                 gtk3ped->core.chess->vdiv );

    gtk3ped->core.gui->lock = 0x00;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPROCEDURALCHESSEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALCHESSEDIT * ) user_data;
    void *imageData = gtk3ped->core.dummy.procedural.image.data;

    if ( imageData ) free ( imageData );

    if ( gtk3ped->pix ) g_object_unref ( G_OBJECT ( gtk3ped->pix ) );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) gtk3ped->core.chess, 256, 256, 24, gtk3ped->core.bindGL );

    free ( gtk3ped );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPROCEDURALCHESSEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALCHESSEDIT * ) user_data;

    updateProceduralChessEdit ( gtk3ped );
}

/******************************************************************************/
GTK3G3DUIPROCEDURALCHESSEDIT* 
gtk3_g3duiproceduralchessedit_create ( GtkWidget          *parent,
                                       GTK3G3DUI          *gtk3gui,
                                       G3DPROCEDURALCHESS *chess,
                                       char               *name,
                                       gint                x,
                                       gint                y,
                                       gint                width,
                                       gint                height,
                                       uint32_t            bindGL ) {
    GTK3G3DUIPROCEDURALCHESSEDIT *gtk3ped = calloc ( 0x01, sizeof ( GTK3G3DUIPROCEDURALCHESSEDIT ) );
    GdkRectangle gdkrec = { 0, 0, width, height };
    GtkWidget *fixed = ui_gtk_fixed_new ( CLASS_MAIN );

    gtk_widget_set_name ( fixed, name );

    gtk3ped->fixed = fixed;

    gtk_widget_set_size_request ( fixed, gdkrec.width, gdkrec.height );

    gtk3ped->core.gui    = gtk3gui;
    gtk3ped->core.chess  = chess;
    gtk3ped->core.bindGL = bindGL;

    g3dproceduralchess_copySettings ( gtk3ped->core.chess, &gtk3ped->core.dummy );

    gtk3ped->core.dummy.procedural.getColor = chess->procedural.getColor;

    g3dprocedural_fill ( &gtk3ped->core.dummy.procedural, 256, 256, 24, 0x00 );

    gtk3ped->pix = gdk_pixbuf_new_from_data ( gtk3ped->core.dummy.procedural.image.data,
                                              GDK_COLORSPACE_RGB,
                                              FALSE,
                                              0x08,
                                              gtk3ped->core.dummy.procedural.image.width,
                                              gtk3ped->core.dummy.procedural.image.height,
                                              gtk3ped->core.dummy.procedural.image.bytesPerLine,
                                              NULL, NULL );


    gtk3ped->drawingArea  = ui_createDrawingArea ( fixed, gtk3ped, "preview" , CLASS_MAIN,         192,  0,    256, 256, previewCbk );
    gtk3ped->color1Button = ui_createColorButton ( fixed, gtk3ped, COLOR1    , CLASS_MAIN,           0,  0, 96, 96,  20, color1Cbk  );
    gtk3ped->color2Button = ui_createColorButton ( fixed, gtk3ped, COLOR2    , CLASS_MAIN,           0, 24, 96, 96,  20, color2Cbk  );
    gtk3ped->uDivEntry    = ui_createIntegerText ( fixed, gtk3ped, UDIVISIONS, CLASS_MAIN,   0, 100, 0, 48, 96, 96,  20, uDivCbk    );
    gtk3ped->vDivEntry    = ui_createIntegerText ( fixed, gtk3ped, VDIVISIONS, CLASS_MAIN,   0, 100, 0, 72, 96, 96,  20, vDivCbk    );

    g_signal_connect ( G_OBJECT (fixed), "realize", G_CALLBACK (Realize), gtk3ped );
    g_signal_connect ( G_OBJECT (fixed), "destroy", G_CALLBACK (Destroy), gtk3ped );

    gtk_widget_show_all ( fixed );


    return gtk3ped;
}
