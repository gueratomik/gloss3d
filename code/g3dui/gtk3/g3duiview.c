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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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

G_DEFINE_TYPE (GtkView, gtk_view, GTK_TYPE_FIXED)

/******************************************************************************/
static void     gtk_view_class_init    ( GtkViewClass * );
static void     gtk_view_init          ( GtkView * );
static void     gtk_view_realize       ( GtkWidget * );
static void     gtk_view_size_request  ( GtkWidget *, GtkRequisition * );
static gboolean gtk_view_expose        ( GtkWidget *, cairo_t * );
static gboolean gtk_view_configure     ( GtkWidget *, GdkEvent *, gpointer );
static void     gtk_view_size_allocate ( GtkWidget *, GtkAllocation * );
static void     gtk_view_event         ( GtkWidget *, GdkEvent *, gpointer );

/******************************************************************************/
static void PostMenu     ( GtkWidget *, GdkEvent *, gpointer );
static void cancelRender ( GtkWidget *, G3DUI * );

/******************************************************************************/
void addGLMenuButton ( G3DUI *gui, GtkWidget *menu, G3DMOUSETOOL *mou ) {
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;
    GtkWidget *btn;

    btn = gtk_menu_item_new_with_label ( mou->name );

    gtk_widget_set_name ( btn, mou->name );

    g_signal_connect( btn, "activate", (GCallback) g3dui_setMouseTool, gui );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( menu ), btn );

    gtk_widget_show ( btn );
}

/******************************************************************************/
void addMenuListButton ( G3DUI *gui, LIST *lmenu, G3DMOUSETOOL *mou ) {
    LIST *ltmpmenu = lmenu;

    while ( ltmpmenu ) {
        GtkWidget *menu = ( GtkWidget * ) ltmpmenu->data;

        addGLMenuButton ( gui, menu, mou );

        ltmpmenu = ltmpmenu->next;
    }
}

/******************************************************************************/
GtkWidget *createViewMenu ( GtkWidget *widget, G3DUI *gui, char *menuname ) {
    GtkView *gvw = ( GtkView * ) widget;
    GtkWidget *menu;

    menu = gtk_menu_new ( );

    gtk_widget_set_name ( menu, menuname );

    gtk_widget_show ( menu );


    return menu;
}

/******************************************************************************/
void g3duiview_useDefaultCameraCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI     *gui = ( G3DUI     * ) user_data;
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;
    GtkView   *gvw = ( GtkView * ) gtk_widget_get_parent ( ggt->curogl );

    gvw->view.cam = gvw->view.defcam;

    gtk_widget_queue_draw ( ggt->curogl );
}

/******************************************************************************/
void g3duiview_useSelectedCameraCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI     *gui = ( G3DUI     * ) user_data;
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;
    GtkView   *gvw = ( GtkView * ) gtk_widget_get_parent ( ggt->curogl );
    G3DSCENE  *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj && obj->type == G3DCAMERATYPE ) {
        G3DCAMERA *cam = ( G3DCAMERA * ) obj;

        cam->ratio = gvw->view.cam->ratio;

        gvw->view.cam = cam;
    }

    gtk_widget_queue_draw ( ggt->curogl );
}

/******************************************************************************/
void g3duiview_toggleNormalsCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI     *gui = ( G3DUI     * ) user_data;
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;
    GtkView   *gvw = ( GtkView * ) gtk_widget_get_parent ( ggt->curogl );

    if ( gvw->view.flags & VIEWNORMALS ) {
        gvw->view.flags &= (~VIEWNORMALS);
    } else {
        gvw->view.flags |= VIEWNORMALS;
    }

    gtk_widget_queue_draw ( ggt->curogl );
}

/******************************************************************************/
void g3duiview_toggleBonesCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI     *gui = ( G3DUI     * ) user_data;
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;
    GtkView   *gvw = ( GtkView * ) gtk_widget_get_parent ( ggt->curogl );

    if ( gvw->view.flags & HIDEBONES ) {
        gvw->view.flags &= (~HIDEBONES);
    } else {
        gvw->view.flags |= HIDEBONES;
    }

    gtk_widget_queue_draw ( ggt->curogl );
}

/******************************************************************************/
void g3duiview_toggleLightingCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI     *gui = ( G3DUI     * ) user_data;
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;
    GtkView   *gvw = ( GtkView * ) gtk_widget_get_parent ( ggt->curogl );

    if ( gvw->view.flags & NOLIGHTING ) {
        gvw->view.flags &= (~NOLIGHTING);

        g3dscene_turnLightsOn  ( gui->sce );
    } else {
        gvw->view.flags |= NOLIGHTING;

        g3dscene_turnLightsOff ( gui->sce );
    }

    gtk_widget_queue_draw ( ggt->curogl );
}

/******************************************************************************/
void g3duiview_toggleGridCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI     *gui = ( G3DUI     * ) user_data;
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;
    GtkView   *gvw = ( GtkView * ) gtk_widget_get_parent ( ggt->curogl );

    if ( gvw->view.flags & HIDEGRID ) {
        gvw->view.flags &= (~HIDEGRID);
    } else {
        gvw->view.flags |= HIDEGRID;
    }

    gtk_widget_queue_draw ( ggt->curogl );
}


/******************************************************************************/
void g3duiview_resetCameraCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI     *gui = ( G3DUI     * ) user_data;
    G3DUIGTK3 *ggt = ( G3DUIGTK3 * ) gui->toolkit_data;
    GtkView   *gvw = ( GtkView * ) gtk_widget_get_parent ( ggt->curogl );
    G3DOBJECT *objcam = ( G3DOBJECT * ) gvw->view.cam;

    memcpy ( &objcam->pos, &gvw->view.defcampos, sizeof ( G3DVECTOR ) );
    memcpy ( &objcam->rot, &gvw->view.defcamrot, sizeof ( G3DVECTOR ) );
    memcpy ( &objcam->sca, &gvw->view.defcamsca, sizeof ( G3DVECTOR ) );

    gvw->view.cam->focal = gvw->view.defcamfoc;


    g3dobject_updateMatrix_r ( objcam, gui->flags );
}

/******************************************************************************/
GtkWidget *createOptionMenu  ( GtkWidget *parent, G3DUI *gui,
                                                  char *name,
                                                  gint x,
                                                  gint y,
                                                  gint width,
                                                  gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *bar = gtk_menu_bar_new ( );
    GtkWidget *menu = gtk_menu_new ( );
    GtkWidget *item = gtk_menu_item_new_with_mnemonic ( "_Options" );

    gtk_widget_set_name ( bar, name );

    gtk_widget_set_name ( item, name );

    gtk_widget_size_allocate ( bar, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), bar, x, y );

    gtk_widget_set_halign ( item, GTK_ALIGN_CENTER );

    gtk_widget_set_size_request ( item, width, BUTTONSIZE );

    gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( item ), menu );

    gtk_menu_shell_append ( GTK_MENU_SHELL ( bar ), item );

    g3dui_addMenuButton ( menu, gui,  "Use default camera"  , width, G_CALLBACK(g3duiview_useDefaultCameraCbk)  );
    g3dui_addMenuButton ( menu, gui,  "Use selected camera" , width, G_CALLBACK(g3duiview_useSelectedCameraCbk) );
    g3dui_addMenuButton ( menu, gui,  "Show / Hide normals" , width, G_CALLBACK(g3duiview_toggleNormalsCbk)     );
    g3dui_addMenuButton ( menu, gui,  "Show / Hide bones"   , width, G_CALLBACK(g3duiview_toggleBonesCbk)       );
    g3dui_addMenuButton ( menu, gui,  "Show / Hide grid"    , width, G_CALLBACK(g3duiview_toggleGridCbk)        );
    g3dui_addMenuButton ( menu, gui,  "Turn on/off lighting", width, G_CALLBACK(g3duiview_toggleLightingCbk)    );
    g3dui_addMenuButton ( menu, gui,  "Reset view"          , width, G_CALLBACK(g3duiview_resetCameraCbk)       );

    gtk_widget_show ( item );
    gtk_widget_show ( menu );
    gtk_widget_show ( bar );


    return bar;
}

/******************************************************************************/
static void shadingCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkComboBoxText *cmbt = GTK_COMBO_BOX_TEXT(widget);
    G3DUI *gui = ( G3DUI * ) user_data;
    const char *str = gtk_combo_box_text_get_active_text ( cmbt );
    GtkView   *gvw = ( GtkView * ) gtk_widget_get_parent ( widget );
    G3DOBJECT *objcam = ( G3DOBJECT * ) gvw->view.cam;

    if ( strcmp ( str, SHADINGMENU_GOURAUD ) == 0x00 ) {
        glEnable      ( GL_LIGHTING );
        glShadeModel  ( GL_SMOOTH );
        glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );

        /*** The no texture flags is VIEW dependent, not GUI dependent ***/
        gvw->view.flags &= (~NOTEXTURE);
        gvw->view.mode   = GLVIEWGOURAUD;
    }

    if ( strcmp ( str, SHADINGMENU_FLAT ) == 0x00 ) {
        glEnable      ( GL_LIGHTING );
        glShadeModel  ( GL_FLAT );
        glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );

        /*** The no texture flags is VIEW dependent, not GUI dependent ***/
        gvw->view.flags &= (~NOTEXTURE);
        gvw->view.mode   = GLVIEWFLAT;
    }

    if ( strcmp ( str, SHADINGMENU_WIREFRAME ) == 0x00 ) {
        glDisable     ( GL_LIGHTING );
        glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );

        /*g3dui_disableTextureImages ( gui );*/

        /*** The no texture flags is VIEW dependent, not GUI dependent ***/
        gvw->view.flags |= NOTEXTURE;
        gvw->view.mode   = GLVIEWWIREFRAME;
    }
}

/******************************************************************************/
static void updateShadingSelection ( GtkWidget *widget, G3DUI *gui ) {
    GtkComboBox *cmb = GTK_COMBO_BOX(widget);
    GtkView   *gvw = ( GtkView * ) gtk_widget_get_parent ( widget );

    if ( gvw->view.mode == GLVIEWGOURAUD   ) gtk_combo_box_set_active ( cmb, 0x00 );
    if ( gvw->view.mode == GLVIEWFLAT      ) gtk_combo_box_set_active ( cmb, 0x01 );
    if ( gvw->view.mode == GLVIEWWIREFRAME ) gtk_combo_box_set_active ( cmb, 0x02 );
}

/******************************************************************************/
static void shadingRealize ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    updateShadingSelection ( widget, gui );
}

/******************************************************************************/
/********* https://developer.gnome.org/gtk3/stable/GtkSpinButton.html *********/
static void createShadingSelection  ( GtkWidget *parent, G3DUI *gui,
                                                         char *name,
                                                         gint x,
                                                         gint y,
                                                         gint width,
                                                         gint height ) {
    GtkWidget     *cmb  = gtk_combo_box_text_new ( );
    GdkRectangle   crec = { 0x00, 0x00, width, height };
    PangoFontDescription *fdesc;

    gtk_widget_set_name ( cmb, name );

    gtk_widget_size_allocate ( cmb, &crec );

    gtk_fixed_put ( GTK_FIXED(parent), cmb, x, y );

    g_signal_connect ( cmb, "changed", G_CALLBACK(shadingCbk), gui );

    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, SHADINGMENU_GOURAUD   );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, SHADINGMENU_FLAT      );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, SHADINGMENU_WIREFRAME );

    gtk_combo_box_set_active ( GTK_COMBO_BOX(cmb), 0x00 );

    g_signal_connect ( G_OBJECT (cmb), "realize", G_CALLBACK (shadingRealize ), gui );





    gtk_widget_show ( cmb );
}

/******************************************************************************/
static void createObjectMenu ( GtkWidget *widget, G3DUI *gui ) {
    GtkView   *gvw  = ( GtkView * ) gtk_widget_get_parent ( widget );
    GtkWidget *menu = createViewMenu ( widget, gui, OBJECTMENUNAME );
    G3DUIVIEW *view = &gvw->view;

    list_insert ( &gui->lobjmenu, menu );

    list_insert ( &view->lmenu  , menu );

/*    g_signal_connect ( G_OBJECT (widget), "button_press_event"  , G_CALLBACK (PostMenu), menu );
    g_signal_connect ( G_OBJECT (widget), "button_release_event", G_CALLBACK (PostMenu), menu );
*/}

/******************************************************************************/
static void createVertexMenu ( GtkWidget *widget, G3DUI *gui ) {
    GtkView   *gvw  = ( GtkView * ) gtk_widget_get_parent ( widget );
    GtkWidget *menu = createViewMenu ( widget, gui, VERTEXMENUNAME );
    G3DUIVIEW *view = &gvw->view;

    list_insert ( &gui->lvermenu, menu );
    list_insert ( &view->lmenu  , menu );
}

/******************************************************************************/
static void createEdgeMenu ( GtkWidget *widget, G3DUI *gui ) {
    GtkView   *gvw  = ( GtkView * ) gtk_widget_get_parent ( widget );
    GtkWidget *menu = createViewMenu ( widget, gui, EDGEMENUNAME );
    G3DUIVIEW *view = &gvw->view;

    list_insert ( &gui->ledgmenu, menu );
    list_insert ( &view->lmenu  , menu );
}


/******************************************************************************/
static void createFaceMenu ( GtkWidget *widget, G3DUI *gui ) {
    GtkView   *gvw  = ( GtkView * ) gtk_widget_get_parent ( widget );
    GtkWidget *menu = createViewMenu ( widget, gui, FACEMENUNAME );
    G3DUIVIEW *view = &gvw->view;

    list_insert ( &gui->lfacmenu, menu );
    list_insert ( &view->lmenu  , menu );
}

/******************************************************************************/
static void PostMenu ( GtkWidget *widget, GdkEvent *event,
                                          gpointer user_data ) {
    GtkView        *gvw      = ( GtkView   * ) gtk_widget_get_parent ( widget );
    G3DUI          *gui      = ( G3DUI * ) user_data;
    GdkEventButton *bev      = ( GdkEventButton * ) event;
    G3DUIVIEW      *view     = &gvw->view;
    GtkWidget      *curmenu  = NULL;
    LIST           *ltmpmenu = view->lmenu;

    while ( ltmpmenu ) {
        GtkWidget *menu = ( GtkWidget * ) ltmpmenu->data;

        if ( gui->flags & VIEWOBJECT ) {
            if ( strcmp ( gtk_widget_get_name ( menu ), OBJECTMENUNAME ) == 0x00 ) curmenu = menu;
        }

        if ( gui->flags & VIEWVERTEX ) {
            if ( strcmp ( gtk_widget_get_name ( menu ), VERTEXMENUNAME ) == 0x00 ) curmenu = menu;
        }

        if ( gui->flags & VIEWEDGE ) {
            if ( strcmp ( gtk_widget_get_name ( menu ), EDGEMENUNAME   ) == 0x00 ) curmenu = menu;
        }

        if ( gui->flags & VIEWFACE ) {
            if ( strcmp ( gtk_widget_get_name ( menu ), FACEMENUNAME   ) == 0x00 ) curmenu = menu;
        }

        ltmpmenu = ltmpmenu->next;
    }

    if ( curmenu ) {
        gtk_menu_popup ( GTK_MENU ( curmenu ), NULL, NULL, NULL, NULL,
                                    bev->button,
                                    gdk_event_get_time( ( GdkEvent * ) event ) );
    }
}

/******************************************************************************/
GtkWidget *gtk_view_getGLArea ( GtkWidget *widget ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;

        if ( GTK_IS_DRAWING_AREA(child) ) {

            return child;
        }

        children = g_list_next ( children );
    }


    return NULL;
}

/******************************************************************************/
static void gtk_view_class_init ( GtkViewClass *view_class ) {

    GObjectClass   *object_class = G_OBJECT_CLASS   ( view_class );
    GtkWidgetClass *widget_class = GTK_WIDGET_CLASS ( view_class );
    GdkPixbuf     **icon         = view_class->icon;

    icon[MAXIMIZEBUTTON]  = gdk_pixbuf_new_from_xpm_data ( maximize_view_xpm  );
    icon[ROTATEBUTTON]    = gdk_pixbuf_new_from_xpm_data ( rotate_view_xpm    );
    icon[TRANSLATEBUTTON] = gdk_pixbuf_new_from_xpm_data ( translate_view_xpm );
    icon[ZOOMBUTTON]      = gdk_pixbuf_new_from_xpm_data ( zoom_view_xpm      );

  /*object_class->set_property  = pfx_foo_set_property;
  object_class->get_property  = pfx_foo_get_property;*/

    widget_class->realize          = gtk_view_realize;
    /*widget_class->configure_event  = gtk_view_configure;*/
    widget_class->size_allocate    = gtk_view_size_allocate;
    /*widget_class->adjust_size_request   = gtk_view_adjust_size_request;*/
    widget_class->draw             = gtk_view_expose;
/*** this is now set in gtk_view_new () so I can set GUI as user_data ***/
    /*widget_class->event            = gtk_view_event;*/

  /* ajout de la propriété PfxFoo:active */
  /*g_object_class_install_property (object_class, PROP_ACTIVE,
                                   g_param_spec_boolean ("active",
                                                         "Active",
                                                         "Whether the display is active",
                                                         FALSE,
                                                         G_PARAM_READWRITE));*/
   
  /* ajout d'un élément privé (utilisé pour contenit les champs privés de
   * l'objet) */
  /*g_type_class_add_private (klass, sizeof (PfxFooPrivate));*/
}

/******************************************************************************/
static void gtk_view_move_pointer ( GtkWidget *widget, GdkEvent *event,
                                                       uint32_t x,
                                                       uint32_t y ) {
    GtkWidget *top = gtk_widget_get_toplevel ( widget );
    gint winx, winy;

    /*** gdk_device_warp() puts the pointer into screen coordinates. ***/
    /*** We have to translate the widgets coordinates to screen ones ***/
    gdk_window_get_origin ( gtk_widget_get_window ( widget ), &winx, &winy );

    gdk_device_warp ( gdk_event_get_device  ( event  ), 
                      gtk_widget_get_screen ( widget ), winx + x, winy + y );

    /*printf ( "reseting mouse to %d %d\n", winx + x, winy + y );*/
}

/******************************************************************************/
static void gtk_view_grab_pointer ( GtkWidget *widget, GdkEvent *event ) {
    gdk_device_grab ( gdk_event_get_device  ( event  ),
                      gtk_widget_get_window ( widget ),
                      GDK_OWNERSHIP_WINDOW,
                      FALSE, 
                      GDK_POINTER_MOTION_MASK |
                      GDK_BUTTON_PRESS_MASK   | 
                      GDK_BUTTON_RELEASE_MASK,
                      NULL,
                      GDK_CURRENT_TIME );
}

/******************************************************************************/
static void gtk_view_ungrab_pointer ( GtkWidget *widget, GdkEvent *event ) {
    gdk_device_ungrab ( gdk_event_get_device  ( event  ), GDK_CURRENT_TIME );
}

/******************************************************************************/
static void gtk_view_redraw_area ( GtkWidget *widget ) {
    GtkWidget *area = gtk_view_getGLArea ( widget );
    GdkRectangle arec;

    /*** Tell cairo to shut the f*** up ***/
    arec.x = arec.y = 0;
    arec.width = arec.height = 1;

    gdk_window_invalidate_rect ( gtk_widget_get_window ( area ), &arec, FALSE );
}

/******************************************************************************/
static void gtk_view_redraw_menu ( GtkWidget *widget ) {
    uint32_t width = gtk_widget_get_allocated_width  ( widget );

    cairo_rectangle_int_t crec = { 0, 0, width, BUTTONSIZE };
    cairo_region_t *creg = cairo_region_create_rectangle ( &crec );

    /*** if we call gtk_widget_queue_draw, GL area is going to ***/
    /*** flicker because GDK erases the whole background and it ***/
    /*** seems there is no way to disable this behavior. Thus, ***/
    /*** we just erase the menubar region ***/
    gdk_window_invalidate_region ( gtk_widget_get_window ( widget ),
                                   creg, 
                                   FALSE );

    cairo_region_destroy ( creg );
}

/******************************************************************************/
static void gtk_view_event ( GtkWidget *widget, GdkEvent *event,
                                                gpointer user_data ) {
    static int xmid, ymid, xori, yori;
    GtkView *gvw = ( GtkView * ) widget;
    G3DUIVIEW *view = &gvw->view;
    G3DCAMERA *cam = view->cam;
    static int xold, yold, grabbing;
    uint32_t width, height;
    static G3DPIVOT *piv;
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt  = ( G3DUIGTK3 * ) gui->toolkit_data;
    G3DSCENE *sce = gui->sce;
    static GtkWidget *area;

    width  = gtk_widget_get_allocated_width  ( widget );
    height = gtk_widget_get_allocated_height ( widget );

    switch ( event->type ) {
        case GDK_BUTTON_PRESS : {
            GdkEventButton *bev = ( GdkEventButton * ) event;
            area = gtk_view_getGLArea ( widget );

            /*** cancel renderprocess if any ***/
            /*g3dui_cancelRenderByID ( gui, ( uint64_t ) area );*/

        G3DUIRENDERPROCESS *rps = common_g3dui_getRenderProcessByID ( gui, ( uint64_t ) area );
        /*** If there was a running render, cancel it and dont go further ***/
        if ( rps ) {
            r3dscene_cancelRender ( rps->rsce );
        }

            view->buttonID = common_g3duiview_getCurrentButton ( view, bev->x, 
                                                                       bev->y );

            xori = xold = bev->x;
            yori = yold = bev->y;

            /*** we exlude the maximize button (ID=0) from grabbing ***/
            if ( ( view->buttonID > 0 ) && ( grabbing == 0x00 ) ) { 
                gdk_window_set_cursor ( gtk_widget_get_window ( ggt->top ), 
                                        gdk_cursor_new ( GDK_BLANK_CURSOR ) );

                switch ( view->buttonID ) {
                    case ROTATEBUTTON : {
                        /*** pivot for rotation ***/
                        piv = g3dpivot_new ( cam, &cam->pivot );

                        /*g3dobject_addChild ( sce, piv );*/
                    } break;
                }

                gtk_view_grab_pointer ( widget, event );

                grabbing = 0x01;
            }

            gtk_view_redraw_menu ( widget );
        } break;

        case GDK_BUTTON_RELEASE : {
            GdkEventButton *bev = ( GdkEventButton * ) event;

            if ( view->buttonID > -1 ) {
                switch ( view->buttonID ) {
                    case MAXIMIZEBUTTON : {
                        GtkWidget *parent = gtk_widget_get_parent ( widget );

                        if ( GTK_IS_QUAD(parent) ) {
                            GtkQuad *gqw = ( GtkQuad * ) parent;
                            GtkAllocation qalloc;

                            gtk_widget_get_allocation ( parent, &qalloc );

                            if ( gqw->maximized_view == NULL ) {
                                gqw->maximized_view = widget;
                            } else {
                                gqw->maximized_view = NULL;
                            }

                            gtk_quad_size_allocate ( parent, &qalloc );
                            gtk_widget_queue_draw  ( parent );
                        }
                    } break;

                    default:
                        if ( grabbing ) {
                            gtk_view_ungrab_pointer ( widget, event );

                            gdk_window_set_cursor ( gtk_widget_get_window ( ggt->top ), 
                                                NULL );

                            grabbing = 0x00;
                        }
                    break;
                }
            }

            /*** Reset selected button flag ***/
            view->buttonID = -1;

            gtk_view_redraw_menu ( widget );

            if ( piv ) {
                /*g3dobject_removeChild ( sce, piv );*/

                g3dobject_free ( ( G3DOBJECT * ) piv );

                piv = NULL;
            }
        } break;

        case GDK_MOTION_NOTIFY : {
            GdkEventMotion *mev = ( GdkEventMotion * ) event;

            /*** If pressed ***/
            if ( view->buttonID > -1 && grabbing ) {

                switch ( view->buttonID ) {
                    case ROTATEBUTTON : {
                        if ( mev->state & GDK_BUTTON1_MASK ) {
                            common_g3duiview_orbit ( view, piv,
                                                           mev->x,
                                                           mev->y, 
                                                           xold, 
                                                           yold );
/*printf("%f %f %f\n", ( float ) mev->x - xold, ( float ) mev->x, ( float ) xold );
*/                        }

                        if ( mev->state & GDK_BUTTON3_MASK ) {
                            common_g3duiview_spin ( view, mev->x, xold );
                        }
                    } break;

                    case ZOOMBUTTON : {
                        if ( mev->state & GDK_BUTTON1_MASK ) {
                            common_g3duiview_zoom ( view, mev->x, xold );
                        }

                        if ( mev->state & GDK_BUTTON3_MASK ) {
                            common_g3duiview_moveForward ( view, mev->x, xold );
                        }
                    } break;

                    case TRANSLATEBUTTON : {
                        if ( mev->state & GDK_BUTTON1_MASK ) {
                            common_g3duiview_moveSideward ( view, mev->x,
                                                           mev->y, 
                                                           xold, 
                                                           yold );
                        }

                        if ( mev->state & GDK_BUTTON3_MASK ) {
                            common_g3duiview_moveForward ( view, mev->x, xold );
                        }
                    } break;

                    default:
                    break;
                }

                gtk_view_redraw_area ( widget );

                if ( ( xori != mev->x ) || ( yori != mev->y ) ) {
                    /*** this must be called before the gtk_events_pending()***/
                    /*** thing because it's also dispatched by the event loop**/
                    /*** and we absolutely want the event to be generated ***/
                    /*** right after the redrawing of the gl area ***/
                    gtk_view_move_pointer ( widget, event, xori, yori );
                }

                /*** while (gtk_events_pending ()) gtk_main_iteration ();***/
while (gtk_events_pending ()) gtk_main_iteration ( );
            }

            xold = xori;
            yold = yori;
        } break;

        default:
        break;
    }
}

/******************************************************************************/
static void gtk_view_size_allocate ( GtkWidget     *widget,
                                     GtkAllocation *allocation ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    GtkView *gvw = ( GtkView * ) widget;

    common_g3duiview_resize ( &gvw->view, allocation->width, 
                                   allocation->height );

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
        const char *child_name = gtk_widget_get_name ( child );
        GdkRectangle gdkrec = { 0x00, 0x00, 0x00, 0x00 };

        if ( gtk_widget_get_has_window ( widget ) == 0x00 ) {
            gdkrec.x += allocation->x;
            gdkrec.y += allocation->y;
        }

        if ( strcmp ( child_name, OPTIONMENUNAME ) == 0x00 ) {
            gdkrec.x      += 0;
            gdkrec.y      += 0;
            gdkrec.width   = 96;
            gdkrec.height  = BUTTONSIZE;

            gtk_widget_size_allocate ( child, &gdkrec );
        }

        if ( strcmp ( child_name, SHADINGMENUNAME ) == 0x00 ) {
            gdkrec.x      += 96;
            gdkrec.y      += 0;
            gdkrec.width   = 112;
            gdkrec.height  = BUTTONSIZE;

            gtk_widget_size_allocate ( child, &gdkrec );
        }

        if ( GTK_IS_DRAWING_AREA(child) ) {
            gdkrec.x      += 0;
            gdkrec.y      += BUTTONSIZE;
            gdkrec.width   = allocation->width;
            gdkrec.height  = allocation->height - BUTTONSIZE;

            gtk_widget_size_allocate ( child, &gdkrec );
        }

        children = g_list_next ( children );
    }
}

/******************************************************************************/
static gboolean gtk_area_filter ( GdkXEvent *xevent,
                  GdkEvent *event,
                  gpointer data ) {
    #ifdef __MINGW32__
    MSG *msg = ( MSG * ) xevent;
    /*** use parent class configure function ***/
    /*GTK_WIDGET_CLASS(gtk_view_parent_class)->configure_event ( widget, event );*/
printf ( "test %d\n", msg->message );
    if ( msg->message == 70 ) return GDK_FILTER_REMOVE;

    #endif

    return GDK_FILTER_CONTINUE;
}

/******************************************************************************/
static gboolean gtk_view_expose ( GtkWidget *widget, cairo_t *cr ) {
    GtkViewClass *view_class = ( GtkViewClass * ) G_OBJECT_GET_CLASS ( widget);
    GtkView *gvw = ( GtkView * ) widget;
    G3DUIVIEW *view = &gvw->view;

    if ( gtk_widget_is_drawable ( widget ) ) {
        GtkStyleContext *context = gtk_widget_get_style_context ( widget );
        uint32_t width  = gtk_widget_get_allocated_width  ( widget ),
                 height = gtk_widget_get_allocated_height ( widget );
        GtkAllocation alloc;
        uint32_t i;

        gtk_render_background ( context, cr, 0x00, 0x00, width, height );

        for ( i = 0x00; i < NBVIEWBUTTON; i++ ) {
            GdkPixbuf *icon = view_class->icon[i];

            if ( i == view->buttonID ) {
                cairo_set_source_rgb ( cr, 0.25f, 0.25f, 0.25f );

                cairo_rectangle ( cr, view->rec[i].x, 
                                      view->rec[i].y, 
                                      view->rec[i].width, 
                                      view->rec[i].height );
                cairo_fill ( cr );
            }

            gdk_cairo_set_source_pixbuf ( cr, icon, view->rec[i].x, 
                                                    view->rec[i].y );
            cairo_paint ( cr );
        }
    }
 
    return GTK_WIDGET_CLASS (gtk_view_parent_class)->draw (widget, cr);
}

/******************************************************************************/
static void gtk_view_init ( GtkView *gvw ) {
    GtkWidget *widget = ( GtkWidget * ) gvw;
    G3DUIVIEW *view   = &gvw->view;

    /*** Expose event won't be called if we dont set has_window ***/
    gtk_widget_set_has_window ( widget, TRUE );

    common_g3duiview_init ( view, gtk_widget_get_allocated_width  ( widget ), 
                                  gtk_widget_get_allocated_height ( widget ) );
}

/******************************************************************************/
static void gtk_view_realize ( GtkWidget *widget ) {
    GdkWindow *parent_window = gtk_widget_get_parent_window ( widget );
    GtkStyleContext  *style  = gtk_widget_get_style_context ( widget );
    GtkView *gvw = ( GtkView * ) widget;
    G3DUIVIEW *view = &gvw->view;
    GdkWindowAttr attributes;
    GtkAllocation allocation;
    GdkWindow *window;

    gtk_widget_set_realized ( widget, TRUE );

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

    attributes.event_mask  |=  ( GDK_EXPOSURE_MASK            |
                                 GDK_KEY_PRESS_MASK           |
			                     GDK_KEY_RELEASE_MASK         |
                                 GDK_BUTTON_PRESS_MASK        |
                                 GDK_BUTTON_RELEASE_MASK      |
                                 GDK_POINTER_MOTION_MASK      |
                                 GDK_POINTER_MOTION_HINT_MASK |
			                     GDK_STRUCTURE_MASK );

    window = gdk_window_new ( parent_window, &attributes, GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL );

    gtk_widget_set_window   ( widget, window );

#if GTK_CHECK_VERSION(3,8,0)
    gtk_widget_register_window ( widget, window );
#else
    gdk_window_set_user_data ( window, widget );
#endif
}

/******************************************************************************/
GtkWidget *gtk_view_new ( G3DCAMERA *cam ) {
    GObject *gob = g_object_new ( gtk_view_get_type ( ), NULL );
    GtkView *gvw = ( GtkView * ) gob;
    G3DUIVIEW *view = &gvw->view;
    G3DOBJECT *objcam = ( G3DOBJECT * ) cam;

    /*** I don't have time to implement GTK+3 property ***/
    /*** thing. Plus, it's , freaking complicated .... ***/
    view->cam = view->defcam = cam;

    memcpy ( &view->defcampos, &objcam->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &view->defcamrot, &objcam->rot, sizeof ( G3DVECTOR ) );
    memcpy ( &view->defcamsca, &objcam->sca, sizeof ( G3DVECTOR ) );

              view->defcamfoc = cam->focal;


    return ( GtkWidget * ) gob;
}

/******************************************************************************/
GtkWidget *createView ( GtkWidget *parent, G3DUI *gui,
                                           char *name,
                                           G3DCAMERA *cam,
                                           gint width,
                                           gint height ) {
    GdkRectangle gdkrec = { 0x00, 0x00, width, height };
    GtkWidget *gvw = gtk_view_new ( cam );
    GtkWidget *area/* = gtk_view_getGLArea ( gvw )*/;

    gtk_widget_set_name ( gvw, name );

    gtk_widget_size_allocate ( gvw, &gdkrec );

    /*** the OpenGL Window ***/
    area = gtk_drawing_area_new ( );

    /*** For keyboard inputs ***/
    gtk_widget_set_can_focus ( area, TRUE );

    /*** Drawing area does not receive mous events by defaults ***/
    gtk_widget_set_events ( area, gtk_widget_get_events ( area ) |
                                  GDK_KEY_PRESS_MASK             |
			          GDK_KEY_RELEASE_MASK           |
                                  GDK_BUTTON_PRESS_MASK          |
                                  GDK_BUTTON_RELEASE_MASK        |
                                  GDK_POINTER_MOTION_MASK        |
                                  GDK_POINTER_MOTION_HINT_MASK );
/*
    gdk_window_add_filter ( gtk_widget_get_window ( area ), gdkevent_to_g3devent, view->xevent );
*/

    gtk_fixed_put ( GTK_FIXED(gvw), area, 0x00, BUTTONSIZE );

    g_signal_connect ( G_OBJECT (gvw), "motion_notify_event" , G_CALLBACK (gtk_view_event), gui );
    g_signal_connect ( G_OBJECT (gvw), "button_press_event"  , G_CALLBACK (gtk_view_event), gui );
    g_signal_connect ( G_OBJECT (gvw), "button_release_event", G_CALLBACK (gtk_view_event), gui );
    /*g_signal_connect ( G_OBJECT (gvw), "configure-event", G_CALLBACK (gtk_view_configure), gui );*/

    /*gdk_window_add_filter ( gtk_widget_get_window ( area ), gtk_area_filter, gui );*/

    createObjectMenu ( area, gui );
    createVertexMenu ( area, gui );
    createEdgeMenu   ( area, gui );
    createFaceMenu   ( area, gui );

    g_signal_connect ( G_OBJECT (area), "size-allocate"       , G_CALLBACK (gtk3_sizeGL ), gui );
    g_signal_connect ( G_OBJECT (area), "realize"             , G_CALLBACK (gtk3_initGL ), gui );
    g_signal_connect ( G_OBJECT (area), "draw"                , G_CALLBACK (gtk3_showGL ), gui );

    g_signal_connect ( G_OBJECT (area), "motion_notify_event" , G_CALLBACK (gtk3_inputGL), gui );
    g_signal_connect ( G_OBJECT (area), "key_press_event"     , G_CALLBACK (gtk3_inputGL), gui );
    g_signal_connect ( G_OBJECT (area), "key_release_event"   , G_CALLBACK (gtk3_inputGL), gui );
    g_signal_connect ( G_OBJECT (area), "button_press_event"  , G_CALLBACK (gtk3_inputGL), gui );
    g_signal_connect ( G_OBJECT (area), "button_release_event", G_CALLBACK (gtk3_inputGL), gui );

    /*** Add the widget to the list of opengl views, allowing us to ***/
    /*** refresh all of them when an action was done.               ***/
    list_insert ( &gui->lglview, gvw );

    gtk_widget_show ( area );

    createOptionMenu       ( gvw, gui, OPTIONMENUNAME , 96, 96,  60, BUTTONSIZE );
    createShadingSelection ( gvw, gui, SHADINGMENUNAME, 96,  0, 112, BUTTONSIZE );

    gtk_widget_show ( gvw );


    return gvw;
}

/******************************************************************************/
static void gdkevent_to_g3devent ( GdkEvent *gdkev, G3DEvent *g3dev ) {
    memset ( g3dev, 0x00, sizeof ( G3DEvent ) );

    switch ( gdkev->type ) {
        case GDK_BUTTON_PRESS : {
            GdkEventButton *gdkbev = ( GdkEventButton * ) gdkev;
            G3DButtonEvent *g3dbev = ( G3DButtonEvent * ) g3dev;

            g3dbev->type  = G3DButtonPress;
            g3dbev->x     = gdkbev->x;
            g3dbev->y     = gdkbev->y;

            g3dbev->button = gdkbev->button;

            if ( gdkbev->state & GDK_CONTROL_MASK ) g3dbev->state |= G3DControlMask;
            if ( gdkbev->state & GDK_SHIFT_MASK   ) g3dbev->state |= G3DShiftMask;
            /*if ( gdkbev->state & GDK_BUTTON1_MASK ) g3dbev->state |= G3DButton1Mask;
            if ( gdkbev->state & GDK_BUTTON2_MASK ) g3dbev->state |= G3DButton2Mask;
            if ( gdkbev->state & GDK_BUTTON3_MASK ) g3dbev->state |= G3DButton3Mask;*/
        } break;

        case GDK_BUTTON_RELEASE : {
            GdkEventButton *gdkbev = ( GdkEventButton * ) gdkev;
            G3DButtonEvent *g3dbev = ( G3DButtonEvent * ) g3dev;

            g3dbev->type = G3DButtonRelease;
            g3dbev->x    = gdkbev->x;
            g3dbev->y    = gdkbev->y;

            g3dbev->button = gdkbev->button;

            if ( gdkbev->state & GDK_CONTROL_MASK ) g3dbev->state |= G3DControlMask;
            if ( gdkbev->state & GDK_SHIFT_MASK   ) g3dbev->state |= G3DShiftMask;
            /*if ( gdkbev->state & GDK_BUTTON1_MASK ) g3dbev->state |= G3DButton1Mask;
            if ( gdkbev->state & GDK_BUTTON2_MASK ) g3dbev->state |= G3DButton2Mask;
            if ( gdkbev->state & GDK_BUTTON3_MASK ) g3dbev->state |= G3DButton3Mask;*/
        } break;

        case GDK_MOTION_NOTIFY : {
            GdkEventMotion *gdkmev = ( GdkEventMotion * ) gdkev;
            G3DMotionEvent *g3dmev = ( G3DMotionEvent * ) g3dev;

            g3dmev->type = G3DMotionNotify;
            g3dmev->x    = gdkmev->x;
            g3dmev->y    = gdkmev->y;

            if ( gdkmev->state & GDK_CONTROL_MASK ) g3dmev->state |= G3DControlMask;
            if ( gdkmev->state & GDK_SHIFT_MASK   ) g3dmev->state |= G3DShiftMask;
            if ( gdkmev->state & GDK_BUTTON1_MASK ) g3dmev->state |= G3DButton1Mask;
            if ( gdkmev->state & GDK_BUTTON2_MASK ) g3dmev->state |= G3DButton2Mask;
            if ( gdkmev->state & GDK_BUTTON3_MASK ) g3dmev->state |= G3DButton3Mask;
        } break;
    }
}

/******************************************************************************/
void gtk3_inputGL ( GtkWidget *widget, GdkEvent *gdkev, 
                                       gpointer user_data ) {
    G3DUI           *gui = ( G3DUI * ) user_data;
    GdkEventButton *bev  = ( GdkEventButton * ) gdkev;
    GtkView        *gvw  = ( GtkView * ) gtk_widget_get_parent ( widget );
    G3DUIVIEW      *view = &gvw->view;
    G3DUIGTK3      *ggt  = ( G3DUIGTK3 * ) gui->toolkit_data;

    ggt->curogl = widget;

    if ( ( gdkev->type == GDK_BUTTON_PRESS ) &&
         ( bev->button == 3 ) ) {
        PostMenu ( widget, gdkev, user_data );

        return;
    }

    gui->curcam = view->cam;

    switch ( gdkev->type ) {
        case GDK_KEY_PRESS : {
            GdkEventKey *kev = ( GdkEventKey * ) gdkev;

            switch ( kev->keyval ) {
                case GDK_KEY_Delete: {
                    common_g3dui_deleteSelectionCbk ( gui );
                } break;

                case GDK_KEY_c: {
                    common_g3dui_copySelectionCbk ( gui );
                } break;

                case GDK_KEY_v: {
                    common_g3dui_pasteSelectionCbk ( gui );
                } break;
            }
        } break;

        case GDK_BUTTON_PRESS : {
            G3DUIRENDERPROCESS *rps = common_g3dui_getRenderProcessByID ( gui, ( uint64_t ) widget );
            /*** If there was a running render, cancel it and dont go further ***/
            if ( rps ) {
                r3dscene_cancelRender ( rps->rsce );

                return;
            }

            /*** For keyboard inputs ***/
            gtk_widget_grab_focus ( widget );
        } break;

        default :
        break;
    }

    if ( gui->mou ) {
        /*** Call the mousetool callback and redraw if return value is > 0 ***/
        G3DEvent g3dev;

        gdkevent_to_g3devent ( gdkev, &g3dev );

        if ( gui->mou->tool ) {
            uint32_t msk = gui->mou->tool ( gui->mou, 
                                            gui->sce,
                                            view->cam, 
                                            gui->urm,
                                            gui->flags, &g3dev );

            if ( msk & REDRAWVIEW ) {
                g3dui_redrawGLViews ( gui );
            }

            if ( msk & REDRAWLIST ) {
                g3dui_redrawObjectList ( gui );
            }

            if ( msk & REDRAWCURRENTOBJECT ) {
                g3dui_updateAllCurrentEdit ( gui );
            }

            if ( msk & REDRAWTIMELINE ) {
                g3dui_redrawTimeline ( gui );
            }

            if ( msk & REDRAWCOORDS ) {
                g3dui_updateCoords ( gui );
            }

            if ( msk & REDRAWCOORDS ) {
                g3dui_updateCoords ( gui );
            }

            if ( msk & NOBUFFEREDSUBDIVISION ) {
                /*** this should be replace by some MEANINGFUL mask ***/
                gui->flags |= ONGOINGANIMATION;
            }

            if ( msk & BUFFEREDSUBDIVISIONOK ) {
                /*** this should be replace by some MEANINGFUL mask ***/
                gui->flags &= (~ONGOINGANIMATION);
            }

            if ( gdkev->type == GDK_BUTTON_RELEASE ) {
                g3dcursor_reset ( &gui->sce->csr );
            }
        }
    }
}

/******************************************************************************/
void gtk3_sizeGL ( GtkWidget *widget, GdkRectangle *allocation, 
                                      gpointer user_data ) {
    GtkView    *gvw    = ( GtkView * ) gtk_widget_get_parent ( widget );
    G3DUIVIEW  *view   = &gvw->view;
    GdkDisplay *gdkdpy = gtk_widget_get_display       ( widget );
    GdkWindow  *gdkwin = gtk_widget_get_window        ( widget );
    G3DUI      *gui    = ( G3DUI * ) user_data;
#ifdef __linux__
    Display    *dpy    = gdk_x11_display_get_xdisplay ( gdkdpy );
    Window      win    = gdk_x11_window_get_xid       ( gdkwin );

    glXMakeCurrent ( dpy, win, view->glctx );
#endif

#ifdef __MINGW32__
    HWND hWnd = GDK_WINDOW_HWND ( gdkwin );
    HDC dc = GetDC ( hWnd );

    wglMakeCurrent ( dc, view->glctx );

    ReleaseDC ( hWnd, dc );
#endif

    /*** cancel renderprocess if any ***/
    common_g3dui_cancelRenderByID ( gui, ( uint64_t ) widget );

    common_g3duiview_sizeGL ( view, allocation->width, allocation->height );

    gtk_widget_queue_draw ( widget );
}

/******************************************************************************/
void gtk3_initGL ( GtkWidget *widget, gpointer user_data ) {
    GtkView         *gvw = ( GtkView * ) gtk_widget_get_parent ( widget );
    G3DUIVIEW      *view = &gvw->view;
    GdkWindow  *p_window = gtk_widget_get_parent_window ( widget );
    GdkDisplay *gdkdpy   = gtk_widget_get_display ( widget );
    GdkWindow  *gdkwin   = gtk_widget_get_window  ( widget );

#ifdef __linux__
    Display    *dis      = gdk_x11_display_get_xdisplay ( gdkdpy );
    Window      root     = DefaultRootWindow ( dis );
    GLint       glattr[] = { GLX_RGBA,
                             GLX_RED_SIZE,    8,
                             GLX_BLUE_SIZE,   8,
                             GLX_GREEN_SIZE,  8,
                             GLX_ALPHA_SIZE,  0,
                             GLX_DEPTH_SIZE, 24,
                             GLX_DOUBLEBUFFER,
                             None };
    static GLXContext shared = NULL;
    G3DUI *gui = ( G3DUI * ) user_data;
    GtkAllocation allocation;
    GdkWindowAttr attr;
    GdkScreen *gdkscr;
    XVisualInfo *vi;
    Window win;

    vi = glXChooseVisual ( dis, 0, glattr );

    if ( vi == 0x00 ) {
        fprintf ( stderr, "Could not create GL Visual\n" );

        return;
    }

/**********************/
    gdkscr   = gtk_widget_get_screen  ( widget );

    attr.title       = "OpenGL Window";
    attr.event_mask  = gtk_widget_get_events ( widget ) | GDK_EXPOSURE_MASK       |
                                                          GDK_STRUCTURE_MASK      |
                                                          GDK_BUTTON_PRESS_MASK   | 
                                                          GDK_BUTTON_RELEASE_MASK | 
                                                          GDK_POINTER_MOTION_MASK |
                                                          GDK_POINTER_MOTION_HINT_MASK;

    gtk_widget_get_allocation ( widget, &allocation );

    attr.x           = allocation.x;
    attr.y           = allocation.y;
    attr.width       = allocation.width;
    attr.height      = allocation.height;
    attr.wclass      = GDK_INPUT_OUTPUT;
    attr.window_type = GDK_WINDOW_CHILD;
    attr.visual      = gdk_x11_screen_lookup_visual ( gdkscr, vi->visualid );

    gdkwin = gdk_window_new ( p_window, &attr, GDK_WA_X | GDK_WA_Y | GDK_WA_VISUAL );

    gtk_widget_set_window   ( widget, gdkwin );

    gdk_window_set_user_data ( gdkwin, widget );
/*************************/

    win = gdk_x11_window_get_xid ( gdkwin );

    /*** Create OpenGL Context ***/
    view->glctx = glXCreateContext( dis, vi, shared, True );

    if ( shared == NULL ) shared = view->glctx;

    /*** Set Context as the current context ***/
    glXMakeCurrent ( dis, win, view->glctx );

    common_g3duiview_initGL ( view );

    /*** Matrix update func must be called after OpenGL initialization ***/
    if ( view->cam ) {
        /*** Realize is done after gtk_view_new(), that's why view->cam exists ***/
        g3dobject_updateMatrix_r ( ( G3DOBJECT * ) view->cam, 0x00 );
    }

    /*** Free the memory assigned for GLwNvisualInfo ***/
    free ( vi );
#endif

#ifdef __MINGW32__
    PIXELFORMATDESCRIPTOR pfd;
    HWND hWnd = GDK_WINDOW_HWND ( gdkwin );
    HDC dc = GetDC ( hWnd );
    int pxf;

    memset ( &pfd, 0x00, sizeof ( PIXELFORMATDESCRIPTOR ) );

    pfd.nSize        = sizeof ( PIXELFORMATDESCRIPTOR );
    pfd.nVersion     = 0x01;
    pfd.dwFlags      = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
    pfd.iPixelType   = PFD_TYPE_RGBA;
    pfd.cDepthBits   = 0x18;
    pfd.cColorBits   = 0x20;

    if ( ( pxf = ChoosePixelFormat ( dc, &pfd ) ) == 0x00 ) {
        fprintf ( stderr, "ChoosePixelFormat failed\n" );

        return 0x00;
    }

    if ( SetPixelFormat ( dc, pxf, &pfd ) == 0x00 ) {
        fprintf ( stderr, "SetPixelFormat failed\n" );

        return 0x00;
    }

    view->glctx = wglCreateContext ( dc );

    wglMakeCurrent ( dc,  view->glctx );

    ext_glActiveTextureARB   = (PFNGLACTIVETEXTUREARBPROC)  wglGetProcAddress("glActiveTextureARB");
    ext_glMultiTexCoord2fARB = (PFNGLMULTITEXCOORD2FARBPROC) wglGetProcAddress("glMultiTexCoord2fARB");
    ext_glClientActiveTextureARB = (PFNGLCLIENTACTIVETEXTUREARBPROC) wglGetProcAddress("glClientActiveTextureARB");
    /*ext_glGenerateMipmap = (PFNGLGENERATEMIPMAPPROC)  wglGetProcAddress("glGenerateMipmap");*/

    printf ( " %d %d %d\n", ext_glActiveTextureARB, ext_glMultiTexCoord2fARB, ext_glClientActiveTextureARB  );

    common_g3duiview_initGL ( view );

    ReleaseDC ( hWnd, dc );
#endif
}

/******************************************************************************/
void gtk3_showGL ( GtkWidget *widget, cairo_t *cr, gpointer user_data ) {
    GtkView      *gvw    = ( GtkView * ) gtk_widget_get_parent ( widget );
    G3DUIVIEW    *view   = &gvw->view;
    GdkDisplay   *gdkdpy = gtk_widget_get_display       ( widget );
    GdkWindow    *gdkwin = gtk_widget_get_window        ( widget );
    G3DCAMERA    *cam    = view->cam;
    G3DUI        *gui    = ( G3DUI * ) user_data;
    G3DUIGTK3    *ggt    = ( G3DUIGTK3 * ) gui->toolkit_data;;
    G3DMOUSETOOL *mou    = gui->mou;
    G3DSCENE     *sce    = gui->sce;
    uint32_t current;

#ifdef __linux__
    Display      *dpy    = gdk_x11_display_get_xdisplay ( gdkdpy );
    Window        win    = gdk_x11_window_get_xid       ( gdkwin );
#endif

#ifdef __MINGW32__
    HWND hWnd = GDK_WINDOW_HWND ( gdkwin );
    HDC dc = GetDC ( hWnd );
#endif

    if ( gui->playLock ) {
         /*** Force disabling real time subdivision ***/
        gui->flags |= ONGOINGANIMATION;
    }

    /*** This helps the drawarea to determine if it should draw mouse tools ***/
    /*** for example (we don't draw mousetool on all window widget. ***/
    current = ( widget == ggt->curogl ) ? 0x01 : 0x00;

#ifdef __linux__
    /*** Set Context as the current context ***/
    glXMakeCurrent ( dpy, win, view->glctx );

    /*** GUI Toolkit Independent part ****/
    /*************************************/
    common_g3duiview_showGL ( gui, sce, cam, mou, current, gui->flags | view->flags );
    /*************************************/

    glXSwapBuffers ( dpy, win );

    XSync ( dpy, False );
#endif

#ifdef __MINGW32__
    /*** Set Context as the current context ***/
    wglMakeCurrent ( dc, view->glctx );

    /*** GUI Toolkit Independent part ****/
    /*************************************/
    common_g3duiview_showGL ( gui, sce, cam, mou, current, gui->flags | view->flags );
    /*************************************/

    SwapBuffers ( dc );

    ReleaseDC ( hWnd, dc );
#endif

    if ( gui->playLock ) {
        /*** Re-enable real time subdivision ***/
        gui->flags &= (~ONGOINGANIMATION);
    }


}


