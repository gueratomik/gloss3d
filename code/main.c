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

/****************** mtrace and muntrace for debugging purpose *****************/
#ifdef __linux__
    #include <mcheck.h>
#endif

/******************************************************************************/
#include <config.h>

#ifdef __linux__
/******************************************************************************/
    #ifdef WITH_MOTIF
        #include <g3dui_motif.h>
    #endif

/******************************************************************************/
    #ifdef WITH_GTK3
        #include <g3dui_gtk3.h>
    #endif
#endif

#ifdef __MINGW32__
    #include <windows.h>
    #include <g3dui_gtk3.h>

char * strsep(char **sp, char *sep) {
    char *p, *s;

    if (sp == NULL || *sp == NULL || **sp == '\0') return(NULL);

    s = *sp;
    p = s + strcspn(s, sep);

    if (*p != '\0') *p++ = '\0';

    *sp = p;

    return(s);
}

#endif

/******************************************************************************/
#ifdef __linux__
int main ( int argc, char *argv[] ) {
#endif

#ifdef __MINGW32__
int CALLBACK WinMain ( HINSTANCE hInstance, HINSTANCE hPrevInstance,
                                            LPSTR lpCmdLine,
                                            int nCmdShow ) {
    int argc = 0x00;
    char **argv = NULL;
#endif
    char *loadFile = ( argc == 0x02 ) ? argv[0x01] : NULL;
    char appname[0x100];

    snprintf ( appname, 0x100, "%s %s", G3DUIAPPNAME, VERSION );

#ifdef WITH_MOTIF
    XtAppContext app;
    Widget top;

    /*** Support multi-threads. Must be called before any Xlib function ***/
    XInitThreads ( );

    XtSetLanguageProc((XtAppContext) 0, (XtLanguageProc) 0, (XtPointer) 0) ;

    XtToolkitInitialize ( );

    top = XtVaAppInitialize ( &app, appname, NULL, 0x00,
                              &argc, argv, NULL, NULL );

    XtVaCreateManagedWidget ( "UI", guiWidgetClass, top,
                              XtNwidth , 1024,
                              XtNheight, 576,
                              XtNloadFile, loadFile,
                              /*XmNforeground, gui->foreground.pixel,*/
       /*  Does not work */   /*XmNfontList, gui->fontlist,*/
                              NULL );

    XtRealizeWidget ( top );


    XtAppMainLoop ( app );
#endif

#ifdef WITH_GTK3
    GtkWidget *window, *glossui;
    GtkWidget *button;

    #ifdef __linux__
    /*** Support multi-threads. Must be called before any Xlib function ***/
    XInitThreads ( );
    #endif

    gtk_init ( &argc, &argv );

    window  = gtk_window_new (GTK_WINDOW_TOPLEVEL);

    gtk_window_set_position ( GTK_WINDOW(window), GTK_WIN_POS_CENTER );

    glossui = gtk_glossui_new ( );

    gtk_container_add ( GTK_CONTAINER(window), glossui );

    gtk_widget_show ( glossui );

    gtk_window_set_title ( GTK_WINDOW ( window ), appname );
    gtk_window_resize    ( GTK_WINDOW ( window ), 1024, 576 );

    g_signal_connect (window, "destroy"     , G_CALLBACK (gtk_main_quit), NULL);
    g_signal_connect (window, "delete-event", G_CALLBACK (g3dui_exitEventCbk), &((GtkGlossUI*)glossui)->gui );

    gtk_widget_show (window);
  
    gtk_main ();
#endif


    exit ( EXIT_SUCCESS );


    return 0x00;
}
