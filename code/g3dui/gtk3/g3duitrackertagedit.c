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
#include <inttypes.h>

#define EDITRACKERTARGET      "Target"
#define EDITRACKERORIENTATION "Orientation"

/******************************************************************************/
static GTK3G3DUITRACKERTAGEDIT *gtk3_g3duitrackertagedit_new ( GTK3G3DUI *gtk3gui, G3DOBJECT *obj ) {
    GTK3G3DUITRACKERTAGEDIT *gtk3tted = calloc ( 0x01, sizeof ( GTK3G3DUITRACKERTAGEDIT ) );

    if ( gtk3tted == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3tted->core.gui = ( G3DUI * ) gtk3gui;
    gtk3tted->core.obj = obj;

    return gtk3tted; 
}

/******************************************************************************/
void gtk3_g3duitrackertagedit_update ( GTK3G3DUITRACKERTAGEDIT *gtk3tted ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3tted->core.gui;
    G3DUI *gui = &gtk3gui->core;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *sel = gtk3tted->core.obj;

    gtk3tted->core.gui->lock = 0x01;

    gtk_combo_box_text_remove_all ( gtk3tted->objectCombo );

    if ( sel ) {
        G3DTAG *tag = sel->selectedTag;

        if ( tag ) {
            if ( tag->type & G3DTAGTRACKERTYPE ) {
                G3DTRACKERTAG *ttag = tag;
                LIST *ltmpobj, *lobj = NULL;
                uint32_t objRank = 0x00;
                int selected = 0x00;

                gtk_combo_box_set_active ( GTK_COMBO_BOX(gtk3tted->orientationCombo), ttag->orientation );

                /*** flatten the object tree ***/
                g3dobject_treeToList_r ( sce, &lobj );

                ltmpobj = lobj;

                while ( ltmpobj ) {
                    G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

                    if ( ( obj != sel ) && ( obj != sce ) ) {
                        char ptrstr[0x10];

                        snprintf ( ptrstr, 0x10, "%" PRIu64, obj );

                        gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(gtk3tted->objectCombo), ptrstr, obj->name );

                        if ( ttag->target == obj ) {
                            gtk_combo_box_set_active ( GTK_COMBO_BOX(gtk3tted->objectCombo), objRank );

                            selected = 0x01;
                        }

                        objRank++;
                    }

                    ltmpobj = ltmpobj->next;
                }

                gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(gtk3tted->objectCombo), NULL, "None" );

                if ( selected == 0x00 ) {
                    gtk_combo_box_set_active ( GTK_COMBO_BOX(gtk3tted->objectCombo), objRank );
                }

                list_free ( &lobj, NULL );
            }
        }
    }

    gtk3tted->core.gui->lock = 0x00;
}

/******************************************************************************/
static void objectSelectorCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITRACKERTAGEDIT *gtk3tted = ( GTK3G3DUITRACKERTAGEDIT * ) user_data;
    GtkComboBox *cmb = GTK_COMBO_BOX(widget);
    char *ptrstr = gtk_combo_box_get_active_id ( cmb );
    G3DOBJECT *obj;

    /*** prevents a loop ***/
    if ( gtk3tted->core.gui->lock ) return;

    if ( ptrstr ) {
        sscanf ( ptrstr, "%" PRIu64, &obj );

        g3duitrackertagedit_setTarget ( gtk3tted, obj );
    } else {
        g3duitrackertagedit_setTarget ( gtk3tted, NULL );
    }
}

/******************************************************************************/
static void orientationCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITRACKERTAGEDIT *gtk3tted = ( GTK3G3DUITRACKERTAGEDIT * ) user_data;
    GtkComboBoxText *cmbt = GTK_COMBO_BOX_TEXT(widget);
    const char *str = gtk_combo_box_text_get_active_text ( cmbt );

    /*** prevents a loop ***/
    if ( gtk3tted->core.gui->lock ) return;

    g3duitrackertagedit_orientation ( gtk3tted, str );
}


/******************************************************************************/
static void createObjectSelector ( GTK3G3DUITRACKERTAGEDIT *gtk3tted,
                                   gint                     x,
                                   gint                     y,
                                   gint                     labwidth,
                                   gint                     txtwidth,
                                   gint                     txtheight ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3tted->core.gui;

    gtk3tted->objectCombo = ui_createSelector ( gtk3tted->fixed, 
                                                gtk3tted,
                                                EDITRACKERTARGET,
                                                CLASS_MAIN,
                                                x, 
                                                y,
                                                labwidth,
                                                txtwidth,
                                                txtheight,
                                                objectSelectorCbk );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITRACKERTAGEDIT *gtk3tted = ( GTK3G3DUITRACKERTAGEDIT * ) user_data;

    free ( gtk3tted );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITRACKERTAGEDIT *gtk3tted = ( GTK3G3DUITRACKERTAGEDIT * ) user_data;

    gtk3_g3duitrackertagedit_update ( gtk3tted );
}

/******************************************************************************/
GTK3G3DUITRACKERTAGEDIT* gtk3_g3duitrackertagedit_create ( GtkWidget *parent,
                                                           GTK3G3DUI *gtk3gui,
                                                           char      *name,
                                                           G3DOBJECT *obj ) {
    GTK3G3DUITRACKERTAGEDIT *gtk3tted = gtk3_g3duitrackertagedit_new ( gtk3gui,
                                                                       obj );
    GtkWidget *fixed = ui_gtk_fixed_new ( CLASS_MAIN );

    gtk3tted->fixed = fixed;

    gtk_widget_set_name ( fixed, name );

    gtk_widget_set_size_request ( fixed, 264, 48 );

    createObjectSelector ( gtk3tted, 0, 0, 96, 96, 20 );

    gtk3tted->orientationCombo = ui_createAxisSelector   ( fixed, gtk3tted, EDITRACKERORIENTATION, CLASS_MAIN, 0, 24, 96, 96, 20, orientationCbk );

    g_signal_connect ( G_OBJECT (fixed), "realize", G_CALLBACK (Realize), gtk3tted );
    g_signal_connect ( G_OBJECT (fixed), "destroy", G_CALLBACK (Destroy), gtk3tted );


    gtk_widget_show_all ( fixed );


    return gtk3tted;
}
