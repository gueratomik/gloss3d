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
#include <g3dui_motif.h>

/******************************************************************************/
static void Input ( Widget w, XtPointer client, 
                              XEvent *event,
                              Boolean *continue_to_dispatch ) {
    Pixel background, foreground;
    Dimension width, height;
    static  int32_t xacc;
    static  int32_t xori, yori, xold, yold, xmid, ymid, pressed_frame;
    static uint32_t oncursor, onkey, dragging; 
    LIST *lselobj;
    G3DSCENE *sce;
    G3DUI *gui = ( G3DUI * ) client;
    static Time click; /*** for detecting double-clicks ***/
    TIMELINEDATA *tdata = NULL;

    /*** retrieve the global GUI structure ***/
    XtVaGetValues ( w, XmNuserData, &tdata,/*
                       XmNbackground, &background, 
                       XmNforeground, &foreground, */
                       XmNwidth, &width,
                       XmNheight, &height, NULL );

    sce = gui->sce;
    lselobj = sce->lsel;

    switch ( event->type ) {
        case KeyPress : {
            XKeyEvent *kev = ( XKeyEvent * ) event;
            KeySym keysym;
            char c;

            if ( XLookupString ( kev, &c, 0x01, &keysym, NULL ) > 0x00 ) {
                switch ( keysym ) {
                    case XK_Delete: {
                        common_timelinedata_deleteKey ( tdata, sce->lsel );

                        g3dui_redrawTimeline ( gui );
                    } break;

                    default :
                    break;
                }
            }
        } break;

        case ButtonPress : {
            XButtonEvent *bev = ( XButtonEvent * ) event;
            uint32_t keep = ( bev->state & ControlMask ) ? 0x01 : 0x00;

            /*** disable buffered subdivision whatever happens. because  ***/
            /*** in animation mode they are slower than their on-the-fly ***/
            /*** counterparts (re-enabled in ButtonReleased ***/
            gui->flags |= ONGOINGANIMATION;

            pressed_frame = common_timelinedata_getFrame ( tdata, bev->x,
                                                                  bev->y,
                                                                  width );

            /*** First check whether or not we clicked the cursor ***/
            if ( pressed_frame == gui->curframe ) {
                oncursor = 0x01;
            }/* else {*/
                /*** else check whether or not we clicked a key ***/
                onkey = common_timelinedata_selectKey ( tdata, sce->lsel, 
                                                               pressed_frame,
                                                               keep,
                                                               width );
            /*}*/

            /*** Move the whole timeline indefinitely. For so, we hide the ***/
            /*** cursor so that the user does not see the mouse pointer    ***/
            /*** moving back to the center of the widget all the time.     ***/
            xold = xori = bev->x;
            yold = yori = bev->y;

            if ( /*( oncursor == 0x00 ) &&
                 ( onkey    == 0x00 ) && */
                 ( dragging == 0x00 ) ) {
                /*g3duitimeline_grab_pointer ( widget, gdkev );
                g3duitimeline_hide_pointer ( widget );*/

                dragging = 0x01;
            }

            xacc = 0x00;

            XClearArea ( XtDisplay ( w ),
                         XtWindow  ( w ), 0x00, 0x00, 0x01, 0x01, True );
        } break;

        case MotionNotify : {
            XMotionEvent *mev = ( XMotionEvent * ) event;

            if ( mev->state & Button1Mask && ( mev->x > 0x00 ) ) {
                /*** WATCH OUT: GdkEventMotion x,y are floating-point vars ***/
                xacc += ( ( int32_t ) mev->x - xold );

                if ( abs ( xacc ) >= tdata->nbpix ) {
                    /*** if we clicked the cursor, drag the cursor ***/
                    if ( oncursor ) {
                        gui->curframe += ( xacc / ( int32_t ) tdata->nbpix );

                        g3dobject_anim_r ( ( G3DOBJECT * ) sce, gui->curframe,
                                                                gui->flags );

                        g3dui_updateCoords  ( gui );
                        g3dui_redrawGLViews ( gui );
                    } else {
                    /*** or else drag the whole timeline ***/
                        tdata->midframe -= ( xacc / ( int32_t ) tdata->nbpix );

                        dragging = 0x02;
                    }

                    xacc = ( xacc % ( int32_t ) tdata->nbpix );

                    XWarpPointer ( XtDisplay ( w ), 
                                   None, 
                                   XtWindow  ( w ), 0, 0, 0, 0, xori, yori );

                    /*g3duitimeline_move_pointer ( widget, gdkev, xori, yori );*/

                    XClearArea ( XtDisplay ( w ),
                                 XtWindow  ( w ), 0x00, 0x00, 0x01, 0x01, True );
                }
            }
        } break;

        case ButtonRelease : {
            XButtonEvent *bev = ( XButtonEvent * ) event;

            if ( dragging ) {
                /*g3duitimeline_ungrab_pointer ( widget, gdkev );
                g3duitimeline_show_pointer   ( widget );*/

                if ( oncursor ) {
                    int32_t xnew = common_timelinedata_getFramePos ( tdata, gui->curframe, width );

                    /*** Recompute buffered subdivided Meshes ***/
                    g3dscene_updateBufferedMeshes ( sce, gui->flags );

                    /*** After dragging the cursor, move ***/
                    /*** the pointer to its position.    ***/
                    /*** Check the new position is within widget boundaries.***/
                    if ( ( xnew < 0x00 ) || ( xnew > width ) ) {
                        XWarpPointer ( XtDisplay ( w ), 
                                       XtWindow  ( w ),
                                       XtWindow  ( w ), 0, 0, width, height, xori, yori );
                    } else {
                        XWarpPointer ( XtDisplay ( w ), 
                                       XtWindow  ( w ),
                                       XtWindow  ( w ), 0, 0, width, height, xnew, yori );
                    }
                } else {
            /*** If press and release position are the same, dragging equals 1***/
            /*** because dragging equals 2 when it go through MotionNotify. ***/
                    if ( dragging == 0x01 ) {
                        gui->curframe = pressed_frame;
                    }
                }

                /*g3duitimeline_move_pointer ( widget, gdkev, xori, yori );*/
            }

            /*** disable animation mode whatever happens ***/
            gui->flags &= (~ONGOINGANIMATION);

            /*** Recompute buffered subdivided Meshes ***/
            g3dscene_updateBufferedMeshes ( sce, gui->flags );

/*** Need for keyboard input !!! ***/
XmProcessTraversal(w, XmTRAVERSE_CURRENT);

             /*** if we got a first click, check the difference time ***/
            if ( click ) {
                Time dift = ( bev->time - click );

                /*** double-click on the same tile as before ***/
                if ( dift < 500 ) {
                    Display *dis = XtDisplay ( w );
                    Window root = XDefaultRootWindow ( dis );
                    int screen = DefaultScreen ( dis );
                    Visual *visual = DefaultVisual ( dis, screen );
                    XWindowAttributes wat;
                    Widget shell, edit;
                    Colormap cmap;
                    XColor scrcol, exacol;

                    XGetWindowAttributes ( dis, root, &wat );

                    cmap = XCreateColormap ( dis, root, visual, AllocNone );

                    XAllocNamedColor ( dis, cmap, BACKGROUNDCOLOR, &scrcol, &exacol );

                    shell = XtVaAppCreateShell ( NULL, "Class",
                                                 topLevelShellWidgetClass,
                                                 dis, 
                                                 XtNtitle,"Key Editor",
                                                 XtNx, ( wat.width/2) -230,
                                                 XtNy, ( wat.height/2) -96,
                                                 XmNvisual, visual,
                                                 XmNcolormap, cmap,
                                                 XtNwidth, 460,
                                                 XtNheight, 192,/*
                                                 XmNbackground, background, 
                                                 XmNforeground, foreground,*/
                                                 NULL );

                    edit = createKeyEdit ( shell, gui, "Key Edit",
                                           0, 0, 460, 192 );

                    XtManageChild ( edit );

                    XtRealizeWidget ( shell );
                }

                /*** no double-click ***/          
                click = 0x00;
            } else {
                click = bev->time;
            }

            /*** disable animation mode whatever happens ***/
            gui->flags &= (~ONGOINGANIMATION);

            oncursor = onkey = dragging = 0x00;

            g3dui_redrawGLViews ( gui );
            XClearArea ( XtDisplay ( w ),
                         XtWindow  ( w ), 0x00, 0x00, 0x01, 0x01, True );
        } break;

        default : {

        } break;
    }
}

/******************************************************************************/
static XtWidgetProc Destroy ( Widget w ) {

    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
/*** frame = the frame with the key                                         ***/
/*** midframe = the frame in the middle of the widget                       ***/
/*** nbpix = number of pixels between 2 frames                              ***/
/*** width = the widget width                                               ***/
/*** height = the widget height                                             ***/
/******************************************************************************/
static void drawKey ( Display *dis, Window win, GC gc,
                      int frame, int midframe,
                      int nbpix, int width, int height,
                      Pixel keypix,    /*** key color    ***/
                      Pixel brdpix ) { /*** border color ***/
    int32_t midx     = ( width >> 0x01 ),
            difframe = ( midframe - frame ),
            framepos = midx - ( difframe * nbpix );

    /*** Draw frame only if it's within the widget window ***/
    if ( framepos >= 0x00 && framepos <= width ) {
    

        XSetForeground ( dis, gc, brdpix );
        XFillRectangle ( dis, win, gc, framepos - 0x02, 0, 0x05, height     );

        XSetForeground ( dis, gc, keypix );
        XFillRectangle ( dis, win, gc, framepos - 0x01, 2, 0x03, height - 4 );
    }
}

/******************************************************************************/
static void drawObject ( Display *dis, Window win, GC gc,
                         G3DOBJECT *obj,
                         int midframe,
                         int nbpix, int width, int height,
                         Pixel keypix,    /*** key color    ***/
                         Pixel brdpix ) { /*** border color ***/
    LIST *ltmp = obj->lkey;

    while ( ltmp ) {
        G3DKEY *key = ( G3DKEY * ) ltmp->data;

        if ( key->flags & KEYSELECTED ) {
            drawKey ( dis, win, gc,
                      key->frame, midframe,
                      nbpix, width, height, brdpix, keypix );
        } else {
            drawKey ( dis, win, gc,
                      key->frame, midframe,
                      nbpix, width, height, keypix, brdpix );
        }

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
static void drawObjectList ( Display *dis, Window win, GC gc,
                             LIST *lobj,
                             int midframe,
                             int nbpix, int width, int height,
                             Pixel keypix,    /*** key color    ***/
                             Pixel brdpix ) { /*** border color ***/
    LIST *ltmp = lobj;

    while ( ltmp ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmp->data;

        drawObject ( dis, win, gc,
                     obj,
                     midframe, nbpix, width, height, keypix, brdpix );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
static void drawCursor ( int x, int y, int width, int height, 
                         Display *dis, Window win, GC gc ) {
    XPoint csr[0x20] = {{x-3,0},{x-2,0},{x-1,0},{x,0},{x+1,0},{x+2,0},{x+3,0},
                        {x-3,1},{x-2,1},{x-1,1},{x,1},{x+1,1},{x+2,1},{x+3,1},
                                {x-2,2},{x-1,2},{x,2},{x+1,2},{x+2,2},
                                {x-2,3},{x-1,3},{x,3},{x+1,3},{x+2,3},
                                        {x-1,4},{x,4},{x+1,4},
                                        {x-1,5},{x,5},{x+1,5},
                                                {x,6},
                                                {x,7}};

    if ( ( x >= 0x00 ) && ( x <= width ) ) {
        XDrawPoints ( dis, win, gc, csr, 0x20, CoordModeOrigin );
        XDrawLine   ( dis, win, gc, x, y, x, y + ( height - 0x01 ) );
    }
}

/******************************************************************************/
static void drawTimeline ( int midframe,             /* Frame in the middle   */
                           int selframe,             /* Frame with the cursor */
                           int nbpix,    /* Number of pixels between 2 frames */
                           int width, int height,    /* Timeline dimensions   */
                           unsigned long foreground, /* Foreground color      */
                           unsigned long background, /* Background color      */
                           unsigned long keycolor,   /* key color             */
                           LIST *lobj,    /* objects whose keys must be drawn */
                           XFontStruct *xfs,         /* Font to use           */
                           Display *dis,             /* Display               */
                           Window win,               /* Drawable              */
                           GC gc ) {                 /* Graphic Context       */
    int nbframe    = ( nbpix ) ? ( width / nbpix ) : 0x00;
    int firstframe = ( midframe - ( nbframe >> 0x01 ) );
    int lastframe  = ( midframe + ( nbframe >> 0x01 ) );
    int x = ( width >> 0x01 ) - ( ( nbframe >> 0x01 ) * nbpix );
    int curframe;
    int csrpos = -1;

    /*** XFillRectangle uses foreground color ***/
    XSetBackground ( dis, gc, background );
    XSetForeground ( dis, gc, background );
    XFillRectangle ( dis, win, gc, 0x00, 0x00, width, height );

    /*** Draw keys ***/
    drawObjectList ( dis, win, gc,
                     lobj,
                     midframe, nbpix, width, height,
                     keycolor, XBlackPixel ( dis, 0x00 ) );

    /*** Draw Timeline ***/
    XSetForeground ( dis, gc, foreground );

    /*** frames on the right ***/
    for ( curframe = firstframe; curframe <= lastframe; curframe++ ) {
        if ( curframe % 0x05 ) { /*** draw bigger stroke every 5 frames ***/
            XDrawLine ( dis, win, gc, x, 0x00, x, 0x04 );
        } else {
            char buf[0x10];
            XTextItem text = { .chars = buf,        /* pointer to string    */
                               .delta = 0x00,       /* number of characters */
                               .font  = xfs->fid }; /* Font to print it in  */
            int width, tx;

            snprintf ( buf, 0x10, "%d", curframe );

            /*** text len ***/
            text.nchars = strlen ( buf );

            /*** Get text width in order to center it ***/
            width = XTextWidth ( xfs, text.chars, text.nchars );
            tx = ( x - ( width >> 0x01 ) );

            XDrawLine ( dis, win, gc, x , 0x00, x    , 0x08 );
            XDrawText ( dis, win, gc, tx, 0x10, &text, 0x01 );
        }

        /*** Store the cursor position for drawCursor ( ... ) ***/
        if ( curframe == selframe ) csrpos = x;

        /*** Jump to next frame position ***/
        x = ( x + nbpix );
    }

    XSetForeground ( dis, gc, XBlackPixel ( dis, 0x00 ) );
    drawCursor ( csrpos, 0x00, width, height, dis, win, gc );
}

/******************************************************************************/
static XtExposeProc Redisplay ( Widget w, XtPointer client, XtPointer call ) {
    Display *dis = XtDisplay ( w );
    Window win = XtWindow ( w );
    Dimension width, height;
    Pixel toppix, botpix;
    XGCValues values;
    G3DUI *gui = ( G3DUI * ) client;
    GC gc;
    G3DUIMOTIF *gmt;
    TIMELINEDATA *tdata = NULL;

    /*** retrieve the global GUI structure ***/
    XtVaGetValues ( w, XmNuserData, &tdata,
                       XmNwidth, &width,
                       XmNheight, &height,
                       XmNtopShadowColor, &toppix,
                       XmNbottomShadowColor, &botpix,
                       NULL );

    gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    gc = XtGetGC ( w, 0x00, &values );

    drawTimeline ( tdata->midframe, /*** the frame on Widget center ***/
                   gui->curframe,     /*** the frame we are on        ***/
                   tdata->nbpix,
                   width, height,
                   toppix,
                   botpix,
                   XWhitePixel ( dis, 0x00 ),
                   gui->sce->lsel,
                   gmt->tinyft, dis, win, gc );

    XtReleaseGC ( w, gc );

    return ( XtExposeProc ) NULL;
}

/******************************************************************************/
Widget createTimeline ( Widget parent, G3DUI *gui, char *name,
                         Dimension x    , Dimension y,
                         Dimension width, Dimension height ) {
    TIMELINEDATA *tdata = common_timelinedata_new ( );
    Widget drw;

    drw = XmVaCreateDrawingArea ( parent, name,
                                         XtNx, x,
                                        XtNy, y,
                                        XtNwidth , width,
                                        XtNheight, height,
                                        /*** needed for keyboard input ***/  XmNtraversalOn,   TRUE,
                                        /*XmNancestorSensitive, TRUE,
                                        XmNsensitive, TRUE,
                                        XmNkeyboardFocusPolicy, XmEXPLICIT,*/
                                        XmNuserData, tdata,
                                        NULL );

    XtAddCallback ( drw, XmNexposeCallback, Redisplay, gui );

    XtAddEventHandler ( drw, ButtonPressMask   | 
                             ButtonReleaseMask | 
                             KeyPressMask      | 
                             KeyReleaseMask    | 
                             PointerMotionMask, False, Input, gui );

    /*** add this widget to the list of Timelines widget for easy update ***/
    list_insert ( &gui->ltimeline, drw );


    XtManageChild ( drw );

    return drw;
}
