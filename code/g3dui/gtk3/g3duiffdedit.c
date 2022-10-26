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

/**** Widget names for FFDEdit TextField widget ***/
#define EDITFFDGENERAL "FFD"
#define EDITFFDCAPS    "Caps"
#define EDITFFDSLICES  "Slices"
#define EDITFFDLENGTH  "Length"
#define EDITFFDRADIUS  "Radius"

/******************************************************************************/
static GTK3G3DUIFFDEDIT *gtk3_g3duiffdedit_new ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUIFFDEDIT *gtk3fed = calloc ( 0x01, sizeof ( GTK3G3DUIFFDEDIT ) );

    if ( gtk3fed == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3fed->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3fed; 
}

/******************************************************************************/
static void updateGeneralPanel ( GTK3G3DUIFFDEDIT *gtk3fed ) {
    gtk3fed->core.gui->lock = 0x01;

    if ( gtk3fed->core.editedFFD ) {
        G3DFFD *ffd = gtk3fed->core.editedFFD;

        gtk_spin_button_set_value ( gtk3fed->xSlicesEntry, ffd->nbx );
        gtk_spin_button_set_value ( gtk3fed->ySlicesEntry, ffd->nby );
        gtk_spin_button_set_value ( gtk3fed->zSlicesEntry, ffd->nbz );
        gtk_spin_button_set_value ( gtk3fed->xRadiusEntry, ffd->locmax.x );
        gtk_spin_button_set_value ( gtk3fed->yRadiusEntry, ffd->locmax.y );
        gtk_spin_button_set_value ( gtk3fed->zRadiusEntry, ffd->locmax.z );
    }

    gtk3fed->core.gui->lock = 0x00;
}

/******************************************************************************/
static void xsliceCbk ( GtkWidget *widget,
                        gpointer   user_data ) {
    GTK3G3DUIFFDEDIT *gtk3fed = ( GTK3G3DUIFFDEDIT * ) user_data;
    G3DUI *gui = gtk3fed->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    int slice = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3fed->core.gui->lock ) return;

    if ( slice ) {
        g3duiffdedit_slice ( gtk3fed, G3DUIXAXIS, slice );
    } else {
        updateGeneralPanel ( gtk3fed );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void ysliceCbk ( GtkWidget *widget,
                        gpointer   user_data ) {
    GTK3G3DUIFFDEDIT *gtk3fed = ( GTK3G3DUIFFDEDIT * ) user_data;
    G3DUI *gui = gtk3fed->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    int slice = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3fed->core.gui->lock ) return;

    if ( slice ) {
        g3duiffdedit_slice ( gtk3fed, G3DUIYAXIS, slice );
    } else {
        updateGeneralPanel ( gtk3fed );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void zsliceCbk ( GtkWidget *widget,
                        gpointer   user_data ) {
    GTK3G3DUIFFDEDIT *gtk3fed = ( GTK3G3DUIFFDEDIT * ) user_data;
    G3DUI *gui = gtk3fed->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    int slice = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3fed->core.gui->lock ) return;

    if ( slice ) {
        g3duiffdedit_slice ( gtk3fed, G3DUIZAXIS, slice );
    } else {
        updateGeneralPanel ( gtk3fed );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void xradiusCbk ( GtkWidget *widget,
                         gpointer   user_data ) {
    GTK3G3DUIFFDEDIT *gtk3fed = ( GTK3G3DUIFFDEDIT * ) user_data;
    G3DUI *gui = gtk3fed->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float rad = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3fed->core.gui->lock ) return;

    if ( rad >= 0.0f ) {
        g3duiffdedit_radius ( gtk3fed, G3DUIXAXIS, rad );
    } else {
        updateGeneralPanel ( gtk3fed );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void yradiusCbk ( GtkWidget *widget,
                         gpointer   user_data ) {
    GTK3G3DUIFFDEDIT *gtk3fed = ( GTK3G3DUIFFDEDIT * ) user_data;
    G3DUI *gui = gtk3fed->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float rad = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3fed->core.gui->lock ) return;

    if ( rad >= 0.0f ) {
        g3duiffdedit_radius ( gtk3fed, G3DUIYAXIS, rad );
    } else {
        updateGeneralPanel ( gtk3fed );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void zradiusCbk ( GtkWidget *widget, 
                         gpointer   user_data ) {
    GTK3G3DUIFFDEDIT *gtk3fed = ( GTK3G3DUIFFDEDIT * ) user_data;
    G3DUI *gui = gtk3fed->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    float rad = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3fed->core.gui->lock ) return;

    if ( rad >= 0.0f ) {
        g3duiffdedit_radius ( gtk3fed, G3DUIZAXIS, rad );
    } else {
        updateGeneralPanel ( gtk3fed );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
void gtk3_g3duiffdedit_update ( GTK3G3DUIFFDEDIT *gtk3fed ) {
    G3DUI *gui = gtk3fed->core.gui;

    gui->lock = 0x01;

    if ( gui->sce ) {
        G3DSCENE *sce = gui->sce;
        uint32_t nbsel = list_count ( sce->lsel );

        if ( nbsel ) {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3fed->notebook), TRUE );

            if ( g3dobjectlist_checkType ( sce->lsel, G3DFFDTYPE ) ) {
                gtk3fed->core.multi = ( nbsel > 0x01 ) ? 0x01 : 0x00;

                gtk3fed->core.editedFFD = ( G3DFFD * ) g3dscene_getLastSelected ( sce );

                if ( gtk3fed->core.editedFFD ) {
                    updateGeneralPanel  ( gtk3fed );
                }
            }
        } else {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3fed->notebook), FALSE );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void createGeneralPanel ( GTK3G3DUIFFDEDIT *gtk3fed,
                                 gint              x,
                                 gint              y,
                                 gint              width,
                                 gint              height ) {
    GtkFixed *pan = ui_createTab ( gtk3fed->notebook,
                                   gtk3fed,
                                   EDITFFDGENERAL,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3fed->xSlicesEntry = ui_createIntegerText ( pan,
                                                   gtk3fed,
                                                   EDITFFDXSLICES,
                                                   CLASS_MAIN,
                                                   1, 100,
                                                   0,   0, 96, 96, 20,
                                                   xsliceCbk  );

    gtk3fed->ySlicesEntry = ui_createIntegerText ( pan,
                                                   gtk3fed,
                                                   EDITFFDYSLICES,
                                                   CLASS_MAIN,
                                                   1, 100,
                                                   0,  24, 96, 96, 20,
                                                   ysliceCbk  );

    gtk3fed->zSlicesEntry = ui_createIntegerText ( pan,
                                                   gtk3fed,
                                                   EDITFFDZSLICES,
                                                   CLASS_MAIN,
                                                   1, 100,
                                                   0,  48, 96, 96, 20,
                                                   zsliceCbk  );

    gtk3fed->xRadiusEntry = ui_createFloatText   ( pan,
                                                   gtk3fed,
                                                   EDITFFDXRADIUS,
                                                   CLASS_MAIN,
                                                   0.0f, FLT_MAX,
                                                   0,  72, 96, 96, 20,
                                                   xradiusCbk );

    gtk3fed->yRadiusEntry = ui_createFloatText   ( pan,
                                                   gtk3fed,
                                                   EDITFFDYRADIUS,
                                                   CLASS_MAIN,
                                                   0.0f, FLT_MAX,
                                                   0,  96, 96, 96, 20,
                                                   yradiusCbk );

    gtk3fed->zRadiusEntry = ui_createFloatText   ( pan,
                                                   gtk3fed,
                                                   EDITFFDZRADIUS,
                                                   CLASS_MAIN,
                                                   0.0f, FLT_MAX,
                                                   0, 120, 96, 96, 20,
                                                   zradiusCbk );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIFFDEDIT *gtk3fed = ( GTK3G3DUIFFDEDIT * ) user_data;

    free ( gtk3fed );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIFFDEDIT *gtk3fed = ( GTK3G3DUIFFDEDIT * ) user_data;

    gtk3_g3duiffdedit_update ( gtk3fed );
}

/******************************************************************************/
GTK3G3DUIFFDEDIT *gtk3_g3duiffdedit_create ( GtkWidget *parent,
                                             GTK3G3DUI *gtk3gui,
                                             char      *name ) {
    GTK3G3DUIFFDEDIT *gtk3fed = gtk3_g3duiffdedit_new ( gtk3gui );
    GtkWidget *notebook = ui_gtk_notebook_new ( CLASS_MAIN );

    gtk3fed->notebook = GTK_NOTEBOOK(notebook);

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(notebook), TRUE );

    gtk_widget_set_name ( notebook, name );

    /*gtk_widget_set_size_request ( tab, width, height );*/

    g_signal_connect ( G_OBJECT (notebook), "realize", G_CALLBACK (Realize), gtk3fed );
    g_signal_connect ( G_OBJECT (notebook), "destroy", G_CALLBACK (Destroy), gtk3fed );

    createGeneralPanel ( gtk3fed, 0, 0, 310, 150 );


    gtk_widget_show ( notebook );


    return gtk3fed;
}
