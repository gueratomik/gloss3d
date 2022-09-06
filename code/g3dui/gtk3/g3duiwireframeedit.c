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

#define EDITWIREFRAMEGENERAL   "Wireframe"
#define EDITWIREFRAMETHICKNESS "Thickness"

/******************************************************************************/
static GTK3G3DUIWIREFRAMEEDIT *gtk3_g3duiwireframeedit_new ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUIWIREFRAMEEDIT *gtk3wed = calloc ( 0x01, sizeof ( GTK3G3DUIWIREFRAMEEDIT ) );

    if ( gtk3wed == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3wed->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3wed; 
}

/******************************************************************************/
static void updateGeneralPanel ( GTK3G3DUIWIREFRAMEEDIT *gtk3wed ) {
    gtk3wed->core.gui->lock = 0x01;

    if ( gtk3wed->core.editedWireframe ) {
        G3DWIREFRAME *wir = gtk3wed->core.editedWireframe;

        gtk_spin_button_set_value ( gtk3wed->thicknessEntry, wir->thickness );
    }

    gtk3wed->core.gui->lock = 0x00;
}

/******************************************************************************/
static void thicknessCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIWIREFRAMEEDIT *gtk3wed = ( GTK3G3DUIWIREFRAMEEDIT * ) user_data;
    G3DUI *gui = gtk3wed->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    double thickness = gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    g3duiwireframeedit_thicknessCbk ( &gtk3wed->core, thickness );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
void gtk3_g3duiwireframeedit_update ( GTK3G3DUIWIREFRAMEEDIT *gtk3wed ) {
    G3DUI *gui = gtk3wed->core.gui;

    gui->lock = 0x01;

    if ( gui->sce ) {
        G3DSCENE *sce = gui->sce;
        uint32_t nbsel = list_count ( sce->lsel );

        if ( nbsel ) {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3wed->notebook), TRUE );

            if ( g3dobjectlist_checkType ( sce->lsel, G3DWIREFRAMETYPE ) ) {
                gtk3wed->core.multi = ( nbsel > 0x01 ) ? 0x01 : 0x00;

                gtk3wed->core.editedWireframe = ( G3DPRIMITIVE * ) g3dscene_getLastSelected ( sce );

                if ( gtk3wed->core.editedWireframe ) {
                    updateGeneralPanel  ( gtk3wed );
                }
            }
        } else {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3wed->notebook), FALSE );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void createGeneralPanel ( GTK3G3DUIWIREFRAMEEDIT *gtk3wed,
                                 gint                    x,
                                 gint                    y,
                                 gint                    width,
                                 gint                    height ) {
    GtkFixed *pan = ui_createTab ( gtk3wed->notebook,
                                   gtk3wed,
                                   EDITWIREFRAMEGENERAL,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3wed->thicknessEntry  = ui_createFloatText   ( pan, 
                                                      gtk3wed,
                                                      EDITWIREFRAMETHICKNESS,
                                                      CLASS_MAIN,
                                                      0.0f, FLT_MAX,
                                                      0, 0, 96, 96, 20,
                                                      thicknessCbk );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIWIREFRAMEEDIT *gtk3wed = ( GTK3G3DUIWIREFRAMEEDIT * ) user_data;

    free ( gtk3wed );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIWIREFRAMEEDIT *gtk3wed = ( GTK3G3DUIWIREFRAMEEDIT * ) user_data;

    gtk3_g3duiwireframeedit_update ( gtk3wed );
}

/******************************************************************************/
GTK3G3DUIWIREFRAMEEDIT *gtk3_g3duiwireframeedit_create ( GtkWidget *parent,
                                                         GTK3G3DUI *gtk3gui,
                                                         char      *name ) {
    GTK3G3DUIWIREFRAMEEDIT *gtk3wed = gtk3_g3duiwireframeedit_new ( gtk3gui );
    GtkWidget *notebook = gtk_notebook_new ( );

    gtk3wed->notebook = GTK_NOTEBOOK(notebook);

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(notebook), TRUE );

    gtk_widget_set_name ( notebook, name );

    /*gtk_widget_set_size_request ( tab, width, height );*/

    g_signal_connect ( G_OBJECT (notebook), "realize", G_CALLBACK (Realize), gtk3wed );
    g_signal_connect ( G_OBJECT (notebook), "destroy", G_CALLBACK (Destroy), gtk3wed );

    createGeneralPanel ( gtk3wed, 0, 0, 310, 150 );


    gtk_widget_show ( notebook );


    return gtk3wed;
}


#ifdef unused

static void updateWireframeForm ( GtkWidget *, G3DUI * );

/******************************************************************************/
static void thicknessCbk  ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    double thickness = gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui    = ( G3DUI * ) user_data;

    common_g3duiwireframeedit_thicknessCbk ( gui, thickness );

    /*updateSubdivisionForm ( parent, gui );*/
}

/******************************************************************************/
static void updateWireframeForm ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DWIREFRAMETYPE ) ) {
        G3DWIREFRAME *wir = ( G3DWIREFRAME * ) obj;

        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *child_name = gtk_widget_get_name ( child );

            if ( GTK_IS_SPIN_BUTTON(child) ) {
                GtkSpinButton *sbn = GTK_SPIN_BUTTON(child);

                if ( strcmp ( child_name, EDITWIREFRAMETHICKNESS ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, wir->thickness );
                }
            }

            children =  g_list_next ( children );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void updateWireframeFrame ( GtkWidget *widget, G3DUI *gui ) {
    GtkWidget *frm = gtk_bin_get_child ( GTK_BIN(widget) );

    if ( frm ) updateWireframeForm ( frm, gui );
}


/******************************************************************************/
static void createWireframeFrame ( GtkWidget *frm, G3DUI *gui,
                                                     gint x,
                                                     gint y,
                                                     gint width,
                                                     gint height ) {
    GtkWidget *wrf;

    wrf = createFrame ( frm, gui, EDITWIREFRAME       , x,   y, width, height );

    createFloatText ( wrf, gui, EDITWIREFRAMETHICKNESS ,  0.0f, FLT_MAX,
                                                          0,  24, 128, 32,
                                                         thicknessCbk );
}


/******************************************************************************/
void updateWireframeEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    /*** prevent a loop ***/
    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( strcmp ( child_name, EDITWIREFRAME ) == 0x00 ) {
            updateWireframeFrame ( child, gui );
        }

        children =  g_list_next ( children );
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    updateWireframeEdit ( widget, gui );
}

/******************************************************************************/
GtkWidget *createWireframeEdit ( GtkWidget *parent, G3DUI *gui,
                                                     char *name,
                                                     gint x,
                                                     gint y,
                                                     gint width,
                                                     gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *frm, *sdf, *fix;

    frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), frm, x, y );

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), gui );

    createWireframeFrame ( frm, gui,   0,   0, 286, 140 );

    gtk_widget_show ( frm );


    return frm;
}

#endif
