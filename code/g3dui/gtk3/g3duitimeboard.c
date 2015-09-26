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
#include <g3dui_gtk3.h>

/******************************************************************************/
#include <xpm/record.xpm>
#include <xpm/stopanim.xpm>
#include <xpm/playanim.xpm>
#include <xpm/prevframe.xpm>
#include <xpm/nextframe.xpm>
#include <xpm/zoomtime.xpm>
#include <xpm/unzoomtime.xpm>

/******************************************************************************/
static GtkWidget *g3duitimeboard_getTimeline ( GtkWidget * );

/******************************************************************************/
void g3dui_stopCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

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
            static GOTOFRAME gtf;

            gtf.action.type = ACTION_GOTOFRAME;
            gtf.action.gui  = gui;
            gtf.frame       = curframe;

            g3duicom_requestActionFromMainThread ( gui, &gtf );
        }

#ifdef __linux__
        /*** wow, is this standard ? I had never used such statement style ***/
        /*** BTW, libGL crashes when it goes to fast, this call is required ***/
        nanosleep((struct timespec[]){{0, 40000000}}, NULL);
#endif
#ifdef  __MINGW32__
        Sleep ( 40);
#endif
    }

#ifdef __linux__
    pthread_exit ( NULL );
#endif

    return NULL;
}


/******************************************************************************/
void g3dui_playCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    #ifdef __MINGW32__
    HANDLE     hdl;
    #endif 

    /*** if scene is not currently played ***/
    if ( gui->playLock /*gui->playthreadid*/ == 0x00 ) {
        gui->playLock = 0x01;
        #ifdef __linux__
        pthread_attr_t attr;

        pthread_attr_init ( &attr );

        /*** launch rays in a thread ***/
        pthread_create ( &gui->playthreadid, &attr, play_t, gui );

        /*** prepare to release resources after thread termination ***/
        pthread_detach ( gui->playthreadid );
        #endif

        #ifdef __MINGW32__
        hdl = CreateThread ( NULL, 
                             0,
                             (LPTHREAD_START_ROUTINE) play_t, 
                             gui,
                             0,
                             &gui->playthreadid );
        #endif
    }

    /*common_g3dui_playCbk ( gui );*/
}

/******************************************************************************/
void g3dui_nextFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    gui->curframe += 1;

    common_g3dui_gotoFrameCbk ( gui );
}

/******************************************************************************/
void g3dui_prevFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    gui->curframe -= 1;

    common_g3dui_gotoFrameCbk ( gui );
}

/******************************************************************************/
void g3dui_zoomFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    GtkWidget *timeline = g3duitimeboard_getTimeline ( parent );
    TIMELINEDATA *tdata = g_object_get_data ( G_OBJECT(timeline),
                                              GTK3WIDGETDATA );

    tdata->nbpix += 0x02;

    if ( tdata->nbpix > MAXIMUMFRAMEGAP ) tdata->nbpix  = MAXIMUMFRAMEGAP;

    gtk_widget_queue_draw ( widget );
}

/******************************************************************************/
void g3dui_unzoomFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    GtkWidget *timeline = g3duitimeboard_getTimeline ( parent );
    TIMELINEDATA *tdata = g_object_get_data ( G_OBJECT(timeline),
                                              GTK3WIDGETDATA );

    tdata->nbpix -= 0x02;

    if ( tdata->nbpix < MINIMUMFRAMEGAP ) tdata->nbpix  = MINIMUMFRAMEGAP;

    gtk_widget_queue_draw ( widget );
}

/******************************************************************************/
void g3dui_recordFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    GtkWidget *timeline = g3duitimeboard_getTimeline ( parent );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( timeline ) {
        common_g3dui_recordFrameCbk ( gui, KEYPOSITION | 
                                           KEYROTATION | 
                                           KEYSCALING );
    }
}

/******************************************************************************/
static GtkWidget *g3duitimeboard_getTimeline ( GtkWidget *widget ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( GTK_IS_DRAWING_AREA(child) ) {
            if ( strcmp ( child_name, TIMELINENAME ) == 0x00 ) {

                return child;
            }
        }

        children = g_list_next ( children );
    }


    return NULL;
}

/******************************************************************************/
void Resize ( GtkWidget *widget, GdkRectangle *allocation,
                                 gpointer user_data ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );
        GdkRectangle gdkrec;

        if ( strcmp ( child_name, TIMELINENAME ) == 0x00 ) {
            gdkrec.x      = 168;
            gdkrec.y      = 0x00;
            gdkrec.width  = allocation->width - 168;
            gdkrec.height = allocation->height;

            if ( gtk_widget_get_has_window ( widget ) == 0x00 ) {
                gdkrec.x += allocation->x;
                gdkrec.y += allocation->y;
            }

            gtk_widget_size_allocate ( child, &gdkrec );
        }

        children = g_list_next ( children );
    }
}

/******************************************************************************/
GtkWidget *createTimeBoard ( GtkWidget *parent, G3DUI *gui,
                                                char *name,
                                                gint x,
                                                gint y,
                                                gint width,
                                                gint height ) {
    GdkRectangle gdkrec = { 0, 0, width, height };
    GtkWidget *frm = gtk_fixed_new ( );

    gtk_widget_set_name      ( frm, name );
    gtk_widget_size_allocate ( frm, &gdkrec );

    g_signal_connect ( G_OBJECT (frm), "size-allocate", G_CALLBACK (Resize), gui );

    gtk_fixed_put ( GTK_FIXED(parent), frm, x, y );


    createImageButton ( frm, gui, TIMEBOARDPREV  , prevframe_xpm ,   0,  0, 24, 24, g3dui_prevFrameCbk   );
    createImageButton ( frm, gui, TIMEBOARDSTOP  , stopanim_xpm  ,  24,  0, 24, 24, g3dui_stopCbk        );
    createImageButton ( frm, gui, TIMEBOARDPLAY  , playanim_xpm  ,  48,  0, 24, 24, g3dui_playCbk        );
    createImageButton ( frm, gui, TIMEBOARDNEXT  , nextframe_xpm ,  72,  0, 24, 24, g3dui_nextFrameCbk   );
    createImageButton ( frm, gui, TIMEBOARDZOOM  , zoomtime_xpm  ,  96,  0, 24, 24, g3dui_zoomFrameCbk   );
    createImageButton ( frm, gui, TIMEBOARDUNZOOM, unzoomtime_xpm, 120,  0, 24, 24, g3dui_unzoomFrameCbk );
    createImageButton ( frm, gui, TIMEBOARDRECORD, record_xpm    , 144,  0, 24, 24, g3dui_recordFrameCbk );

    createTimeline    ( frm, gui, TIMELINENAME , 168, 0, width - 168, height );






    gtk_widget_show_all ( frm );


    return frm;
}
