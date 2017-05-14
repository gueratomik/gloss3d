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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
#include <xpm/record.xpm>
#include <xpm/stopanim.xpm>
#include <xpm/playanim.xpm>
#include <xpm/prevframe.xpm>
#include <xpm/nextframe.xpm>
#include <xpm/zoomtime.xpm>
#include <xpm/unzoomtime.xpm>

/******************************************************************************/
static Widget g3duitimeboard_getTimeline ( Widget );

/******************************************************************************/
static void stopCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    common_g3dui_stopCbk ( gui );
}

/******************************************************************************/
static void *play_t ( void *ptr ) {
    G3DUI *gui = ( G3DUI * ) ptr;
    G3DSCENE *sce = gui->sce;
    float curframe = gui->curframe;

    /*** http://www.motifdeveloper.com/tips/tip6.html ***/
    while ( gui->playLock ) {
	curframe += 1.0f;

        /*** skip sending event if animation has not completed yet ***/
        if ( ( gui->flags & ONGOINGANIMATION ) == 0x00 ) {
	    g3duicom_goToFrame ( gui, curframe );
        }

        /*** wow, is this standard ? I had never used such statement style ***/
        /*** BTW, libGL crashes when it goes to fast, this call is required ***/
        nanosleep((struct timespec[]){{0, 40000000}}, NULL);
    }

    pthread_exit ( NULL );


    return NULL;
}

/******************************************************************************/
static void playCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    gui->playLock = 0x01;

    /*** if scene is not currently played ***/
    if ( gui->playthreadid == 0x00 ) {
        pthread_attr_t attr;

        pthread_attr_init ( &attr );

        /*** launch rays in a thread ***/
        pthread_create ( &gui->playthreadid, &attr, play_t, gui );

        /*** prepare to release resources after thread termination ***/
        pthread_detach ( gui->playthreadid );
    }

    /*common_g3dui_playCbk ( gui );*/
}

/******************************************************************************/
static void nextFrameCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    gui->curframe += 1.0f;

    common_g3dui_gotoFrameCbk ( gui );
}


/******************************************************************************/
static void prevFrameCbk ( Widget w, XtPointer client, XtPointer call ) {
    G3DUI *gui = ( G3DUI * ) client;

    gui->curframe += 1.0f;

    common_g3dui_gotoFrameCbk ( gui );
}

/******************************************************************************/
static void zoomFrameCbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget parent = XtParent ( w );
    Widget timeline = g3duitimeboard_getTimeline ( parent );
    TIMELINEDATA *tdata = NULL;

    if ( timeline ) {
        XtVaGetValues ( timeline, XmNuserData, &tdata, NULL );

        if ( tdata ) {
            tdata->nbpix += 0x02;

            if ( tdata->nbpix > MAXIMUMFRAMEGAP ) tdata->nbpix  = MAXIMUMFRAMEGAP;


            XClearArea ( XtDisplay ( timeline ),
                         XtWindow  ( timeline ),
                         0x00, 0x00, 0x00, 0x00, True );
        }
    }
}

/******************************************************************************/
static void unzoomFrameCbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget parent = XtParent ( w );
    Widget timeline = g3duitimeboard_getTimeline ( parent );
    TIMELINEDATA *tdata = NULL;

    if ( timeline ) {
        XtVaGetValues ( timeline, XmNuserData, &tdata, NULL );

        if ( tdata ) {
            tdata->nbpix -= 0x02;

            if ( tdata->nbpix > MAXIMUMFRAMEGAP ) tdata->nbpix  = MAXIMUMFRAMEGAP;


            XClearArea ( XtDisplay ( timeline ),
                         XtWindow  ( timeline ),
                         0x00, 0x00, 0x00, 0x00, True );
        }
    }
}

/******************************************************************************/
static void recordFrameCbk ( Widget w, XtPointer client, XtPointer call ) {
    Widget parent = XtParent ( w );
    Widget timeline = g3duitimeboard_getTimeline ( parent );
    G3DUI *gui = ( G3DUI * ) client;

    if ( timeline ) {
        common_g3dui_recordFrameCbk ( gui, KEYPOSITION | 
                                           KEYROTATION | 
                                           KEYSCALING );
    }
}

/******************************************************************************/
static Widget g3duitimeboard_getTimeline ( Widget parent ) {
    WidgetList children;
    Cardinal nc;
    int i;
        
    XtVaGetValues ( parent, XmNchildren, &children,
                            XmNnumChildren, &nc,
                            NULL );

    for ( i = 0x00; i < nc; i++ ) {
        Widget child = children[i];

        if ( strcmp ( TIMELINENAME, XtName ( child ) ) == 0x00 ) {

            return child;
        }
    }

    return NULL;
}

/******************************************************************************/
Widget createTimeboard ( Widget parent, G3DUI *gui, char *name,
                         Dimension x    , Dimension y,
                         Dimension width, Dimension height ) {
    Pixel background, foreground;
    Widget frm, timeline, btn;

    XtVaGetValues ( parent, XmNbackground, &background,
                            XmNforeground, &foreground, NULL );

    frm = XmVaCreateForm ( parent, name,
                           XtNx, x,
                           XtNy, y,
                           XtNwidth , width,
                           XtNheight, height,
                           XmNbackground, background,
                           XmNforeground, foreground,
                           /*** needed for keyboard input ***/  
                           XmNtraversalOn,   TRUE,
                           XmNancestorSensitive, TRUE,
                           XmNsensitive, TRUE,
                           NULL );

    /***********************************************************************/
          createImageButton ( frm, gui, TIMEBOARDPREV  , prevframe_xpm ,   0,  0, 24, 24, prevFrameCbk   );
          createImageButton ( frm, gui, TIMEBOARDSTOP  , stopanim_xpm  ,  24,  0, 24, 24, stopCbk        );
          createImageButton ( frm, gui, TIMEBOARDPLAY  , playanim_xpm  ,  48,  0, 24, 24, playCbk        );
          createImageButton ( frm, gui, TIMEBOARDNEXT  , nextframe_xpm ,  72,  0, 24, 24, nextFrameCbk   );
          createImageButton ( frm, gui, TIMEBOARDZOOM  , zoomtime_xpm  ,  96,  0, 24, 24, zoomFrameCbk   );
          createImageButton ( frm, gui, TIMEBOARDUNZOOM, unzoomtime_xpm, 120,  0, 24, 24, unzoomFrameCbk );
    btn = createImageButton ( frm, gui, TIMEBOARDRECORD, record_xpm    , 144,  0, 24, 24, recordFrameCbk );

    timeline = createTimeline ( frm, gui, TIMELINENAME , 168, 0, width - 168, height );

    XtVaSetValues ( timeline, XmNleftAttachment, XmATTACH_WIDGET,
                              XmNleftWidget    , btn, NULL );

    XtVaSetValues ( timeline, XmNrightAttachment, XmATTACH_FORM, NULL );


    XtManageChild ( frm );

    return frm;
}
