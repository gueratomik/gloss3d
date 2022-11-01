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
static GTK3M3DUIMOUSETOOLEDIT *gtk3_m3duimousetooledit_new ( GTK3M3DUI *gtk3mui ) {
    uint32_t memsize = sizeof ( GTK3M3DUIMOUSETOOLEDIT );
    GTK3M3DUIMOUSETOOLEDIT *gtk3mtledit = calloc ( 0x01, memsize );

    if ( gtk3mtledit == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3mtledit->core.mui = ( M3DUI * ) gtk3mui;


    return gtk3mtledit; 
}

/******************************************************************************/
void gtk3_m3duimousetooledit_update ( GTK3M3DUIMOUSETOOLEDIT *gtk3mtledit ) {
    GTK3M3DUIBUCKETTOOLEDIT *buckedit = ( GTK3M3DUIBUCKETTOOLEDIT * ) gtk3mtledit->core.buckedit;
    GTK3M3DUIPENTOOLEDIT    *penedit  = ( GTK3M3DUIPENTOOLEDIT    * ) gtk3mtledit->core.penedit;
    M3DUI *mui = gtk3mtledit->core.mui;
    G3DUIMOUSETOOL *mtl = mui->curmou;

    if ( buckedit ) gtk_widget_hide ( buckedit->fixed );
    if ( penedit  ) gtk_widget_hide ( penedit->fixed  );

    if ( mtl ) {
        G3DMOUSETOOL *tool = mtl->tool;

        if ( tool ) {
            if ( strcmp ( tool->name, BUCKETTOOL ) == 0x00 ) {
                gtk3_m3duibuckettooledit_update ( buckedit );

                gtk_widget_show ( buckedit->fixed );
            }

            if ( strcmp ( tool->name, PENTOOL ) == 0x00 ) {
                gtk3_m3duipentooledit_update ( penedit );

                gtk_widget_show ( penedit->fixed );
            }
        }
    }
}

/******************************************************************************/
static void createBucketToolEdit ( GTK3M3DUIMOUSETOOLEDIT *gtk3mtledit ) {
    GTK3M3DUI *gtk3mui = ( GTK3M3DUI * ) gtk3mtledit->core.mui;
    GTK3M3DUIBUCKETTOOLEDIT *gtk3buckedit;

    gtk3buckedit = gtk3_m3duibuckettooledit_create ( gtk3mtledit->mousetoolFixed,
                                                     gtk3mui,
                                                     "Bucket Tool" );

    gtk_fixed_put ( GTK_FIXED(gtk3mtledit->mousetoolFixed), gtk3buckedit->fixed, 0, 0 );

    gtk3mtledit->core.buckedit = ( M3DUIBUCKETTOOLEDIT * ) gtk3buckedit;
}

/******************************************************************************/
static void createPenToolEdit ( GTK3M3DUIMOUSETOOLEDIT *gtk3mtledit ) {
    GTK3M3DUI *gtk3mui = ( GTK3M3DUI * ) gtk3mtledit->core.mui;
    GTK3M3DUIPENTOOLEDIT *gtk3penedit;

    gtk3penedit = gtk3_m3duipentooledit_create ( gtk3mtledit->mousetoolFixed,
                                                 gtk3mui,
                                                 "Pen Tool" );

    gtk_fixed_put ( GTK_FIXED(gtk3mtledit->mousetoolFixed), gtk3penedit->fixed, 0, 0 );

    gtk3mtledit->core.penedit = ( M3DUIPENTOOLEDIT * ) gtk3penedit;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3M3DUIMOUSETOOLEDIT *gtk3mtledit = ( GTK3M3DUIMOUSETOOLEDIT * ) user_data;

    free ( gtk3mtledit );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3M3DUIMOUSETOOLEDIT *gtk3mtledit = ( GTK3M3DUIMOUSETOOLEDIT * ) user_data;

    gtk3_m3duimousetooledit_update ( gtk3mtledit );
}

/******************************************************************************/
GTK3M3DUIMOUSETOOLEDIT *gtk3_m3duimousetooledit_create ( GtkWidget *parent, 
                                                         GTK3M3DUI *gtk3mui,
                                                         char      *name ) {
    GTK3M3DUIMOUSETOOLEDIT *gtk3mtledit = gtk3_m3duimousetooledit_new ( gtk3mui );

    gtk3mtledit->scrolled       = ui_gtk_scrolled_window_new ( CLASS_MAIN, NULL, NULL );
    gtk3mtledit->mousetoolFixed = ui_gtk_fixed_new ( CLASS_MAIN );

#if GTK_CHECK_VERSION(3,8,0)
    gtk_container_add ( GTK_CONTAINER(gtk3mtledit->scrolled), gtk3mtledit->mousetoolFixed );
#else
    gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW(gtk3mtledit->scrolled), gtk3objedit->mousetoolFixed );
#endif

    g_signal_connect ( G_OBJECT (gtk3mtledit->scrolled), "realize", G_CALLBACK (Realize), gtk3mtledit );
    g_signal_connect ( G_OBJECT (gtk3mtledit->scrolled), "destroy", G_CALLBACK (Destroy), gtk3mtledit );

    /*** This is type dependent: hidden if not of ***/
    /*** selected mousetool type showed otherwise.   ***/

    createBucketToolEdit ( gtk3mtledit );
    createPenToolEdit ( gtk3mtledit );

    gtk_widget_show ( gtk3mtledit->mousetoolFixed );
    gtk_widget_show ( gtk3mtledit->scrolled );


    return gtk3mtledit;
}
