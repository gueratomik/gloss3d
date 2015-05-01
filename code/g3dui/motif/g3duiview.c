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
#include <xpm/translate_view.xpm>
#include <xpm/rotate_view.xpm>
#include <xpm/maximize_view.xpm>
#include <xpm/zoom_view.xpm>

/******************************************************************************/
/** A supprimer apres adaptation ***/
Widget createMenuBarButton ( Widget wmenu, G3DUI *gui, char *name,
                             Dimension width,
                             void (*actioncbk) ( Widget, XtPointer, 
                                                         XtPointer ) ) {
    Pixel background, foreground;
    Widget wbtn;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    XtVaGetValues ( wmenu, XmNbackground, &background,
                           XmNforeground, &foreground, NULL );

    /************************************************/
    /*** XmCreateFileSelectionDialog() called by openfilecbk() crashes ***/
    /*** if wopen is a PushButtonGadget. So I replace it by a regular  ***/
    /*** PushButton ***/
    wbtn = XmVaCreatePushButton ( wmenu, name,
                                         XmNwidth, width,
                                         XmNrecomputeSize, False,
                                         XmNfontList, gmt->fontlist,
                                         XmNhighlightThickness, 0x00,
                                         XmNshadowThickness, 0x00,
                                         XmNbackground, background,
                                         XmNforeground, 0x00,
                                         NULL );

    /*** I had some strange behavior using XmNarmCallback.  ***/
    /*** I switched to XmNactivateCallback. It works better ***/
    if ( actioncbk ) XtAddCallback ( wbtn, XmNactivateCallback, actioncbk, gui );

    XtManageChild ( wbtn );


    return wbtn;
}

/******************************************************************************/
Widget g3duiview_getGLArea ( Widget w ) {
    WidgetList children;
    Cardinal nc;
    int i;

    XtVaGetValues ( w, XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( XtClass ( child ) == glwMDrawingAreaWidgetClass ) {
            return child;
        }
    }

    return NULL;
}

/******************************************************************************/
Widget g3duiview_redraw_area ( Widget w ) {
    Widget area = g3duiview_getGLArea ( w );

    XClearArea ( XtDisplay ( area ),
                 XtWindow  ( area ), 0, 0, 0, 0, True );

    return NULL;
}

/******************************************************************************/
static void cancelRender ( Widget w ) {
    GViewWidget gw = ( GViewWidget ) w;

    if ( gw->gview.render_tid ) {
        /*** Here it will happen that the rendering thread has already     ***/
        /*** finished. This should not be a problem because pthread_cancel ***/
        /*** will return an error an should not crash. ***/
        pthread_cancel ( gw->gview.render_tid );

        gw->gview.render_tid = 0x00;
    }
}

/******************************************************************************/
static void GViewMouseEvent ( Widget w, XEvent *event,
                              String *params, Cardinal *num_params ) {
    GViewWidget gw = ( GViewWidget ) w;
    G3DUIVIEW *view = &gw->gview;
    G3DCAMERA *cam = gw->gview.cam;
    static int xold, yold;
    static int xmid, ymid, xori, yori;
    Dimension width, height;
    G3DUI *gui;
    static G3DPIVOT *piv;
    G3DSCENE *sce;

    XtVaGetValues ( w, XmNwidth   , &width, 
                       XmNheight  , &height,
                       XmNuserData, &gui,
                       NULL );

    sce = gui->sce;   

    switch ( event->type ) {
        case ButtonPress : {
            XButtonEvent *bev = ( XButtonEvent * ) event;
            Window win = XtWindow ( w );
            Display *dis = XtDisplay ( w );
            XWindowAttributes attr;
            Widget area = g3duiview_getGLArea ( w );

            G3DUIRENDERPROCESS *rps = common_g3dui_getRenderProcessByID ( gui, ( uint64_t ) area );
            /*** If there was a running render, cancel it and dont go further ***/
            if ( rps ) {
                r3dscene_cancelRender ( rps->rsce );
            }

            gw->gview.buttonID = common_g3duiview_getCurrentButton ( view, bev->x, 
                                                                    bev->y );

            xold = xmid = ( width  >> 0x01 );
            yold = ymid = ( height >> 0x01 );

            xori = bev->x;
            yori = bev->y;

            /*** we exlude the maximize button (ID=0) from grabbing ***/
            if ( view->buttonID > 0 ) {
                switch ( view->buttonID ) {
                    case ROTATEBUTTON : {
                        /*** pivot for rotation ***/
                        piv = g3dpivot_new ( cam, &cam->pivot );

                        g3dobject_addChild ( sce, piv );
                    } break;
                }

	        XGetWindowAttributes(dis, win, &attr );

                XSelectInput ( dis, win, None );

                XWarpPointer ( dis, None, win, 0, 0, 0, 0, xmid, ymid );
                XSync( dis, False );

                XSelectInput ( dis, win, attr.your_event_mask );
            }

            /*** then redraw the widget ***/
            XClearArea ( XtDisplay ( w ), XtWindow ( w ), 0, 0, 0, 0, True );
        } break;

        case MotionNotify : {
            XMotionEvent *mev = ( XMotionEvent * ) event;
            Window win = XtWindow ( w );
            Display *dis = XtDisplay ( w );
            XWindowAttributes attr;

            /*** If pressed ***/
            if ( view->buttonID > -1 ) {
                switch ( view->buttonID ) {
                    case ROTATEBUTTON : {
                        if ( cam && ( mev->state & Button1Mask ) ) {
                            common_g3duiview_orbit ( &gw->gview, piv,
                                                                 mev->x,
                                                                 mev->y, 
                                                                 xold, 
                                                                 yold );
                        }

                        if ( cam && ( mev->state & Button3Mask ) ) {
                            common_g3duiview_spin ( &gw->gview, mev->x, xold );
                        }
                    } break;

                    case ZOOMBUTTON : {
                        if ( cam && ( mev->state & Button1Mask ) ) {
                            common_g3duiview_zoom ( &gw->gview, mev->x, xold );
                        }

                        if ( cam && ( mev->state & Button3Mask ) ) {
                            common_g3duiview_moveForward ( &gw->gview, mev->x, xold );
                        }
                    } break;

                    case TRANSLATEBUTTON : {
                        if ( mev->state & Button1Mask ) {
                            common_g3duiview_moveSideward ( &gw->gview, mev->x,
                                                                 mev->y, 
                                                                 xold, 
                                                                 yold );
                        }

                        if ( mev->state & Button3Mask ) {
                            common_g3duiview_moveForward ( &gw->gview, mev->x, xold );
                        }
                    } break;

                    default :
                    break;
                }

    /*root_window = XRootWindow(dpy, 0);*/
	        XGetWindowAttributes(dis, win, &attr );

                XSelectInput ( dis, win, None );

                XWarpPointer ( dis, None, win, 0, 0, 0, 0, xmid, ymid );
                XSync( dis, False );

                XSelectInput ( dis, win, attr.your_event_mask );
            }

            xold = xmid;
            yold = ymid;

            g3duiview_redraw_area ( w );
        } break;

        case ButtonRelease : {
            /*XButtonEvent *bev = ( XButtonEvent * ) event;*/
            Window win = XtWindow ( w );
            Display *dis = XtDisplay ( w );
            XWindowAttributes attr;

            /*** if we release a button, then set the normal pixmap ***/
            if ( view->buttonID > -1 ) {

                switch ( view->buttonID ) {
                    case MAXIMIZEBUTTON : {
                        Widget parent = XtParent ( w );
                        GQuadWidget gqw = ( GQuadWidget ) parent;

                        if ( parent ) {
                            if ( gqw->maximized_view == NULL ) {
                                gqw->maximized_view = w;
                            } else {
                                gqw->maximized_view = NULL;
                            }

                            ((CoreClassRec*)gquadWidgetClass)->core_class.resize ( gqw );
                        }
                    } break;
                }

	        XGetWindowAttributes(dis, win, &attr );

                XSelectInput ( dis, win, None );

                XWarpPointer ( dis, None, win, 0, 0, 0, 0, xori, yori );
                XSync( dis, False );

                XSelectInput ( dis, win, attr.your_event_mask );

                view->buttonID = -1;
            }

            XClearArea ( XtDisplay ( w ),
                         XtWindow ( w ), 0x00, 0x00, 0x00, 0x00, True );

            if ( piv ) {
                g3dobject_removeChild ( sce, piv );

                g3dobject_free ( piv );

                piv = NULL;
            }
        } break;

        default :
        break;
    }
}

/******************************************************************************/
static XtResource widgetRes[] = {
    { XtNcamera, 
      XtNcamera, XtRPointer, sizeof ( XtPointer ),
      XtOffsetOf ( GViewRec, gview.cam ),
      XtRImmediate, ( XtPointer ) NULL },
    { XtNdefaultCamera, 
      XtNdefaultCamera, XtRPointer, sizeof ( XtPointer ),
      XtOffsetOf ( GViewRec, gview.defcam ),
      XtRImmediate, ( XtPointer ) NULL } };

/******************************************************************************/
static char defaultTranslations[] = 
"<PtrMoved>: GViewMouseEvent()\n\
<BtnDown>:   GViewMouseEvent()\n\
<BtnUp>:     GViewMouseEvent()\n";

/******************************************************************************/
static XtActionsRec Actions[] = { { "GViewMouseEvent", GViewMouseEvent },
                                  { "GViewMouseEvent", GViewMouseEvent },
                                  { "GViewMouseEvent", GViewMouseEvent } };

/******************************************************************************/
/**** To replace with XPM ***/
#ifdef TO_REPLACE
static char BITMAPDATA[0x04][0x20] = 
{ /*** the maximize button ***/
    { 0x00, 0x00, 0xFE, 0x07, 0xEA, 0x07, 0xFE, 0x07,
      0x02, 0x04, 0x02, 0x04, 0x02, 0x04, 0x02, 0x04,
      0x02, 0x7C, 0x02, 0x7C, 0xFE, 0x47, 0x00, 0x41,
      0x00, 0x41, 0x00, 0x41, 0x00, 0x7F, 0x00, 0x00 },
  /*** the rotation button ***/
    { 0x00, 0x00, 0xC0, 0x07, 0x30, 0x78, 0x08, 0x38, 
      0xC4, 0x3B, 0xE4, 0x2B, 0x72, 0x42, 0x32, 0x4C, 
      0x32, 0x4C, 0x72, 0x4E, 0xE4, 0x27, 0xC4, 0x23, 
      0x08, 0x10, 0x30, 0x0C, 0xC0, 0x03, 0x00, 0x00 },
  /*** the move button ***/
    { 0x00, 0x00, 0x80, 0x01, 0xC0, 0x03, 0xE0, 0x07, 
      0x80, 0x01, 0x88, 0x11, 0x8C, 0x31, 0x7E, 0x7E, 
      0x7E, 0x7E, 0x8C, 0x31, 0x88, 0x11, 0x80, 0x01, 
      0xE0, 0x07, 0xC0, 0x03, 0x80, 0x01, 0x00, 0x00 },
  /*** the zoom button ***/
    { 0x00, 0x00, 0x00, 0x06, 0x80, 0x19, 0x40, 0x20,
      0x40, 0x20, 0x20, 0x40, 0x20, 0x40, 0x40, 0x20,
      0x40, 0x20, 0xE0, 0x19, 0x70, 0x06, 0x38, 0x00,
      0x1C, 0x00, 0x0E, 0x00, 0x06, 0x00, 0x00, 0x00 } };
#endif

/******************************************************************************/
static XtProc ClassInitialize ( ) {

    return ( XtProc ) NULL;
}

/******************************************************************************/
/******************************************************************************/
static void showGL ( Widget w, XtPointer client, XtPointer call ) {
    GViewWidget    gw = ( GViewWidget ) XtParent ( w );
    Display      *dis = XtDisplay ( w );
    Window        win = XtWindow  ( w );
    G3DCAMERA    *cam = gw->gview.cam;
    G3DUI        *gui = ( G3DUI * ) client;
    G3DMOUSETOOL *mou;
    G3DSCENE     *sce;
    uint32_t current;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;;
    G3DUIVIEW *view = &gw->gview;

    if ( gui->playLock ) {
         /*** Force disabling real time subdivision ***/
        gui->flags |= ONGOINGANIMATION;
    }

    /*** Retrieve the current 3D Scene ***/
    sce = gui->sce;

    /*** Retrieve the current mouse tool ***/
    mou = gui->mou;

    /*** This helps the drawarea to determine if it should draw mouse tools ***/
    /*** for example (we don't draw mousetool on all window widget. ***/
    current = ( w == gmt->curogl ) ? 0x01 : 0x00;

    /*** Set Context as the current context ***/
    glXMakeCurrent ( dis, win, gw->gview.glctx );

    /*** GUI Toolkit Independent part ***/
    common_g3duiview_showGL ( gui, sce, cam, mou, current, gui->flags | gw->gview.flags );

    GLwDrawingAreaSwapBuffers ( w );

    XSync ( dis, False );

    if ( gui->playLock ) {
        /*** Re-enable real time subdivision ***/
        gui->flags &= (~ONGOINGANIMATION);
    }
}

/*******************************************************************************/
static void sizeGL ( Widget w, XtPointer client, XtPointer call ) {
    GViewWidget gw   = ( GViewWidget ) XtParent ( w );
    Dimension width  = gw->core.width;
    Dimension height = gw->core.height;
    G3DCAMERA *cam = gw->gview.cam;
    Display *dis = XtDisplay ( w );
    Window win   = XtWindow  ( w );

    glXMakeCurrent ( dis, win, gw->gview.glctx );

    glViewport ( 0, 0, width, height - BUTTONSIZE );

    if ( cam ){
        /*** retrieve the viewport matrix, we'll use it for rendering ***/
        glGetIntegerv ( GL_VIEWPORT, cam->vmatrix );

        /*** Really, you don't want a divide by zero ***/
        cam->ratio = ( height ) ? ( double ) width / height : 1.0f;
    }
}

/******************************************************************************/
static void x11event_to_g3devent ( XEvent *x11ev, G3DEvent *g3dev ) {
    memset ( g3dev, 0x00, sizeof ( G3DEvent ) );

    switch ( x11ev->type ) {
        case ButtonPress : {
            XButtonEvent   *x11bev = ( XButtonEvent   * ) x11ev;
            G3DButtonEvent *g3dbev = ( G3DButtonEvent * ) g3dev;

            g3dbev->type = G3DButtonPress;
            g3dbev->x    = x11bev->x;
            g3dbev->y    = x11bev->y;

            g3dbev->button = x11bev->button;

            if ( x11bev->state & ControlMask ) g3dbev->state |= G3DControlMask;
            if ( x11bev->state & ShiftMask   ) g3dbev->state |= G3DShiftMask;
        } break;

        case ButtonRelease : {
            XButtonEvent   *x11bev = ( XButtonEvent   * ) x11ev;
            G3DButtonEvent *g3dbev = ( G3DButtonEvent * ) g3dev;

            g3dbev->type = G3DButtonRelease;
            g3dbev->x    = x11bev->x;
            g3dbev->y    = x11bev->y;

            g3dbev->button = x11bev->button;

            if ( x11bev->state & ControlMask ) g3dbev->state |= G3DControlMask;
            if ( x11bev->state & ShiftMask   ) g3dbev->state |= G3DShiftMask;
        } break;

        case MotionNotify : {
            XMotionEvent   *x11mev = ( XMotionEvent   * ) x11ev;
            G3DMotionEvent *g3dmev = ( G3DMotionEvent * ) g3dev;

            g3dmev->type = G3DMotionNotify;
            g3dmev->x    = x11mev->x;
            g3dmev->y    = x11mev->y;

            if ( x11mev->state & ControlMask ) g3dmev->state |= G3DControlMask;
            if ( x11mev->state & ShiftMask   ) g3dmev->state |= G3DShiftMask;
            if ( x11mev->state & Button1Mask ) g3dmev->state |= G3DButton1Mask;
            if ( x11mev->state & Button2Mask ) g3dmev->state |= G3DButton2Mask;
            if ( x11mev->state & Button3Mask ) g3dmev->state |= G3DButton3Mask;
        } break;
    }
}

/******************************************************************************/
static void inputGL ( Widget wid, XtPointer client, XtPointer call ) {
    GLwDrawingAreaCallbackStruct *das = ( GLwDrawingAreaCallbackStruct * ) call;
    XEvent *event = das->event;
    GViewWidget gw = ( GViewWidget ) XtParent ( wid );
    Display *dis = XtDisplay ( wid );
    Window win   = XtWindow  ( wid );
    G3DUICLIPBOARD *cli;
    G3DURMANAGER *urm;
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce;
    static uint32_t shiftKey;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    /*** Set the current OGL Widget only if a mouse click happened ***/
    /*if ( event->type == ButtonPress ) {*/
    gmt->curogl = wid;

    gui->curcam = gw->gview.cam;
/*printf("RENDERTID:%d\n",gw->gview.render_tid );*/
    cancelRender ( ( Widget ) gw );

    urm = gui->urm;
    cli = gui->cli;
    sce = gui->sce;

    /*** needed for keyboard input ***/
    XmProcessTraversal ( wid, XmTRAVERSE_CURRENT ); 

    glXMakeCurrent ( dis, win, gw->gview.glctx );

    switch ( das->event->type ) {
        case ButtonPress : {
            G3DUIRENDERPROCESS *rps = common_g3dui_getRenderProcessByID ( gui, ( uint64_t ) wid );
            /*** If there was a running render, cancel it and dont go further ***/
            if ( rps ) {
                r3dscene_cancelRender ( rps->rsce );

                return;
            }
        } break;

        case KeyPress : {
            XKeyEvent *kev = ( XKeyEvent * ) das->event;
            KeySym keysym;
            char c;

            if ( XLookupString ( kev, &c, 0x01, &keysym, NULL ) > 0x00 ) {
                switch ( keysym ) {
                    case XK_Delete: {
                        g3dui_setHourGlass ( gui );

                        /*** gw has XmNuserData set ***/
                        common_g3dui_deleteSelectionCbk ( gui );

                        g3dui_unsetHourGlass ( gui );
                    } break;

        /*** For an unknown reason, KeyPress state shiftMask does not work ***/
                    case XK_z: {
                        /*** undo ***/
                        if ( kev->state & ControlMask ) {
                            common_g3dui_undoCbk ( gui );
                        }
                    } break;

                    case XK_y: {
                        /*** redo ***/
                        if ( kev->state & ControlMask ) {
                            common_g3dui_redoCbk ( gui );
                        }
                    } break;

                    case XK_s:
                        if ( kev->state & ControlMask ) {
                            if ( gui->filename ) {
                                printf ( "saving as:%s\n", gui->filename );

                                common_g3dui_saveG3DFile ( gui );
                            } else {
                                g3dui_saveAsCbk ( wid, gui, call );
                            }
                        }
                    break;

                    /*** This piece of code is also in g3duilist ****/
                    /*** Ctrl+c is copy object ***/
                    case XK_c :
                        if ( kev->state & ControlMask ) {
printf("copying %d object(s)\n", list_count ( sce->lsel ) );
                           g3dui_setHourGlass ( gui );
                           /* a decommenter apres adaptation ***/
                           /*g3duiclipboard_copyObject ( cli, sce, sce->lsel, 0x01, gui->flags );*/
                           g3dui_unsetHourGlass ( gui );
                        }
                    break;

                    /*** Ctrl+v is paste object ***/
                    case XK_v :
                        if ( kev->state & ControlMask ) {
                           G3DOBJECT *src = g3dscene_getSelectedObject ( sce );
                           G3DOBJECT *dst = ( G3DOBJECT * ) sce;

                           /*** add the copied object to the   ***/
                           /*** selected object's parent objet ***/
                           if ( src ) dst = src->parent;
printf("pasting\n");
                           g3dui_setHourGlass ( gui );
                           /* a decommenter apres adaptation ***/
                           /*g3duiclipboard_paste ( cli, urm, sce, dst, gui->flags );*/
                           g3dui_unsetHourGlass ( gui );
                        }

                        g3dui_redrawObjectList     ( gui );
                        g3dui_updateAllCurrentEdit ( gui );
                        g3dui_redrawGLViews        ( gui );
                    break;
                    /*************************************************/

                    case XK_a: {
                        common_g3dui_selectAllCbk ( gui );
                    } break;
 
                    default :
                    break;
                }
            }
        } break;
    }

    /*}*/

    if ( gui->mou ) {
        /*** Call the mousetool callback and redraw if return value is > 0 ***/
        /*** Call the mousetool callback and redraw if return value is > 0 ***/
        G3DEvent g3dev;

        x11event_to_g3devent ( event, &g3dev );

        if ( gui->mou->tool ) {
            uint32_t msk = gui->mou->tool ( gui->mou, gui->sce,
                                            gui->curcam, gui->urm,
                                            gui->flags, &g3dev );
            /*** The curent tool must be allowed to ***/
            /*** run in the current drawing mode.   ***/
        /*if ( ( gui->mou->flags & MODEMASK ) == ( gui->flags & MODEMASK ) ) {*/

                if ( msk & REDRAWVIEW ) {
                    g3dui_redrawGLViews ( gui );
                }

                if ( msk & REDRAWLIST ) {
                    g3dui_redrawObjectList ( gui );
                }

                if ( msk & REDRAWCOORDS ) {
                    g3dui_updateCoords ( gui );
                }

                if ( msk & REDRAWCURRENTOBJECT ) {
                    g3dui_updateAllCurrentEdit ( gui );
                }

                if ( msk & NOBUFFEREDSUBDIVISION ) {
                    /*** this should be replace by some MEANINGFUL mask ***/
                    gui->flags |= ONGOINGANIMATION;
                }

                if ( msk & BUFFEREDSUBDIVISIONOK ) {
                    /*** this should be replace by some MEANINGFUL mask ***/
                    gui->flags &= (~ONGOINGANIMATION);
                }

                if ( event->type == ButtonRelease ) {
                    g3dcursor_reset ( &gui->sce->csr );
                }
        }
    }
}

/******************************************************************************/
static void initGL ( Widget w, XtPointer client, XtPointer call ) {
    GViewWidget gw = ( GViewWidget ) XtParent ( w );
    XVisualInfo *vi;
    /*** By default, GLwDrawingArea does not handle mouse motion event      ***/
    /*** We so have to change the behavior by using a different translation ***/
    /*** table. It (should) work(s) like a charm.                           ***/
    static String actions = "<KeyDown>:      glwInput() \n\
                             <KeyUp>:        glwInput() \n\
                             <BtnDown>:      glwInput() \n\
                             <BtnUp>:        glwInput() \n\
                             <BtnMotion>:    glwInput() \n\
                             <PtrMoved>:     glwInput()";
    static GLXContext shared = NULL;
    Display *dis = XtDisplay ( w );
    Window win   = XtWindow  ( w );
    G3DUIVIEW *view = &gw->gview;

    XtTranslations trans = XtParseTranslationTable ( actions );

    /*** Set the new translation table ***/
    XtOverrideTranslations ( w, trans );

    XtVaGetValues ( w, GLwNvisualInfo, &vi, NULL );

    /*** Create OpenGL Context ***/
    view->glctx = glXCreateContext( dis, vi, shared, True );

    if ( shared == NULL ) shared = view->glctx;

    /*** Set Context as the current context ***/
    glXMakeCurrent ( dis, win, view->glctx );

    common_g3duiview_initGL ( view );

    /*** Free the memory assigned for GLwNvisualInfo ***/
    free ( vi );
}

/******************************************************************************/
void addGLMenuButton ( Widget menu, G3DUI *gui, G3DMOUSETOOL *mou ) {
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    Pixel background, foreground;
    Widget btn;

    XtVaGetValues ( menu, XmNbackground, &background,
                          XmNforeground, &foreground, NULL );

    btn = XmVaCreatePushButton ( menu, mou->name,
                                 /*** arguments ***/
                                 XmNx, 0x00,
                                 XmNy, 0x00,
                                 XmNfontList, gmt->fontlist,
                                 XmNhighlightThickness, 0x01,
                                 XmNshadowThickness, 0x00,
                                 XmNmarginHeight, 0x04,
                                 XmNmarginWidth, 0x04,
                                 XmNbackground, background,
                                 XmNforeground, foreground,
                                 NULL );

    XtAddCallback ( btn, XmNactivateCallback, g3dui_setMouseTool, gui );

    XtManageChild ( btn );
}

/******************************************************************************/
void addMenuListButton ( G3DUI *gui, LIST *lmenu, G3DMOUSETOOL *mou ) {
    LIST *ltmpmenu = lmenu;

    while ( ltmpmenu ) {
        Widget menu = ( Widget ) ltmpmenu->data;

        addGLMenuButton ( menu, gui, mou );

        ltmpmenu = ltmpmenu->next;
    }
}

/******************************************************************************/
Widget createViewMenu ( Widget w, G3DUI *gui, char *menuname ) {
    Pixel background, foreground;
    Widget menu;

    XtVaGetValues ( w, XmNbackground, &background,
                       XmNforeground, &foreground, NULL );

    menu = XmVaCreateSimplePopupMenu ( w, menuname, NULL,
                                       XmNbackground, background,
                                       XmNshadowThickness, 0x01,
                                       /***************************************/
                                       XmNspacing, 0x00,
                                       XmNmarginWidth, 0x00,
                                       XmNmarginHeight, 0x00,
                                       /*XmNmenuPost, "<Btn3Down>",*/
                                       XmNpopupEnabled, XmPOPUP_DISABLED,
                                       XmNhighlightThickness, 0x00,
                                       XmNbackground, background,
                                       XmNforeground, foreground,
                                       XmNmappedWhenManaged, False,
                                       NULL );


    return menu;
}

/******************************************************************************/
Widget getMenu ( LIST *lmenu, Widget parent ) {
    LIST *ltmpmenu = lmenu;

    while ( ltmpmenu ) {
        Widget menu  = ( Widget ) ltmpmenu->data;
        Widget shell = XtParent ( menu );

        if ( XtParent ( shell ) == parent ) return menu;

        ltmpmenu = ltmpmenu->next;
    }

    return NULL;
}

/******************************************************************************/
Widget getViewMenu ( Widget w, G3DUI *gui ) {
    if ( gui->flags & VIEWOBJECT ) return getMenu ( gui->lobjmenu, w );
    if ( gui->flags & VIEWFACE   ) return getMenu ( gui->lfacmenu, w );
    if ( gui->flags & VIEWEDGE   ) return getMenu ( gui->ledgmenu, w );
    if ( gui->flags & VIEWVERTEX ) return getMenu ( gui->lvermenu, w );

    return NULL;
}

/******************************************************************************/
static void PostMenu ( Widget w, XtPointer client, XEvent *event, Boolean *c ) {
    XButtonEvent *ev = ( XButtonEvent * ) event;
    G3DUI *gui = ( G3DUI * ) client;
    Widget parent = XtParent ( w );
    Widget menu = getViewMenu ( parent, gui );

    if ( menu == NULL ) { return; }

    if ( ev->button != Button3 ) return;

    /*** When buttonPress event occurs, we dont want the mouse event to be ***/
    /*** Dispatched to other widget, so we cancel the event-dispatching ***/
    if ( ev->type == ButtonPress ) {
        (*c) = False;

         return;
    }

    if ( menu ) {
        XmMenuPosition ( ( Widget ) menu, ev );
        XtManageChild  ( ( Widget ) menu );

        /*** Stop treating events and callbacks ***/
        (*c) = False;
    }
}

/******************************************************************************/
static void createObjectMenu ( Widget w, G3DUI *gui ) {
    Widget menu = createViewMenu ( w, gui, OBJECTMENUNAME );

    list_insert ( &gui->lobjmenu, menu );
}

/******************************************************************************/
static void createVertexMenu ( Widget w, G3DUI *gui ) {
    Widget menu = createViewMenu ( w, gui, VERTEXMENUNAME );

    list_insert ( &gui->lvermenu, menu );
}

/******************************************************************************/
static void createEdgeMenu ( Widget w, G3DUI *gui ) {
    Widget menu = createViewMenu ( w, gui, EDGEMENUNAME );

    list_insert ( &gui->ledgmenu, menu );
}


/******************************************************************************/
static void createFaceMenu ( Widget w, G3DUI *gui ) {
    Widget menu = createViewMenu ( w, gui, FACEMENUNAME );

    list_insert ( &gui->lfacmenu, menu );
}

/******************************************************************************/
extern WidgetClass glwMDrawingAreaWidgetClass;

/******************************************************************************/
Widget createGLView ( Widget wid, G3DUI *gui, Dimension x,
                                              Dimension y,
                                              Dimension width,
                                              Dimension height ) {
    Display *dpy = XtDisplay ( wid );
    XWindowAttributes attr;
    GViewWidget gw = ( GViewWidget ) wid;
    Widget ogl;
 
    XGetWindowAttributes ( dpy, XDefaultRootWindow ( dpy ), &attr );

    switch ( attr.depth ) {
        case 0x10 : /*** 16 Bpp ***/
            ogl = XtVaCreateWidget ( "GLVIEW", glwMDrawingAreaWidgetClass, wid,
                                     GLwNrgba, TRUE,
                                     GLwNdoublebuffer, TRUE,
                                     GLwNredSize,      0x05,
                                     GLwNblueSize,     0x05,
                                     GLwNgreenSize,    0x06,
                                     GLwNalphaSize,    0x00,
                                     GLwNdepthSize,    0x10,
/*** needed for keyboard input ***/  XmNtraversalOn,   TRUE,
                                     XmNx,             x,
                                     XmNy,             y,
                                     XmNwidth,         width,
                                     XmNheight,        height,
                                     NULL );
        break;

        case 0x18 : /*** 24 Bpp ***/
            ogl = XtVaCreateWidget ( "GLVIEW", glwMDrawingAreaWidgetClass, wid,
                                     GLwNrgba, TRUE,
                                     GLwNdoublebuffer, TRUE,
                                     GLwNredSize,      0x08,
                                     GLwNblueSize,     0x08,
                                     GLwNgreenSize,    0x08,
                                     GLwNalphaSize,    0x00,
                                     GLwNdepthSize,    0x18,
/*** needed for keyboard input ***/  XmNtraversalOn,   TRUE,
                                     XmNx,             x,
                                     XmNy,             y,
                                     XmNwidth,         width,
                                     XmNheight,        height,
                                     NULL );
        break;

        case 0x20 : /*** 32 Bpp ***/
            ogl = XtVaCreateWidget ( "GLVIEW", glwMDrawingAreaWidgetClass, wid,
                                     GLwNrgba, TRUE,
                                     GLwNdoublebuffer, TRUE,
                                     GLwNredSize,      0x08,
                                     GLwNblueSize,     0x08,
                                     GLwNgreenSize,    0x08,
                                     GLwNalphaSize,    0x08,
                                     GLwNdepthSize,    0x20,
/*** needed for keyboard input ***/  XmNtraversalOn,   TRUE,
                                     XmNx,             x,
                                     XmNy,             y,
                                     XmNwidth,         width,
                                     XmNheight,        height,
                                     NULL );
        break;

        default :
            fprintf ( stderr, "Unsupported depth : %d bpp\n", attr.depth );
        break;
    }

    if ( ogl == NULL ) {
        fprintf ( stderr, "createGLView: Could not create Widget\n" );

        /*** needs some exit function ***/
    } else {
        XtAddCallback ( ogl, GLwNginitCallback , initGL , gw->gview.cam );
        XtAddCallback ( ogl, GLwNresizeCallback, sizeGL , gui );
        XtAddCallback ( ogl, GLwNexposeCallback, showGL , gui );
        XtAddCallback ( ogl, GLwNinputCallback , inputGL, gui );

        XtManageChild ( ogl );

        createObjectMenu ( wid, gui );
        createVertexMenu ( wid, gui );
        createEdgeMenu   ( wid, gui );
        createFaceMenu   ( wid, gui );

        XtAddEventHandler ( ogl, ButtonReleaseMask | 
                                 ButtonPressMask, False, PostMenu, gui );
    }

    return ogl;
}

/******************************************************************************/
static void createPixmaps ( Widget wid ) {
    GViewWidget w = ( GViewWidget ) wid;
    Pixel foreground = w->manager.highlight_color,
          background = w->core.background_pixel,
          armedcolor = w->manager.foreground;
    int depth        = w->core.depth;
    Display *dis = XtDisplay ( w );
    int i;

    createTransparentPixmap ( dis, &w->icon_nor[MAXIMIZEBUTTON],
                                   &w->icon_arm[MAXIMIZEBUTTON],
                                   maximize_view_xpm, depth,
                                   background, foreground );

    createTransparentPixmap ( dis, &w->icon_nor[ROTATEBUTTON],
                                   &w->icon_arm[ROTATEBUTTON],
                                   rotate_view_xpm, depth,
                                   background, foreground );

    createTransparentPixmap ( dis, &w->icon_nor[TRANSLATEBUTTON],
                                   &w->icon_arm[TRANSLATEBUTTON],
                                   translate_view_xpm, depth,
                                   background, foreground );

    createTransparentPixmap ( dis, &w->icon_nor[ZOOMBUTTON],
                                   &w->icon_arm[ZOOMBUTTON],
                                   zoom_view_xpm, depth,
                                   background, foreground );
}

/******************************************************************************/
static XtInitProc Initialize ( Widget request, Widget init,
                               ArgList args, Cardinal *num ) {
    GViewWidget gw = ( GViewWidget ) init;
    Dimension width, height;
    G3DUIVIEW *view = &gw->gview;

    XtVaGetValues ( init, XmNwidth, &width, XmNheight, &height, NULL );

    /*** Compute the rectangle position for buttons ***/
    /*** This is must set in the initalize function because no resizing ***/
    /*** callbacks will be called when our program is mapped ***/
    common_g3duiview_init ( view, width, height );


    return ( XtInitProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Destroy ( Widget w ) {

    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
static XtExposeProc Redisplay ( Widget wid, XEvent *event, Region region ) {
    GViewWidget gw = ( GViewWidget ) wid;
    G3DUIVIEW *view = &gw->gview;
    Display *dis = XtDisplay ( wid );
    Window win = XtWindow ( wid );
    XGCValues values;
    GC gc;
    int i, xpos;

    gc = XtGetGC ( wid, 0x00, &values );

    for ( i = 0x00, xpos = 0x00; i < NBVIEWBUTTON; i++, ( xpos += (BUTTONSIZE+2) ) ) {
        Pixmap icon_cur;

        if ( i == view->buttonID ) {
            icon_cur = gw->icon_arm[i];
        } else {
            icon_cur = gw->icon_nor[i];
        }

        XCopyArea ( dis,
                    icon_cur,
                    win,
                    gc,
                    0x00,
                    0x00,
                    BUTTONSIZE,
                    BUTTONSIZE,
                    view->rec[i].x,
                    view->rec[i].y );
    }

    XtReleaseGC ( wid, gc );

    return ( XtExposeProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Resize ( Widget w ) {
    GViewWidget gw = ( GViewWidget ) w;
    G3DUIVIEW *view = &gw->gview;
    Widget ogl = g3duiview_getGLArea ( w );
    Dimension glwidth  =   gw->core.width,
              glheight = ( gw->core.height );
    WidgetList children;
    Cardinal nc;
    int i;

    XtVaGetValues ( w, XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );
    /*WidgetList children;
    Cardinal nc;*/

    /*** And use the parent class of xmManager for the resizing routines ***/
    if ( xmManagerClassRec.core_class.resize ) {
        xmManagerClassRec.core_class.resize ( w );
    }

    /*** Cancel ongoing rendering if we resize the window ***/
    cancelRender ( w );

    /*** XmManager doesnot provide a resize function, so we have to do some ***/
    /*** routines by ourselves.                                             ***/
    if ( XtIsRealized ( w ) ) {
        XClearArea ( XtDisplay ( w ), XtWindow ( w ), 0x00, 0x00,
                                                      0x00, 0x00, True );
    }

    /*** Compute the rectangle position for buttons ***/
    common_g3duiview_resize ( view, glwidth, glheight );

    if ( XtIsRealized ( w ) ) {
        if ( ogl && XtIsRealized ( ogl ) && ( glwidth > 0x00  ) &&
                                            ( glheight > 0x00 ) ) {
            XtConfigureWidget ( ogl, view->glrec.x    , view->glrec.y     , 
                                     view->glrec.width, view->glrec.height, 
                                     0x00 );
        }
    }


    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
static void displayModeCbk ( Widget w, XtPointer client, XtPointer call ) {
    XmComboBoxCallbackStruct *cbs = ( XmComboBoxCallbackStruct * ) call;
    GViewWidget gw = ( GViewWidget ) XtParent ( w );
    G3DUI *gui;
    char *str;
    Widget ogl = g3duiview_getGLArea ( gw );

    if ( ogl ) {
        glXMakeCurrent ( XtDisplay ( ogl ),
                         XtWindow  ( ogl ), gw->gview.glctx );

        XmStringGetLtoR ( cbs->item_or_text, XmFONTLIST_DEFAULT_TAG, &str );

        XtVaGetValues ( w, XmNuserData, &gui, NULL );

        if ( strcmp ( str, "Gouraud Shading" ) == 0x00 ) {
            glEnable      ( GL_LIGHTING );
            glShadeModel  ( GL_SMOOTH );
            glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );

            /*** The no texture flags is VIEW dependent, not GUI dependent ***/
            gw->gview.flags &= (~NOTEXTURE);

            gw->gview.mode = GLVIEWGOURAUD;
        }

        if ( strcmp ( str, "Flat Shading" ) == 0x00 ) {
            glEnable      ( GL_LIGHTING );
            glShadeModel  ( GL_FLAT );
            glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );

            /*** The no texture flags is VIEW dependent, not GUI dependent ***/
            gw->gview.flags &= (~NOTEXTURE);

            gw->gview.mode = GLVIEWFLAT;
        }

        if ( strcmp ( str, "Wireframe" ) == 0x00 ) {
            glDisable     ( GL_LIGHTING );
            glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );

            g3dui_disableTextureImages ( gui );

            /*** The no texture flags is VIEW dependent, not GUI dependent ***/
            gw->gview.flags |= NOTEXTURE;

            gw->gview.mode = GLVIEWWIREFRAME;
        }

        XtFree ( str );
    }
}

/******************************************************************************/
void updateGLView ( Widget w ) {
    GViewWidget gw = ( GViewWidget ) w;
    WidgetList children;
    Cardinal nc;
    G3DUI *gui;
    int i;

    XtVaGetValues ( w, XmNuserData, &gui,
                       XmNchildren, &children,
                       XmNnumChildren, &nc, NULL );

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];
        char *name = XtName ( child );

        if ( XtClass ( child ) == xmComboBoxWidgetClass ) {
            if ( strcmp ( name, EDITGLVIEWMODE ) == 0x00 ) {
                /*** gw->gview.mode: 0 = gouraud, 1 = flat, 2 = wireframe ***/
                XtVaSetValues ( child, XmNselectedPosition,
                                       gw->gview.mode, NULL );
            }
        }
    }
}

/******************************************************************************/
static void usecameracbk ( Widget w, XtPointer client, XtPointer call ) {
    GViewWidget gw = ( GViewWidget ) XtParent ( XtParent ( XtParent ( XtParent ( w ) ) ) );
    G3DUI *gui = ( G3DUI * ) client;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( obj && obj->type == G3DCAMERATYPE ) {
        G3DCAMERA *cam = ( G3DCAMERA * ) obj;

        cam->ratio = gw->gview.cam->ratio;

        gw->gview.cam = cam;
    }
}

/******************************************************************************/
static void usedefcamcbk ( Widget w, XtPointer client, XtPointer call ) {
    GViewWidget gw = ( GViewWidget ) XtParent ( XtParent ( XtParent ( XtParent ( w ) ) ) );
    G3DUI *gui = ( G3DUI * ) client;

    gw->gview.defcam->ratio = gw->gview.cam->ratio;

    gw->gview.cam = gw->gview.defcam;
}

/******************************************************************************/
static void normalscbk ( Widget w, XtPointer client, XtPointer call ) {
    GViewWidget gw = ( GViewWidget ) XtParent ( XtParent ( XtParent ( XtParent ( w ) ) ) );
    G3DUI *gui = ( G3DUI * ) client;
    XmString str;

    if ( gw->gview.flags & VIEWNORMALS ) {
        gw->gview.flags &= (~VIEWNORMALS);

        /*XmStringCreate ( "Show normals", XmFONTLIST_DEFAULT_TAG );*/
    } else {
        gw->gview.flags |= VIEWNORMALS;

        /*XmStringCreate ( "Hide normals", XmFONTLIST_DEFAULT_TAG );*/
    }

    /*XtVaSetValues ( w, XmNlabelString, str, NULL );

    XmStringFree ( str );*/
}

/******************************************************************************/
static void bonescbk ( Widget w, XtPointer client, XtPointer call ) {
    GViewWidget gw = ( GViewWidget ) XtParent ( XtParent ( XtParent ( XtParent ( w ) ) ) );
    G3DUI *gui = ( G3DUI * ) client;
    XmString str;

    if ( gw->gview.flags & HIDEBONES ) {
        gw->gview.flags &= (~HIDEBONES);
    } else {
        gw->gview.flags |= HIDEBONES;
    }
}

/******************************************************************************/
static void gridcbk ( Widget w, XtPointer client, XtPointer call ) {
    GViewWidget gw = ( GViewWidget ) XtParent ( XtParent ( XtParent ( XtParent ( w ) ) ) );
    G3DUI *gui = ( G3DUI * ) client;
    XmString str;

    if ( gw->gview.flags & HIDEGRID ) {
        gw->gview.flags &= (~HIDEGRID);
    } else {
        gw->gview.flags |= HIDEGRID;
    }
}

/******************************************************************************/
static void lightingcbk ( Widget w, XtPointer client, XtPointer call ) {
    GViewWidget gw = ( GViewWidget ) XtParent ( XtParent ( XtParent ( XtParent ( w ) ) ) );
    G3DUI *gui = ( G3DUI * ) client;
    XmString str;

    if ( gw->gview.flags & NOLIGHTING ) {
        gw->gview.flags &= (~NOLIGHTING);

        g3dscene_turnLightsOn  ( gui->sce );
    } else {
        gw->gview.flags |= NOLIGHTING;

        g3dscene_turnLightsOff ( gui->sce );
    }
}

/******************************************************************************/
static void stringToVector ( char *str, G3DVECTOR *vec ) {
    float valx, valy, valz, valf;

    sscanf ( str, "%f %f %f", &valx, &valy, &valz );

    vec->x = valx;
    vec->y = valy;
    vec->z = valz;
}

/******************************************************************************/
static void resetcamcbk ( Widget w, XtPointer client, XtPointer call ) {
    GViewWidget gw = ( GViewWidget ) XtParent ( XtParent ( XtParent ( XtParent ( w ) ) ) );
    G3DCAMERA *cam = gw->gview.cam;
    G3DOBJECT *objcam = ( G3DOBJECT * ) cam;
    G3DUI *gui = ( G3DUI * ) client;
    XmString str;

    /*** Normally, a GLView should have a camera, but we check the value ***/
    if ( objcam ) {
        memcpy ( &objcam->pos, &gw->gview.defcampos, sizeof ( G3DVECTOR ) );
        memcpy ( &objcam->rot, &gw->gview.defcamrot, sizeof ( G3DVECTOR ) );
        memcpy ( &objcam->sca, &gw->gview.defcamsca, sizeof ( G3DVECTOR ) );

        gw->gview.cam->focal = gw->gview.defcamfoc;


        g3dobject_updateMatrix_r ( objcam, gui->flags );
    }
}

/******************************************************************************/
static void createGLViewMenuBar ( Widget w ) {
    Widget parent = XtParent ( w );
    Pixel background, foreground;
    Widget wopts, copts;
    Widget menubar;
    G3DUI *gui;
    G3DUIMOTIF *gmt;

    XtVaGetValues ( parent, XmNuserData, &gui,
                            XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    menubar = XmVaCreateSimpleMenuBar ( w, MENUBARNAME,
                                        XmNfontList, gmt->fontlist,
                                        XmNx, 0x00,
                                        XmNy, 0x00,
                                        XmNheight, BUTTONSIZE,
                                        XmNwidth,  0x60,
                                        XmNrecomputeSize, False,
                                        XmNmarginHeight, 0x00,
                                        XmNmarginWidth, 0x00,
                                        XmNhighlightThickness, 0x00,
                                        XmNshadowThickness, 0x00,
                                        XmNbackground, background,
                                        XmNforeground, foreground,
                                        NULL );

    wopts = XmCreatePulldownMenu ( menubar, "FilePullDown", NULL, 0x00 );

    XtVaSetValues ( wopts, XmNhighlightThickness, 0x00,
                           XmNuserData, gui,
                           XmNshadowThickness, 0x01, NULL );

    copts = XmVaCreateCascadeButton ( menubar, "Options",
                                      XmNwidth, 0x60,
                                      XmNheight, BUTTONSIZE,
                                      XmNrecomputeSize, False,
                                      XmNfontList, gmt->fontlist,
                                      XmNhighlightThickness, 0x00,
                                      XmNshadowThickness, 0x01,
                                      XmNbackground, background,
                                      XmNforeground, 0x00,
                                      XmNsubMenuId, wopts,
                                      NULL );

    createMenuBarButton ( wopts, gui, "Use default camera"  , 0x80, usedefcamcbk );
    createMenuBarButton ( wopts, gui, "Use selected camera" , 0x80, usecameracbk );
    createMenuBarButton ( wopts, gui, "Show / Hide normals" , 0x80, normalscbk   );
    createMenuBarButton ( wopts, gui, "Show / Hide bones"   , 0x80, bonescbk     );
    createMenuBarButton ( wopts, gui, "Show / Hide grid"    , 0x80, gridcbk      );
    createMenuBarButton ( wopts, gui, "Turn on/off lighting", 0x80, lightingcbk  );
    createMenuBarButton ( wopts, gui, "Reset view"          , 0x80, resetcamcbk  );

    XtManageChild ( copts );

    XtManageChild ( menubar );
}

/******************************************************************************/
static XtRealizeProc Realize ( Widget w, XtValueMask *value_mask,
                               XSetWindowAttributes *attributes ) {
    GViewWidget gw = ( GViewWidget ) w;
    G3DUIVIEW *view = &gw->gview;
    XColor foreground = { .pixel = 0x00 };
    XColor background = { .pixel = 0x00 };
    Widget parent = XtParent ( w );
    XmString *dmlist = ( XmString * ) XtMalloc ( sizeof ( XmString ) * 0x03 );
    Arg cmbargv[] = { XmNx                 , ( XtArgVal ) 0x60,
                      XmNy                 , ( XtArgVal ) 0x00,
                      XmNwidth             , ( XtArgVal ) 0x80,
                      XmNmarginHeight      , ( XtArgVal ) 0x00,
                      XmNmarginWidth       , ( XtArgVal ) 0x02,
                      XmNhighlightThickness, ( XtArgVal ) 0x00,
                      XmNshadowThickness   , ( XtArgVal ) 0x01,
                      XmNtraversalOn       , ( XtArgVal ) False,
                      XmNborderWidth       , ( XtArgVal ) 0x00,
                      XmNarrowSize         , ( XtArgVal ) 0x0A,
                      XmNitemCount         , ( XtArgVal ) 0x03,
                      XmNitems             , ( XtArgVal ) dmlist,
                      XmNvisibleItemCount  , ( XtArgVal ) 0x03 };
    Cardinal cmbargc = 0x0D;
    XmString str;
    Widget ddw, area;
    G3DUI *gui;


    /*** this is required if we want to inherit the event mask. ***/
    /*** Redisplay procedure would not be called otherwise      ***/
    xmManagerClassRec.core_class.realize ( w, value_mask, attributes );

    /*** Get parent color ***/
    if ( parent ) {
        XtVaGetValues ( parent, XtNbackground, &background.pixel, 
                                XtNforeground, &foreground.pixel, NULL );
    }

    XtVaSetValues ( w, XtNbackground, background.pixel,
                       XtNforeground, foreground.pixel, NULL );

    createPixmaps ( ( Widget ) gw );

    XtVaGetValues ( ( Widget ) gw, XmNuserData, &gui, NULL );

    area = createGLView ( w, gui, view->glrec.x    , view->glrec.y,
                                  view->glrec.width, view->glrec.height );

    /*** Add the widget to the list of opengl views, allowing us to ***/
    /*** refresh all of them when an action was done.               ***/
    list_append ( &gui->lglview, w );

    /*** Matrix update func must be called after OpenGL initialization ***/
    if ( gw->gview.cam ) {
        g3dobject_updateMatrix_r ( ( G3DOBJECT * ) gw->gview.cam, 0x00 );
    }

    dmlist[0x00] = XmStringCreate ( "Gouraud Shading", XmFONTLIST_DEFAULT_TAG );
    dmlist[0x01] = XmStringCreate ( "Flat Shading"   , XmFONTLIST_DEFAULT_TAG );
    dmlist[0x02] = XmStringCreate ( "Wireframe"      , XmFONTLIST_DEFAULT_TAG );

    ddw = XmCreateDropDownList ( w, EDITGLVIEWMODE, cmbargv, cmbargc );

    XtManageChild ( ddw );

    /*** OpenMotif prior to 2.3.4 crashes if this is set before managing ***/
    XtVaSetValues ( ddw, XmNheight, BUTTONSIZE,
                         XmNuserData, gui,
                         NULL );

    XmComboBoxSelectItem ( ddw, dmlist[0x00] );

    XmStringFree ( dmlist[0x00] );
    XmStringFree ( dmlist[0x01] );
    XmStringFree ( dmlist[0x02] );
    XtFree ( ( char * ) dmlist );

    XtAddCallback ( ddw, XmNselectionCallback, displayModeCbk, NULL );


    createGLViewMenuBar ( w );



    return ( XtRealizeProc ) NULL;
}

/******************************************************************************/
static XtSetValuesFunc set_values ( Widget current, Widget request, Widget set,
                                    ArgList args, Cardinal *numargs ) {
    GViewWidget gw = ( GViewWidget ) current;
    /*Cardinal i;

    for ( i = 0x00; i < (*numargs); i++ ) {
        printf ( "%s\n", args[i].name );
    }*/

    return 0x00;
}

/******************************************************************************/
GViewClassRec gviewClassRec = { { (WidgetClass) &xmManagerClassRec,   /*parent*/
                                  "GVIEW",                      /* class_name */
                                  sizeof ( GViewRec ),         /* widget size */
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
WidgetClass gviewWidgetClass = ( WidgetClass ) &gviewClassRec;

/******************************************************************************/
Widget createView ( Widget parent, G3DUI *gui, char *name, 
                    G3DCAMERA *cam,
                    Dimension x    , Dimension y,
                    Dimension width, Dimension height ) {
    G3DOBJECT *objcam = ( G3DOBJECT * ) cam;
    Pixel background, foreground;
    G3DUIVIEW *view;
    Widget gvw;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    gvw = XtVaCreateWidget ( name, gviewWidgetClass, parent,
                             XtNx, x,
                             XtNy, y,
                             XtNwidth , width,
                             XtNheight, height,
                             XtNcamera, cam,
                             XtNdefaultCamera, cam,
                             XmNforeground, foreground,
                             XmNbackground, background,
                             XmNuserData, gui,
                             NULL );

    if ( gvw == NULL ) {
        fprintf ( stderr, "createView: Could not create Widget\n" );
    } else {
        XtManageChild ( gvw );
    }

    view = &((GViewWidget)gvw)->gview;

    /*** I don't have time to implement GTK+3 property ***/
    /*** thing. Plus, it's , freaking complicated .... ***/
    view->cam = view->defcam = cam;

    memcpy ( &view->defcampos, &objcam->pos, sizeof ( G3DVECTOR ) );
    memcpy ( &view->defcamrot, &objcam->rot, sizeof ( G3DVECTOR ) );
    memcpy ( &view->defcamsca, &objcam->sca, sizeof ( G3DVECTOR ) );

              view->defcamfoc = cam->focal;

    return gvw;
}
