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
static G3DUITIMELINE *g3duitimeline_new ( G3DUI *gui ) {
    G3DUITIMELINE *tim = calloc ( 0x01, sizeof ( G3DUITIMELINE ) );

    if ( tim == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );
    }

    tim->grp.gui = gui;
    tim->tdata = common_timelinedata_new ( );

    return tim; 
}

/******************************************************************************/
static void g3duitimeline_hide_pointer ( GtkWidget *widget ) {
    GtkWidget *top = gtk_widget_get_toplevel ( widget );
    GdkWindow *gdktopwin =  gtk_widget_get_window ( top );

    gdk_window_set_cursor ( gdktopwin, gdk_cursor_new ( GDK_BLANK_CURSOR ) );
}

/******************************************************************************/
static void g3duitimeline_show_pointer ( GtkWidget *widget ) {
    GtkWidget *top = gtk_widget_get_toplevel ( widget );
    GdkWindow *gdktopwin =  gtk_widget_get_window ( top );

    gdk_window_set_cursor ( gdktopwin, NULL );
}

/******************************************************************************/
static void g3duitimeline_grab_pointer ( GtkWidget *widget, GdkEvent *event ) {
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
static void g3duitimeline_ungrab_pointer ( GtkWidget *widget, GdkEvent *event ) {
    gdk_device_ungrab ( gdk_event_get_device  ( event  ), GDK_CURRENT_TIME );
}

/******************************************************************************/
static void g3duitimeline_move_pointer ( GtkWidget *widget, 
                                         GdkEvent  *event,
                                         uint32_t   x,
                                         uint32_t   y ) {
    GtkWidget *top = gtk_widget_get_toplevel ( widget );
    gint winx, winy;

    /*** gdk_device_warp() puts the pointer into screen coordinates. ***/
    /*** We have to translate the widgets coordinates to screen ones ***/
    gdk_window_get_origin ( gtk_widget_get_window ( widget ), &winx, &winy );

    gdk_device_warp ( gdk_event_get_device  ( event  ), 
                      gtk_widget_get_screen ( widget ), winx + x, winy + y );
}

/******************************************************************************/
static void drawObjectName ( GtkStyleContext *context, 
                             cairo_t         *cr,
                             G3DOBJECT       *obj, 
                             float            x, 
                             float            y ) {
    cairo_text_extents_t te;
    GdkRGBA fg;

    cairo_text_extents ( cr, obj->name, &te );

    gtk_style_context_get_color ( context, GTK_STATE_FLAG_NORMAL, &fg );

    if ( obj->flags & OBJECTSELECTED ) {
        cairo_set_source_rgba ( cr, 1.0f  , 0.0f    , 0.0f   , fg.alpha );
    } else {
        cairo_set_source_rgba ( cr, fg.red, fg.green, fg.blue, fg.alpha );
    }

    y = y - ( te.height / 2 + te.y_bearing ) + LISTINDENT * 0.5f;

    cairo_move_to   ( cr, x, y );
    cairo_show_text ( cr, obj->name );
}

/******************************************************************************/
static void drawKey ( GtkStyleContext *context, 
                      cairo_t         *cr,
                      uint32_t         x, 
                      uint32_t         y,
                      uint32_t         height,
                      uint32_t         selected ) {


    /*** outline ***/
    cairo_set_source_rgb ( cr, 0.0f, 0.0f, 0.0f );
    cairo_rectangle      ( cr, x - 0x02, y, 0x05, height );
    cairo_fill           ( cr );

    /*** inline ***/
    if ( selected ) cairo_set_source_rgb ( cr, 1.0f, 0.0f, 0.0f );
    else            cairo_set_source_rgb ( cr, 1.0f, 1.0f, 1.0f );
    cairo_rectangle      ( cr, x - 0x01, y + 0x01, 0x03, height - 0x02 );
    cairo_fill           ( cr );
}

/******************************************************************************/
static void drawObject ( GtkStyleContext *context, 
                         cairo_t         *cr,
                         TIMELINEDATA    *tdata,
                         G3DOBJECT       *obj,
                         uint32_t         width,
                         uint32_t         height,
                         float           (*funcKey)(G3DKEY *key,
                                                    void   *data ),
                         void            *funcData ) {
    LIST *ltmp = obj->lkey;

    while ( ltmp ) {
        G3DKEY *key = ( G3DKEY * ) ltmp->data;
        float frame = ( ( key->flags & KEYSELECTED ) && funcKey ) ? funcKey ( key, funcData ) : key->frame;
        uint32_t kx = common_timelinedata_getFramePos ( tdata,
                                                        frame,
                                                        width ),
                 ky = 0x00;

        /*** Draw frame only if it's within the widget window ***/
        if ( kx >= 0x00 && kx <= width ) {
            drawKey ( context, cr, kx, ky, height, key->flags & KEYSELECTED );
        }

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
static void drawObjectList ( GtkStyleContext *context, 
                             cairo_t         *cr,
                             TIMELINEDATA    *tdata,
                             LIST            *lobj,
                             uint32_t         width,
                             uint32_t         height,
                             float           (*funcKey)(G3DKEY *key,
                                                        void   *data ),
                             void            *funcData ) {
    LIST *ltmp = lobj;

    while ( ltmp ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmp->data;

        drawObject ( context, cr, tdata, obj, width, height, funcKey, funcData );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
static void drawCursor ( GtkStyleContext *context, 
                         cairo_t         *cr,
                         uint32_t         x, 
                         uint32_t         y, 
                         uint32_t         height ) {
    gtk_render_arrow ( context, cr, G_PI, x - 0x03, y, 0x7 );

    gtk_render_line  ( context, cr, x, y, x, height );
}

/******************************************************************************/
typedef struct _SCALEKEYDATA {
    float reference;
    float factor;
} SCALEKEYDATA;

static float scaleKeyFunc ( G3DKEY       *key, 
                            SCALEKEYDATA *skd ) {
    return ( int ) key->frame + ( ( key->frame - skd->reference ) * skd->factor );
}

/******************************************************************************/
static int  scaleKeysToolInput ( GtkWidget *widget,
                                 GdkEvent  *gdkev, 
                                 gpointer   user_data ) {
    G3DUITIMELINE *tim = ( G3DUITIMELINE * ) user_data;
    G3DUI *gui = tim->grp.gui;
    TIMELINEDATA *tdata = tim->tdata;
    static SCALEKEYDATA skd;
    static int x, xold;
    static int32_t pressed_frame;
    uint32_t width, height;

    width  = gtk_widget_get_allocated_width  ( widget );
    height = gtk_widget_get_allocated_height ( widget );

    switch ( gdkev->type ) {
        case GDK_BUTTON_PRESS : {
            GdkEventButton *bev = ( GdkEventButton * ) gdkev;

            pressed_frame = common_timelinedata_getFrame ( tdata, bev->x,
                                                                  bev->y,
                                                                  width );

            if ( ( gdkev->type == GDK_BUTTON_PRESS ) &&
                 ( bev->button == 0x01 ) ) {
                xold = bev->x;

                skd.reference = pressed_frame;

                tdata->funcKey  =  scaleKeyFunc;
                tdata->funcData = &skd;
            } else {
                gtk_menu_popup ( GTK_MENU ( tim->menu ), 
                                 NULL, 
                                 NULL,
                                 /*SetMenuPosition*/NULL,
                                 /*bev*/NULL,
                                 bev->button,
                                 gdk_event_get_time( ( GdkEvent * ) gdkev ) );
            }

            gtk_widget_queue_draw ( widget );
        } break;

        case GDK_MOTION_NOTIFY : {
            GdkEventMotion *mev = ( GdkEventMotion * ) gdkev;

            if ( mev->state & GDK_BUTTON1_MASK ) {
                skd.factor    = ( float ) ( mev->x - xold ) * 0.1f;
            }

            gtk_widget_queue_draw ( widget );
        } break;

        case GDK_BUTTON_RELEASE : {
            GdkEventButton *bev = ( GdkEventButton * ) gdkev;

            if ( bev->button == 0x01 ) {
                LIST *ltmpobj = gui->sce->lsel;

                 g3durm_objectList_scaleSelectedKeys ( gui->urm,
                                                       gui->sce->lsel,
                                                       skd.factor,
                                                       skd.reference,
                                                       gui->engine_flags,
                                                       REDRAWTIMELINE | 
                                                       REDRAWVIEW );
            }

            tdata->funcKey  = NULL;
            tdata->funcData = NULL;

            gtk_widget_queue_draw ( widget );
            g3dui_redrawGLViews ( gui );
        } break;

        default:
        break;
    }

    return 0x00;
}

/******************************************************************************/
typedef struct _DRIFTKEYDATA {
    int deltaframe;
} DRIFTKEYDATA;

static float driftKeyFunc ( G3DKEY       *key, 
                            DRIFTKEYDATA *dkd ) {
    return ( float ) key->frame + dkd->deltaframe;
}

/******************************************************************************/
static int driftKeysToolInput ( GtkWidget *widget,
                                GdkEvent  *gdkev, 
                                gpointer   user_data ) {
    G3DUITIMELINE *tim = ( G3DUITIMELINE * ) user_data;
    G3DUI *gui = tim->grp.gui;
    TIMELINEDATA *tdata = tim->tdata;
    static DRIFTKEYDATA dkd;
    static int x, xold;

    switch ( gdkev->type ) {
        case GDK_BUTTON_PRESS : {
            GdkEventButton *bev = ( GdkEventButton * ) gdkev;

            if ( ( gdkev->type == GDK_BUTTON_PRESS ) &&
                 ( bev->button == 0x01 ) ) {
                xold = bev->x;


                dkd.deltaframe = 0;

                tdata->funcKey  =  driftKeyFunc;
                tdata->funcData = &dkd;
            } else {
                gtk_menu_popup ( GTK_MENU ( tim->menu ), 
                                 NULL, 
                                 NULL,
                                 /*SetMenuPosition*/NULL,
                                 /*bev*/NULL,
                                 bev->button,
                                 gdk_event_get_time( ( GdkEvent * ) gdkev ) );
            }

            gtk_widget_queue_draw ( widget );
        } break;

        case GDK_MOTION_NOTIFY : {
            GdkEventMotion *mev = ( GdkEventMotion * ) gdkev;

            if ( mev->state & GDK_BUTTON1_MASK ) {
                dkd.deltaframe = ( ( mev->x - xold ) / tdata->nbpix );
            }

            gtk_widget_queue_draw ( widget );
        } break;

        case GDK_BUTTON_RELEASE : {
            GdkEventButton *bev = ( GdkEventButton * ) gdkev;

            if ( bev->button == 0x01 ) {
                LIST *ltmpobj = gui->sce->lsel;

                 g3durm_objectList_driftSelectedKeys ( gui->urm,
                                                       gui->sce->lsel,
                                                       dkd.deltaframe,
                                                       gui->engine_flags,
                                                       REDRAWTIMELINE | 
                                                       REDRAWVIEW );
            }

            tdata->funcKey  = NULL;
            tdata->funcData = NULL;

            gtk_widget_queue_draw ( widget );
            g3dui_redrawGLViews ( gui );
        } break;

        default:
        break;
    }

    return 0x00;
}

/******************************************************************************/
static gboolean panToolInput ( GtkWidget *widget,
                               GdkEvent  *gdkev, 
                               gpointer   user_data ) {
    G3DUITIMELINE *tim = ( G3DUITIMELINE * ) user_data;
    G3DUI *gui = tim->grp.gui;
    TIMELINEDATA *tdata = tim->tdata;
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    static  int32_t xacc;
    static  int32_t xori, yori, xold, yold, xmid, ymid, pressed_frame;
    static uint32_t oncursor, onkey, dragging; 
    uint32_t width, height;

    width  = gtk_widget_get_allocated_width  ( widget );
    height = gtk_widget_get_allocated_height ( widget );

    switch ( gdkev->type ) {
        case GDK_KEY_PRESS : {
            GdkEventKey *kev = ( GdkEventKey * ) gdkev;

            switch ( kev->keyval ) {
                case GDK_KEY_Delete: {
                    common_g3duitimeline_deleteSelectedKeys ( gui );
                } break;

                case GDK_KEY_a: {
                    if ( kev->state & GDK_CONTROL_MASK ) {
                        common_timelinedata_selectAllKeys ( gui, tdata );
                    }
                } break;
            }
        } break;

        case GDK_2BUTTON_PRESS : {
            GdkEventButton *bev = ( GdkEventButton * ) gdkev;
            uint32_t keep  = ( bev->state & GDK_CONTROL_MASK ) ? 0x01 : 0x00;

            pressed_frame = common_timelinedata_getFrame ( tdata, bev->x,
                                                                  bev->y,
                                                                  width );

            onkey = common_timelinedata_selectKey ( gui, 
                                                    tdata, 
                                                    pressed_frame,
                                                    keep,
                                                    0x00,
                                                    width );

            if ( onkey ) {
                GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );

                createKeyEdit ( dial, gui, "KEYEDIT", 0, 0, 416, 192 );

                gtk_widget_show ( dial );
            }
        } break;

        case GDK_BUTTON_PRESS : {
            GdkEventButton *bev = ( GdkEventButton * ) gdkev;

            if ( ( gdkev->type == GDK_BUTTON_PRESS ) &&
                 ( bev->button == 1 ) ) {
                /*** For keyboard inputs ***/
                gtk_widget_grab_focus ( widget );

                /*** disable buffered subdivision whatever happens. because  ***/
                /*** in animation mode they are slower than their on-the-fly ***/
                /*** counterparts (re-enabled in ButtonReleased ***/
                gui->engine_flags |= ONGOINGANIMATION;

                pressed_frame = common_timelinedata_getFrame ( tdata, bev->x,
                                                                      bev->y,
                                                                      width );

                /*** First check whether or not we clicked the cursor ***/
                if ( ( pressed_frame == gui->curframe ) && ( onkey == 0x00 ) ) {
                    oncursor = 0x01;
                } else {
                    /*** else check whether or not we clicked a key ***/
                    onkey = common_timelinedata_isOnKey ( gui, 
                                                          tdata, 
                                                          pressed_frame );

                    /*** get prepared to move the key, just in case ***/
                    driftKeysToolInput ( widget, gdkev, user_data );
                }

                /*** Move the whole timeline indefinitely. For so, we hide the ***/
                /*** cursor so that the user does not see the mouse pointer    ***/
                /*** moving back to the center of the widget all the time.     ***/
                xold = xori = bev->x;
                yold = yori = bev->y;

                if ( /*( oncursor == 0x00 ) &&
                     ( onkey    == 0x00 ) && */
                     ( dragging == 0x00 ) ) {
                    g3duitimeline_hide_pointer ( widget );
                    g3duitimeline_grab_pointer ( widget, gdkev );

                    dragging = 0x01;
                }

                xacc = 0x00;
            } else {
                gtk_menu_popup ( GTK_MENU ( tim->menu ), 
                                 NULL, 
                                 NULL,
                                 /*SetMenuPosition*/NULL,
                                 /*bev*/NULL,
                                 bev->button,
                                 gdk_event_get_time( ( GdkEvent * ) gdkev ) );
            }

            gtk_widget_queue_draw ( widget );
        } break;

        case GDK_MOTION_NOTIFY : {
            GdkEventMotion *mev = ( GdkEventMotion * ) gdkev;

            if ( mev->state & GDK_BUTTON1_MASK && ( mev->x > 0x00 ) ) {

                /*** WATCH OUT: GdkEventMotion x,y are floating-point vars ***/
                xacc += ( ( int32_t ) mev->x - xold );

                if ( abs ( xacc ) >= tdata->nbpix ) {
                    /*** if we clicked the cursor, drag the cursor ***/
                    if ( onkey ) {
                        /*** move the key ***/
                        return driftKeysToolInput ( widget, gdkev, user_data );
                    } else {
                        if ( oncursor ) {
                            gui->curframe += ( xacc / ( int32_t ) tdata->nbpix );

                            common_g3dui_processAnimatedImages ( gui );

                            g3dobject_anim_r ( ( G3DOBJECT * ) sce, gui->curframe,
                                                                    gui->engine_flags );

                            g3dobject_update_r ( ( G3DOBJECT * ) sce, gui->engine_flags );

                            g3dui_updateCoords  ( gui );
                            g3dui_redrawGLViews ( gui );
                        } else {
                        /*** or else drag the whole timeline ***/
                            tdata->midframe -= ( xacc / ( int32_t ) tdata->nbpix );

                            dragging = 0x02;
                        }
                    }

                    xacc = ( xacc % ( int32_t ) tdata->nbpix );

                    g3duitimeline_move_pointer ( widget, gdkev, xori, yori );

                    gtk_widget_queue_draw ( widget );
                }
            }
        } break;

        case GDK_BUTTON_RELEASE : {
            GdkEventButton *bev = ( GdkEventButton * ) gdkev;

            /*** disable animation mode whatever happens ***/
            gui->engine_flags &= (~ONGOINGANIMATION);

            if ( dragging ) {
                g3duitimeline_ungrab_pointer ( widget, gdkev );
                g3duitimeline_show_pointer   ( widget );

                if ( onkey ) {
                    if ( ( int ) bev->x == ( int ) xori ) {
                        uint32_t keep  = ( bev->state & GDK_CONTROL_MASK ) ? 0x01 : 0x00;
                        uint32_t range = ( bev->state & GDK_SHIFT_MASK   ) ? 0x01 : 0x00;

                        common_timelinedata_selectKey ( gui, 
                                                        tdata, 
                                                        pressed_frame,
                                                        keep,
                                                        range,
                                                        width );
                    } else {
                        /*** move the keys ***/
                        driftKeysToolInput ( widget, gdkev, user_data );
                    }

                    gui->curframe = pressed_frame;
                } else {
                    if ( oncursor ) {
                        int32_t xnew = common_timelinedata_getFramePos ( tdata, gui->curframe, width );

                        /*** Recompute buffered subdivided Meshes ***/
/*
                        g3dscene_updateMeshes ( sce, gui->engine_flags );
*/
                        g3dobject_update_r ( ( G3DOBJECT * ) sce, gui->engine_flags );


                        /*** After dragging the cursor, move ***/
                        /*** the pointer to its position.    ***/
                        /*** Check the new position is within widget boundaries.***/
                        if ( ( xnew < 0x00 ) || ( xnew > width ) ) {
                            g3duitimeline_move_pointer ( widget, gdkev, xori, yori );
                        } else {
                            g3duitimeline_move_pointer ( widget, gdkev, xnew, yori );
                        }
                    } else {
                /*** If press and release position are the same, dragging equals 1***/
                /*** because dragging equals 2 when it go through MotionNotify. ***/
                        if ( dragging == 0x01 ) {
                            gui->curframe = pressed_frame;
                        }
                    }
                }

                /*g3duitimeline_move_pointer ( widget, gdkev, xori, yori );*/
            }

            oncursor = dragging = 0x00;

            g3dui_redrawGLViews ( gui );
            gtk_widget_queue_draw ( widget );
        } break;

        default:
        break;
    }

    return 0x00;
}

/******************************************************************************/
static gboolean Input ( GtkWidget *widget,
                        GdkEvent  *gdkev, 
                        gpointer   user_data ) {
    G3DUITIMELINE *tim = ( G3DUITIMELINE * ) user_data;
    G3DUI *gui = tim->grp.gui;
    TIMELINEDATA *tdata = tim->tdata;

    switch ( tdata->tool ) {
        case TIME_MOVE_TOOL :
            return driftKeysToolInput ( widget, gdkev, user_data );
        break;

        case TIME_SCALE_TOOL :
            return scaleKeysToolInput ( widget, gdkev, user_data );
        break;

        case TIME_PAN_TOOL :
            return panToolInput ( widget, gdkev, user_data );
        break;

        default :
        break;
    }

    return 0x00;
}

/******************************************************************************/
static void drawTimeline ( GtkStyleContext *context, cairo_t *cr,
                                                     TIMELINEDATA *tdata,
                                                     LIST *lobj,
                                                     int width,
                                                     int height ) {
    int nbframe    = ( tdata->nbpix ) ? ( width / tdata->nbpix ) : 0x00;
    int firstframe = ( tdata->midframe - ( nbframe >> 0x01 ) );
    int lastframe  = ( tdata->midframe + ( nbframe >> 0x01 ) );
    int x = ( width >> 0x01 ) - ( ( nbframe >> 0x01 ) * tdata->nbpix );
    int curframe;
    int csrpos = -1;
    GdkRGBA fg, bg;
    cairo_text_extents_t te;

    gtk_style_context_get_color ( context, GTK_STATE_FLAG_NORMAL, &bg );

    /*gtk_render_frame ( context, cr, 0x00, 0x00, width, height );*/
    cairo_set_line_width  ( cr, 0.5 );
    cairo_set_source_rgba ( cr, bg.red, bg.green, bg.blue, bg.alpha );
    cairo_rectangle       ( cr, 0.0f, 0.0f, width, height );
    cairo_stroke          ( cr );

    /*** Draw keys ***/
    drawObjectList ( context, cr, tdata, lobj, width, height, tdata->funcKey, tdata->funcData );

    /*** Draw Timeline ***/
    cairo_set_source_rgba ( cr, bg.red, bg.green, bg.blue, bg.alpha );

    /*** frames on the right ***/
    for ( curframe = firstframe; curframe <= lastframe; curframe++ ) {
        if ( curframe % 0x05 ) { /*** draw bigger stroke every 5 frames ***/
            cairo_set_line_width ( cr, 0.5 );
            cairo_move_to ( cr, x, 0x00 );
            cairo_line_to ( cr, x, 0x04 );
            cairo_stroke  ( cr );
        } else {
            char buf[0x10];
            int width, tx;

            snprintf ( buf, 0x10, "%d", curframe );

            cairo_text_extents ( cr, buf, &te );

            /*** Get text width in order to center it ***/
            tx = ( x - ( ( uint32_t ) te.width >> 0x01 ) );

            cairo_set_line_width ( cr, 0.5 );
            cairo_move_to ( cr,  x, 0x00 );
            cairo_line_to ( cr,  x, 0x08 );
            cairo_stroke  ( cr );

            cairo_move_to   ( cr, tx, 0x10 );
            cairo_show_text ( cr, buf );
        }

        /*** Jump to next frame position ***/
        x = ( x + tdata->nbpix );
    }
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    G3DUITIMELINE *tim = ( G3DUITIMELINE * ) user_data;

    free ( tim->tdata );
    free ( tim );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );

    /*** Render with button style ***/
    gtk_style_context_add_class ( context, GTK_STYLE_CLASS_BUTTON );
}

/******************************************************************************/
static gboolean Draw ( GtkWidget *widget, 
                       cairo_t   *cr, 
                       gpointer   user_data ) {
    G3DUITIMELINE   *tim = ( G3DUITIMELINE * ) user_data;
    TIMELINEDATA    *tdata = tim->tdata;
    G3DUI           *gui = tim->grp.gui;
    G3DSCENE        *sce = gui->sce;
    GtkStyleContext *context = gtk_widget_get_style_context ( widget );
    GtkAllocation allocation;
    int32_t csrpos;

    gtk_widget_get_allocation ( widget, &allocation );

    gtk_render_background ( context, cr,
                                     0x00,
                                     0x00,
                                     allocation.width,
                                     allocation.height );

    drawTimeline ( context, cr, tdata, sce->lsel, allocation.width, 
                                                  allocation.height );

    csrpos = common_timelinedata_getFramePos ( tdata, gui->curframe,
                                                      allocation.width );

    drawCursor ( context, cr, csrpos, 0x00, allocation.height );


    return FALSE;
}

/******************************************************************************/
void timeline_setTool ( GtkWidget *timeline, uint32_t tool ) {
    G3DUITIMELINE *tim = ( G3DUITIMELINE * ) g_object_get_data ( G_OBJECT(timeline),
                                                                 "private" );

    tim->tdata->tool = tool;
}

/******************************************************************************/
void timeline_incZoom ( GtkWidget *timeline, uint32_t nbpix ) {
    G3DUITIMELINE *tim = ( G3DUITIMELINE * ) g_object_get_data ( G_OBJECT(timeline),
                                                                 "private" );

    tim->tdata->nbpix += nbpix;

    if ( tim->tdata->nbpix > MAXIMUMFRAMEGAP ) tim->tdata->nbpix  = MAXIMUMFRAMEGAP;
}

/******************************************************************************/
void timeline_decZoom ( GtkWidget *timeline, uint32_t nbpix ) {
    G3DUITIMELINE *tim = ( G3DUITIMELINE * ) g_object_get_data ( G_OBJECT(timeline),
                                                                 "private" );

    tim->tdata->nbpix -= nbpix;

    if ( tim->tdata->nbpix < MINIMUMFRAMEGAP ) tim->tdata->nbpix  = MINIMUMFRAMEGAP;
}

/******************************************************************************/
GtkWidget *createTimeline ( GtkWidget *parent, 
                            G3DUI     *gui,
                            char      *name,
                            gint       x,
                            gint       y,
                            gint       width,
                            gint       height ) {
    G3DUITIMELINE *tim = g3duitimeline_new ( gui );
    GdkRectangle drwrec = { 0, 0, width, height };
    GtkWidget *drw;

    /*** Drawing area within the Scrolled Window ***/
    drw = gtk_drawing_area_new ( );

    g_object_set_data ( G_OBJECT(drw), "private", (gpointer) tim );

    gtk_widget_set_name ( drw, name );

    gtk_widget_size_allocate ( drw, &drwrec );
    /*gtk_widget_set_size_request ( drw, 400, 50 );*/
    /*** For keyboard inputs ***/
    gtk_widget_set_can_focus ( drw, TRUE );

    /*** Drawing area does not receive mous events by defaults ***/
    gtk_widget_set_events ( drw, gtk_widget_get_events ( drw )  |
                                 GDK_KEY_PRESS_MASK             |
			         GDK_KEY_RELEASE_MASK           |
                                 GDK_BUTTON_PRESS_MASK          |
                                 GDK_BUTTON_RELEASE_MASK        |
                                 GDK_POINTER_MOTION_MASK        |
                                 GDK_POINTER_MOTION_HINT_MASK );

/*    g_signal_connect ( G_OBJECT (drw), "size-allocate"       , G_CALLBACK (gtk3_sizeGL ), view );
*/
    g_signal_connect ( G_OBJECT (drw), "realize", G_CALLBACK (Realize), tim );
    g_signal_connect ( G_OBJECT (drw), "destroy", G_CALLBACK (Destroy), tim );

    g_signal_connect ( G_OBJECT (drw), "draw"                , G_CALLBACK (Draw), tim );

    g_signal_connect ( G_OBJECT (drw), "motion_notify_event" , G_CALLBACK (Input), tim );
    g_signal_connect ( G_OBJECT (drw), "button-press-event"  , G_CALLBACK (Input), tim );
    g_signal_connect ( G_OBJECT (drw), "button-release-event", G_CALLBACK (Input), tim );
    g_signal_connect ( G_OBJECT (drw), "key_press_event"     , G_CALLBACK (Input), tim );
    g_signal_connect ( G_OBJECT (drw), "key_release_event"   , G_CALLBACK (Input), tim );

    gtk_fixed_put ( GTK_FIXED(parent), drw, x, y );

    tim->menu = createTimeContextMenu ( drw, tim, name, 0x60, 0x60 );

    list_insert ( &gui->ltimeline, drw );

    gtk_widget_show ( drw );


    return drw;
}
