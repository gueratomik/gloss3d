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
#include <X11/cursorfont.h>

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
#include <Xm/CascadeB.h>
#include <Xm/ComboBox.h>

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
static void init2D ( Widget w, XtPointer client, XtPointer call ) {
    float spec[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    GViewWidget gw = ( GViewWidget ) XtParent ( w );
    glDisable ( GL_LIGHTING );
    glEnable ( GL_LIGHT0 );
    /*glEnable ( GL_COLOR_MATERIAL );*/

    glLightfv ( GL_LIGHT0, GL_SPECULAR, ( const float * ) spec );

    glEnable ( GL_DEPTH_TEST );

    /*** I still have to understand this below ***/
    /*** I got it from Internet for avoiding Z-Fighting ***/
    glPolygonOffset ( -1.0f, -1.0f );
    glEnable(GL_POLYGON_OFFSET_FILL);

    glPolygonMode ( GL_FRONT_AND_BACK, GL_LINE );

    gw->gview.mode = GLVIEWWIREFRAME;
}

/******************************************************************************/
static void init3D ( Widget w, XtPointer client, XtPointer call ) {
    float spec[] = { 0.0f, 0.0f, 0.0f, 0.0f };

    GViewWidget gw = ( GViewWidget ) XtParent ( w );
    glEnable ( GL_LIGHTING );
    glEnable ( GL_LIGHT0 );
    /*glEnable ( GL_COLOR_MATERIAL );*/

    glLightfv ( GL_LIGHT0, GL_SPECULAR, ( const float * ) spec );

    glEnable ( GL_DEPTH_TEST );

    /*** I still have to understand this below ***/
    /*** I got it from Internet for avoiding Z-Fighting ***/
    glPolygonOffset ( 1.0f, 1.0f );
    glEnable(GL_POLYGON_OFFSET_FILL);

    glPolygonMode ( GL_FRONT_AND_BACK, GL_FILL );
    /*glLightModeli ( GL_LIGHT_MODEL_TWO_SIDE, GL_TRUE );*/

    gw->gview.mode = GLVIEWGOURAUD;
}

/******************************************************************************/
/* Find the current button ID according to x and y coordinates. This helsp us */
/* determine which button we are on. We don't use Motif button here because   */
/* we need to track the mouse after a click, something I think, will be easier*/
/* to achieve that way. I might be wrong though :-/.                          */
/******************************************************************************/
static int getCurrentButton ( Widget wid, int x, int y ) {
    GViewWidget w = ( GViewWidget ) wid;
    int i;

    for ( i = 0x00; i < NBVIEWBUTTON; i++ ) {
        XRectangle *rec = &w->gview.rec[i];

        if ( ( ( x >= rec->x ) && ( x <= ( rec->x + rec->width  ) ) ) &&
             ( ( y >= rec->y ) && ( y <= ( rec->y + rec->height ) ) ) ) {
            return i;
        }
    }

    return -1;
}

/******************************************************************************/
static void GViewMouseEvent ( Widget w, XEvent *event,
                              String *params, Cardinal *num_params ) {
    GViewWidget gw = ( GViewWidget ) w;
    G3DCAMERA *cam = gw->gview.cam;
    Widget ogl = gw->gview.ogl;
    static int xold, yold;
    static int btnid = -1;
    static int xmid, ymid, xori, yori;
    Dimension width, height;
    G3DUI *gui;

    XtVaGetValues ( w, XmNwidth   , &width, 
                       XmNheight  , &height,
                       XmNuserData, &gui,
                       NULL );
    
    switch ( event->type ) {
        case ButtonPress : {
            XButtonEvent *bev = ( XButtonEvent * ) event;

            btnid = getCurrentButton ( w, bev->x, bev->y );

            /*** If we press a button, set the highlight pixmap ***/
            if ( btnid >= 0x00 ) {
                gw->gview.curpix[btnid] = gw->gview.armpix[btnid];
            }

            xmid = ( width  >> 0x01 );
            ymid = ( height >> 0x01 );

            xold = xori = bev->x;
            yold = yori = bev->y;

            /*** then redraw the widget ***/
            XClearArea ( XtDisplay ( w ), XtWindow ( w ), 0, 0, 0, 0, True );
        } break;

        case MotionNotify : {
            XMotionEvent *mev = ( XMotionEvent * ) event;
            Window win = XtWindow ( w );
            Display *dis = XtDisplay ( w );
            XWindowAttributes attr;

            /*** If pressed ***/
            if ( btnid >= 0x00 ) {
                switch ( btnid ) {
                    case ROTATEBUTTON : {
                        G3DOBJECT *obj = ( G3DOBJECT * ) cam;

                        if ( cam && ( mev->state & Button1Mask ) ) {
                            obj->rot.x += ( yold - mev->y );
                            obj->rot.y += ( xold - mev->x );
                        }

                        if ( cam && ( mev->state & Button3Mask ) ) {
                            obj->rot.z += ( xold - mev->x );
                        }

                        g3dobject_updateMatrix_r ( obj, 0x00 );
                    } break;

                    case ZOOMBUTTON : {
                        G3DOBJECT *obj = ( G3DOBJECT * ) cam;
                        int difx = ( xold - mev->x ),
                            dify = ( yold - mev->y );

                        if ( cam && ( mev->state & Button1Mask ) ) {
                            cam->focal -= ( (float) ( difx ) / 20.0f );
                        }

                        if ( cam && ( mev->state & Button3Mask ) ) {
                            obj->pos.z -= ( (float) ( difx ) / 20.0f );

                            /*** if orthogonal view, dont let the camera ***/
                            /*** go negative ***/
                            if ( cam->focal == 2.0f ) {
                                if ( obj->pos.z > cam->zfar  ) obj->pos.z = cam->zfar;
                                if ( obj->pos.z < cam->znear ) obj->pos.z = cam->znear;
                            }
                        }

                        g3dobject_updateMatrix_r ( obj, 0x00 );
                    } break;

                    case TRANSLATEBUTTON : {
                        G3DOBJECT *obj = ( G3DOBJECT * ) cam;
                        float posx, posy, posz;
                        int difx = ( xold - mev->x ),
                            dify = ( yold - mev->y );

                        if ( cam && ( mev->state & Button1Mask ) ) {
                            obj->pos.x -= ( (float) ( difx ) / 20.0f );
                            obj->pos.y += ( (float) ( dify ) / 20.0f );
                        }

                        if ( cam && ( mev->state & Button3Mask ) ) {
                            obj->pos.z -= ( (float) ( difx ) / 20.0f );

                            /*** if orthogonal view, dont let the camera ***/
                            /*** go negative ***/
                            if ( cam->focal == 2.0f ) {
                                if ( obj->pos.z > cam->zfar  ) obj->pos.z = cam->zfar;
                                if ( obj->pos.z < cam->znear ) obj->pos.z = cam->znear;
                            }
                        }

                        g3dobject_updateMatrix_r ( obj, 0x00 );
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

            XClearArea ( XtDisplay ( ogl ),
                         XtWindow  ( ogl ), 0, 0, 0, 0, True );
        } break;

        case ButtonRelease : {
            /*XButtonEvent *bev = ( XButtonEvent * ) event;*/
            Window win = XtWindow ( w );
            Display *dis = XtDisplay ( w );
            XWindowAttributes attr;

            /*** if we release a button, then set the normal pixmap ***/
            if ( btnid >= 0x00 ) {
                gw->gview.curpix[btnid] = gw->gview.norpix[btnid];

                switch ( btnid ) {
                    case MAXIMIZEBUTTON : {
                        Widget parent = XtParent ( w );

                        if ( parent ) {
                            Widget wmax;

                            XtVaGetValues ( parent, XtNmax, &wmax, NULL );

                            if ( wmax == w ) {
                                XtVaSetValues ( parent, XtNmax, NULL, NULL );
                            } else {
                                XtVaSetValues ( parent, XtNmax, w, NULL );
                            }
                        }
                    } break;
                }

	        XGetWindowAttributes(dis, win, &attr );

                XSelectInput ( dis, win, None );

                XWarpPointer ( dis, None, win, 0, 0, 0, 0, xori, yori );
                XSync( dis, False );

                XSelectInput ( dis, win, attr.your_event_mask );

                btnid = -1;
            }

            XClearArea ( XtDisplay ( w ),
                         XtWindow ( w ), 0x00, 0x00, 0x00, 0x00, True );
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
      XtRImmediate, ( XtPointer ) NULL },
    { XtNdefaultCameraPosition, XtCString, XtRString, sizeof ( String ),
      XtOffsetOf ( GViewRec, gview.defpos ),
      XtRImmediate, ( XtPointer ) NULL },
    { XtNdefaultCameraRotation,
      XtNdefaultCameraRotation, XtRString, sizeof ( XtPointer ),
      XtOffsetOf ( GViewRec, gview.defrot ),
      XtRImmediate, ( XtPointer ) NULL },
    { XtNdefaultCameraScaling,
      XtNdefaultCameraScaling, XtRString, sizeof ( XtPointer ),
      XtOffsetOf ( GViewRec, gview.defsca ),
      XtRImmediate, ( XtPointer ) NULL },
    { XtNdefaultCameraFocal,
      XtNdefaultCameraFocal, XtRString, sizeof ( XtPointer ),
      XtOffsetOf ( GViewRec, gview.deffoc ),
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

/******************************************************************************/
static XtProc ClassInitialize ( ) {

    return ( XtProc ) NULL;
}

/******************************************************************************/
static void showGL ( Widget w, XtPointer client, XtPointer call ) {
    GViewWidget gw = ( GViewWidget ) XtParent ( w );
    Display *dis = XtDisplay ( w );
    Window win   = XtWindow  ( w );
    G3DCAMERA *cam = gw->gview.cam;
    G3DUI *gui = NULL;
    G3DMOUSETOOL *mou;

    /*** Retrieve our global datas ***/
    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    /*** Retrieve the current mouse tool ***/
    mou = gui->mou;

    /*** Set Context as the current context ***/
    glXMakeCurrent ( dis, win, gw->gview.glx );

    if ( gui ) {
        G3DSCENE *sce = gui->sce;
        G3DVECTOR vec = { 0.0f, 0.0f, 0.0f, 1.0f };
        G3DOBJECT *selobj = g3dscene_getSelectedObject ( sce );

        glClear ( GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT );

        glMatrixMode ( GL_PROJECTION );
        glLoadIdentity ( );

        if ( cam ) g3dcamera_project ( cam, gui->flags | gw->gview.flags );

        glMatrixMode ( GL_MODELVIEW );
        glLoadIdentity ( );


        if ( cam ) {
            G3DVECTOR pos = { 0.0f, 0.0f, 0.0f, 1.0f };
            double TMPX[0x10];

            if ( selobj ) {
                if ( gui->flags & VIEWOBJECT ) {
                    g3dvector_matrix ( &pos, selobj->wmatrix, &cam->pivot );
                }

                if ( selobj->type == G3DMESHTYPE ) {
                    G3DMESH *mes= ( G3DMESH * ) selobj;

                    if ( gui->flags & VIEWVERTEX ) {
                        g3dmesh_getSelectedVerticesWorldPosition ( mes, 
                                                                   &cam->pivot);
                    }

                    if ( gui->flags & VIEWFACE ) {
                        g3dmesh_getSelectedFacesWorldPosition ( mes, 
                                                                &cam->pivot);
                    }

                    if ( gui->flags & VIEWEDGE ) {
                        g3dmesh_getSelectedEdgesWorldPosition ( mes, 
                                                                &cam->pivot);
                    }
                }


            }

            g3dcamera_view ( cam, gui->flags | gw->gview.flags );
        }

        g3dobject_draw ( ( G3DOBJECT * ) sce, cam, gui->flags | gw->gview.flags );

        g3dscene_draw  ( ( G3DOBJECT * ) sce, cam, gui->flags | gw->gview.flags );

        /*** draw the mouse tool only in the current workspace window ***/
        if ( w == gui->curogl ) {
            if ( mou && mou->draw ) {
                mou->draw ( mou, sce, gui->flags | gw->gview.flags );
            }
        }
    } else {
        fprintf ( stderr, "Could not retrieve global datas\n" );
    }

    GLwDrawingAreaSwapBuffers ( w );

    XSync ( dis, False );
}

/*******************************************************************************/
static void sizeGL ( Widget w, XtPointer client, XtPointer call ) {
    GViewWidget gw   = ( GViewWidget ) XtParent ( w );
    Dimension width  = gw->core.width;
    Dimension height = gw->core.height;
    G3DCAMERA *cam = gw->gview.cam;
    Display *dis = XtDisplay ( w );
    Window win   = XtWindow  ( w );

    glXMakeCurrent ( dis, win, gw->gview.glx );

    glViewport ( 0, 0, width, height - BUTTONSIZE );

    if ( cam ){
        /*** retrieve the viewport matrix, we'll use it for rendering ***/
        glGetIntegerv ( GL_VIEWPORT, cam->vmatrix );

        /*** Really, you don't want a divide by zero ***/
        cam->ratio = ( height ) ? ( double ) width / height : 1.0f;
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
    G3DUI *gui = NULL;
    G3DSCENE *sce;
    static uint32_t shiftKey;

    /*** Retrieve our global datas ***/
    XtVaGetValues ( wid, XmNuserData, &gui, NULL );

    /*** Set the current OGL Widget only if a mouse click happened ***/
    /*if ( event->type == ButtonPress ) {*/
    gui->curogl = wid;

    gui->curcam = gw->gview.cam;
/*printf("RENDERTID:%d\n",gw->gview.render_tid );*/
    cancelRender ( ( Widget ) gw );

    urm = gui->urm;
    cli = gui->cli;
    sce = gui->sce;

    /*** needed for keyboard input ***/
    XmProcessTraversal ( wid, XmTRAVERSE_CURRENT ); 

    glXMakeCurrent ( dis, win, gw->gview.glx );

    switch ( das->event->type ) {
        case KeyPress : {
            XKeyEvent *kev = ( XKeyEvent * ) das->event;
            KeySym keysym;
            char c;

            if ( XLookupString ( kev, &c, 0x01, &keysym, NULL ) > 0x00 ) {
                switch ( keysym ) {
                    case XK_Delete: {
                        g3dui_setHourGlass ( gui );

                        /*** gw has XmNuserData set ***/
                        deleteSelectedItems ( wid, client, call );

                        g3dui_unsetHourGlass ( gui );
                    } break;

        /*** For an unknown reason, KeyPress state shiftMask does not work ***/
                    case XK_z: {
                        /*** undo ***/
                        if ( kev->state & ControlMask ) {
                            undocbk ( wid, gui, call );
                        }
                    } break;

                    case XK_y: {
                        /*** redo ***/
                        if ( kev->state & ControlMask ) {
                            redocbk ( wid, gui, call );
                        }
                    } break;

                    case XK_s:
                        if ( kev->state & ControlMask ) {
                            if ( gui->filename ) {
                                printf ( "saving as:%s\n", gui->filename );

                                g3duiSaveG3DFile ( gui );
                            } else {
                                saveascbk ( wid, client, call );
                            }
                        }
                    break;

                    /*** This piece of code is also in g3duilist ****/
                    /*** Ctrl+c is copy object ***/
                    case XK_c :
                        if ( kev->state & ControlMask ) {
printf("copying %d object(s)\n", list_count ( sce->lsel ) );
                           g3dui_setHourGlass ( gui );
                           g3duiclipboard_copyObject ( cli, sce, sce->lsel, 0x01, gui->flags );
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
                           g3duiclipboard_paste ( cli, urm, sce, dst, gui->flags );
                           g3dui_unsetHourGlass ( gui );
                        }

                        redrawObjectList     ( gui );
                        updateAllCurrentEdit ( gui );
                        redrawGLViews        ( gui );
                    break;
                    /*************************************************/

                    case XK_a: {
                        /*** select all objects ***/
                        if ( gui->flags & VIEWOBJECT ) {
                            g3dscene_unselectAllObjects ( sce, gui->flags );
                            g3dscene_selectAllObjects   ( sce, gui->flags );
                        }

                        /*** select all vertices ***/
                        if ( gui->flags & VIEWVERTEX ) {
                            G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

                            if ( obj && obj->type == G3DMESHTYPE ) {
                                G3DMESH *mes = ( G3DMESH * ) obj;

                                g3dmesh_unselectAllVertices ( mes );
                                g3dmesh_selectAllVertices   ( mes );
                            }
                        }

                        /*** select all edges ***/
                        if ( gui->flags & VIEWEDGE ) {
                            G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

                            if ( obj && obj->type == G3DMESHTYPE ) {
                                G3DMESH *mes = ( G3DMESH * ) obj;

                                g3dmesh_unselectAllEdges ( mes );
                                g3dmesh_selectAllEdges   ( mes );
                            }
                        }

                        /*** select all vertices ***/
                        if ( gui->flags & VIEWFACE ) {
                            G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

                            if ( obj && obj->type == G3DMESHTYPE ) {
                                G3DMESH *mes = ( G3DMESH * ) obj;

                                g3dmesh_unselectAllFaces ( mes );
                                g3dmesh_selectAllFaces   ( mes );
                            }
                        }

                        redrawGLViews ( gui );
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

        if ( gui->mou->tool ) {
            uint32_t msk = gui->mou->tool ( gui->mou, gui->sce,
                                            gui->curcam, gui->urm,
                                            gui->flags, event );
            /*** The curent tool must be allowed to ***/
            /*** run in the current drawing mode.   ***/
        /*if ( ( gui->mou->flags & MODEMASK ) == ( gui->flags & MODEMASK ) ) {*/

                if ( msk & REDRAWVIEW ) {
                    redrawGLViews ( gui );
                }

                if ( msk & REDRAWLIST ) {
                    redrawObjectList ( gui );
                }

                if ( msk & REDRAWCOORDS ) {
                    updateCoords ( gui );
                }

                if ( msk & REDRAWCURRENTOBJECT ) {
                    updateAllCurrentEdit ( gui );
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
    Display *dis = XtDisplay ( w );
    Window win   = XtWindow  ( w );
    G3DCAMERA *cam = ( G3DCAMERA * ) client;
    static GLXContext shared = NULL;

    XtTranslations trans = XtParseTranslationTable ( actions );

    /*** Set the new translation table ***/
    XtOverrideTranslations ( w, trans );

    XtVaGetValues ( w, GLwNvisualInfo, &vi, NULL );

    /*** Create OpenGL Context ***/
    gw->gview.glx = glXCreateContext( dis, vi, shared, True );

    if ( shared == NULL ) shared = gw->gview.glx;

    /*** Set Context as the current context ***/
    glXMakeCurrent ( dis, win, gw->gview.glx );

    /*** Set clear color for the OpenGL Window ***/
    glClearColor ( 0.40f, 0.40f, 0.40f, 1.0f );

    if ( vi->depth < 0x20 ) {
        glEnable ( GL_DITHER );
    }

    /*** Free the memory assigned for GLwNvisualInfo ***/
    free ( vi );

    /*** Temp, this must be done by the camera in the future ***/
    if ( cam && ((G3DOBJECT*)cam)->id == 0x00 ) {
        init3D ( w, client, call );
    } else {
        init2D ( w, client, call );
    }
}

/******************************************************************************/
void addGLMenuButton ( G3DUI *gui, Widget menu, G3DMOUSETOOL *mou ) {
    Pixel background, foreground;
    Widget btn;

    XtVaGetValues ( menu, XmNbackground, &background,
                          XmNforeground, &foreground, NULL );

    btn = XmVaCreatePushButton ( menu, mou->name,
                                 /*** arguments ***/
                                 XmNx, 0x00,
                                 XmNy, 0x00,
                                 XmNfontList, gui->fontlist,
                                 XmNhighlightThickness, 0x01,
                                 XmNshadowThickness, 0x00,
                                 XmNmarginHeight, 0x04,
                                 XmNmarginWidth, 0x04,
                                 XmNbackground, background,
                                 XmNforeground, foreground,
                                 XmNuserData, gui,
                                 NULL );

    XtAddCallback ( btn, XmNactivateCallback, setMouseTool, mou );

    XtManageChild ( btn );
}

/******************************************************************************/
void addMenuListButton ( G3DUI *gui, LIST *lmenu, G3DMOUSETOOL *mou ) {
    LIST *ltmpmenu = lmenu;

    while ( ltmpmenu ) {
        Widget menu = ( Widget ) ltmpmenu->data;

        addGLMenuButton ( gui, menu, mou );

        ltmpmenu = ltmpmenu->next;
    }
}

/******************************************************************************/
void dispatchMenuButton ( G3DUI *gui, G3DMOUSETOOL *mou, uint32_t tool_flags ) {
    LIST *lobjmenu = gui->lobjmenu,
         *lvermenu = gui->lvermenu,
         *ledgmenu = gui->ledgmenu,
         *lfacmenu = gui->lfacmenu;

    if ( tool_flags & OBJECTMODETOOL ) addMenuListButton ( gui, lobjmenu, mou );
    if ( tool_flags & VERTEXMODETOOL ) addMenuListButton ( gui, lvermenu, mou );
    if ( tool_flags & EDGEMODETOOL   ) addMenuListButton ( gui, ledgmenu, mou );
    if ( tool_flags & FACEMODETOOL   ) addMenuListButton ( gui, lfacmenu, mou );
}

/******************************************************************************/
Widget createViewMenu ( Widget w, G3DUI *gui, char *menuname ) {
    GViewWidget gw = ( GViewWidget ) XtParent ( w );
    Pixel background, foreground;
    Widget menu;

    XtVaGetValues ( w, XmNbackground, &background,
                       XmNforeground, &foreground, NULL );

    menu = XmVaCreateSimplePopupMenu ( w, menuname, NULL,
                                       XmNbackground, background,
                                       XmNshadowThickness, 0x01,
                                       /** this will allow oglmenu to easily **/
                                       /** retrieve Scene Datas without the  **/
                                       /** need for global variables.        **/
                                       XmNuserData, gui,
                                       /***************************************/
                                       XmNspacing, 0x00,
                                       XmNmarginWidth, 0x00,
                                       XmNmarginHeight, 0x00,
                                       /*XmNmenuPost, "<Btn3Down>",*/
                                       XmNpopupEnabled, XmPOPUP_DISABLED,
                                       XmNhighlightThickness, 0x00,
                                       XmNbackground, background,
                                       XmNforeground, foreground,
                                       NULL );


    return menu;
}

/******************************************************************************/
Widget getMenu ( Widget w, char *menuname ) {
    WidgetList children;
    Cardinal nc;
    uint32_t i;

    XtVaGetValues ( w, XmNchildren, &children, XmNnumChildren, &nc, NULL );

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( strcmp ( XtName ( child ), menuname ) == 0x00 ) return child;
    }

    return NULL;
}

/******************************************************************************/
static void PostMenu ( Widget w, XtPointer client, XEvent *event, Boolean *c ) {
    XButtonEvent *ev = ( XButtonEvent * ) event;
    Widget menu = ( Widget ) client;
    G3DUI *gui;

    XtVaGetValues ( menu, XmNuserData, &gui, NULL );

    if ( ev->button != Button3 ) return;

    /*** When buttonPress event occurs, we dont want the mouse event to be ***/
    /*** Dispatched to other widget, so we cancel the event-dispatching ***/
    if ( ev->type == ButtonPress ) {
        (*c) = False;

         return;
    }

    /*** When buttonRelease occurs, display the right Menu ***/
    if ( gui->flags & VIEWOBJECT ) {
        if ( strcmp ( XtName ( menu ), OBJECTMENUNAME ) != 0x00 ) return;
    }

    if ( gui->flags & VIEWVERTEX ) {
        if ( strcmp ( XtName ( menu ), VERTEXMENUNAME ) != 0x00 ) return;
    }

    if ( gui->flags & VIEWEDGE ) {
        if ( strcmp ( XtName ( menu ), EDGEMENUNAME   ) != 0x00 ) return;
    }

    if ( gui->flags & VIEWFACE ) {
        if ( strcmp ( XtName ( menu ), FACEMENUNAME   ) != 0x00 ) return;
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

    XtAddEventHandler ( w, ButtonReleaseMask | 
                           ButtonPressMask, False, PostMenu, menu );
}

/******************************************************************************/
static void createVertexMenu ( Widget w, G3DUI *gui ) {
    Widget menu = createViewMenu ( w, gui, VERTEXMENUNAME );

    list_insert ( &gui->lvermenu, menu );

    XtAddEventHandler ( w, ButtonReleaseMask | 
                           ButtonPressMask, False, PostMenu, menu );
}

/******************************************************************************/
static void createEdgeMenu ( Widget w, G3DUI *gui ) {
    Widget menu = createViewMenu ( w, gui, EDGEMENUNAME );

    list_insert ( &gui->ledgmenu, menu );

    XtAddEventHandler ( w, ButtonReleaseMask | 
                           ButtonPressMask, False, PostMenu, menu );
}


/******************************************************************************/
static void createFaceMenu ( Widget w, G3DUI *gui ) {
    Widget menu = createViewMenu ( w, gui, FACEMENUNAME );

    list_insert ( &gui->lfacmenu, menu );

    XtAddEventHandler ( w, ButtonReleaseMask | 
                           ButtonPressMask, False, PostMenu, menu );
}

/******************************************************************************/
extern WidgetClass glwMDrawingAreaWidgetClass;

/******************************************************************************/
static void createGLView ( Widget wid, G3DUI *gui, Dimension x,
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
                                     XmNuserData,      gui,
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
                                     XmNuserData,      gui,
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
                                     XmNuserData,      gui,
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
        XtAddCallback ( ogl, GLwNresizeCallback, sizeGL , NULL );
        XtAddCallback ( ogl, GLwNexposeCallback, showGL , NULL );
        XtAddCallback ( ogl, GLwNinputCallback , inputGL, NULL );

        /*** Add the widget to the list of opengl views, allowing us to ***/
        /*** refresh all of them when an action was done.               ***/
        list_append ( &gui->lglview, gw );

        gw->gview.ogl = ogl;

        XtManageChild ( ogl );

        createObjectMenu ( ogl, gui );
        createVertexMenu ( ogl, gui );
        createEdgeMenu   ( ogl, gui );
        createFaceMenu   ( ogl, gui );
    }
}

/******************************************************************************/
static void createPixmaps ( Widget wid ) {
    GViewWidget w = ( GViewWidget ) wid;
    Pixel foreground = w->manager.highlight_color,
          background = w->core.background_pixel,
          armedcolor = w->manager.foreground;
    int depth        = w->core.depth;
    Display *dis = XtDisplay ( w );
    Window win = XtWindow ( w );
    int i;

    for ( i = 0x00; i < NBVIEWBUTTON; i++ ) {
        Pixmap norpix = XCreatePixmapFromBitmapData ( dis, win, BITMAPDATA[i],
                                                      BUTTONSIZE, BUTTONSIZE,
                                                      foreground, background,
                                                      depth  ),

               armpix = XCreatePixmapFromBitmapData ( dis, win, BITMAPDATA[i],
                                                      BUTTONSIZE, BUTTONSIZE,
                                                      foreground, armedcolor,
                                                      depth  );

        w->gview.curpix[i] = norpix;
        w->gview.norpix[i] = norpix;
        w->gview.armpix[i] = armpix;
    }
}

/******************************************************************************/
static void buttonPosition ( GViewWidget gw ) {
    int i, xpos = ( gw->core.width - BUTTONSIZE ), brd = 0x02;

    /*** set rectangle position for each button ***/
    for ( i = 0x00; i < NBVIEWBUTTON; i++, xpos = ( xpos - BUTTONSIZE - brd ) ) {
        gw->gview.rec[i].x = xpos;
        gw->gview.rec[i].y = 0x00;
        gw->gview.rec[i].width  = BUTTONSIZE;
        gw->gview.rec[i].height = BUTTONSIZE;
    }
}

/******************************************************************************/
static XtInitProc Initialize ( Widget request, Widget init,
                               ArgList args, Cardinal *num ) {
    GViewWidget gw = ( GViewWidget ) init;

    /*** Compute the rectangle position for buttons ***/
    /*** This is must set in the initalize function because no resizing ***/
    /*** callbacks will be called when our program is mapped ***/
    buttonPosition ( gw );

    return ( XtInitProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Destroy ( Widget w ) {

    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
static XtExposeProc Redisplay ( Widget wid, XEvent *event, Region region ) {
    GViewWidget w = ( GViewWidget ) wid;
    Display *dis = XtDisplay ( wid );
    Window win = XtWindow ( wid );
    XGCValues values;
    GC gc;
    int i, xpos;

    gc = XtGetGC ( wid, 0x00, &values );

    for ( i = 0x00, xpos = 0x00; i < NBVIEWBUTTON; i++, ( xpos += (BUTTONSIZE+2) ) ) {
        XCopyArea ( dis, w->gview.curpix[i], win, gc,
                    0x00, 0x00, BUTTONSIZE, BUTTONSIZE,
                    w->gview.rec[i].x, w->gview.rec[i].y );
    }

    XtReleaseGC ( wid, gc );

    return ( XtExposeProc ) NULL;
}

/******************************************************************************/
static XtWidgetProc Resize ( Widget w ) {
    GViewWidget gw = ( GViewWidget ) w;
    Widget ogl = gw->gview.ogl;
    Dimension glwidth  =   gw->core.width,
              glheight = ( gw->core.height - BUTTONSIZE );
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
    buttonPosition ( gw );

    if ( XtIsRealized ( w ) && XtIsRealized ( ogl ) && ( glwidth > 0x00  ) &&
                                                       ( glheight > 0x00 ) ) {

        XtConfigureWidget ( ogl, 0x00, BUTTONSIZE, glwidth, glheight, 0x00 );
    }


    return ( XtWidgetProc ) NULL;
}

/******************************************************************************/
static void displayModeCbk ( Widget w, XtPointer client, XtPointer call ) {
    XmComboBoxCallbackStruct *cbs = ( XmComboBoxCallbackStruct * ) call;
    GViewWidget gw = ( GViewWidget ) XtParent ( w );
    G3DUI *gui;
    char *str;

    glXMakeCurrent ( XtDisplay ( gw->gview.ogl ),
                     XtWindow  ( gw->gview.ogl ), gw->gview.glx );

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

    if ( obj->type == G3DCAMERATYPE ) {
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


        if ( gw->gview.defpos ) stringToVector (gw->gview.defpos, &objcam->pos);
        if ( gw->gview.defrot ) stringToVector (gw->gview.defrot, &objcam->rot);
        if ( gw->gview.defsca ) stringToVector (gw->gview.defsca, &objcam->sca);
        if ( gw->gview.deffoc ) cam->focal = atof ( gw->gview.deffoc );

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

    XtVaGetValues ( parent, XmNuserData, &gui,
                            XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    menubar = XmVaCreateSimpleMenuBar ( w, MENUBARNAME,
                                        XmNfontList, gui->fontlist,
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
                                      XmNfontList, gui->fontlist,
                                      XmNhighlightThickness, 0x00,
                                      XmNshadowThickness, 0x01,
                                      XmNbackground, background,
                                      XmNforeground, 0x00,
                                      XmNsubMenuId, wopts,
                                      NULL );

    createMenuBarButton ( gui, wopts, "Use default camera"  , 0x80, usedefcamcbk );
    createMenuBarButton ( gui, wopts, "Use selected camera" , 0x80, usecameracbk );
    createMenuBarButton ( gui, wopts, "Show / Hide normals" , 0x80, normalscbk   );
    createMenuBarButton ( gui, wopts, "Show / Hide bones"   , 0x80, bonescbk     );
    createMenuBarButton ( gui, wopts, "Show / Hide grid"    , 0x80, gridcbk      );
    createMenuBarButton ( gui, wopts, "Turn on/off lighting", 0x80, lightingcbk  );
    createMenuBarButton ( gui, wopts, "Reset view"          , 0x80, resetcamcbk  );

    XtManageChild ( copts );

    XtManageChild ( menubar );
}

/******************************************************************************/
static XtRealizeProc Realize ( Widget w, XtValueMask *value_mask,
                               XSetWindowAttributes *attributes ) {
    GViewWidget gw = ( GViewWidget ) w;
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
    Widget ddw;
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

    createGLView  ( ( Widget ) gw, gui, 0x00,
                                        BUTTONSIZE,
                                        gw->core.width,
                                        gw->core.height - BUTTONSIZE );

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
void createDefaultViews ( Widget quad, G3DUI *gui ) {
    G3DCAMERA *cam[0x04] = { g3dcamera_new ( 0x00, "Camera",  0.0f, 1.0f,
                                             0.1f, 10000.0f ),
                             g3dcamera_new ( 0x01, "Camera",  0.0f, 1.0f,
                                             -1000.0f, 1000.0f ),
                             g3dcamera_new ( 0x02, "Camera",  0.0f, 1.0f,
                                             -1000.0f, 1000.0f ),
                             g3dcamera_new ( 0x03, "Camera",  0.0f, 1.0f,
                                             -1000.0f, 1000.0f ) };
    void (*grid[0x04])(G3DCAMERA *,uint32_t) = { g3dcamera_grid3D,
                                                 g3dcamera_gridXY,
                                                 g3dcamera_gridZX,
                                                 g3dcamera_gridYZ };
    /*** We pass float values as string because XtVa* will convert float as ***/
    /*** double (because it's a variable argument function) and this will ***/
    /*** put a mess in Motif and will make it crash (segfault). ***/

    /*** Default rotation values ***/
    char *rot[0x04] = { "15.0 45.0 0.0",
                        " 0.0  0.0 0.0",
                        "90.0  0.0 0.0",
                        " 0.0 90.0 0.0" };
    /*** Default position values ***/
    char *pos[0x04] = { "0.0 0.0 -4.0 ",
                        "0.0 0.0 -1.25",
                        "0.0 0.0 -1.25",
                        "0.0 0.0 -1.25" };
    /*** Default scaling values ***/
    char *sca[0x04] = { "1.0 1.0 1.0",
                        "1.0 1.0 1.0",
                        "1.0 1.0 1.0",
                        "1.0 1.0 1.0" };
    /*** Default focal values ***/
    char *focal[0x04] = { "45.0", "2.0", "2.0", "2.0" };
    XSegment *seg;
    Widget view;
    uint32_t i;

    XtVaGetValues ( ( Widget ) quad, XtNsegment, &seg, NULL );

    gui->defcam = gui->curcam = cam[0x00];

    for ( i = 0x00; i < 0x04; i++ ) {
        G3DOBJECT *objcam = ( G3DOBJECT * ) cam[i];
        cam[i]->focal = atof ( focal[i] );
        cam[i]->grid  = grid[i];

        stringToVector ( rot[i], &objcam->rot );
        stringToVector ( pos[i], &objcam->pos );
        stringToVector ( sca[i], &objcam->sca );

        cam[i]->focal = atof ( focal[i] );

        /*memcpy ( &((G3DOBJECT*)cam[i])->rot, &rot[i], sizeof ( G3DVECTOR ) );
        memcpy ( &((G3DOBJECT*)cam[i])->pos, &pos[i], sizeof ( G3DVECTOR ) );
        memcpy ( &((G3DOBJECT*)cam[i])->sca, &sca[i], sizeof ( G3DVECTOR ) );*/

        view = XtVaCreateWidget ( "GLVIEW", gviewWidgetClass, ( Widget ) quad,
                                  XmNx, seg[i].x1,
                                  XmNy, seg[i].y1,
                                  XmNwidth, seg[i].x2 - seg[i].x1,
                                  XmNheight, seg[i].y2 - seg[i].y1,

                                  XmNcamera, cam[i],
                                  XmNdefaultCamera   , cam[i],
                                  XmNuserData, gui,
                                  /*** Backup the default position ***/
                                  XtNdefaultCameraPosition, pos[i],
                                  /*** Backup the default rotation ***/
                                  XtNdefaultCameraRotation, rot[i],
                                  /*** Backup the default scaling factor ***/
                                  XtNdefaultCameraScaling , sca[i],
                                  /*** Backup the default Focal ***/
                                  XtNdefaultCameraFocal   , focal[i],

                                  NULL );

        XtManageChild ( view );

        /*** Update Widget comboboxes ***/
        updateGLView ( view );
    }
}
