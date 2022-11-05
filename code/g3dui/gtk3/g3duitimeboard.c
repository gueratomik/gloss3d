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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2020         */
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
#include <xpm/scale_keys.xpm>
#include <xpm/pick_keys.xpm>

/******************************************************************************/



/******************************************************************************/
static GTK3G3DUITIMEBOARD *gtk3_g3duitimeboard_new ( GTK3G3DUI *gtk3gui ) {
    uint32_t memSize =  sizeof ( GTK3G3DUITIMEBOARD );
    GTK3G3DUITIMEBOARD *gtk3timeboard = ( GTK3G3DUITIMEBOARD * ) calloc ( 0x01, memSize );

    if ( gtk3timeboard == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;

    }

    gtk3timeboard->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3timeboard;
}

/******************************************************************************/
void gtk3_g3duitimeboard_resize ( GTK3G3DUITIMEBOARD *gtk3timeboard,
                                  uint32_t              width,
                                  uint32_t              height ) {
    GdkRectangle gdkrec;

    g3duitimeboard_resize ( &gtk3timeboard->core,
                             width,
                             height );

    if ( gtk3timeboard->buttonsFixed ) {
        g3duirectangle_toGdkRectangle ( &gtk3timeboard->core.btnrec, &gdkrec );

        gtk_layout_move ( gtk3timeboard->layout,
               GTK_WIDGET(gtk3timeboard->buttonsFixed),
                          gdkrec.x,
                          gdkrec.y );

        gtk_widget_set_size_request ( GTK_WIDGET(gtk3timeboard->buttonsFixed),
                                      gdkrec.width,
                                      gdkrec.height );
    }

    /*** Timeline ***/
    if ( gtk3timeboard->core.timeline ) {
        GTK3G3DUITIMELINE *gtk3timeline = ( GTK3G3DUITIMELINE * )
                                           gtk3timeboard->core.timeline;

        g3duirectangle_toGdkRectangle ( &gtk3timeboard->core.linerec, &gdkrec );

        gtk_layout_move ( gtk3timeboard->layout,
               GTK_WIDGET(gtk3timeline->area), 
                          gdkrec.x,
                          gdkrec.y );

        gtk_widget_set_size_request ( GTK_WIDGET(gtk3timeline->area), 
                                      gdkrec.width,
                                      gdkrec.height  );
    }


    if ( gtk3timeboard->functionsFixed ) {
        g3duirectangle_toGdkRectangle ( &gtk3timeboard->core.funcrec, &gdkrec );

        gtk_layout_move ( gtk3timeboard->layout,
              GTK_WIDGET(gtk3timeboard->functionsFixed),
                          gdkrec.x,
                          gdkrec.y );

        gtk_widget_set_size_request ( GTK_WIDGET(gtk3timeboard->functionsFixed),
                                      gdkrec.width,
                                      gdkrec.height );
    }
}

/******************************************************************************/
static void createTimeline ( GTK3G3DUITIMEBOARD *gtk3timeboard ) {
    GTK3G3DUITIMELINE *gtk3timeline;

    gtk3timeline = gtk3_g3duitimeline_create ( gtk3timeboard->layout,
                               ( GTK3G3DUI * ) gtk3timeboard->core.gui,
                                                "Timeline" );

    gtk_layout_put ( GTK_LAYOUT(gtk3timeboard->layout), gtk3timeline->area, 0, 0 );

    gtk3timeboard->core.timeline = ( G3DUIOBJECTEDIT * ) gtk3timeline;
}

/******************************************************************************/
static void useMoveToolCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITIMEBOARD *gtk3timeboard = ( GTK3G3DUITIMEBOARD * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3timeboard->core.gui;

    g3duitimeline_setTool ( gtk3timeboard->core.timeline, TIME_MOVE_TOOL );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWTIMELINE );
}

/******************************************************************************/
static void usePanToolCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITIMEBOARD *gtk3timeboard = ( GTK3G3DUITIMEBOARD * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3timeboard->core.gui;

    g3duitimeline_setTool ( gtk3timeboard->core.timeline, TIME_PAN_TOOL );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWTIMELINE );
}

/******************************************************************************/
static void useScaleToolCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITIMEBOARD *gtk3timeboard = ( GTK3G3DUITIMEBOARD * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3timeboard->core.gui;

    g3duitimeline_setTool ( gtk3timeboard->core.timeline, TIME_SCALE_TOOL );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWTIMELINE );
}

/******************************************************************************/
static void createFunctions ( GTK3G3DUITIMEBOARD *gtk3timeboard ) {
    GtkFixed *fixed = ui_gtk_fixed_new ( CLASS_MAIN );

    gtk3timeboard->functionsFixed = fixed;

    gtk_widget_set_size_request ( GTK_WIDGET(fixed), 48, 24 );

    gtk_layout_put ( GTK_LAYOUT(gtk3timeboard->layout), 
                     GTK_WIDGET(fixed), 
                     0, 0 );


    gtk3timeboard->scaleButton = ui_createImageButton ( fixed,
                                                              gtk3timeboard,
                                                              TIMEBOARDRECORD,
                                                              CLASS_MAIN,
                                                              scale_keys_xpm,
                                                              0,  0, 24, 24,
                                                              useScaleToolCbk );

    gtk3timeboard->panButton = ui_createImageButton ( fixed,
                                                            gtk3timeboard,
                                                            TIMEBOARDRECORD,
                                                            CLASS_MAIN,
                                                            pick_keys_xpm,
                                                            24,  0, 24, 24,
                                                            usePanToolCbk );

    gtk_widget_show_all ( GTK_WIDGET(fixed) );
}

/******************************************************************************/
static void stopCbk ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUITIMEBOARD *gtk3timeboard = ( GTK3G3DUITIMEBOARD * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3timeboard->core.gui;
    uint64_t ret;

    ret = g3duitimeboard_stop ( &gtk3timeboard->core );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );
}

/******************************************************************************/
static void *play_t ( void *user_data ) {
    GTK3G3DUITIMEBOARD *gtk3timeboard = ( GTK3G3DUITIMEBOARD * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3timeboard->core.gui;
    G3DUI *gui = gtk3timeboard->core.gui;
    G3DSCENE *sce = gui->sce;

    if ( sce ) {
        float curframe = gui->curframe;

        /*** http://www.motifdeveloper.com/tips/tip6.html ***/
        while ( gui->playLock ) {
	        curframe += 1.0f;

            /*** skip sending event if animation has not completed yet ***/
            if ( ( gui->engine_flags & ONGOINGANIMATION ) == 0x00 ) {
                static GOTOFRAME gtf;

                gtf.action.type = ACTION_GOTOFRAME;
                gtf.action.gui  = gui;
                gtf.frame       = curframe;

                gtk3_g3duicom_requestActionFromMainThread ( gui, &gtf );
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
    }

    return NULL;
}

/******************************************************************************/
static void playCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITIMEBOARD *gtk3timeboard = ( GTK3G3DUITIMEBOARD * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3timeboard->core.gui;
    G3DUI *gui = gtk3timeboard->core.gui;

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
        pthread_create ( &gui->playthreadid, &attr, play_t, gtk3timeboard );

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
static void nextFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITIMEBOARD *gtk3timeboard = ( GTK3G3DUITIMEBOARD * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3timeboard->core.gui;
    G3DUI *gui = gtk3timeboard->core.gui;

    gui->curframe += 1;

    g3duitimeboard_gotoFrame ( &gtk3timeboard->core );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void prevFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITIMEBOARD *gtk3timeboard = ( GTK3G3DUITIMEBOARD * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3timeboard->core.gui;
    G3DUI *gui = gtk3timeboard->core.gui;

    gui->curframe -= 1;

    g3duitimeboard_gotoFrame ( &gtk3timeboard->core );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void zoomFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITIMEBOARD *gtk3timeboard = ( GTK3G3DUITIMEBOARD * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3timeboard->core.gui;

    g3duitimeline_incZoom ( gtk3timeboard->core.timeline, 0x02 );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWTIMELINE );
}

/******************************************************************************/
static void unzoomFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITIMEBOARD *gtk3timeboard = ( GTK3G3DUITIMEBOARD * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3timeboard->core.gui;

    g3duitimeline_decZoom ( gtk3timeboard->core.timeline, 0x02 );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWTIMELINE );
}

/******************************************************************************/
static void recordFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITIMEBOARD *gtk3timeboard = ( GTK3G3DUITIMEBOARD * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3timeboard->core.gui;

    g3duitimeboard_recordFrame ( &gtk3timeboard->core, 
                                     KEYPOSITION | 
                                     KEYROTATION | 
                                     KEYSCALING  /*|
                                     KEYDATA*/ );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWTIMELINE );
}

/******************************************************************************/
static void createButtons ( GTK3G3DUITIMEBOARD *gtk3timeboard ) {
    GtkFixed *fixed = ui_gtk_fixed_new ( CLASS_MAIN );

    gtk3timeboard->buttonsFixed = fixed;

    gtk_widget_set_size_request ( GTK_WIDGET(fixed), 168, 24 );

    gtk_layout_put ( GTK_LAYOUT(gtk3timeboard->layout),
                     GTK_WIDGET(fixed),
                     0, 0 );

    ui_createImageButton ( fixed,
                           gtk3timeboard,
                           TIMEBOARDPREV,
                           CLASS_MAIN,
                           prevframe_xpm,
                           0,  0, 24, 24,
                           prevFrameCbk );

    ui_createImageButton ( fixed,
                           gtk3timeboard,
                           TIMEBOARDSTOP,
                           CLASS_MAIN,
                           stopanim_xpm,
                           24,  0, 24, 24,
                           stopCbk );

    ui_createImageButton ( fixed,
                           gtk3timeboard,
                           TIMEBOARDPLAY,
                           CLASS_MAIN,
                           playanim_xpm,
                           48,  0, 24, 24,
                           playCbk );

    ui_createImageButton ( fixed,
                           gtk3timeboard,
                           TIMEBOARDNEXT,
                           CLASS_MAIN,
                           nextframe_xpm,
                           72,  0, 24, 24,
                           nextFrameCbk );

    ui_createImageButton ( fixed,
                           gtk3timeboard,
                           TIMEBOARDZOOM,
                           CLASS_MAIN,
                           zoomtime_xpm,
                           96,  0, 24, 24,
                           zoomFrameCbk );

    ui_createImageButton ( fixed,
                           gtk3timeboard,
                           TIMEBOARDUNZOOM,
                           CLASS_MAIN,
                           unzoomtime_xpm,
                           120,  0, 24, 24,
                           unzoomFrameCbk );

    ui_createImageButton ( fixed,
                           gtk3timeboard,
                           TIMEBOARDRECORD,
                           CLASS_MAIN,
                           record_xpm,
                           144,  0, 24, 24,
                           recordFrameCbk );

    gtk_widget_show_all ( GTK_WIDGET(fixed) );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITIMEBOARD *gtk3timeboard = ( GTK3G3DUITIMEBOARD * ) user_data;

    free ( gtk3timeboard );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, 
                      gpointer   user_data ) {
    GTK3G3DUITIMEBOARD *gtk3timeboard = ( GTK3G3DUITIMEBOARD * ) user_data;


}

/******************************************************************************/
GTK3G3DUITIMEBOARD *gtk3_g3duitimeboard_create ( GtkWidget *parent,
                                                 GTK3G3DUI *gtk3gui,
                                                 char      *name ) {
    GTK3G3DUITIMEBOARD *gtk3timeboard = gtk3_g3duitimeboard_new ( gtk3gui );
    GtkWidget    *layout = ui_gtk_layout_new ( CLASS_MAIN, NULL, NULL );

    gtk3timeboard->layout = GTK_LAYOUT(layout);

    gtk_widget_set_name ( layout, name );

    /*gtk_widget_add_events(GTK_WIDGET(layout), GDK_CONFIGURE);*/

    g_signal_connect ( G_OBJECT (layout), "realize", G_CALLBACK (Realize), gtk3timeboard );
    g_signal_connect ( G_OBJECT (layout), "destroy", G_CALLBACK (Destroy), gtk3timeboard );

    gtk_widget_show ( layout );

    createButtons   ( gtk3timeboard );
    createTimeline  ( gtk3timeboard );
    createFunctions ( gtk3timeboard );

    return gtk3timeboard;
}
