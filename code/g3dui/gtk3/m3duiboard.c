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
#include <xpm/translate_view.xpm>
#include <xpm/rotate_view.xpm>
#include <xpm/maximize_view.xpm>
#include <xpm/zoom_view.xpm>

/******************************************************************************/
static GTK3M3DUIBOARD *gtk3_m3duiboard_new ( GTK3M3DUI *gtk3mui ) {
    uint32_t memSize =  sizeof ( GTK3M3DUIBOARD );
    GTK3M3DUIBOARD *gtk3board = ( GTK3M3DUIBOARD * ) calloc ( 0x01, memSize );

    if ( gtk3board == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;

    }

    gtk3board->core.mui = ( M3DUI * ) gtk3mui;


    return gtk3board;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, 
                      gpointer   user_data ) {
    GTK3M3DUIBOARD *gtk3board = ( GTK3M3DUIBOARD * ) user_data;

    free ( gtk3board );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, 
                      gpointer   user_data ) {
    GTK3M3DUIBOARD *gtk3brd = ( GTK3M3DUIBOARD * ) user_data;
}

/******************************************************************************/
static void gtk3_m3duiboard_createMousetoolEdit ( GTK3M3DUIBOARD *gtk3board ) {
    GTK3M3DUI *gtk3mui = ( GTK3M3DUI * ) gtk3board->core.mui;
    GTK3M3DUIMOUSETOOLEDIT *gtk3tooledit;

    gtk3tooledit = gtk3_m3duimousetooledit_create ( gtk3board->layout,
                                                    gtk3mui,
                                                    "Current Tool" );

    gtk_layout_put ( GTK_LAYOUT(gtk3board->layout), 
                     GTK_WIDGET(gtk3tooledit->scrolled), 0, 0 );

    gtk3board->core.tooledit = ( M3DUIMOUSETOOLEDIT * ) gtk3tooledit;
}

/******************************************************************************/
static void gtk3_m3duiboard_createColorPicker ( GTK3M3DUIBOARD *gtk3board ) {
    GTK3M3DUI *gtk3mui = ( GTK3M3DUI * ) gtk3board->core.mui;
    GTK3M3DUICOLORPICKER *gtk3colpick;

    gtk3colpick = gtk3_m3duicolorpicker_create ( gtk3board->layout,
                                                 gtk3mui,
                                                 "Color Picker" );

    gtk_layout_put ( GTK_LAYOUT(gtk3board->layout), 
                     GTK_WIDGET(gtk3colpick->area), 0, 0 );

    gtk3board->core.colpick = ( M3DUICOLORPICKER * ) gtk3colpick;
}

/******************************************************************************/
static void gtk3_m3duiboard_createPatternList ( GTK3M3DUIBOARD *gtk3board ) {
    GTK3M3DUI *gtk3mui = ( GTK3M3DUI * ) gtk3board->core.mui;
    GTK3M3DUIPATTERNLIST *gtk3patlist;

    gtk3patlist = gtk3_m3duipatternlist_create ( gtk3board->layout,
                                                 gtk3mui,
                                                 "Pattern List" );

    gtk_layout_put ( GTK_LAYOUT(gtk3board->layout), 
                     GTK_WIDGET(gtk3patlist->scrolled), 0, 0 );

    gtk3board->core.patlist = ( M3DUIPATTERNLIST * ) gtk3patlist;
}

/******************************************************************************/
void gtk3_m3duiboard_resize ( GTK3M3DUIBOARD *gtk3board,
                              uint32_t        width,
                              uint32_t        height ) {
    GTK3M3DUIPATTERNLIST   *gtk3patlist  = ( GTK3M3DUIPATTERNLIST   * ) gtk3board->core.patlist;
    GTK3M3DUICOLORPICKER   *gtk3colpick  = ( GTK3M3DUICOLORPICKER   * ) gtk3board->core.colpick;
    GTK3M3DUIMOUSETOOLEDIT *gtk3tooledit = ( GTK3M3DUIMOUSETOOLEDIT * ) gtk3board->core.tooledit;

    GdkRectangle gdkrec;

    m3duiboard_resize ( &gtk3board->core,
                         width,
                         height );

    /********************************/

    if ( gtk3patlist ) {
        g3duirectangle_toGdkRectangle ( &gtk3board->core.patrec, &gdkrec );

        gtk_layout_move ( gtk3board->layout,
                          GTK_WIDGET(gtk3patlist->scrolled),
                          gdkrec.x,
                          gdkrec.y );

        gtk_widget_set_size_request ( GTK_WIDGET(gtk3patlist->scrolled),
                                      gdkrec.width,
                                      gdkrec.height );
    }

    /********************************/

    if ( gtk3colpick ) {
        g3duirectangle_toGdkRectangle ( &gtk3board->core.colrec, &gdkrec );

        gtk_layout_move ( gtk3board->layout,
                          GTK_WIDGET(gtk3colpick->area),
                          gdkrec.x,
                          gdkrec.y );

        gtk_widget_set_size_request ( GTK_WIDGET(gtk3colpick->area),
                                      gdkrec.width,
                                      gdkrec.height );
    }

    /*******************************/

    if ( gtk3tooledit ) {
        g3duirectangle_toGdkRectangle ( &gtk3board->core.toolrec, &gdkrec );

        gtk_layout_move ( gtk3board->layout,
                          GTK_WIDGET(gtk3tooledit->scrolled),
                          gdkrec.x,
                          gdkrec.y );

        gtk_widget_set_size_request ( GTK_WIDGET(gtk3tooledit->scrolled),
                                      gdkrec.width,
                                      gdkrec.height );
    }
}

/******************************************************************************/
GTK3M3DUIBOARD *gtk3_m3duiboard_create ( GtkWidget *parent,
                                         GTK3M3DUI *gtk3mui,
                                         char      *name ) {
    GTK3M3DUIBOARD *gtk3board = gtk3_m3duiboard_new ( gtk3mui );
    GtkLayout    *layout = ui_gtk_layout_new ( CLASS_MAIN, NULL, NULL );

    g_signal_connect ( G_OBJECT (layout), "realize", G_CALLBACK (Realize), gtk3board );
    g_signal_connect ( G_OBJECT (layout), "destroy", G_CALLBACK (Destroy), gtk3board );

    gtk3board->layout = layout;

    gtk3_m3duiboard_createPatternList   ( gtk3board );
    gtk3_m3duiboard_createColorPicker   ( gtk3board );
    gtk3_m3duiboard_createMousetoolEdit ( gtk3board );

    gtk_widget_show ( GTK_WIDGET(layout) );

    return gtk3board;
}




