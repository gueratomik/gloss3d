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

/******************************************************************************/
static void XmTabbedBookMouseEvent ( Widget w, XEvent *event,
                                     String *params, Cardinal *num_params ) {
    XmTabbedBookWidget tbw = ( XmTabbedBookWidget ) w;

    switch ( event->type ) {
        case ButtonPress : {
            LIST *ltmp = tbw->tabbedbook.ltab;
            XButtonEvent *bev = ( XButtonEvent * ) event;

            /*** First get the selected tab ***/
            while ( ltmp ) {
                XMTAB *tab = ( XMTAB * ) ltmp->data;

                if ( ( bev->x > tab->x ) && bev->x < ( tab->x + tab->width ) &&
                     ( bev->y > tab->y ) && bev->y < ( tab->y + tab->height ) ){

                    tbw->tabbedbook.seltab = tab;
                }

                ltmp = ltmp->next;
            }

            /*** if any, unmap all widgets but the ***/
            /*** one related to the selected tab.  ***/
            if ( tbw->tabbedbook.seltab ) {
                LIST *ltmp = tbw->tabbedbook.ltab;

                while ( ltmp ) {
                    XMTAB *tab = ( XMTAB * ) ltmp->data;

                    if ( tab != tbw->tabbedbook.seltab ) {
                        XtUnmapWidget ( tab->w );
                    } else {
                        XtMapWidget   ( tab->w );
                    }

                    ltmp = ltmp->next;
                }

            }

            XClearArea ( XtDisplay ( w ), XtWindow ( w ), 0, 0, 0, 0, True );
        } break;

        default :
        break;
    }
}

/******************************************************************************/
static XtResource widgetRes[] = {
    { XmNfontList, XmCFontList, XmRFontList, sizeof ( XmFontList ),
      XtOffsetOf ( XmTabbedBookRec, tabbedbook.fontlist ),
      XtRImmediate, ( XtPointer ) NULL },
    { XmNtabsPerLine, XmNtabsPerLine, XtRInt, sizeof ( uint32_t ),
      XtOffsetOf ( XmTabbedBookRec, tabbedbook.tabsperline ),
      XtRImmediate, ( XtPointer ) 0x00 } };

/******************************************************************************/
static char defaultTranslations[] = 
"<PtrMoved>: XmTabbedBookMouseEvent()\n\
<BtnDown>:   XmTabbedBookMouseEvent()\n\
<BtnUp>:     XmTabbedBookMouseEvent()\n";

/******************************************************************************/
static XtActionsRec Actions[] = { { "XmTabbedBookMouseEvent",
                                     XmTabbedBookMouseEvent } };

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

    /*** this is required if we want to inherit the event mask. ***/
    /*** Redisplay procedure would not be called otherwise      ***/
    xmManagerClassRec.core_class.realize ( w, value_mask, attributes );

    return ( XtRealizeProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Destroy ( Widget w ) {


    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
static void resizeTabs ( Widget w ) {
    XmTabbedBookWidget tbw = ( XmTabbedBookWidget ) w;
    int nbtab = list_count ( tbw->tabbedbook.ltab );
    LIST *ltmp = tbw->tabbedbook.ltab;
    Dimension width, height;
    uint32_t tabx     = 0x00, taby      = 0x00;
    uint32_t tabwidth = 0x00, tabheight = 0x10;

    XtVaGetValues ( w, XmNwidth, &width, XmNheight, &height, NULL );

    if ( tbw->tabbedbook.tabsperline ) {

    } else {
        if ( nbtab ) {
            tabwidth = width / nbtab;
        }
    }

    while ( ltmp ) {
        XMTAB *xmt = ( XMTAB * ) ltmp->data;

        xmt->x      = tabx;
        xmt->y      = taby;
        xmt->width  = tabwidth - 0x01;
        xmt->height = tabheight;

        tabx   = ( tabx + tabwidth );

        XtConfigureWidget ( xmt->w, 0x00,
                                    xmt->y + xmt->height,
                                    width,
                                    height, 0x00 );

        ltmp = ltmp->next;
    }
}

/******************************************************************************/
static XtWidgetProc Resize ( Widget w ) {
    resizeTabs ( w );

    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
static void drawTab ( XMTAB *tab, Display *dis, Drawable d, GC gc,
                      XmFontList fontlist,
                      Pixel norpix, Pixel armpix,
                      Pixel toppix, Pixel botpix, uint32_t selected ) {

    if ( selected ) {
        XSetForeground ( dis, gc, armpix );
        XFillRectangle ( dis, d, gc, tab->x, tab->y,
                                     tab->width, tab->height + 0x01 );
    } else {
        XSetForeground ( dis, gc, norpix );
        XFillRectangle ( dis, d, gc, tab->x, tab->y,
                                     tab->width, tab->height );
    }

    /*** left line ***/
    XSetForeground ( dis, gc, toppix );
    XDrawLine ( dis, d, gc, tab->x,
                            tab->y,
                            tab->x,
                            tab->y + tab->height - 0x01 );

    /*** top line ***/
    XSetForeground ( dis, gc, toppix );
    XDrawLine ( dis, d, gc, tab->x,
                            tab->y,
                            tab->x + tab->width,
                            tab->y );

    /*** right line ***/
    XSetForeground ( dis, gc, botpix );
    XDrawLine ( dis, d, gc, tab->x + tab->width,
                            tab->y,
                            tab->x + tab->width,
                            tab->y + tab->height - 0x01 );

    XSetForeground ( dis, gc, XBlackPixel ( dis, 0x00 ) );
    XmStringDraw ( dis, d, fontlist, tab->name,
                                     gc,
                                     tab->x,
                                     tab->y,
                                     tab->width,
                                     XmALIGNMENT_CENTER, 0x00, NULL );
}

/******************************************************************************/
static XtExposeProc Redisplay ( Widget w, XEvent *event, Region region ) {
    XmTabbedBookWidget tbw = ( XmTabbedBookWidget ) w;
    Pixel toppix, botpix, foreground, background;
    LIST *ltmp = tbw->tabbedbook.ltab;
    Display *dis = XtDisplay ( w );
    Window win = XtWindow ( w );
    Dimension width, height;
    XmFontList fontlist;
    XGCValues values;
    GC gc;

    gc = XtGetGC ( w, 0x00, &values );

    XtVaGetValues ( w, XmNwidth, &width,
                       XmNheight, &height,
                       XmNfontList, &fontlist,
                       XmNforeground, &foreground,
                       XmNbackground, &background,
                       XmNtopShadowColor, &toppix,
                       XmNbottomShadowColor, &botpix,
                       NULL );

    while ( ltmp ) {
        XMTAB *tab = ( XMTAB * ) ltmp->data;

        if ( tab == tbw->tabbedbook.seltab ) {
            drawTab ( tab, dis, win, gc, fontlist, botpix, background,
                                                   toppix, botpix, 0x01 );
        } else {
            drawTab ( tab, dis, win, gc, fontlist, botpix, background,
                                                   toppix, botpix, 0x00 );
        }

        ltmp = ltmp->next;
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
static XMTAB *xmtab_new ( Widget w, uint32_t x, uint32_t y, uint32_t width,
                                                            uint32_t height ) {
    XMTAB *xmt = ( XMTAB * ) calloc ( 0x01, sizeof ( XMTAB ) );
    Widget parent = XtParent ( w );

    if ( xmt == NULL ) {
        fprintf ( stderr, "xmtab_new: memory allocation failed\n" );

        return NULL;
    }

    xmt->w = w;

    xmt->x = x;
    xmt->y = y;
    xmt->width  = width;
    xmt->height = height;
    xmt->name   = XmStringCreate ( XtName ( w ), XmFONTLIST_DEFAULT_TAG );


    return xmt;
}

/******************************************************************************/
static void insert_child ( Widget w ) {
    XmTabbedBookWidget tbw = ( XmTabbedBookWidget ) XtParent ( w );
    XMTAB *xmt = xmtab_new ( w, 0x00, 0x00, 0x00, 0x00 );
    LIST *ltmp = tbw->tabbedbook.ltab;

    /*** call the parent class insert child_method ***/
    xmManagerClassRec.composite_class.insert_child ( w );

    list_append ( &tbw->tabbedbook.ltab, xmt );

    if ( tbw->tabbedbook.seltab == NULL ) {
        tbw->tabbedbook.seltab = xmt;
    } else {
/*printf("unmanaging %s\n", XtName ( w ) );*/
        XtVaSetValues ( w, XmNmappedWhenManaged, False, NULL );
        /*XtUnmanageChild ( w );*/
    }

    resizeTabs ( ( Widget ) tbw );
}

/******************************************************************************/
XmTabbedBookClassRec xmTabbedBookClassRec = 
                              { { (WidgetClass) &xmManagerClassRec,/*parent*/
                                  "Tabbed Book",                /* class_name */
                                  sizeof ( XmTabbedBookRec ),  /* widget size */
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
                                  /* Composite class part */
                                { XtInheritGeometryManager,
                                  XtInheritChangeManaged,
                                  insert_child,
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
/* extension                    */ NULL },
                                { 0x00 } };

WidgetClass xmTabbedBookWidgetClass = ( WidgetClass ) &xmTabbedBookClassRec;

/******************************************************************************/
Widget XmCreateTabbedBook ( Widget parent, char *name, ArgList  args,
                                                       Cardinal narg ) {
    Widget tbk = XtCreateWidget ( name, xmTabbedBookWidgetClass,
                                  parent, args, narg );

    return tbk;
}

/******************************************************************************/
Widget XmVaCreateTabbedBook ( Widget parent, char *name, ... ) {
    register Widget w;
    va_list var;
    int count;

    va_start ( var,name );
    count = XmeCountVaListSimple ( var );
    va_end ( var );

    va_start ( var, name );
    w = XmeVLCreateWidget ( name, 
                            xmTabbedBookWidgetClass, 
                            parent, False, 
                            var, count );
    va_end( var );

    return w;
}

/******************************************************************************/
Widget XmVaCreateManagedTabbedBook ( Widget parent, char *name, ... ) {
    register Widget w;
    va_list var;
    int count;

    va_start ( var,name );
    count = XmeCountVaListSimple ( var );
    va_end ( var );

    va_start ( var, name );
    w = XmeVLCreateWidget ( name, 
                            xmTabbedBookWidgetClass, 
                            parent, False, 
                            var, count );
    va_end( var );

    XtManageChild ( w );

    return w;
}
