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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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

G_DEFINE_TYPE (GtkGlossUI, gtk_glossui, GTK_TYPE_FIXED)

/******************************************************************************/
static void     gtk_glossui_class_init    ( GtkGlossUIClass * );
static void     gtk_glossui_init          ( GtkGlossUI * );
static void     gtk_glossui_realize       ( GtkWidget * );
static void     gtk_glossui_size_request  ( GtkWidget *, GtkRequisition * );
static gboolean gtk_glossui_expose        ( GtkWidget *, cairo_t * );
static gboolean gtk_glossui_configure     ( GtkWidget *, GdkEvent * );
static void     gtk_glossui_size_allocate ( GtkWidget *, GtkAllocation * );
static void     gtk_glossui_show          ( GtkWidget * );

/******************************************************************************/
GtkWidget* getChild ( GtkWidget* parent, const gchar* name) {
    if ( strcmp ( gtk_widget_get_name ( parent ), name ) == 0 ) {
        return parent;
    }

    if ( GTK_IS_BIN ( parent ) ) {
        GtkWidget *child = gtk_bin_get_child(GTK_BIN(parent));

        return getChild ( child, name );
    }

    if ( GTK_IS_CONTAINER ( parent ) ) {
        GList *children = gtk_container_get_children ( GTK_CONTAINER ( parent ) );
        while ( ( children = g_list_next ( children ) ) != NULL ) {
            GtkWidget* widget = getChild ( children->data, name );
            if ( widget != NULL ) {
                return widget;
            }
        }
    }

    return NULL;
}

/******************************************************************************/
void g3dui_renderViewCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI     *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;
    uint32_t width  = gtk_widget_get_allocated_width  ( ggt->curogl ),
             height = gtk_widget_get_allocated_height ( ggt->curogl );
    G3DUIRENDERPROCESS *rps = common_g3dui_getRenderProcessByID ( gui, ( uint64_t ) ggt->curogl );
    R3DFILTER *progressiveDisplay = r3dfilter_toGtkWidget_new ( ggt->curogl, 0x01 );
    /*R3DFILTER *finalDisplay = r3dfilter_toGtkWidget_new ( ggt->curogl, 0x01, 0x00 );*/
    /*** Filter to free R3DSCENE, Filters & G3DUIRENDERPROCESS ***/
    R3DFILTER *clean = r3dfilter_new ( FILTERIMAGE, "CLEAN", g3dui_renderClean,
                                       NULL, 
                                       gui );
    LIST *lfilters = NULL;
    G3DSYSINFO *sysinfo = g3dsysinfo_get ( );
    float backgroundWidthRatio = ( ( float ) sysinfo->renderRectangle[0x01].x -
                                             sysinfo->renderRectangle[0x00].x ) / width;
    /* declared static because must survive */
    static R3DRENDERSETTINGS viewRsg;

    /* First cancel running render on that window  if any */
    if ( rps ) {
        r3dscene_cancelRender ( rps->rsce );
    }

    r3drendersettings_copy ( &viewRsg, gui->currsg );

    list_append ( &lfilters, progressiveDisplay );
    /*list_append ( &lfilters, r3dfilter_VectorMotionBlur_new ( width, height) );
    list_append ( &lfilters, finalDisplay );*/
    list_append ( &lfilters, clean );

    viewRsg.input.sce      = gui->sce;
    viewRsg.input.cam      = g3dui_getMainViewCamera ( gui );
    viewRsg.input.lfilters = lfilters;

    viewRsg.background.widthRatio = backgroundWidthRatio;

    viewRsg.output.x1 = 0x00;
    viewRsg.output.x2 = width - 1;
    viewRsg.output.y1 = 0x00;
    viewRsg.output.y2 = height - 1;
    viewRsg.output.width = width;
    viewRsg.output.height = height;
    viewRsg.background.image = sysinfo->backgroundImage;

    g3dui_setHourGlass ( gui );

    rps = common_g3dui_render ( gui, &viewRsg,
                                     gui->curframe,
                        ( uint64_t ) ggt->curogl,
                                     0x00 );

    g3dui_unsetHourGlass ( gui );
}

/******************************************************************************/
uint32_t g3dui_renderClean ( R3DFILTER *fil, R3DSCENE *rsce,
                                             float frameID,
                                             unsigned char *img, 
                                             uint32_t from, 
                                             uint32_t to, 
                                             uint32_t depth, 
                                             uint32_t width ) {
    G3DUI *gui = ( G3DUI * ) fil->data;
    G3DUIRENDERPROCESS *rps = common_g3dui_getRenderProcessByScene ( gui, rsce );

    /*** clean renderprocess if any ***/
    if ( rps ) {
        g3duirenderprocess_free ( rps );

        list_remove ( &gui->lrps, rps );

        return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
GtkWidget *createHorizontalScale ( GtkWidget *parent, G3DUI *gui,
                                                      char *name,
                                                      gint x, 
                                                      gint y,
                                                      gint width,
                                                      gint height,
                                                      float min,
                                                      float max,
                                                      float step,
                                                      void (*cbk)( GtkWidget *, 
                                                                   gpointer ) ) {
    GtkWidget *scl = gtk_scale_new_with_range ( GTK_ORIENTATION_HORIZONTAL, 
                                                min,
                                                max,
                                                step );

    gtk_widget_set_name ( scl, name );

    gtk_scale_set_value_pos ( GTK_SCALE(scl), GTK_POS_RIGHT );

    gtk_widget_set_size_request ( scl, width, height );

    if ( cbk ) {
        g_signal_connect ( scl, "value-changed", G_CALLBACK ( cbk ), gui );
    }

    gtk_fixed_put ( GTK_FIXED(parent), scl, x, y );

    gtk_widget_show ( scl );


    return scl;
}

/******************************************************************************/
GtkWidget *createColorButton ( GtkWidget *parent, G3DUI *gui,
                                                  char *name,
                                                  gint x, 
                                                  gint y,
                                                  gint width,
                                                  gint height,
                                                  void (*cbk)( GtkWidget *, 
                                                               gpointer ) ) {
    GtkWidget *btn = gtk_color_button_new ( );

    gtk_widget_set_name ( btn, name );

    gtk_widget_set_size_request ( btn, width, height );

    if ( cbk ) {
        g_signal_connect ( btn, "color-set", G_CALLBACK ( cbk ), gui );
    }

    gtk_fixed_put ( GTK_FIXED(parent), btn, x, y );

    gtk_widget_show ( btn );


    return btn;
}

/******************************************************************************/
GtkWidget *createStatusBar   ( GtkWidget *parent, G3DUI *gui,
                                                  char *name,
                                                  gint x, 
                                                  gint y,
                                                  gint width,
                                                  gint height ) {
    GtkWidget *bar = gtk_statusbar_new ( );

    gtk_widget_set_name ( bar, name );

    gtk_widget_set_size_request ( bar, width, height );

    gtk_fixed_put ( GTK_FIXED(parent), bar, x, y );

    gtk_widget_show ( bar );


    return bar;
}

/******************************************************************************/
GtkWidget *createToggleLabel ( GtkWidget *parent, G3DUI *gui,
                                                  char *name,
                                                  gint x, 
                                                  gint y,
                                                  gint width,
                                                  gint height,
                                                  void (*cbk)( GtkWidget *, 
                                                               gpointer ) ) {
    GtkWidget *btn = gtk_check_button_new_with_label ( name );

    gtk_widget_set_name ( btn, name );

    gtk_widget_set_size_request ( btn, width, height );

    gtk_toggle_button_set_mode ( GTK_TOGGLE_BUTTON(btn), TRUE );

    if ( cbk ) {
        g_signal_connect ( btn, "toggled", G_CALLBACK ( cbk ), gui );
    }

    gtk_fixed_put ( GTK_FIXED(parent), btn, x, y );

    gtk_widget_show ( btn );


    return btn;
}

/******************************************************************************/
GtkWidget *createRadioLabel ( GtkWidget *parent, G3DUI *gui,
                                                  char *name,
                                                  GtkWidget *master,
                                                  gint x, 
                                                  gint y,
                                                  gint width,
                                                  gint height,
                                                  void (*cbk)( GtkWidget *, 
                                                               gpointer ) ) {
    GSList    *grp = ( master ) ? gtk_radio_button_get_group ( GTK_RADIO_BUTTON(master) ) : NULL; 
    GtkWidget *btn = gtk_radio_button_new_with_label ( grp, name );

    gtk_widget_set_name ( btn, name );

    gtk_widget_set_size_request ( btn, width, height );

    if ( cbk ) {
        g_signal_connect ( btn, "toggled", G_CALLBACK ( cbk ), gui );
    }

    gtk_fixed_put ( GTK_FIXED(parent), btn, x, y );

    gtk_widget_show ( btn );


    return btn;
}

/******************************************************************************/
GtkWidget *createPushButton ( GtkWidget *parent, G3DUI *gui,
                                                 char *name,
                                                 gint x, 
                                                 gint y,
                                                 gint width,
                                                 gint height,
                                                 void (*cbk)( GtkWidget *, 
                                                              gpointer ) ) {
    return createButton ( parent, gui, name, NULL, x, y, width, height, cbk );
}

/******************************************************************************/
GtkWidget *createImageButton ( GtkWidget *parent, G3DUI *gui,
                                                  char *name,
                                                  const char **xpm,
                                                  gint x, 
                                                  gint y,
                                                  gint width,
                                                  gint height,
                                                  void (*cbk)( GtkWidget *, 
                                                               gpointer ) ) {
    return createButton ( parent, gui, name, xpm, x, y, width, height, cbk );
}

/******************************************************************************/
GtkWidget *createButton ( GtkWidget *parent, G3DUI *gui,
                                             char *name,
                                             const char **xpm,
                                             gint x, 
                                             gint y,
                                             gint width,
                                             gint height,
                                             void (*cbk)( GtkWidget *, 
                                                          gpointer ) ) {
    GtkWidget *btn;

    if ( xpm ) btn = gtk_button_new ( );
    else       btn = gtk_button_new_with_label ( name );


    gtk_widget_set_name ( btn, name );

    gtk_widget_set_size_request ( btn, width, height );

    if ( cbk ) {
        g_signal_connect ( btn, "clicked", G_CALLBACK ( cbk ), gui );
    }

    gtk_fixed_put ( GTK_FIXED(parent), btn, x, y );

    if ( xpm ) {
        GdkPixbuf *img = gdk_pixbuf_new_from_xpm_data ( xpm );

        if ( img ) {
            GtkWidget *imgwid = gtk_image_new_from_pixbuf ( img );

            gtk_widget_show ( imgwid );

            gtk_button_set_image ( GTK_BUTTON(btn), imgwid );
        }
    }


    gtk_widget_show ( btn );


    return btn;
}

/******************************************************************************/
void createSimpleLabel ( GtkWidget *parent, G3DUI *gui,
                                            char *name,
                                            gint x, 
                                            gint y,
                                            gint width,
                                            gint height ) {
    GdkRectangle lrec = { 0x00, 0x00, width, height };
    GtkWidget   *lab  = gtk_label_new ( name );

    gtk_widget_set_name ( lab, name );

    gtk_widget_size_allocate ( lab, &lrec );

    gtk_fixed_put ( GTK_FIXED(parent), lab, x, y );

    gtk_widget_show ( lab );
}

/******************************************************************************/
GtkWidget *createComboBox ( GtkWidget *parent, G3DUI *gui,
                                               char *name,
                                               gint x, 
                                               gint y,
                                               gint width,
                                               gint height ) {
    GdkRectangle lrec = { 0x00, 0x00, width, height };
    GtkWidget   *cmb  = gtk_combo_box_text_new ( );

    gtk_widget_set_name ( cmb, name );

    gtk_widget_size_allocate ( cmb, &lrec );

    gtk_fixed_put ( GTK_FIXED(parent), cmb, x, y );

    gtk_widget_show ( cmb );

    return cmb;
}

/******************************************************************************/
GtkWidget *createFixed ( GtkWidget *parent, G3DUI *gui,
                                            char *name,
                                            gint x, 
                                            gint y,
                                            gint width,
                                            gint height ) {
    G3DUIGTK3   *ggt  = ( G3DUIGTK3 * ) gui->toolkit_data;
    GdkRectangle frec = { 0x00, 0x00, width, height };
    GtkWidget   *frm  = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &frec );

    gtk_fixed_put ( GTK_FIXED(parent), frm, x, y );

    gtk_widget_show ( frm );

    return frm;
}

/******************************************************************************/
GtkWidget *createFrame ( GtkWidget *parent, G3DUI *gui,
                                            char *name,
                                            gint x, 
                                            gint y,
                                            gint width,
                                            gint height ) {
    G3DUIGTK3   *ggt  = ( G3DUIGTK3 * ) gui->toolkit_data;
    GdkRectangle frec = { 0x00, 0x00, width, height };
    GtkWidget   *frm  = gtk_frame_new ( name );
    GtkWidget   *fix  = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &frec );

    /*** must ***/
    gtk_widget_set_size_request ( frm, width, height );

    gtk_fixed_put ( GTK_FIXED(parent), frm, x, y );



    gtk_widget_set_name ( fix, name );

    gtk_widget_size_allocate ( fix, &frec );

    gtk_container_add ( GTK_CONTAINER(frm), fix );

    gtk_widget_show ( fix );



    gtk_widget_show ( frm );

    /*** we return the fixed container because we can add multiple children ***/
    return fix;
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
void createCharText ( GtkWidget *parent, G3DUI *gui,
                                               char *name,
                                               gint x, gint y,
                                               gint labwidth,
                                               gint txtwidth,
                                               void (*cbk)( GtkWidget *,
                                                            GdkEvent  *, 
                                                            gpointer ) ) {
    GdkRectangle brec = { 0x00, 0x00, txtwidth, 0x12 };
    GtkWidget *ent = gtk_entry_new ( );

    gtk_widget_set_name ( ent, name );

    gtk_widget_size_allocate ( ent, &brec );

    /*gtk_entry_set_width_chars ( ent, 12 );*/

    gtk_fixed_put ( GTK_FIXED(parent), ent, x + labwidth, y );

    if ( labwidth ) {
        GdkRectangle lrec = { 0x00, 0x00, labwidth, 0x12 };
        GtkWidget   *lab  = gtk_label_new ( name );

        gtk_widget_set_name ( lab, name );

        gtk_widget_size_allocate ( lab, &lrec );

        gtk_fixed_put ( GTK_FIXED(parent), lab, x, y );

        gtk_widget_show ( lab );
    }

    if ( cbk ) {
        g_signal_connect ( ent, "key-release-event", G_CALLBACK(cbk), gui );
    }

    gtk_widget_show ( ent );
}

/********* https://developer.gnome.org/gtk3/stable/GtkSpinButton.html *********/
void createTextField ( GtkWidget *parent, 
                       G3DUI     *gui,
                       char      *name,
                       gint       x, 
                       gint       y,
                       gint       textWidth,
                       gint       textHeight,
                       void     (*cbk)( GtkTextBuffer *,
                                        gpointer ) ) {
    GtkWidget   *win = gtk_scrolled_window_new ( NULL, NULL );
    GdkRectangle brec = { 0x00, 0x00, textWidth, textHeight };
    GtkWidget   *txt = gtk_text_view_new ( );
    GtkTextBuffer *buf = gtk_text_view_get_buffer(txt);


    gtk_container_add(GTK_CONTAINER(win), txt);

    gtk_widget_set_name ( win, name );
    gtk_widget_set_name ( txt, name );

    gtk_widget_set_size_request ( win, textWidth, textHeight );

    gtk_widget_size_allocate ( win, &brec );

    gtk_fixed_put ( GTK_FIXED(parent), win, x, y );

    if ( cbk ) {
        g_signal_connect ( buf, "changed", G_CALLBACK(cbk), gui );
    }

    gtk_widget_show ( win );

/*
    gtk_widget_set_name ( txt, name );

    gtk_widget_size_allocate ( txt, &brec );

    gtk_widget_set_size_request ( txt, textWidth, textHeight );

    gtk_fixed_put ( GTK_FIXED(parent), txt, x, y );

    if ( cbk ) {
        g_signal_connect ( txt, "insert-at-cursor", G_CALLBACK(cbk), gui );
    }


    gtk_widget_show ( txt ); */


}

/******************************************************************************/
/********* https://developer.gnome.org/gtk3/stable/GtkSpinButton.html *********/
GtkWidget *createNumericText ( GtkWidget *parent, G3DUI *gui,
                                                  GtkAdjustment *adj,
                                                  char *name,
                                                  gint x, gint y,
                                                  gint labwidth,
                                                  gint txtwidth,
                                                  void (*cbk)( GtkWidget *, 
                                                               gpointer ) ) {
    GtkWidget     *btn  = gtk_spin_button_new ( adj, 1.0, 0 );
    GtkStyleContext *context = gtk_widget_get_style_context ( btn );
    GdkRectangle   brec = { 0x00, 0x00, txtwidth, 0x12 };
    PangoFontDescription *fdesc;
    uint32_t charwidth;

    gtk_style_context_get ( context, GTK_STATE_FLAG_NORMAL, "font", &fdesc, NULL );

    charwidth = pango_font_description_get_size ( fdesc ) / PANGO_SCALE;

    /*printf ( "%s\n", pango_font_description_get_family ( fdesc ) );*/

    /*gtk_spin_button_set_numeric ( btn, TRUE );*/

    gtk_widget_set_name ( btn, name );

    gtk_widget_size_allocate ( btn, &brec );

    gtk_widget_set_hexpand ( btn, FALSE );

    if ( txtwidth ) {
        gtk_entry_set_width_chars ( GTK_ENTRY(btn), txtwidth / charwidth );
    }

    gtk_fixed_put ( GTK_FIXED(parent), btn, x + labwidth, y );

    if ( labwidth ) {
        GdkRectangle lrec = { 0x00, 0x00, labwidth, 0x12 };
        GtkWidget   *lab  = gtk_label_new ( name );

        gtk_widget_set_name ( lab, name );

        gtk_widget_size_allocate ( lab, &lrec );

        gtk_fixed_put ( GTK_FIXED(parent), lab, x, y );

        gtk_widget_show ( lab );
    }

    if ( cbk ) {
        g_signal_connect ( btn, "value-changed"     , G_CALLBACK(cbk)    , gui);
        g_signal_connect ( btn, "button-press-event", G_CALLBACK(grabCbk), gui);
    }

    gtk_widget_show ( btn );

    return btn;
}

/******************************************************************************/
/********* https://developer.gnome.org/gtk3/stable/GtkSpinButton.html *********/
void createIntegerText ( GtkWidget *parent, G3DUI *gui, 
                                            char *name,
                                            gint x, gint y,
                                            gint labwidth,
                                            gint txtwidth,
                                            void (*cbk)( GtkWidget *, 
                                                         gpointer ) ) {
    GtkAdjustment *adj = gtk_adjustment_new ( 0.0, 
                                             -INT_MAX, 
                                              INT_MAX,
                                              1.0,
                                              1.0,
                                              5.0 );

    GtkWidget *btn = createNumericText ( parent, gui,
                                         adj, 
                                         name,
                                         x, 
                                         y, 
                                         labwidth, 
                                         txtwidth, 
                                         cbk );}

/******************************************************************************/
/********* https://developer.gnome.org/gtk3/stable/GtkSpinButton.html *********/
void createFloatText ( GtkWidget *parent, G3DUI *gui, 
                                          char *name,
                                          gint x, gint y,
                                          gint labwidth,
                                          gint txtwidth,
                                          void (*cbk)( GtkWidget *, 
                                                       gpointer ) ) {
    GtkAdjustment *adj = gtk_adjustment_new ( 0.0, 
                                             -FLT_MAX, 
                                              FLT_MAX,
                                              0.1,
                                              1.0,
                                              5.0 );
    GtkWidget *btn = createNumericText ( parent, gui,
                                         adj, 
                                         name,
                                         x, 
                                         y, 
                                         labwidth, 
                                         txtwidth, 
                                         cbk );

    gtk_spin_button_set_digits ( GTK_SPIN_BUTTON(btn), 3 );
}

/******************************************************************************/
/********* https://developer.gnome.org/gtk3/stable/GtkSpinButton.html *********/
void createOrientationSelection ( GtkWidget *parent, G3DUI *gui, 
                                                     char *name,
                                                     gint x, gint y,
                                                     gint labwidth,
                                                     gint txtwidth,
                                                     void (*cbk)( GtkWidget *, 
                                                                  gpointer ) ) {
    GtkWidget     *cmb  = gtk_combo_box_text_new ( );
    GdkRectangle   crec = { 0x00, 0x00, txtwidth, 0x12 };

    /*gtk_spin_button_set_numeric ( btn, TRUE );*/

    gtk_widget_set_name ( cmb, name );

    gtk_widget_size_allocate ( cmb, &crec );

    /*gtk_entry_set_width_chars ( GTK_ENTRY(cmb), 18 );*/

    gtk_fixed_put ( GTK_FIXED(parent), cmb, x + labwidth, y );

    if ( labwidth ) {
        GdkRectangle lrec = { 0x00, 0x00, labwidth, 0x12 };
        GtkWidget   *lab  = gtk_label_new ( name );

        gtk_widget_set_name ( lab, name );

        gtk_widget_size_allocate ( lab, &lrec );

        gtk_fixed_put ( GTK_FIXED(parent), lab, x, y );

        gtk_widget_show ( lab );
    }

    if ( cbk ) { 
        g_signal_connect ( cmb, "changed", G_CALLBACK(cbk), gui );
    }

    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, ZXSTR );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, XYSTR );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, YZSTR );

    gtk_combo_box_set_active ( GTK_COMBO_BOX(cmb), 0x00 );

    gtk_widget_show ( cmb );
}

/******************************************************************************/
void createProceduralTypeSelection ( GtkWidget *parent, 
                                     G3DUI     *gui, 
                                     char      *name,
                                     gint       x, 
                                     gint       y,
                                     gint       labwidth,
                                     gint       txtwidth,
                                     void     (*cbk)( GtkWidget *, 
                                                      gpointer ) ) {
    GtkWidget     *cmb  = gtk_combo_box_text_new ( );
    GdkRectangle   crec = { 0x00, 0x00, txtwidth, 0x12 };

    /*gtk_spin_button_set_numeric ( btn, TRUE );*/

    gtk_widget_set_name ( cmb, name );

    gtk_widget_size_allocate ( cmb, &crec );

    /*gtk_entry_set_width_chars ( GTK_ENTRY(cmb), 18 );*/

    gtk_fixed_put ( GTK_FIXED(parent), cmb, x + labwidth, y );

    if ( labwidth ) {
        GdkRectangle lrec = { 0x00, 0x00, labwidth, 0x12 };
        GtkWidget   *lab  = gtk_label_new ( name );

        gtk_widget_set_name ( lab, name );

        gtk_widget_size_allocate ( lab, &lrec );

        gtk_fixed_put ( GTK_FIXED(parent), lab, x, y );

        gtk_widget_show ( lab );
    }

    if ( cbk ) { 
        g_signal_connect ( cmb, "changed", G_CALLBACK(cbk), gui );
    }

    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, PERLINNOISE );
    /*gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, XYSTR );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, YZSTR );*/

    gtk_combo_box_set_active ( GTK_COMBO_BOX(cmb), 0x00 );

    gtk_widget_show ( cmb );
}

/******************************************************************************/
void createProceduralResSelection ( GtkWidget *parent, 
                                           G3DUI     *gui, 
                                           char      *name,
                                           gint       x, 
                                           gint       y,
                                           gint       labwidth,
                                           gint       txtwidth,
                                           void     (*cbk)( GtkWidget *, 
                                                            gpointer ) ) {
    GtkWidget     *cmb  = gtk_combo_box_text_new ( );
    GdkRectangle   crec = { 0x00, 0x00, txtwidth, 0x12 };

    /*gtk_spin_button_set_numeric ( btn, TRUE );*/

    gtk_widget_set_name ( cmb, name );

    gtk_widget_size_allocate ( cmb, &crec );

    /*gtk_entry_set_width_chars ( GTK_ENTRY(cmb), 18 );*/

    gtk_fixed_put ( GTK_FIXED(parent), cmb, x + labwidth, y );

    if ( labwidth ) {
        GdkRectangle lrec = { 0x00, 0x00, labwidth, 0x12 };
        GtkWidget   *lab  = gtk_label_new ( name );

        gtk_widget_set_name ( lab, name );

        gtk_widget_size_allocate ( lab, &lrec );

        gtk_fixed_put ( GTK_FIXED(parent), lab, x, y );

        gtk_widget_show ( lab );
    }

    if ( cbk ) { 
        g_signal_connect ( cmb, "changed", G_CALLBACK(cbk), gui );
    }

    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, PROCRES128  );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, PROCRES256  );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, PROCRES512  );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, PROCRES1024 );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, PROCRES2048 );

    gtk_combo_box_set_active ( GTK_COMBO_BOX(cmb), 0x00 );

    gtk_widget_show ( cmb );
}

/******************************************************************************/
void g3dui_updateAllCurrentEdit ( G3DUI *gui ) {
    LIST *ltmp = gui->lcuredit;

    while ( ltmp ) {
        GtkWidget *widget = ( GtkWidget * ) ltmp->data;

        updateCurrentEdit ( widget, gui );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
void g3dui_updateCoords         ( G3DUI *gui ) {
    LIST *ltmpcoordedit = gui->lcoordedit;

    while ( ltmpcoordedit ) {
        GtkWidget *widget = ( GtkWidget * ) ltmpcoordedit->data;

        updateCoordinatesEdit ( widget, gui );

        ltmpcoordedit = ltmpcoordedit->next;
    }
}

/******************************************************************************/
void g3dui_redrawWidgetList ( G3DUI *gui, LIST *lwidget ) {
    LIST *ltmpwidget = lwidget;

    while ( ltmpwidget ) {
        GtkWidget *widget = ( GtkWidget * ) ltmpwidget->data;

        gtk_widget_queue_draw ( widget );

        ltmpwidget = ltmpwidget->next;
    }
}

/******************************************************************************/
void g3dui_redrawMaterialList ( G3DUI *gui ) {
    g3dui_redrawWidgetList ( gui, gui->lmatlist );
}

/******************************************************************************/
void g3dui_redrawAllWeightGroupList ( G3DUI *gui ) {
    g3dui_redrawWidgetList ( gui, gui->lweightgroup );
}

/******************************************************************************/
void g3dui_updateMaterialEdit ( G3DUI *gui ) {
    LIST *ltmpmatedit = gui->lmatedit;

    while ( ltmpmatedit ) {
        GtkWidget *widget = ( GtkWidget * ) ltmpmatedit->data;

        updateMaterialEdit ( widget, gui );

        ltmpmatedit = ltmpmatedit->next;
    }
}

/******************************************************************************/
void g3dui_redrawObjectList     ( G3DUI *gui ) {
    g3dui_redrawWidgetList ( gui, gui->lobjlist );
}

/******************************************************************************/
void g3dui_redrawTimeline       ( G3DUI *gui ) {
    g3dui_redrawWidgetList ( gui, gui->ltimeline );
}

/******************************************************************************/
static GtkWidget *getFocusedWidget_r ( GtkWidget *widget ) {
    if ( gtk_widget_is_focus ( widget ) ) {
        return widget;
    } else {
        if ( GTK_IS_CONTAINER ( widget ) ) {
            GList *children = gtk_container_get_children ( GTK_CONTAINER (widget) );

            while ( children ) {
                GtkWidget *child = ( GtkWidget * ) children->data;

                if ( getFocusedWidget_r ( child ) ) {
                    return child;
                }

                children = g_list_next ( children );
            }
        }
    }

    return NULL;
}

/******************************************************************************/
void g3dui_setHourGlass ( G3DUI *gui ) {
    G3DUIGTK3  *ggt  = ( G3DUIGTK3 * ) gui->toolkit_data;
    /*GtkWindow  *win  = gtk_widget_get_parent(ggt->top);
    GtkWidget  *foc  = gtk_window_get_focus ( win );
    GdkWindow  *pwin = gtk_widget_get_parent_window ( foc );
    GdkDisplay *dis  = gdk_window_get_display ( pwin );*/

    GdkDisplay       *dis = gdk_display_get_default();
    GdkDeviceManager *mgr = gdk_display_get_device_manager ( dis );
    GdkDevice        *dev = gdk_device_manager_get_client_pointer ( mgr );
    GdkCursor        *cur = gdk_cursor_new_for_display ( dis, GDK_WATCH );
    gint x, y;

    ggt->winAtPosition = gdk_device_get_window_at_position ( dev, &x, &y ); 
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
    gdk_window_set_cursor ( ggt->winAtPosition, cur );
    gdk_display_sync ( dis );
    gdk_cursor_unref ( cur );
    /** must flush **/
    gdk_flush ( );

}

/******************************************************************************/
void g3dui_unsetHourGlass       ( G3DUI *gui ) {
    G3DUIGTK3        *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;
    /*GdkDisplay       *dis = gdk_display_get_default();
    GdkDeviceManager *mgr = gdk_display_get_device_manager ( dis );
    GdkDevice        *dev = gdk_device_manager_get_client_pointer ( mgr );
    GdkCursor        *cur = gdk_cursor_new_for_display ( dis, GDK_WATCH );
    gint x, y;

    ggt->winAtPosition = gdk_device_get_window_at_position ( dev, &x, &y ); */
    /* return to normal */
    gdk_window_set_cursor ( ggt->winAtPosition, NULL );

#ifdef unused
    gdk_device_ungrab ( dev, GDK_CURRENT_TIME );
#endif
}

/******************************************************************************/
void g3dui_updateAllCurrentMouseTools ( G3DUI *gui ) {
    LIST *ltmpmtools = gui->lmtools;

    while ( ltmpmtools ) {
        GtkWidget *mtool = ( GtkWidget * ) ltmpmtools->data;

        updateCurrentMouseTool ( mtool, gui );


        ltmpmtools = ltmpmtools->next;
    }
}

/******************************************************************************/
void g3dui_updateKeyEdit        ( G3DUI *gui ) {
    LIST *ltmpkeyedit = gui->lkeyedit;

    while ( ltmpkeyedit ) {
        GtkWidget *keyedit = ( GtkWidget * ) ltmpkeyedit->data;

        updateKeyEdit ( keyedit, gui );


        ltmpkeyedit = ltmpkeyedit->next;
    }
}

/******************************************************************************/
void g3dui_updateSelectedMaterialPreview ( G3DUI *gui ) {
    LIST *ltmpmatlist = gui->lmatlist;
    G3DMATERIAL *mat = gui->selmat;

    if ( mat ) {
        while ( ltmpmatlist ) {
            GtkWidget *matlst = ( GtkWidget * ) ltmpmatlist->data;

            g3duimateriallist_updatePreview ( matlst, mat );


            ltmpmatlist = ltmpmatlist->next;
        }
    }
}

/******************************************************************************/
void g3dui_clearMaterials   ( G3DUI *gui ) {
    LIST *ltmpmatlist = gui->lmatlist;

    while ( ltmpmatlist ) {
        GtkWidget *matlst = ( GtkWidget * ) ltmpmatlist->data;

        g3duimateriallist_removeAllPreviews ( matlst );


        ltmpmatlist = ltmpmatlist->next;
    }
}

/******************************************************************************/
void g3dui_importMaterials      ( G3DUI *gui ) {
    LIST *ltmpmatlist = gui->lmatlist;

    while ( ltmpmatlist ) {
        GtkWidget *matlst = ( GtkWidget * ) ltmpmatlist->data;

        g3duimateriallist_importFromScene ( matlst, gui->sce );


        ltmpmatlist = ltmpmatlist->next;
    }
}

/******************************************************************************/
void g3dui_redrawGLViews ( G3DUI *gui ) {
    LIST *ltmp = gui->lglview;

    while ( ltmp ) {
        GtkWidget *glview = ( GtkWidget * ) ltmp->data;
        GtkWidget *area   = gtk_view_getGLArea ( glview );
        GdkRectangle arec;

        arec.x = arec.y = 0x00;
        arec.width = arec.height = 0x01;

        gdk_window_invalidate_rect ( gtk_widget_get_window ( area ), &arec, FALSE );


        ltmp = ltmp->next;
    }
}

/******************************************************************************/
G3DCAMERA *g3dui_getMainViewCamera ( G3DUI *gui ) {
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;
    GtkView   *gvw = ( GtkView * ) ggt->mainView;

    return gvw->view.cam;
}

/******************************************************************************/
G3DCAMERA *g3dui_getCurrentViewCamera ( G3DUI *gui ) {
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;
    GtkView   *gvw = ( GtkView * ) gtk_widget_get_parent ( ggt->curogl );

    return gvw->view.cam;
}

/******************************************************************************/
void g3dui_setMouseTool ( GtkWidget *widget, gpointer user_data ) {
    G3DUI        *gui = ( G3DUI        * ) user_data;
    G3DUIGTK3    *ggt = ( G3DUIGTK3    * ) gui->toolkit_data;
    const char  *name = gtk_widget_get_name ( widget );
    G3DMOUSETOOL *mou = common_g3dui_getMouseTool ( gui, name );
    G3DCAMERA *cam = g3dui_getCurrentViewCamera ( gui );

    if ( gui->lock ) return;

    if ( mou ) {
        common_g3dui_setMouseTool ( gui, cam, mou );

        if ( ( mou->flags & MOUSETOOLNOCURRENT ) == 0x00 ) {
            /*** Remember that widget ID, for example to be unset when a toggle button 
            from another parent widget is called (because XmNradioBehavior won't talk
            to other parent widget ***/
            if ( ggt->curmou && widget != ggt->curmou ) {
                gui->lock = 0x01;

                if ( GTK_IS_TOGGLE_TOOL_BUTTON ( ggt->curmou ) ) {
                    GtkToggleToolButton *ttb = GTK_TOGGLE_TOOL_BUTTON(ggt->curmou);

                    gtk_toggle_tool_button_set_active ( ttb, FALSE );
                }

                gui->lock = 0x00;
                /*XtVaSetValues ( ggt->curmou, XmNset, False, NULL );*/
            }

            ggt->curmou = widget;

           g3dui_updateAllCurrentMouseTools ( gui );
        }
    } else {
        fprintf ( stderr, "No such mousetool %s\n", name );
    }
}

/******************************************************************************/
G3DUIGTK3 *g3duigtk3_new ( GtkWidget *widget ) {
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) calloc ( 0x01, sizeof ( G3DUIGTK3 ) );

    if ( ggt == NULL ) { 
        fprintf ( stderr, "g3duimotif_new: malloc failed\n" );

        return NULL;
    }

    ggt->top  = widget;
    ggt->main = widget;

    return ggt;
}

/******************************************************************************/
void g3duigtk3_free ( G3DUIGTK3 *ggt ) {
    free ( ggt );
}

/******************************************************************************/
static void gtk3_common_g3dui_resizeWidget ( GtkWidget *widget ) {
    GtkGlossUI *glossui = ( GtkGlossUI * ) widget;
    G3DUI *gui = &glossui->gui;
    int width  = gtk_widget_get_allocated_width ( widget ),
        height = gtk_widget_get_allocated_height ( widget );

    common_g3dui_resizeWidget ( gui, width, height );


   /*printf("test %d %d\n", width, height );*/
}

/******************************************************************************/
static void gtk_glossui_adjust_size_request ( GtkWidget *widget,
                                            GtkOrientation orientation,
                                            gint *minimum_size,
                                            gint *natural_size ) {

}

/******************************************************************************/
static void gtk_glossui_class_init ( GtkGlossUIClass *glossui_class ) {

    GObjectClass   *object_class = G_OBJECT_CLASS   ( glossui_class );
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS ( glossui_class );

  /*object_class->set_property  = pfx_foo_set_property;
  object_class->get_property  = pfx_foo_get_property;*/

    widget_class->realize          = gtk_glossui_realize;
    /*widget_class->configure_event  = gtk_glossui_configure;*/
    widget_class->size_allocate    = gtk_glossui_size_allocate;
    /*widget_class->adjust_size_request   = gtkglossui_adjust_size_request;*/
    /*widget_class->show             = gtkglossui_show;*/

  /* ajout de la propri\E9t\E9 PfxFoo:active */
  /*g_object_class_install_property (object_class, PROP_ACTIVE,
                                   g_param_spec_boolean ("active",
                                                         "Active",
                                                         "Whether the display is active",
                                                         FALSE,
                                                         G_PARAM_READWRITE));*/
   
  /* ajout d'un \E9l\E9ment priv\E9 (utilis\E9 pour contenit les champs priv\E9s de
   * l'objet) */
  /*g_type_class_add_private (klass, sizeof (PfxFooPrivate));*/

    g_signal_new( "action", G_TYPE_FROM_CLASS(glossui_class), 
                               G_SIGNAL_RUN_FIRST,
                               G_STRUCT_OFFSET (GtkGlossUIClass,action),
                               NULL, NULL,
                               g_cclosure_marshal_VOID__POINTER,
                               G_TYPE_NONE, 1, G_TYPE_POINTER );
}

/******************************************************************************/
static gboolean gtk_glossui_configure ( GtkWidget *widget, GdkEvent *event ) {

}

/******************************************************************************/
void gtk_glossui_size_allocate ( GtkWidget *widget, GdkRectangle *allocation ) {
    GtkGlossUI *glossui = ( GtkGlossUI * ) widget;
    G3DUI *gui = &glossui->gui;
    GList *children = gtk_container_get_children ( GTK_CONTAINER (widget) );

    common_g3dui_resizeWidget ( gui, allocation->width, allocation->height );

    gtk_widget_set_allocation ( widget, allocation );

    if ( gtk_widget_get_realized ( widget ) ) {
        gdk_window_move_resize ( gtk_widget_get_window ( widget ),
                                 allocation->x, 
                                 allocation->y,
                                 allocation->width, 
                                 allocation->height );
    }

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *name = gtk_widget_get_name ( child );
        G3DUIRECTANGLE *rec = NULL;

        if ( strcmp ( name, MENUBARNAME     ) == 0x00 ) rec = &gui->menurec;
        if ( strcmp ( name, TOOLBARNAME     ) == 0x00 ) rec = &gui->tbarrec;
        if ( strcmp ( name, MODEBARNAME     ) == 0x00 ) rec = &gui->mbarrec;
        if ( strcmp ( name, QUADNAME        ) == 0x00 ) rec = &gui->quadrec;
        if ( strcmp ( name, TIMEBOARDNAME   ) == 0x00 ) rec = &gui->timerec;
        if ( strcmp ( name, MAINBOARDNAME   ) == 0x00 ) rec = &gui->mbrdrec;
        if ( strcmp ( name, BOTTOMBOARDNAME ) == 0x00 ) rec = &gui->proprec;

        if ( rec && rec->width && rec->height ) {
            GdkRectangle gdkrec;

            gdkrec.x      = rec->x;
            gdkrec.y      = rec->y;
            gdkrec.width  = rec->width;
            gdkrec.height = rec->height;

            if ( gtk_widget_get_has_window ( widget ) == 0x00 ) {
                gdkrec.x += allocation->x;
                gdkrec.y += allocation->y;
            }

            gtk_widget_size_allocate ( child, &gdkrec );
        }
 
        children = g_list_next ( children );
    }
}

/******************************************************************************/
static void gtk_glossui_init ( GtkGlossUI *glossui ) {
    GtkWidget *widget = ( GtkWidget * ) glossui;
    G3DUI *gui = &glossui->gui;

    /*** Expose event won't be called if we dont set has_window ***/
    gtk_widget_set_has_window ( widget, TRUE );

    g_signal_connect ( G_OBJECT(widget), 
                       "action",
                       G_CALLBACK ( g3duicom_handleAction ), gui );
}

/******************************************************************************/
static void gtk_glossui_realize ( GtkWidget *widget ) {
    GdkWindow *parent_window = gtk_widget_get_parent_window ( widget );
    GtkStyleContext  *style  = gtk_widget_get_style_context ( widget );
    GdkWindowAttr attributes;
    GtkAllocation allocation;
    GdkWindow       *window;
    GtkGlossUI *glossui = ( GtkGlossUI * ) widget;
    G3DUI *gui = &glossui->gui;
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;
    R3DRENDERSETTINGS *rsg;
    GtkCssProvider *provider = gtk_css_provider_new ();
    static const gchar *myCSS = { "* {                      \n"
    #ifdef __linux__
                                  "    font: Lucida Sans 8; \n"
    #endif
    #ifdef __MINGW32__
                                  "    font: Verdana 8; \n"
    #endif
                                  "}                        \n"
                                  "GtkEntry {               \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "GtkButton {              \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "GtkSpinButton {          \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     1px; \n"
                                  "    margin-right:   1px; \n"
                                  "    margin-left:    1px; \n"
                                  "    margin-bottom:  1px; \n"
                                  "    padding-top:    1px; \n"
                                  "    padding-right:  1px; \n"
                                  "    padding-left:   1px; \n"
                                  "    padding-bottom: 1px; \n"
                                  "}                        \n"
                                  "GtkQuad {                \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n"
                                  "GtkMenuItem#Option_Menu {\n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     1px; \n"
                                  "    margin-right:   1px; \n"
                                  "    margin-left:    1px; \n"
                                  "    margin-bottom:  1px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "    font-size:      6; \n"
                                  "}                        \n"
                                  "GtkComboBoxText#Shading_Menu {   \n"
                                  "    border-width:   0px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "    font-size:      6; \n"
                                  "}                        \n"
                                  "GtkToolbar {             \n"
                                  "    border-width:   1px; \n"
                                  "    border-radius:  0px; \n"
                                  "    margin-top:     0px; \n"
                                  "    margin-right:   0px; \n"
                                  "    margin-left:    0px; \n"
                                  "    margin-bottom:  0px; \n"
                                  "    padding-top:    0px; \n"
                                  "    padding-right:  0px; \n"
                                  "    padding-left:   0px; \n"
                                  "    padding-bottom: 0px; \n"
                                  "}                        \n" };
    GdkDisplay *display = gdk_display_get_default ( );
    GdkScreen  *screen  = gdk_display_get_default_screen ( display );
    #ifdef __linux__
    char *home = getenv ( "HOME" );
    #endif
    #ifdef __MINGW32__
    char *home = getenv ( "USERPROFILE" );
    #endif
    char configFileName[0x100];
    GtkWidget *tab;

    #ifdef __linux__
    snprintf ( configFileName, 0x100, "%s/.gloss3d/gloss3d.conf", home );
    #endif
    #ifdef __MINGW32__
    snprintf ( configFileName, 0x100, "%s\\.gloss3d\\gloss3d.conf", home );
    #endif

    gtk_style_context_add_provider_for_screen ( screen,
                                 GTK_STYLE_PROVIDER (provider),
                                 GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);

    gtk_css_provider_load_from_data ( provider, myCSS, -1, NULL );

    g_object_unref ( provider );

    common_g3dui_loadConfiguration ( gui, configFileName );

    common_g3dui_createDefaultCameras ( gui );

    /*** undo redo manager ***/
    gui->urm = g3durmanager_new ( gui->conf.undolevel );

    gui->lrps = NULL;

    gui->toolkit_data = g3duigtk3_new ( widget );

    /*** copy-paste manager ***/
    gui->cli = g3duiclipboard_new ( );

    gui->flags = ( VIEWOBJECT | XAXIS | YAXIS | ZAXIS );

    rsg = r3drendersettings_new ( ); /*** default render settings ***/

    common_g3dui_addRenderSettings ( gui, rsg );
    common_g3dui_useRenderSettings ( gui, rsg );

    /******************/

    gtk_widget_get_allocation ( widget, &allocation );

    attributes.window_type  = GDK_WINDOW_CHILD;
    attributes.visual       = gtk_widget_get_visual ( widget );
    attributes.x            = allocation.x;
    attributes.y            = allocation.y;
    attributes.width        = allocation.width;
    attributes.height       = allocation.height;
    attributes.wclass       = GDK_INPUT_OUTPUT;
    attributes.event_mask   = gtk_widget_get_events (widget);

    attributes.event_mask  |=  ( GDK_EXPOSURE_MASK     |
                                 GDK_KEY_PRESS_MASK    |
                                 GDK_KEY_RELEASE_MASK  |
                                 GDK_ENTER_NOTIFY_MASK |
                                 GDK_LEAVE_NOTIFY_MASK |
                                 GDK_LEAVE_NOTIFY_MASK |
                                 GDK_STRUCTURE_MASK    |
                                 GDK_FOCUS_CHANGE_MASK );

    window = gdk_window_new ( parent_window, &attributes, GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL );

    gtk_widget_set_window   ( widget, window );

#if GTK_CHECK_VERSION(3,8,0)
    gtk_widget_register_window ( widget, window );
#else
    gdk_window_set_user_data ( window, widget );
#endif

    gtk_widget_set_realized ( widget, 1 );

    common_g3dui_resizeWidget ( gui, allocation.width, allocation.height );

    createModeBar   ( widget, gui, MODEBARNAME  , gui->mbarrec.x,
                                                  gui->mbarrec.y,
                                                  gui->mbarrec.width,
                                                  gui->mbarrec.height );

    createToolBar   ( widget, gui, TOOLBARNAME  , gui->tbarrec.x,
                                                  gui->tbarrec.y,
                                                  gui->tbarrec.width,
                                                  gui->tbarrec.height );

    createMenuBar   ( widget, gui, MENUBARNAME  , gui->menurec.x,
                                                  gui->menurec.y,
                                                  gui->menurec.width,
                                                  gui->menurec.height );

    createQuad      ( widget, gui, QUADNAME     , gui->quadrec.x,
                                                  gui->quadrec.y,
                                                  gui->quadrec.width,
                                                  gui->quadrec.height );

    createTimeBoard ( widget, gui, TIMEBOARDNAME, gui->timerec.x,
                                                  gui->timerec.y,
                                                  gui->timerec.width,
                                                  gui->timerec.height );

    /*** Add mouse tools AFTER GLViews creation ***/
    common_g3dui_initDefaultMouseTools ( gui, g3dui_getMainViewCamera ( gui ) );

    /*** File loading must be done AFTER OpenGL init ***/
    gui->sce = g3dscene_new  ( 0x00, "Gloss3D scene" );

    /*** MainBoard uses G3DSCENE, we must created it after scene creation ***/
    createMainBoard ( widget, gui, MAINBOARDNAME, gui->mbrdrec.x,
                                                  gui->mbrdrec.y,
                                                  gui->mbrdrec.width,
                                                  gui->mbrdrec.height );



    /*gui->sce = g3dscene_import3ds ( "../SKULL.3DS", gui->flags );*/

    /*gui->sce = g3dscene_open ( "../models/T-Rex_60_Animated_Textured.g3d", gui->flags );*/




    /*gtk_style_context_set_background ( gtk_widget_get_style_context ( widget ),
                                       window );*/

}

/******************************************************************************/
GtkWidget *gtk_glossui_new ( ) {
  return g_object_new ( gtk_glossui_get_type ( ),
                        /*"active", NULL,*/
                        NULL);
}

/******************************************************************************/
void g3dui_init ( G3DUI *gui, G3DSCENE *sce ) {
    gui->sce    = sce;
    gui->flags |= VIEWOBJECT;
}
