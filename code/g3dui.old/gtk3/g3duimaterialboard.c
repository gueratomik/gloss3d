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
GtkWidget *createMaterialBoard ( GtkWidget *parent, G3DUI *gui,
                                                    char *name,
                                                    gint x,
                                                    gint y,
                                                    gint width,
                                                    gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *label = gtk_label_new ( name );
    GtkWidget *curedit, *objlist, *tab, *frm, *menu;
    uint32_t menuheight;

    frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &gdkrec );

    menu = createMaterialMenuBar ( frm, gui, "MENU", 0, 0, width, 32 );

    /*menuheight = gtk_widget_get_allocated_height ( menu );*/

    createMaterialList     ( frm, gui, "Materials"    , 0,  32, 320, 320 );
    createMaterialEdit     ( frm, gui, "Material Edit", 0, 352, 304, 168 );

    gtk_notebook_append_page ( GTK_NOTEBOOK(parent), frm, label );


    gtk_widget_show_all ( frm );


    return frm;
}
