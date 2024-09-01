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
#define _GNU_SOURCE
#include <string.h> /*** strcasestr() ***/


#include <config.h>
#include <g3dui_gtk3.h>



static void gtk3_updateAllCurrentMouseTools ( );

/******************************************************************************/
void g3duirectangle_toGdkRectangle ( G3DUIRECTANGLE *in, 
                                     GdkRectangle   *out  ) {
    out->x      = in->x;
    out->y      = in->y;
    out->width  = in->width;
    out->height = in->height;
}

/******************************************************************************/
GtkFrame *ui_gtk_frame_new ( char       *class,
                             const char *label ) {
    GtkWidget *frm = gtk_frame_new ( label );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( frm );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_FRAME(frm);
}

/******************************************************************************/
GtkScrolledWindow *ui_gtk_scrolled_window_new ( char          *class,
                                                GtkAdjustment *hadjustment,
                                                GtkAdjustment *vadjustment ) {
    GtkWidget *cmb = gtk_scrolled_window_new ( hadjustment, vadjustment );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( cmb );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_SCROLLED_WINDOW(cmb);
}

/******************************************************************************/
GtkEntry *ui_gtk_entry_new ( char *class ) {
    GtkWidget *ent = gtk_entry_new ( );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( ent );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_ENTRY(ent);
}

/******************************************************************************/
GtkComboBoxText *ui_gtk_combo_box_text_new ( char *class ) {
    GtkWidget *cmb = gtk_combo_box_text_new ( );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( cmb );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_COMBO_BOX_TEXT(cmb);
}


/******************************************************************************/
GtkCheckButton *ui_gtk_check_button_new_with_label ( char *class,
                                                     char *label ) {
    GtkWidget *btn = gtk_check_button_new_with_label ( label );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( btn );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_CHECK_BUTTON(btn);
}

/******************************************************************************/
GtkCheckButton *ui_gtk_check_button_new ( char *class ) {
    GtkWidget *btn = gtk_check_button_new ( );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( btn );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_CHECK_BUTTON(btn);
}

/******************************************************************************/
GtkSpinButton *ui_gtk_spin_button_new ( char          *class,
                                        GtkAdjustment *adjustment,
                                        gdouble        climb_rate,
                                        guint          digits ) {
    GtkWidget *spin = gtk_spin_button_new ( adjustment, climb_rate, digits );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( spin );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_SPIN_BUTTON(spin);
}

/******************************************************************************/
GtkStatusbar *ui_gtk_statusbar_new ( char *class ) {
    GtkWidget *statusBar = gtk_statusbar_new ( );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( statusBar );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_STATUSBAR(statusBar);
}

/******************************************************************************/
GtkDialog *ui_gtk_dialog_new ( char *class ) {
    GtkWidget *dial = gtk_dialog_new ( );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( dial );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_DIALOG(dial);
}

/******************************************************************************/
GtkToolbar *ui_gtk_toolbar_new ( char *class ) {
    GtkWidget *toolbar = gtk_toolbar_new ( );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( toolbar );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_TOOLBAR(toolbar);
}

/******************************************************************************/
GtkMenuBar *ui_gtk_menu_bar_new ( char *class ) {
    GtkWidget *menuBar = gtk_menu_bar_new ( );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( menuBar );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_MENU_BAR(menuBar);
}

/******************************************************************************/
GtkCheckMenuItem *ui_gtk_check_menu_item_new_with_mnemonic ( char *class,
                                                             const gchar* label ) {
    GtkWidget *menuItem = gtk_check_menu_item_new_with_mnemonic ( label );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( menuItem );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_CHECK_MENU_ITEM(menuItem);
}

/******************************************************************************/
GtkMenuItem *ui_gtk_menu_item_new_with_mnemonic ( char *class,
                                                  const gchar* label ) {
    GtkWidget *menuItem = gtk_menu_item_new_with_mnemonic ( label );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( menuItem );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_MENU_ITEM(menuItem);
}

/******************************************************************************/
GtkWindow *ui_gtk_window_new ( char         *class,
                               GtkWindowType type ) {
    GtkWidget *window = gtk_window_new ( type );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( window );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_WINDOW(window);
}

/******************************************************************************/
GtkToggleToolButton *ui_gtk_toggle_tool_button_new ( char *class ) {
    GtkWidget *btn = ( GtkWidget * ) gtk_toggle_tool_button_new ( );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( btn );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_TOGGLE_TOOL_BUTTON(btn);
}

/******************************************************************************/
GtkEventBox *ui_gtk_event_box_new ( char *class ) {
    GtkWidget *box = gtk_event_box_new ( );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( box );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_EVENT_BOX(box);
}

/******************************************************************************/
GtkToolItem *ui_gtk_tool_button_new ( char        *class,
                                      GtkWidget   *icon_widget,
                                      const gchar *label ) {
    GtkWidget *btn = ( GtkWidget * ) gtk_tool_button_new ( icon_widget, label );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( btn );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_TOOL_ITEM(btn);
}

/******************************************************************************/
GtkFixed *ui_gtk_fixed_new ( char *class ) {
    GtkWidget *fixed = gtk_fixed_new ( );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( fixed );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_FIXED(fixed);
}

/******************************************************************************/
GtkLayout *ui_gtk_layout_new ( char          *class, 
                               GtkAdjustment *hadjustment,
                               GtkAdjustment *vadjustment ) {
    GtkWidget *layout = gtk_layout_new ( hadjustment, vadjustment );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( layout );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_LAYOUT(layout);
}

/******************************************************************************/
GtkLabel *ui_gtk_label_new ( char *class,
                             char *name ) {
    GtkWidget *lab = gtk_label_new ( name );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( lab );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_LABEL(lab);
}

/******************************************************************************/
GtkButton *ui_gtk_button_new ( char *class ) {
    GtkWidget *btn = gtk_button_new ( );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( btn );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_BUTTON(btn);
}

/******************************************************************************/
GtkTextView *ui_gtk_text_view_new ( char *class ) {
    GtkWidget *txt = gtk_text_view_new ( );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( txt );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_TEXT_VIEW(txt);
}

/******************************************************************************/
GtkMenu *ui_gtk_menu_new ( char *class ) {
    GtkWidget *menu = gtk_menu_new ( );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( menu );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_MENU(menu);
}

/******************************************************************************/
GtkScale *ui_gtk_scale_new_with_range ( char          *class,
                                        GtkOrientation orientation,
                                        double         min,
                                        double         max,
                                        double         step ) {
    GtkWidget *scl = gtk_scale_new_with_range ( orientation, min, max, step );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( scl );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_SCALE(scl);
}

/******************************************************************************/
GtkDrawingArea *ui_gtk_drawing_area_new ( char *class ) {
    GtkWidget *area = gtk_drawing_area_new ( );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( area );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_DRAWING_AREA(area);
}

/******************************************************************************/
GtkNotebook *ui_gtk_notebook_new ( char *class ) {
    GtkWidget *notebook = gtk_notebook_new ( );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( notebook );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_NOTEBOOK(notebook);
}

/******************************************************************************/
GtkButton *ui_gtk_button_new_with_label ( char *class,
                                          char *name ) {
    GtkWidget *btn = gtk_button_new_with_label ( name );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( btn );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_BUTTON(btn);
}

/******************************************************************************/
GtkImage *ui_gtk_image_new_from_pixbuf ( char      *class,
                                         GdkPixbuf *pixbuf ) {
    GtkWidget *img = gtk_image_new_from_pixbuf ( pixbuf );


    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( img );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_IMAGE(img);
}

/******************************************************************************/
GtkDrawingArea *ui_createDrawingArea ( GtkFixed *parent,
                                       void     *data,
                                       char     *name,
                                       char     *class,
                                       gint      x, 
                                       gint      y,
                                       gint      width,
                                       gint      height,
                                       void    (*cbk)( GtkWidget *, 
                                                       cairo_t   *cr,
                                                       gpointer ) ) {
    GtkDrawingArea *area = ui_gtk_drawing_area_new ( class );

    gtk_widget_set_name ( GTK_WIDGET(area), name );

    gtk_widget_set_size_request ( GTK_WIDGET(area), width, height );

    if ( cbk ) {
        g_signal_connect ( G_OBJECT(area), "draw", G_CALLBACK (cbk), data );
    }

    gtk_fixed_put ( parent, GTK_WIDGET(area), x, y );

    gtk_widget_show ( GTK_WIDGET(area) );


    return area;
}

/******************************************************************************/
GtkFixed *ui_createTab ( GtkNotebook *parent, 
                         void        *data,
                         char        *name,
                         char        *class,
                         gint         x,
                         gint         y,
                         gint         width,
                         gint         height ) {
    GdkRectangle prec = { 0x00, 0x00, width, height };
    GtkLabel *lab = ui_gtk_label_new ( class, name );
    GtkFixed *pan = ui_gtk_fixed_new ( class );

    gtk_widget_set_name ( GTK_WIDGET(pan), name );

    gtk_widget_set_size_request ( GTK_WIDGET(pan), width, height );

    /*gtk_fixed_put ( parent, pan, x, y );*/
    gtk_notebook_append_page ( parent, GTK_WIDGET(pan), GTK_WIDGET(lab) );

    gtk_widget_show ( GTK_WIDGET(lab) );
    gtk_widget_show ( GTK_WIDGET(pan) );


    return pan;
}

/******************************************************************************/
GtkCheckButton *ui_createToggleLabel ( GtkFixed *parent, 
                                       void     *data,
                                       char     *name,
                                       char     *class,
                                       gint      x, 
                                       gint      y,
                                       gint      labwidth,
                                       gint      btnwidth,
                                       gint      height,
                                       void    (*cbk)( GtkWidget *, 
                                                       gpointer ) ) {
    GtkCheckButton *btn = ui_gtk_check_button_new ( class );
    GtkLabel       *lab = ui_gtk_label_new ( class, name );

    gtk_label_set_xalign ( lab, 0.9f );

    gtk_widget_set_name ( GTK_WIDGET(lab), name );
    gtk_widget_set_name ( GTK_WIDGET(btn), name );

    gtk_widget_set_size_request ( GTK_WIDGET(lab), labwidth, height );
    gtk_widget_set_size_request ( GTK_WIDGET(btn), btnwidth, height );

    gtk_toggle_button_set_mode ( GTK_TOGGLE_BUTTON(btn), TRUE );

    if ( cbk ) {
        g_signal_connect ( GTK_WIDGET(btn), "toggled", G_CALLBACK ( cbk ), data );
    }

    gtk_fixed_put ( GTK_FIXED(parent), GTK_WIDGET(lab), x, y );
    gtk_fixed_put ( GTK_FIXED(parent), GTK_WIDGET(btn), x + labwidth, y );

    gtk_widget_show ( GTK_WIDGET(lab) );
    gtk_widget_show ( GTK_WIDGET(btn) );


    return btn;
}

/********* https://developer.gnome.org/gtk3/stable/GtkSpinButton.html *********/
GtkTextView *ui_createTextView ( GtkWidget *parent, 
                                 void      *data,
                                 char      *name,
                                 char      *class,
                                 gint       x, 
                                 gint       y,
                                 gint       width,
                                 gint       height,
                                 void     (*cbk)( GtkTextBuffer *,
                                                  gpointer ) ) {
    GtkScrolledWindow *win = ui_gtk_scrolled_window_new ( class, NULL, NULL );
    GtkTextView       *txt = ui_gtk_text_view_new ( class );
    GtkTextBuffer     *buf = gtk_text_view_get_buffer( txt );

    gtk_container_add( GTK_CONTAINER(win), GTK_WIDGET(txt) );

    gtk_widget_set_size_request ( GTK_WIDGET(win), width, height );

    gtk_fixed_put ( GTK_FIXED(parent), GTK_WIDGET(win), x, y );

    if ( cbk ) {
        g_signal_connect ( G_OBJECT(buf), "changed", G_CALLBACK(cbk), data );
    }

    gtk_widget_show ( GTK_WIDGET(win) );


    return txt;
}

/******************************************************************************/
GtkLabel *ui_createSimpleLabel ( GtkFixed *parent,
                                 void     *data,
                                 char     *name,
                                 char     *class,
                                 gint      x, 
                                 gint      y,
                                 gint      width,
                                 gint      height ) {
    GtkLabel *lab  = ui_gtk_label_new ( class, name );

    gtk_widget_set_name ( GTK_WIDGET(lab), name );
    gtk_widget_set_size_request ( GTK_WIDGET(lab), width, height );

    gtk_fixed_put ( GTK_FIXED(parent), GTK_WIDGET(lab), x, y );

    gtk_widget_show ( GTK_WIDGET(lab) );

    return lab;
}

/******************************************************************************/
GtkScale *ui_createHorizontalScale ( GtkFixed  *parent,
                                     void      *data,
                                     char      *name,
                                     char      *class,
                                     gint       x, 
                                     gint       y,
                                     gint       labwidth,
                                     gint       width,
                                     gint       height,
                                     float      min,
                                     float      max,
                                     float      step,
                                     void     (*cbk)( GtkWidget *, 
                                                      gpointer ) ) {
    GtkScale *scl = ui_gtk_scale_new_with_range ( class,
                                                  GTK_ORIENTATION_HORIZONTAL, 
                                                  min,
                                                  max,
                                                  step );

    gtk_widget_set_name ( GTK_WIDGET(scl), name );

    gtk_scale_set_value_pos ( GTK_SCALE(scl), GTK_POS_RIGHT );

    gtk_widget_set_size_request ( GTK_WIDGET(scl), width, height );

    if ( cbk ) {
        g_signal_connect ( G_OBJECT(scl), "value-changed", G_CALLBACK ( cbk ), data );
    }

    gtk_fixed_put ( GTK_FIXED(parent), GTK_WIDGET(scl), x + labwidth + 8, y );

    if ( labwidth ) {
        GdkRectangle lrec = { 0x00, 0x00, labwidth, height };
        GtkLabel   *lab  = ui_gtk_label_new ( class, name );

        gtk_label_set_xalign ( lab, 0.9f );

        gtk_widget_set_name ( GTK_WIDGET(lab), name );

        gtk_widget_set_size_request ( GTK_WIDGET(lab), lrec.width, lrec.height );

        gtk_fixed_put ( GTK_FIXED(parent), GTK_WIDGET(lab), x, y );

        gtk_widget_show ( GTK_WIDGET(lab) );
    }

    gtk_widget_show ( GTK_WIDGET(scl) );


    return scl;
}

/******************************************************************************/
GtkColorButton *ui_createColorButton ( GtkFixed *parent,
                                       void     *data,
                                       char     *name,
                                       char     *class,
                                       gint      x, 
                                       gint      y,
                                       gint      labwidth,
                                       gint      btnwidth,
                                       gint      btnheight,
                                       void    (*cbk)( GtkWidget *, 
                                                       gpointer ) ) {
    GtkWidget *btn = gtk_color_button_new ( );

    gtk_widget_set_name ( btn, name );

    gtk_widget_set_size_request ( btn, btnwidth, btnheight );

    if ( cbk ) {
        g_signal_connect ( btn, "color-set", G_CALLBACK ( cbk ), data );
    }

    gtk_fixed_put ( GTK_FIXED(parent), btn, x + labwidth, y );

    if ( labwidth ) {
        GdkRectangle lrec = { 0x00, 0x00, labwidth, btnheight };
        GtkLabel   *lab  = ui_gtk_label_new ( class, name );

        gtk_label_set_xalign ( lab, 0.9f );

        gtk_widget_set_name ( GTK_WIDGET(lab), name );

        gtk_widget_set_size_request ( GTK_WIDGET(lab), lrec.width, lrec.height );

        gtk_fixed_put ( GTK_FIXED(parent), GTK_WIDGET(lab), x, y );

        gtk_widget_show ( GTK_WIDGET(lab) );
    }

    gtk_widget_show ( btn );


    return GTK_COLOR_BUTTON(btn);
}

/******************************************************************************/
static gboolean grabCbk ( GtkWidget *widget, GdkEvent  *event, 
                                             gpointer user_data ) {
    /*** Take keyboard input when clicked-in ***/
    gtk_widget_grab_focus ( widget );

    /*** keep processing ***/
    return FALSE;
}

/******************************************************************************/
/********* https://developer.gnome.org/gtk3/stable/GtkSpinButton.html *********/
GtkSpinButton *ui_createNumericText ( GtkFixed      *parent, 
                                      void          *data,
                                      GtkAdjustment *adj,
                                      char          *name,
                                      char          *class,
                                      gint           x, 
                                      gint           y,
                                      gint           labwidth,
                                      gint           txtwidth,
                                      gint           txtheight,
                                      void (*cbk)( GtkWidget *, 
                                                   gpointer ) ) {
    GtkSpinButton *btn  = ui_gtk_spin_button_new ( class, adj, 1.0, 0 );
    uint32_t charwidth;

    gtk_entry_set_width_chars   ( GTK_ENTRY(btn), 0 );

    gtk_widget_set_name         ( GTK_WIDGET(btn), name );
    gtk_widget_set_size_request ( GTK_WIDGET(btn), txtwidth, txtheight );
    gtk_widget_set_hexpand      ( GTK_WIDGET(btn), FALSE );

    gtk_fixed_put ( GTK_FIXED(parent), GTK_WIDGET(btn), x + labwidth, y );

    if ( labwidth ) {
        GtkLabel *lab  = ui_gtk_label_new ( class, name );

        gtk_label_set_xalign ( lab, 0.9f );

        gtk_widget_set_name         ( GTK_WIDGET(lab), name );
        gtk_widget_set_size_request ( GTK_WIDGET(lab), labwidth, txtheight );

        gtk_fixed_put ( GTK_FIXED(parent), GTK_WIDGET(lab), x, y );

        gtk_widget_show ( GTK_WIDGET(lab) );
    }

    gtk_widget_show ( GTK_WIDGET(btn) );

    return GTK_SPIN_BUTTON(btn);
}

/******************************************************************************/
/********* https://developer.gnome.org/gtk3/stable/GtkSpinButton.html *********/
GtkSpinButton *ui_createIntegerText ( GtkFixed *parent, 
                                      void     *data, 
                                      char     *name,
                                      char     *class,
                                      gdouble   min, 
                                      gdouble   max,
                                      gint      x, 
                                      gint      y,
                                      gint      labwidth,
                                      gint      txtwidth,
                                      gint      txtheight,
                                      void    (*cbk)( GtkWidget *, 
                                                      gpointer ) ) {
    GtkAdjustment *adj = gtk_adjustment_new ( 0.0, 
                                              min, 
                                              max,
                                              1.0,
                                              1.0,
                                              0.0 );

    GtkSpinButton *btn = ui_createNumericText ( parent, 
                                                data,
                                                adj, 
                                                name,
                                                class,
                                                x, 
                                                y, 
                                                labwidth, 
                                                txtwidth, 
                                                txtheight,
                                                cbk );

    if ( cbk ) {
        g_signal_connect ( GTK_WIDGET(btn), "value-changed"     , G_CALLBACK(cbk)    , data);
        g_signal_connect ( GTK_WIDGET(btn), "button-press-event", G_CALLBACK(grabCbk), data);
    }

    return btn;
}

/******************************************************************************/
/********* https://developer.gnome.org/gtk3/stable/GtkSpinButton.html *********/
GtkSpinButton *ui_createFloatText ( GtkFixed *parent, 
                                    void      *data, 
                                    char      *name,
                                    char      *class,
                                    gdouble    min, 
                                    gdouble    max,
                                    gint       x, 
                                    gint       y,
                                    gint       labwidth,
                                    gint       txtwidth,
                                    gint       txtheight,
                                    void       (*cbk)( GtkWidget *, 
                                                       gpointer ) ) {
    GtkAdjustment *adj = gtk_adjustment_new ( 0.0, 
                                              min,
                                              max,
                                              0.1,
                                              1.0,
                                              0.0 );

    GtkSpinButton *btn = ui_createNumericText ( parent,
                                                data,
                                                adj,
                                                name,
                                                class,
                                                x,
                                                y,
                                                labwidth,
                                                txtwidth,
                                                txtheight,
                                                cbk );

    /*** careful. This triggers the callback ***/
    gtk_spin_button_set_digits ( GTK_SPIN_BUTTON(btn), 3 );

    if ( cbk ) {
        g_signal_connect ( btn, "value-changed"     , G_CALLBACK(cbk)    , data);
        g_signal_connect ( btn, "button-press-event", G_CALLBACK(grabCbk), data);
    }

    return btn;
}

/******************************************************************************/
GtkComboBoxText *ui_createSelector ( GtkFixed  *parent, 
                                     void      *data,
                                     char      *name,
                                     char      *class,
                                     gint       x, 
                                     gint       y,
                                     gint       labwidth,
                                     gint       txtwidth,
                                     gint       txtheight,
                                     void       (*cbk)( GtkWidget *, 
                                                        gpointer ) ) {

    GtkComboBoxText *cmb  = ui_gtk_combo_box_text_new ( class );
    GdkRectangle   crec = { 0x00, 0x00, txtwidth, txtheight };

    gtk_widget_set_name ( GTK_WIDGET(cmb), name );

    gtk_widget_set_size_request ( GTK_WIDGET(cmb), crec.width, crec.height );

    gtk_fixed_put ( GTK_FIXED(parent), GTK_WIDGET(cmb), x + labwidth, y );

    if ( labwidth ) {
        GdkRectangle lrec = { 0x00, 0x00, labwidth, 0x12 };
        GtkLabel    *lab  = ui_gtk_label_new ( class, name );

        gtk_label_set_xalign ( lab, 0.9f );

        gtk_widget_set_name ( GTK_WIDGET(lab), name );

        gtk_widget_set_size_request ( GTK_WIDGET(lab), lrec.width, lrec.height );

        gtk_fixed_put ( GTK_FIXED(parent), GTK_WIDGET(lab), x, y );

        gtk_widget_show ( GTK_WIDGET(lab) );
    }

    if ( cbk ) { 
        g_signal_connect ( G_OBJECT(cmb), "changed", G_CALLBACK(cbk), data );
    }

    gtk_widget_show ( GTK_WIDGET(cmb) );


    return cmb;
}

/******************************************************************************/
GtkEntry *ui_createCharText ( GtkWidget *parent, 
                              void      *data,
                              char      *name,
                              char      *class,
                              gint       x, 
                              gint       y,
                              gint       labwidth,
                              gint       txtwidth,
                              gint       txtheight,
                              void     (*cbk)( GtkWidget *,
                                               GdkEvent  *, 
                                               gpointer ) ) {
    GtkEntry *ent = ui_gtk_entry_new ( class );

    gtk_widget_set_name ( GTK_WIDGET(ent), name );

    gtk_widget_set_size_request ( GTK_WIDGET(ent), txtwidth, txtheight );

    /*gtk_entry_set_width_chars ( ent, 12 );*/

    gtk_fixed_put ( GTK_FIXED(parent), GTK_WIDGET(ent), x + labwidth, y );

    if ( labwidth ) {
        GtkLabel *lab  = ui_gtk_label_new ( class, name );

        gtk_label_set_xalign ( lab, 0.9f );

        gtk_widget_set_name ( GTK_WIDGET(lab), name );

        gtk_widget_set_size_request ( GTK_WIDGET(lab), labwidth, txtheight );

        gtk_fixed_put ( GTK_FIXED(parent), GTK_WIDGET(lab), x, y );

        gtk_widget_show ( GTK_WIDGET(lab) );
    }

    if ( cbk ) {
        g_signal_connect ( G_OBJECT(ent), "key-release-event", G_CALLBACK(cbk), data );
    }

    gtk_widget_show ( GTK_WIDGET(ent) );

    return ent;
}

/******************************************************************************/
GtkComboBoxText *ui_createProjectionSelector ( GtkFixed *parent,
                                               void     *data, 
                                               char     *name,
                                               char     *class,
                                               gint     x,
                                               gint     y,
                                               gint     labwidth,
                                               gint     txtwidth,
                                               gint     txtheight,
                                               void   (*cbk)( GtkWidget *, 
                                                              gpointer ) ) {
    GtkComboBoxText *cmb = ui_createSelector ( parent,
                                               data,
                                               name,
                                               class,
                                               x,
                                               y,
                                               labwidth,
                                               txtwidth,
                                               txtheight,
                                               cbk );

    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, FLATPROJECTION        );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, SPHERICALPROJECTION   );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, CYLINDRICALPROJECTION );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, BACKGROUNDPROJECTION );

    return cmb;
}

/******************************************************************************/
GtkComboBoxText *ui_createProceduralSelector ( GtkFixed *parent,
                                               void     *data, 
                                               char     *name,
                                               char     *class,
                                               gint     x,
                                               gint     y,
                                               gint     labwidth,
                                               gint     txtwidth,
                                               gint     txtheight,
                                               void   (*cbk)( GtkWidget *, 
                                                              gpointer ) ) {
    GtkComboBoxText *cmb = ui_createSelector ( parent,
                                               data,
                                               name,
                                               class,
                                               x,
                                               y,
                                               labwidth,
                                               txtwidth,
                                               txtheight,
                                               cbk );

    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, PROCPERLINNOISE );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, PROCCHESSBOARD  );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, PROCBRICK       );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, PROCGRADIENT    );

    return cmb;
}

/******************************************************************************/
GtkComboBoxText *ui_createAxisSelector ( GtkFixed *parent,
                                          void     *data, 
                                          char     *name,
                                          char     *class,
                                          gint     x,
                                          gint     y,
                                          gint     labwidth,
                                          gint     txtwidth,
                                          gint     txtheight,
                                          void   (*cbk)( GtkWidget *, 
                                                         gpointer ) ) {
    GtkComboBoxText *cmb = ui_createSelector ( parent,
                                               data,
                                               name,
                                               class,
                                               x,
                                               y,
                                               labwidth,
                                               txtwidth,
                                               txtheight,
                                               cbk );

    gtk_combo_box_text_append ( cmb, NULL, XSTR );
    gtk_combo_box_text_append ( cmb, NULL, YSTR );
    gtk_combo_box_text_append ( cmb, NULL, ZSTR );

    return cmb;
}

/******************************************************************************/
GtkComboBoxText *ui_createPlaneSelector ( GtkFixed *parent,
                                          void     *data, 
                                          char     *name,
                                          char     *class,
                                          gint     x,
                                          gint     y,
                                          gint     labwidth,
                                          gint     txtwidth,
                                          gint     txtheight,
                                          void   (*cbk)( GtkWidget *, 
                                                         gpointer ) ) {
    GtkComboBoxText *cmb = ui_createSelector ( parent,
                                               data,
                                               name,
                                               class,
                                               x,
                                               y,
                                               labwidth,
                                               txtwidth,
                                               txtheight,
                                               cbk );

    gtk_combo_box_text_append ( cmb, NULL, ZXSTR );
    gtk_combo_box_text_append ( cmb, NULL, XYSTR );
    gtk_combo_box_text_append ( cmb, NULL, YZSTR );

    return cmb;
}

/******************************************************************************/
static GtkButton *ui_createButton ( GtkFixed    *parent,
                                    void        *data, 
                                    char        *name,
                                    char        *class,
                              const char       **xpm,
                                    gint         x, 
                                    gint         y,
                                    gint         width,
                                    gint         height,
                                    void       (*cbk)( GtkWidget *, 
                                                       gpointer ) ) {
    GtkButton *btn;

    if ( xpm ) btn = ui_gtk_button_new ( class );
    else       btn = ui_gtk_button_new_with_label ( class, name );

    gtk_widget_set_name ( GTK_WIDGET(btn), name );

    gtk_widget_set_size_request ( GTK_WIDGET(btn), width, height );

    if ( cbk ) {
        g_signal_connect ( btn, "clicked", G_CALLBACK ( cbk ), data );
    }

    gtk_fixed_put ( parent, GTK_WIDGET(btn), x, y );

    if ( xpm ) {
        GdkPixbuf *img = gdk_pixbuf_new_from_xpm_data ( xpm );

        if ( img ) {
            GtkImage *imgwid = ui_gtk_image_new_from_pixbuf ( class, img );

            gtk_widget_show ( GTK_WIDGET(imgwid) );

            gtk_button_set_image ( GTK_BUTTON(btn), GTK_WIDGET(imgwid) );
        }
    }

    gtk_widget_show ( GTK_WIDGET(btn) );


    return btn;
}

/******************************************************************************/
GtkButton *ui_createPushButton ( GtkFixed *parent,
                                 void     *data,
                                 char     *name,
                                 char     *class,
                                 gint      x, 
                                 gint      y,
                                 gint      width,
                                 gint      height,
                                 void    (*cbk)( GtkWidget *, 
                                                 gpointer ) ) {
    return ui_createButton ( parent,
                             data,
                             name,
                             class,
                             NULL,
                             x,
                             y,
                             width,
                             height,
                             cbk );
}

/******************************************************************************/
GtkButton *ui_createImageButton ( GtkFixed  *parent,
                                  void        *data, 
                                  char        *name,
                                  char        *class,
                           const  char       **xpm,
                                  gint         x,
                                  gint         y,
                                  gint         width,
                                  gint         height,
                                  void       (*cbk)( GtkWidget *, 
                                                     gpointer ) ) {
    return ui_createButton ( parent,
                             data,
                             name,
                             class,
                             xpm,
                             x,
                             y,
                             width,
                             height,
                             cbk );
}

/******************************************************************************/
uint64_t gtk3_loadImageForChannel ( GTK3G3DUI  *gtk3gui,
                                    uint32_t    channelID ) {
    G3DUI *gui = ( G3DUI * ) gtk3gui;

    GtkFileFilter *filter = gtk_file_filter_new ();
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Open File",
                                           GTK_WINDOW(gtk3gui->topWin),
                        /*** from ristretto-0.3.5/src/main_window.c ***/
                                           GTK_FILE_CHOOSER_ACTION_OPEN,
                                           "_Cancel", 
                                           GTK_RESPONSE_CANCEL,
                                           "_Open", 
                                           GTK_RESPONSE_OK,
                                           NULL );

    /* extension filters */
    gtk_file_filter_add_pattern ( filter, "*.jpg" );
    gtk_file_filter_add_pattern ( filter, "*.png" );
    gtk_file_filter_add_pattern ( filter, "*.avi" );
    gtk_file_filter_add_pattern ( filter, "*.mkv" );
    gtk_file_filter_add_pattern ( filter, "*.flv" );
    gtk_file_filter_add_pattern ( filter, "*.gif" );
    gtk_file_filter_add_pattern ( filter, "*.mp4" );
    gtk_file_chooser_set_filter ( GTK_FILE_CHOOSER ( dialog ), filter );

    gtk_window_set_position ( dialog, GTK_WIN_POS_CENTER );

    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_OK ) {
        GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
        char *filename = gtk_file_chooser_get_filename ( chooser );
        G3DUIMATERIALEDIT matedit = { .gui = gui };

        g3duimaterialedit_channelChooseImage ( &matedit,
                                                channelID,
                                                filename,
                                                0x00 );

        g_free    ( ( gpointer ) filename );
    }

    gtk_widget_destroy ( dialog );

    return REDRAWMATERIALLIST | 
           UPDATECURRENTMATERIAL | UPDATECURRENTMATERIALPREVIEW;
}

/******************************************************************************/
void gtk3_renderView ( GTK3G3DUI *gtk3gui ) {
    G3DUI *gui = ( G3DUI * ) gtk3gui;

    if ( gui->currentView ) {
        GTK3G3DUIVIEW *gtk3view = ( GTK3G3DUIVIEW * ) gui->currentView;
        uint32_t width  = gtk_widget_get_allocated_width  ( gtk3view->glarea ),
                 height = gtk_widget_get_allocated_height ( gtk3view->glarea );

        G3DUIRENDERPROCESS *rps = g3dui_getRenderProcessByID ( gui, 
                                                  ( uint64_t ) gtk3view->glarea );

        if ( rps ) {
            /* First cancel running render on that window  if any */
            g3dui_cancelRenderByID ( gui, ( uint64_t ) gtk3view->glarea );
        } else {
            /*** We recreate the XImage for each rendering because the viewing window ***/
            /*** size could change in between. So, clear() and init() ***/
            g3duirenderbuffer_clear ( &gtk3view->core.rbuf );

        #ifdef __linux__
            GdkDisplay *gdkdpy = gtk_widget_get_display ( gtk3view->glarea );
            GdkWindow  *gdkwin = gtk_widget_get_window  ( gtk3view->glarea );

            g3duirenderbuffer_init ( &gtk3view->core.rbuf,
                                      gdk_x11_display_get_xdisplay ( gdkdpy ),
                                      gdk_x11_window_get_xid       ( gdkwin ),
                                      width, 
                                      height );

            Q3DFILTER *progressiveDisplay = q3dfilter_toWindow_new ( gtk3view->core.rbuf.dis,
                                                                     gtk3view->core.rbuf.win,
                                                                     gtk3view->core.rbuf.gc,
                                                                     width,
                                                                     height,
                                                                     gtk3view->core.rbuf.ximg,
                                                                     0x01 );
        #endif
        #ifdef __MINGW32__
            HWND hWnd = GDK_WINDOW_HWND ( gtk_widget_get_window ( gtk3view->glarea ) );
            HDC dc = GetDC ( hWnd );

            g3duirenderbuffer_init ( &gtk3view->core.rbuf,
                                     hWnd,
                                     width,
                                     height );

            Q3DFILTER *progressiveDisplay = q3dfilter_toWindow_new ( gtk3view->core.rbuf.hWnd,
                                                                     width,
                                                                     height,
                                                                     gtk3view->core.rbuf.wimg,
                                                                     0x01 );
        #endif

            G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
            float backgroundWidthRatio = ( ( float ) sysinfo->renderRectangle[0x01].x -
                                                     sysinfo->renderRectangle[0x00].x ) / width;
            /* declared static because must survive */
            static Q3DSETTINGS viewRsg;
            G3DCAMERA *cam = gtk3view->core.cam;

            q3dsettings_copy ( &viewRsg, gui->currsg );

            viewRsg.input.sce = gui->sce;
            viewRsg.input.cam = cam;

            viewRsg.background.widthRatio = backgroundWidthRatio;

            viewRsg.output.x1 = 0x00;
            viewRsg.output.x2 = width - 1;
            viewRsg.output.y1 = 0x00;
            viewRsg.output.y2 = height - 1;
            viewRsg.output.width = width;
            viewRsg.output.height = height;
            viewRsg.background.image = sysinfo->backgroundImage;

            viewRsg.output.startframe = gui->curframe;

            viewRsg.flags = gui->currsg->flags & ( RENDERWIREFRAME |
                                                   DISABLETEXTURING |
                                                   WIREFRAMELIGHTING |
                                                   ENABLEAA        |
                                                   RENDERDOF       |
                                                   RENDERFOG );

            rps = g3dui_render_q3d ( gui, 
                                    &viewRsg,
                                     progressiveDisplay,
                                     gui->toframe,
                                     NULL,
                                     NULL,
                                     cam,
                                     gui->curframe,
                        ( uint64_t ) gtk3view->glarea,
                                     0x00,
                                     JOBFREEONCOMPLETION );
        }
    }
}

/******************************************************************************/
uint64_t gtk3_newScene ( GTK3G3DUI *gtk3gui ) {
    G3DUI * gui = ( G3DUI * ) gtk3gui;
    GtkWidget *dialog;
    gint       res;
    uint64_t ret;

    dialog = gtk_message_dialog_new ( NULL,
                                      GTK_DIALOG_MODAL,
                                      GTK_MESSAGE_QUESTION,
                                      GTK_BUTTONS_YES_NO,
                                      "Close scene and create a new one ?" );

    gtk_window_set_position ( dialog, GTK_WIN_POS_CENTER );

    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_YES ) {
        ret = g3dui_closeScene ( gui );

        gui->sce = g3dscene_new ( 0x00, "Gloss3D scene" );
    }

    gtk_widget_destroy ( dialog );

    return ret;
}

/******************************************************************************/
uint64_t gtk3_importFile ( GTK3G3DUI   *gtk3gui,
                           const gchar *filedesc ) {
    G3DUI * gui = ( G3DUI * ) gtk3gui;
    GtkWidget *dialog;
    gint       res;
    uint64_t ret = 0x00;

    dialog = gtk_file_chooser_dialog_new ( "Import file ...",
                                           GTK_WINDOW(gtk3gui->topWin),
                        /*** from ristretto-0.3.5/src/main_window.c ***/
                                           GTK_FILE_CHOOSER_ACTION_OPEN,
                                           "_Cancel", 
                                           GTK_RESPONSE_CANCEL,
                                           "_Open", 
                                           GTK_RESPONSE_OK,
                                           NULL );

    gtk_window_set_position ( dialog, GTK_WIN_POS_CENTER );

    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_OK ) {
        GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
        const char *filename = gtk_file_chooser_get_filename ( chooser );

        ret = g3dui_importFileOk ( &gtk3gui->core, filedesc, filename );

        g_free    ( ( gpointer ) filename );
    }

    gtk_widget_destroy ( dialog );


    return ret;
}

/******************************************************************************/
gint gtk3_exit ( GTK3G3DUI *gtk3gui ) {
    GtkWidget *dialog;
    gint       res;

    if ( gtk3_g3dui_saveAlteredImages ( gtk3gui ) ) {
        return;
    }

    dialog = gtk_message_dialog_new ( NULL,
                                      GTK_DIALOG_MODAL,
                                      GTK_MESSAGE_QUESTION,
                                      GTK_BUTTONS_YES_NO,
                                      "Leave Gloss3D ?" );

    gtk_window_set_position ( dialog, GTK_WIN_POS_CENTER );

    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_YES ) {
        g3dui_exitOk ( gtk3gui );
    }

    gtk_widget_destroy ( dialog );

    return res;
}


/******************************************************************************/
uint64_t gtk3_exportFile ( GTK3G3DUI   *gtk3gui,
                           const gchar *filedesc ) {
    G3DUI * gui = ( G3DUI * ) gtk3gui;
    GtkWidget *dialog;
    gint       res;
    uint64_t ret = 0x00;

    dialog = gtk_file_chooser_dialog_new ( "Export file ...",
                                           GTK_WINDOW(gtk3gui->topWin),
                        /*** from ristretto-0.3.5/src/main_window.c ***/
                                           GTK_FILE_CHOOSER_ACTION_SAVE,
                                           "_Cancel", 
                                           GTK_RESPONSE_CANCEL,
                                           "_Open", 
                                           GTK_RESPONSE_OK,
                                           NULL );

    gtk_window_set_position ( dialog, GTK_WIN_POS_CENTER );

    gtk_file_chooser_set_do_overwrite_confirmation ( GTK_FILE_CHOOSER(dialog),
                                                     TRUE );

    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_OK ) {
        GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
        const char *filename = gtk_file_chooser_get_filename ( chooser );

        ret = g3dui_exportFileOk ( gui, filedesc, filename );

        g_free    ( ( gpointer ) filename );
    }

    gtk_widget_destroy ( dialog );


    return 0x00;
}

/******************************************************************************/
uint64_t gtk3_saveAs ( GTK3G3DUI *gtk3gui ) {
    G3DUI * gui = ( G3DUI * ) gtk3gui;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Save file as ...",
                                           GTK_WINDOW(gtk3gui->topWin),
                        /*** from ristretto-0.3.5/src/main_window.c ***/
                                           GTK_FILE_CHOOSER_ACTION_SAVE,
                                           "_Cancel", 
                                           GTK_RESPONSE_CANCEL,
                                           "_Open", 
                                           GTK_RESPONSE_OK,
                                           NULL );

    gtk_window_set_position ( dialog, GTK_WIN_POS_CENTER );

    gtk_file_chooser_set_do_overwrite_confirmation ( GTK_FILE_CHOOSER(dialog), TRUE );

    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_OK ) {
        GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
        char           *filename = gtk_file_chooser_get_filename ( chooser );
        static char     filenameext[0x1000] = { 0x00 };
        static char     windowname[0x1000] = { 0x00 };

        if ( strstr ( filename, ".g3d" ) == NULL ) {
            snprintf ( filenameext, 0x1000, "%s.g3d", filename );
        } else {
            snprintf ( filenameext, 0x1000, "%s", filename );
        }

        g3dui_setFileName ( gui, filenameext );

        g3dui_saveG3DFile ( gui );

        snprintf ( windowname, 
                   0x1000, 
                  "%s %s (%s)", 
                   G3DUIAPPNAME,
                   VERSION,
                   basename ( filenameext ) );

        gtk_window_set_title ( gtk3gui->topWin, windowname );

        g_free    ( ( gpointer ) filename );
    }

    gtk_widget_destroy ( dialog );


    return 0x00;
}

/******************************************************************************/
uint64_t gtk3_mergeFile ( GTK3G3DUI *gtk3gui ) {
    G3DUI * gui = ( G3DUI * ) gtk3gui;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Merge File",
                                           GTK_WINDOW(gtk3gui->topWin),
                        /*** from ristretto-0.3.5/src/main_window.c ***/
                                           GTK_FILE_CHOOSER_ACTION_OPEN,
                                           "_Cancel", 
                                           GTK_RESPONSE_CANCEL,
                                           "_Open", 
                                           GTK_RESPONSE_OK,
                                           NULL );

    gtk_window_set_position ( dialog, GTK_WIN_POS_CENTER );

    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_OK ) {
        GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
        char           *filename = gtk_file_chooser_get_filename ( chooser );

        if ( filename ) {
            g3dui_mergeG3DFile ( gui, filename );

            g_free    ( filename );
        }
    }

    gtk_widget_destroy ( dialog );


    return UPDATEANDREDRAWALL;
}

/******************************************************************************/
uint64_t gtk3_saveFile ( GTK3G3DUI *gtk3gui ) {
    G3DUI * gui = ( G3DUI * ) gtk3gui;
    GtkWidget *dialog;
    gint       res;

    if ( gui->filename ) {
        printf ( "saving as:%s\n", gui->filename );

        g3dui_saveG3DFile ( gui );
    } else {
        gtk3_saveAs ( gtk3gui );
    }

    return 0x00;
}

/******************************************************************************/
/*** WTF: https://developer.gnome.org/gtk3/stable/GtkFileChooserDialog.html ***/
uint64_t gtk3_openFile ( GTK3G3DUI *gtk3gui ) {
    G3DUI * gui = ( G3DUI * ) gtk3gui;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Open File",
                                           GTK_WINDOW(gtk3gui->topWin),
                        /*** from ristretto-0.3.5/src/main_window.c ***/
                                           GTK_FILE_CHOOSER_ACTION_OPEN,
                                           "_Cancel", 
                                           GTK_RESPONSE_CANCEL,
                                           "_Open", 
                                           GTK_RESPONSE_OK,
                                           NULL );

    gtk_window_set_position ( dialog, GTK_WIN_POS_CENTER );

    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_OK ) {
        GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
        char           *filename = gtk_file_chooser_get_filename ( chooser );
        static char     windowname[0x1000] = { 0x00 };


        if ( filename ) {
            g3dui_closeScene ( gui );

            g3dui_openG3DFile ( gui, filename );

            snprintf ( windowname, 
                       0x1000, 
                      "%s %s (%s)", 
                       G3DUIAPPNAME,
                       VERSION,
                       basename ( filename ) );

            gtk_window_set_title ( gtk3gui->topWin, windowname );

            g_free    ( filename );
        }

    }

    gtk_widget_destroy ( dialog );


    return UPDATEANDREDRAWALL;
}

/******************************************************************************/
void gtk3_runRender ( GTK3G3DUI *gtk3gui ) {
    G3DUI * gui = ( G3DUI * ) gtk3gui;
    Q3DSETTINGS *rsg = gui->currsg;

    if ( ( gui->renderWindow == NULL   ) &&
         ( gui->main                   ) &&
         ( gui->main->quad             ) &&
         ( gui->main->quad->view[0x00] ) ) {
        G3DCAMERA *mainCamera = gui->main->quad->view[0x00]->cam;

        if ( mainCamera ) {
            GtkWindow *dial = ui_gtk_window_new ( CLASS_MAIN, 
                                                  GTK_WINDOW_TOPLEVEL );

            gtk3_g3duirenderwindow_create ( GTK_WINDOW(dial), 
                                            gtk3gui, 
                                            "Rendering" );

            gtk_window_set_position ( GTK_WINDOW(dial), GTK_WIN_POS_CENTER_ALWAYS );
            gtk_widget_set_size_request ( GTK_WIDGET(dial), 800, 600 );

            gtk_widget_show ( GTK_WIDGET(dial) );
        }
    }
}

/******************************************************************************/
uint64_t gtk3_setMouseTool ( GTK3G3DUI *gtk3gui, 
                             GtkWidget *button,
                             char      *toolName ) {
    G3DUI * gui = ( G3DUI * ) gtk3gui;
    G3DUIMOUSETOOL *mou = g3dui_getMouseTool ( gui, toolName );
    /*G3DCAMERA *cam = gui->currentView->cam;*/

    if ( mou ) {
        g3dui_setMouseTool ( gui, mou );

        if ( ( mou->tool->flags & MOUSETOOLNOCURRENT ) == 0x00 ) {
            /*** Remember that widget ID, for example to be unset when a toggle button 
            from another parent widget is called (because XmNradioBehavior won't talk
            to other parent widget ***/
            if ( gtk3gui->currentMouseToolButton ) {
                if ( button != gtk3gui->currentMouseToolButton ) {
                     /*** disengage ***/
                    if ( GTK_IS_TOGGLE_TOOL_BUTTON ( gtk3gui->currentMouseToolButton ) ) {
                        GtkToggleToolButton *ttb = GTK_TOGGLE_TOOL_BUTTON(gtk3gui->currentMouseToolButton);

                        gui->lock = 0x01;

                        gtk_toggle_tool_button_set_active ( ttb, FALSE );

                        gui->lock = 0x00;
                    }
                }
            }

            gtk3gui->currentMouseToolButton = button;
        }
    } else {
        fprintf ( stderr, "No such mousetool %s\n", toolName );
    }

    return UPDATECURRENTMOUSETOOL;
}

/******************************************************************************/
void gtk3_setHourGlass ( GTK3G3DUI *gtk3gui ) {
    G3DUI *gui = ( G3DUI * ) gtk3gui;

    /*GtkWindow  *win  = gtk_widget_get_parent(ggt->top);
    GtkWidget  *foc  = gtk_window_get_focus ( win );
    GdkWindow  *pwin = gtk_widget_get_parent_window ( foc );
    GdkDisplay *dis  = gdk_window_get_display ( pwin );*/

    GdkDisplay       *dis = gdk_display_get_default();
    GdkDeviceManager *mgr = gdk_display_get_device_manager ( dis );
    GdkDevice        *dev = gdk_device_manager_get_client_pointer ( mgr );
    GdkCursor        *cur = gdk_cursor_new_for_display ( dis, GDK_WATCH );
    gint x, y;

    gtk3gui->winAtPosition = gdk_device_get_window_at_position ( dev, &x, &y ); 
    /* set watch cursor */

#ifdef unused
    gdk_device_grab ( dev,
                      ggt->winAtPosition,
                      GDK_OWNERSHIP_WINDOW,
                      FALSE,
                      0x00,
                      cur,
                      GDK_CURRENT_TIME );
#endif
    gdk_window_set_cursor ( gtk3gui->winAtPosition, cur );
    gdk_display_sync ( dis );
    gdk_cursor_unref ( cur );
    /** must flush **/
    gdk_flush ( );

}

/******************************************************************************/
void gtk3_unsetHourGlass ( GTK3G3DUI *gtk3gui ) {
    G3DUI *gui = ( G3DUI * ) gtk3gui;

    /*GdkDisplay       *dis = gdk_display_get_default();
    GdkDeviceManager *mgr = gdk_display_get_device_manager ( dis );
    GdkDevice        *dev = gdk_device_manager_get_client_pointer ( mgr );
    GdkCursor        *cur = gdk_cursor_new_for_display ( dis, GDK_WATCH );
    gint x, y;

    ggt->winAtPosition = gdk_device_get_window_at_position ( dev, &x, &y ); */
    /* return to normal */
    gdk_window_set_cursor ( gtk3gui->winAtPosition, NULL );

#ifdef unused
    gdk_device_ungrab ( dev, GDK_CURRENT_TIME );
#endif
}

/******************************************************************************/
static void gtk3_updateMouseTool ( GTK3G3DUI *gtk3gui ) {
    if (  gtk3gui->core.main ) {
        G3DUIMAIN *main = ( G3DUIMAIN * ) gtk3gui->core.main;

        if (  main ) {
            G3DUIBOARD *board = main->board;

            if ( board ) {
                GTK3G3DUIMOUSETOOLEDIT *gtk3mtledit = ( GTK3G3DUIMOUSETOOLEDIT * ) board->mtledit;

                if ( gtk3mtledit ) {
                    gtk3_g3duimousetooledit_update ( gtk3mtledit );
                }
            }
        }
    }
}

/******************************************************************************/
static void gtk3_updateUVMouseTool ( GTK3G3DUI *gtk3gui ) {
    if (  gtk3gui->core.mui ) {
        if (  gtk3gui->core.mui ) {
            M3DUI *mui = gtk3gui->core.mui;
            M3DUIMAIN *main = mui->main;

            if (  main ) {
                M3DUIBOARD *board = main->board;

                if ( board ) {
                    GTK3M3DUIMOUSETOOLEDIT *gtk3mtledit = ( GTK3M3DUIMOUSETOOLEDIT * ) board->tooledit;

                    if ( gtk3mtledit ) {
                        gtk3_m3duimousetooledit_update ( gtk3mtledit );
                    }
                }
            }
        }
    }
}

/******************************************************************************/
static void gtk3_updateCoords ( GTK3G3DUI *gtk3gui ) {
    if (  gtk3gui->core.main ) {
        G3DUIMAIN *main = ( G3DUIMAIN * ) gtk3gui->core.main;

        if (  main ) {
            G3DUIBOARD *board = main->board;

            if ( board ) {
                GTK3G3DUICOORDINATESEDIT *gtk3coordsedit = ( GTK3G3DUICOORDINATESEDIT * ) board->coordsedit;

                if ( gtk3coordsedit ) {
                    gtk3_g3duicoordinatesedit_update ( gtk3coordsedit );
                }
            }
        }
    }
}

/******************************************************************************/
static void gtk3_redrawUVMapEditor ( GTK3G3DUI *gtk3gui ) {
    if (  gtk3gui->core.mui ) {
        M3DUI *mui = gtk3gui->core.mui;

        if (  mui->main ) {
            M3DUIMAIN *uvmain = mui->main;

            if ( uvmain ) {
                M3DUIVIEW *uvview = uvmain->view;

                if ( uvview ) {
                    GTK3M3DUIVIEW *gtk3view = ( GTK3M3DUIVIEW * ) uvview;

                    gtk_widget_queue_draw ( GTK_WIDGET(gtk3view->glarea) );
                }
            }
        }
    }
}

/******************************************************************************/
static void gtk3_updateObjectEdit ( GTK3G3DUI *gtk3gui ) {
    if (  gtk3gui->core.main ) {
        G3DUIMAIN *main = ( G3DUIMAIN * ) gtk3gui->core.main;

        if (  main ) {
            G3DUIBOARD *board = main->board;

            if ( board ) {
                G3DUIOBJECTBOARD *objboard = board->objboard;

                if ( objboard ) {
                    GTK3G3DUIOBJECTEDIT *gtk3objedit = ( GTK3G3DUIOBJECTEDIT * ) objboard->objedit;

                    if ( gtk3objedit ) {
                        gtk3_g3duiobjectedit_update ( gtk3objedit );
                    }
                }
            }
        }
    }
}

/******************************************************************************/
static void gtk3_updateMainMenu ( GTK3G3DUI *gtk3gui ) {
    if (  gtk3gui->core.main ) {
        G3DUIMAIN *main = ( G3DUIMAIN * ) gtk3gui->core.main;

        if (  main ) {
            GTK3G3DUIMAIN *gtk3main = ( GTK3G3DUIMAIN * ) main;

            gtk3_g3duimain_updateMenuBar ( gtk3main );
        }
    }
}

/******************************************************************************/
static void gtk3_updateObjectBoardMenu ( GTK3G3DUI *gtk3gui ) {
    if (  gtk3gui->core.main ) {
        G3DUIMAIN *main = ( G3DUIMAIN * ) gtk3gui->core.main;

        if (  main ) {
            G3DUIBOARD *board = main->board;

            if ( board ) {
                GTK3G3DUIOBJECTBOARD *gtk3objboard = ( GTK3G3DUIOBJECTBOARD * ) board->objboard;

                if ( gtk3objboard ) {
                    gtk3_g3duiobjectboard_updateMenuBar ( gtk3objboard );
                }
            }
        }
    }
}

/******************************************************************************/
static void gtk3_redrawObjectList ( GTK3G3DUI *gtk3gui ) {
    if (  gtk3gui->core.main ) {
        G3DUIMAIN *main = ( G3DUIMAIN * ) gtk3gui->core.main;

        if (  main ) {
            G3DUIBOARD *board = main->board;

            if ( board ) {
                G3DUIOBJECTBOARD *objboard = board->objboard;

                if ( objboard ) {
                    G3DUIOBJECTLIST *objlist = objboard->objlist;
                    GTK3G3DUIOBJECTLIST *gtk3objlist = ( GTK3G3DUIOBJECTLIST * ) objlist;

                    if ( gtk3objlist ) {
                        gtk_widget_queue_draw ( gtk3objlist->area );
                    }
                }
            }
        }
    }
}

/******************************************************************************/
static void gtk3_redrawMaterialList ( GTK3G3DUI *gtk3gui ) {
    if (  gtk3gui->core.main ) {
        G3DUIMAIN *main = ( G3DUIMAIN * ) gtk3gui->core.main;

        if (  main ) {
            G3DUIBOARD *board = main->board;

            if ( board ) {
                G3DUIMATERIALBOARD *matboard = board->matboard;

                if ( matboard ) {
                    G3DUIMATERIALLIST *matlist = matboard->matlist;
                    GTK3G3DUIMATERIALLIST *gtk3matlist = ( GTK3G3DUIMATERIALLIST * ) matlist;

                    if ( gtk3matlist ) {
                        gtk_widget_queue_draw ( gtk3matlist->area );
                    }
                }
            }
        }
    }
}

/******************************************************************************/
static void gtk3_updateMaterialList ( GTK3G3DUI *gtk3gui ) {
    if (  gtk3gui->core.main ) {
        G3DUIMAIN *main = ( G3DUIMAIN * ) gtk3gui->core.main;

        if (  main ) {
            G3DUIBOARD *board = main->board;

            if ( board ) {
                G3DUIMATERIALBOARD *matboard = board->matboard;

                if ( matboard ) {
                    G3DUIMATERIALLIST *matlist = matboard->matlist;
                    GTK3G3DUIMATERIALLIST *gtk3matlist = ( GTK3G3DUIMATERIALLIST * ) matlist;

                    gtk3_g3duimateriallist_update ( gtk3matlist );
                }
            }
        }
    }
}

/******************************************************************************/
static void gtk3_updateMaterialEdit ( GTK3G3DUI *gtk3gui ) {
    if (  gtk3gui->core.main ) {
        G3DUIMAIN *main = ( G3DUIMAIN * ) gtk3gui->core.main;

        if (  main ) {
            G3DUIBOARD *board = main->board;

            if ( board ) {
                G3DUIMATERIALBOARD *matboard = board->matboard;

                if ( matboard ) {
                    G3DUIMATERIALEDIT *matedit = matboard->matedit;
                    GTK3G3DUIMATERIALEDIT *gtk3matedit = ( GTK3G3DUIMATERIALEDIT * ) matedit;

                    gtk3_g3duimaterialedit_update ( gtk3matedit );
                }
            }
        }
    }
}

/******************************************************************************/
static void gtk3_updateCurrentMaterialPreview ( GTK3G3DUI *gtk3gui ) {
    if ( gtk3gui->core.lselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) gtk3gui->core.lselmat->data;

        if (  gtk3gui->core.main ) {
            G3DUIMAIN *main = ( G3DUIMAIN * ) gtk3gui->core.main;

            if (  main ) {
                G3DUIBOARD *board = main->board;

                if ( board ) {
                    G3DUIMATERIALBOARD *matboard = board->matboard;

                    if ( matboard ) {
                        G3DUIMATERIALLIST *matlist = matboard->matlist;
                        GTK3G3DUIMATERIALLIST *gtk3matlist = ( GTK3G3DUIMATERIALLIST * ) matlist;

                        gtk3_g3duimateriallist_updatePreview ( gtk3matlist, mat );
                    }
                }
            }
        }
    }
}

/******************************************************************************/
static void gtk3_updateGLViewsMenu ( GTK3G3DUI *gtk3gui ) {
    G3DUI *gui = ( G3DUI * ) &gtk3gui->core;
    LIST *ltmpview = gtk3gui->core.lview;

    while ( ltmpview ) {
        GTK3G3DUIVIEW *gtk3view = ( GTK3G3DUIVIEW * ) ltmpview->data;

        gtk3_g3duiview_updateMenuBar ( gtk3view );

        ltmpview = ltmpview->next;
    }
}

/******************************************************************************/
static void gtk3_redrawRenderWindowMenu ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUIRENDERWINDOW *gtk3rwin = ( GTK3G3DUIRENDERWINDOW * ) gtk3gui->core.renderWindow;

    if ( gtk3rwin ) {
        gtk3_g3duirenderwindow_updateMenuBar ( gtk3rwin );
    }
}

/******************************************************************************/
static void gtk3_updateUVViewMenu ( GTK3G3DUI *gtk3gui ) {
    if ( gtk3gui->core.mui ) {
        M3DUI *mui = gtk3gui->core.mui;

        if ( mui ) {
            M3DUIMAIN *main = mui->main;

            if ( main ) {
                M3DUIVIEW *view = main->view;

                if ( view ) {
                    gtk3_m3duiview_updateMenuBar ( ( GTK3M3DUIVIEW * ) view );
                }
            }
        }
    }
}

/******************************************************************************/
static void gtk3_resizeRenderWindow ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUIRENDERWINDOW *gtk3rwin = ( GTK3G3DUIRENDERWINDOW * ) gtk3gui->core.renderWindow;

    if ( gtk3rwin ) {
        uint32_t width = gtk_widget_get_allocated_width  ( GTK_WIDGET(gtk3rwin->layout) );
        uint32_t height = gtk_widget_get_allocated_height  ( GTK_WIDGET(gtk3rwin->layout) );


        gtk3_g3duirenderwindow_resize ( gtk3rwin,
                                        width,
                                        height );
    }
}

/******************************************************************************/
static void gtk3_redrawGLViews ( GTK3G3DUI *gtk3gui ) {
    G3DUI *gui = ( G3DUI * ) &gtk3gui->core;
    LIST *ltmpview = gtk3gui->core.lview;

    while ( ltmpview ) {
        GTK3G3DUIVIEW *gtk3view = ( GTK3G3DUIVIEW * ) ltmpview->data;
        GdkWindow *window = gtk_widget_get_window ( GTK_WIDGET(gtk3view->glarea) );

        gdk_window_invalidate_rect ( window, NULL, FALSE );


        ltmpview = ltmpview->next;
    }
}

/******************************************************************************/
static void gtk3_redrawTimeline ( GTK3G3DUI *gtk3gui ) {
    G3DUI *gui = ( G3DUI * ) &gtk3gui->core;
    GTK3G3DUIMAIN *gtk3main = ( GTK3G3DUIMAIN * ) gtk3gui->core.main;

    if ( gtk3main ) {
        GTK3G3DUITIMEBOARD *gtk3timeboard = ( GTK3G3DUITIMEBOARD * ) gtk3main->core.timeboard;

        if ( gtk3timeboard ) {
            GTK3G3DUITIMELINE *gtk3timeline = ( GTK3G3DUITIMELINE * ) gtk3timeboard->core.timeline;

            if ( gtk3timeline ) {
                gtk_widget_queue_draw ( GTK_WIDGET(gtk3timeline->area) );
            }
        }
    }
}

/******************************************************************************/
static GTK3G3DUIMENU *buildMenuFromList ( LIST  *lmenu,
                                          G3DUI *gui, 
                                          void  *data ) {
    uint32_t nbitems = list_count ( lmenu );
    GTK3G3DUIMENU *retmenu = NULL;
    static G3DUIMENU rootnode = { NULL,
                                  "Context Menu",
                                  MENU_CLASS_MAIN,
                                  G3DUIMENUTYPE_SUBMENU,
                                  NULL,
                                  NULL,
                                 .nodes = NULL };

    if ( nbitems ) {
        /*** build a temporary menu array ready for parsing ****/
        /*** note: last item is NULL ****/
        G3DUIMENU **items = calloc ( nbitems + 0x01, sizeof ( G3DUIMENU * ) );
        LIST *ltmpmenu = lmenu;
        int i = 0x00;

        while ( ltmpmenu ) {
            G3DUIMENU *menu = ( G3DUIMENU * ) ltmpmenu->data;

            items[i++] = menu;

            ltmpmenu = ltmpmenu->next;
        }

        rootnode.nodes = items;

        /*** then parse it ***/
        retmenu = gtk3_g3duimenu_parse_r ( &rootnode, gui, data );

        free ( items );
    }

    return retmenu;
}

/******************************************************************************/
static void gtk3_buildContextMenus ( GTK3G3DUI *gtk3gui,
                                     void      *data ) {
    G3DUI *gui = ( G3DUI * ) gtk3gui;

    gui->objectModeMenu        = ( G3DUIMENU * ) buildMenuFromList ( gui->lObjectModeMenu,
                                                                     gui, 
                                                                     data );

    gui->vertexModeMeshMenu    = ( G3DUIMENU * ) buildMenuFromList ( gui->lVertexModeMeshMenu,
                                                                     gui,
                                                                     data );

    gui->edgeModeMeshMenu      = ( G3DUIMENU * ) buildMenuFromList ( gui->lEdgeModeMeshMenu,
                                                                     gui,
                                                                     data );

    gui->faceModeMeshMenu      = ( G3DUIMENU * ) buildMenuFromList ( gui->lFaceModeMeshMenu,
                                                                     gui,
                                                                     data );

    gui->sculptModeMeshMenu    = ( G3DUIMENU * ) buildMenuFromList ( gui->lSculptModeMeshMenu,
                                                                     gui,
                                                                     data );

    gui->vertexModeSplineMenu  = ( G3DUIMENU * ) buildMenuFromList ( gui->lVertexModeSplineMenu,
                                                                     gui,
                                                                     data );

    gui->vertexModeMorpherMenu = ( G3DUIMENU * ) buildMenuFromList ( gui->lVertexModeMorpherMenu,
                                                                     gui,
                                                                     data );
}

/******************************************************************************/
void gtk3_createRenderEdit ( GTK3G3DUI *gtk3gui ) {
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    GtkWindow *dial = ui_gtk_dialog_new ( CLASS_MAIN );
    GtkWidget *box = gtk_dialog_get_content_area ( dial );
    GTK3G3DUIRENDEREDIT *gtk3redit = 
                            gtk3_g3duirenderedit_create ( NULL, 
                                                          gtk3gui,
                                                          "Render edit",
                                                          gtk3gui->core.currsg );

    gtk_container_add ( GTK_CONTAINER(box), GTK_WIDGET(gtk3redit->fixed) );

    gtk_window_set_position ( dial, GTK_WIN_POS_CENTER );

    g_signal_connect_swapped ( dial,
                               "response",
                               G_CALLBACK (gtk_widget_destroy),
                               dial);

    gtk_dialog_run ( dial );
}

/******************************************************************************/
uint32_t gtk3_g3dui_saveChannelImageAs ( GTK3G3DUI  *gtk3gui,
                                         G3DIMAGE   *img ) {
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Save file as ...",
                                           GTK_WINDOW(gtk3gui->topWin),
                        /*** from ristretto-0.3.5/src/main_window.c ***/
                                           GTK_FILE_CHOOSER_ACTION_SAVE,
                                           "_Cancel", 
                                           GTK_RESPONSE_CANCEL,
                                           "_Open", 
                                           GTK_RESPONSE_OK,
                                           NULL );

    gtk_window_set_position ( dialog, GTK_WIN_POS_CENTER );

    gtk_file_chooser_set_do_overwrite_confirmation ( GTK_FILE_CHOOSER(dialog), TRUE );

    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_OK ) {
        GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
        char           *filename = gtk_file_chooser_get_filename ( chooser );
        static char     filenameext[0x400] = { 0x00 };

        /*** default to JPG ***/
    #ifdef __linux__
        if ( ( strcasestr ( filename, ".jpg"  ) == NULL ) &&
             ( strcasestr ( filename, ".jpeg" ) == NULL ) &&
             ( strcasestr ( filename, ".png"  ) == NULL ) ) {
    #endif
    #ifdef __MINGW32__
        if ( ( StrStrIA ( filename, ".jpg"  ) == NULL ) &&
             ( StrStrIA ( filename, ".jpeg" ) == NULL ) &&
             ( StrStrIA ( filename, ".png"  ) == NULL ) ) {
    #endif
            snprintf ( filenameext, sizeof ( filenameext ), "%s.jpg", filename );

            g3dimage_setFileName ( img, filenameext );

            img->flags |= JPGIMAGE;
        } else {
    #ifdef __linux__
            if ( strcasestr ( filename, ".jpg"  ) ||
                 strcasestr ( filename, ".jpeg" ) ) {
    #endif
    #ifdef __MINGW32__
            if ( StrStrIA ( filename, ".jpg"  ) ||
                 StrStrIA ( filename, ".jpeg" ) ) {
    #endif
                img->flags |= JPGIMAGE;
            }
    #ifdef __linux__
            if ( strcasestr ( filename, ".png" ) ) {
    #endif
    #ifdef __MINGW32__
            if ( StrStrIA ( filename, ".png" ) ) {
    #endif
                img->flags |= PNGIMAGE;
            }

            g3dimage_setFileName ( img, filename );
        }

        g3dimage_writeToDisk ( img );

        g_free    ( ( gpointer ) filename );
    }

    gtk_widget_destroy ( dialog );


    return 0x00;
}

/******************************************************************************/
uint64_t gtk3_g3dui_saveChannelAlteredImage ( GTK3G3DUI  *gtk3gui,
                                              char       *materialName,
                                              G3DCHANNEL *chn,
                                              uint32_t    ask,
                                              uint32_t    rename ) {

    if ( chn->image ) {
        G3DIMAGE *img = chn->image;
        uint32_t doSave = 0x00;

        if ( rename == 0x00 ) {
            if ( ( img->flags & ALTEREDIMAGE ) ||
                 ( img->filename == NULL     ) ) {
                char *imageName = ( img->filename ) ? img->filename : 
                                                      "Untitled";
                char str[0x400];

                snprintf ( str, 
                           sizeof ( str ),
                           "Save image \"%s\" for channel \"%s:%s\" ?",
                           imageName,
                           materialName,
                           chn->name );

                if ( ask ) {
                    gint response;
                    GtkWidget *dial;
                    dial = gtk_message_dialog_new ( NULL,
                                                    GTK_DIALOG_MODAL | 
                                                    GTK_DIALOG_DESTROY_WITH_PARENT,
                                                    GTK_MESSAGE_QUESTION,
                                                    GTK_BUTTONS_NONE,
                                                    str );

                    gtk_dialog_add_buttons ( dial,
                                             GTK_STOCK_YES,
                                             GTK_RESPONSE_YES,
                                             GTK_STOCK_NO,
                                             GTK_RESPONSE_NO,
                                             GTK_STOCK_CANCEL,
                                             GTK_RESPONSE_CANCEL,
                                             NULL );

                    gtk_window_set_title ( GTK_WINDOW ( dial ), "Save image ?" );

                    gtk_window_set_position ( dial, GTK_WIN_POS_CENTER );

                    response = gtk_dialog_run ( GTK_DIALOG ( dial ) );

                    switch ( response ) {
                        case GTK_RESPONSE_YES:
                            doSave = 0x01;
                        break;

                        case GTK_RESPONSE_NO:
                        break;

                        case GTK_RESPONSE_CANCEL:
                            gtk_widget_destroy ( dial );

                            return 0x01;
                        break;

                        default : 
                        break;
                    }

                    gtk_widget_destroy ( dial );
                } else {
                    doSave = 0x01;
                }

                if ( doSave ) {
                    if ( img->filename ) {
                        g3dimage_writeToDisk ( img );
                    } else {
                        gtk3_g3dui_saveChannelImageAs ( gtk3gui, img );
                    }
                }
            }
        } else {
            gtk3_g3dui_saveChannelImageAs ( gtk3gui, img );
        }
    }

    return 0x00;
}

/******************************************************************************/
uint64_t gtk3_g3dui_saveAlteredImages ( GTK3G3DUI *gtk3gui ) {
    LIST *ltmpmat = gtk3gui->core.sce->lmat;
    uint32_t ret = 0x00;

    while ( ltmpmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpmat->data;

        ret += gtk3_g3dui_saveChannelAlteredImage ( gtk3gui, 
                                                    mat->name,
                                                   &mat->diffuse,
                                                    0x01,
                                                    0x00 );

        ret += gtk3_g3dui_saveChannelAlteredImage ( gtk3gui, 
                                                    mat->name,
                                                   &mat->specular,
                                                    0x01,
                                                    0x00 );

        ret += gtk3_g3dui_saveChannelAlteredImage ( gtk3gui, 
                                                    mat->name,
                                                   &mat->displacement,
                                                    0x01,
                                                    0x00 );

        ret += gtk3_g3dui_saveChannelAlteredImage ( gtk3gui, 
                                                    mat->name,
                                                   &mat->bump,
                                                    0x01,
                                                    0x00 );

        ret += gtk3_g3dui_saveChannelAlteredImage ( gtk3gui, 
                                                    mat->name,
                                                   &mat->alpha,
                                                    0x01,
                                                    0x00 );

        ret += gtk3_g3dui_saveChannelAlteredImage ( gtk3gui, 
                                                    mat->name,
                                                   &mat->reflection,
                                                    0x01,
                                                    0x00 );

        ret += gtk3_g3dui_saveChannelAlteredImage ( gtk3gui, 
                                                    mat->name,
                                                   &mat->refraction,
                                                    0x01,
                                                    0x00 );

        ret += gtk3_g3dui_saveChannelAlteredImage ( gtk3gui, 
                                                    mat->name,
                                                   &mat->ambient,
                                                    0x01,
                                                    0x00 );

        ltmpmat = ltmpmat->next;
    }

    return ret;
}

/******************************************************************************/
void gtk3_interpretUIReturnFlags ( GTK3G3DUI *gtk3gui,
                                   uint64_t   msk ) {
    G3DUI *gui = ( G3DUI * ) gtk3gui;

    if ( msk & REDRAWVIEW ) {
        gtk3_redrawGLViews ( gtk3gui );
    }

    if ( msk & UPDATEVIEWMENU ) {
        gtk3_updateGLViewsMenu ( gtk3gui );
    }

    if ( msk & UPDATECURRENTOBJECT ) {
        gtk3_updateObjectEdit ( gtk3gui );
    }

    if ( msk & UPDATERENDERWINDOWMENU ) {
        gtk3_redrawRenderWindowMenu ( gtk3gui );
    }

    if ( msk & UPDATEOBJECTBOARDMENU ) {
        gtk3_updateObjectBoardMenu ( gtk3gui );
    }

    if ( msk & UPDATEMATERIALLIST ) {
        gtk3_updateMaterialList ( gtk3gui );
        /*gtk3_updateMaterialEdit ( );*/
    }

    if ( msk & UPDATECURRENTMATERIAL ) {
        gtk3_updateMaterialEdit ( gtk3gui );
    }

    if ( msk & UPDATECURRENTMATERIALPREVIEW ) {
        gtk3_updateCurrentMaterialPreview ( gtk3gui );
    }

    if ( msk & UPDATECURRENTMOUSETOOL ) {
        gtk3_updateMouseTool ( gtk3gui );
        gtk3_updateUVMouseTool ( gtk3gui );
    }

    if ( msk & UPDATECOORDS ) {
        gtk3_updateCoords ( gtk3gui );
    }

    if ( msk & UPDATEMAINMENU ) {
        gtk3_updateMainMenu    ( gtk3gui );
    }

    if ( msk & UPDATEUVVIEWMENU ) {
        gtk3_updateUVViewMenu    ( gtk3gui );
    }

    if ( msk & RESIZERENDERWINDOW ) {
        gtk3_resizeRenderWindow ( gtk3gui );
    }

    if ( msk & REDRAWOBJECTLIST ) {
        gtk3_redrawObjectList ( gtk3gui );
    }

    if ( msk & REDRAWMATERIALLIST ) {
        gtk3_redrawMaterialList ( gtk3gui );
    }

    if ( msk & REDRAWTIMELINE ) {
        gtk3_redrawTimeline ( gtk3gui );
    }

    if ( msk & REDRAWUVMAPEDITOR ) {
        gtk3_redrawUVMapEditor ( gtk3gui );
    }

#ifdef TODO
    if ( msk & NOBUFFEREDSUBDIVISION ) {
        /*** this should be replace by some MEANINGFUL mask ***/
        gui->engine_flags |= ONGOINGANIMATION;
    }

    if ( msk & BUFFEREDSUBDIVISIONOK ) {
        /*** this should be replace by some MEANINGFUL mask ***/
        gui->engine_flags &= (~ONGOINGANIMATION);
    }
#endif
}

/******************************************************************************/
static uint64_t setMouseToolCbk ( G3DUIMENU *menu, 
                                  void      *data ) {
    GTK3G3DUIMENU *gtk3menu = ( GTK3G3DUIMENU * ) menu;

    return gtk3_setMouseTool ( ( GTK3G3DUI * ) menu->gui, 
                                               gtk3menu->item,
                                               menu->name );
}

/******************************************************************************/
void gtk3_initDefaultMouseTools ( GTK3G3DUI *gtk3gui ) {
    G3DMOUSETOOL *pickTool               = ( G3DMOUSETOOL * ) g3dmousetoolpick_new ( ),
                 *createSphereTool       = ( G3DMOUSETOOL * ) g3dmousetoolcreatesphere_new ( ),
                 *createCubeTool         = ( G3DMOUSETOOL * ) g3dmousetoolcreatecube_new ( ),
                 *createPlaneTool        = ( G3DMOUSETOOL * ) g3dmousetoolcreateplane_new ( ),
                 *createCylinderTool     = ( G3DMOUSETOOL * ) g3dmousetoolcreatecylinder_new ( ),
                 *createTubeTool         = ( G3DMOUSETOOL * ) g3dmousetoolcreatetube_new ( ),
                 *createTorusTool        = ( G3DMOUSETOOL * ) g3dmousetoolcreatetorus_new ( ),
                 *createBoneTool         = ( G3DMOUSETOOL * ) g3dmousetoolcreatebone_new ( ),
                 *createMakeEditableTool = ( G3DMOUSETOOL * ) g3dmousetoolmakeeditable_new ( ),

                 *cutMeshTool            = ( G3DMOUSETOOL * ) g3dmousetoolcutmesh_new ( ),
                 *createVertexTool       = ( G3DMOUSETOOL * ) g3dmousetoolcreatevertex_new ( ),
                 *revertSplineTool       = ( G3DMOUSETOOL * ) g3dmousetoolrevertspline_new ( ),
                 *bridgeVertexTool       = ( G3DMOUSETOOL * ) g3dmousetoolbridge_new ( ),
                 *extrudeFaceTool        = ( G3DMOUSETOOL * ) g3dmousetoolextrudeface_new ( ),
                 *extrudeInnerTool       = ( G3DMOUSETOOL * ) g3dmousetoolextrudeinner_new ( ),
                 *untriangulateTool      = ( G3DMOUSETOOL * ) g3dmousetooluntriangulate_new ( ),
                 *triangulateTool        = ( G3DMOUSETOOL * ) g3dmousetooltriangulate_new ( ),
                 *roundSplinePointTool   = ( G3DMOUSETOOL * ) g3dmousetoolroundsplinepoint_new ( ),
                 *weldVerticesTool       = ( G3DMOUSETOOL * ) g3dmousetoolweldvertices_new ( ),
                 *weldNeighboursTool     = ( G3DMOUSETOOL * ) g3dmousetoolweldneighbours_new ( ),
                 *invertNormalTool       = ( G3DMOUSETOOL * ) g3dmousetoolinvertnormal_new ( ),
                 *createFacegroupTool    = ( G3DMOUSETOOL * ) g3dmousetoolcreatefacegroup_new ( ),
                 *sculptInflateTool      = ( G3DMOUSETOOL * ) g3dmousetoolsculpt_new ( SCULPTINFLATE ),
                 *sculptCreaseTool       = ( G3DMOUSETOOL * ) g3dmousetoolsculpt_new ( SCULPTCREASE ),
                 *unsculptTool           = ( G3DMOUSETOOL * ) g3dmousetoolsculpt_new ( SCULPTUNSCULPT ),

                 *moveTool               = ( G3DMOUSETOOL * ) g3dmousetoolmove_new ( ),
                 *scaleTool              = ( G3DMOUSETOOL * ) g3dmousetoolscale_new ( ),
                 *rotateTool             = ( G3DMOUSETOOL * ) g3dmousetoolrotate_new ( ),

                 *pickUVTool             = ( G3DMOUSETOOL * ) g3dmousetoolpickUV_new ( ),
                 *moveUVTool             = ( G3DMOUSETOOL * ) g3dmousetoolmoveUV_new ( ),
                 *scaleUVTool            = ( G3DMOUSETOOL * ) g3dmousetoolscaleUV_new ( ),
                 *rotateUVTool           = ( G3DMOUSETOOL * ) g3dmousetoolrotateUV_new ( ),

                 *penUVTool              = ( G3DMOUSETOOL * ) m3dmousetoolpen_new ( ),
                 *eraserUVTool           = ( G3DMOUSETOOL * ) m3dmousetooleraser_new ( ),
                 *selectUVTool           = ( G3DMOUSETOOL * ) m3dmousetoolselect_new ( ),
                 *bucketUVTool           = ( G3DMOUSETOOL * ) m3dmousetoolbucket_new ( );

    G3DUI *gui = &gtk3gui->core;

#ifdef TODO
/*g3dmousetoolremovevertexpose_new*/
#endif

    /*** static because must keep existing in memory *****/
    static G3DUIMENU pickToolMenu  = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
              createSphereToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
              createCubeToolMenu   = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
              createPlaneToolMenu  = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
            createCylinderToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
                createTubeToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
               createTorusToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
                createBoneToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
        createMakeEditableToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
                   cutMeshToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
              createVertexToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
              revertSplineToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
              bridgeVertexToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
               extrudeFaceToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
              extrudeInnerToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
             untriangulateToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
               triangulateToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
          roundSplinePointToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
              weldVerticesToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
            weldNeighboursToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
              invertNormalToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
           createFacegroupToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
             sculptInflateToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
              sculptCreaseToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
                  unsculptToolMenu = { .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL };

    g3dui_addMouseTool ( gui, g3duimousetool_new (  pickTool,
                                                    NULL,
                                                    0x00 ) );

    /*******************************************/

    g3dui_addMouseTool ( gui, g3duimousetool_new (  createMakeEditableTool,
                                                   &createMakeEditableToolMenu,
                                                    OBJECTMODETOOL | 
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  createBoneTool,
                                                   &createBoneToolMenu,
                                                    OBJECTMODETOOL | 
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  createTorusTool,
                                                   &createTorusToolMenu,
                                                    OBJECTMODETOOL | 
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  createTubeTool,
                                                   &createTubeToolMenu,
                                                    OBJECTMODETOOL | 
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  createCylinderTool,
                                                   &createCylinderToolMenu,
                                                    OBJECTMODETOOL | 
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  createPlaneTool,
                                                   &createPlaneToolMenu,
                                                    OBJECTMODETOOL | 
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  createCubeTool,
                                                   &createCubeToolMenu,
                                                    OBJECTMODETOOL | 
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  createSphereTool,
                                                   &createSphereToolMenu,
                                                    OBJECTMODETOOL | 
                                                    GLMENUTOOL ) );

    /*******************************************/


    g3dui_addMouseTool ( gui, g3duimousetool_new (  unsculptTool,
                                                   &unsculptToolMenu,
                                                    SCULPTMODETOOL | 
                                                    MESHTOOL     |
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  sculptCreaseTool,
                                                   &sculptCreaseToolMenu,
                                                    SCULPTMODETOOL | 
                                                    MESHTOOL     |
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  sculptInflateTool,
                                                   &sculptInflateToolMenu,
                                                    SCULPTMODETOOL | 
                                                    MESHTOOL     |
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  createFacegroupTool,
                                                   &createFacegroupToolMenu,
                                                    FACEMODETOOL |
                                                    MESHTOOL     | 
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  invertNormalTool,
                                                   &invertNormalToolMenu,
                                                    FACEMODETOOL |
                                                    MESHTOOL     | 
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  weldNeighboursTool,
                                                   &weldNeighboursToolMenu,
                                                    VERTEXMODETOOL |
                                                    MESHTOOL     | 
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  weldVerticesTool,
                                                   &weldVerticesToolMenu,
                                                    VERTEXMODETOOL |
                                                    MESHTOOL     | 
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  roundSplinePointTool,
                                                   &roundSplinePointToolMenu,
                                                    VERTEXMODETOOL |
                                                    SPLINETOOL     | 
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  triangulateTool,
                                                   &triangulateToolMenu,
                                                    FACEMODETOOL | 
                                                    MESHTOOL     |
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  untriangulateTool,
                                                   &untriangulateToolMenu,
                                                    FACEMODETOOL | 
                                                    MESHTOOL     |
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  extrudeInnerTool,
                                                   &extrudeInnerToolMenu,
                                                    FACEMODETOOL | 
                                                    MESHTOOL     |
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  extrudeFaceTool,
                                                   &extrudeFaceToolMenu,
                                                    FACEMODETOOL | 
                                                    MESHTOOL     |
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  bridgeVertexTool,
                                                   &bridgeVertexToolMenu,
                                                    VERTEXMODETOOL |
                                                    MESHTOOL       |
                                                    SPLINETOOL     |
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  revertSplineTool,
                                                   &revertSplineToolMenu,
                                                    VERTEXMODETOOL |
                                                    SPLINETOOL     |
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  revertSplineTool,
                                                   &revertSplineToolMenu,
                                                    VERTEXMODETOOL |
                                                    SPLINETOOL     |
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  createVertexTool,
                                                   &createVertexToolMenu,
                                                    VERTEXMODETOOL |
                                                    MESHTOOL       |
                                                    SPLINETOOL     |
                                                    GLMENUTOOL ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new (  cutMeshTool,
                                                   &cutMeshToolMenu,
                                                    VERTEXMODETOOL | 
                                                    FACEMODETOOL   | 
                                                    EDGEMODETOOL   |
                                                    MESHTOOL       |
                                                    SPLINETOOL     |
                                                    GLMENUTOOL ) );

    /*******************************************/

    g3dui_addMouseTool ( gui, g3duimousetool_new ( moveTool    , NULL, 0x00 ) );
    g3dui_addMouseTool ( gui, g3duimousetool_new ( scaleTool   , NULL, 0x00 ) );
    g3dui_addMouseTool ( gui, g3duimousetool_new ( rotateTool  , NULL, 0x00 ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new ( pickUVTool  , NULL, 0x00 ) );
    g3dui_addMouseTool ( gui, g3duimousetool_new ( moveUVTool  , NULL, 0x00 ) );
    g3dui_addMouseTool ( gui, g3duimousetool_new ( scaleUVTool , NULL, 0x00 ) );
    g3dui_addMouseTool ( gui, g3duimousetool_new ( rotateUVTool, NULL, 0x00 ) );

    g3dui_addMouseTool ( gui, g3duimousetool_new ( penUVTool   , NULL, 0x00 ) );
    g3dui_addMouseTool ( gui, g3duimousetool_new ( eraserUVTool, NULL, 0x00 ) );
    g3dui_addMouseTool ( gui, g3duimousetool_new ( selectUVTool, NULL, 0x00 ) );
    g3dui_addMouseTool ( gui, g3duimousetool_new ( bucketUVTool, NULL, 0x00 ) );

    gtk3_buildContextMenus ( gtk3gui, NULL );
}
