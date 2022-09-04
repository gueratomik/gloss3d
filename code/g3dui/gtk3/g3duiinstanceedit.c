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
#define EDITINSTANCEGENERAL        "Instance"
#define EDITINSTANCEOBJECTSELECTOR "Source object"
#define EDITINSTANCEMIRRORED       "Mirrored"
#define EDITINSTANCEMIRRORINGPLANE "Mirroring plane"

/******************************************************************************/
static GTK3G3DUIINSTANCEEDIT *gtk3_g3duiinstanceedit_new ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUIINSTANCEEDIT *gtk3ied = calloc ( 0x01, sizeof ( GTK3G3DUIINSTANCEEDIT ) );

    if ( gtk3ied == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3ied->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3ied; 
}

/******************************************************************************/
static void populateObjectSelector ( GTK3G3DUIINSTANCEEDIT *gtk3ied ) {
    GtkComboBoxText *cmb = gtk3ied->objectSelector;
    G3DSCENE *sce = gtk3ied->core.gui->sce;

    if ( sce ) {
        G3DOBJECT *sel = g3dscene_getLastSelected ( sce );

        gtk_combo_box_text_remove_all ( cmb );

        if ( sel ) {
            if ( sel->type == G3DINSTANCETYPE ) {
                G3DINSTANCE *ins = ( G3DINSTANCE * ) sel;
                LIST *ltmpobj, *lobj = NULL;
                uint32_t objRank = 0x00;
                int selected = 0x00;

                /*** flatten the object tree ***/
                g3dobject_treeToList_r ( sce, &lobj );

                ltmpobj = lobj;

                while ( ltmpobj ) {
                    G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

                    if ( obj != ins ) {
                        if ( obj->type != G3DSCENETYPE ) {
                            gtk_combo_box_text_append ( cmb, NULL, obj->name );

                            if ( ins->ref == obj ) {
                                gtk_combo_box_set_active ( GTK_COMBO_BOX(cmb), objRank );

                                selected = 0x01;
                            }

                            objRank++;
                        }
                    }

                    ltmpobj = ltmpobj->next;
                }

                gtk_combo_box_text_append ( cmb, NULL, "None" );

                if ( selected == 0x00 ) {
                    gtk_combo_box_set_active ( GTK_COMBO_BOX(cmb), objRank );
                }

                list_free ( &lobj, NULL );
            }
        }
    }
}

/******************************************************************************/
static void updateGeneralPanel ( GTK3G3DUIINSTANCEEDIT *gtk3ied ) {
    gtk3ied->core.gui->lock = 0x01;

    if ( gtk3ied->core.editedInstance ) {
        G3DFFD *ffd = gtk3ied->core.editedInstance;

        if ( gtk3ied->core.editedInstance->obj.flags & INSTANCEMIRRORED ) {
            gtk_toggle_button_set_active ( gtk3ied->mirroredToggle, TRUE  );

            gtk_widget_set_sensitive ( gtk3ied->mirroredPlanSelector, TRUE );
        } else {
            gtk_toggle_button_set_active ( gtk3ied->mirroredToggle, FALSE );

            gtk_widget_set_sensitive ( gtk3ied->mirroredPlanSelector, FALSE );
        }

        gtk_combo_box_set_active ( GTK_COMBO_BOX(gtk3ied->mirroredPlanSelector), 
                                   gtk3ied->core.editedInstance->orientation );

        populateObjectSelector ( gtk3ied );
    }

    gtk3ied->core.gui->lock = 0x00;
}

/******************************************************************************/
static void mirroredToggleCbk  ( GtkWidget *widget,
                                 gpointer   user_data ) {
    GTK3G3DUIINSTANCEEDIT *gtk3ied = ( GTK3G3DUIINSTANCEEDIT * ) user_data;
    G3DUI *gui = gtk3ied->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ied->core.gui->lock ) return;

    g3duiinstanceedit_mirroredToggleCbk ( gtk3ied );

    gtk3_g3duiinstanceedit_update ( gtk3ied );
}

/******************************************************************************/
static void objectSelectorCbk ( GtkWidget *widget,
                                gpointer   user_data ) {
    GTK3G3DUIINSTANCEEDIT *gtk3ied = ( GTK3G3DUIINSTANCEEDIT * ) user_data;
    G3DUI *gui = gtk3ied->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    uint32_t rank = gtk_combo_box_get_active ( GTK_COMBO_BOX(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ied->core.gui->lock ) return;

    g3duiinstanceedit_setReferenceCbk ( gtk3ied, rank );
}

/******************************************************************************/
static void orientationCbk ( GtkWidget *widget,
                             gpointer   user_data ) {
    GTK3G3DUIINSTANCEEDIT *gtk3ied = ( GTK3G3DUIINSTANCEEDIT * ) user_data;
    G3DUI *gui = gtk3ied->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    const char *str = gtk_combo_box_text_get_active_text ( GTK_COMBO_BOX_TEXT(widget) );

    /*** prevents loop and possibly lock reset if some panels are updated ***/
    if ( gtk3ied->core.gui->lock ) return;

    g3duiinstanceedit_orientationCbk ( gtk3ied, str );
}

/******************************************************************************/
void gtk3_g3duiinstanceedit_update ( GTK3G3DUIINSTANCEEDIT *gtk3ied ) {
    G3DUI *gui = gtk3ied->core.gui;

    gui->lock = 0x01;

    if ( gui->sce ) {
        G3DSCENE *sce = gui->sce;
        uint32_t nbsel = list_count ( sce->lsel );

        if ( nbsel ) {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3ied->notebook), TRUE );

            if ( g3dobjectlist_checkType ( sce->lsel, G3DINSTANCETYPE ) ) {
                gtk3ied->core.multi = ( nbsel > 0x01 ) ? 0x01 : 0x00;

                gtk3ied->core.editedInstance = ( G3DFFD * ) g3dscene_getLastSelected ( sce );

                if ( gtk3ied->core.editedInstance ) {
                    updateGeneralPanel  ( gtk3ied );
                }
            }
        } else {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3ied->notebook), FALSE );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void createGeneralPanel ( GTK3G3DUIINSTANCEEDIT *gtk3ied,
                                 gint                   x,
                                 gint                   y,
                                 gint                   width,
                                 gint                   height ) {
    GtkFixed *pan = ui_createTab ( gtk3ied->notebook,
                                   gtk3ied,
                                   EDITINSTANCEGENERAL,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3ied->objectSelector = ui_createSelector ( pan,
                                                  gtk3ied,
                                                  EDITINSTANCEOBJECTSELECTOR,
                                                  CLASS_MAIN,
                                                  0,  0, 96, 96, 20,
                                                  objectSelectorCbk );

    gtk3ied->mirroredToggle = ui_createToggleLabel ( pan,
                                                     gtk3ied,
                                                     EDITINSTANCEMIRRORED,
                                                     CLASS_MAIN,
                                                     0,  24, 96, 20, 20,
                                                     mirroredToggleCbk );

    gtk3ied->mirroredPlanSelector = ui_createPlaneSelector ( pan, 
                                                             gtk3ied, 
                                                             EDITINSTANCEMIRRORINGPLANE,  
                                                             CLASS_MAIN,
                                                             0, 48, 96, 96, 20,
                                                             orientationCbk );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIINSTANCEEDIT *gtk3ied = ( GTK3G3DUIINSTANCEEDIT * ) user_data;

    free ( gtk3ied );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIINSTANCEEDIT *gtk3ied = ( GTK3G3DUIINSTANCEEDIT * ) user_data;

    gtk3_g3duiinstanceedit_update ( gtk3ied );
}

/******************************************************************************/
GTK3G3DUIINSTANCEEDIT *gtk3_g3duiinstanceedit_create ( GtkWidget *parent,
                                                       GTK3G3DUI *gtk3gui,
                                                       char      *name ) {
    GTK3G3DUIINSTANCEEDIT *gtk3ied = gtk3_g3duiinstanceedit_new ( gtk3gui );
    GtkWidget *notebook = gtk_notebook_new ( );

    gtk3ied->notebook = GTK_NOTEBOOK(notebook);

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(notebook), TRUE );

    gtk_widget_set_name ( notebook, name );

    /*gtk_widget_set_size_request ( tab, width, height );*/

    g_signal_connect ( G_OBJECT (notebook), "realize", G_CALLBACK (Realize), gtk3ied );
    g_signal_connect ( G_OBJECT (notebook), "destroy", G_CALLBACK (Destroy), gtk3ied );

    createGeneralPanel ( gtk3ied, 0, 0, 310, 150 );


    gtk_widget_show ( notebook );


    return gtk3ied;
}
