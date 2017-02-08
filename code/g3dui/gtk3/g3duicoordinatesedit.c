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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2015         */
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
static uint32_t getAbsolute ( GtkWidget *widget ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    while ( children ) {
        GtkWidget *child = children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( GTK_IS_TOGGLE_BUTTON ( child ) ) {
            if ( strcmp ( child_name, EDITABSOLUTE ) == 0x00 ) {
                return gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON ( child ) );
            } 
        }

        children =  g_list_next ( children );
    }

    return 0x00;
}

/******************************************************************************/
static void posCbk ( GtkWidget *widget, gpointer user_data ) {
    uint32_t absolute = getAbsolute ( gtk_widget_get_parent ( widget )  );
    float val = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    const char *name = gtk_widget_get_name ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( strcmp ( name, EDITXPOSITION ) == 0x00 ) {
        common_g3duicoordinatesedit_posCbk ( gui, G3DUIXAXIS, absolute, val );
    }

    if ( strcmp ( name, EDITYPOSITION ) == 0x00 ) {
        common_g3duicoordinatesedit_posCbk ( gui, G3DUIYAXIS, absolute, val );
    }

    if ( strcmp ( name, EDITZPOSITION ) == 0x00 ) {
        common_g3duicoordinatesedit_posCbk ( gui, G3DUIZAXIS, absolute, val );
    }
}

/******************************************************************************/
static void rotCbk ( GtkWidget *widget, gpointer user_data ) {
    float val = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    const char *name = gtk_widget_get_name ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( strcmp ( name, EDITXROTATION ) == 0x00 ) {
        common_g3duicoordinatesedit_rotCbk ( gui, G3DUIXAXIS, val );
    }

    if ( strcmp ( name, EDITYROTATION ) == 0x00 ) {
        common_g3duicoordinatesedit_rotCbk ( gui, G3DUIYAXIS, val );
    }

    if ( strcmp ( name, EDITZROTATION ) == 0x00 ) {
        common_g3duicoordinatesedit_rotCbk ( gui, G3DUIZAXIS, val );
    }
}

/******************************************************************************/
static void scaCbk ( GtkWidget *widget, gpointer user_data ) {
    float val = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    const char *name = gtk_widget_get_name ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( strcmp ( name, EDITXSCALING ) == 0x00 ) {
        common_g3duicoordinatesedit_scaCbk ( gui, G3DUIXAXIS, val );
    }

    if ( strcmp ( name, EDITYSCALING ) == 0x00 ) {
        common_g3duicoordinatesedit_scaCbk ( gui, G3DUIYAXIS, val );
    }

    if ( strcmp ( name, EDITZSCALING ) == 0x00 ) {
        common_g3duicoordinatesedit_scaCbk ( gui, G3DUIZAXIS, val );
    }
}

/******************************************************************************/
static void absoluteCbk ( GtkWidget *widget, gpointer user_data ) {
    gboolean absolute = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;
}

/******************************************************************************/
void updateCoordinatesEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = children->data;
        const char *child_name = gtk_widget_get_name ( child );
        char buf[0x10];

        if ( obj ) {
            gtk_widget_set_sensitive ( child, TRUE );

            if ( GTK_IS_SPIN_BUTTON ( child ) ) {
                GtkSpinButton *spb = GTK_SPIN_BUTTON(child);
                G3DVECTOR pos = { .x = obj->pos.x, 
                                  .y = obj->pos.y,
                                  .z = obj->pos.z },
                          rot = { .x = obj->rot.x, 
                                  .y = obj->rot.y,
                                  .z = obj->rot.z },
                          sca = { .x = obj->sca.x, 
                                  .y = obj->sca.y,
                                  .z = obj->sca.z };

                if ( obj->type == G3DMESHTYPE ) {
                    G3DMESH *mes = ( G3DMESH * ) obj;

                    if ( gui->flags & VIEWVERTEX ) {
                        g3dvertex_getAveragePositionFromList ( mes->lselver, &pos );
                    }
                }

                /****************** Position *****************/
                if ( strcmp ( child_name, EDITXPOSITION ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, pos.x );
                }

                if ( strcmp ( child_name, EDITYPOSITION ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, pos.y );
                }

                if ( strcmp ( child_name, EDITZPOSITION ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, pos.z );
                }

                /****************** Rotation *****************/
                if ( strcmp ( child_name, EDITXROTATION ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, rot.x );
                }

                if ( strcmp ( child_name, EDITYROTATION ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, rot.y );
                }

                if ( strcmp ( child_name, EDITZROTATION ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, rot.z );
                }

                /****************** Scaling  *****************/
                if ( strcmp ( child_name, EDITXSCALING  ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, sca.x );
                }

                if ( strcmp ( child_name, EDITYSCALING  ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, sca.y );
                }

                if ( strcmp ( child_name, EDITZSCALING  ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, sca.z );
                }
            }
        } else {
            gtk_widget_set_sensitive ( child, FALSE );
        }


        children =  g_list_next ( children );
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    list_remove ( &gui->lcoordedit, widget );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    updateCoordinatesEdit ( widget, gui );
}

/******************************************************************************/
GtkWidget* createCoordinatesEdit ( GtkWidget *parent, G3DUI *gui,
                                                      char *name,
                                                      gint x,
                                                      gint y,
                                                      gint width,
                                                      gint height ) {
    GdkRectangle gdkrec = { 0, 0, width, height };
    GtkWidget *frm = gtk_fixed_new ( );
    GtkWidget *label = gtk_label_new ( name );

    gtk_widget_set_name ( frm, name );

    gtk_notebook_append_page ( GTK_NOTEBOOK(parent), frm, label );

    gtk_widget_show ( label );

    /*gtk_widget_size_allocate ( frm, &gdkrec );*/

    gtk_widget_set_size_request ( frm, gdkrec.width, gdkrec.height );

    /*** For some reason, GtkSpinButtons calls its callbacks ***/
    /*** when being realized. With this trick I bypass that. ***/
    /*** Callbacks will return prematurely if gui->lock == 0x01 ***/
    gui->lock = 0x01;

    createSimpleLabel ( frm, gui, "Position"   ,   0,  0, 0, 48 );
    createFloatText   ( frm, gui, EDITXPOSITION,   0, 24, 0, 48, posCbk );
    createFloatText   ( frm, gui, EDITYPOSITION,   0, 48, 0, 48, posCbk );
    createFloatText   ( frm, gui, EDITZPOSITION,   0, 72, 0, 48, posCbk );

    createSimpleLabel ( frm, gui, "Rotation"   ,  96,  0, 0, 48 );
    createFloatText   ( frm, gui, EDITXROTATION,  96, 24, 0, 48, rotCbk );
    createFloatText   ( frm, gui, EDITYROTATION,  96, 48, 0, 48, rotCbk );
    createFloatText   ( frm, gui, EDITZROTATION,  96, 72, 0, 48, rotCbk );

    createSimpleLabel ( frm, gui, "Scaling"    , 192,  0, 0, 48 );
    createFloatText   ( frm, gui, EDITXSCALING , 192, 24, 0, 48, scaCbk );
    createFloatText   ( frm, gui, EDITYSCALING , 192, 48, 0, 48, scaCbk );
    createFloatText   ( frm, gui, EDITZSCALING , 192, 72, 0, 48, scaCbk );

    createToggleLabel ( frm, gui, EDITABSOLUTE,    0, 96, 96, 20, absoluteCbk );


    gui->lock = 0x00;

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), gui );
    g_signal_connect ( G_OBJECT (frm), "destroy", G_CALLBACK (Destroy), gui );


    list_insert ( &gui->lcoordedit, frm );

    gtk_widget_show_all ( frm );


    return frm;
}
