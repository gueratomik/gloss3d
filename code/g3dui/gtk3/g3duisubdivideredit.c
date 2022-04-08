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

static void updateSubdivisionForm ( GtkWidget *, G3DUI * );

#define EDITSUBDIVIDERGENERAL            "General"
#define EDITSUBDIVIDERPREVIEW            "Preview"
#define EDITSUBDIVIDERRENDER             "Render"
#define EDITSUBDIVIDERSYNC               "Sync"
#define EDITSUBDIVIDERDISPLACEMENT       "Displacement"
#define EDITSUBDIVIDERDISPLACEMENTMODE   "Mode"
#define EDITSUBDIVIDERDISPLACEMENTHEIGHT "Height"
#define EDITSUBDIVIDERDISPLACEMENTSCULPT "Sculpt"

/******************************************************************************/
typedef struct _G3DUISUBDIVIDEREDIT {
    G3DUIWIDGETGROUP grp;

    GtkWidget       *main;
    GtkWidget       *previewEntry;
    GtkWidget       *renderEntry;
    GtkWidget       *syncToggle;

    GtkWidget       *sculptRadio;
    GtkWidget       *heightRadio;

    G3DSUBDIVIDER   *editedSubdivider;
} G3DUISUBDIVIDEREDIT;

/******************************************************************************/
static G3DUISUBDIVIDEREDIT *g3duisubdivideredit_new ( G3DUI *gui ) {
    G3DUISUBDIVIDEREDIT *sed = calloc ( 0x01, sizeof ( G3DUISUBDIVIDEREDIT ) );

    if ( sed == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );
    }

    sed->grp.gui = gui;

    return sed; 
}

/******************************************************************************/
/*static void useIsoLinesCbk  ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duimeshedit_useIsoLinesCbk ( gui );
}*/

/******************************************************************************/
static void updateSubdividerDisplacementPanel ( G3DUISUBDIVIDEREDIT *sed ) {
    sed->grp.gui->lock = 0x01;

    if ( sed->editedSubdivider ) {
        gtk_widget_set_sensitive ( sed->sculptRadio, TRUE );
        gtk_widget_set_sensitive ( sed->heightRadio, TRUE );

        if ( sed->editedSubdivider->sculptMode == SCULPTMODE_SCULPT ) {
            gtk_toggle_button_set_active ( sed->sculptRadio, TRUE  );
        } else {
            gtk_toggle_button_set_active ( sed->sculptRadio, FALSE );
        }

        if ( sed->editedSubdivider->sculptMode == SCULPTMODE_HEIGHT ) {
            gtk_toggle_button_set_active ( sed->heightRadio, TRUE  );
        } else {
            gtk_toggle_button_set_active ( sed->heightRadio, FALSE );
        }
    } else {
        gtk_widget_set_sensitive ( sed->sculptRadio, FALSE );
        gtk_widget_set_sensitive ( sed->heightRadio, FALSE );
    }

    sed->grp.gui->lock = 0x00;
}

/******************************************************************************/
static void radioSubdivHeightCbk ( GtkWidget *widget, 
                                   gpointer   user_data ) {
    G3DUISUBDIVIDEREDIT *sed = ( G3DUISUBDIVIDEREDIT * ) user_data;

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( sed->grp.gui->lock ) return;

    common_g3duisubdivideredit_displacementHeightCbk ( sed->grp.gui );
}

/******************************************************************************/
static void radioSubdivSculptCbk ( GtkWidget *widget, 
                                   gpointer   user_data ) {
    G3DUISUBDIVIDEREDIT *sed = ( G3DUISUBDIVIDEREDIT * ) user_data;

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( sed->grp.gui->lock ) return;

    common_g3duisubdivideredit_displacementSculptCbk ( sed->grp.gui );
}

/******************************************************************************/
static GtkWidget *createSubdividerDisplacementPanel ( GtkWidget           *parent, 
                                                      G3DUISUBDIVIDEREDIT *sed,
                                                      char                *name,
                                                      gint                 x,
                                                      gint                 y,
                                                      gint                 width,
                                                      gint                 height ) {
    GtkWidget *pan = createPanel ( parent, sed, name, x, y, width, height );

    sed->heightRadio = createRadioLabel ( pan, 
                                          sed,
                                          EDITSUBDIVIDERDISPLACEMENTHEIGHT, 
                                          NULL,
                                          0,  0, 128, 32,
                                          radioSubdivHeightCbk );

    sed->sculptRadio = createRadioLabel ( pan, 
                                          sed,
                                          EDITSUBDIVIDERDISPLACEMENTSCULPT, 
                                          sed->heightRadio,
                                          0, 24, 128, 32,
                                          radioSubdivSculptCbk );


    return pan;
}

/******************************************************************************/
static void updateSubdividerGeneralPanel ( G3DUISUBDIVIDEREDIT *sed ) {
    sed->grp.gui->lock = 0x01;

    if ( sed->editedSubdivider ) {
        gtk_widget_set_sensitive ( sed->syncToggle   , TRUE );
        gtk_widget_set_sensitive ( sed->previewEntry , TRUE );
        gtk_widget_set_sensitive ( sed->renderEntry  , TRUE );

        if ( ((G3DOBJECT*)sed->editedSubdivider)->flags & SYNCLEVELS ) {
            gtk_toggle_button_set_active ( sed->syncToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( sed->syncToggle, FALSE );
        }

        gtk_spin_button_set_value ( sed->previewEntry   , 
                                    sed->editedSubdivider->subdiv_preview );

        gtk_spin_button_set_value ( sed->renderEntry   , 
                                    sed->editedSubdivider->subdiv_render );
    } else {
        gtk_widget_set_sensitive ( sed->syncToggle   , FALSE );
        gtk_widget_set_sensitive ( sed->previewEntry , FALSE );
        gtk_widget_set_sensitive ( sed->renderEntry  , FALSE );
    }

    sed->grp.gui->lock = 0x00;
}

/******************************************************************************/
static void subdivSyncCbk  ( GtkWidget *widget, gpointer user_data ) {
    G3DUISUBDIVIDEREDIT *sed = ( G3DUISUBDIVIDEREDIT * ) user_data;

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( sed->grp.gui->lock ) return;

    common_g3duisubdivideredit_subdivSyncCbk ( sed->grp.gui );
}

/******************************************************************************/
static void subdivRenderCbk  ( GtkWidget *widget, gpointer user_data ) {
    G3DUISUBDIVIDEREDIT *sed = ( G3DUISUBDIVIDEREDIT * ) user_data;
    GtkWidget *parent = gtk_widget_get_parent     ( widget );
    int level  = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( sed->grp.gui->lock ) return;

    if ( level >= 0x00 ) {
        common_g3duisubdivideredit_subdivRenderCbk ( sed->grp.gui, level );
    }

    updateSubdividerGeneralPanel ( sed );
}

/******************************************************************************/
static void subdivPreviewCbk  ( GtkWidget *widget, gpointer user_data ) {
    G3DUISUBDIVIDEREDIT *sed = ( G3DUISUBDIVIDEREDIT * ) user_data;
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    int level  = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( sed->grp.gui->lock ) return;

    if ( level >= 0x00 ) {
        common_g3duisubdivideredit_subdivPreviewCbk ( sed->grp.gui, level );
    }

    updateSubdividerGeneralPanel ( sed );
}

/******************************************************************************/
static GtkWidget *createSubdividerGeneralPanel ( GtkWidget           *parent, 
                                                 G3DUISUBDIVIDEREDIT *sed,
                                                 char                *name,
                                                 gint                 x,
                                                 gint                 y,
                                                 gint                 width,
                                                 gint                 height ) {
    GtkWidget *pan = createPanel ( parent, sed, name, x, y, width, height );

    sed->previewEntry = createIntegerText ( pan, 
                                            sed,
                                            EDITSUBDIVIDERPREVIEW, 
                                            0,  9,
                                            0,  0, 128, 32,
                                            subdivPreviewCbk );

    sed->renderEntry = createIntegerText ( pan,
                                           sed,
                                           EDITSUBDIVIDERRENDER,
                                           0,  9,
                                           0, 24, 128, 32,
                                           subdivRenderCbk );

    sed->syncToggle = createToggleLabel ( pan, 
                                          sed,
                                          EDITSUBDIVIDERSYNC,
                                          230, 12, 20, 20,
                                          subdivSyncCbk );

    return pan;
}

/******************************************************************************/
void updateSubdividerEdit ( GtkWidget     *w, 
                            G3DSUBDIVIDER *sdr ) {
    G3DUISUBDIVIDEREDIT *sed = ( G3DUISUBDIVIDEREDIT * ) g_object_get_data ( G_OBJECT(w),
                                                                             "private" );

    sed->editedSubdivider = sdr;

    updateSubdividerGeneralPanel      ( sed );
    updateSubdividerDisplacementPanel ( sed );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    G3DUISUBDIVIDEREDIT *sed = ( G3DUISUBDIVIDEREDIT * ) user_data;

    free ( sed );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUISUBDIVIDEREDIT *sed = ( G3DUISUBDIVIDEREDIT * ) user_data;

    updateSubdividerEdit ( widget, NULL );
}

/******************************************************************************/
GtkWidget *createSubdividerEdit ( GtkWidget *parent, 
                                  G3DUI     *gui,
                                  char      *name,
                                  gint       x,
                                  gint       y,
                                  gint       width,
                                  gint       height ) {
    GdkRectangle gdkrec = { 0x00, 0x00, width, height };
    GtkWidget *tab = gtk_notebook_new ( );
    G3DUISUBDIVIDEREDIT *sed = g3duisubdivideredit_new ( gui );

    g_object_set_data ( G_OBJECT(tab), "private", (gpointer) sed );

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(tab), TRUE );

    gtk_widget_set_name ( tab, name );

    gtk_widget_size_allocate ( tab, &gdkrec );
    /*gtk_widget_set_size_request ( tab, width, height );*/

    gtk_fixed_put ( GTK_FIXED(parent), tab, x, y );

    g_signal_connect ( G_OBJECT (tab), "realize", G_CALLBACK (Realize), sed );
    g_signal_connect ( G_OBJECT (tab), "destroy", G_CALLBACK (Destroy), sed );

    createSubdividerGeneralPanel      ( tab, 
                                        sed, 
                                        EDITSUBDIVIDERGENERAL, 
                                        0x00, 
                                        0x00, 
                                        width,
                                        height );

    createSubdividerDisplacementPanel ( tab, 
                                        sed, 
                                        EDITSUBDIVIDERDISPLACEMENT,
                                        0x00,
                                        0x00,
                                        width,
                                        height );

    /*list_insert ( &gui->lligedit, tab );*/

    gtk_widget_show ( tab );


    return tab;
}
