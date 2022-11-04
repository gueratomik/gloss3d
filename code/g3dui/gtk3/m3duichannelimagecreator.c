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

#define WIDTHBUTTONNAME  "Width"
#define HEIGHTBUTTONNAME "Height"

/******************************************************************************/
static GTK3M3DUICHANNELIMAGECREATOR *gtk3_m3duichannelimagecreator_new ( GTK3M3DUI  *gtk3mui,
                                                                         G3DCHANNEL *chn,
                                                                         uint32_t    resize ) {
    GTK3M3DUICHANNELIMAGECREATOR *gtk3cic = calloc ( 0x01, sizeof ( GTK3M3DUICHANNELIMAGECREATOR ) );

    if ( gtk3cic == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3cic->core.mui    = ( M3DUI * ) gtk3mui;
    gtk3cic->core.chn    = ( M3DUI * ) chn;
    gtk3cic->core.resize = ( M3DUI * ) resize;


    return gtk3cic;
}

/******************************************************************************/
static void updateInfo ( GTK3M3DUICHANNELIMAGECREATOR *gtk3cic ) {
    G3DUI *gui = gtk3cic->core.mui->gui;
    uint32_t width  = gtk_spin_button_get_value ( gtk3cic->widthEntry  ),
             height = gtk_spin_button_get_value ( gtk3cic->heightEntry );
    char text[0x100];

    width  = g3dcore_getNextPowerOfTwo ( width  );
    height = g3dcore_getNextPowerOfTwo ( height );

    snprintf ( text,
               0x100,
               "The final size will be power-of-2 compliant: %dx%d\n",
               width,
               height );

    /*** prevents a loop ***/
    gui->lock = 0x01;

    gtk_label_set_text ( gtk3cic->infoLabel, text );

    gui->lock = 0x00;
}

/******************************************************************************/
static void widthCbk ( GtkWidget *widget, 
                       gpointer   user_data ) {
    GTK3M3DUICHANNELIMAGECREATOR *gtk3cic = ( GTK3M3DUICHANNELIMAGECREATOR * ) user_data;
    uint32_t width = ( uint32_t ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = gtk3cic->core.mui->gui;

    if ( gui->lock ) return;

    updateInfo ( gtk3cic );
}

/******************************************************************************/
static void heightCbk ( GtkWidget *widget, 
                        gpointer   user_data ) {
    GTK3M3DUICHANNELIMAGECREATOR *gtk3cic = ( GTK3M3DUICHANNELIMAGECREATOR * ) user_data;
    uint32_t height = ( uint32_t ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = gtk3cic->core.mui->gui;

    if ( gui->lock ) return;

    updateInfo ( gtk3cic );
}

/******************************************************************************/
static void okCbk ( GtkWidget *widget, 
                    gpointer   user_data ) {
    GTK3M3DUICHANNELIMAGECREATOR *gtk3cic = ( GTK3M3DUICHANNELIMAGECREATOR * ) user_data;
    uint32_t width  = gtk_spin_button_get_value ( gtk3cic->widthEntry  ),
             height = gtk_spin_button_get_value ( gtk3cic->heightEntry );
    G3DUI *gui = gtk3cic->core.mui->gui;
    G3DCHANNEL *chn = gtk3cic->core.chn;

    if ( gtk3cic->core.resize ) {
        if ( chn->flags & USEIMAGECOLOR ) {
            if ( chn->image ) {
                g3dimage_resize ( chn->image, 
                                  g3dcore_getNextPowerOfTwo ( width  ), 
                                  g3dcore_getNextPowerOfTwo ( height ) );
            }
        }
    } else {
        if ( chn->image ) {
            g3dscene_unregisterImage ( gui->sce, chn->image );
        }
    /*** note: image is binded to GL lib in order to view it in the editor ***/
        chn->image = g3dimage_new ( g3dcore_getNextPowerOfTwo ( width  ), 
                                    g3dcore_getNextPowerOfTwo ( height ), 
                                    24, 
                                    0x01 );

        g3dscene_registerImage ( gui->sce, chn->image );
    }

    g3dchannel_enableImageColor ( chn );

    gtk3_interpretUIReturnFlags ( ( GTK3G3DUI * ) gui, UPDATECURRENTMATERIAL |
                                                       UPDATECURRENTMATERIALPREVIEW |
                                                       REDRAWVIEW );  

    gtk_widget_destroy ( gtk_widget_get_ancestor ( widget, GTK_TYPE_WINDOW ) );
}

/******************************************************************************/
void gtk3_m3duichannelimagecreator_update ( GTK3M3DUICHANNELIMAGECREATOR *gtk3cic ) {
    G3DUI *gui = gtk3cic->core.mui->gui;

    gui->lock = 0x01;

    if ( gtk3cic->core.resize ) {
        if ( gtk3cic->core.chn->flags & USEIMAGECOLOR ) {
            if ( gtk3cic->core.chn->image ) {
                gtk_spin_button_set_value ( gtk3cic->widthEntry , gtk3cic->core.chn->image->width  );
                gtk_spin_button_set_value ( gtk3cic->heightEntry, gtk3cic->core.chn->image->height );
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3M3DUICHANNELIMAGECREATOR *gtk3cic = ( GTK3M3DUICHANNELIMAGECREATOR * ) user_data;

    free ( gtk3cic );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    GTK3M3DUICHANNELIMAGECREATOR *gtk3cic = ( GTK3M3DUICHANNELIMAGECREATOR * ) user_data;

    gtk3_m3duichannelimagecreator_update ( gtk3cic );

    updateInfo ( gtk3cic );
}

/******************************************************************************/
GTK3M3DUICHANNELIMAGECREATOR *gtk3_m3duichannelimagecreator_create ( GtkWidget  *parent,
                                                                     GTK3M3DUI  *gtk3mui,
                                                                     G3DCHANNEL *chn,
                                                                     uint32_t    resize,
                                                                     char       *name ) {
    GTK3M3DUICHANNELIMAGECREATOR *gtk3cic = 
                                   gtk3_m3duichannelimagecreator_new ( gtk3mui,
                                                                       chn,
                                                                       resize );
    GtkWidget *fixed = ui_gtk_fixed_new ( CLASS_MAIN );

    gtk3cic->fixed = fixed;

    gtk_widget_set_size_request ( fixed, 200, 72 );

    gtk3cic->widthEntry  = ui_createIntegerText ( fixed,
                                                  gtk3cic,
                                                  WIDTHBUTTONNAME,
                                                  CLASS_MAIN,
                                                  0, 8192,
                                                  8,  0, 96, 96, 20,
                                                  widthCbk );

    gtk3cic->heightEntry = ui_createIntegerText ( fixed,
                                                  gtk3cic,
                                                  HEIGHTBUTTONNAME,
                                                  CLASS_MAIN,
                                                  0, 8192,
                                                  8, 24, 96, 96, 20,
                                                  heightCbk );

    gtk3cic->infoLabel   = ui_createSimpleLabel ( fixed,
                                                  gtk3cic,
                                                  "",
                                                  CLASS_MAIN,
                                                  0, 48, 128, 20 );

                           ui_createPushButton ( fixed,
                                                 gtk3cic,
                                                 "OK",
                                                 CLASS_MAIN,
                                                 60, 72, 32, 20,
                                                 okCbk );

    gtk3cic->core.mui->gui->lock = 0x01;

    gtk_spin_button_set_value ( gtk3cic->widthEntry , 512 );
    gtk_spin_button_set_value ( gtk3cic->heightEntry, 512 );

    gtk3cic->core.mui->gui->lock = 0x00;

    g_signal_connect ( G_OBJECT (fixed), "realize", G_CALLBACK (Realize), gtk3cic );
    g_signal_connect ( G_OBJECT (fixed), "destroy", G_CALLBACK (Destroy), gtk3cic );


    gtk_widget_show_all ( fixed );


    return gtk3cic;
}
