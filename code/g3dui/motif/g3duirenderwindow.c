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

static void Unmap ( Widget w, XtPointer client, XtPointer call );
static void Map   ( Widget w, XtPointer client, XtPointer call );

/******************************************************************************/
static void Destroy ( Widget w, XtPointer client, XtPointer call ) {
    Unmap ( w, client, call );
}

/******************************************************************************/
static void Size ( Widget w, XtPointer client, XtPointer call ) {
    if ( XtIsRealized ( w ) ) {
        XClearArea ( XtDisplay ( w ), XtWindow ( w ), 0, 0, 0, 0, False );
    }
}

/******************************************************************************/
static void Draw ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DUIRENDERPROCESS *rps = common_g3dui_getRenderProcessByID ( gui, ( uint64_t ) w );

    if ( rps == NULL ) {
        /*** start the rendering process once ***/
        Map ( w, client, call );

        rps = common_g3dui_getRenderProcessByID ( gui, ( uint64_t ) w );
    }

    /* rps might not be allocated yet by the map signal of the renderwindow */
    if ( rps ) {
        R3DFILTER *fil = r3dscene_getFilter ( rps->rsce, TOWINDOWFILTERNAME );

        if ( fil ) {
            FILTERTOWINDOW *ftw = ( FILTERTOWINDOW * ) fil->data;
            uint32_t x = 0, y = 0;
            Display    *dis      = XtDisplay ( w );
            Window      win      = XtWindow  ( w );
            static pthread_mutex_t lock = PTHREAD_MUTEX_INITIALIZER;
            Dimension width, height;

            XtVaGetValues ( w, XmNwidth, &width, 
                               XmNheight, &height, NULL );

            if ( width  > ftw->ximg->width  ) x = ( width  - ftw->ximg->width  ) * 0.5f;
            if ( height > ftw->ximg->height ) y = ( height - ftw->ximg->height ) * 0.5f;

            /*pthread_mutex_lock ( &lock );*/
            XShmPutImage ( ftw->dis, ftw->win, 
                                     ftw->gc, 
                                     ftw->ximg,
                                     0x00, 0x00,
                                     x, y,
                                     ftw->ximg->width, 
                                     ftw->ximg->height, False );
            /*** must be called or else expect jerky effects ***/
            XSync ( ftw->dis, 0 ); 
            XFlush ( ftw->dis );

            /*pthread_mutex_unlock ( &lock );*/
        }
    }
}

/******************************************************************************/
static void Map ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;
    G3DUIRENDERPROCESS *rps = common_g3dui_getRenderProcessByID ( gui, ( uint64_t ) w );
    G3DUIRENDERSETTINGS *rsg = gui->currsg;
    Dimension width, height;

    XtVaGetValues ( w, XmNwidth, &width, XmNheight, &height, NULL );

    rsg->width  = width;
    rsg->height = height;

    if ( rps == NULL ) {
        /*** this filter is used for displaying ***/
        R3DFILTER *towin = r3dfilter_toWidget_new ( w, 0x00 );
        /*** This filter is used for saving images ***/
        R3DFILTER *tobuf = r3dfilter_toBuffer_new ( rsg->width, 
                                                    rsg->height, 
                                                    rsg->depth, 
                                                    rsg->background );
        /*** Filter to free R3DSCENE, Filters & G3DUIRENDERPROCESS ***/
        /*R3DFILTER *clean = r3dfilter_new ( FILTERIMAGE, "CLEAN", g3dui_renderClean,
                                           NULL, 
                                           gui );*/

        LIST *lfilters = NULL;

        list_append ( &lfilters, tobuf );
        list_append ( &lfilters, towin );

        if ( rsg->flags & RENDERSAVE ) {
            if ( rsg->format == RENDERTOVIDEO ) {
                R3DFILTER *ffmpg;
                char buf[0x100];

                snprintf ( buf, 0x100, "%s.avi", rsg->outfile );

                ffmpg = r3dfilter_toFfmpeg_new ( 0x00, rsg->width,
                                                       rsg->height,
                                                       rsg->depth,
                                                       rsg->fps,
                                                       buf,
                                                       rsg->ffmpegpath,
                                                       rsg->ffplaypath );

                /*** ffmpg is NULL is ffmpeg is not found ***/
                if ( ffmpg ) {
                    list_insert ( &lfilters, ffmpg );
                } else {
                    /*GtkWidget *dialog = gtk_message_dialog_new ( NULL,
                                                                 GTK_DIALOG_DESTROY_WITH_PARENT,
                                                                 GTK_MESSAGE_ERROR,
                                                                 GTK_BUTTONS_CLOSE,
                                                                 "FFMpeg not found in PATH!" );
                    gtk_dialog_run (GTK_DIALOG (dialog));
                    gtk_widget_destroy (dialog);*/
                }
            }

            if ( rsg->format == RENDERTOIMAGE ) {
                R3DFILTER *toimg;
                char buf[0x100];

                snprintf ( buf, 0x100, "%s.jpg", rsg->outfile );

                if ( gui->currsg->startframe != gui->currsg->endframe ) {
                    toimg = r3dfilter_writeImage_new ( buf, 0x01 );
                } else {
                    toimg = r3dfilter_writeImage_new ( buf, 0x00 );
                }

                list_insert ( &lfilters, toimg );
            }
        }

        if ( rsg->mblur > 0x00 ) {
            R3DFILTER *mblur = r3dfilter_MotionBlur_new ( rsg->width, 
                                                          rsg->height,
                                                          rsg->depth, 
                                                          rsg->mblur );
            list_insert ( &lfilters, mblur );
        }

        /*** COMMENTED - Cleaning is done when closing window ***/
        /*if ( clean ) list_append ( &lfilters, clean );*/

        if ( gui->currsg->startframe != gui->currsg->endframe ) {
            /*** this filter tells the engine to go to the next frame ***/
            R3DFILTER *tofrm = r3dfilter_gotoframe_new ( gui );

            list_insert ( &lfilters, tofrm );

            rps = common_g3dui_render ( gui, ( uint64_t ) w,
                                             0x00, 0x00,
                                             rsg->width  - 0x01,
                                             rsg->height - 0x01,
                                             rsg->width,
                                             rsg->height,
                                             lfilters, 0x01 );
        } else {
            rps = common_g3dui_render ( gui, ( uint64_t ) w,
                                             0x00, 0x00,
                                             rsg->width  - 0x01,
                                             rsg->height - 0x01,
                                             rsg->width,
                                             rsg->height,
                                             lfilters, 0x00 );
        }
    }
}

/******************************************************************************/
static void Unmap ( Widget w, XtPointer client, XtPointer call ) {
/*static gboolean Destroy ( GtkWidget *widget, GdkEvent *event,
                                             gpointer user_data ) {*/
    G3DUI *gui = ( G3DUI * ) client;
    G3DUIRENDERPROCESS *rps = common_g3dui_getRenderProcessByID ( gui, ( uint64_t ) w );

    /*** cancel renderprocess if any ***/
    common_g3dui_cancelRenderByID ( gui, ( uint64_t ) w );

    if ( rps ) {
        g3duirenderprocess_free ( rps );

        list_remove ( &gui->lrps, rps );
    }

    /*return TRUE;*/
}

/******************************************************************************/
Widget createRenderWindowMenuBar ( Widget parent, G3DUI *gui, char *name,
                                   Dimension x    , Dimension y,
                                   Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget wfile, cfile;
    Widget wjpg, wpng;
    Widget menubar;
    G3DUIMOTIF *gmt = ( G3DUIMOTIF * ) gui->toolkit_data;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    menubar = XmVaCreateSimpleMenuBar ( parent, name,
                                        XmNfontList, gmt->fontlist,
                                        XmNx, x,
                                        XmNy, y,
                                        XmNwidth, width,
                                        XmNheight, height,
                                        XmNhighlightThickness, 0x01,
                                        XmNshadowThickness, 0x01,
                                        XmNbackground, background,
                                        XmNforeground, foreground,
                                        XmNmarginWidth, 0x00,
                                        XmNmarginHeight, 0x00,
                                        XmNresizeWidth, False,
                                        XmNresizeHeight, False,
                                        XmNtopAttachment  , XmATTACH_FORM,
                                        XmNleftAttachment , XmATTACH_FORM,
                                        XmNrightAttachment, XmATTACH_FORM,

                                        NULL );

    wfile = XmCreatePulldownMenu ( menubar, "FilePullDown", NULL, 0x00 );

    XtVaSetValues ( wfile, XmNhighlightThickness, 0x00,
                           XmNshadowThickness   , 0x01, NULL );

    cfile = XmVaCreateCascadeButton ( menubar, "File",
                                      XmNx, 0x00,
                                      XmNy, 0x00,
                                      XmNwidth, 0x60,
                                      XmNheight, height,
                                      XmNrecomputeSize, False,
                                      XmNfontList, gmt->fontlist,
                                      XmNhighlightThickness, 0x01,
                                      XmNshadowThickness, 0x01,
                                      XmNbackground, background,
                                      XmNforeground, 0x00,
                                      XmNsubMenuId, wfile,
                                      NULL );

    /************************************************/
    wjpg = XmVaCreatePushButton ( wfile, "Save as JPEG",
                                        XmNwidth, 0x60,
                                        XmNrecomputeSize, False,
                                        XmNfontList, gmt->fontlist,
                                        XmNhighlightThickness, 0x00,
                                        XmNshadowThickness, 0x00,
                                        XmNbackground, background,
                                        XmNforeground, 0x00,
                                        NULL );
    /*XtAddCallback ( wjpg, XmNarmCallback, newscenecbk, gui );*/
    XtManageChild ( wjpg );

    /************************************************/
    wpng = XmVaCreatePushButton ( wfile, "Save as PNG",
                                        XmNwidth, 0x60,
                                        XmNrecomputeSize, False,
                                        XmNfontList, gmt->fontlist,
                                        XmNhighlightThickness, 0x00,
                                        XmNshadowThickness, 0x00,
                                        XmNbackground, background,
                                        XmNforeground, 0x00,
                                        NULL );
    /*XtAddCallback ( wjpg, XmNarmCallback, newscenecbk, gui );*/
    XtManageChild ( wpng );


    XtManageChild ( cfile );

    XtManageChild ( menubar );


    return menubar;
}

/******************************************************************************/
Widget createRenderWindowDrawingArea ( Widget parent, Widget mnu, 
                                                      G3DUI *gui,
                                                      char *name,
                                                      Dimension x,
                                                      Dimension y,
                                                      Dimension width,
                                                      Dimension height ) {
    Pixel background, foreground;
    Widget wrk;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    wrk = XmVaCreateDrawingArea ( parent, RENDERWINDOWMENUWORKAREANAME,
                                          XtNx, 0x00,
                                          XtNy, 0x18,
                                          XtNwidth , width,
                                          XtNheight, height,
                                          XmNforeground, foreground,
                                          XmNbackground, background,
                                          XmNborderWidth, 0x01,

                                          XmNtopAttachment   , XmATTACH_WIDGET,
                                          XmNtopWidget       , mnu,
                                          XmNleftAttachment  , XmATTACH_FORM,
                                          XmNrightAttachment , XmATTACH_FORM,
                                          XmNbottomAttachment, XmATTACH_FORM,


                                          NULL );

    XtAddCallback ( wrk, XmNexposeCallback , Draw   , gui );
    XtAddCallback ( wrk, XmNresizeCallback , Size   , gui );
    XtAddCallback ( wrk, XmNdestroyCallback, Destroy, gui );

    /*XtAddCallback ( area, XmNexposeCallback, renderexposecbk, NULL ); */

    XtManageChild ( wrk );


    return wrk;
}

/******************************************************************************/
Widget g3duiRenderWindow_getWorkArea ( Widget mwd ) {
    WidgetList *children;
    Cardinal nc;
    int i;

    XtVaGetValues ( mwd, XmNchildren, &children, XmNnumChildren, &nc, NULL );

    for ( i = 0x00; i < nc; i++ ) {
        char *name = XtName ( ( Widget ) children[i] );

        if ( strcmp ( name, RENDERWINDOWMENUWORKAREANAME ) == 0x00 ) {

            return ( Widget ) children[i];
        }
    }

    return NULL;
}

/******************************************************************************/
Widget createRenderWindow ( Widget parent, G3DUI *gui, char *name,
                            Dimension x    , Dimension y,
                            Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget mwd, wrk, mnu;

    /*XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );*/

    mwd = XmVaCreateForm ( parent, name,
                                         XtNx, x,
                                         XtNy, y,
                                         XtNwidth , width,
                                         XtNheight, height,
                                         /*XmNforeground, foreground,
                                         XmNbackground, background,*/
                                         XmNborderWidth, 0x00,
                                         XmNuserData, gui,
                                         NULL );

    mnu = createRenderWindowMenuBar     ( mwd, gui, RENDERWINDOWMENUBARNAME     , 0,  0, width, 32 );
          createRenderWindowDrawingArea ( mwd, mnu, gui, RENDERWINDOWMENUWORKAREANAME, 0, 32, width, height - 32 );
  

    XtManageChild ( mwd );


    return mwd;
}
