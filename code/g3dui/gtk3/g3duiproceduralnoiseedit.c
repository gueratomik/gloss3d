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
typedef struct _PAIREEDIT {
    uint32_t   id;
    GtkWidget *fixedWidget;
    GtkWidget *thresholdWidget;
    GtkWidget *color1Widget;
    GtkWidget *color2Widget;
} PAIREEDIT;

typedef struct _PROCEDURALEDIT {
    G3DUI              *gui;
    G3DPROCEDURALNOISE *noise;
    GdkPixbuf          *pix;
    PAIREEDIT           pair[MAXNOISECOLORS];
    GtkWidget          *mainFrame;
    GtkWidget          *areaWidget;
    uint32_t            bindGL;
    G3DPROCEDURALNOISE  dummy; /* Stores the temp image at the correct size */
} PROCEDURALEDIT;

/******************************************************************************/
typedef struct _PAIRCOLOR {
    PROCEDURALEDIT *ped;
    uint32_t        pairID;
    uint32_t        colorID;
} PAIRCOLOR;

/******************************************************************************/
static void updateProceduralNoiseEdit ( GtkWidget      *widget, 
                                        PROCEDURALEDIT *ped );

/******************************************************************************/
static PAIRCOLOR *paircolor_new ( PROCEDURALEDIT *ped,
                                 uint32_t        pairID,
                                 uint32_t        colorID ) {
    PAIRCOLOR *pc = ( PAIRCOLOR * ) calloc ( 0x01, sizeof ( PAIRCOLOR ) );

    if ( pc == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n");

        return NULL;
    }

    pc->ped     = ped;
    pc->pairID  = pairID;
    pc->colorID = colorID;

    return pc;
}

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
static void thresholdCbk ( GtkWidget *widget, gpointer user_data ) {
    PAIRCOLOR *pc  = ( PAIRCOLOR * ) user_data;
    float str = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevent a loop ***/
    if ( pc->ped->gui->lock ) return;

    pc->ped->noise->threshold[pc->pairID] = str;

    g3dproceduralnoise_copySettings ( pc->ped->noise, &pc->ped->dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &pc->ped->dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( pc->ped->areaWidget );

    g3dui_updateSelectedMaterialPreview ( pc->ped->gui );
}

/******************************************************************************/
static void colorCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkColorChooser *ccr = GTK_COLOR_CHOOSER ( widget );
    PAIRCOLOR *pc  = ( PAIRCOLOR * ) user_data;
    GdkRGBA color;

    /*** prevent a loop ***/
    if ( pc->ped->gui->lock ) return;

    gtk_color_chooser_get_rgba ( ccr, &color );

    pc->ped->noise->colorPair[pc->pairID][pc->colorID].r = color.red;
    pc->ped->noise->colorPair[pc->pairID][pc->colorID].g = color.green;
    pc->ped->noise->colorPair[pc->pairID][pc->colorID].b = color.blue;
    pc->ped->noise->colorPair[pc->pairID][pc->colorID].a = color.alpha;

    g3dproceduralnoise_copySettings ( pc->ped->noise, &pc->ped->dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &pc->ped->dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( pc->ped->areaWidget );

    g3dui_updateSelectedMaterialPreview ( pc->ped->gui );
}

/******************************************************************************/
static void nbpairsCbk ( GtkWidget *widget, gpointer user_data ) {
    int nbPairs = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON ( widget ) );
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;

    /*** prevent a loop ***/
    if ( ped->gui->lock ) return;

    ped->noise->nbColors = nbPairs;

    g3dproceduralnoise_copySettings ( ped->noise, &ped->dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &ped->dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( ped->areaWidget );

    g3dui_updateSelectedMaterialPreview ( ped->gui );

    ped->gui->lock = 0x01;

    updateProceduralNoiseEdit ( ped->mainFrame, ped );

    ped->gui->lock = 0x00;
}

/******************************************************************************/
static void octavesCbk ( GtkWidget *widget, gpointer user_data ) {
    int nbOctaves = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON ( widget ) );
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;

    /*** prevent a loop ***/
    if ( ped->gui->lock ) return;

    ped->noise->nbOctaves = nbOctaves;

    g3dproceduralnoise_copySettings ( ped->noise, &ped->dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &ped->dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( ped->areaWidget );

    g3dui_updateSelectedMaterialPreview ( ped->gui );
}

/******************************************************************************/
static void xGradientsCbk ( GtkWidget *widget, gpointer user_data ) {
    int nbGradientX = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON ( widget ) );
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;

    /*** prevent a loop ***/
    if ( ped->gui->lock ) return;

    ped->noise->nbGradientX = nbGradientX;

    g3dproceduralnoise_copySettings ( ped->noise, &ped->dummy );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) &ped->dummy, 256, 256, 24, 0x00 );

    gtk_widget_queue_draw ( ped->areaWidget );

    g3dui_updateSelectedMaterialPreview ( ped->gui );
}

/******************************************************************************/
static void yGradientsCbk ( GtkWidget *widget, gpointer user_data ) {
    int nbGradientY = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON ( widget ) );
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;

    /*** prevent a loop ***/
    if ( ped->gui->lock ) return;

    ped->noise->nbGradientY = nbGradientY;

    g3dproceduralnoise_copySettings ( ped->noise, &ped->dummy );

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
static void updateProceduralNoisePair ( PAIREEDIT      *pair, 
                                        PROCEDURALEDIT *ped ) {
    uint32_t pid = pair->id;
    GdkRGBA rgba1 = { .red   = ( double ) ped->noise->colorPair[pid][0x00].r,
                      .green = ( double ) ped->noise->colorPair[pid][0x00].g,
                      .blue  = ( double ) ped->noise->colorPair[pid][0x00].b,
                      .alpha = ( double ) 1.0f };
    GdkRGBA rgba2 = { .red   = ( double ) ped->noise->colorPair[pid][0x01].r,
                      .green = ( double ) ped->noise->colorPair[pid][0x01].g,
                      .blue  = ( double ) ped->noise->colorPair[pid][0x01].b,
                      .alpha = ( double ) 1.0f };

    ped->gui->lock = 0x01;

    gtk_color_chooser_set_rgba ( pair->color1Widget, &rgba1 );
    gtk_color_chooser_set_rgba ( pair->color2Widget, &rgba2 );

    gtk_spin_button_set_value ( pair->thresholdWidget, 
                                ped->noise->threshold[pid] );

    if ( pid < ped->noise->nbColors ) {
        gtk_widget_show ( pair->fixedWidget  );
    } else {
        gtk_widget_hide ( pair->fixedWidget );
    }

    ped->gui->lock = 0x00;
}

/******************************************************************************/
static void updateProceduralNoiseEdit ( GtkWidget      *widget, 
                                        PROCEDURALEDIT *ped ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    uint32_t i;

    ped->gui->lock = 0x01;

    for ( i = 0x00; i <  MAXNOISECOLORS; i++ ) {
        updateProceduralNoisePair ( &ped->pair[i], ped );
    }

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( GTK_IS_SPIN_BUTTON ( child ) ) {
            GtkSpinButton *sbn = GTK_SPIN_BUTTON(child);
            uint32_t i;

            if ( strcmp ( child_name, UGRADIENTS ) == 0x00 ) {
                gtk_spin_button_set_value ( sbn, ped->noise->nbGradientX );
            }

            if ( strcmp ( child_name, VGRADIENTS ) == 0x00 ) {
                gtk_spin_button_set_value ( sbn, ped->noise->nbGradientY );
            }

            if ( strcmp ( child_name, OCTAVES ) == 0x00 ) {
                gtk_spin_button_set_value ( sbn, ped->noise->nbOctaves );
            }

            if ( strcmp ( child_name, NBPAIRS ) == 0x00 ) {
                gtk_spin_button_set_value ( sbn, ped->noise->nbColors );
            }
        }

        children =  g_list_next ( children );
    }

    ped->gui->lock = 0x00;
}

/******************************************************************************/
static void DestroyPairColor ( GtkWidget *widget, gpointer user_data ) {
    PAIRCOLOR *pc  = ( PAIRCOLOR * ) user_data;

    free ( pc );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;
    void *imageData = ped->dummy.procedural.image.data;

    if ( imageData ) free ( imageData );

    if ( ped->pix ) g_object_unref ( G_OBJECT ( ped->pix ) );

    g3dprocedural_fill ( ( G3DPROCEDURAL * ) ped->noise, 256, 256, 24, ped->bindGL );

    free ( ped );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    PROCEDURALEDIT *ped = ( PROCEDURALEDIT * ) user_data;

    updateProceduralNoiseEdit ( widget, ped );
}

/******************************************************************************/
GtkWidget* createProceduralNoiseEdit ( GtkWidget           *parent,
                                       G3DUI               *gui,
                                       G3DPROCEDURALNOISE *noise,
                                       char               *name,
                                       gint                x,
                                       gint                y,
                                       gint                width,
                                       gint                height,
                                       uint32_t            bindGL ) {
    PROCEDURALEDIT *ped = calloc ( 0x01, sizeof ( PROCEDURALEDIT ) );
    GdkRectangle gdkrec = { 0, 0, width, height };
    GtkWidget *frm = gtk_fixed_new ( );
    int32_t pcy = 96;
    uint32_t i;

    gtk_widget_set_name ( frm, name );

    /*gtk_widget_size_allocate ( frm, &gdkrec );*/

    gtk_widget_set_size_request ( frm, gdkrec.width, gdkrec.height );

    /*** For some reason, GtkSpinButtons calls its callbacks ***/
    /*** when being realized. With this trick I bypass that. ***/
    /*** Callbacks will return prematurely if gui->lock == 0x01 ***/
    gui->lock = 0x01;

    ped->gui    = gui;
    ped->noise  = noise;
    ped->bindGL = bindGL;

    ped->mainFrame = frm;

    g3dproceduralnoise_copySettings ( ped->noise, &ped->dummy );

    ped->dummy.procedural.getColor = noise->procedural.getColor;

    g3dprocedural_fill ( &ped->dummy.procedural, 256, 256, 24, 0x00 );

    ped->pix = gdk_pixbuf_new_from_data ( ped->dummy.procedural.image.data,
                                          GDK_COLORSPACE_RGB,
                                          FALSE,
                                          0x08,
                                          ped->dummy.procedural.image.width,
                                          ped->dummy.procedural.image.height,
                                          ped->dummy.procedural.image.bytesPerLine,
                                          NULL, NULL );


    ped->areaWidget = createDrawingArea ( frm, ped, "preview" , 256, 0, 256, 256, previewCbk    );

    createIntegerText ( frm, ped, UGRADIENTS,  1, 16, 0,  0, 96, 48, xGradientsCbk );
    createIntegerText ( frm, ped, VGRADIENTS,  1, 16, 0, 24, 96, 48, yGradientsCbk );

    createIntegerText ( frm, ped, OCTAVES   ,  1, 10, 0, 48, 96, 48, octavesCbk );
    createIntegerText ( frm, ped, NBPAIRS   ,  1, MAXNOISECOLORS, 0, 72, 96, 48, nbpairsCbk );

    for ( i = 0x00; i < MAXNOISECOLORS; i++ ) {
        PAIRCOLOR *pc1 = paircolor_new ( ped, i, 0 ),
                  *pc2 = paircolor_new ( ped, i, 1 );
        GtkWidget *fix = gtk_fixed_new ( );

        gtk_widget_set_name ( fix, NOISEPAIR );

        gtk_fixed_put ( GTK_FIXED(frm), fix, 0, pcy );

        ped->pair[i].fixedWidget = fix;
        ped->pair[i].id = i;

                                    createSimpleLabel ( fix, gui, "Pair" ,   0, 0, 32, 24           );
                                    createSimpleLabel ( fix, gui, "Col 1",  32, 0, 32, 24           );
        ped->pair[i].color1Widget = createColorButton ( fix, pc1, "Col 1",  64, 0, 32, 24, colorCbk );
                                    createSimpleLabel ( fix, gui, "Col 2",  96, 0, 32, 24 );
        ped->pair[i].color2Widget = createColorButton ( fix, pc2, "Col 2", 128, 0, 32, 24, colorCbk );

        ped->pair[i].thresholdWidget = createFloatText ( fix, pc1, "Threshold", 0.0f, 1.0f, 0, 24, 64, 24, thresholdCbk );

        g_signal_connect ( G_OBJECT (ped->pair[i].color1Widget), "destroy", G_CALLBACK (DestroyPairColor), pc1 );
        g_signal_connect ( G_OBJECT (ped->pair[i].color2Widget), "destroy", G_CALLBACK (DestroyPairColor), pc2 );

        pcy += 48;
    }

    gui->lock = 0x00;

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), ped );
    g_signal_connect ( G_OBJECT (frm), "destroy", G_CALLBACK (Destroy), ped );

    gtk_container_add ( GTK_CONTAINER(parent), frm );

    gtk_widget_show_all ( frm );

    return frm;
}
