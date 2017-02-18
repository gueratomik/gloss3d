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
static uint32_t drawMeshWeightGroups ( Display *dis, Window win, 
                                                     GC gc,
                                                     XmFontList ftl,
                                                     G3DMESH *mes ) {
    unsigned long whitepixel = XWhitePixel ( dis, 0x00 ),
                  blackpixel = XBlackPixel ( dis, 0x00 );
    uint32_t maxheight = 0x00, maxwidth = 0x00;
    LIST *ltmpgrp = mes->lgrp;
    float y = 0;

    while ( ltmpgrp ) {
        G3DWEIGHTGROUP *grp = ( G3DWEIGHTGROUP * ) ltmpgrp->data;
        XmString str = XmStringCreate ( grp->name, XmFONTLIST_DEFAULT_TAG );
        uint32_t strwidth  = XmStringWidth  ( ftl, str );

        if ( maxwidth < strwidth ) maxwidth = strwidth;

        if ( grp->flags & WEIGHTGROUPSELECTED ) {
            XSetForeground ( dis, gc, 0xFF0000 );
        } else {
            XSetForeground ( dis, gc, blackpixel );
        }

        /*** Draw the weightgroup name ***/
        XmStringDraw ( dis, win, ftl, str, gc, 0, y, strwidth,
                                                     XmALIGNMENT_BEGINNING, 
                                                     0x00,
                                                     NULL );

        y += LISTINDENT;


        ltmpgrp = ltmpgrp->next;
    }

    maxheight = ( y );


    return ( ( maxheight << 0x10 ) | maxwidth );
}

/******************************************************************************/
static void Input ( Widget w, XtPointer client, 
                              XEvent *event,
                              Boolean *continue_to_dispatch ) {
    G3DUI    *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DURMANAGER *urm = gui->urm;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    switch ( event->type ) {
        case KeyPress : {
            XKeyEvent *kev = ( XKeyEvent * ) event;
            KeySym keysym;
            char c;

            if ( XLookupString ( kev, &c, 0x01, &keysym, NULL ) > 0x00 ) {
                switch ( keysym ) {
                    case XK_Delete: {
                        common_g3duiweightgrouplist_deleteSelectedCbk ( gui );
                    } break;
                }
            }
        } break;

        case ButtonPress : {
            XButtonEvent *bev = ( XButtonEvent * ) event;

            if ( obj && ( obj->type == G3DMESHTYPE ) ) {
                G3DMESH *mes= ( G3DMESH * ) obj;
                G3DWEIGHTGROUP *grp = common_g3duiweightgrouplist_getWeightGroup ( mes, bev->x,
                                                                                        bev->y );

                if ( grp ) {
                    common_g3duiweightgrouplist_selectCbk ( gui, grp );
                }

               XClearArea ( XtDisplay ( w ),
                            XtWindow  ( w ), 0x00, 0x00, 0x00, 0x00, True );
            }
        } break;

        case ButtonRelease : {
            XButtonEvent *bev = ( XButtonEvent * ) event;

        } break;

        default:
        break;
    }
}

/******************************************************************************/
static void Redisplay ( Widget w, XtPointer client, XtPointer call ) {
    Display *dis = XtDisplay ( w );
    Window win = XtWindow ( w );
    XGCValues values;
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Dimension winwidth = 0x0, winheight = 0x00, gotwidth, gotheight;
    GC gc = XtGetGC ( w, 0x00, &values );
    uint32_t size;
    unsigned long whitepixel = XWhitePixel ( dis, 0x00 ),
                  blackpixel = XBlackPixel ( dis, 0x00 );
    Dimension width, height;

    XtVaGetValues ( w, XmNwidth, &width, 
                       XmNheight, &height, NULL );



    if ( obj && obj->type == G3DMESHTYPE ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        size = drawMeshWeightGroups ( dis, win, gc, gmt->fontlist, mes );
    }

    if ( size ) {
        XtMakeResizeRequest ( w, size & 0x0000FFFF,
                                 size >> 0x10, 
                                &gotwidth, 
                                &gotheight );

        if ( gotwidth && gotheight ) {
            XtResizeWidget ( w, gotwidth, gotheight, 0x00 );
        }
    }

    XtReleaseGC ( w, gc );
}

/******************************************************************************/
Widget createWeightgroupList ( Widget parent, G3DUI *gui, 
                                              char *name,
                                              Dimension x,
                                              Dimension y,
                                              Dimension width,
                                              Dimension height ) {
    Pixel background, foreground;
    Widget drw;
    Widget scr;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    scr = XmVaCreateScrolledWindow ( parent, name,
                                             XmNx, x,
                                             XmNy, y,
                                             XmNwidth , width,
                                             XmNheight, height,
                                             XmNscrollingPolicy, XmAUTOMATIC,
                                             XmNspacing, 0x00,
                                             XmNforeground, foreground,
                                             XmNbackground, background,
                                             XmNtopShadowColor, foreground,
                                             XmNbottomShadowColor, foreground,
                                             XmNshadowThickness, 0x01,
                                             NULL );

    drw = XmVaCreateDrawingArea ( scr, name,
                                          XtNx, 0,
                                          XtNy, 0,
                                          XtNwidth , width - 0x10,
                                          XtNheight, height - 0x10,
                                          /*** needed for keyboard input ***/
                                          XmNtraversalOn,   TRUE,
                                          /*XmNancestorSensitive, TRUE,
                                          XmNsensitive, TRUE,
                                          XmNkeyboardFocusPolicy, XmEXPLICIT,*/
                                          NULL );

    XtAddCallback ( drw, XmNexposeCallback, Redisplay, gui );

    XtAddEventHandler ( drw, ButtonPressMask   | 
                             ButtonReleaseMask | 
                             KeyPressMask      | 
                             KeyReleaseMask    /*| 
                             PointerMotionMask*/, False, Input, gui );

    XtManageChild ( drw );

    /*** add this widget to the list of ObjectList widget for easy update ***/
    list_insert ( &gui->lweightgroup, drw );

    XtManageChild ( scr );


    return drw;
}



