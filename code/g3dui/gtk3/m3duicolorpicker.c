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
static GTK3M3DUICOLORPICKER *gtk3_m3duicolorpicker_new ( GTK3M3DUI *gtk3mui ) {
    uint32_t memSize =  sizeof ( GTK3M3DUICOLORPICKER );
    GTK3M3DUICOLORPICKER *gtk3colpick = ( GTK3M3DUICOLORPICKER * ) calloc ( 0x01, memSize );

    if ( gtk3colpick == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;

    }

    gtk3colpick->core.mui = ( M3DUI * ) gtk3mui;


    return gtk3colpick;
}

/******************************************************************************/
static void setForegroundColor ( GtkDialog *dialog,
                                 gint       response_id,
                                 gpointer   user_data ) {
    static GdkRGBA color;

    gtk_color_chooser_get_rgba ( GTK_COLOR_CHOOSER ( dialog ), &color );

    if ( response_id == GTK_RESPONSE_OK ) {
        M3DSYSINFO *sysinfo = m3dsysinfo_get ( );
        uint32_t R = color.red   * 255.0f,
                 G = color.green * 255.0f,
                 B = color.blue  * 255.0f;

        sysinfo->fgcolor = ( R << 0x10 ) | ( G << 0x08 ) | B;
    }

    gtk_widget_destroy ( GTK_WIDGET ( dialog ) );
}

/******************************************************************************/
static void setBackgroundColor ( GtkDialog *dialog,
                                 gint       response_id,
                                 gpointer   user_data ) {
    static GdkRGBA color;

    gtk_color_chooser_get_rgba ( GTK_COLOR_CHOOSER ( dialog ), &color );

    if ( response_id == GTK_RESPONSE_OK ) {
        M3DSYSINFO *sysinfo = m3dsysinfo_get ( );
        uint32_t R = color.red   * 255.0f,
                 G = color.green * 255.0f,
                 B = color.blue  * 255.0f;

        sysinfo->bgcolor = ( R << 0x10 ) | ( G << 0x08 ) | B;
    }

    gtk_widget_destroy ( GTK_WIDGET ( dialog ) );
}

/******************************************************************************/
static void Input ( GtkWidget *widget, 
                    GdkEvent  *gdkev, 
                    gpointer   user_data ) {
    GTK3M3DUICOLORPICKER *gtk3colpick = ( GTK3M3DUICOLORPICKER * ) user_data;
    M3DSYSINFO *sysinfo = m3dsysinfo_get ( );

    switch ( gdkev->type ) {
        case GDK_BUTTON_RELEASE : {
            GdkEventButton *bev = ( GdkEventButton * ) gdkev;

            if ( ( bev->x >= gtk3colpick->core.fgcolrec.x ) &&
                 ( bev->y >= gtk3colpick->core.fgcolrec.y ) &&
                 ( bev->x <= ( gtk3colpick->core.fgcolrec.x + 
                               gtk3colpick->core.fgcolrec.width  ) ) &&
                 ( bev->y <= ( gtk3colpick->core.fgcolrec.y + 
                               gtk3colpick->core.fgcolrec.height ) ) ) {
                GtkWidget *dialog = gtk_color_chooser_dialog_new ( "Choose foreground color", NULL );

                g_signal_connect ( G_OBJECT (dialog), "response",
                                   G_CALLBACK (setForegroundColor) , NULL );

                gtk_widget_show ( dialog );
            } else {
                if ( ( bev->x >= gtk3colpick->core.bgcolrec.x ) &&
                     ( bev->y >= gtk3colpick->core.bgcolrec.y ) &&
                     ( bev->x <= ( gtk3colpick->core.bgcolrec.x + 
                                   gtk3colpick->core.bgcolrec.width  ) ) &&
                     ( bev->y <= ( gtk3colpick->core.bgcolrec.y + 
                                   gtk3colpick->core.bgcolrec.height ) ) ) {
                    GtkWidget *dialog = gtk_color_chooser_dialog_new ( "Choose background color", NULL );

                    g_signal_connect ( G_OBJECT (dialog), "response",
                                       G_CALLBACK (setBackgroundColor) , NULL );

                    gtk_widget_show ( dialog );
                }
            }

            if ( ( bev->x >= gtk3colpick->core.bwcolrec.x ) &&
                 ( bev->y >= gtk3colpick->core.bwcolrec.y ) &&
                 ( bev->x <= ( gtk3colpick->core.bwcolrec.x + 
                               gtk3colpick->core.bwcolrec.width  ) ) &&
                 ( bev->y <= ( gtk3colpick->core.bwcolrec.y + 
                               gtk3colpick->core.bwcolrec.height ) ) ) {
                sysinfo->bgcolor = 0xFFFFFFFF;
                sysinfo->fgcolor = 0x00000000;
            }

            if ( ( bev->x >= gtk3colpick->core.swcolrec.x ) &&
                 ( bev->y >= gtk3colpick->core.swcolrec.y ) &&
                 ( bev->x <= ( gtk3colpick->core.swcolrec.x + 
                               gtk3colpick->core.swcolrec.width  ) ) &&
                 ( bev->y <= ( gtk3colpick->core.swcolrec.y + 
                               gtk3colpick->core.swcolrec.height ) ) ) {
                uint32_t tmpcolor = sysinfo->bgcolor;

                sysinfo->bgcolor = sysinfo->fgcolor;
                sysinfo->fgcolor = tmpcolor;
            }

            gtk_widget_queue_draw ( widget );
        } break;

        default:
        break;
    }
}

/******************************************************************************/
static void Resize ( GtkWidget    *widget, 
                     GdkRectangle *allocation, 
                     gpointer      user_data ) {
    GTK3M3DUICOLORPICKER *gtk3colpick = ( GTK3M3DUICOLORPICKER * ) user_data;

    m3duicolorpicker_resize ( &gtk3colpick->core,
                               allocation->width,
                               allocation->height );
}

/******************************************************************************/
static void Draw ( GtkWidget *widget,  
                   cairo_t   *cr,  
                   gpointer   user_data ) {
    GTK3M3DUICOLORPICKER *gtk3colpick = ( GTK3M3DUICOLORPICKER * ) user_data;
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );
    GtkAllocation allocation;
    cairo_text_extents_t te;
    int32_t x1, y1, x2, y2;
    M3DSYSINFO *sysinfo = m3dsysinfo_get ( );
    float FR = ( ( sysinfo->fgcolor & 0x00FF0000 ) >> 16 ) / 255.0f,
          FG = ( ( sysinfo->fgcolor & 0x0000FF00 ) >>  8 ) / 255.0f,
          FB = ( ( sysinfo->fgcolor & 0x000000FF ) >>  0 ) / 255.0f;
    float BR = ( ( sysinfo->bgcolor & 0x00FF0000 ) >> 16 ) / 255.0f,
          BG = ( ( sysinfo->bgcolor & 0x0000FF00 ) >>  8 ) / 255.0f,
          BB = ( ( sysinfo->bgcolor & 0x000000FF ) >>  0 ) / 255.0f;

    gtk_widget_get_allocation ( widget, &allocation );

    gtk_render_background ( context, cr,
                                     0x00,
                                     0x00,
                                     allocation.width,
                                     allocation.height );

    /******************** Background *********************/
    cairo_set_source_rgb ( cr, BR, BG, BB );
    cairo_rectangle      ( cr, gtk3colpick->core.bgcolrec.x,
                               gtk3colpick->core.bgcolrec.y,
                               gtk3colpick->core.bgcolrec.width,
                               gtk3colpick->core.bgcolrec.height );
    cairo_fill           ( cr );
    cairo_set_source_rgb ( cr, 0.0f, 0.0f, 0.0f );
    cairo_set_line_width ( cr, 1.0f );
    cairo_rectangle      ( cr, gtk3colpick->core.bgcolrec.x,
                               gtk3colpick->core.bgcolrec.y,
                               gtk3colpick->core.bgcolrec.width,
                               gtk3colpick->core.bgcolrec.height );
    cairo_stroke         ( cr );

    /******************* Foreground **********************/
    cairo_set_source_rgb ( cr, FR, FG, FB );
    cairo_rectangle      ( cr, gtk3colpick->core.fgcolrec.x,
                               gtk3colpick->core.fgcolrec.y,
                               gtk3colpick->core.fgcolrec.width,
                               gtk3colpick->core.fgcolrec.height );
    cairo_fill           ( cr );
    cairo_set_source_rgb ( cr, 0.0f, 0.0f, 0.0f );
    cairo_set_line_width ( cr, 1.0f );
    cairo_rectangle      ( cr, gtk3colpick->core.fgcolrec.x,
                               gtk3colpick->core.fgcolrec.y,
                               gtk3colpick->core.fgcolrec.width,
                               gtk3colpick->core.fgcolrec.height );
    cairo_stroke         ( cr );


    /************************ B & W **********************/
    cairo_set_source_rgb ( cr, 0.0f, 0.0f, 0.0f );
    cairo_rectangle      ( cr, gtk3colpick->core.bwcolrec.x,
                               gtk3colpick->core.bwcolrec.y,
                               gtk3colpick->core.bwcolrec.width  / 2,
                               gtk3colpick->core.bwcolrec.height / 2 );
    cairo_fill           ( cr );
    cairo_set_source_rgb ( cr, 0.0f, 0.0f, 0.0f );
    cairo_set_line_width ( cr, 1.0f );
    cairo_rectangle      ( cr, gtk3colpick->core.bwcolrec.x,
                               gtk3colpick->core.bwcolrec.y,
                               gtk3colpick->core.bwcolrec.width  / 2,
                               gtk3colpick->core.bwcolrec.height / 2 );
    cairo_stroke         ( cr );

    cairo_set_source_rgb ( cr, 1.0f, 1.0f, 1.0f );
    cairo_rectangle      ( cr, gtk3colpick->core.bwcolrec.x + gtk3colpick->core.bwcolrec.width  / 2,
                               gtk3colpick->core.bwcolrec.y + gtk3colpick->core.bwcolrec.height / 2,
                               gtk3colpick->core.bwcolrec.width  / 2,
                               gtk3colpick->core.bwcolrec.height / 2 );
    cairo_fill           ( cr );
    cairo_set_source_rgb ( cr, 0.0f, 0.0f, 0.0f );
    cairo_set_line_width ( cr, 1.0f );
    cairo_rectangle      ( cr, gtk3colpick->core.bwcolrec.x + gtk3colpick->core.bwcolrec.width  / 2,
                               gtk3colpick->core.bwcolrec.y + gtk3colpick->core.bwcolrec.height / 2,
                               gtk3colpick->core.bwcolrec.width  / 2,
                               gtk3colpick->core.bwcolrec.height / 2 );
    cairo_stroke         ( cr );

    /************************ Switch **********************/
    cairo_set_source_rgb ( cr, BR, BG, BB );
    cairo_rectangle      ( cr, gtk3colpick->core.swcolrec.x,
                               gtk3colpick->core.swcolrec.y,
                               gtk3colpick->core.swcolrec.width  / 2,
                               gtk3colpick->core.swcolrec.height / 2 );
    cairo_fill           ( cr );
    cairo_set_source_rgb ( cr, 0.0f, 0.0f, 0.0f );
    cairo_set_line_width ( cr, 1.0f );
    cairo_rectangle      ( cr, gtk3colpick->core.swcolrec.x,
                               gtk3colpick->core.swcolrec.y,
                               gtk3colpick->core.swcolrec.width  / 2,
                               gtk3colpick->core.swcolrec.height / 2 );
    cairo_stroke         ( cr );

    cairo_set_source_rgb ( cr, FR, FG, FB );
    cairo_rectangle      ( cr, gtk3colpick->core.swcolrec.x + gtk3colpick->core.bwcolrec.width  / 2,
                               gtk3colpick->core.swcolrec.y + gtk3colpick->core.bwcolrec.height / 2,
                               gtk3colpick->core.swcolrec.width  / 2,
                               gtk3colpick->core.swcolrec.height / 2 );
    cairo_fill           ( cr );
    cairo_set_source_rgb ( cr, 0.0f, 0.0f, 0.0f );
    cairo_set_line_width ( cr, 1.0f );
    cairo_rectangle      ( cr, gtk3colpick->core.swcolrec.x + gtk3colpick->core.bwcolrec.width  / 2,
                               gtk3colpick->core.swcolrec.y + gtk3colpick->core.bwcolrec.height / 2,
                               gtk3colpick->core.swcolrec.width  / 2,
                               gtk3colpick->core.swcolrec.height / 2 );
    cairo_stroke         ( cr );
}

/******************************************************************************/
GTK3M3DUICOLORPICKER *gtk3_m3duicolorpicker_create ( GtkWidget *parent,
                                                     GTK3M3DUI *gtk3mui,
                                                     char      *name ) {
    GTK3M3DUICOLORPICKER *gtk3colpick = gtk3_m3duicolorpicker_new ( gtk3mui );
    GtkWidget *drw = ui_gtk_drawing_area_new ( CLASS_MAIN );

    gtk3colpick->area = drw;

    /*** Drawing area does not receive mous events by defaults ***/
    gtk_widget_set_events ( drw, gtk_widget_get_events ( drw )  |
                                 GDK_BUTTON_PRESS_MASK          |
                                 GDK_BUTTON_RELEASE_MASK        |
                                 GDK_POINTER_MOTION_MASK        |
                                 GDK_POINTER_MOTION_HINT_MASK );

    g_signal_connect ( G_OBJECT (drw), "size-allocate", 
                                       G_CALLBACK (Resize), gtk3colpick );

    g_signal_connect ( G_OBJECT (drw), "draw"                ,
                                       G_CALLBACK (Draw)  , gtk3colpick );

    g_signal_connect ( G_OBJECT (drw), "button_release_event",
                                       G_CALLBACK (Input) , gtk3colpick );

    /*gtk_widget_set_size_request ( drw, width, height );*/

    gtk_widget_show ( drw );


    return gtk3colpick;
}
