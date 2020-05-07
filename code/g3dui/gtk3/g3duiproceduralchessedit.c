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
    G3DUI              *gui;
    G3DPROCEDURALCHESS *chess;
    GdkPixbuf          *pix;
    GtkWidget          *areaWidget;
    uint32_t            bindGL;
    G3DPROCEDURALCHESS  dummy; /* Stores the temp image at the correct size */
} PROCEDURALEDIT;

/******************************************************************************/
#define COLOR1     "Color 1"
#define COLOR2     "Color 2"
#define UDIVISIONS "U Divisions"
#define VDIVISIONS "V Divisions"

/******************************************************************************/
static void color1Cbk ( GtkWidget *widget, gpointer user_data ) {
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER ( widget );
    GdkRGBA color;

    /*** prevent a loop ***/
    if ( ped->gui->lock ) return;

    gtk_color_chooser_get_rgba ( ccr, &color );

    ped->chess->color1.r = color.red;
    ped->chess->color1.g = color.green;
    ped->chess->color1.b = color.blue;
    ped->chess->color1.a = color.alpha;

    g3dproceduralchess_copySettings ( ped->chess, &ped->dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &ped->dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( ped->areaWidget );

    g3dui_updateSelectedMaterialPreview ( ped->gui );
}

/******************************************************************************/
static void color2Cbk ( GtkWidget *widget, gpointer user_data ) {
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER ( widget );
    GdkRGBA color;

    /*** prevent a loop ***/
    if ( ped->gui->lock ) return;

    gtk_color_chooser_get_rgba ( ccr, &color );

    ped->chess->color2.r = color.red;
    ped->chess->color2.g = color.green;
    ped->chess->color2.b = color.blue;
    ped->chess->color2.a = color.alpha;

    g3dproceduralchess_copySettings ( ped->chess, &ped->dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &ped->dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( ped->areaWidget );

    g3dui_updateSelectedMaterialPreview ( ped->gui );
}

/******************************************************************************/
static void udivisionsCbk ( GtkWidget *widget, gpointer user_data ) {
    int udiv = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON ( widget ) );
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;

    /*** prevent a loop ***/
    if ( ped->gui->lock ) return;

    ped->chess->udiv = udiv;

    g3dproceduralchess_copySettings ( ped->chess, &ped->dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &ped->dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( ped->areaWidget );

    g3dui_updateSelectedMaterialPreview ( ped->gui );
}

/******************************************************************************/
static void vdivisionsCbk ( GtkWidget *widget, gpointer user_data ) {
    int vdiv = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON ( widget ) );
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;

    /*** prevent a loop ***/
    if ( ped->gui->lock ) return;

    ped->chess->vdiv = vdiv;

    g3dproceduralchess_copySettings ( ped->chess, &ped->dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &ped->dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( ped->areaWidget );

    g3dui_updateSelectedMaterialPreview ( ped->gui );
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
static void updateProceduralChessEdit ( GtkWidget *widget, PROCEDURALEDIT *ped ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    ped->gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( GTK_IS_COLOR_BUTTON ( child ) ) {
            GtkColorChooser *ccr = GTK_COLOR_CHOOSER(child);

            if ( strcmp ( child_name, COLOR1 ) == 0x00 ) {
                GdkRGBA rgba = { .red   = ( double ) ped->chess->color1.r,
                                 .green = ( double ) ped->chess->color1.g,
                                 .blue  = ( double ) ped->chess->color1.b,
                                 .alpha = ( double ) 1.0f };

                gtk_color_chooser_set_rgba ( ccr, &rgba );
            }

            if ( strcmp ( child_name, COLOR2 ) == 0x00 ) {
                GdkRGBA rgba = { .red   = ( double ) ped->chess->color2.r,
                                 .green = ( double ) ped->chess->color2.g,
                                 .blue  = ( double ) ped->chess->color2.b,
                                 .alpha = ( double ) 1.0f };

                gtk_color_chooser_set_rgba ( ccr, &rgba );
            }
        }

        if ( GTK_IS_SPIN_BUTTON ( child ) ) {
            GtkSpinButton *sbn = GTK_SPIN_BUTTON(child);

            if ( strcmp ( child_name, UDIVISIONS ) == 0x00 ) {
                gtk_spin_button_set_value ( sbn, ped->chess->udiv );
            }

            if ( strcmp ( child_name, VDIVISIONS ) == 0x00 ) {
                gtk_spin_button_set_value ( sbn, ped->chess->vdiv );
            }
        }

        children =  g_list_next ( children );
    }

    ped->gui->lock = 0x00;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;
    void *imageData = ped->dummy.procedural.image.data;

    if ( imageData ) free ( imageData );

    if ( ped->pix ) g_object_unref ( G_OBJECT ( ped->pix ) );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) ped->chess, 256, 256, 24, ped->bindGL );

    free ( ped );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;

    updateProceduralChessEdit ( widget, ped );
}

/******************************************************************************/
GtkWidget* createProceduralChessEdit ( GtkWidget           *parent,
                                       G3DUI               *gui,
                                       G3DPROCEDURALCHESS *chess,
                                       char               *name,
                                       gint                x,
                                       gint                y,
                                       gint                width,
                                       gint                height,
                                       uint32_t            bindGL ) {
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

    ped->gui    = gui;
    ped->chess  = chess;
    ped->bindGL = bindGL;

    g3dproceduralchess_copySettings ( ped->chess, &ped->dummy );

    ped->dummy.procedural.getColor = chess->procedural.getColor;

    g3dprocedural_fill ( &ped->dummy.procedural, 256, 256, 24, 0x00 );

    ped->pix = gdk_pixbuf_new_from_data ( ped->dummy.procedural.image.data,
                                          GDK_COLORSPACE_RGB,
                                          FALSE,
                                          0x08,
                                          ped->dummy.procedural.image.width,
                                          ped->dummy.procedural.image.height,
                                          ped->dummy.procedural.image.bytesPerLine,
                                          NULL, NULL );


    ped->areaWidget = createDrawingArea ( frm, ped, "preview" ,        192,   0, 256, 256, previewCbk    );

                      createSimpleLabel ( frm, ped, COLOR1    ,          0,   0,  96,  24                );
                      createColorButton ( frm, ped, COLOR1    ,         96,   0,  96,  24, color1Cbk     );
                      createSimpleLabel ( frm, ped, COLOR2    ,          0,  24,  96,  24                );
                      createColorButton ( frm, ped, COLOR2    ,         96,  24,  96,  24, color2Cbk     );
                      createIntegerText ( frm, ped, UDIVISIONS,  0, 100, 0,  48,  96,  48, udivisionsCbk         );
                      createIntegerText ( frm, ped, VDIVISIONS,  0, 100, 0,  72,  96,  48, vdivisionsCbk );

    gui->lock = 0x00;

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), ped );
    g_signal_connect ( G_OBJECT (frm), "destroy", G_CALLBACK (Destroy), ped );

    gtk_container_add ( GTK_CONTAINER(parent), frm );

    gtk_widget_show_all ( frm );


    return frm;
}
