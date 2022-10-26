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

#define EDITBONEGENERAL "General"
#define EDITBONELENGTH  "Length"
#define EDITBONERIGGING "Rigging"

/******************************************************************************/
static GTK3G3DUIBONEEDIT *gtk3_g3duiboneedit_new ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUIBONEEDIT *gtk3bed = calloc ( 0x01, sizeof ( GTK3G3DUIBONEEDIT ) );

    if ( gtk3bed == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3bed->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3bed; 
}

/******************************************************************************/
typedef struct _WEIGHTROUPDATA {
    G3DBONE           *bon;
    G3DMESH           *mes;
    G3DWEIGHTGROUP    *weigrp;
    GTK3G3DUIBONEEDIT *bedit;
} WEIGHTROUPDATA;

/******************************************************************************/
static void selectWeightgroupCbk ( GtkWidget *widget,
                                   gpointer   user_data ) {
    WEIGHTROUPDATA *wgd = ( WEIGHTROUPDATA * ) user_data;
    G3DSKIN *skn = g3dobject_getChildByType ( wgd->mes, G3DSKINTYPE );

    if ( skn ) {
        G3DRIG *rig = g3dbone_getRigBySkin ( wgd->bon, skn );

        if ( rig ) {
            if ( list_seek ( rig->lweightgroup, wgd->weigrp ) == NULL ) {
                g3dbone_addWeightGroup ( wgd->bon, skn, wgd->weigrp );
            } else {
                g3dbone_removeWeightGroup ( wgd->bon, skn, wgd->weigrp );
            }
        } else {
            g3dbone_addWeightGroup ( wgd->bon, skn, wgd->weigrp );
        }
    }
}

/******************************************************************************/
static void destroyWeightgroupCbk ( GtkWidget *widget,
                                    gpointer   user_data ) {
    WEIGHTROUPDATA *wgd = ( WEIGHTROUPDATA * ) user_data;

    free ( wgd );
}

/******************************************************************************/
static void updateRiggingPanel ( GTK3G3DUIBONEEDIT *gtk3bed ) {
    gtk3bed->core.gui->lock = 0x01;

    gtk_container_foreach ( GTK_CONTAINER ( gtk3bed->weightgroupFixed ),
                 ( void * ) gtk_widget_destroy,
                            NULL );

    if ( gtk3bed->core.editedBone ) {
        if ( gtk3bed->core.editedBone->obj.type == G3DBONETYPE ) {
            G3DBONE *bon = ( G3DBONE * ) gtk3bed->core.editedBone;
            G3DOBJECT *parent = ((G3DOBJECT*)bon)->parent;
            uint32_t y = 0x00;

            /*** go through every parent mesh ***/
            while ( parent ) {
                if ( parent->type == G3DMESHTYPE ) {
                    G3DMESH *mes =( G3DMESH * ) parent;
                    G3DSKIN *skn = g3dobject_getChildByType ( mes, 
                                                              G3DSKINTYPE );

                    if ( skn ) {
                        LIST *ltmpgrp = mes->lweigrp;

                        while ( ltmpgrp ) {
                            G3DWEIGHTGROUP *weigrp = ( G3DWEIGHTGROUP * ) ltmpgrp->data;
                            G3DRIG *rig = g3dbone_getRigBySkin ( bon, skn );
                            char buf[0x100];

                            snprintf ( buf, 0x100, "%s - %s", mes->obj.name, weigrp->name );

                            GtkWidget *checkButton = ui_gtk_check_button_new_with_label ( CLASS_MAIN, buf );
                            WEIGHTROUPDATA *wgd = calloc ( 0x01, sizeof ( WEIGHTROUPDATA ) );

                            wgd->bon    = bon;
                            wgd->mes    = mes;
                            wgd->weigrp = weigrp;
                            wgd->bedit  = &gtk3bed->core;

                            if ( rig ) {
                                if ( list_seek ( rig->lweightgroup, weigrp ) ) {
                                    gtk_toggle_button_set_active ( checkButton, TRUE  );
                                } else {
                                    gtk_toggle_button_set_active ( checkButton, FALSE );
                                }
                            }

                            gtk_fixed_put ( gtk3bed->weightgroupFixed, checkButton, 0, y );

                            gtk_widget_show ( checkButton );

                            gtk_widget_set_size_request ( checkButton, 308, 24 );

                            g_signal_connect ( checkButton, "toggled", G_CALLBACK( selectWeightgroupCbk  ), wgd );
                            g_signal_connect ( checkButton, "destroy", G_CALLBACK( destroyWeightgroupCbk ), wgd );

                            y += 24;

                            ltmpgrp = ltmpgrp->next;
                        }
                    } else {
                        char buf[0x100];

                        snprintf ( buf, 0x100, "%s - no Skin modifier found!", mes->obj.name );

                        GtkWidget *lab = ui_createSimpleLabel ( gtk3bed->weightgroupFixed,
                                                                NULL,
                                                                buf,
                                                                CLASS_WARNING,
                                                                0, 
                                                                y,
                                                                308,
                                                                24 );
                        y += 24;
                    }

                }

                parent = parent->parent;
            }
        }
    }


    gtk3bed->core.gui->lock = 0x00;
}

/******************************************************************************/
static void createRiggingPanel ( GTK3G3DUIBONEEDIT *gtk3bed,
                             gint               x,
                             gint               y,
                             gint               width,
                             gint               height ) {
    GtkFixed *pan = ui_createTab ( gtk3bed->notebook,
                                   gtk3bed,
                                   EDITBONERIGGING,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );
    GtkScrolledWindow *scrolled = ui_gtk_scrolled_window_new ( CLASS_MAIN, NULL, NULL );
    GtkFixed *fixed = ui_gtk_fixed_new ( CLASS_MAIN );

    gtk_container_add ( GTK_CONTAINER(scrolled), fixed    );

    gtk_widget_set_size_request ( scrolled, width, height );
    gtk_widget_set_size_request ( fixed   , width, height );

    gtk_fixed_put ( pan, scrolled, x, y );

    gtk3bed->weightgroupFixed = fixed;

    gtk_widget_show_all ( scrolled );
}

/******************************************************************************/
static void updateGeneralPanel ( GTK3G3DUIBONEEDIT *gtk3bed ) {
    gtk3bed->core.gui->lock = 0x01;

    if ( gtk3bed->core.editedBone ) {
        gtk_spin_button_set_value ( gtk3bed->lengthEntry, 
                                    gtk3bed->core.editedBone->len );
    }

    gtk3bed->core.gui->lock = 0x00;
}

/******************************************************************************/
static void lengthCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIBONEEDIT *gtk3bed = ( GTK3G3DUIBONEEDIT * ) user_data;
    float val = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    /*** prevents a loop ***/
    if ( gtk3bed->core.gui->lock ) return;

    g3duiboneedit_setLength ( &gtk3bed->core, val );
}

/******************************************************************************/
static void createGeneralPanel ( GTK3G3DUIBONEEDIT *gtk3bed,
                                 gint               x,
                                 gint               y,
                                 gint               width,
                                 gint               height ) {
    GtkFixed *pan = ui_createTab ( gtk3bed->notebook,
                                   gtk3bed,
                                   EDITBONEGENERAL,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3bed->lengthEntry = ui_createFloatText ( pan,
                                                gtk3bed,
                                                EDITBONELENGTH,
                                                CLASS_MAIN,
                                                0.0f, FLT_MAX,
                                                0,  0, 96, 96, 20,
                                                lengthCbk );
}

/******************************************************************************/
void gtk3_g3duiboneedit_update ( GTK3G3DUIBONEEDIT *gtk3bed ) {
    G3DUI *gui = gtk3bed->core.gui;

    gui->lock = 0x01;

    if ( gui->sce ) {
        G3DSCENE *sce = gui->sce;
        uint32_t nbsel = list_count ( sce->lsel );

        if ( nbsel ) {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3bed->notebook), TRUE );

            if ( g3dobjectlist_checkType ( sce->lsel, G3DBONETYPE ) ) {
                gtk3bed->core.multi = ( nbsel > 0x01 ) ? 0x01 : 0x00;

                gtk3bed->core.editedBone = ( G3DBONE * ) g3dscene_getLastSelected ( sce );

                if ( gtk3bed->core.editedBone ) {
                    updateGeneralPanel ( gtk3bed );
                    updateRiggingPanel ( gtk3bed );
                }
            }
        } else {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3bed->notebook), FALSE );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIBONEEDIT *gtk3bed = ( GTK3G3DUIBONEEDIT * ) user_data;

    free ( gtk3bed );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIBONEEDIT *gtk3bed = ( GTK3G3DUIBONEEDIT * ) user_data;

    gtk3_g3duiboneedit_update ( gtk3bed );
}

/******************************************************************************/
GTK3G3DUIBONEEDIT *gtk3_g3duiboneedit_create ( GtkWidget *parent,
                                               GTK3G3DUI *gtk3gui,
                                               char      *name ) {
    GTK3G3DUIBONEEDIT *gtk3bed = gtk3_g3duiboneedit_new ( gtk3gui );
    GtkWidget *notebook = ui_gtk_notebook_new ( CLASS_MAIN );

    gtk3bed->notebook = GTK_NOTEBOOK(notebook);

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(notebook), TRUE );

    gtk_widget_set_name ( notebook, name );

    g_signal_connect ( G_OBJECT (notebook), "realize", G_CALLBACK (Realize), gtk3bed );
    g_signal_connect ( G_OBJECT (notebook), "destroy", G_CALLBACK (Destroy), gtk3bed );

    createGeneralPanel ( gtk3bed, 0, 0, 310, 150 );
    createRiggingPanel ( gtk3bed, 0, 0, 310, 150 );

    gtk_widget_show ( notebook );


    return gtk3bed;
}
