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
#define BRICKCOLOR      "Brick color"
#define SPACINGCOLOR    "Spacing color"
#define NBLINES         "Num. lines"
#define NBBRICKSPERLINE "Bricks per line"
#define USPACING        "U spacing"
#define VSPACING        "V spacing"

/******************************************************************************/
static void spacingColorCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPROCEDURALBRICKEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALBRICKEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER ( widget );
    GdkRGBA color;

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    gtk_color_chooser_get_rgba ( ccr, &color );

    gtk3ped->core.brick->spacingColor.r = color.red;
    gtk3ped->core.brick->spacingColor.g = color.green;
    gtk3ped->core.brick->spacingColor.b = color.blue;
    gtk3ped->core.brick->spacingColor.a = color.alpha;

    g3dproceduralbrick_copySettings ( gtk3ped->core.brick, &gtk3ped->core.dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &gtk3ped->core.dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( gtk3ped->drawingArea );

    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMATERIALPREVIEW |
                                           REDRAWMATERIALLIST           |
                                           REDRAWVIEW ); 
}

/******************************************************************************/
static void brickColorCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPROCEDURALBRICKEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALBRICKEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER ( widget );
    GdkRGBA color;

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    gtk_color_chooser_get_rgba ( ccr, &color );

    gtk3ped->core.brick->brickColor.r = color.red;
    gtk3ped->core.brick->brickColor.g = color.green;
    gtk3ped->core.brick->brickColor.b = color.blue;
    gtk3ped->core.brick->brickColor.a = color.alpha;

    g3dproceduralbrick_copySettings ( gtk3ped->core.brick, &gtk3ped->core.dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &gtk3ped->core.dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( gtk3ped->drawingArea );

    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMATERIALPREVIEW |
                                           REDRAWMATERIALLIST           |
                                           REDRAWVIEW ); 
}

/******************************************************************************/
static void nbBricksPerLineCbk ( GtkWidget *widget, gpointer user_data ) {
    int nbBricksPerLine = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON ( widget ) );
    GTK3G3DUIPROCEDURALBRICKEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALBRICKEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    gtk3ped->core.brick->nbBricksPerLine = nbBricksPerLine;

    g3dproceduralbrick_copySettings ( gtk3ped->core.brick, &gtk3ped->core.dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &gtk3ped->core.dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( gtk3ped->drawingArea );

    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMATERIALPREVIEW |
                                           REDRAWMATERIALLIST           |
                                           REDRAWVIEW ); 
}

/******************************************************************************/
static void nbLinesCbk ( GtkWidget *widget, gpointer user_data ) {
    int nbLines = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON ( widget ) );
    GTK3G3DUIPROCEDURALBRICKEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALBRICKEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    gtk3ped->core.brick->nbLines = nbLines;

    g3dproceduralbrick_copySettings ( gtk3ped->core.brick, &gtk3ped->core.dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &gtk3ped->core.dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( gtk3ped->drawingArea );

    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMATERIALPREVIEW |
                                           REDRAWMATERIALLIST           |
                                           REDRAWVIEW ); 
}

/******************************************************************************/
static void uspacingCbk ( GtkWidget *widget, gpointer user_data ) {
    float uspacing = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON ( widget ) );
    GTK3G3DUIPROCEDURALBRICKEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALBRICKEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    gtk3ped->core.brick->uspacing = uspacing;

    g3dproceduralbrick_copySettings ( gtk3ped->core.brick, &gtk3ped->core.dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &gtk3ped->core.dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( gtk3ped->drawingArea );

    gtk3_interpretUIReturnFlags ( gtk3gui, UPDATECURRENTMATERIALPREVIEW |
                                           REDRAWMATERIALLIST           |
                                           REDRAWVIEW ); 
}

/******************************************************************************/
static void vspacingCbk ( GtkWidget *widget, gpointer user_data ) {
    float vspacing = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON ( widget ) );
    GTK3G3DUIPROCEDURALBRICKEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALBRICKEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ped->core.gui;

    /*** prevent a loop ***/
    if ( gtk3ped->core.gui->lock ) return;

    gtk3ped->core.brick->vspacing = vspacing;

    g3dproceduralbrick_copySettings ( gtk3ped->core.brick, &gtk3ped->core.dummy );

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
    GTK3G3DUIPROCEDURALBRICKEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALBRICKEDIT * ) user_data;

    gdk_cairo_set_source_pixbuf ( cr, gtk3ped->pix, 0, 0 );

    cairo_paint ( cr );

    return FALSE;
}

/******************************************************************************/
static void updateProceduralBrickEdit ( GTK3G3DUIPROCEDURALBRICKEDIT *gtk3ped ) {
    GdkRGBA brickColor = { .red   = ( float ) gtk3ped->core.brick->brickColor.r,
                           .green = ( float ) gtk3ped->core.brick->brickColor.g,
                           .blue  = ( float ) gtk3ped->core.brick->brickColor.b,
                           .alpha = ( float ) 1.0f };
    GdkRGBA spacingColor = { .red   = ( float ) gtk3ped->core.brick->spacingColor.r,
                             .green = ( float ) gtk3ped->core.brick->spacingColor.g,
                             .blue  = ( float ) gtk3ped->core.brick->spacingColor.b,
                             .alpha = ( float ) 1.0f };

    gtk3ped->core.gui->lock = 0x01;

    gtk_color_chooser_set_rgba ( gtk3ped->brickColorButton    , &brickColor   );
    gtk_color_chooser_set_rgba ( gtk3ped->spacingColorButton  , &spacingColor );
    gtk_spin_button_set_value  ( gtk3ped->nbLinesEntry        ,
                                 gtk3ped->core.brick->nbLines );
    gtk_spin_button_set_value  ( gtk3ped->nbBricksPerLineEntry,
                                 gtk3ped->core.brick->nbBricksPerLine );
    gtk_spin_button_set_value  ( gtk3ped->uSpacingEntry,
                                 gtk3ped->core.brick->uspacing );
    gtk_spin_button_set_value  ( gtk3ped->vSpacingEntry,
                                 gtk3ped->core.brick->vspacing );

    gtk3ped->core.gui->lock = 0x00;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPROCEDURALBRICKEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALBRICKEDIT * ) user_data;
    void *imageData = gtk3ped->core.dummy.procedural.image.data;

    if ( imageData ) free ( imageData );

    if ( gtk3ped->pix ) g_object_unref ( G_OBJECT ( gtk3ped->pix ) );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) gtk3ped->core.brick, 256, 256, 24, gtk3ped->core.bindGL );

    free ( gtk3ped );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIPROCEDURALBRICKEDIT *gtk3ped = ( GTK3G3DUIPROCEDURALBRICKEDIT * ) user_data;

    updateProceduralBrickEdit ( gtk3ped );
}

/******************************************************************************/
GTK3G3DUIPROCEDURALBRICKEDIT* 
gtk3_g3duiproceduralbrickedit_create ( GtkWidget          *parent,
                                       GTK3G3DUI          *gtk3gui,
                                       G3DPROCEDURALBRICK *brick,
                                       char               *name,
                                       gint                x,
                                       gint                y,
                                       gint                width,
                                       gint                height,
                                       uint32_t            bindGL ) {
    GTK3G3DUIPROCEDURALBRICKEDIT *gtk3ped = calloc ( 0x01, sizeof ( GTK3G3DUIPROCEDURALBRICKEDIT ) );
    GdkRectangle gdkrec = { 0, 0, width, height };
    GtkWidget *fixed = ui_gtk_fixed_new ( CLASS_MAIN );

    gtk_widget_set_name ( fixed, name );

    gtk3ped->fixed = fixed;

    gtk_widget_set_size_request ( fixed, gdkrec.width, gdkrec.height );

    gtk3ped->core.gui    = gtk3gui;
    gtk3ped->core.brick  = brick;
    gtk3ped->core.bindGL = bindGL;

    g3dproceduralbrick_copySettings ( gtk3ped->core.brick, &gtk3ped->core.dummy );

    gtk3ped->core.dummy.procedural.getColor = brick->procedural.getColor;

    g3dprocedural_fill ( &gtk3ped->core.dummy.procedural, 256, 256, 24, 0x00 );

    gtk3ped->pix = gdk_pixbuf_new_from_data ( gtk3ped->core.dummy.procedural.image.data,
                                              GDK_COLORSPACE_RGB,
                                              FALSE,
                                              0x08,
                                              gtk3ped->core.dummy.procedural.image.width,
                                              gtk3ped->core.dummy.procedural.image.height,
                                              gtk3ped->core.dummy.procedural.image.bytesPerLine,
                                              NULL, NULL );


    gtk3ped->drawingArea          = ui_createDrawingArea ( fixed, gtk3ped, "preview"      , CLASS_MAIN,         192,   0,    256, 256, previewCbk         );

    gtk3ped->brickColorButton     = ui_createColorButton ( fixed, gtk3ped, BRICKCOLOR     , CLASS_MAIN,           0,   0, 96, 96,  20, brickColorCbk      );
    gtk3ped->spacingColorButton   = ui_createColorButton ( fixed, gtk3ped, SPACINGCOLOR   , CLASS_MAIN,           0,  24, 96, 96,  20, spacingColorCbk    );
    gtk3ped->nbLinesEntry         = ui_createIntegerText ( fixed, gtk3ped, NBLINES        , CLASS_MAIN,   0, 100, 0,  48, 96, 96,  20, nbLinesCbk         );
    gtk3ped->nbBricksPerLineEntry = ui_createIntegerText ( fixed, gtk3ped, NBBRICKSPERLINE, CLASS_MAIN,   0, 100, 0,  72, 96, 96,  20, nbBricksPerLineCbk );
    gtk3ped->uSpacingEntry        = ui_createFloatText   ( fixed, gtk3ped, USPACING       , CLASS_MAIN,   0, 100, 0,  96, 96, 96,  20, uspacingCbk        );
    gtk3ped->vSpacingEntry        = ui_createFloatText   ( fixed, gtk3ped, VSPACING       , CLASS_MAIN,   0, 100, 0, 120, 96, 96,  20, vspacingCbk        );

    g_signal_connect ( G_OBJECT (fixed), "realize", G_CALLBACK (Realize), gtk3ped );
    g_signal_connect ( G_OBJECT (fixed), "destroy", G_CALLBACK (Destroy), gtk3ped );

    gtk_widget_show_all ( fixed );


    return gtk3ped;
}
