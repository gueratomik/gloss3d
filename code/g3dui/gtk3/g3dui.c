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
GTK3G3DUI *gtk3_getUI ( ) {
    static GTK3G3DUI gtk3gui;

    return &gtk3gui;
}

/******************************************************************************/
void g3duirectangle_toGdkRectangle ( G3DUIRECTANGLE *in, 
                                     GdkRectangle   *out  ) {
    out->x      = in->x;
    out->y      = in->y;
    out->width  = in->width;
    out->height = in->height;
}

/******************************************************************************/
GtkCheckButton *ui_gtk_check_button_new ( char *class ) {
    GtkWidget *btn = gtk_check_button_new ( );
    GtkStyleContext *context = gtk_widget_get_style_context ( btn );

    gtk_style_context_add_class ( context, class );

    return GTK_CHECK_BUTTON(btn);
}

/******************************************************************************/
GtkSpinButton *ui_gtk_spin_button_new ( char          *class,
                                        GtkAdjustment *adjustment,
                                        gdouble        climb_rate,
                                        guint          digits ) {
    GtkWidget *spin = gtk_spin_button_new ( adjustment, climb_rate, digits );
    GtkStyleContext *context = gtk_widget_get_style_context ( spin );

    gtk_style_context_add_class ( context, class );

    return GTK_SPIN_BUTTON(spin);
}

/******************************************************************************/
GtkFixed *ui_gtk_fixed_new ( char *class ) {
    GtkWidget *fixed = gtk_fixed_new ( );
    GtkStyleContext *context = gtk_widget_get_style_context ( fixed );

    gtk_style_context_add_class ( context, class );

    return GTK_FIXED(fixed);
}

/******************************************************************************/
GtkLabel *ui_gtk_label_new ( char *class,
                             char *name ) {
    GtkWidget *lab = gtk_label_new ( name );
    GtkStyleContext *context = gtk_widget_get_style_context ( lab );

    gtk_style_context_add_class ( context, class );


    return GTK_LABEL(lab);
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
    GtkSpinButton *btn  = ui_gtk_spin_button_new ( class, adj, 1.0, 0 );
    GtkStyleContext *context = gtk_widget_get_style_context ( GTK_WIDGET(btn) );
    GdkRectangle   brec = { 0x00, 0x00, txtwidth, txtheight };
    PangoFontDescription *fdesc;
    uint32_t charwidth;

    gtk_style_context_get ( context, GTK_STATE_FLAG_NORMAL, "font", &fdesc, NULL );

    charwidth = pango_font_description_get_size ( fdesc ) / PANGO_SCALE;

    /*printf ( "%s\n", pango_font_description_get_family ( fdesc ) );*/

    /*gtk_spin_button_set_numeric ( btn, TRUE );*/

    gtk_widget_set_name ( GTK_WIDGET(btn), name );

    gtk_widget_set_size_request ( GTK_WIDGET(btn), brec.width, brec.height );

    gtk_widget_set_hexpand ( GTK_WIDGET(btn), FALSE );

    if ( txtwidth ) {
        gtk_entry_set_width_chars ( GTK_ENTRY(btn), txtwidth / charwidth );
    }

    gtk_fixed_put ( GTK_FIXED(parent), GTK_WIDGET(btn), x + labwidth, y );

    if ( labwidth ) {
        GdkRectangle lrec = { 0x00, 0x00, labwidth, txtheight };
        GtkLabel   *lab  = ui_gtk_label_new ( class, name );

        gtk_widget_set_name ( GTK_WIDGET(lab), name );

        gtk_widget_set_size_request ( GTK_WIDGET(lab), lrec.width, lrec.height );

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
void gtk3_setMouseTool ( GtkWidget *widget, 
                         gpointer   user_data ) {
    GTK3G3DUI *gtk3gui = gtk3_getUI ( );
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    const char  *name = gtk_widget_get_name ( widget );
    G3DMOUSETOOL *mou = g3dui_getMouseTool ( gui, name );
    G3DCAMERA *cam = gui->currentView->cam;

    if ( gui->lock ) return;

    if ( mou ) {
        g3dui_setMouseTool ( gui, cam, mou );

        if ( ( mou->flags & MOUSETOOLNOCURRENT ) == 0x00 ) {
            /*** Remember that widget ID, for example to be unset when a toggle button 
            from another parent widget is called (because XmNradioBehavior won't talk
            to other parent widget ***/
            if ( gtk3gui->curmou && widget != gtk3gui->curmou ) {
                gui->lock = 0x01;

                if ( GTK_IS_TOGGLE_TOOL_BUTTON ( gtk3gui->curmou ) ) {
                    GtkToggleToolButton *ttb = GTK_TOGGLE_TOOL_BUTTON(gtk3gui->curmou);

                    gtk_toggle_tool_button_set_active ( ttb, FALSE );
                }

                gui->lock = 0x00;
                /*XtVaSetValues ( ggt->curmou, XmNset, False, NULL );*/
            }

            gtk3gui->curmou = widget;

           gtk3_updateAllCurrentMouseTools ( gui );
        }
    } else {
        fprintf ( stderr, "No such mousetool %s\n", name );
    }
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
static void gtk3_dispatchGLMenuButton ( GTK3G3DUI    *gtk3gui,
                                        G3DMOUSETOOL *mou, 
                                        uint32_t      tool_flags ) {
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    uint32_t vertexModeSplineFlags  = ( VERTEXMODETOOL | SPLINETOOL  ),
             vertexModeMeshFlags    = ( VERTEXMODETOOL | MESHTOOL    ),
             edgeModeMeshFlags      = ( EDGEMODETOOL   | MESHTOOL    ),
             faceModeMeshFlags      = ( FACEMODETOOL   | MESHTOOL    ),
             sculptModeMeshFlags    = ( SCULPTMODETOOL | MESHTOOL    ),
             vertexModeMorpherFlags = ( VERTEXMODETOOL | MORPHERTOOL );
#ifdef TODO
    if ( tool_flags & ( OBJECTMODETOOL ) ) {
        gtk3_addMenuListButton ( gui, 
                                 gtk3gui->lObjectModeMenu,
                                 mou );
    }

    if ( ( tool_flags & vertexModeMeshFlags ) == vertexModeMeshFlags ) {
        gtk3_addMenuListButton ( gui, 
                                 gtk3gui->lVertexModeMeshMenu,
                                 mou );
    }

    if ( ( tool_flags & edgeModeMeshFlags ) == edgeModeMeshFlags ) {
        gtk3_addMenuListButton ( gui,
                                 gtk3gui->lEdgeModeMeshMenu,
                                 mou );
    }

    if ( ( tool_flags & faceModeMeshFlags ) == faceModeMeshFlags ) {
        gtk3_addMenuListButton ( gui,
                                 gtk3gui->lFaceModeMeshMenu,
                                 mou );
    }

    if ( ( tool_flags & sculptModeMeshFlags ) == sculptModeMeshFlags ) {
        gtk3_addMenuListButton ( gui,
                                 gtk3gui->lSculptModeMeshMenu,
                                 mou );
    }

    if ( ( tool_flags & vertexModeSplineFlags ) == vertexModeSplineFlags ) {
        gtk3_addMenuListButton ( gui,
                                 gtk3gui->lVertexModeSplineMenu,
                                 mou );
    }

    if ( ( tool_flags & vertexModeMorpherFlags ) == vertexModeMorpherFlags ) {
        gtk3_addMenuListButton ( gui,
                                 gtk3gui->lVertexModeMorpherMenu,
                                 mou );
    }
#endif
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

    if ( msk & REDRAWTIMELINE ) {
        gtk3_redrawTimeline ( );
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
void gtk3_initDefaultMouseTools ( GTK3G3DUI *gtk3gui ) {
    G3DUI *gui = ( G3DUI * ) gtk3gui;
    G3DMOUSETOOL *mou;

    /********************************/

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

    g3dui_addMouseTool   ( gui, mou = g3dmousetoolremovevertexpose_new ( ) );
    gtk3_dispatchGLMenuButton ( gtk3gui, mou, VERTEXMODETOOL |
                                     MORPHERTOOL     |
                                     GLMENUTOOL );

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
}
