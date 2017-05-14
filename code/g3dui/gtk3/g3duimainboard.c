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
GtkWidget *createMainBoard ( GtkWidget *parent, G3DUI *gui,
                                                char *name,
                                                gint x,
                                                gint y,
                                                gint width,
                                                gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *tab = gtk_notebook_new ( );
    GtkWidget *objbrd, *matbrd;

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(tab), TRUE );

    gtk_widget_set_name ( tab, name );

    gtk_widget_size_allocate ( tab, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), tab, x, y );

    /*createMaterialBoard ( tab, &guw->gui, "Material Board", 0x00, 0x00, 0x140, 0x300 );
*/  objbrd = createObjectBoard   ( tab, gui, OBJECTBOARDNAME  , 0, 0, width, height );
    matbrd = createMaterialBoard ( tab, gui, MATERIALBOARDNAME, 0, 0, width, height );


    gtk_widget_show ( tab );


    return tab;
}
