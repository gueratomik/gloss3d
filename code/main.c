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
/* Please avoid using global variables at all costs in this file, and never   */
/* forget this :                                                              */
/*                         Keep It Simple Stupid !                            */
/*                                                                            */
/******************************************************************************/

#include <stdio.h>
#include <stdlib.h>

/******************************************************************************/
#include <config.h>

#ifdef __linux__
    #include <g3dui_gtk3.h>
#endif

#ifdef __MINGW32__
    #include <g3dui_gtk3.h>
#endif

/******************************************************************************/
gboolean Delete( GtkWidget *widget,
                 GdkEvent  *event,
                 gpointer   data ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) data;

    if ( gtk3_exit ( gtk3gui ) == GTK_RESPONSE_YES ) return FALSE;

    return TRUE;
}

/******************************************************************************/
static gboolean Configure ( GtkWindow *window, 
                            GdkEvent  *event,
                            gpointer   data ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) data;
    GTK3G3DUIMAIN *gtk3main = gtk3gui->core.main;

/*printf("%d\n", event->configure.width, event->configure.height);*/

    if ( gtk_widget_get_realized ( GTK_WIDGET ( window ) ) ) {
        gtk3_g3duimain_resize ( &gtk3main->core, 
                                 event->configure.width, 
                                event->configure.height );
    }


    /*gtk_layout_set_size ( child, 
                          event->configure.width,
                          event->configure.height );*/

    return TRUE;
}

/******************************************************************************/
int main ( int argc, char *argv[] ) {
    static G3DUI gui;
    char *loadFile = NULL;
    char appname[0x100];

    G3DSYSINFO *g3dsysinfo = g3dsysinfo_get ( );
    M3DSYSINFO *m3dsysinfo = m3dsysinfo_get ( );
    int i;

    snprintf ( appname, 0x100, "%s %s", G3DUIAPPNAME, VERSION );

#ifdef __linux__
    for ( i = 0x01; i < argc; i++ ) {
        if ( strlen ( argv[i] ) > 0x01 ) {
            if ( argv[i][0x00] == '-' ) {
                switch ( argv[i][0x01] ) {
                    case 'd' :
                        g3dsysinfo->debug = 0x01;
                        m3dsysinfo->debug = 0x01;
                    break;
 
                    default :
                    break;
                }
            } else {
                loadFile = argv[i];
            }
        } else {
            loadFile = argv[i];
        }
    }
#endif

#ifdef HAVE_GTK3
    GtkWidget *window, *glossui;
    GtkWidget *button;
    GTK3G3DUI gtk3gui; /**** Gloss3D ****/
    GTK3G3DUIMAIN *gtk3main;

    memset ( &gtk3gui, 0x00, sizeof ( GTK3G3DUI ) );

    #ifdef __linux__
    /*** Support multi-threads. Must be called before any Xlib function ***/
    XInitThreads ( );
    #endif

    printf("starting %s\n", appname );

    gtk_init ( &argc, &argv );

    window  = ui_gtk_window_new ( CLASS_MAIN, GTK_WINDOW_TOPLEVEL );

    /*** Drawing area does not receive mous events by defaults ***/
    /*gtk_widget_set_events ( window, 
                            gtk_widget_get_events ( window ) |
                            GDK_STRUCTURE_MASK );*/

    gtk3gui.topWin = window;

    gtk_window_set_position ( GTK_WINDOW(window), GTK_WIN_POS_CENTER );

    gtk3main = gtk3_g3duimain_create (  window,
                                       &gtk3gui,
                                        "Main",
                                        0, 0,
                                        1280, 720, loadFile );


    gtk_container_add ( GTK_CONTAINER(window), gtk3main->layout );

    /*gtk_widget_show ( glossui );*/

    gtk_window_set_title ( GTK_WINDOW ( window ), appname );
    gtk_window_resize    ( GTK_WINDOW ( window ), 1280, 720 );

    g_signal_connect (window, "destroy"        , G_CALLBACK (gtk_main_quit), &gtk3gui );
    g_signal_connect (window, "configure-event", G_CALLBACK (Configure)    , &gtk3gui );
    g_signal_connect (window, "delete-event"   , G_CALLBACK (Delete)       , &gtk3gui );

    gtk_widget_show (window);

    gtk_main ();
#endif

    exit ( EXIT_SUCCESS );


    return 0x00;
}

