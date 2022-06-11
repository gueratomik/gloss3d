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

#define EDITBUCKETTOOLTOLERANCE "Tolerance"

/******************************************************************************/
static void setToleranceCbk  ( GtkWidget *widget, 
                               gpointer   user_data ) {
    int tolerance = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    M3DUI *mui = ( M3DUI * ) user_data;

    common_g3duibuckettooledit_setToleranceCbk ( mui, tolerance );
}

/******************************************************************************/
void updateBucketToolEdit ( GtkWidget *widget,
                            M3DUI     *mui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    M3DMOUSETOOL *tool = common_g3dui_getMouseTool ( mui->gui, BUCKETTOOL );
    M3DMOUSETOOLBUCKET *mtb = ( M3DMOUSETOOLBUCKET * ) tool;
    M3DSYSINFO *sysinfo = m3dsysinfo_get ( );

    /*** prevent a loop ***/
    mui->gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( GTK_IS_SPIN_BUTTON(child) ) {
            GtkSpinButton *spb = GTK_SPIN_BUTTON(child);

            if ( strcmp ( child_name, EDITBUCKETTOOLTOLERANCE   ) == 0x00 ) {
                M3DBUCKET *bkt = mtb->ltool.obj;

                gtk_spin_button_set_value ( spb, bkt->tolerance );
            }
        }

        children =  g_list_next ( children );
    }

    mui->gui->lock = 0x00;
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    M3DUI *mui = ( M3DUI * ) user_data;

    updateBucketToolEdit ( widget, mui );
}

/******************************************************************************/
GtkWidget *createBucketToolEdit ( GtkWidget        *parent, 
                                  M3DUI *mui,
                                  char             *name,
                                  gint              x,
                                  gint              y,
                                  gint              width,
                                  gint              height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *frm, *ptf, *fix;

    frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), frm, x, y );

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), mui );

    createIntegerText     ( frm, mui, EDITBUCKETTOOLTOLERANCE, 
                                  0, 256,
                                  0, 24, 96,  32, setToleranceCbk );

    gtk_widget_show ( frm );


    return frm;
}
