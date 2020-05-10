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
static void projectionCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkComboBoxText *cmbt = GTK_COMBO_BOX_TEXT(widget);
    G3DUI *gui = ( G3DUI * ) user_data;
    const char *str = gtk_combo_box_text_get_active_text ( cmbt );

    common_g3duiuvmap_projectionCbk ( gui, str );
}

/******************************************************************************/
static void lockUVMapCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duiuvmap_lockUVMapCbk ( gui );
}

/******************************************************************************/
static void uvmapEditorCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *dial = gtk_window_new ( GTK_WINDOW_TOPLEVEL );
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DUIGTK3 *ggt = gui->toolkit_data;

    createUVMapEditor ( dial, gui, "UVMAPEDITOR", 800, 600 );

    gtk_widget_show ( dial );
    gtk_widget_destroy ( gtk_widget_get_toplevel ( widget ) );
}

/******************************************************************************/
void createProjectionSelection ( GtkWidget *parent, G3DUI *gui, 
                                                    char *name,
                                                    gint x, gint y,
                                                    gint labwidth,
                                                    gint txtwidth,
                                                    void (*cbk)( GtkWidget *, 
                                                                 gpointer ) ) {
    GtkWidget     *cmb  = gtk_combo_box_text_new ( );
    GdkRectangle   crec = { 0x00, 0x00, txtwidth, 0x12 };

    /*gtk_spin_button_set_numeric ( btn, TRUE );*/

    gtk_widget_set_name ( cmb, name );

    gtk_widget_size_allocate ( cmb, &crec );

    /*gtk_entry_set_width_chars ( GTK_ENTRY(cmb), 18 );*/

    gtk_fixed_put ( GTK_FIXED(parent), cmb, x + labwidth, y );

    if ( labwidth ) {
        GdkRectangle lrec = { 0x00, 0x00, labwidth, 0x12 };
        GtkWidget   *lab  = gtk_label_new ( name );

        gtk_widget_set_name ( lab, name );

        gtk_widget_size_allocate ( lab, &lrec );

        gtk_fixed_put ( GTK_FIXED(parent), lab, x, y );

        gtk_widget_show ( lab );
    }

    if ( cbk ) { 
        g_signal_connect ( cmb, "changed", G_CALLBACK(cbk), gui );
    }

    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, FLATPROJECTION        );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, SPHERICALPROJECTION   );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, CYLINDRICALPROJECTION );
    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, BACKGROUNDPROJECTION );

    gtk_combo_box_set_active ( GTK_COMBO_BOX(cmb), 0x00 );

    gtk_widget_show ( cmb );
}

/******************************************************************************/
void updateUVMapEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent useless primitive building when XmTextSetString is called, ***/
    /*** as XmTextSetString will call XmNvalueChanged callback whereas we   ***/
    /*** already know the value !                                           ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DUVMAPTYPE ) ) {
        G3DUVMAP *map = ( G3DUVMAP * ) obj;

        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *child_name = gtk_widget_get_name ( child );

            if ( GTK_IS_RADIO_BUTTON(child) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITUVMAPFIXED ) == 0x00 ) {
                    if ( ((G3DOBJECT*)map)->flags & UVMAPFIXED ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }
            }

            if ( GTK_IS_COMBO_BOX_TEXT(child) ) {
                GtkComboBox *cmb = GTK_COMBO_BOX(child);

                if ( strcmp ( child_name, EDITUVMAPPROJECTION   ) == 0x00 ) {
                    gtk_combo_box_set_active ( cmb, map->projection );
                }
            }

            children =  g_list_next ( children );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
GtkWidget *createUVMapEdit ( GtkWidget *parent, G3DUI *gui, 
                                                char *name,
                                                gint x, 
                                                gint y,
                                                gint width,
                                                gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    gtk_widget_set_size_request ( frm, gdkrec.width, gdkrec.height );

    /*** For some reason, GtkSpinButtons calls its callbacks ***/
    /*** when being realized. With this trick I bypass that. ***/
    /*** Callbacks will return prematurely if gui->lock == 0x01 ***/
    gui->lock = 0x01;

    createToggleLabel         ( frm, gui, EDITUVMAPFIXED,
                                0,  0, 200, 20, lockUVMapCbk );

    createProjectionSelection ( frm, gui, EDITUVMAPPROJECTION, 
                                0, 24, 96, 96, projectionCbk );


    createPushButton  ( frm, gui, 
                             EDITUVMAPEDITOR,
                              0, 48, 96, 18,
                             uvmapEditorCbk );

    gui->lock = 0x00;

    gtk_container_add ( GTK_CONTAINER(parent), frm );

    gtk_widget_show ( frm );


    return frm;
}
