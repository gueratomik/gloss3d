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
#include <Xm/Text.h>
#include <Xm/PrimitiveP.h>

/******************************************************************************/
#include <X11/xpm.h>

/******************************************************************************/
#include <pthread.h>

/******************************************************************************/
#include <GL/gl.h>
#include <GL/glu.h>
#include <GL/GLwMDrawA.h>

/******************************************************************************/
#include <list.h>

/******************************************************************************/
#include <TabbedBook.h>
#include <ColorEdit.h>

/******************************************************************************/
#include <g3d.h>
#include <g3dcom.h>
#include <r3d.h>
#include <r3dnet.h>
#include <g3durmanager.h>
#include <g3dmouse.h>
#include <g3dui.h>

/******************************************************************************/
void updateRenderSettings ( Widget );

/******************************************************************************/
void g3duirendersettings_getFfmpegPath ( G3DUIRENDERSETTINGS *rsg ) {
    FILE *fp = popen ("which ffmpeg", "r" );

    if ( fp ) {
        uint32_t len;

        fgets ( rsg->ffmpegpath, FFMPEGPATHLEN, fp );

        len = strlen ( rsg->ffmpegpath );

        if ( len ) {
            /*** trim the final newline ***/
            rsg->ffmpegpath[len-1] = 0x00;
        }

        pclose ( fp );
    }
}

/******************************************************************************/
void g3duirendersettings_getFfplayPath ( G3DUIRENDERSETTINGS *rsg ) {
    FILE *fp = popen ("which ffplay", "r" );

    if ( fp ) {
        uint32_t len;

        fgets ( rsg->ffplaypath, FFMPEGPATHLEN, fp );

        len = strlen ( rsg->ffplaypath );

        if ( len ) {
            /*** trim the final newline ***/
            rsg->ffplaypath[len-1] = 0x00;
        }

        pclose ( fp );
    }
}

/******************************************************************************/
G3DUIRENDERSETTINGS *g3duirendersettings_new ( ) {
    G3DUIRENDERSETTINGS *rsg = calloc ( 0x01, sizeof ( G3DUIRENDERSETTINGS ) );

    if ( rsg == NULL ) {
        fprintf ( stderr, "g3duirendersettings_new: memory allocation failed\n" );

        return NULL;
    }

    rsg->flags       = 0x00;
    rsg->fps         = 0x18;
    rsg->depth       = 0x18;
    rsg->startframe  = 0x00;
    rsg->endframe    = 0x00;
    rsg->outfile     = "output.avi";
    rsg->format      = RENDERTOJPG;
    rsg->width       = 640;
    rsg->height      = 480;
    rsg->ratio       = 0.0f;
    rsg->background  = 0x00404040;

    g3duirendersettings_getFfmpegPath ( rsg );
    g3duirendersettings_getFfplayPath ( rsg );

    rsg->lfilter     = NULL;


    return rsg;
}

/******************************************************************************/
void g3duirendersettings_rendertoview ( G3DUIRENDERSETTINGS *rsg,
                                        G3DSCENE *sce,
                                        G3DCAMERA *cam,
                                        uint32_t x1, uint32_t y1,
                                        uint32_t x2, uint32_t y2,
                                        Widget area ) {
    GViewWidget gw = ( GViewWidget ) XtParent ( area );
    Display *dis = XtDisplay ( area );
    Window win   = XtWindow  ( area );
    R3DFILTER *fil = r3dfilter_toWindow_new ( dis, win );
    XWindowAttributes wat;
    LIST *lfilters = NULL;
    R3DSCENE *rsce = NULL;
    pthread_attr_t attr;
    pthread_t tid;


    if ( gw->gview.render_tid == 0x00 ) {
        XGetWindowAttributes ( dis, win, &wat );

        r3dfilter_setType ( fil, FILTERLINE );

        list_append ( &lfilters, fil );

        pthread_attr_init ( &attr );

        /*** start thread son all CPUs ***/
        pthread_attr_setscope ( &attr, PTHREAD_SCOPE_SYSTEM );

        rsce = r3dscene_new ( sce, cam, x1, y1,
                                        x2, y2,
                                        wat.width, wat.height,
                                        rsg->background,
                                        rsg->startframe,
                                        rsg->endframe,
                                        lfilters );

        rsce->render_tid = &gw->gview.render_tid;

        /*** launch rays in a thread ***/
        pthread_create ( &tid, &attr,(void*(*)(void*))r3dscene_render_frame_t, rsce );

        /*** Remember the thread id for cancelling on mouse input e.g ***/
        gw->gview.render_tid = tid;

        /*** prepare to release resources after thread termination ***/
        pthread_detach ( tid );
    }
}

/******************************************************************************/
void g3duirendersettings_rendertoimage ( G3DUIRENDERSETTINGS *rsg, 
                                         G3DSCENE *sce,
                                         G3DCAMERA *cam,
                                         Widget area ) {
    Display *dis = ( area ) ? XtDisplay ( area ) : NULL;
    Window win   = ( area ) ? XtWindow  ( area ) : 0x00;
    XWindowAttributes wat;
    LIST *lfilters = NULL;
    R3DSCENE *rsce = NULL;
    pthread_attr_t attr;
    pthread_t tid;
    R3DFILTER *towin = NULL, *tobuf = NULL;
    float ratio = ( rsg->ratio ) ? rsg->ratio : cam->ratio;
    uint32_t rsgwidth = rsg->height * ratio;

    if ( area ) {
        towin = r3dfilter_toWidget_new ( area );
        tobuf = r3dfilter_toBuffer_new ( rsgwidth, rsg->height, 
                                         rsg->depth, rsg->background );

        XGetWindowAttributes ( dis, win, &wat );

        list_append ( &lfilters, tobuf );
        list_append ( &lfilters, towin );
    }

    pthread_attr_init ( &attr );

    /*** start thread son all CPUs ***/
    pthread_attr_setscope ( &attr, PTHREAD_SCOPE_SYSTEM );

    rsce = r3dscene_new ( sce, cam, 0x00, 0x00,
                                    rsgwidth - 1, rsg->height - 1,
                                    rsgwidth    , rsg->height,
                                    rsg->background,
                                    rsg->startframe,
                                    rsg->endframe,
                                    lfilters );

    /*** launch rays in a thread ***/
    pthread_create ( &tid, &attr, (void*(*)(void*))r3dscene_render_frame_t, rsce );

    /*** If the rendering window is closed while the raytracer is running,  ***/
    /*** this will allow us to cleanly terminate the rendering thread using ***/
    /*** the DrawingArea Widget's destroywindow callback. ***/
    if ( area ) {
        G3DUIRENDERPROCESS *rps = g3duirenderprocess_new ( tid, towin->data,
                                                                tobuf->data );

        XtVaSetValues ( area, XmNuserData, rps, NULL );
    }

    /*** prepare to release resources after thread termination ***/
    pthread_detach ( tid );
}

/******************************************************************************/
void g3duirendersettings_rendertovideo ( G3DUIRENDERSETTINGS *rsg, 
                                         G3DSCENE *sce,
                                         G3DCAMERA *cam,
                                         Widget area ) {
    Display *dis = ( area ) ? XtDisplay ( area ) : NULL;
    Window win   = ( area ) ? XtWindow  ( area ) : 0x00;
    XWindowAttributes wat;
    LIST *lfilters = NULL;
    R3DSCENE *rsce = NULL;
    pthread_attr_t attr;
    pthread_t tid;
    float ratio = ( rsg->ratio ) ? rsg->ratio : cam->ratio;
    uint32_t rsgwidth = rsg->height * ratio;
    R3DFILTER *ffmpg = r3dfilter_toFfmpeg_new ( 0x00, 
                                                rsgwidth,
                                                rsg->height,
                                                rsg->depth,
                                                rsg->fps,
                                                rsg->outfile,
                                                rsg->ffmpegpath,
                                                rsg->ffplaypath );
    R3DFILTER *towin = NULL, *tobuf = NULL;

    /*** ffmpg is NULL is ffmpeg is not found ***/
    if ( ffmpg ) {
        list_insert ( &lfilters, ffmpg );
    }

    if ( area ) {
        towin = r3dfilter_toWidget_new ( area );
        tobuf = r3dfilter_toBuffer_new ( rsgwidth, rsg->height, 
                                         rsg->depth, rsg->background );

        list_insert ( &lfilters, tobuf );
        list_insert ( &lfilters, towin );
    }

    if ( rsg->mblur > 0x00 ) {
        R3DFILTER *mblur = r3dfilter_MotionBlur_new ( rsgwidth, 
                                                      rsg->height,
                                                      rsg->depth, rsg->mblur );
        list_insert ( &lfilters, mblur );
    }

    pthread_attr_init ( &attr );

    /*** start thread son all CPUs ***/
    /*pthread_attr_setscope ( &attr, PTHREAD_SCOPE_SYSTEM );*/

    rsce = r3dscene_new ( sce, cam, 0x00, 0x00,
                                    rsgwidth - 1, rsg->height - 1,
                                    rsgwidth    , rsg->height,
                                    rsg->background,
                                    rsg->startframe,
                                    rsg->endframe,
                                    lfilters );

    /*** launch rays in a thread ***/
    pthread_create ( &tid, &attr, (void*(*)(void*))r3dscene_render_sequence_t, rsce );

    /*** If the rendering window is closed while the raytracer is running,  ***/
    /*** this will allow us to cleanly terminate the rendering thread using ***/
    /*** the DrawingArea Widget's destroywindow callback. ***/
    if ( area ) {
        G3DUIRENDERPROCESS *rps = g3duirenderprocess_new ( tid, towin->data,
                                                                tobuf->data );

        XtVaSetValues ( area, XmNuserData, rps, NULL );
    }

    /*** prepare to release resources after thread termination ***/
    pthread_detach ( tid );
}

/******************************************************************************/
static void startfrmcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DUIRENDERSETTINGS *rsg = gui->currsg;
    char *value = XmTextGetString ( w );
    uint32_t startframe = strtol ( value, NULL, 10 );

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->startframe = startframe;
}

/******************************************************************************/
static void endfrmcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DUIRENDERSETTINGS *rsg = gui->currsg;
    char *value = XmTextGetString ( w );
    uint32_t endframe = strtol ( value, NULL, 10 );

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->endframe = endframe;
}

/******************************************************************************/
static void mblurcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DUIRENDERSETTINGS *rsg = gui->currsg;
    char *value = XmTextGetString ( w );
    uint32_t mblur = strtol ( value, NULL, 10 );

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->mblur = mblur;
}

/******************************************************************************/
static void fpscbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DUIRENDERSETTINGS *rsg = gui->currsg;
    char *value = XmTextGetString ( w );
    uint32_t fps = strtol ( value, NULL, 10 );

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->fps = fps;
}

/******************************************************************************/
static void outputcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    /*list_remove ( &gui->lrendersettings, w );*/
}

/******************************************************************************/
static void rendercbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DUIRENDERSETTINGS *rsg = gui->currsg;
    G3DSCENE *sce = gui->sce;
    G3DCAMERA *cam = gui->curcam;

    if ( rsg->endframe == rsg->startframe ) {
        g3duirendersettings_rendertoimage ( rsg, sce, cam, NULL );
    } else {
        g3duirendersettings_rendertovideo ( rsg, sce, cam, NULL );
    }
}

/******************************************************************************/
static void destroycbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

}

/******************************************************************************/
static void previewcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DUIRENDERSETTINGS *rsg = gui->currsg;

    if ( rsg->flags & RENDERPREVIEW ) {
        rsg->flags &= (~RENDERPREVIEW);
    } else {
        rsg->flags |=   RENDERPREVIEW;
    } 
}

/******************************************************************************/
void updateRenderHeight ( Widget w ) {
    G3DUIRENDERSETTINGS *rsg;
    WidgetList children;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    G3DUI *gui;
    Cardinal nc;
    int i;

    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    rsg = gui->currsg;

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    XtVaGetValues ( w, XmNchildren, &children,
                       XmNnumChildren, &nc,
                       NULL );

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];
        char *name = XtName ( child );
        char buf[0x10];


        if ( XtClass ( child ) == xmTextWidgetClass ) {
            /****************** Height *****************/
            if ( strcmp ( name, EDITRENDERHEIGHT ) == 0x00 ) {
                float ratio = ( rsg->ratio ) ? rsg->ratio : gui->curcam->ratio;
                uint32_t rsgheight = rsg->width / ratio;

                snprintf ( buf, 0x10, "%d", rsgheight );

                XmTextSetString ( child, buf );
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
void updateRenderWidth ( Widget w ) {
    G3DUIRENDERSETTINGS *rsg;
    WidgetList children;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    G3DUI *gui;
    Cardinal nc;
    int i;

    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    rsg = gui->currsg;

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    XtVaGetValues ( w, XmNchildren, &children,
                       XmNnumChildren, &nc,
                       NULL );

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];
        char *name = XtName ( child );
        char buf[0x10];


        if ( XtClass ( child ) == xmTextWidgetClass ) {
            /****************** Width *****************/
            if ( strcmp ( name, EDITRENDERWIDTH ) == 0x00 ) {
                float ratio = ( rsg->ratio ) ? rsg->ratio : gui->curcam->ratio;
                uint32_t rsgwidth = rsg->height * ratio;

                snprintf ( buf, 0x10, "%d", rsgwidth );

                XmTextSetString ( child, buf );
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void widthcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DUIRENDERSETTINGS *rsg = gui->currsg;
    char *value = XmTextGetString ( w );
    uint32_t width = strtol ( value, NULL, 10 );
    Widget parent = XtParent ( w );

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->width  = width;
    rsg->height = width / rsg->ratio;

    updateRenderHeight ( parent );
}

/******************************************************************************/
static void heightcbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DUIRENDERSETTINGS *rsg = gui->currsg;
    char *value = XmTextGetString ( w );
    uint32_t height = strtol ( value, NULL, 10 );
    Widget parent = XtParent ( w );

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->height = height;
    rsg->width  = height * rsg->ratio;

    updateRenderWidth ( parent );
}

/******************************************************************************/
static void ratiocbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DUIRENDERSETTINGS *rsg = gui->currsg;
    char *value = XmTextGetString ( w );
    float ratio = strtof ( value, NULL );
    Widget parent = XtParent ( w );

    if ( gui->lock ) return; /*** prevent a loop ***/

    rsg->ratio = ratio;

    rsg->width = rsg->height * rsg->ratio;

    updateRenderWidth ( parent );
}

/******************************************************************************/
void updateRenderSettings ( Widget w ) {
    G3DUIRENDERSETTINGS *rsg;
    WidgetList children;
    G3DOBJECT *obj;
    G3DSCENE *sce;
    G3DUI *gui;
    Cardinal nc;
    int i;

    XtVaGetValues ( w, XmNuserData, &gui, NULL );

    rsg = gui->currsg;

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    XtVaGetValues ( w, XmNchildren, &children,
                       XmNnumChildren, &nc,
                       NULL );

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];
        char *name = XtName ( child );
        char buf[0x10];

        if ( XtClass ( child ) == xmToggleButtonWidgetClass ) {
            /*if ( strcmp ( name, EDITKEYLOOP ) == 0x00 ) {

            }*/
        }

        if ( XtClass ( child ) == xmPushButtonWidgetClass ) {
            /*if ( strcmp ( name, EDITRENDERBACKGROUND ) == 0x00 ) {
                uint32_t pixel = rsg->background;

                XtVaSetValues ( child, XmNbackground, pixel, NULL );
            }*/
        }

        if ( XtClass ( child ) == xmTextWidgetClass ) {
            /****************** Start Frame *****************/
            if ( strcmp ( name, EDITRENDERSTART ) == 0x00 ) {
                snprintf ( buf, 0x10, "%d", rsg->startframe );

                XmTextSetString ( child, buf );
            }

            /****************** End Frame *****************/
            if ( strcmp ( name, EDITRENDEREND ) == 0x00 ) {
                snprintf ( buf, 0x10, "%d", rsg->endframe );

                XmTextSetString ( child, buf );
            }

            /****************** End Frame *****************/
            if ( strcmp ( name, EDITRENDERMBLUR ) == 0x00 ) {
                snprintf ( buf, 0x10, "%d", rsg->mblur );

                XmTextSetString ( child, buf );
            }

            /****************** End Frame *****************/
            if ( strcmp ( name, EDITRENDERRATIO ) == 0x00 ) {
                float ratio = ( rsg->ratio ) ? rsg->ratio : gui->curcam->ratio;

                snprintf ( buf, 0x10, "%f", ratio );

                XmTextSetString ( child, buf );
            }

            /****************** FPS *****************/
            if ( strcmp ( name, EDITRENDERFPS ) == 0x00 ) {
                snprintf ( buf, 0x10, "%d", rsg->fps );

                XmTextSetString ( child, buf );
            }

            /****************** FPS *****************/
            if ( strcmp ( name, EDITRENDERWIDTH ) == 0x00 ) {
                float ratio = ( rsg->ratio ) ? rsg->ratio : gui->curcam->ratio;
                uint32_t rsgwidth = rsg->height * ratio;

                snprintf ( buf, 0x10, "%d", rsgwidth );

                XmTextSetString ( child, buf );
            }

            /****************** FPS *****************/
            if ( strcmp ( name, EDITRENDERHEIGHT ) == 0x00 ) {
                snprintf ( buf, 0x10, "%d", rsg->height );

                XmTextSetString ( child, buf );
            }

            if ( strcmp ( name, EDITRENDEROUTPUT ) == 0x00 ) {
                if ( rsg->outfile ) {
                    uint32_t len = strlen ( rsg->outfile ) + 0x01;
                    char *txt = calloc ( len, sizeof ( char ) );

                    snprintf ( txt, len, "%s", rsg->outfile );

                    XmTextSetString ( child, txt );

                    free ( txt );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void backgroundcbk ( uint32_t pixel, void *data ) {
    G3DUI *gui = ( G3DUI * ) data;
    G3DUIRENDERSETTINGS *rsg = gui->currsg;

    if ( rsg ) {
        LIST *ltmp = gui->lmatlist;

        rsg->background = pixel;
    }
}

/******************************************************************************/
Widget createRenderSettings ( Widget parent, G3DUI *gui, char *name,
                                             Dimension x    , Dimension y,
                                             Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget frm, col;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    frm = XmVaCreateManagedForm ( parent, name,
                                  XmNx, x,
                                  XmNy, y,
                                  XmNwidth , width,
                                  XmNheight, height,
                                  XmNforeground, foreground,
                                  XmNbackground, background,
                                  XmNuserData, gui,
                                  NULL );


    createToggleLabel ( frm, EDITRENDERPREVIEW, 0,   0, 104, 20, previewcbk  );
    createIntegerText ( frm, EDITRENDERSTART  , 0,  20, 96,  32, startfrmcbk );
    createIntegerText ( frm, EDITRENDEREND    , 0,  40, 96,  32, endfrmcbk   );
    createIntegerText ( frm, EDITRENDERFPS    , 0,  60, 96,  32, fpscbk      );
    createFloatText   ( frm, EDITRENDERRATIO  , 0, 140, 96,  64, ratiocbk    );
    createIntegerText ( frm, EDITRENDERWIDTH  , 0,  80, 96,  32, widthcbk    );
    createIntegerText ( frm, EDITRENDERHEIGHT , 0, 100, 96,  32, heightcbk   );
    createCharText    ( frm, EDITRENDEROUTPUT , 0, 120, 96, 200, outputcbk   );
    createIntegerText ( frm, EDITRENDERMBLUR  , 0, 160, 96,  64, mblurcbk    );

    createSimpleLabel ( frm, EDITRENDERBACKGROUND, 0, 180, 96 );

    col = XmCreateColorEditLabel ( frm, EDITRENDERBACKGROUND,
                                        96, 180,
                                        32, 18,
                                        0x00, backgroundcbk, gui );
    XtManageChild ( col );

    createSimplePushButton  ( frm, "Render",   0x140, 0x160, 0x30, 0x14, renderfinalcbk );


    updateRenderSettings ( frm );

    /*list_insert ( &gui->lrendersettings, frm );*/

    /*** We have to remove this widget form the keyedit list when closed ***/
    XtAddCallback ( frm, XmNdestroyCallback, destroycbk, gui );


    return frm;
}

/******************************************************************************/
/*** This filter is declared in the g3dui layer because of the Widget struct***/
R3DFILTER *r3dfilter_toWidget_new ( Widget w ) {
    Display *dis = XtDisplay ( w );
    Window win = XtWindow ( w );
    R3DFILTER *fil;

    fil = r3dfilter_new ( FILTERLINE, TOWINDOWFILTERNAME,
                                      filtertowindow_draw,
   /*** When destroyed, the widget is in charge of freeing the private data ***/
                                      NULL, 
                                      filtertowindow_new ( dis, win ) );

    return fil;
}

/******************************************************************************/
G3DUIRENDERPROCESS *g3duirenderprocess_new ( pthread_t tid,
                                             FILTERTOWINDOW *ftw,
                                             FILTERTOBUFFER *ftb ) {
    uint32_t size = sizeof ( G3DUIRENDERPROCESS );
    G3DUIRENDERPROCESS *rps = ( G3DUIRENDERPROCESS * ) calloc ( 0x01, size );

    if ( rps == NULL ) {
        fprintf ( stderr, "g3duirenderprocess_new: mem allocation failed\n" );
    }

    rps->tid = tid;
    rps->ftw = ftw;
    rps->ftb = ftb;

    return rps;
}

/******************************************************************************/
void g3duirenderprocess_free ( G3DUIRENDERPROCESS *rps ) {
    filtertowindow_free ( rps->ftw );
    filtertobuffer_free ( rps->ftb );

    if ( rps->filename ) free ( rps->filename );

    free ( rps );
}

/******************************************************************************/
void g3duirenderprocess_filename ( G3DUIRENDERPROCESS *rps, char *filename ) {
    uint32_t len = strlen ( filename ) + 0x01;

    rps->filename = realloc ( rps->filename, len );

    memset ( rps->filename, 0x00, len );

    memcpy ( rps->filename, filename, len );
}
