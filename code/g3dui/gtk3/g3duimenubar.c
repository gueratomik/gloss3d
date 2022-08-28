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
static void g3duirenderwindow_saveJPGCbk ( GtkWidget *widget, 
                                               gpointer   user_data ) {
    G3DUIRENDERWINDOW *grw = ( G3DUIRENDERWINDOW * ) user_data;
    GTK3G3DUI *gtk3g3dui = ( GTK3G3DUI * ) grw->gui;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Save image ...",
                                           GTK_WINDOW(gtk3g3dui->topWin),
                        /*** from ristretto-0.3.5/src/main_window.c ***/
                                           GTK_FILE_CHOOSER_ACTION_SAVE,
                                           "_Cancel", 
                                           GTK_RESPONSE_CANCEL,
                                           "_Open", 
                                           GTK_RESPONSE_OK,
                                           NULL );

    gtk_file_chooser_set_do_overwrite_confirmation ( GTK_FILE_CHOOSER(dialog),
                                                     TRUE );

    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_OK ) {
        GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
        char           *filename = gtk_file_chooser_get_filename ( chooser );
        G3DUIRENDERPROCESS *rps = grw->rps;

        if ( rps ) {
            g3duirenderprocess_savejpg ( rps, filename );
        }

        g_free    ( filename );
    }

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
static void exitRenderWindowCbk ( G3DUIMENU *menu, void *data ) {
/***
    G3DUIRENDERWINDOW *grw = ( G3DUIRENDERWINDOW * ) user_data;

    gtk_widget_destroy ( grw->topLevel );
*/
}


/******************************************************************************/
/******************************************************************************/
GtkWidget *createRenderWindowMenuBar ( GtkWidget         *parent,
                                       G3DUIRENDERWINDOW *rwn,
                                       char              *name,
                                       gint               x,
                                       gint               y,
                                       gint               width,
                                       gint               height ) {
#ifdef TODO
    GdkRectangle gdkrec = { x, y, width, height };

    parseMenu_r ( &renderwindowrootnode, NULL, rwn );

    gtk_widget_size_allocate ( renderwindowrootnode.menu, &gdkrec );
    gtk_fixed_put ( GTK_FIXED(parent), renderwindowrootnode.menu, x, y );

    gtk_widget_show ( renderwindowrootnode.menu );


    return renderwindowrootnode.menu;
#endif
}


/******************************************************************************/
/******************************************************************************/
GtkWidget *createTimeContextMenu ( GtkWidget     *parent,
                                   G3DUITIMELINE *tim,
                                   char          *name,
                                   gint           width,
                                   gint           height ) {
#ifdef TODO
    GdkRectangle gdkrec = { 0, 0, width, height };

    gtk3_parseMenu_r ( &time_menu, tim );

    gtk_widget_size_allocate ( time_menu.menu, &gdkrec );

    gtk_widget_show ( time_menu.menu );


    return time_menu.menu;
#endif
}

/******************************************************************************/
/******************************************************************************/
GtkWidget *createOptionMenu ( GtkWidget *parent,
                              GtkWidget *gvw,
                              char      *name,
                              gint       x,
                              gint       y,
                              gint       width,
                              gint       height ) {
#ifdef TODO
    GdkRectangle gdkrec = { x, y, width, height };

    parseMenu_r ( &viewrootnode, NULL, gvw );

    gtk_widget_size_allocate ( viewrootnode.menu, &gdkrec );
    gtk_fixed_put ( GTK_FIXED(parent), viewrootnode.menu, x, y );

    gtk_widget_show ( viewrootnode.menu );


    return viewrootnode.menu;
#endif
}

/******************************************************************************/
GtkWidget *createMaterialMenuBar ( GtkWidget *parent, 
                                   G3DUI     *gui,
                                   char      *name,
                                   gint       x,
                                   gint       y,
                                   gint       width,
                                   gint       height ) {
#ifdef TODO
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *bar = gtk_menu_bar_new ( );

    gtk_widget_set_name ( bar, name );

    /*gtk_widget_size_allocate ( bar, &gdkrec );*/
    gtk_widget_set_size_request ( bar, width, height );

    parseMenu_r ( &matrootnode, NULL, gui );

    gtk_widget_size_allocate ( matrootnode.menu, &gdkrec );
    gtk_fixed_put ( GTK_FIXED(parent), matrootnode.menu, x, y );

    gtk_widget_show ( bar );


    return bar;
#endif
}

/******************************************************************************/
GtkWidget *createObjectsMenuBar ( GtkWidget *parent, 
                                  G3DUI     *gui,
                                  char      *name,
                                  gint       x,
                                  gint       y,
                                  gint       width,
                                  gint       height ) {
#ifdef TODO
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *bar = gtk_menu_bar_new ( );

    gtk_widget_set_name ( bar, name );

    /*gtk_widget_size_allocate ( bar, &gdkrec );*/
    gtk_widget_set_size_request ( bar, width, height );

    parseMenu_r ( &objrootnode, NULL, gui );

    gtk_widget_size_allocate ( objrootnode.menu, &gdkrec );
    gtk_fixed_put ( GTK_FIXED(parent), objrootnode.menu, x, y );

    gtk_widget_show ( bar );


    return bar;
#endif
}

/******************************************************************************/
/******************************************************************************/
/******************************************************************************/
void g3dui_aboutCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    gchar *authors[] = { "Gary GABRIEL", NULL };

    gtk_show_about_dialog ( NULL,
                           "authors", authors,
                           "program-name", "Gloss3D",
                           "license", GLOSS3DLICENSE,
                           "comments", "Graphics by a Lightweight Open-Source Software",
                           "version", VERSION,
                           "copyright", "2012-2021 The Gloss3D Team",
                           "website", "http://www.gloss3d.net",
                           "title", ("About Gloss3D"),
                           NULL );
}


/******************************************************************************/
/******************************************************************************/
static void resizeChannelImageCbk ( G3DUIMENU *menu, 
                                        void      *data ) {
#ifdef TODO
    M3DUI *mui = ( M3DUI * ) user_data;
    G3DCHANNEL *chn = common_m3dui_getWorkingChannel ( mui );

    if ( chn ) {
        GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );

        createChannelImage ( dial, 
                             mui, 
                             chn,
                             0x01,
                             "Channel Image", 0, 0, 200, 96 );

        gtk_widget_show ( dial );
    }
#endif

    return 0x00;
}

/******************************************************************************/
static void createChannelImageCbk ( G3DUIMENU *menu, 
                                        void      *data ) {
#ifdef TODO
    M3DUI *mui = ( M3DUI * ) user_data;
    G3DCHANNEL *chn = common_m3dui_getWorkingChannel ( mui );

    if ( chn ) {
        GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );

        createChannelImage ( dial, 
                             lui, 
                             chn,
                             0x00,
                             "Channel Image", 0, 0, 200, 96 );

        gtk_widget_show ( dial );
    }
#endif
}


/******************************************************************************/
GtkWidget *createUVMenuBar ( GtkWidget *parent,  
                             M3DUI      *lui,
                             char      *name,
                             gint       x,
                             gint       y,
                             gint       width,
                             gint       height ) {
#ifdef TODO
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *bar = gtk_menu_bar_new ( );

    gtk_widget_set_name ( bar, name );

    gtk_widget_size_allocate ( bar, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), bar, x, y );

    parseMenu_r ( &uvrootnode, NULL, lui );

    gtk_widget_size_allocate ( uvrootnode.menu, &gdkrec );
    gtk_fixed_put ( GTK_FIXED(parent), uvrootnode.menu, x, y );

    /*createUVHelpMenu ( bar, gui, "HelpMenu" , 90 );*/

    gtk_widget_show ( bar );
#endif
}

/******************************************************************************/
static void renderSettingsCbk ( G3DUIMENU *menu, 
                                    void      *data ) {
#ifdef TODO
    G3DUI *gui = ( G3DUI * ) user_data;
    GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );

    /*** For some reason, GtkSpinButtons calls its callbacks ***/
    /*** when being realized. With this trick I bypass that. ***/
    /*** Callbacks will return prematurely if gui->lock == 0x01 ***/
    gui->lock = 0x01;

    createRenderEdit ( dial, gui, "RENDEREDIT", 0, 0, 480, 340 );

    gtk_widget_show ( dial );

    gui->lock = 0x00;
#endif
}


/******************************************************************************/
static void getObjectStatsCbk ( G3DUIMENU *menu, 
                                    void      *data ) {
#ifdef TODO
    G3DUI *gui = ( G3DUI * ) user_data;
    char buffer[0x200] = { 0 };
    GtkWidget *dialog;

    g3dui_getObjectStatsCbk ( menu->gui, buffer, 0x200 );

    if ( strlen ( buffer ) ) {
        dialog = gtk_message_dialog_new ( NULL,
                                          GTK_DIALOG_MODAL,
                                          GTK_MESSAGE_INFO,
                                          GTK_BUTTONS_NONE,
                                          buffer,
                                          NULL );

        gtk_dialog_run ( GTK_DIALOG ( dialog ) );
        gtk_widget_destroy ( dialog );
    }
#endif
}


/******************************************************************************/
gboolean g3dui_exitEventCbk ( GtkWidget *widget, 
                              GdkEvent  *event, 
                              gpointer   user_data ) {
#ifdef TODO
    G3DUI *gui = ( G3DUI * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    GtkWidget *dialog;
    gint       res;

    if ( g3dui_saveAlteredImages ( gui ) ) {
        return TRUE;
    }

    dialog = gtk_message_dialog_new ( NULL,
                                      GTK_DIALOG_MODAL,
                                      GTK_MESSAGE_QUESTION,
                                      GTK_BUTTONS_YES_NO,
                                      "Leave Gloss3D ?" );


    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_YES ) {
        g3dui_exitokcbk ( gui );

        return FALSE;
    }

    gtk_widget_destroy ( dialog );
#endif
    return TRUE;
}

/******************************************************************************/
void g3dui_exitcbk ( GtkWidget *widget, 
                         gpointer   user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_message_dialog_new ( NULL,
                                      GTK_DIALOG_MODAL,
                                      GTK_MESSAGE_QUESTION,
                                      GTK_BUTTONS_YES_NO,
                                      "Leave Gloss3D ?" );


    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_YES ) {
        g3dui_exitokcbk ( gui );
    }

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
void g3dui_exportfilecbk ( GtkWidget *widget, gpointer user_data ) {
    const gchar *filedesc = gtk_widget_get_name  ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Export file ...",
                                           GTK_WINDOW(gtk3gui->topWin),
                        /*** from ristretto-0.3.5/src/main_window.c ***/
                                           GTK_FILE_CHOOSER_ACTION_SAVE,
                                           "_Cancel", 
                                           GTK_RESPONSE_CANCEL,
                                           "_Open", 
                                           GTK_RESPONSE_OK,
                                           NULL );

    gtk_file_chooser_set_do_overwrite_confirmation ( GTK_FILE_CHOOSER(dialog),
                                                     TRUE );

    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_OK ) {
        GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
        const char *filename = gtk_file_chooser_get_filename ( chooser );

        g3dui_exportfileokcbk ( gui, filedesc, filename );

        g_free    ( ( gpointer ) filename );
    }

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
void g3dui_importfilecbk ( GtkWidget *widget, gpointer user_data ) {
    const gchar *filedesc = gtk_widget_get_name  ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    GtkWidget *dialog;
    gint       res;

    dialog = gtk_file_chooser_dialog_new ( "Import file ...",
                                           GTK_WINDOW(gtk3gui->topWin),
                        /*** from ristretto-0.3.5/src/main_window.c ***/
                                           GTK_FILE_CHOOSER_ACTION_OPEN,
                                           "_Cancel", 
                                           GTK_RESPONSE_CANCEL,
                                           "_Open", 
                                           GTK_RESPONSE_OK,
                                           NULL );


    res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

    if ( res == GTK_RESPONSE_OK ) {
        GtkFileChooser *chooser  = GTK_FILE_CHOOSER ( dialog );
        const char *filename = gtk_file_chooser_get_filename ( chooser );

        g3dui_importfileokcbk ( gui, filedesc, filename );

        g_free    ( ( gpointer ) filename );
    }

    gtk_widget_destroy ( dialog );
}

/******************************************************************************/
static void menuItemCallback ( GtkWidget *widget, 
                               gpointer   user_data ) {
    GTK3G3DUIMENU *gtk3node = ( GTK3G3DUIMENU * ) user_data;
    G3DUI *gui = gtk3node->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;

    /*** prevents a loop ***/
    if ( gui->lock ) return;

    gtk3_setHourGlass ( gtk3gui );

    if ( gtk3node->core.callback ) {
        uint64_t ret = gtk3node->core.callback ( &gtk3node->core, 
                                                  gtk3node->core.data );

        gtk3_interpretUIReturnFlags ( gtk3gui, ret );
    }

    gtk3_unsetHourGlass ( gtk3gui );
}

/******************************************************************************/
GTK3G3DUIMENU *gtk3_g3duimenu_parse_r ( G3DUIMENU *node,
                                        G3DUI     *gui,
                                        void      *data ) {
    uint32_t structSize = sizeof ( GTK3G3DUIMENU ); 
    GTK3G3DUIMENU *gtk3node = ( GTK3G3DUIMENU * ) calloc ( 0x01, structSize );
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;

    memcpy ( &gtk3node->core, node, sizeof ( G3DUIMENU ) );
    
    gtk3node->core.data = data;
    gtk3node->core.gui  = gui;

    switch ( node->type ) {
        case G3DUIMENUTYPE_SEPARATOR :
            gtk3node->item = gtk_separator_menu_item_new ( );

            gtk_widget_show ( gtk3node->item );
        break;

        case G3DUIMENUTYPE_TOGGLEBUTTON :
            gtk3node->item = gtk_check_menu_item_new_with_mnemonic ( node->name );

            if ( node->callback ) {
                g_signal_connect ( G_OBJECT ( gtk3node->item ), 
                                   "toggled", 
                                   menuItemCallback, 
                                   gtk3node );
            }

            gtk_widget_show ( gtk3node->item );
        break;

        case G3DUIMENUTYPE_PUSHBUTTON :
            gtk3node->item = gtk_menu_item_new_with_mnemonic ( node->name );

            if ( node->callback ) {
                g_signal_connect ( G_OBJECT ( gtk3node->item ), 
                                   "activate", 
                                   menuItemCallback, 
                                   gtk3node );
            }

            gtk_widget_show ( gtk3node->item );
        break;

        case G3DUIMENUTYPE_MENUBAR : {
            uint32_t i = 0x00;

            gtk3node->menu = gtk_menu_bar_new ( );

            gtk_widget_set_name ( gtk3node->menu, node->name );

            while ( node->nodes[i] != NULL ) {
                GTK3G3DUIMENU *child = gtk3_g3duimenu_parse_r ( node->nodes[i],
                                                                gui,
                                                                data );

                gtk_menu_shell_append ( GTK_MENU_SHELL ( gtk3node->menu ), 
                                        child->item );

                list_insert ( &gtk3node->core.lchildren, child );

                i++;
            }

            gtk_widget_show ( gtk3node->menu );
        } break;

        case G3DUIMENUTYPE_SUBMENU : {
            uint32_t i = 0x00;

            gtk3node->menu = gtk_menu_new ( );

            gtk_widget_set_name ( gtk3node->menu, node->name );

            gtk3node->item = gtk_menu_item_new_with_mnemonic ( node->name );

            gtk_menu_item_set_submenu ( GTK_MENU_ITEM ( gtk3node->item ), gtk3node->menu );

            while ( node->nodes[i] != NULL ) {
                GTK3G3DUIMENU *child = gtk3_g3duimenu_parse_r ( node->nodes[i],
                                                                gui,
                                                                data );

                gtk_menu_shell_append ( GTK_MENU_SHELL ( gtk3node->menu ), 
                                        child->item );

                list_insert ( &gtk3node->core.lchildren, child );

                i++;
            }

            gtk_widget_show ( gtk3node->menu );
            gtk_widget_show ( gtk3node->item );

            gtk_widget_set_size_request ( gtk3node->menu, 0x60, 24 );
        } break;

        default :
        break;
    }

    if ( node->type == G3DUIMENUTYPE_SUBMENU  ) {
        int height = gtk_widget_get_allocated_height ( gtk3node->item );
        GdkRectangle gdkrec = { 0, 0, 0x60, height };

        /*gtk_widget_set_halign ( node->item, GTK_ALIGN_CENTER );*/

    /*gtk_widget_size_allocate ( gtk3node->item, &gdkrec );*/

        gtk_widget_set_size_request ( gtk3node->item, 0x60, height );
    }

    /*gtk_widget_set_name ( gtk3node->item, node->name );*/
    /*gtk_widget_show ( gtk3node->item );*/


    return gtk3node;
}

/******************************************************************************/
void gtk3_g3duimenu_update_r ( GTK3G3DUIMENU *gtk3node ) {
    G3DUI *gui = ( G3DUI * ) gtk3node->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    uint32_t condret = 0x00;

    gui->lock = 0x01;

    if ( gtk3node->item ) {
        /*** reset to default state ***/
        gtk_widget_set_sensitive ( gtk3node->item, TRUE  );

        if ( gtk3node->core.condition ) {
            condret = gtk3node->core.condition ( gtk3node, 
                                                 gtk3node->core.data );

            if ( condret & MENU_CONDITION_SENSITIVE ) {
                gtk_widget_set_sensitive ( gtk3node->item, TRUE  );
            } else {
                gtk_widget_set_sensitive ( gtk3node->item, FALSE );
            }
        }
    }

    switch ( gtk3node->core.type ) {
        case G3DUIMENUTYPE_SEPARATOR :
        break;

        case G3DUIMENUTYPE_TOGGLEBUTTON :
            if ( condret & MENU_CONDITION_ACTIVE ) {
                gtk_check_menu_item_set_active ( gtk3node->item, TRUE  );
            } else {
                gtk_check_menu_item_set_active ( gtk3node->item, FALSE );
            }
        break;

        case G3DUIMENUTYPE_PUSHBUTTON :
        break;

        case G3DUIMENUTYPE_MENUBAR :
        case G3DUIMENUTYPE_SUBMENU : {
            LIST *ltmpchildren = gtk3node->core.lchildren;

            while ( ltmpchildren ) {
                GTK3G3DUIMENU *child = ( GTK3G3DUIMENU * ) ltmpchildren->data;

                gtk3_g3duimenu_update_r ( child );

                ltmpchildren = ltmpchildren->next;
            }
        } break;

        default :
        break;
    }

    gui->lock = 0x00;
}
