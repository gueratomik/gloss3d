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
#include <g3dui_motif.h>

/******************************************************************************/
static XtProc ClassInitialize ( void );
static XtInitProc Initialize ( Widget, Widget, ArgList, Cardinal * );
static XtRealizeProc Realize ( Widget, XtValueMask *, XSetWindowAttributes * );
static XtWidgetProc Destroy ( Widget );
static XtWidgetProc Resize ( Widget );
static XtExposeProc Redisplay ( Widget, XEvent *, Region );
static void motif_quad_createDefaultViews ( GQuadWidget );

/******************************************************************************/
static void GQuadMouseEvent ( Widget w, XEvent *event,
                              String *params, Cardinal *num_params ) {
    Dimension width, height, xpos, ypos, xratio, yratio;
    static int xflag = 0x00, yflag = 0x00;

    XtVaGetValues ( w, XtNwidth, &width, XtNheight, &height,
                       XtNxpos, &xpos, XtNypos, &ypos, NULL );

    switch ( event->type ) {
        case ButtonPress : {
            XButtonEvent *bevent = ( XButtonEvent * ) event;

            /*** start gliding on X axis ***/
            if ( ( bevent->x >= ( xpos - MARGDB ) ) &&
                 ( bevent->x <= ( xpos + MARGDB ) ) ) {

                xflag = 0x01;
            }

            /*** start gliding on Y axis ***/
            if ( ( bevent->y >= ( ypos - MARGDB ) ) &&
                 ( bevent->y <= ( ypos + MARGDB ) ) ) {

                yflag = 0x01;
            }
        } break;

        case ButtonRelease : {
            /*XButtonEvent *bevent = ( XButtonEvent * ) event;*/

            /*** stop gliding on all axis ***/
            xflag = yflag = 0x00;
        } break;

        case MotionNotify : {
            XMotionEvent *mevent = ( XMotionEvent * ) event;

            if ( mevent->x < 0x00 ) mevent->x = 0x00;
            if ( mevent->y < 0x00 ) mevent->y = 0x00;

            if ( xflag && width && ( mevent->x < ( width - MARGDB ) ) ) {
                xratio = ( ( mevent->x << 0x08 ) / width );

                XtVaSetValues ( w, XtNxratio, xratio, NULL );
            }

            if ( yflag && height && ( mevent->y < ( height - MARGDB ) ) ) {
                yratio = ( ( mevent->y << 0x08 ) / height );

                XtVaSetValues ( w, XtNyratio, yratio, NULL );
            }

            if ( xflag || yflag ) {
                Resize ( w );
            }
        } break;

        default :
        break;
    }
}

/******************************************************************************/
static char defaultTranslations[] = 
"<PtrMoved>: GQuadMouseEvent()\n\
<BtnDown>:   GQuadMouseEvent()\n\
<BtnUp>:     GQuadMouseEvent()\n";

/******************************************************************************/
static XtActionsRec Actions[] = { { "GQuadMouseEvent", GQuadMouseEvent },
                                  { "GQuadMouseEvent", GQuadMouseEvent },
                                  { "GQuadMouseEvent", GQuadMouseEvent } };

/******************************************************************************/
static XtResource widgetRes[] = {
    { XtNxpos, XtNxpos, XtRDimension, sizeof ( Dimension ),
        XtOffsetOf ( GQuadRec, quad.Xpos ),
        XtRImmediate, ( XtPointer ) 0x00 },
    { XtNypos, XtNypos, XtRDimension, sizeof ( Dimension ),
        XtOffsetOf ( GQuadRec, quad.Ypos ),
        XtRImmediate, ( XtPointer ) 0x00 },
    { XtNxratio, XtNxratio, XtRDimension, sizeof ( Dimension ),
        XtOffsetOf ( GQuadRec, quad.Xratio ),
        XtRImmediate, ( XtPointer ) 0x80 },
    { XtNyratio, XtNyratio, XtRDimension, sizeof ( Dimension ),
        XtOffsetOf ( GQuadRec, quad.Yratio ),
        XtRImmediate, ( XtPointer ) 0x80 }
/*** Crashes. I dont exactly know why and I have no time for troubleshooting***/
/*,
    { XtNsegment, XtNsegment, XtRPointer, sizeof ( XtPointer ),
        XtOffsetOf ( GQuadRec, quad.seg ),
        XtRImmediate, ( XtPointer ) NULL }*/ };

/******************************************************************************/
static XtProc ClassInitialize ( ) {

    return ( XtProc ) NULL;
}

/******************************************************************************/
static XtInitProc Initialize ( Widget request, Widget init,
                               ArgList args, Cardinal *num ) {
    GQuadWidget gqw = ( GQuadWidget ) init;
    Dimension width, height;

    XtVaGetValues ( init, XmNwidth, &width, XmNheight, &height, NULL );

    common_g3duiquad_init ( &gqw->quad, width, height );


    return ( XtInitProc ) NULL;
}

/******************************************************************************/
static XtRealizeProc Realize ( Widget w, XtValueMask *value_mask,
                               XSetWindowAttributes *attributes ) {
    /*** this is required if we want to inherit the event mask. ***/
    /*** Redisplay procedure would not be called otherwise      ***/
    xmManagerClassRec.core_class.realize ( w, value_mask, attributes );

    motif_quad_createDefaultViews ( w );


    return ( XtRealizeProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Destroy ( Widget w ) {

    return ( XtWidgetProc ) NULL;
}

/******************************* Draw child border ****************************/
static void DrawTile ( Display *dis, Drawable d, GC gc, G3DUISEGMENT *seg,
                       Pixel toppix, Pixel botpix ) {
    /*** retrieve the original rectangle ***/
    int x1 = seg->x1 - MARGIN,
        y1 = seg->y1 - MARGIN,
        x2 = seg->x2 + MARGIN,
        y2 = seg->y2 + MARGIN;

    XSetForeground ( dis, gc, toppix );
    XDrawLine ( dis, d, gc, x1, y1, x1, y2 );
    XDrawLine ( dis, d, gc, x1, y1, x2, y1 );

    XSetForeground ( dis, gc, botpix );
    XDrawLine ( dis, d, gc, x1, y2, x2, y2 );
    XDrawLine ( dis, d, gc, x2, y1, x2, y2 );
}

/******************************************************************************/
static XtWidgetProc Resize ( Widget w ) {
    GQuadWidget gqw = ( GQuadWidget ) w;
    Dimension xratio, yratio;
    Dimension width, height;
    WidgetList children;
    G3DUISEGMENT *seg = gqw->quad.seg;
    Cardinal nc;
    int i;

    /*** XmManager doesnot provide a resize function, so we have to do some ***/
    /*** routines by ourselves.                                             ***/
    if ( XtIsRealized ( w ) ) {
        XClearArea ( XtDisplay ( w ), XtWindow ( w ), 0x00, 0x00,
                                                      0x00, 0x00, True );
    }

    /*** And use the parent class of xmManager for the resizing routines ***/
    if ( xmManagerClassRec.core_class.resize ) {
        xmManagerClassRec.core_class.resize ( w );
    }

    XtVaGetValues ( w, XtNwidth, &width,
                       XtNheight, &height,
                       XtNchildren, &children,
                       XtNnumChildren, &nc,
                       XtNxratio, &xratio,
                       XtNyratio, &yratio, NULL );

    /*** Fixed point math ***/
    gqw->quad.Xpos = ( width  * xratio ) >> 0x08;
    gqw->quad.Ypos = ( height * yratio ) >> 0x08;

    if ( gqw->quad.Xpos < MARGDB ) gqw->quad.Xpos = MARGDB;
    if ( gqw->quad.Ypos < MARGDB ) gqw->quad.Ypos = MARGDB;

    /*** Set sub-rectangles ***/

    common_g3duiquad_divideSegments ( gqw->quad.seg, gqw->quad.Xpos, gqw->quad.Ypos, width, height );

    if ( gqw->maximized_view ) {
        for ( i = 0x00; ( i < nc ) && ( i < 0x04 ); i++ ) {
            Widget child = children[i];

            if ( child == gqw->maximized_view ) {
                Dimension x = 0x00 + MARGIN,
                          y = 0x00 + MARGIN,
                          w = width - MARGDB,
                          h = height - MARGDB;

                if ( w && h ) XtConfigureWidget ( child, x, y, w, h, 0x00 );
            /*** Hide the other children ***/
            } else {
                XtUnmanageChild ( child );
            }
        }
    } else {
        for ( i = 0x00; ( i < nc ) && ( i < 0x04 ); i++ ) {
            Widget child = children[i];
            Dimension x = seg[i].x1,
                      y = seg[i].y1,
                      w = ( seg[i].x2 - seg[i].x1 ),
                      h = ( seg[i].y2 - seg[i].y1 );

            /*** be sure the child is not hidden ***/
            if ( XtIsManaged ( child ) == 0x00 ) {
                XtManageChild ( child );
            }

            /*** resize child ***/
            if ( w && h ) XtConfigureWidget ( child, x, y, w, h, 0x00 );
        }
    }

    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
static XtExposeProc Redisplay ( Widget w, XEvent *event, Region region ) {
    GQuadWidget gqw = ( GQuadWidget ) w;
    Display *dis = XtDisplay ( w );
    Window win = XtWindow ( w );
    Pixel toppix, botpix;
    XGCValues values;
    G3DUISEGMENT *seg;
    GC gc;
    int i;

    gc = XtGetGC ( w, 0x00, &values );

    XtVaGetValues ( w, XmNtopShadowColor, &toppix,
                       XmNbottomShadowColor, &botpix, NULL );

    for ( i = 0x00; i < 0x04; i++ ) {
        DrawTile ( dis, win, gc, &gqw->quad.seg[i], toppix, botpix );
    }

    XtReleaseGC ( w, gc );

    return ( XtExposeProc ) NULL;
}

/******************************************************************************/
static XtSetValuesFunc set_values ( Widget current, Widget request, Widget set,
                                    ArgList args, Cardinal *numargs ) {

    return 0x00;
}

/******************************************************************************/
GQuadClassRec gquadClassRec = { { (WidgetClass) &xmManagerClassRec,/*parent*/
                                  "GQUAD",                      /* class_name */
                                  sizeof ( GQuadRec ),         /* widget size */
                       ( XtProc ) ClassInitialize,        /* class_initialize */
                                  NULL,              /* class_part_initialize */
                                  FALSE,                      /* class_inited */
                   ( XtInitProc ) Initialize,                   /* initialize */
                                  NULL,                    /* initialize_hook */
                ( XtRealizeProc ) Realize,                         /* realize */
                                  Actions/*NULL*/,                 /* actions */
                                  XtNumber ( Actions )/*0x00*/,/* num_actions */
                                  widgetRes,                      /* resouces */
                                  XtNumber ( widgetRes ),    /* num_resources */
                                  NULLQUARK,                      /*xrm_class */
                                  TRUE,                    /* compress_motion */
                                  XtExposeCompressMaximal,/*compress_exposure */
                                  TRUE,               /* compress_enterleave */
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
                                  /* Composite class part */
                                { XtInheritGeometryManager,
                                  XtInheritChangeManaged,
                                  XtInheritInsertChild,
                                  XtInheritDeleteChild,
                                  NULL }
                                  /* Constraint class part */
                                ,{ NULL,
                                  0x00,
                                  0x00,
                                  NULL,
                                  NULL,
                                  NULL,
                                  NULL },
                                  /* XmManager class part */
                                {
/* translations                 */ XtInheritTranslations,
/* syn_resources                */ NULL,
/* num_syn_resources            */ 0x00,
/* syn_constraint_resources     */ NULL,
/* num_syn_constraint_resources */ 0x00,
/* parent_process               */ XmInheritParentProcess,
/* extension                    */ NULL } };

/******************************************************************************/
WidgetClass gquadWidgetClass = ( WidgetClass ) &gquadClassRec;

/******************************************************************************/
Widget createQuad ( Widget parent, G3DUI *gui, char *name,
                    Dimension x    , Dimension y,
                    Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget quad;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    quad = XtVaCreateWidget ( name, gquadWidgetClass, parent,
                              XtNx, x,
                              XtNy, y,
                              XtNwidth , width,
                              XtNheight, height,
                              XmNforeground, foreground,
                              XmNbackground, background,
                              XmNuserData, gui,
                              NULL );

    if ( quad == NULL ) {
        fprintf ( stderr, "createQuad: Could not create Widget\n" );
    } else {
        XtManageChild ( quad );
    }

    return quad;
}


/******************************************************************************/
static void motif_quad_createDefaultViews ( GQuadWidget gqw ) {
    Pixel background, foreground;
    Widget widget = ( Widget * ) gqw;
    G3DUIQUAD *quad = &gqw->quad;
    G3DCAMERA **cam;
    G3DUI *gui;
    uint32_t i;

    XtVaGetValues ( widget, XmNuserData, &gui,
                            XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    cam = common_g3dui_createDefaultCameras ( gui );

    /*gui->defcam = gui->curcam = cam[0x00];*/

    for ( i = 0x00; i < 0x04; i++ ) {
        uint32_t vwidth  = ( quad->seg[i].x2 - quad->seg[i].x1 ),
                 vheight = ( quad->seg[i].y2 - quad->seg[i].y1 );
        Widget gvw;

        /*** Create OpenGL Views ***/

        gvw = createView ( gqw, gui, "View", cam[i], quad->seg[i].x1,
                                                     quad->seg[i].y1, vwidth, vheight );
    }
}
