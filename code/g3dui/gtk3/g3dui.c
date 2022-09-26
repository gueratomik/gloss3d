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
    GtkFrame *frm = gtk_frame_new ( label );

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
    GtkEntry *ent = gtk_entry_new ( );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( ent );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_COMBO_BOX_TEXT(ent);
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
GtkMenu *ui_gtk_menu_new ( char *class ) {
    GtkWidget *menu = gtk_menu_new ( );

    if ( class ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( menu );
        gtk_style_context_add_class ( context, class );
    }

    return GTK_MENU(menu);
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
    gtk_notebook_append_page ( parent, GTK_WIDGET(pan), lab );

    gtk_widget_show ( GTK_WIDGET(lab) );
    gtk_widget_show ( GTK_WIDGET(pan) );


    return pan;
}

/******************************************************************************/
GtkToggleButton *ui_createToggleLabel ( GtkFixed *parent, 
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

    gtk_widget_set_halign ( GTK_WIDGET(lab), GTK_ALIGN_END );

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

        gtk_widget_set_name ( GTK_WIDGET(lab), name );

        gtk_widget_set_size_request ( GTK_WIDGET(lab), lrec.width, lrec.height );

        gtk_fixed_put ( GTK_FIXED(parent), GTK_WIDGET(lab), x, y );

        gtk_widget_show ( GTK_WIDGET(lab) );
    }

    gtk_widget_show ( btn );


    return btn;
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
    GtkSpinButton *btn  = ui_gtk_spin_button_new ( CLASS_ENTRY, adj, 1.0, 0 );
    uint32_t charwidth;

    gtk_entry_set_width_chars   ( GTK_ENTRY(btn), 0 );

    gtk_widget_set_name         ( GTK_WIDGET(btn), name );
    gtk_widget_set_size_request ( GTK_WIDGET(btn), txtwidth, txtheight );
    gtk_widget_set_hexpand      ( GTK_WIDGET(btn), FALSE );

    gtk_fixed_put ( GTK_FIXED(parent), GTK_WIDGET(btn), x + labwidth, y );

    if ( labwidth ) {
        GtkLabel *lab  = ui_gtk_label_new ( class, name );

        gtk_widget_set_name         ( GTK_WIDGET(lab), name );
        gtk_widget_set_size_request ( GTK_WIDGET(lab), labwidth, txtheight );

        gtk_fixed_put ( GTK_FIXED(parent), GTK_WIDGET(lab), x, y );

        gtk_widget_show ( GTK_WIDGET(lab) );
    }

    gtk_widget_show ( btn );

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

        gtk_widget_set_name ( GTK_WIDGET(lab), name );

        gtk_widget_set_size_request ( GTK_WIDGET(lab), lrec.width, lrec.height );

        gtk_fixed_put ( GTK_FIXED(parent), GTK_WIDGET(lab), x, y );

        gtk_widget_show ( GTK_WIDGET(lab) );
    }

    if ( cbk ) { 
        g_signal_connect ( cmb, "changed", G_CALLBACK(cbk), data );
    }

    gtk_widget_show ( cmb );


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
    GtkWidget *ent = ui_gtk_entry_new ( class );

    gtk_widget_set_name ( ent, name );

    gtk_widget_set_size_request ( ent, txtwidth, txtheight );

    /*gtk_entry_set_width_chars ( ent, 12 );*/

    gtk_fixed_put ( GTK_FIXED(parent), ent, x + labwidth, y );

    if ( labwidth ) {
        GtkWidget   *lab  = gtk_label_new ( name );

        gtk_widget_set_name ( lab, name );

        gtk_widget_set_size_request ( lab, labwidth, txtheight );

        gtk_fixed_put ( GTK_FIXED(parent), lab, x, y );

        gtk_widget_show ( lab );
    }

    if ( cbk ) {
        g_signal_connect ( ent, "key-release-event", G_CALLBACK(cbk), data );
    }

    gtk_widget_show ( ent );

    return ent;
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
                                    char       **xpm,
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
                                  char       **xpm,
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
void gtk3_newScene ( GTK3G3DUI *gtk3gui ) {
    G3DUI * gui = ( G3DUI * ) gtk3gui;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_message_dialog_new ( NULL,
                                      GTK_DIALOG_MODAL,
                                      GTK_MESSAGE_QUESTION,
                                      GTK_BUTTONS_YES_NO,
                                      "Close scene and create a new one ?" );


    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_YES ) {
        g3dui_closeScene ( gui );
#ifdef TODO
        g3dui_clearMaterials ( gui );
#endif
        gui->sce = g3dscene_new ( 0x00, "SCENE" );
    }

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
void gtk3_saveAs ( GTK3G3DUI *gtk3gui ) {
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

        gtk_window_set_title ( gtk_widget_get_parent ( gtk3gui->topWin ), windowname );

        g_free    ( ( gpointer ) filename );
    }

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
void gtk3_saveFile ( GTK3G3DUI *gtk3gui ) {
    G3DUI * gui = ( G3DUI * ) gtk3gui;
    GtkWidget *dialog;
    gint       res;

    if ( gui->filename ) {
        printf ( "saving as:%s\n", gui->filename );

        g3dui_saveG3DFile ( gui );
    } else {
        gtk3_saveAs ( gtk3gui );
    }
}

/******************************************************************************/
/*** WTF: https://developer.gnome.org/gtk3/stable/GtkFileChooserDialog.html ***/
void gtk3_openFile ( GTK3G3DUI *gtk3gui ) {
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

            gtk_window_set_title ( gtk_widget_get_parent ( gtk3gui->topWin ), windowname );

            g_free    ( filename );
        }
    }

    gtk_widget_destroy ( dialog );
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
            GtkWidget *dial = ui_gtk_window_new ( CLASS_MAIN, 
                                                  GTK_WINDOW_TOPLEVEL );

            gtk3_g3duirenderwindow_create ( dial, 
                                            gtk3gui, 
                                            "Rendering" );

            gtk_window_set_position ( dial, GTK_WIN_POS_CENTER_ALWAYS );
            gtk_widget_set_size_request ( dial, 800, 600 );

            gtk_widget_show ( dial );
        }
    }
}

/******************************************************************************/
uint32_t gtk3_setMouseTool ( GTK3G3DUI *gtk3gui, char *name ) {
    G3DUI * gui = ( G3DUI * ) gtk3gui;
    G3DUIMOUSETOOL *mou = g3dui_getMouseTool ( gui, name );
    G3DCAMERA *cam = gui->currentView->cam;

    if ( mou ) {
        g3dui_setMouseTool ( gui, cam, mou );

        if ( ( mou->tool->flags & MOUSETOOLNOCURRENT ) == 0x00 ) {
            gtk3_updateAllCurrentMouseTools ( gui );

            return 0x01;
        }
    } else {
        fprintf ( stderr, "No such mousetool %s\n", name );
    }

    return 0x00;
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
static void gtk3_updateAllCurrentMouseTools ( GTK3G3DUI *gtk3gui ) {
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    LIST *ltmpmtools = gtk3gui->lmtools;

    while ( ltmpmtools ) {
        GtkWidget *mtool = ( GtkWidget * ) ltmpmtools->data;

#ifdef TODO
        updateCurrentMouseTool ( mtool, gui );
#endif


        ltmpmtools = ltmpmtools->next;
    }
}

/******************************************************************************/
static void gtk3_updateKeyEdit ( GTK3G3DUI *gtk3gui ) {
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    LIST *ltmpkeyedit = gtk3gui->lkeyedit;

    while ( ltmpkeyedit ) {
        GtkWidget *keyedit = ( GtkWidget * ) ltmpkeyedit->data;

#ifdef TODO
        updateKeyEdit ( keyedit, gui );
#endif

        ltmpkeyedit = ltmpkeyedit->next;
    }
}

/******************************************************************************/
static void gtk3_updateSelectedMaterialPreview ( GTK3G3DUI *gtk3gui ) {
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    LIST *ltmpmatlist = gtk3gui->lmatlist;
    LIST *ltmpselmat = gui->lselmat;

    while ( ltmpselmat ) {
        G3DMATERIAL *mat = ( G3DMATERIAL * ) ltmpselmat->data;

        while ( ltmpmatlist ) {
            GtkWidget *matlst = ( GtkWidget * ) ltmpmatlist->data;

            g3duimateriallist_updatePreview ( matlst, mat );


            ltmpmatlist = ltmpmatlist->next;
        }

        ltmpselmat = ltmpselmat->next;
    }
}

/******************************************************************************/
static void gtk3_clearMaterials ( GTK3G3DUI *gtk3gui ) {
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    LIST *ltmpmatlist = gtk3gui->lmatlist;

    while ( ltmpmatlist ) {
        GtkWidget *matlst = ( GtkWidget * ) ltmpmatlist->data;

        g3duimateriallist_removeAllPreviews ( matlst );


        ltmpmatlist = ltmpmatlist->next;
    }
}

/******************************************************************************/
static void gtk3_importMaterials ( GTK3G3DUI *gtk3gui ) {
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    LIST *ltmpmatlist = gtk3gui->lmatlist;

    while ( ltmpmatlist ) {
        GtkWidget *matlst = ( GtkWidget * ) ltmpmatlist->data;

        g3duimateriallist_importFromScene ( matlst, gui->sce );


        ltmpmatlist = ltmpmatlist->next;
    }
}

/******************************************************************************/
static void gtk3_resizeUVMapEditors ( GTK3G3DUI *gtk3gui ) {
#ifdef TODO
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    LIST *ltmp = gtk3gui->luvmapeditor;

    while ( ltmp ) {
        GtkUVMapEditor *guv = ( GtkUVMapEditor * ) ltmp->data;

        /*** resize buffers ***/
        m3dui_resizeBuffers ( &guv->mui );

        ltmp = ltmp->next;
    }
#endif
}

/******************************************************************************/
static void gtk3_redrawUVMapEditors ( GTK3G3DUI *gtk3gui ) {
#ifdef TODO
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    LIST *ltmp = gtk3gui->luvmapeditor;

    while ( ltmp ) {
        GtkWidget *guv = ( GtkWidget * ) ltmp->data;
        GtkWidget *area   = gtk_uvmapeditor_getGLArea ( guv );
        GdkRectangle arec;

        arec.x = arec.y = 0x00;
        arec.width = arec.height = 0x01;

        gdk_window_invalidate_rect ( gtk_widget_get_window ( area ), &arec, FALSE );

        ltmp = ltmp->next;
    }
#endif
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
                    G3DUIOBJECTEDIT *objedit = objboard->objedit;

                    if ( objedit ) {
                        gtk3_g3duiobjectedit_update ( ( GTK3G3DUIOBJECTEDIT * ) objedit );
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
    gtk3_g3duirenderwindow_updateMenuBar ( gtk3gui->core.renderWindow );
}

/******************************************************************************/
static void gtk3_resizeRenderWindow ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUIRENDERWINDOW *gtk3rwin = gtk3gui->core.renderWindow;

    gtk3_g3duirenderwindow_resize ( gtk3rwin,
                                    gtk_widget_get_allocated_width  ( gtk3rwin->layout ),
                                    gtk_widget_get_allocated_height ( gtk3rwin->layout ) );
}

/******************************************************************************/
static void gtk3_redrawGLViews ( GTK3G3DUI *gtk3gui ) {
    G3DUI *gui = ( G3DUI * ) &gtk3gui->core;
    LIST *ltmpview = gtk3gui->core.lview;

    while ( ltmpview ) {
        GTK3G3DUIVIEW *gtk3view = ( GTK3G3DUIVIEW * ) ltmpview->data;
        GdkWindow *window = gtk_widget_get_window ( gtk3view->glarea );
        GdkRectangle arec;

        arec.x = arec.y = 0x00;
        arec.width = arec.height = 1;

        gdk_window_invalidate_rect ( window, &arec, FALSE );


        ltmpview = ltmpview->next;
    }
}

/******************************************************************************/
static void gtk3_redrawTimeline ( GTK3G3DUI *gtk3gui ) {
    G3DUI *gui = ( G3DUI * ) &gtk3gui->core;
    GTK3G3DUIMAIN *gtk3main = gtk3gui->core.main;

    if ( gtk3main ) {
        GTK3G3DUITIMEBOARD *gtk3timeboard = gtk3main->core.timeboard;

        if ( gtk3timeboard ) {
            GTK3G3DUITIMELINE *gtk3timeline = gtk3timeboard->core.timeline;

            if ( gtk3timeline ) {
                gtk_widget_queue_draw ( gtk3timeline->area );
                /*gdk_window_invalidate_rect ( window, &arec, FALSE );*/
            }
        }
    }
}

/******************************************************************************/
static GTK3G3DUIMENU *buildMenuFromList ( LIST  *lmenu,
                                          G3DUI *gui, 
                                          void  *data ) {
    uint32_t nbitems = list_count ( lmenu );
    GTK3G3DUIMENU *retmenu = NULL;;
    static G3DUIMENU rootnode = { NULL,
                                  "Context Menu",
                                  MENU_CLASS_MAIN,
                                  G3DUIMENUTYPE_SUBMENU,
                                  NULL,
                                  NULL,
                                 .nodes = NULL };

    if ( nbitems ) {
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

        retmenu = gtk3_g3duimenu_parse_r ( &rootnode, gui, data );

        free ( items );
    }

    return retmenu;
}

/******************************************************************************/
static void gtk3_buildContextMenus ( GTK3G3DUI *gtk3gui,
                                     void      *data ) {
    G3DUI *gui = ( G3DUI * ) gtk3gui;

    gui->objectModeMenu        = buildMenuFromList ( gui->lObjectModeMenu,
                                                     gui, 
                                                     data );

    gui->vertexModeMeshMenu    = buildMenuFromList ( gui->lVertexModeMeshMenu,
                                                     gui,
                                                     data );

    gui->edgeModeMeshMenu      = buildMenuFromList ( gui->lEdgeModeMeshMenu,
                                                     gui,
                                                     data );

    gui->faceModeMeshMenu      = buildMenuFromList ( gui->lFaceModeMeshMenu,
                                                     gui,
                                                     data );

    gui->sculptModeMeshMenu    = buildMenuFromList ( gui->lSculptModeMeshMenu,
                                                     gui,
                                                     data );

    gui->vertexModeSplineMenu  = buildMenuFromList ( gui->lVertexModeSplineMenu,
                                                     gui,
                                                     data );

    gui->vertexModeMorpherMenu = buildMenuFromList ( gui->lVertexModeMorpherMenu,
                                                     gui,
                                                     data );
}

/******************************************************************************/
void gtk3_createRenderEdit ( GTK3G3DUI *gtk3gui ) {
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    GtkWidget *dial = ui_gtk_window_new ( CLASS_MAIN, GTK_WINDOW_TOPLEVEL );
    GTK3G3DUIRENDEREDIT *gtk3redit = 
                            gtk3_g3duirenderedit_create ( NULL, 
                                                          gtk3gui,
                                                          "Render edit",
                                                          gtk3gui->core.currsg );

    gtk_container_add ( dial, gtk3redit->fixed );

    gtk_widget_show ( dial );
}

/******************************************************************************/
void gtk3_interpretUIReturnFlags ( GTK3G3DUI *gtk3gui,
                                   uint64_t   msk ) {
    G3DUI *gui = ( G3DUI * ) gtk3gui;

    if ( msk & REDRAWVIEW ) {
        gtk3_redrawGLViews ( gtk3gui );
    }

    if ( msk & REDRAWVIEWMENU ) {

        gtk3_updateGLViewsMenu ( gtk3gui );
    }

    if ( msk & REDRAWCURRENTOBJECT ) {
        gtk3_updateObjectEdit ( gtk3gui );
    }

    if ( msk & REDRAWTIMELINE ) {
        gtk3_redrawTimeline ( gtk3gui );
    }

    if ( msk & RESIZERENDERWINDOW ) {
        gtk3_resizeRenderWindow ( gtk3gui );
    }

    if ( msk & REDRAWRENDERWINDOWMENU ) {
        gtk3_redrawRenderWindowMenu ( gtk3gui );
    }

    if ( msk & CREATERENDEREDIT ) {
        gtk3_createRenderEdit ( gtk3gui );
    }
#ifdef TODO
    if ( msk & REDRAWCURRENTMATERIAL ) {
        gtk3_updateMaterialEdit ( );
        gtk3_updateSelectedMaterialPreview ( );
    }

    if ( msk & REDRAWMATERIALLIST ) {
        gtk3_redrawMaterialList ( );
    }

    if ( msk & REBUILDMATERIALLIST ) {
        gtk3_clearMaterials     ( );
        gtk3_importMaterials    ( );
        gtk3_redrawMaterialList ( );
    }

    if ( msk & REDRAWUVMAPEDITOR ) {
        gtk3_redrawUVMapEditors ( );
    }

    if ( msk & REDRAWMENU ) {
        gtk3_updateMenuBar    ( );
    }

    if ( msk & REDRAWLIST ) {
        gtk3_redrawObjectList ( );
    }

    if ( msk & REDRAWCURRENTMOUSETOOL ) {
        gtk3_updateAllCurrentMouseTools ( );
    }

    if ( msk & REDRAWCOORDS ) {
        gtk3_updateCoords ( );
    }

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
    g3dui_setMouseTool ( menu->gui, 
                         menu->gui->currentView->cam, 
                         g3dui_getMouseTool ( menu->gui, menu->name ) );

    return 0x00;
}

/******************************************************************************/
void gtk3_initDefaultMouseTools ( GTK3G3DUI *gtk3gui ) {
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    G3DMOUSETOOL *pickTool               = g3dmousetoolpick_new ( ),
                 *createSphereTool       = g3dmousetoolcreatesphere_new ( ),
                 *createCubeTool         = g3dmousetoolcreatecube_new ( ),
                 *createPlaneTool        = g3dmousetoolcreateplane_new ( ),
                 *createCylinderTool     = g3dmousetoolcreatecylinder_new ( ),
                 *createTubeTool         = g3dmousetoolcreatetube_new ( ),
                 *createTorusTool        = g3dmousetoolcreatetorus_new ( ),
                 *createBoneTool         = g3dmousetoolcreatebone_new ( ),
                 *createMakeEditableTool = g3dmousetoolmakeeditable_new ( ),

                 *cutMeshTool            = g3dmousetoolcutmesh_new ( ),
                 *createVertexTool       = g3dmousetoolcreatevertex_new ( ),
                 *revertSplineTool       = g3dmousetoolrevertspline_new ( ),
                 *bridgeVertexTool       = g3dmousetoolbridge_new ( ),
                 *extrudeFaceTool        = g3dmousetoolextrudeface_new ( ),
                 *extrudeInnerTool       = g3dmousetoolextrudeinner_new ( ),
                 *untriangulateTool      = g3dmousetooluntriangulate_new ( ),
                 *triangulateTool        = g3dmousetooltriangulate_new ( ),
                 *roundSplinePointTool   = g3dmousetoolroundsplinepoint_new ( ),
                 *weldVerticesTool       = g3dmousetoolweldvertices_new ( ),
                 *weldNeighboursTool     = g3dmousetoolweldneighbours_new ( ),
                 *invertNormalTool       = g3dmousetoolinvertnormal_new ( ),
                 *createFacegroupTool    = g3dmousetoolcreatefacegroup_new ( ),
                 *sculptInflateTool      = g3dmousetoolsculpt_new ( SCULPTINFLATE ),
                 *sculptCreaseTool       = g3dmousetoolsculpt_new ( SCULPTCREASE ),
                 *unsculptTool           = g3dmousetoolsculpt_new ( SCULPTUNSCULPT ),

                 *moveTool               = g3dmousetoolmove_new ( ),
                 *scaleTool              = g3dmousetoolscale_new ( ),
                 *rotateTool             = g3dmousetoolrotate_new ( ),
                 *pickUVTool             = g3dmousetoolpickUV_new ( ),
                 *moveUVTool             = g3dmousetoolmoveUV_new ( ),
                 *scaleUVTool            = g3dmousetoolscaleUV_new ( ),
                 *rotateUVTool           = g3dmousetoolrotateUV_new ( );

#ifdef TODO
/*g3dmousetoolremovevertexpose_new*/
#endif

    G3DUIMENU pickToolMenu         = { .name      = pickTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
              createSphereToolMenu = { .name      = createSphereTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
              createCubeToolMenu   = { .name      = createCubeTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
              createPlaneToolMenu  = { .name      = createPlaneTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
            createCylinderToolMenu = { .name      = createCylinderTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
                createTubeToolMenu = { .name      = createTubeTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
               createTorusToolMenu = { .name      = createTorusTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
                createBoneToolMenu = { .name      = createBoneTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
        createMakeEditableToolMenu = { .name      = createMakeEditableTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
                   cutMeshToolMenu = { .name      = cutMeshTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
              createVertexToolMenu = { .name      = createVertexTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
              revertSplineToolMenu = { .name      = revertSplineTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
              bridgeVertexToolMenu = { .name      = bridgeVertexTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
               extrudeFaceToolMenu = { .name      = extrudeFaceTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
              extrudeInnerToolMenu = { .name      = extrudeInnerTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
             untriangulateToolMenu = { .name      = untriangulateTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
               triangulateToolMenu = { .name      = triangulateTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
          roundSplinePointToolMenu = { .name      = roundSplinePointTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
              weldVerticesToolMenu = { .name      = weldVerticesTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
            weldNeighboursToolMenu = { .name      = weldNeighboursTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
              invertNormalToolMenu = { .name      = invertNormalTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
           createFacegroupToolMenu = { .name      = createFacegroupTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
             sculptInflateToolMenu = { .name      = sculptInflateTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
              sculptCreaseToolMenu = { .name      = sculptCreaseTool->name,
                                       .class     = CLASS_MAIN,
                                       .type      = G3DUIMENUTYPE_PUSHBUTTON,
                                       .condition = NULL,
                                       .callback  = setMouseToolCbk,
                                       .data      = NULL },
                  unsculptToolMenu = { .name      = unsculptTool->name,
                                       .class     = CLASS_MAIN,
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

    gtk3_buildContextMenus ( gtk3gui, NULL );

#ifdef unused

/*
                  penTool = m3dmousetoolpen_new ( ) );
                  eraserTool = m3dmousetooleraser_new ( ) );
                  selectTool = m3dmousetoolselect_new ( ) );
                  bucketTool = m3dmousetoolbucket_new ( ) );
*/


    mou = ( G3DMOUSETOOL * ) g3dmousetoolpick_new ( );

    g3dui_addMouseTool ( gui, mou );

    /*** Pick is the default mouse tool ***/
    gtk3_interpretUIReturnFlags ( gtk3gui, g3dui_setMouseTool ( gui,
                                                                NULL,
                                                                mou ) );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolcreatesphere_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, OBJECTMODETOOL |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolcreatecube_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, OBJECTMODETOOL | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolcreateplane_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, OBJECTMODETOOL | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolcreatecylinder_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, OBJECTMODETOOL | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolcreatetube_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, OBJECTMODETOOL | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolcreatetorus_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, OBJECTMODETOOL | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolcreatebone_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, OBJECTMODETOOL | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolmakeeditable_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, OBJECTMODETOOL |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolcutmesh_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, VERTEXMODETOOL | 
                                     FACEMODETOOL   | 
                                     EDGEMODETOOL   |
                                     MESHTOOL       |
                                     SPLINETOOL     |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolcreatevertex_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, VERTEXMODETOOL |
                                     MESHTOOL       |
                                     SPLINETOOL     |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolrevertspline_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, VERTEXMODETOOL |
                                     SPLINETOOL     |
                                     GLMENUTOOL );

    /********************************/
#ifdef TODO
    g3dui_addMouseTool   ( gui, mou = g3dmousetoolremovevertexpose_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, VERTEXMODETOOL |
                                     MORPHERTOOL     |
                                     GLMENUTOOL );
#endif

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolbridge_new ( ) ); 
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, VERTEXMODETOOL |
                                     MESHTOOL       |
                                     SPLINETOOL     |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolextrudeface_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, FACEMODETOOL | 
                                     MESHTOOL     |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolextrudeinner_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, FACEMODETOOL | 
                                     MESHTOOL     |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetooluntriangulate_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, FACEMODETOOL |
                                     MESHTOOL     | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetooltriangulate_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, FACEMODETOOL |
                                     MESHTOOL     | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolroundsplinepoint_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, VERTEXMODETOOL |
                                     SPLINETOOL     | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolweldvertices_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, VERTEXMODETOOL |
                                     MESHTOOL     | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolweldneighbours_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, VERTEXMODETOOL |
                                     MESHTOOL     | 
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolinvertnormal_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, FACEMODETOOL |
                                     MESHTOOL     |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolcreatefacegroup_new ( ) ); 
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, FACEMODETOOL | 
                                     MESHTOOL     |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolsculpt_new ( SCULPTINFLATE ) ); 
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, SCULPTMODETOOL | 
                                     MESHTOOL     |
                                     GLMENUTOOL );

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolsculpt_new ( SCULPTCREASE ) ); 
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, SCULPTMODETOOL | 
                                     MESHTOOL     |
                                     GLMENUTOOL );

    /********************************/

    /*common_g3dui_addMouseTool ( gui,
                                g3dmousetoolsculpt_new ( SCULPTFLATTEN ), 
                                SCULPTMODETOOL | 
                                MESHTOOL     |
                                GLMENUTOOL );*/

    /********************************/

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolsculpt_new ( SCULPTUNSCULPT ) ); 
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, SCULPTMODETOOL | 
                                     MESHTOOL     |
                                     GLMENUTOOL );

    /********************************/

    /*common_g3dui_addMouseTool ( gui,
                                g3dmousetoolsculpt_new ( SCULPTSMOOTH ), 
                                SCULPTMODETOOL | 
                                MESHTOOL     |
                                GLMENUTOOL );*/

    /********************************/

    g3dui_addMouseTool   ( gui, g3dmousetoolmove_new ( ) );

    /********************************/

    g3dui_addMouseTool   ( gui, g3dmousetoolscale_new ( ) );

    /********************************/

    g3dui_addMouseTool   ( gui, g3dmousetoolrotate_new ( ) );

    /********************************/

    g3dui_addMouseTool   ( gui, g3dmousetoolpickUV_new ( ) );

    /********************************/

    g3dui_addMouseTool   ( gui, g3dmousetoolmoveUV_new ( ) );

    /********************************/

    g3dui_addMouseTool   ( gui, g3dmousetoolscaleUV_new ( ) );

    /********************************/

    g3dui_addMouseTool   ( gui, g3dmousetoolrotateUV_new ( ) );

    /********************************/

    g3dui_addMouseTool   ( gui, m3dmousetoolpen_new ( ) );

    /********************************/

    g3dui_addMouseTool   ( gui, m3dmousetooleraser_new ( ) );

    /********************************/

    g3dui_addMouseTool   ( gui, m3dmousetoolselect_new ( ) );

    /********************************/

    g3dui_addMouseTool   ( gui, m3dmousetoolbucket_new ( ) );

#endif
}
