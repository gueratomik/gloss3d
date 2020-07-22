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

#define FGBGBORDER 0x02

/******************************************************************************/
typedef struct _FGBGDATASTRUCT {
    G3DUIRECTANGLE fgcolorrec;
    G3DUIRECTANGLE bgcolorrec;
    G3DUIRECTANGLE bwcolorrec;
    G3DUIRECTANGLE swcolorrec;
    G3DUI         *gui;
} FGBGDATASTRUCT;

/******************************************************************************/
static FGBGDATASTRUCT *fgbgdatastruct_new ( G3DUI *gui ) {
    void *memarea = calloc ( 0x01, sizeof ( FGBGDATASTRUCT ) );
    FGBGDATASTRUCT *fgbgdata = ( FGBGDATASTRUCT * ) memarea;

    if ( fgbgdata == NULL ) {
        fprintf ( stderr, "%s: memory allocation failed\n", __func__ );

        return NULL;
    }

    fgbgdata->gui = gui;

    return fgbgdata;
}

/******************************************************************************/
static void setForegroundColor ( GtkDialog *dialog,
                                 gint       response_id,
                                 gpointer   user_data ) {
    static GdkRGBA color;

    gtk_color_chooser_get_rgba ( GTK_COLOR_CHOOSER ( dialog ), &color );

    if ( response_id == GTK_RESPONSE_OK ) {
        L3DSYSINFO *sysinfo = l3dsysinfo_get ( );
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
        L3DSYSINFO *sysinfo = l3dsysinfo_get ( );
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
    FGBGDATASTRUCT *fgbgdata = ( FGBGDATASTRUCT * ) user_data;
    L3DSYSINFO *sysinfo = l3dsysinfo_get ( );

    switch ( gdkev->type ) {
        case GDK_BUTTON_RELEASE : {
            GdkEventButton *bev = ( GdkEventButton * ) gdkev;

            if ( ( bev->x >= fgbgdata->fgcolorrec.x ) &&
                 ( bev->y >= fgbgdata->fgcolorrec.y ) &&
                 ( bev->x <= ( fgbgdata->fgcolorrec.x + 
                               fgbgdata->fgcolorrec.width  ) ) &&
                 ( bev->y <= ( fgbgdata->fgcolorrec.y + 
                               fgbgdata->fgcolorrec.height ) ) ) {
                GtkWidget *dialog = gtk_color_chooser_dialog_new ( "Choose foreground color", NULL );

                g_signal_connect ( G_OBJECT (dialog), "response",
                                   G_CALLBACK (setForegroundColor) , NULL );

                gtk_widget_show ( dialog );
            } else {
                if ( ( bev->x >= fgbgdata->bgcolorrec.x ) &&
                     ( bev->y >= fgbgdata->bgcolorrec.y ) &&
                     ( bev->x <= ( fgbgdata->bgcolorrec.x + 
                                   fgbgdata->bgcolorrec.width  ) ) &&
                     ( bev->y <= ( fgbgdata->bgcolorrec.y + 
                                   fgbgdata->bgcolorrec.height ) ) ) {
                    GtkWidget *dialog = gtk_color_chooser_dialog_new ( "Choose background color", NULL );

                    g_signal_connect ( G_OBJECT (dialog), "response",
                                       G_CALLBACK (setBackgroundColor) , NULL );

                    gtk_widget_show ( dialog );
                }
            }

            if ( ( bev->x >= fgbgdata->bwcolorrec.x ) &&
                 ( bev->y >= fgbgdata->bwcolorrec.y ) &&
                 ( bev->x <= ( fgbgdata->bwcolorrec.x + 
                               fgbgdata->bwcolorrec.width  ) ) &&
                 ( bev->y <= ( fgbgdata->bwcolorrec.y + 
                               fgbgdata->bwcolorrec.height ) ) ) {
                sysinfo->bgcolor = 0xFFFFFFFF;
                sysinfo->fgcolor = 0x00000000;
            }

            if ( ( bev->x >= fgbgdata->swcolorrec.x ) &&
                 ( bev->y >= fgbgdata->swcolorrec.y ) &&
                 ( bev->x <= ( fgbgdata->swcolorrec.x + 
                               fgbgdata->swcolorrec.width  ) ) &&
                 ( bev->y <= ( fgbgdata->swcolorrec.y + 
                               fgbgdata->swcolorrec.height ) ) ) {
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
    FGBGDATASTRUCT *fgbgdata = ( FGBGDATASTRUCT * ) user_data;

    fgbgdata->fgcolorrec.x      = ( 0x00 ) + FGBGBORDER;
    fgbgdata->fgcolorrec.y      = ( 0x00 ) + FGBGBORDER;
    fgbgdata->fgcolorrec.width  = ( allocation->width  * 0x05 / 0x08 ) - ( FGBGBORDER * 2 );
    fgbgdata->fgcolorrec.height = ( allocation->height * 0x05 / 0x08 ) - ( FGBGBORDER * 2 );

    fgbgdata->bgcolorrec.x      = ( allocation->width  * 0x03 / 0x08 ) + FGBGBORDER;
    fgbgdata->bgcolorrec.y      = ( allocation->height * 0x03 / 0x08 ) + FGBGBORDER;
    fgbgdata->bgcolorrec.width  = ( allocation->width  * 0x05 / 0x08 ) - ( FGBGBORDER * 2 );
    fgbgdata->bgcolorrec.height = ( allocation->height * 0x05 / 0x08 ) - ( FGBGBORDER * 2 );

    fgbgdata->bwcolorrec.x      = ( 0x00 ) + FGBGBORDER;
    fgbgdata->bwcolorrec.y      = ( allocation->height * 0x05 / 0x08 ) + FGBGBORDER;
    fgbgdata->bwcolorrec.width  = ( allocation->width  * 0x03 / 0x08 ) - ( FGBGBORDER * 2 );
    fgbgdata->bwcolorrec.height = ( allocation->height * 0x03 / 0x08 ) - ( FGBGBORDER * 2 );

    fgbgdata->swcolorrec.x      = ( allocation->height * 0x05 / 0x08 ) + FGBGBORDER;
    fgbgdata->swcolorrec.y      = ( 0x00 ) + FGBGBORDER;
    fgbgdata->swcolorrec.width  = ( allocation->width  * 0x03 / 0x08 ) - ( FGBGBORDER * 2 );
    fgbgdata->swcolorrec.height = ( allocation->height * 0x03 / 0x08 ) - ( FGBGBORDER * 2 );
}

/******************************************************************************/
static void Draw ( GtkWidget *widget,  
                   cairo_t   *cr,  
                   gpointer   user_data ) {
    FGBGDATASTRUCT *fgbgdata = ( FGBGDATASTRUCT * ) user_data;
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );
    GtkAllocation allocation;
    cairo_text_extents_t te;
    int32_t x1, y1, x2, y2;
    L3DSYSINFO *sysinfo = l3dsysinfo_get ( );
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
    cairo_rectangle      ( cr, fgbgdata->bgcolorrec.x,
                               fgbgdata->bgcolorrec.y,
                               fgbgdata->bgcolorrec.width,
                               fgbgdata->bgcolorrec.height );
    cairo_fill           ( cr );
    cairo_set_source_rgb ( cr, 0.0f, 0.0f, 0.0f );
    cairo_set_line_width ( cr, 1.0f );
    cairo_rectangle      ( cr, fgbgdata->bgcolorrec.x,
                               fgbgdata->bgcolorrec.y,
                               fgbgdata->bgcolorrec.width,
                               fgbgdata->bgcolorrec.height );
    cairo_stroke         ( cr );

    /******************* Foreground **********************/
    cairo_set_source_rgb ( cr, FR, FG, FB );
    cairo_rectangle      ( cr, fgbgdata->fgcolorrec.x,
                               fgbgdata->fgcolorrec.y,
                               fgbgdata->fgcolorrec.width,
                               fgbgdata->fgcolorrec.height );
    cairo_fill           ( cr );
    cairo_set_source_rgb ( cr, 0.0f, 0.0f, 0.0f );
    cairo_set_line_width ( cr, 1.0f );
    cairo_rectangle      ( cr, fgbgdata->fgcolorrec.x,
                               fgbgdata->fgcolorrec.y,
                               fgbgdata->fgcolorrec.width,
                               fgbgdata->fgcolorrec.height );
    cairo_stroke         ( cr );


    /************************ B & W **********************/
    cairo_set_source_rgb ( cr, 0.0f, 0.0f, 0.0f );
    cairo_rectangle      ( cr, fgbgdata->bwcolorrec.x,
                               fgbgdata->bwcolorrec.y,
                               fgbgdata->bwcolorrec.width  / 2,
                               fgbgdata->bwcolorrec.height / 2 );
    cairo_fill           ( cr );
    cairo_set_source_rgb ( cr, 0.0f, 0.0f, 0.0f );
    cairo_set_line_width ( cr, 1.0f );
    cairo_rectangle      ( cr, fgbgdata->bwcolorrec.x,
                               fgbgdata->bwcolorrec.y,
                               fgbgdata->bwcolorrec.width  / 2,
                               fgbgdata->bwcolorrec.height / 2 );
    cairo_stroke         ( cr );

    cairo_set_source_rgb ( cr, 1.0f, 1.0f, 1.0f );
    cairo_rectangle      ( cr, fgbgdata->bwcolorrec.x + fgbgdata->bwcolorrec.width  / 2,
                               fgbgdata->bwcolorrec.y + fgbgdata->bwcolorrec.height / 2,
                               fgbgdata->bwcolorrec.width  / 2,
                               fgbgdata->bwcolorrec.height / 2 );
    cairo_fill           ( cr );
    cairo_set_source_rgb ( cr, 0.0f, 0.0f, 0.0f );
    cairo_set_line_width ( cr, 1.0f );
    cairo_rectangle      ( cr, fgbgdata->bwcolorrec.x + fgbgdata->bwcolorrec.width  / 2,
                               fgbgdata->bwcolorrec.y + fgbgdata->bwcolorrec.height / 2,
                               fgbgdata->bwcolorrec.width  / 2,
                               fgbgdata->bwcolorrec.height / 2 );
    cairo_stroke         ( cr );

    /************************ Switch **********************/
    cairo_set_source_rgb ( cr, BR, BG, BB );
    cairo_rectangle      ( cr, fgbgdata->swcolorrec.x,
                               fgbgdata->swcolorrec.y,
                               fgbgdata->swcolorrec.width  / 2,
                               fgbgdata->swcolorrec.height / 2 );
    cairo_fill           ( cr );
    cairo_set_source_rgb ( cr, 0.0f, 0.0f, 0.0f );
    cairo_set_line_width ( cr, 1.0f );
    cairo_rectangle      ( cr, fgbgdata->swcolorrec.x,
                               fgbgdata->swcolorrec.y,
                               fgbgdata->swcolorrec.width  / 2,
                               fgbgdata->swcolorrec.height / 2 );
    cairo_stroke         ( cr );

    cairo_set_source_rgb ( cr, FR, FG, FB );
    cairo_rectangle      ( cr, fgbgdata->swcolorrec.x + fgbgdata->bwcolorrec.width  / 2,
                               fgbgdata->swcolorrec.y + fgbgdata->bwcolorrec.height / 2,
                               fgbgdata->swcolorrec.width  / 2,
                               fgbgdata->swcolorrec.height / 2 );
    cairo_fill           ( cr );
    cairo_set_source_rgb ( cr, 0.0f, 0.0f, 0.0f );
    cairo_set_line_width ( cr, 1.0f );
    cairo_rectangle      ( cr, fgbgdata->swcolorrec.x + fgbgdata->bwcolorrec.width  / 2,
                               fgbgdata->swcolorrec.y + fgbgdata->bwcolorrec.height / 2,
                               fgbgdata->swcolorrec.width  / 2,
                               fgbgdata->swcolorrec.height / 2 );
    cairo_stroke         ( cr );
}

/******************************************************************************/
GtkWidget *createFgBgButton ( GtkWidget *parent, 
                              G3DUI     *gui,
                              char      *name,
                              gint       x,
                              gint       y,
                              gint       width,
                              gint       height ) {
    FGBGDATASTRUCT *fgbgdata = fgbgdatastruct_new ( gui );
    GtkWidget *drw = gtk_drawing_area_new ( );

    gtk_widget_set_name ( drw, name );

    /*** Drawing area does not receive mous events by defaults ***/
    gtk_widget_set_events ( drw, gtk_widget_get_events ( drw )  |
                                 GDK_BUTTON_PRESS_MASK          |
                                 GDK_BUTTON_RELEASE_MASK        |
                                 GDK_POINTER_MOTION_MASK        |
                                 GDK_POINTER_MOTION_HINT_MASK );

    g_signal_connect ( G_OBJECT (drw), "size-allocate", 
                                       G_CALLBACK (Resize), fgbgdata );

    g_signal_connect ( G_OBJECT (drw), "draw"                ,
                                       G_CALLBACK (Draw)  , fgbgdata );

    g_signal_connect ( G_OBJECT (drw), "button_release_event",
                                       G_CALLBACK (Input) , fgbgdata );

    gtk_widget_set_size_request ( drw, width, height );

    gtk_fixed_put ( GTK_FIXED(parent), drw, x, y );

    gtk_widget_show ( drw );


    return drw;
}
