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
GTK3M3DUI *gtk3_m3dui_create ( GTK3G3DUI *gtk3gui ) {
    GTK3M3DUI* gtk3mui = ( GTK3M3DUI * ) calloc ( 0x01, sizeof ( GTK3M3DUI ) );

    if ( gtk3mui == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    m3dui_init ( &gtk3mui->core, &gtk3gui->core );


    return gtk3mui;
}

/******************************************************************************/
void gtk3_m3dui_display ( GTK3M3DUI *gtk3mui ) {
    GtkWidget *win = ui_gtk_window_new ( CLASS_MAIN, GTK_WINDOW_TOPLEVEL );
    GTK3M3DUIMAIN *gtk3main = gtk3_m3duimain_create ( win,
                                                      gtk3mui,
                                                      "MakeUp\n" );
    if ( gtk3mui->core.main == NULL ) {
        gtk3mui->core.main = ( M3DUIMAIN * ) gtk3main;

        gtk_container_add ( win, gtk3main->layout );

        gtk_window_resize ( win, 1024, 576 );

        gtk_widget_show ( win );
    }
}
