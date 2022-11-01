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
static GTK3G3DUIMOUSETOOLEDIT *gtk3_g3duimousetooledit_new ( GTK3G3DUI *gtk3gui ) {
    uint32_t memsize = sizeof ( GTK3G3DUIMOUSETOOLEDIT );
    GTK3G3DUIMOUSETOOLEDIT *gtk3mtledit = calloc ( 0x01, memsize );

    if ( gtk3mtledit == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3mtledit->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3mtledit; 
}

/******************************************************************************/
void gtk3_g3duimousetooledit_update ( GTK3G3DUIMOUSETOOLEDIT *gtk3mtledit ) {
    GTK3G3DUIPICKTOOLEDIT *pickedit = ( GTK3G3DUIPICKTOOLEDIT * ) gtk3mtledit->core.pickedit;
    GTK3G3DUICUTMESHTOOLEDIT *cmedit = ( GTK3G3DUICUTMESHTOOLEDIT * ) gtk3mtledit->core.cutmeshedit;
    GTK3G3DUISCULPTTOOLEDIT *sculptedit = ( GTK3G3DUISCULPTTOOLEDIT * ) gtk3mtledit->core.sculptedit;
    G3DUI *gui = gtk3mtledit->core.gui;
    G3DUIMOUSETOOL *mtl = gui->curmou;

    if ( pickedit   ) gtk_widget_hide ( pickedit->fixed   );
    if ( cmedit     ) gtk_widget_hide ( cmedit->fixed     );
    if ( sculptedit ) gtk_widget_hide ( sculptedit->fixed );

    if ( mtl ) {
        G3DMOUSETOOL *tool = mtl->tool;

        if ( tool ) {
            if ( strcmp ( tool->name, PICKTOOL ) == 0x00 ) {
                gtk3_g3duipicktooledit_update ( pickedit );

                gtk_widget_show ( pickedit->fixed );
            }

            if ( strcmp ( tool->name, CUTMESHTOOL ) == 0x00 ) {
                gtk3_g3duicutmeshtooledit_update ( cmedit );

                gtk_widget_show ( cmedit->fixed );
            }

            if ( strcmp ( tool->name, INFLATETOOL ) == 0x00 ) {
                gtk3_g3duisculpttooledit_update ( sculptedit );

                gtk_widget_show ( sculptedit->fixed );
            }

            if ( strcmp ( tool->name, CREASETOOL ) == 0x00 ) {
                gtk3_g3duisculpttooledit_update ( sculptedit );

                gtk_widget_show ( sculptedit->fixed );
            }

            if ( strcmp ( tool->name, UNSCULPTTOOL ) == 0x00 ) {
                gtk3_g3duisculpttooledit_update ( sculptedit );

                gtk_widget_show ( sculptedit->fixed );
            }
        }
    }
}

/******************************************************************************/
static void createSculptToolEdit ( GTK3G3DUIMOUSETOOLEDIT *gtk3mtledit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3mtledit->core.gui;
    GTK3G3DUISCULPTTOOLEDIT *gtk3sculptedit;

    gtk3sculptedit = gtk3_g3duisculpttooledit_create ( gtk3mtledit->mousetoolFixed,
                                                   gtk3gui,
                                                   "Sculpt Tool" );

    gtk_fixed_put ( GTK_FIXED(gtk3mtledit->mousetoolFixed), gtk3sculptedit->fixed, 0, 0 );

    gtk3mtledit->core.sculptedit = ( GTK3G3DUISCULPTTOOLEDIT * ) gtk3sculptedit;
}

/******************************************************************************/
static void createPickToolEdit ( GTK3G3DUIMOUSETOOLEDIT *gtk3mtledit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3mtledit->core.gui;
    GTK3G3DUIPICKTOOLEDIT *gtk3pickedit;

    gtk3pickedit = gtk3_g3duipicktooledit_create ( gtk3mtledit->mousetoolFixed,
                                                   gtk3gui,
                                                   "Pick Tool" );

    gtk_fixed_put ( GTK_FIXED(gtk3mtledit->mousetoolFixed), gtk3pickedit->fixed, 0, 0 );

    gtk3mtledit->core.pickedit = ( GTK3G3DUIPICKTOOLEDIT * ) gtk3pickedit;
}

/******************************************************************************/
static void createCutMeshToolEdit ( GTK3G3DUIMOUSETOOLEDIT *gtk3mtledit ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3mtledit->core.gui;
    GTK3G3DUICUTMESHTOOLEDIT *gtk3cmedit;

    gtk3cmedit = gtk3_g3duicutmeshtooledit_create ( gtk3mtledit->mousetoolFixed,
                                                    gtk3gui,
                                                    "Cut Mesh Tool" );

    gtk_fixed_put ( GTK_FIXED(gtk3mtledit->mousetoolFixed), gtk3cmedit->fixed, 0, 0 );

    gtk3mtledit->core.cutmeshedit = ( GTK3G3DUICUTMESHTOOLEDIT * ) gtk3cmedit;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIMOUSETOOLEDIT *gtk3mtledit = ( GTK3G3DUIMOUSETOOLEDIT * ) user_data;

    free ( gtk3mtledit );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIMOUSETOOLEDIT *gtk3mtledit = ( GTK3G3DUIMOUSETOOLEDIT * ) user_data;

    gtk3_g3duimousetooledit_update ( gtk3mtledit );
}

/******************************************************************************/
GTK3G3DUIMOUSETOOLEDIT *gtk3_g3duimousetooledit_create ( GtkWidget *parent, 
                                                         GTK3G3DUI *gtk3gui,
                                                         char      *name ) {
    GTK3G3DUIMOUSETOOLEDIT *gtk3mtledit = gtk3_g3duimousetooledit_new ( gtk3gui );

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

    createPickToolEdit    ( gtk3mtledit );
    createCutMeshToolEdit ( gtk3mtledit );
    createSculptToolEdit  ( gtk3mtledit );

    gtk_widget_show ( gtk3mtledit->mousetoolFixed );
    gtk_widget_show ( gtk3mtledit->scrolled );


    return gtk3mtledit;
}
