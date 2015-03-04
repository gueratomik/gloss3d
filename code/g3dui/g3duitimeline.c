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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2014         */
/*                                                                            */
/******************************************************************************/

/******************************************************************************/
/*                                                                            */
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>

/******************************************************************************/
#include <unistd.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/stat.h>
#include <mqueue.h>

/******************************************************************************/
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

/******************************************************************************/
#include <sys/ipc.h>
#include <sys/shm.h>
#include <X11/extensions/XShm.h>

/******************************************************************************/
#include <pthread.h>

/******************************************************************************/
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>
#include <Xm/Manager.h>
#include <Xm/ManagerP.h>
#include <Xm/Primitive.h>
#include <Xm/PrimitiveP.h>

/******************************************************************************/
#include <X11/xpm.h>

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/GLwMDrawA.h>

/******************************************************************************/
#include <list.h>
#include <g3d.h>
#include <g3dcom.h>
#include <r3d.h>
#include <r3dnet.h>
#include <g3durmanager.h>
#include <g3dmouse.h>
#include <g3dui.h>

/******************************************************************************/
#include <xpm/record.xpm>
#include <xpm/stopanim.xpm>
#include <xpm/playanim.xpm>
#include <xpm/prevframe.xpm>
#include <xpm/nextframe.xpm>
#include <xpm/zoomtime.xpm>
#include <xpm/unzoomtime.xpm>

/******************************************************************************/
static XtProc ClassInitialize ( void );
static XtInitProc Initialize ( Widget, Widget, ArgList, Cardinal * );
static XtRealizeProc Realize ( Widget, XtValueMask *, XSetWindowAttributes * );
static XtWidgetProc Destroy ( Widget );
static XtWidgetProc Resize ( Widget );
static XtExposeProc Redisplay ( Widget, XEvent *, Region );

/******************************************************************************/
G3DUITIMELINE *g3duitimeline_new ( uint32_t fps, uint32_t nbframes ) {
    uint32_t structsize = sizeof ( G3DUITIMELINE );
    G3DUITIMELINE *tim = ( G3DUITIMELINE * ) calloc ( 0x01, structsize );

    if ( tim == NULL ) {
        fprintf ( stderr, "g3duitimeline_new: memory allocation failed\n" );

        return NULL;
    }

    tim->fps      = fps;
    tim->nbframes = nbframes;
    tim->curframe = 0x00;

    return tim;
}

/******************************************************************************/
/*** Function = Did we click on the timeline cursor  ?                      ***/
/*** x, y = mouse coord                                                     ***/
/*** nbpix= number of pixels between 2 frames                               ***/
/*** curframe = the frame the cursor is on                                  ***/
/*** midframe = frame in the middle of the widget                           ***/
/*** width = width of the widget                                            ***/
/******************************************************************************/
static int onCursor ( int x, int y, int nbpix, int midframe,
                                               int curframe, int width ) {
    int32_t midx  = ( width >> 0x01 ),
            difx  = ( x - midx ),
            /*** let's go to the nearest frame ***/
            nbfrm = ( int32_t ) roundf ( ( float ) difx / nbpix );

    if ( curframe == ( midframe + nbfrm ) ) {

        return 0x01;
    }

    return 0x00;
}

/******************************************************************************/
static void GTimelineMouseEvent ( Widget w, XEvent *event,
                                  String *params, Cardinal *num_params ) {
    GTimelineWidget gtw = ( GTimelineWidget ) w;
    Pixel background, foreground;
    Dimension width, height;
    static int xold, xacc, xpre, oncur;
    LIST *lselobj;
    G3DSCENE *sce;
    G3DUI *gui;
    static Time click; /*** for detecting double-clicks ***/

    /*** retrieve the global GUI structure ***/
    XtVaGetValues ( w, XmNuserData, &gui,
                            XmNbackground, &background, 
                            XmNforeground, &foreground, 
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
                        if ( lselobj ) {
                            LIST *ltmpobj = lselobj;

                            while ( ltmpobj ) {
                                G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

                                g3dobject_removeSelectedKeys ( obj );

                                ltmpobj = ltmpobj->next;
                            }

                            redrawTimeline ( gui );
                        }
                    } break;

                    default :
                    break;
                }
            }
        } break;

        case ButtonPress : {
            XButtonEvent *bev = ( XButtonEvent * ) event;
            G3DUITIMELINE *tim = gui->tim;

            if ( onCursor ( bev->x, bev->y,
                            gtw->timeline.nbpix,
                            gtw->timeline.midframe,
                            tim->curframe, width ) ) {

                oncur = 0x01;
            }

            /*** disable buffered subdivision whatever happens. because  ***/
            /*** in animation mode they are slower than their on-the-fly ***/
            /*** counterparts (re-enabled in ButtonReleased ***/
            gui->flags |= ONGOINGANIMATION;

            xold = xpre = bev->x;
            xacc = 0x00;
        } break;

        case MotionNotify : {
            XMotionEvent *mev = ( XMotionEvent * ) event;

            if ( mev->state & Button1Mask ) {
                int nbpix = gtw->timeline.nbpix;
                G3DUITIMELINE *tim = gui->tim;

                xacc += ( mev->x - xold );
                if ( abs ( xacc ) >= nbpix ) {
                    /*** if we clicked the cursor, drag the cursor ***/
                    if ( oncur ) {
                        tim->curframe += ( xacc / nbpix );

                        g3dobject_anim_r ( ( G3DOBJECT * ) sce, tim->curframe,
                                                                gui->flags );

                        updateCoords  ( gui );
                        redrawGLViews ( gui );
                    } else {
                    /*** or else drag the whole timeline ***/
                        gtw->timeline.midframe -= ( xacc / nbpix );
                    }

                    xacc = ( xacc % nbpix );
                }

                XClearArea ( XtDisplay ( w ),
                             XtWindow  ( w ), 0x00, 0x00, 0x01, 0x01, True );
            }

            /*** no double-click ***/          
            click = 0x00;

            xold = mev->x;
        } break;

        case ButtonRelease : {
            XButtonEvent *bev = ( XButtonEvent * ) event;
            G3DUITIMELINE *tim = gui->tim;

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
                    XWindowAttributes wat;
                    Widget shell, edit;

                    XGetWindowAttributes ( dis, root, &wat );

                    shell = XtVaAppCreateShell ( NULL, "Class",
                                                 topLevelShellWidgetClass,
                                                 dis, 
                                                 XtNtitle,"Key Editor",
                                                 XtNx, ( wat.width/2) -145,
                                                 XtNy, ( wat.height/2) -50,
                                                 XtNwidth, 290,
                                                 XtNheight, 100,
                                                 XmNbackground, background, 
                                                 XmNforeground, foreground,
                                                 NULL );

                    edit = createKeyEdit ( shell, gui, "Key Edit",
                                           0x00, 0x00, 290, 100 );

                    XtManageChild ( edit );

                    XtRealizeWidget ( shell );
                }

                /*** no double-click ***/          
                click = 0x00;
            } else {
                click = bev->time;
            }

            /*** if no timeline browse was made, we move the cursor ***/
            if ( xpre == bev->x ) {
                int32_t midx  = ( width >> 0x01 ),
                        difx  = ( bev->x - midx ),
                        /*** nbpix = number of pixels between 2 frames ***/
                        nbpix = gtw->timeline.nbpix,
                        /*** let's go to the nearest frame ***/
                        nbfrm = ( int32_t ) roundf ( ( float ) difx / nbpix );
                G3DKEY *key;

                tim->curframe = ( gtw->timeline.midframe + nbfrm );

                if ( lselobj ) {
                    LIST *lbeg, *lend, *lequ;
                    LIST *ltmpobj = lselobj;

                    while ( ltmpobj ) {
                        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

                        if ( ( bev->state & ControlMask ) == 0x00 ) {
                            g3dobject_unselectAllKeys ( obj );
                        }

                        key = g3dobject_getKeyByFrame ( obj, tim->curframe );

                        if ( key ) {
                            g3dobject_selectKey ( obj, key );
                        }

                        ltmpobj = ltmpobj->next;
                    }
                }

                /*** Redraw ***/
                XClearArea ( XtDisplay ( w ),
                             XtWindow  ( w ), 0x00, 0x00, 0x00, 0x00, True );
            }

            redrawGLViews ( gui );

            oncur = 0x00;
        } break;

        default : {

        } break;
    }
}

/******************************************************************************/
static char defaultTranslations[] = 
"<PtrMoved>: GTimelineMouseEvent()\n\
<BtnDown>:   GTimelineMouseEvent()\n\
<BtnUp>:     GTimelineMouseEvent()\n\
<KeyPress>:  GTimelineMouseEvent()\n";

/******************************************************************************/
static XtActionsRec Actions[] = {{ "GTimelineMouseEvent", GTimelineMouseEvent}};

/******************************************************************************/
static XtProc ClassInitialize ( ) {


    return ( XtProc ) NULL;
}

/******************************************************************************/
static XtInitProc Initialize ( Widget request, Widget init,
                               ArgList args, Cardinal *num ) {

    return ( XtInitProc ) NULL;
}

/******************************************************************************/
static XtRealizeProc Realize ( Widget w, XtValueMask *value_mask,
                               XSetWindowAttributes *attributes ) {
    GTimelineWidget gtw = ( GTimelineWidget ) w;
    XColor purecol, bestcol;
    Colormap cmap;

    /*** this is required if we want to inherit the event mask. ***/
    /*** Redisplay procedure would not be called otherwise      ***/
    xmPrimitiveClassRec.core_class.realize ( w, value_mask, attributes );

    /*** Number of pixel between 2 frames ***/
    gtw->timeline.nbpix = DEFPIXELGAP;

    return ( XtRealizeProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Destroy ( Widget w ) {

    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Resize ( Widget w ) {

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
static XtExposeProc Redisplay ( Widget w, XEvent *event, Region region ) {
    GTimelineWidget gtw = ( GTimelineWidget ) w;
    Display *dis = XtDisplay ( w );
    Window win = XtWindow ( w );
    Dimension width, height;
    Pixel toppix, botpix;
    XGCValues values;
    G3DUI *gui;
    GC gc;

    /*** retrieve the global GUI structure ***/
    XtVaGetValues ( w, XmNuserData, &gui,
                       XmNwidth, &width,
                       XmNheight, &height,
                       XmNtopShadowColor, &toppix,
                       XmNbottomShadowColor, &botpix,
                       NULL );

    gc = XtGetGC ( w, 0x00, &values );

    drawTimeline ( gtw->timeline.midframe, /*** the frame on Widget center ***/
                   gui->tim->curframe,     /*** the frame we are on        ***/
                   gtw->timeline.nbpix,
                   width, height,
                   toppix,
                   botpix,
                   XWhitePixel ( dis, 0x00 ),
                   gui->sce->lsel,
                   gui->tinyft, dis, win, gc );

    XtReleaseGC ( w, gc );

    return ( XtExposeProc ) NULL;
}

/******************************************************************************/
static XtSetValuesFunc set_values ( Widget current, Widget request, Widget set,
                                    ArgList args, Cardinal *numargs ) {

    return 0x00;
}

/******************************************************************************/
GTimelineClassRec gtimelineClassRec = 
                              { { (WidgetClass) &xmPrimitiveClassRec,/*parent*/
                                  "GTIMELINE",                  /* class_name */
                                  sizeof ( GTimelineRec ),     /* widget size */
                       ( XtProc ) ClassInitialize,        /* class_initialize */
                                  NULL,              /* class_part_initialize */
                                  FALSE,                      /* class_inited */
                   ( XtInitProc ) Initialize,                   /* initialize */
                                  NULL,                    /* initialize_hook */
                ( XtRealizeProc ) Realize,                         /* realize */
                                  Actions/*NULL*/,                 /* actions */
                                  XtNumber ( Actions )/*0x00*/,/* num_actions */
                                  /*widgetRes*/NULL,              /* resouces */
                                  /*XtNumber ( widgetRes )*/0x00,/* num_resources */
                                  NULLQUARK,                      /*xrm_class */
                                  TRUE,                    /* compress_motion */
                                  XtExposeCompressMaximal,/*compress_exposure */
                                  TRUE,                /* compress_enterleave */
                                  FALSE,                  /* visible_interest */
                ( XtWidgetProc )  Destroy,                          /*destroy */
                ( XtWidgetProc )  Resize,                           /* resize */
                ( XtExposeProc )  Redisplay,                        /* expose */
             ( XtSetValuesFunc )  set_values,                   /* set_values */
                                  NULL,                    /* set_values_hook */
                                  XtInheritSetValuesAlmost,/*set_values_almost*/
                                  NULL,                    /* get_values_hook */
                                  NULL,                       /* accept_focus */
                                  XtVersion,                       /* version */
                                  NULL,                   /* callback_offsets */
                                  defaultTranslations,            /* tm_table */
                                  XtInheritQueryGeometry,   /* query_geometry */
                                  NULL,                /* display_accelerator */
                                  NULL },                        /* extension */
                                  /* Primitive class part */
                                { XmInheritBorderHighlight,
                                  XmInheritBorderUnhighlight,
                                  XtInheritTranslations,
                                  NULL,
                                  NULL,
                                  0x00,
                                  NULL },
                                /*** Timeline Class part ***/
                                { 0x00 } };

/******************************************************************************/
WidgetClass gtimelineWidgetClass = ( WidgetClass ) &gtimelineClassRec;

/******************************************************************************/
static GTimelineWidget getTimelineWidget ( Widget parent ) {
    if ( parent ) {
        WidgetList children;
        Cardinal nc;
        int i;
        
        XtVaGetValues ( parent, XmNchildren, &children,
                                XmNnumChildren, &nc,
                                NULL );

        for ( i = 0x00; i < nc; i++ ) {
            Widget child = children[i];

            if ( strcmp ( TIMELINENAME, XtName ( child ) ) == 0x00 ) {

                return ( GTimelineWidget ) child;
            }
        }
    }

    return NULL;
}

/******************************************************************************/
void recordcbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget parent = XtParent ( w );
    GTimelineWidget gtw = getTimelineWidget ( parent );
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DUITIMELINE *tim = gui->tim;
    LIST *lselobj = sce->lsel;
    LIST *ltmpobj = lselobj;

    while ( ltmpobj ) {
        G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

        g3dobject_pose ( obj, ( float ) gui->tim->curframe, &obj->pos,
                                                            &obj->rot,
                                                            &obj->sca );

        printf("Keying at frame %d for %s\n", gui->tim->curframe, obj->name );

        if ( tim->curframe > tim->endframe   ) tim->endframe   = tim->curframe;
        if ( tim->curframe < tim->startframe ) tim->startframe = tim->curframe;

        ltmpobj = ltmpobj->next;
    }

    /*** Redraw Timeline ***/
    if ( gtw ) {
        XClearArea ( XtDisplay ( gtw ), XtWindow  ( gtw ),
                     0x00, 0x00, 0x00, 0x00, True );
    }

    /*** redraw key editors ***/
    g3dui_updateAllKeyEdits ( gui );
}

/******************************************************************************/
void stopcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    /*** if scene is currently played ***/
    if ( gui->playthreadid ) {
        pthread_cancel ( gui->playthreadid );
        /*** COMMENTED. Causes crash when using both ***/
        /*pthread_join   ( gui->playthreadid, NULL );*/
    }
}

/******************************************************************************/
void  playcbk_t_clean ( void *ptr ) {
    G3DUI *gui = ( G3DUI * ) ptr;

    gui->playthreadid = 0x00;

    /*** stop forcing real time subdivision ***/
    gui->flags &= (~ONGOINGANIMATION);

    g3dscene_updateBufferedMeshes ( gui->sce, gui->flags );
}

/******************************************************************************/
void *playcbk_t ( void *ptr ) {
    G3DUI *gui = ( G3DUI * ) ptr;
    G3DSCENE *sce = gui->sce;
    G3DCOMGOTOFRAME nxf;
    G3DUITIMELINE *tim = gui->tim;

    pthread_cleanup_push ( playcbk_t_clean, gui );

    while ( 1 ) {
        /*** we tell the signal handler to set the scene to its starting frame ***/
        g3dcomgotoframe_init ( &nxf, sce, tim->curframe++ );

        g3dcomgotoframe_suspend ( &nxf );

        /*** Tell Gloss3D to transform matrices for the current frame ***/
        sigqueue ( getpid ( ), SIGUSR1, (const union sigval)(void*) &nxf );

        /*** Wait until matrix transformations are done ***/
        g3dcomgotoframe_wait ( &nxf );

        redrawGLViews ( gui );

        /*** wow, is this standard ? I had never used such statement style ***/
        /*** BTW, libGL crashes when it goes to fast, this call is required ***/
        nanosleep((struct timespec[]){{0, 40000000}}, NULL);
    }

    pthread_cleanup_pop (1);

    pthread_exit ( NULL );


    return NULL;
}

/******************************************************************************/
void playcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    /*** Force disabling real time subdivision ***/
    gui->flags |= ONGOINGANIMATION;

    /*** if scene is not currently played ***/
    if ( gui->playthreadid == 0x00 ) {
        pthread_attr_t attr;

        pthread_attr_init ( &attr );

        /*** launch rays in a thread ***/
        pthread_create ( &gui->playthreadid, &attr, playcbk_t, gui );

        /*** prepare to release resources after thread termination ***/
        pthread_detach ( gui->playthreadid );
    }
}

/******************************************************************************/
void nextcbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget parent = XtParent ( w );
    GTimelineWidget gtw = getTimelineWidget ( parent );
    G3DCOMGOTOFRAME nxf;
    G3DUI *gui;

    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    if ( gui ) {
        G3DUITIMELINE *tim = gui->tim;
        G3DSCENE *sce = gui->sce;

        /*** Force disabling real time subdivision ***/
        gui->flags |= ONGOINGANIMATION;

        tim->curframe++;

        /*** we tell the signal handler to set the scene to its starting frame ***/
        g3dcomgotoframe_init ( &nxf, sce, tim->curframe );

        g3dcomgotoframe_suspend ( &nxf );

        /*** Tell Gloss3D to transform matrices for the current frame ***/
        sigqueue ( getpid ( ), SIGUSR1, (const union sigval)(void*) &nxf );

        /*** Wait until matrix transformations are done ***/
        g3dcomgotoframe_wait ( &nxf );

        /*** Re-enable real time subdivision ***/
        gui->flags &= (~ONGOINGANIMATION);

        /*** Update buffered subdivided meshes ***/
        g3dscene_updateBufferedMeshes ( gui->sce, gui->flags );

        redrawGLViews ( gui );

        XClearArea ( XtDisplay ( gtw ),
                     XtWindow  ( gtw ),
                     0x00, 0x00, 0x00, 0x00, True );
    }
}

/******************************************************************************/
void prevcbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget parent = XtParent ( w );
    GTimelineWidget gtw = getTimelineWidget ( parent );
    G3DCOMGOTOFRAME nxf;
    G3DUI *gui;

    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    if ( gui ) {
        G3DUITIMELINE *tim = gui->tim;
        G3DSCENE *sce = gui->sce;

        /*** Force disabling real time subdivision ***/
        gui->flags |= ONGOINGANIMATION;

        tim->curframe--;

        /*** we tell the signal handler to set the scene to its starting frame ***/
        g3dcomgotoframe_init ( &nxf, sce, tim->curframe );

        g3dcomgotoframe_suspend ( &nxf );

        /*** Tell Gloss3D to transform matrices for the current frame ***/
        sigqueue ( getpid ( ), SIGUSR1, (const union sigval)(void*) &nxf );

        /*** Wait until matrix transformations are done ***/
        g3dcomgotoframe_wait ( &nxf );

        /*** Re-enable real time subdivision ***/
        gui->flags &= (~ONGOINGANIMATION);

        /*** Update buffered subdivided meshes ***/
        g3dscene_updateBufferedMeshes ( gui->sce, gui->flags );

        redrawGLViews ( gui );

        XClearArea ( XtDisplay ( gtw ),
                     XtWindow  ( gtw ),
                     0x00, 0x00, 0x00, 0x00, True );
    }
}

/******************************************************************************/
void zoomcbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget parent = XtParent ( w );
    GTimelineWidget gtw = getTimelineWidget ( parent );

    if ( gtw ) {
        if ( gtw->timeline.nbpix < MAXPIXELGAP ) {
            gtw->timeline.nbpix += 0x02;
        } else {
            gtw->timeline.nbpix = MAXPIXELGAP;
        }

        XClearArea ( XtDisplay ( gtw ),
                     XtWindow  ( gtw ),
                     0x00, 0x00, 0x00, 0x00, True );
    }
}

/******************************************************************************/
void unzoomcbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget parent = XtParent ( w );
    GTimelineWidget gtw = getTimelineWidget ( parent );

    if ( gtw ) {
        if ( gtw->timeline.nbpix > MINPIXELGAP ) {
            gtw->timeline.nbpix -= 0x02;
        } else {
            gtw->timeline.nbpix = MINPIXELGAP;
        }

        XClearArea ( XtDisplay ( gtw ),
                     XtWindow  ( gtw ),
                     0x00, 0x00, 0x00, 0x00, True );
    }
}

/******************************************************************************/
static Widget addPushButton ( Widget parent, int x, int y,
                              char *name, char **data,
                              void (*buttonfunc)( Widget,
                                                XtPointer,
                                                XtPointer ) ){
    Pixel background, foreground;
    Pixmap pixnor, pixarm;
    Dimension height;
    Widget btn;
    G3DUI *gui;
    int depth;

    XtVaGetValues ( parent, XmNdepth, &depth,
                            XmNuserData, &gui,
                            XmNheight, &height,
                            XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    createTransparentPixmap ( XtDisplay ( parent ),
                              &pixnor, &pixarm,
                              data, depth,
                              background, foreground );

    btn = XmVaCreateManagedPushButton ( parent, name,
                                        XtNx, x,
                                        XtNy, y,
                                        XtNwidth , 0x10,
                                        XtNheight, height,
                                        XmNlabelType, XmPIXMAP,
                                        XmNlabelPixmap , pixnor,
                                        XmNselectPixmap, pixarm,
                                        XmNhighlightThickness, 0x00,
                                        XmNshadowThickness, 0x01,
                                        XmNmarginHeight, 0x00,
                                        XmNmarginWidth, 0x00,
                                        XmNbackground, background,
                                        XmNforeground, foreground,
                                        XmNuserData, gui,
                                        NULL );

    XtAddCallback ( btn, XmNarmCallback, buttonfunc, gui );


    return btn;
}

/******************************************************************************/
Widget createTimeboard ( Widget parent, G3DUI *gui, char *name,
                         Dimension x    , Dimension y,
                         Dimension width, Dimension height ) {
    Pixel background, foreground;
    Pixmap pixnor, pixarm;
    Widget tfrm;
    Widget tline;
    Widget b;
    int depth;

    XtVaGetValues ( parent, XmNdepth, &depth,
                            XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    tfrm = XmVaCreateForm ( parent, name,
                            XtNx, x,
                            XtNy, y,
                            XtNwidth , width,
                            XtNheight, height,
                            XmNbackground, background,
                            XmNforeground, foreground,
/*** needed for keyboard input ***/  XmNtraversalOn,   TRUE,
XmNancestorSensitive, TRUE,
XmNsensitive, TRUE,
                            XmNuserData, gui,
                            NULL );

    /***********************************************************************/
    b = addPushButton ( tfrm, 0x00, 0x00, "REC"   , record_xpm    , recordcbk );
    b = addPushButton ( tfrm, 0x10, 0x00, "ZOOM"  , zoomtime_xpm  , zoomcbk   );
    b = addPushButton ( tfrm, 0x20, 0x00, "UNZOOM", unzoomtime_xpm, unzoomcbk );
    b = addPushButton ( tfrm, 0x30, 0x00, "PREV"  , prevframe_xpm , prevcbk   );
    b = addPushButton ( tfrm, 0x40, 0x00, "NEXT"  , nextframe_xpm , nextcbk   );
    b = addPushButton ( tfrm, 0x50, 0x00, "STOP"  , stopanim_xpm  , stopcbk   );
    b = addPushButton ( tfrm, 0x60, 0x00, "PLAY"  , playanim_xpm  , playcbk   );

    tline = XtVaCreateWidget ( TIMELINENAME, gtimelineWidgetClass,
                               tfrm,
                               XtNx, 0x70,
                               XtNy, 0x00,
                               XtNwidth , width - 0x70,
                               XtNheight, height,

                               XmNtopAttachment   , XmATTACH_FORM,
                               XmNrightAttachment , XmATTACH_FORM,
                               XmNleftAttachment  , XmATTACH_WIDGET,
                               XmNleftWidget, b,
/*** needed for keyboard input ***/  XmNtraversalOn,   TRUE,
XmNancestorSensitive, TRUE,
XmNsensitive, TRUE,
XmNkeyboardFocusPolicy, XmEXPLICIT,
                               XmNuserData, gui,
                               NULL );


    /*** add this widget to the list of Timelines widget for easy update ***/
    list_insert ( &gui->ltimeline, tline );


    XtManageChild ( tline );


    XtManageChild ( tfrm );

    return tfrm;
}
