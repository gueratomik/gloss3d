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
typedef struct _PROCEDURALEDIT {
    G3DUIWIDGETGROUP       grp;
    G3DPROCEDURALGRADIENT *gradient;
    GdkPixbuf             *pix;
    GtkWidget             *areaWidget;
    GtkWidget             *color1Button;
    GtkWidget             *color2Button;
    GtkWidget             *horizontalToggle;
    uint32_t               bindGL;
    G3DPROCEDURALGRADIENT  dummy; /* Stores the temp image at the correct size */
} PROCEDURALEDIT;

/******************************************************************************/
#define COLOR1     "Color 1"
#define COLOR2     "Color 2"
#define HORIZONTAL "Horizontal"

/******************************************************************************/
static void color1Cbk ( GtkWidget *widget, gpointer user_data ) {
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER ( widget );
    GdkRGBA color;

    /*** prevent a loop ***/
    if ( ped->grp.gui->lock ) return;

    gtk_color_chooser_get_rgba ( ccr, &color );

    ped->gradient->color1.r = color.red;
    ped->gradient->color1.g = color.green;
    ped->gradient->color1.b = color.blue;
    ped->gradient->color1.a = color.alpha;

    g3dproceduralgradient_copySettings ( ped->gradient, &ped->dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &ped->dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( ped->areaWidget );

    g3dui_updateSelectedMaterialPreview ( ped->grp.gui );
}

/******************************************************************************/
static void color2Cbk ( GtkWidget *widget, gpointer user_data ) {
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER ( widget );
    GdkRGBA color;

    /*** prevent a loop ***/
    if ( ped->grp.gui->lock ) return;

    gtk_color_chooser_get_rgba ( ccr, &color );

    ped->gradient->color2.r = color.red;
    ped->gradient->color2.g = color.green;
    ped->gradient->color2.b = color.blue;
    ped->gradient->color2.a = color.alpha;

    g3dproceduralgradient_copySettings ( ped->gradient, &ped->dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &ped->dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( ped->areaWidget );

    g3dui_updateSelectedMaterialPreview ( ped->grp.gui );
}

/******************************************************************************/
static void horizontalToggleCbk ( GtkWidget *widget, 
                                  gpointer   user_data ) {
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;

    /*** prevent a loop ***/
    if ( ped->grp.gui->lock ) return;

    ped->gradient->horizontal = ( ped->gradient->horizontal == 0x00 );

    g3dproceduralgradient_copySettings ( ped->gradient, &ped->dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &ped->dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( ped->areaWidget );

    g3dui_updateSelectedMaterialPreview ( ped->grp.gui );
}

/******************************************************************************/
static gboolean previewCbk ( GtkWidget *widget,  
                             cairo_t   *cr,  
                             gpointer   user_data ) {
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;

    gdk_cairo_set_source_pixbuf ( cr, ped->pix, 0, 0 );

    cairo_paint ( cr );

    return FALSE;
}

/******************************************************************************/
static void updateProceduralGradientEdit ( GtkWidget      *widget, 
                                           PROCEDURALEDIT *ped ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    GdkRGBA rgba1 = { .red   = ( double ) ped->gradient->color1.r,
                      .green = ( double ) ped->gradient->color1.g,
                      .blue  = ( double ) ped->gradient->color1.b,
                      .alpha = ( double ) 1.0f };
    GdkRGBA rgba2 = { .red   = ( double ) ped->gradient->color2.r,
                      .green = ( double ) ped->gradient->color2.g,
                      .blue  = ( double ) ped->gradient->color2.b,
                      .alpha = ( double ) 1.0f };

    ped->grp.gui->lock = 0x01;

    gtk_color_chooser_set_rgba ( ped->color1Button, &rgba1 );
    gtk_color_chooser_set_rgba ( ped->color2Button, &rgba2 );

    if ( ped->gradient->horizontal ) {
        gtk_toggle_button_set_active ( ped->horizontalToggle, TRUE  );
    } else {
        gtk_toggle_button_set_active ( ped->horizontalToggle, FALSE );
    }

    ped->grp.gui->lock = 0x00;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;
    void *imageData = ped->dummy.procedural.image.data;

    if ( imageData ) free ( imageData );

    if ( ped->pix ) g_object_unref ( G_OBJECT ( ped->pix ) );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) ped->gradient, 256, 256, 24, ped->bindGL );

    free ( ped );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;

    updateProceduralGradientEdit ( widget, ped );
}

/******************************************************************************/
GtkWidget* createProceduralGradientEdit ( GtkWidget             *parent,
                                          G3DUI                 *gui,
                                          G3DPROCEDURALGRADIENT *gradient,
                                          char                  *name,
                                          gint                   x,
                                          gint                   y,
                                          gint                   width,
                                          gint                   height,
                                          uint32_t               bindGL ) {
    PROCEDURALEDIT *ped = calloc ( 0x01, sizeof ( PROCEDURALEDIT ) );
    GdkRectangle gdkrec = { 0, 0, width, height };
    GtkWidget *frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    /*gtk_widget_size_allocate ( frm, &gdkrec );*/

    gtk_widget_set_size_request ( frm, gdkrec.width, gdkrec.height );

    /*** For some reason, GtkSpinButtons calls its callbacks ***/
    /*** when being realized. With this trick I bypass that. ***/
    /*** Callbacks will return prematurely if gui->lock == 0x01 ***/
    gui->lock = 0x01;

    ped->grp.gui   = gui;
    ped->gradient  = gradient;
    ped->bindGL    = bindGL;

    g3dproceduralgradient_copySettings ( ped->gradient, &ped->dummy );

    ped->dummy.procedural.getColor = gradient->procedural.getColor;

    g3dprocedural_fill ( &ped->dummy.procedural, 256, 256, 24, 0x00 );

    ped->pix = gdk_pixbuf_new_from_data ( ped->dummy.procedural.image.data,
                                          GDK_COLORSPACE_RGB,
                                          FALSE,
                                          0x08,
                                          ped->dummy.procedural.image.width,
                                          ped->dummy.procedural.image.height,
                                          ped->dummy.procedural.image.bytesPerLine,
                                          NULL, NULL );

    ped->areaWidget       = createDrawingArea ( frm, ped, "preview" , 192,   0, 256, 256, previewCbk          );
                            createSimpleLabel ( frm, ped, COLOR1    ,   0,   0,  96,  24                      );
    ped->color1Button     = createColorButton ( frm, ped, COLOR1    ,  96,   0,  96,  24, color1Cbk           );
                            createSimpleLabel ( frm, ped, COLOR2    ,   0,  24,  96,  24                      );
    ped->color2Button     = createColorButton ( frm, ped, COLOR2    ,  96,  24,  96,  24, color2Cbk           );
    ped->horizontalToggle = createToggleLabel ( frm, ped, HORIZONTAL,   0,  48, 192,  18, horizontalToggleCbk );

    gui->lock = 0x00;

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), ped );
    g_signal_connect ( G_OBJECT (frm), "destroy", G_CALLBACK (Destroy), ped );

    gtk_container_add ( GTK_CONTAINER(parent), frm );

    gtk_widget_show_all ( frm );


    return frm;
}
