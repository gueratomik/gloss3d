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

#define EDITSUBDIVIDERGENERAL            "Subdivider"
#define EDITSUBDIVIDERPREVIEW            "Preview"
#define EDITSUBDIVIDERRENDER             "Render"
#define EDITSUBDIVIDERSYNC               "Sync"
#define EDITSUBDIVIDERDISPLACEMENT       "Displacement"
#define EDITSUBDIVIDERDISPLACEMENTMODE   "Mode"
#define EDITSUBDIVIDERDISPLACEMENTHEIGHT "Height"
#define EDITSUBDIVIDERDISPLACEMENTSCULPT "Sculpt"


/******************************************************************************/
static GTK3G3DUISUBDIVIDEREDIT *gtk3_g3duisubdivideredit_new ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUISUBDIVIDEREDIT *gtk3sed = calloc ( 0x01, sizeof ( GTK3G3DUISUBDIVIDEREDIT ) );

    if ( gtk3sed == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3sed->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3sed; 
}

/******************************************************************************/
static void updateDisplacementPanel ( GTK3G3DUISUBDIVIDEREDIT *gtk3sed ) {
    gtk3sed->core.gui->lock = 0x01;

    if ( gtk3sed->core.editedSubdivider ) {
        G3DSUBDIVIDER *sdr = ( G3DSUBDIVIDER * ) gtk3sed->core.editedSubdivider;

        if ( sdr->sculptMode == SCULPTMODE_SCULPT ) {
            gtk_toggle_button_set_active ( gtk3sed->sculptRadio, TRUE  );
        } else {
            gtk_toggle_button_set_active ( gtk3sed->sculptRadio, FALSE );
        }

        if ( sdr->sculptMode == SCULPTMODE_HEIGHT ) {
            gtk_toggle_button_set_active ( gtk3sed->heightRadio, TRUE  );
        } else {
            gtk_toggle_button_set_active ( gtk3sed->heightRadio, FALSE );
        }
    }

    gtk3sed->core.gui->lock = 0x00;
}

/******************************************************************************/
static void radioSubdivHeightCbk ( GtkWidget *widget, 
                                   gpointer   user_data ) {
    GTK3G3DUISUBDIVIDEREDIT *gtk3sed = ( GTK3G3DUISUBDIVIDEREDIT * ) user_data;
    G3DUI *gui = gtk3sed->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3sed->core.gui->lock ) return;

    g3duisubdivideredit_displacementHeight ( &gtk3sed->core );

    gtk3_g3duisubdivideredit_update ( gtk3sed );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void radioSubdivSculptCbk ( GtkWidget *widget, 
                                   gpointer   user_data ) {
    GTK3G3DUISUBDIVIDEREDIT *gtk3sed = ( GTK3G3DUISUBDIVIDEREDIT * ) user_data;
    G3DUI *gui = gtk3sed->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3sed->core.gui->lock ) return;

    g3duisubdivideredit_displacementSculpt ( &gtk3sed->core );

    gtk3_g3duisubdivideredit_update ( gtk3sed );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void createDisplacementPanel ( GTK3G3DUISUBDIVIDEREDIT *gtk3sed,
                                      gint               x,
                                      gint               y,
                                      gint               width,
                                      gint               height ) {
    GtkFixed *pan = ui_createTab ( gtk3sed->notebook,
                                   gtk3sed,
                                   EDITSUBDIVIDERDISPLACEMENT,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3sed->heightRadio = ui_createToggleLabel ( pan, 
                                                  gtk3sed,
                                                  EDITSUBDIVIDERDISPLACEMENTHEIGHT,
                                                  CLASS_MAIN,
                                                  0,  0, 96, 96, 20,
                                                  radioSubdivHeightCbk );

    gtk3sed->sculptRadio = ui_createToggleLabel ( pan, 
                                                  gtk3sed,
                                                  EDITSUBDIVIDERDISPLACEMENTSCULPT,
                                                  CLASS_MAIN,
                                                  0, 24, 96, 96, 20,
                                                  radioSubdivSculptCbk );
}

/******************************************************************************/
static void updateGeneralPanel ( GTK3G3DUISUBDIVIDEREDIT *gtk3sed ) {
    gtk3sed->core.gui->lock = 0x01;

    if ( gtk3sed->core.editedSubdivider ) {
        G3DSUBDIVIDER *sdr = gtk3sed->core.editedSubdivider;

        if ( sdr->mod.mes.obj.flags & SYNCLEVELS ) {
            gtk_toggle_button_set_active ( gtk3sed->syncToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( gtk3sed->syncToggle, FALSE );
        }

        gtk_spin_button_set_value ( gtk3sed->previewEntry, sdr->subdiv_preview );
        gtk_spin_button_set_value ( gtk3sed->renderEntry , sdr->subdiv_render  );
    }

    gtk3sed->core.gui->lock = 0x00;
}

/******************************************************************************/
static void subdivSyncCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUISUBDIVIDEREDIT *gtk3sed = ( GTK3G3DUISUBDIVIDEREDIT * ) user_data;
    G3DUI *gui = gtk3sed->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3sed->core.gui->lock ) return;

    g3duisubdivideredit_subdivSync ( &gtk3sed->core );

    gtk3_g3duisubdivideredit_update ( gtk3sed );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void subdivRenderCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUISUBDIVIDEREDIT *gtk3sed = ( GTK3G3DUISUBDIVIDEREDIT * ) user_data;
    G3DUI *gui = gtk3sed->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    int level = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3sed->core.gui->lock ) return;

    if ( level >= 0x00 ) {
        g3duisubdivideredit_subdivRender ( &gtk3sed->core, level );
    }

    gtk3_g3duisubdivideredit_update ( gtk3sed );
}

/******************************************************************************/
static void subdivPreviewCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUISUBDIVIDEREDIT *gtk3sed = ( GTK3G3DUISUBDIVIDEREDIT * ) user_data;
    G3DUI *gui = gtk3sed->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    int level = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3sed->core.gui->lock ) return;

    if ( level >= 0x00 ) {
        g3duisubdivideredit_subdivPreview ( &gtk3sed->core, level );
    }

    gtk3_g3duisubdivideredit_update ( gtk3sed );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void createGeneralPanel ( GTK3G3DUISUBDIVIDEREDIT *gtk3sed,
                                 gint               x,
                                 gint               y,
                                 gint               width,
                                 gint               height ) {
    GtkFixed *pan = ui_createTab ( gtk3sed->notebook,
                                   gtk3sed,
                                   EDITSUBDIVIDERGENERAL,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3sed->previewEntry = ui_createIntegerText ( pan, 
                                                   gtk3sed,
                                                   EDITSUBDIVIDERPREVIEW,
                                                   CLASS_MAIN,
                                                   0,  9,
                                                   0, 0, 96, 96, 20,
                                                   subdivPreviewCbk );

    gtk3sed->renderEntry  = ui_createIntegerText ( pan, 
                                                   gtk3sed,
                                                   EDITSUBDIVIDERRENDER,
                                                   CLASS_MAIN,
                                                   0,  9,
                                                   0, 24, 96, 96, 20,
                                                   subdivRenderCbk );

    gtk3sed->syncToggle  = ui_createToggleLabel ( pan, 
                                                  gtk3sed,
                                                  EDITSUBDIVIDERSYNC,
                                                  CLASS_MAIN,
                                                  0, 48, 96, 96, 20,
                                                  subdivSyncCbk );
}

/******************************************************************************/
void gtk3_g3duisubdivideredit_update ( GTK3G3DUISUBDIVIDEREDIT *gtk3sed ) {
    G3DUI *gui = gtk3sed->core.gui;

    gui->lock = 0x01;

    if ( gui->sce ) {
        G3DSCENE *sce = gui->sce;
        uint32_t nbsel = list_count ( sce->lsel );

        if ( nbsel ) {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3sed->notebook), TRUE );

            if ( g3dobjectlist_checkType ( sce->lsel, G3DSUBDIVIDERTYPE ) ) {
                gtk3sed->core.multi = ( nbsel > 0x01 ) ? 0x01 : 0x00;

                gtk3sed->core.editedSubdivider = ( G3DSUBDIVIDER * ) g3dscene_getLastSelected ( sce );

                if ( gtk3sed->core.editedSubdivider ) {
                    updateGeneralPanel     ( gtk3sed );
                    updateDisplacementPanel( gtk3sed );
                }
            }
        } else {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3sed->notebook), FALSE );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUISUBDIVIDEREDIT *gtk3sed = ( GTK3G3DUISUBDIVIDEREDIT * ) user_data;

    free ( gtk3sed );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUISUBDIVIDEREDIT *gtk3sed = ( GTK3G3DUISUBDIVIDEREDIT * ) user_data;

    gtk3_g3duisubdivideredit_update ( gtk3sed );
}

/******************************************************************************/
GTK3G3DUISUBDIVIDEREDIT *gtk3_g3duisubdivideredit_create ( GtkWidget *parent,
                                                           GTK3G3DUI *gtk3gui,
                                                           char      *name ) {
    GTK3G3DUISUBDIVIDEREDIT *gtk3sed = gtk3_g3duisubdivideredit_new ( gtk3gui );
    GtkWidget *notebook = gtk_notebook_new ( );

    gtk3sed->notebook = GTK_NOTEBOOK(notebook);

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(notebook), TRUE );

    gtk_widget_set_name ( notebook, name );

    /*gtk_widget_set_size_request ( tab, width, height );*/

    g_signal_connect ( G_OBJECT (notebook), "realize", G_CALLBACK (Realize), gtk3sed );
    g_signal_connect ( G_OBJECT (notebook), "destroy", G_CALLBACK (Destroy), gtk3sed );

    createGeneralPanel      ( gtk3sed, 0, 0, 310, 150 );
    createDisplacementPanel ( gtk3sed, 0, 0, 310, 150 );


    gtk_widget_show ( notebook );


    return gtk3sed;
}
