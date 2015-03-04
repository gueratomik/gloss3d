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
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>
#include <Xm/Manager.h>
#include <Xm/ManagerP.h>
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
#include <r3d.h>
#include <r3dnet.h>
#include <g3durmanager.h>
#include <g3dmouse.h>
#include <g3dui.h>

/******************************************************************************/

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

/******************************************************************************/
#include <X11/X.h>
#include <X11/Intrinsic.h>
#include <X11/IntrinsicP.h>
#include <X11/StringDefs.h>

/******************************************************************************/
#include <Xm/Xm.h>
#include <Xm/Form.h>
#include <Xm/MainW.h>
#include <Xm/RowColumn.h>
#include <Xm/ToggleB.h>
#include <Xm/PushB.h>
#include <Xm/Manager.h>
#include <Xm/ManagerP.h>
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
#include <g3dmouse.h>
#include <g3dui.h>

/******************************************************************************/
static XtProc ClassInitialize ( void );
static XtInitProc Initialize ( Widget, Widget, ArgList, Cardinal * );
static XtRealizeProc Realize ( Widget, XtValueMask *, XSetWindowAttributes * );
static XtWidgetProc Destroy ( Widget );
static XtWidgetProc Resize ( Widget );
static XtExposeProc Redisplay ( Widget, XEvent *, Region );

/******************************************************************************/
static XtResource widgetRes[] = {
    { XmNorientation, XmCOrientation, XtRInt, sizeof ( int ),
      XtOffsetOf ( GTabRec, tab.orientation ),
      XtRImmediate, ( XtPointer ) XmHORIZONTAL },
    { XtNtabType, XtNtabType, XtRInt, sizeof ( int ),
      XtOffsetOf ( GTabRec, tab.tabtype ),
      XtRImmediate, ( XtPointer ) XmSTRING },
    { XtNtabStart, XtNtabStart, XtRInt, sizeof ( int ),
      XtOffsetOf ( GTabRec, tab.tabstart ),
      XtRImmediate, ( XtPointer ) 0x00 },
    { XtNtabWidth, XtNtabWidth, XtRInt, sizeof ( int ),
      XtOffsetOf ( GTabRec, tab.tabwidth ),
      XtRImmediate, ( XtPointer ) 0x00 },
    { XtNtabHeight, XtNtabHeight, XtRInt, sizeof ( int ),
      XtOffsetOf ( GTabRec, tab.tabheight ),
      XtRImmediate, ( XtPointer ) 0x00 },
    { XtNselectedTab, XtNselectedTab, XtRInt, sizeof ( int ),
      XtOffsetOf ( GTabRec, tab.seltab ),
      XtRImmediate, ( XtPointer ) 0x00 } };

/******************************************************************************/
static void GTabMouseEvent ( Widget w, XEvent *event,
                             String *params, Cardinal *num_params ) {
    GTabWidget gtw = ( GTabWidget ) w;

    switch ( event->type ) {
        case ButtonPress : {
            LIST *ltmp = gtw->tab.ltab;
            XButtonEvent *bev = ( XButtonEvent * ) event;
            int i = 0x00;
            GTABITEM *sel = NULL;

            /*** First get the selected tab ***/
            while ( ltmp ) {
                GTABITEM *tab = ( GTABITEM * ) ltmp->data;

                if ( ( bev->x > tab->x ) && bev->x < ( tab->x + tab->width ) &&
                     ( bev->y > tab->y ) && bev->y < ( tab->y + tab->height ) ){

                    gtw->tab.seltab = i;
                    sel = tab;
                }

                ltmp = ltmp->next;
                i++;
            }

            /*** if any, unmap all widgets but the ***/
            /*** one related to the selected tab.  ***/
            if ( sel ) {
                LIST *ltmp = gtw->tab.ltab;

                while ( ltmp ) {
                    GTABITEM *tab = ( GTABITEM * ) ltmp->data;

                    if ( tab != sel ) {
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
static char defaultTranslations[] = 
"<PtrMoved>: GTabMouseEvent()\n\
<BtnDown>:   GTabMouseEvent()\n\
<BtnUp>:     GTabMouseEvent()\n";

/******************************************************************************/
static XtActionsRec Actions[] = { { "GTabMouseEvent", GTabMouseEvent } };

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
    Dimension width, height;
    WidgetList children;
    Cardinal nc;
    int i;

    /*** this is required if we want to inherit the event mask. ***/
    /*** Redisplay procedure would not be called otherwise      ***/
    xmManagerWidgetClass->core_class.realize ( w, value_mask, attributes );

    XtVaGetValues ( w, XmNchildren, &children,
                       XmNnumChildren, &nc,
                       XmNwidth, &width,
                       XmNheight, &height,
                       NULL );

    /*for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        XtConfigureWidget ( child, 0x00, GTABITEMHEIGHT, 
                                   width, height -  GTABITEMHEIGHT, 0x00 );
    }*/

    return ( XtRealizeProc ) NULL;
}

/******************************************************************************/
static void drawItemString ( GTABITEM *tab, Display *dis, Drawable d, GC gc,
                             int selected,
                             XmFontList fontlist,
                             Pixel toppix, Pixel botpix ) {
    XRectangle rec = { .x     = tab->x    , .y      = tab->y,
                       .width = tab->width, .height = tab->height };

    XSetForeground ( dis, gc, XBlackPixel ( dis, 0x00 ) );
    XmStringDraw ( dis, d, fontlist, tab->str,
                                     gc,
                                     tab->x,
                                     tab->y,
                                     tab->width,
                                     XmALIGNMENT_CENTER, 0x00, NULL );
}

/******************************************************************************/
static void drawItemPixmap ( GTABITEM *tab, Display *dis, Drawable d, GC gc,
                             int selected,
                             Pixel toppix, Pixel botpix ) {
    Window win;
    int x, y;
    unsigned int width, height;
    unsigned int border;
    unsigned int depth;
    int difw, difh, posx, posy;

    /*** if tab is clicked, draw the armed pixmap ***/
    if ( selected ) {
        if ( tab->pixarm ) {
            XGetGeometry ( dis, tab->pixarm, &win, &x, &y, &width, &height,
                                             &border, &depth );

            XCopyArea ( dis, tab->pixarm, d, gc, 0x00, 0x00, width, height,
                                                 tab->x, tab->y );
        }
    /*** Otherwise, draw the normal pixmap ***/
    } else {
        if ( tab->pixnor ) {
            XGetGeometry ( dis, tab->pixnor, &win, &x, &y, &width, &height,
                                             &border, &depth );

            XCopyArea ( dis, tab->pixnor, d, gc, 0x00, 0x00, width, height,
                                                 tab->x, tab->y );
        }
    }
}

/******************************************************************************/
static void drawItem ( GTABITEM *tab, Display *dis, Drawable d, GC gc,
                       int type, int selected,
                       XmFontList fontlist,
                       Pixel norpix, Pixel armpix,
                       Pixel toppix, Pixel botpix ) {

    if ( selected ) {
        XSetForeground ( dis, gc, armpix );
        XFillRectangle ( dis, d, gc, tab->x, tab->y,
                                     tab->width, tab->height + 0x01 );
    } else {
        XSetForeground ( dis, gc, norpix );
        XFillRectangle ( dis, d, gc, tab->x, tab->y,
                                     tab->width, tab->height );
    }

    switch ( type ) {
        case XmSTRING : {
            drawItemString ( tab, dis, d, gc, selected, fontlist, toppix, botpix );
        } break;

        case XmPIXMAP : {
            drawItemPixmap ( tab, dis, d, gc, selected, toppix, botpix );
        } break;

        default :
        break;
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
}

/******************************************************************************/
static XtExposeProc Redisplay ( Widget w, XEvent *event, Region region ) {
    GTabWidget gtw = ( GTabWidget ) w;
    Display *dis = XtDisplay ( w );
    Window win = XtWindow ( w );
    LIST *ltmp = gtw->tab.ltab;
    Pixel toppix, botpix, foreground, background;
    XGCValues values;
    Dimension width, height;
    G3DUI *gui;
    GC gc;
    int i = 0x00;

    /*** retrieve the global GUI structure ***/
    XtVaGetValues ( w, XmNuserData, &gui,
                       XmNwidth, &width,
                       XmNheight, &height,
                       XmNforeground, &foreground,
                       XmNbackground, &background,
                       XmNtopShadowColor, &toppix,
                       XmNbottomShadowColor, &botpix,
                       NULL );

    gc = XtGetGC ( w, 0x00, &values );

    XSetForeground ( dis, gc, toppix );
    XDrawLine ( dis, win, gc, 0x00 , gtw->tab.tabheight,
                              width, gtw->tab.tabheight );

    while ( ltmp ) {
        GTABITEM *tab  = ( GTABITEM * ) ltmp->data;

        if ( i == gtw->tab.seltab ) {
            drawItem ( tab, dis, win, gc, gtw->tab.tabtype,
                                          0x01, gui->fontlist,
                                          botpix, background,
                                          toppix, botpix );
        } else {
            drawItem ( tab, dis, win, gc, gtw->tab.tabtype,
                                          0x00, gui->fontlist,
                                          botpix, background,
                                          toppix, botpix );
        }

        ltmp = ltmp->next;
        i++;
    }

    XtReleaseGC ( w, gc );

    return ( XtExposeProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Destroy ( Widget w ) {

    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
/*** ltab   : list of GTABITEM structure                                    ***/
/*** start  : index of first tab                                            ***/
/*** tabwidth : tabitem width                                               ***/
/*** tabheight : tabitem height                                             ***/
/*** orientation : Widget orientation (XmHORIZONTAL or XmVERTICAL)          ***/
/*** width  : widget width                                                  ***/
/*** height : widget height                                                 ***/
/******************************************************************************/
static void resizeTabItems ( LIST *ltab, int start,
                                         int tabwidth, int tabheight,
                                         int orientation,
                                         int width, int height ) {
    int i = 0x00, accu = 0x00;
    LIST *ltmp = ltab;

    while ( ltmp ) {
        GTABITEM *tab = ( GTABITEM * ) ltmp->data;

        if ( i >= start ) {
            if ( orientation == XmHORIZONTAL ) {
                tab->x      = accu;
                tab->y      = 0x00;
                tab->width  = tabwidth;
                tab->height = tabheight;

                accu = ( accu + tabwidth  ) + 0x01;
            } else {
                tab->x      = 0x00;
                tab->y      = accu;
                tab->width  = tabwidth;
                tab->height = tabheight;

                accu = ( accu + tabheight ) + 0x01;
            }
        } else {
            tab->x = tab->y = tab->width = tab->height = 0x00;
        }

        ltmp = ltmp->next;
        i++;
    }
}

/******************************************************************************/
static XtWidgetProc Resize ( Widget w ) {
    GTabWidget gtw = ( GTabWidget ) w;
    LIST *ltmp = gtw->tab.ltab;
    Dimension width, height, x, y;

    /*** XmManager doesnot provide a resize function, so we have to do some ***/
    /*** routines by ourselves.                                             ***/
    if ( XtIsRealized ( w ) ) {
        XClearArea ( XtDisplay ( w ), XtWindow ( w ), 0x00, 0x00,
                                                      0x00, 0x00, True );
    }

    /*** And use the parent class of xmManager for the resizing routines ***/
    if ( xmManagerWidgetClass->core_class.resize ) {
        xmManagerWidgetClass->core_class.resize ( w );
    }

    XtVaGetValues ( w, XmNx, &x,
                       XmNy, &y,
                       XmNwidth , &width,
                       XmNheight, &height, NULL );

    resizeTabItems ( gtw->tab.ltab, gtw->tab.tabstart,
                                    gtw->tab.tabwidth,
                                    gtw->tab.tabheight,
                                    gtw->tab.orientation,
                                    width, height );

    /*while ( ltmp ) {
        GTABITEM *tab = ( GTABITEM * ) ltmp->data;

        ltmp = ltmp->next;
    }*/

    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
GTABITEM *gtabitem_new ( Pixmap pixnor, Pixmap pixarm, Widget w ) {
    GTABITEM *tab = ( GTABITEM * ) calloc ( 0x01, sizeof ( GTABITEM ) );

    if ( tab == NULL ) {
        fprintf ( stderr, "gtabitem_new: memory allocation failed\n" );

        return NULL;
    }

    tab->str    = XmStringCreate ( XtName ( w ), XmFONTLIST_DEFAULT_TAG );
    tab->pixnor = pixnor;
    tab->pixarm = pixarm;
    tab->w      = w;

    return tab;
}

/******************************************************************************/
static GTABITEM *gtabitem_seek ( Widget w ) {
    GTabWidget gtw = ( GTabWidget ) XtParent ( w );
    LIST *ltmp = gtw->tab.ltab;

    while ( ltmp ) {
        GTABITEM *tab = ( GTABITEM * ) ltmp->data;

        if ( tab->w == w ) {
            return tab;
        }

        ltmp = ltmp->next;
    }

    return NULL;
}

/******************************************************************************/
void gtabitem_free ( GTABITEM *tab ) {
    XmStringFree ( tab->str );

    free ( tab );
}

/******************************************************************************/
static void addItem ( Widget tab, Pixmap pixnor, Pixmap pixarm, Widget w ) {
    GTabWidget gtw = ( GTabWidget ) tab;
    GTABITEM *tabitem = gtabitem_new ( pixnor, pixarm, w );

    list_insert ( &gtw->tab.ltab, tabitem );

    switch ( gtw->tab.orientation ) {
        case XmHORIZONTAL : {
            /*** Move the child widget to avoid overlapping our tabitems ***/
            if ( tabitem->w ) {
                XtMoveWidget ( tabitem->w, 0x00, gtw->tab.tabheight + 0x01 );
            }
        } break;

        case XmVERTICAL : {
            /*** Move the child widget to avoid overlapping our tabitems ***/
            if ( tabitem->w ) {
                XtMoveWidget ( tabitem->w, gtw->tab.tabwidth + 0x01, 0x00  );
            }
        } break;

        default :
        break;
    }
}

/******************************************************************************/
static XtSetValuesFunc set_values ( Widget current, Widget request, Widget set,
                                    ArgList args, Cardinal *numargs ) {
    GTabWidget gtw = ( GTabWidget ) set;
    Pixel background, foreground, toppix, botpix;
    Dimension width, height;
    Cardinal i;
    int depth;

    XtVaGetValues ( set, XmNdepth, &depth,
                         XmNwidth, &width,
                         XmNheight, &height,
                         XmNforeground, &foreground,
                         XmNbackground, &background,
                         XmNtopShadowColor, &toppix,
                         XmNbottomShadowColor, &botpix,
                         NULL );

    for ( i = 0x00; i < (*numargs); i++ ) {
        if ( strcmp ( args[i].name, XtNtabitem ) == 0x00 ) {
            GTABITEM *tabitem = ( GTABITEM * ) args[i].value;
            Pixmap pixnor = 0x00, pixarm = 0x00;

            if ( tabitem->xpm ) {
                createTransparentPixmap ( XtDisplay ( set ),
                                          &pixnor, 
                                          &pixarm,
                                          tabitem->xpm, depth,
                                          botpix, background );
            }

printf("%s\n", XtName(tabitem->w));
            XtVaSetValues ( tabitem->w, XmNtopAttachment   , XmATTACH_FORM,
                                        XmNbottomAttachment, XmATTACH_FORM,
                                        XmNleftAttachment  , XmATTACH_FORM,
                                        XmNrightAttachment , XmATTACH_FORM,
                                        NULL );

            addItem ( set, pixnor, pixarm, tabitem->w );
        }
    }

    /*** recompute tabs position ***/
    resizeTabItems ( gtw->tab.ltab, gtw->tab.tabstart,
                                    gtw->tab.tabwidth,
                                    gtw->tab.tabheight,
                                    gtw->tab.orientation,
                                    width, height );

    return 0x00;
}

/******************************************************************************/
GTabClassRec gtabClassRec = { { (WidgetClass) &xmManagerClassRec,/*parent*/
                                  "GTAB",                      /* class_name */
                                  sizeof ( GTabRec ),         /* widget size */
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
                                  NULL },
                                  /* Constraint class part */
                                { NULL,
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

/******************************************************************************/
WidgetClass gtabWidgetClass = ( WidgetClass ) &gtabClassRec;

/******************************************************************************/
Widget createTab ( Widget parent, G3DUI *gui, char *name,
                   Dimension x    , Dimension y,
                   Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget tab;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    tab = XtVaCreateWidget ( name, gtabWidgetClass, parent,
                             XmNx, x,
                             XmNy, y,
                             XmNwidth , width,
                             XmNheight, height,
                             XtNtabWidth, 0x40,
                             XtNtabHeight, 0x10,
                             XtNtabType, XmSTRING,
                             XmNforeground, foreground,
                             XmNbackground, background,
                             XmNorientation, XmHORIZONTAL,
                             XmNuserData, gui,
                             NULL );

    if ( tab == NULL ) {
        fprintf ( stderr, "createTab: Could not create Tab Widget\n" );
    } else {
        XtManageChild ( tab );
    }

    return tab;
}
