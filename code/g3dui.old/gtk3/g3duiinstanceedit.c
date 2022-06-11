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
typedef struct _G3DUIINSTANCEEDIT {
    G3DUIWIDGETGROUP grp;
    GtkWidget       *mainFrame;
    GtkWidget       *objectSelector;
    GtkWidget       *mirroredToggle;
    GtkWidget       *mirroredPlanSelection;
    G3DINSTANCE     *editedInstance;
} G3DUIINSTANCEEDIT;

/******************************************************************************/
#define EDITINSTANCEOBJECTSELECTOR "Instanciated object"
#define EDITINSTANCEMIRRORED       "Mirrored"
#define EDITINSTANCEMIRRORINGPLANE "Mirroring plane"

/******************************************************************************/
static G3DUIINSTANCEEDIT *g3duiinstanceedit_new ( G3DUI *gui ) {
    G3DUIINSTANCEEDIT *ied = calloc ( 0x01, sizeof ( G3DUIINSTANCEEDIT ) );

    if ( ied == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );
    }

    ied->grp.gui = gui;

    return ied; 
}

/******************************************************************************/
static void mirroredToggleCbk  ( GtkWidget *widget, gpointer user_data ) {
    G3DUIINSTANCEEDIT *ied = ( G3DUIINSTANCEEDIT * ) user_data;

    common_g3duiinstanceedit_mirroredToggleCbk ( ied->grp.gui );

    updateInstanceEdit ( ied->mainFrame, ied->editedInstance );
}

/******************************************************************************/
static void objectSelectorCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIINSTANCEEDIT *ied = ( G3DUIINSTANCEEDIT * ) user_data;
    G3DUI *gui = ( G3DUI * ) ied->grp.gui;
    uint32_t rank = gtk_combo_box_get_active ( GTK_COMBO_BOX(widget) );

    common_g3duiinstanceedit_setReferenceCbk ( gui, rank );
}

/******************************************************************************/
static void orientationCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUIINSTANCEEDIT *ied = ( G3DUIINSTANCEEDIT * ) user_data;
    GtkComboBoxText *cmbt = GTK_COMBO_BOX_TEXT(widget);
    G3DUI *gui = ( G3DUI * ) ied->grp.gui;
    const char *str = gtk_combo_box_text_get_active_text ( cmbt );

    common_g3duiinstanceedit_orientationCbk ( gui, str );
}

/******************************************************************************/
static void populateObjectSelector ( GtkWidget *cmb,
                                     G3DUI     *gui ) {
    G3DSCENE *sce = gui->sce;
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
                        gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, obj->name );

                        if ( ins->ref == obj ) {
                            gtk_combo_box_set_active ( GTK_COMBO_BOX(cmb), objRank );

                            selected = 0x01;
                        }

                        objRank++;
                    }
                }

                ltmpobj = ltmpobj->next;
            }

            gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, "None" );

            if ( selected == 0x00 ) {
                gtk_combo_box_set_active ( GTK_COMBO_BOX(cmb), objRank );
            }

            list_free ( &lobj, NULL );
        }
    }
}

/******************************************************************************/
void updateInstanceEdit ( GtkWidget    *w, 
                          G3DINSTANCE  *ins ) {
    G3DUIINSTANCEEDIT *ied = ( G3DUIINSTANCEEDIT * ) g_object_get_data ( G_OBJECT(w),
                                                                         "private" );

    ied->editedInstance = ins;

    ied->grp.gui->lock = 0x01;

    if ( ied->editedInstance ) {
        gtk_widget_set_sensitive ( ied->objectSelector       , TRUE );
        gtk_widget_set_sensitive ( ied->mirroredToggle       , TRUE );

        if ( ((G3DOBJECT*)ied->editedInstance)->flags & INSTANCEMIRRORED ) {
            gtk_toggle_button_set_active ( ied->mirroredToggle, TRUE  );

            gtk_widget_set_sensitive ( ied->mirroredPlanSelection, TRUE );
        } else {
            gtk_toggle_button_set_active ( ied->mirroredToggle, FALSE );

            gtk_widget_set_sensitive ( ied->mirroredPlanSelection, FALSE );
        }

        populateObjectSelector ( ied->objectSelector, ied->grp.gui );
    } else {
        gtk_widget_set_sensitive ( ied->objectSelector       , FALSE );
        gtk_widget_set_sensitive ( ied->mirroredToggle       , FALSE );
        gtk_widget_set_sensitive ( ied->mirroredPlanSelection, FALSE );
    }

    ied->grp.gui->lock = 0x00;
}

/******************************************************************************/
static GtkWidget *createObjectSelector ( GtkWidget         *parent, 
                                         G3DUIINSTANCEEDIT *ied,
                                         char              *name,
                                         gint               x, 
                                         gint               y,
                                         gint               labwidth,
                                         gint               txtwidth,
                                         void               (*cbk)( GtkWidget *, 
                                                                    gpointer ) ) {

    GtkWidget     *cmb  = gtk_combo_box_text_new ( );
    GdkRectangle   crec = { 0x00, 0x00, txtwidth, 0x12 };
    G3DUI *gui = ( G3DUI * ) ied->grp.gui;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *sel = g3dscene_getLastSelected ( sce );

    gtk_widget_set_name ( cmb, name );

    gtk_widget_size_allocate ( cmb, &crec );

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
        g_signal_connect ( cmb, "changed", G_CALLBACK(cbk), ied );
    }

    populateObjectSelector ( cmb, gui );

    gtk_widget_show ( cmb );

    return cmb;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    G3DUIINSTANCEEDIT *ied = ( G3DUIINSTANCEEDIT * ) user_data;

    free ( ied );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUIINSTANCEEDIT *ied = ( G3DUIINSTANCEEDIT * ) user_data;

    updateInstanceEdit ( widget, NULL );
}

/******************************************************************************/
GtkWidget *createInstanceEdit ( GtkWidget *parent,
                                G3DUI     *gui,
                                char      *name,
                                gint       x,
                                gint       y,
                                gint       width,
                                gint       height ) {
    GdkRectangle gdkrec = { 0x00, 0x00, width, height };
    G3DUIINSTANCEEDIT *ied = g3duiinstanceedit_new ( gui );
    GtkWidget *frm = gtk_fixed_new ( );

    g_object_set_data ( G_OBJECT(frm), "private", (gpointer) ied );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), frm, x, y );

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), ied );
    g_signal_connect ( G_OBJECT (frm), "destroy", G_CALLBACK (Destroy), ied );

    ied->mainFrame = frm;

    ied->objectSelector = createObjectSelector ( frm,
                                                 ied,
                                                 EDITINSTANCEOBJECTSELECTOR,
                                                 0,  0, 96, 18,
                                                 objectSelectorCbk );

    ied->mirroredToggle = createToggleLabel ( frm,
                                              ied,
                                              EDITINSTANCEMIRRORED,
                                              0,  24, 192, 18,
                                              mirroredToggleCbk );

    ied->mirroredPlanSelection = createOrientationSelection ( frm, 
                                                              ied, 
                                                              EDITINSTANCEMIRRORINGPLANE,  
                                                              ZXSTR,
                                                              XYSTR,
                                                              YZSTR,
                                                              0, 48,
                                                              96, 96, orientationCbk );


    gtk_widget_show ( frm );


    return frm;
}
