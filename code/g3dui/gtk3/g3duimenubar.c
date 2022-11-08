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
            gtk3node->item = ui_gtk_check_menu_item_new_with_mnemonic ( node->class, node->name );

            if ( node->callback ) {
                g_signal_connect ( G_OBJECT ( gtk3node->item ), 
                                   "toggled", 
                                   menuItemCallback, 
                                   gtk3node );
            }

            gtk_widget_show ( gtk3node->item );
        break;

        case G3DUIMENUTYPE_PUSHBUTTON :
            gtk3node->item = ui_gtk_menu_item_new_with_mnemonic ( node->class, node->name );

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

            gtk3node->menu = ui_gtk_menu_bar_new ( node->class );

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

            gtk3node->menu = ui_gtk_menu_new ( node->class );

            gtk_widget_set_name ( gtk3node->menu, node->name );

            gtk3node->item = ui_gtk_menu_item_new_with_mnemonic ( node->class, node->name );

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

            /*gtk_widget_set_size_request ( gtk3node->item, 
                                          0x60,
                                          0x00 );*/
        } break;

        default :
        break;
    }

    if ( node->type == G3DUIMENUTYPE_SUBMENU  ) {
        int height = gtk_widget_get_allocated_height ( gtk3node->item );
        GdkRectangle gdkrec = { 0, 0, 0x60, height };

        /*gtk_widget_set_halign ( node->item, GTK_ALIGN_CENTER );*/

    /*gtk_widget_size_allocate ( gtk3node->item, &gdkrec );*/

        gtk_widget_set_size_request ( gtk3node->item, 0x60, 0x00 );
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
