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
static GTK3G3DUIUVMAPEDIT *gtk3_g3duiuvmapedit_new ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUIUVMAPEDIT *gtk3uvmed = calloc ( 0x01, sizeof ( GTK3G3DUIUVMAPEDIT ) );

    if ( gtk3uvmed == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3uvmed->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3uvmed; 
}

/******************************************************************************/
void gtk3_g3duiuvmapedit_update ( GTK3G3DUIUVMAPEDIT *gtk3uvmed ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3uvmed->core.gui;
    G3DUI *gui = &gtk3gui->core;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    /*** prevents a loop ***/
    gui->lock = 0x01;

    if ( obj ) {
        if ( obj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DUVMAP *map = g3dmesh_getSelectedUVMap ( mes );

            if ( map ) {
                gtk_entry_set_text ( gtk3uvmed->nameEntry, ((G3DOBJECT*)map)->name );

                if ( ((G3DOBJECT*)map)->flags & UVMAPFIXED ) {
                    gtk_toggle_button_set_active ( gtk3uvmed->fixedToggle, TRUE  );
                    gtk_widget_set_sensitive ( gtk3uvmed->projectionCombo, FALSE  );
                } else {
                    gtk_toggle_button_set_active ( gtk3uvmed->fixedToggle, FALSE );
                    gtk_widget_set_sensitive ( gtk3uvmed->projectionCombo, TRUE );
                }

                gtk_combo_box_set_active ( gtk3uvmed->projectionCombo, map->projection );
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void projectionCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIUVMAPEDIT *gtk3uvmed = ( GTK3G3DUIUVMAPEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3uvmed->core.gui;
    G3DUI *gui = &gtk3gui->core;
    GtkComboBoxText *cmbt = GTK_COMBO_BOX_TEXT(widget);
    const char *str = gtk_combo_box_text_get_active_text ( cmbt );
    uint64_t ret;

    if ( gtk3uvmed->core.gui->lock ) return;

    ret = g3duiuvmapedit_projection ( gtk3uvmed, str );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );
}

/******************************************************************************/
static void lockUVMapCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIUVMAPEDIT *gtk3uvmed = ( GTK3G3DUIUVMAPEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3uvmed->core.gui;
    G3DUI *gui = &gtk3gui->core;
    uint64_t ret;

    if ( gtk3uvmed->core.gui->lock ) return;

     ret = g3duiuvmapedit_lockUVMap ( gtk3uvmed );

    gtk3_g3duiuvmapedit_update ( gtk3uvmed );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );
}

/******************************************************************************/
static void nameUVMapCbk  ( GtkWidget *widget, 
                            GdkEvent  *event, 
                            gpointer   user_data ) {
    GTK3G3DUIUVMAPEDIT *gtk3uvmed = ( GTK3G3DUIUVMAPEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3uvmed->core.gui;
    G3DUI *gui = &gtk3gui->core;
    const char *mapname = gtk_entry_get_text ( GTK_ENTRY(widget) );
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( gtk3uvmed->core.gui->lock ) return;

    if ( obj ) {
        if ( obj->type & MESH ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DUVMAP *map = g3dmesh_getSelectedUVMap ( mes );

            if ( map ) {
                g3dobject_name ( (G3DOBJECT*) map, mapname );
            }
        }
    }
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIUVMAPEDIT *gtk3uvmed = ( GTK3G3DUIUVMAPEDIT * ) user_data;

    free ( gtk3uvmed );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIUVMAPEDIT *gtk3uvmed = ( GTK3G3DUIUVMAPEDIT * ) user_data;

    gtk3_g3duiuvmapedit_update ( gtk3uvmed );
}

/******************************************************************************/
GTK3G3DUIUVMAPEDIT* gtk3_g3duiuvmapedit_create ( GtkWidget *parent,
                                                 GTK3G3DUI *gtk3gui,
                                                 char      *name,
                                                 gint       x,
                                                 gint       y,
                                                 gint       width,
                                                 gint       height ) {
    GTK3G3DUIUVMAPEDIT *gtk3uvmed = gtk3_g3duiuvmapedit_new ( gtk3gui );
    GtkWidget *fixed = ui_gtk_fixed_new ( CLASS_MAIN );

    gtk3uvmed->fixed = fixed;

    gtk_widget_set_name ( fixed, name );

    gtk_widget_set_size_request ( fixed, width, height );

    gtk3uvmed->nameEntry       = ui_createCharText           ( fixed, gtk3uvmed, EDITUVMAPNAME      , CLASS_ENTRY, 0,  0, 96, 96, 20, nameUVMapCbk  );
    gtk3uvmed->projectionCombo = ui_createProjectionSelector ( fixed, gtk3uvmed, EDITUVMAPPROJECTION, CLASS_MAIN, 0, 24, 96, 96, 20, projectionCbk );
    gtk3uvmed->fixedToggle     = ui_createToggleLabel        ( fixed, gtk3uvmed, EDITUVMAPFIXED     , CLASS_MAIN, 0, 48, 96, 20, 20, lockUVMapCbk  );

    g_signal_connect ( G_OBJECT (fixed), "realize", G_CALLBACK (Realize), gtk3uvmed );
    g_signal_connect ( G_OBJECT (fixed), "destroy", G_CALLBACK (Destroy), gtk3uvmed );


    gtk_widget_show_all ( fixed );


    return gtk3uvmed;
}
