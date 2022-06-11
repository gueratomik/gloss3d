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
    G3DPROCEDURALBRICK *brick;
    GdkPixbuf          *pix;
    GtkWidget          *areaWidget;
    uint32_t            bindGL;
    G3DPROCEDURALBRICK  dummy; /* Stores the temp image at the correct size */
} PROCEDURALEDIT;

/******************************************************************************/
#define BRICKCOLOR      "brickColor"
#define SPACINGCOLOR    "spacingColor"
#define NBLINES         "nbLines"
#define NBBRICKSPERLINE "nbBricksPerLine"
#define USPACING        "uspacing"
#define VSPACING        "vspacing"

/******************************************************************************/
static void spacingColorCbk ( GtkWidget *widget, gpointer user_data ) {
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER ( widget );
    GdkRGBA color;

    /*** prevent a loop ***/
    if ( ped->gui->lock ) return;

    gtk_color_chooser_get_rgba ( ccr, &color );

    ped->brick->spacingColor.r = color.red;
    ped->brick->spacingColor.g = color.green;
    ped->brick->spacingColor.b = color.blue;
    ped->brick->spacingColor.a = color.alpha;

    g3dproceduralbrick_copySettings ( ped->brick, &ped->dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &ped->dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( ped->areaWidget );

    g3dui_updateSelectedMaterialPreview ( ped->gui );
}

/******************************************************************************/
static void brickColorCbk ( GtkWidget *widget, gpointer user_data ) {
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER ( widget );
    GdkRGBA color;

    /*** prevent a loop ***/
    if ( ped->gui->lock ) return;

    gtk_color_chooser_get_rgba ( ccr, &color );

    ped->brick->brickColor.r = color.red;
    ped->brick->brickColor.g = color.green;
    ped->brick->brickColor.b = color.blue;
    ped->brick->brickColor.a = color.alpha;

    g3dproceduralbrick_copySettings ( ped->brick, &ped->dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &ped->dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( ped->areaWidget );

    g3dui_updateSelectedMaterialPreview ( ped->gui );
}

/******************************************************************************/
static void nbBricksPerLineCbk ( GtkWidget *widget, gpointer user_data ) {
    int nbBricksPerLine = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON ( widget ) );
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;

    /*** prevent a loop ***/
    if ( ped->gui->lock ) return;

    ped->brick->nbBricksPerLine = nbBricksPerLine;

    g3dproceduralbrick_copySettings ( ped->brick, &ped->dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &ped->dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( ped->areaWidget );

    g3dui_updateSelectedMaterialPreview ( ped->gui );
}

/******************************************************************************/
static void nbLinesCbk ( GtkWidget *widget, gpointer user_data ) {
    int nbLines = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON ( widget ) );
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;

    /*** prevent a loop ***/
    if ( ped->gui->lock ) return;

    ped->brick->nbLines = nbLines;

    g3dproceduralbrick_copySettings ( ped->brick, &ped->dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &ped->dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( ped->areaWidget );

    g3dui_updateSelectedMaterialPreview ( ped->gui );
}

/******************************************************************************/
static void uspacingCbk ( GtkWidget *widget, gpointer user_data ) {
    float uspacing = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON ( widget ) );
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;

    /*** prevent a loop ***/
    if ( ped->gui->lock ) return;

    ped->brick->uspacing = uspacing;

    g3dproceduralbrick_copySettings ( ped->brick, &ped->dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &ped->dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( ped->areaWidget );

    g3dui_updateSelectedMaterialPreview ( ped->gui );
}

/******************************************************************************/
static void vspacingCbk ( GtkWidget *widget, gpointer user_data ) {
    float vspacing = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON ( widget ) );
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;

    /*** prevent a loop ***/
    if ( ped->gui->lock ) return;

    ped->brick->vspacing = vspacing;

    g3dproceduralbrick_copySettings ( ped->brick, &ped->dummy );

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
static void updateProceduralBrickEdit ( GtkWidget *widget, PROCEDURALEDIT *ped ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    ped->gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( GTK_IS_COLOR_BUTTON ( child ) ) {
            GtkColorChooser *ccr = GTK_COLOR_CHOOSER(child);

            if ( strcmp ( child_name, BRICKCOLOR ) == 0x00 ) {
                GdkRGBA rgba = { .red   = ( float ) ped->brick->brickColor.r,
                                 .green = ( float ) ped->brick->brickColor.g,
                                 .blue  = ( float ) ped->brick->brickColor.b,
                                 .alpha = ( float ) 1.0f };

                gtk_color_chooser_set_rgba ( ccr, &rgba );
            }

            if ( strcmp ( child_name, SPACINGCOLOR ) == 0x00 ) {
                GdkRGBA rgba = { .red   = ( float ) ped->brick->spacingColor.r,
                                 .green = ( float ) ped->brick->spacingColor.g,
                                 .blue  = ( float ) ped->brick->spacingColor.b,
                                 .alpha = ( float ) 1.0f };

                gtk_color_chooser_set_rgba ( ccr, &rgba );
            }
        }

        if ( GTK_IS_SPIN_BUTTON ( child ) ) {
            GtkSpinButton *sbn = GTK_SPIN_BUTTON(child);

            if ( strcmp ( child_name, NBLINES ) == 0x00 ) {
                gtk_spin_button_set_value ( sbn, ped->brick->nbLines );
            }

            if ( strcmp ( child_name, NBBRICKSPERLINE ) == 0x00 ) {
                gtk_spin_button_set_value ( sbn, ped->brick->nbBricksPerLine );
            }

            if ( strcmp ( child_name, USPACING ) == 0x00 ) {
                gtk_spin_button_set_value ( sbn, ped->brick->uspacing );
            }

            if ( strcmp ( child_name, VSPACING ) == 0x00 ) {
                gtk_spin_button_set_value ( sbn, ped->brick->vspacing );
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

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) ped->brick, 256, 256, 24, ped->bindGL );

    free ( ped );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;

    updateProceduralBrickEdit ( widget, ped );
}

/******************************************************************************/
GtkWidget* createProceduralBrickEdit ( GtkWidget           *parent,
                                       G3DUI               *gui,
                                       G3DPROCEDURALBRICK *brick,
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
    ped->brick  = brick;
    ped->bindGL = bindGL;

    g3dproceduralbrick_copySettings ( ped->brick, &ped->dummy );

    ped->dummy.procedural.getColor = brick->procedural.getColor;

    g3dprocedural_fill ( &ped->dummy.procedural, 256, 256, 24, 0x00 );

    ped->pix = gdk_pixbuf_new_from_data ( ped->dummy.procedural.image.data,
                                          GDK_COLORSPACE_RGB,
                                          FALSE,
                                          0x08,
                                          ped->dummy.procedural.image.width,
                                          ped->dummy.procedural.image.height,
                                          ped->dummy.procedural.image.bytesPerLine,
                                          NULL, NULL );


    ped->areaWidget = createDrawingArea ( frm, ped, "preview"      ,        192,   0, 256, 256, previewCbk         );

                      createSimpleLabel ( frm, ped, BRICKCOLOR     ,          0,   0,  96,  24                     );
                      createColorButton ( frm, ped, BRICKCOLOR     ,         96,   0,  96,  24, brickColorCbk      );
                      createSimpleLabel ( frm, ped, SPACINGCOLOR   ,          0,  24,  96,  24                     );
                      createColorButton ( frm, ped, SPACINGCOLOR   ,         96,  24,  96,  24, spacingColorCbk    );
                      createIntegerText ( frm, ped, NBLINES        ,  0, 100, 0,  48,  96,  48, nbLinesCbk         );
                      createIntegerText ( frm, ped, NBBRICKSPERLINE,  0, 100, 0,  72,  96,  48, nbBricksPerLineCbk );
                      createFloatText   ( frm, ped, USPACING       ,  0, 100, 0,  96,  48,  48, uspacingCbk        );
                      createFloatText   ( frm, ped, VSPACING       ,  0, 100, 0, 120,  48,  48, vspacingCbk        );

    gui->lock = 0x00;

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), ped );
    g_signal_connect ( G_OBJECT (frm), "destroy", G_CALLBACK (Destroy), ped );

    gtk_container_add ( GTK_CONTAINER(parent), frm );

    gtk_widget_show_all ( frm );


    return frm;
}
