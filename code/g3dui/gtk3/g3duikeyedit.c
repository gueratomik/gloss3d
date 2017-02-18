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
static void usePosCbk ( GtkWidget *widget, gpointer user_data ) {
    gboolean rpos = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( rpos ) common_g3duikeyedit_setFlagCbk   ( gui, KEYPOSITION );
    else        common_g3duikeyedit_unsetFlagCbk ( gui, KEYPOSITION );
}

/******************************************************************************/
static void useRotCbk ( GtkWidget *widget, gpointer user_data ) {
    gboolean rrot = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( rrot ) common_g3duikeyedit_setFlagCbk   ( gui, KEYROTATION );
    else        common_g3duikeyedit_unsetFlagCbk ( gui, KEYROTATION );
}

/******************************************************************************/
static void useScaCbk ( GtkWidget *widget, gpointer user_data ) {
    gboolean rsca = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( rsca ) common_g3duikeyedit_setFlagCbk   ( gui, KEYSCALING );
    else        common_g3duikeyedit_unsetFlagCbk ( gui, KEYSCALING );
}

/******************************************************************************/
static void loopFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    float frm = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duikeyedit_loopFrameCbk ( gui, frm );
}

/******************************************************************************/
static void loopCbk ( GtkWidget *widget, gpointer user_data ) {
    gboolean loop = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duikeyedit_loopCbk ( gui, loop );
}

/******************************************************************************/
static void keyCbk ( GtkWidget *widget, gpointer user_data ) {
    const char *field_name = gtk_widget_get_name ( widget );
    float val = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duikeyedit_keyCbk ( gui, field_name, val );
}

/******************************************************************************/
void updateKeyEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevents a loop ***/
    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = children->data;
        const char *child_name = gtk_widget_get_name ( child );
        char buf[0x10];

        if ( obj && obj->lselkey ) {
            G3DVECTOR keypos, keyrot, keysca;
            uint32_t keyloop = 0x00;
            float loopFrame;
            uint32_t loopFlag;
            uint32_t usePos = g3dkey_getUsePositionFromList ( obj->lselkey );
            uint32_t useRot = g3dkey_getUseRotationFromList ( obj->lselkey );
            uint32_t useSca = g3dkey_getUseScalingFromList  ( obj->lselkey );

            loopFlag = g3dkey_getLoopFrameFromList ( obj->lselkey, &loopFrame );

            g3dkey_getTransformationFromList ( obj->lselkey, &keypos,
                                                             &keyrot,
                                                             &keysca );

            if ( GTK_IS_CHECK_BUTTON ( child ) ) {
                GtkToggleButton *tgb = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITKEYLOOP ) == 0x00 ) {
                    if ( loopFlag ) {
                        gtk_toggle_button_set_active ( tgb, True );
                    } else {
                        gtk_toggle_button_set_active ( tgb, False );
                    }
                }

                if ( strcmp ( child_name, EDITKEYPOSITION ) == 0x00 ) {
                    gtk_toggle_button_set_active ( tgb, usePos );
                }

                if ( strcmp ( child_name, EDITKEYROTATION ) == 0x00 ) {
                    gtk_toggle_button_set_active ( tgb, useRot );
                }

                if ( strcmp ( child_name, EDITKEYSCALING ) == 0x00 ) {
                    gtk_toggle_button_set_active ( tgb, useSca );
                }
            }

            if ( GTK_IS_SPIN_BUTTON ( child ) ) {
                GtkSpinButton *spb = GTK_SPIN_BUTTON(child);

                if ( strcmp ( child_name, EDITKEYLOOPFRAME ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, loopFrame );
                }

                /****************** Position *****************/
                if ( strcmp ( child_name, EDITKEYXPOSITION ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, keypos.x );
                }

                if ( strcmp ( child_name, EDITKEYYPOSITION ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, keypos.y );
                }

                if ( strcmp ( child_name, EDITKEYZPOSITION ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, keypos.z );
                }

                /****************** Rotation *****************/
                if ( strcmp ( child_name, EDITKEYXROTATION ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, keyrot.x );
                }

                if ( strcmp ( child_name, EDITKEYYROTATION ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, keyrot.y );
                }

                if ( strcmp ( child_name, EDITKEYZROTATION ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, keyrot.z );
                }

                /****************** Scaling  *****************/
                if ( strcmp ( child_name, EDITKEYXSCALING  ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, keysca.x );
                }

                if ( strcmp ( child_name, EDITKEYYSCALING  ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, keysca.y );
                }

                if ( strcmp ( child_name, EDITKEYZSCALING  ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, keysca.z );
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

    list_remove ( &gui->lkeyedit, widget );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    updateKeyEdit ( widget, gui );
}

/******************************************************************************/
GtkWidget* createKeyEdit ( GtkWidget *parent, G3DUI *gui,
                                              char *name,
                                              gint x,
                                              gint y,
                                              gint width,
                                              gint height ) {
    GdkRectangle gdkrec = { 0, 0, width, height };
    GtkWidget *frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    /*gtk_widget_size_allocate ( frm, &gdkrec );*/

    gtk_widget_set_size_request ( frm, gdkrec.width, gdkrec.height );

    /*** For some reason, GtkSpinButtons calls its callbacks ***/
    /*** when being realized. With this trick I bypass that. ***/
    /*** Callbacks will return prematurely if gui->lock == 0x01 ***/
    gui->lock = 0x01;

    /******** Translation values text fields *****************/

    createToggleLabel ( frm, gui, EDITKEYPOSITION ,   8, 0, 96, 20, usePosCbk );

    createFloatText   ( frm, gui, EDITKEYXPOSITION,   8, 24, 0, 96, keyCbk );
    createFloatText   ( frm, gui, EDITKEYYPOSITION,   8, 48, 0, 96, keyCbk );
    createFloatText   ( frm, gui, EDITKEYZPOSITION,   8, 72, 0, 96, keyCbk );

    /********** Rotation values text fields ****************/

    createToggleLabel ( frm, gui, EDITKEYROTATION , 160, 0, 96, 20, useRotCbk );

    createFloatText   ( frm, gui, EDITKEYXROTATION, 160, 24, 0, 96, keyCbk );
    createFloatText   ( frm, gui, EDITKEYYROTATION, 160, 48, 0, 96, keyCbk );
    createFloatText   ( frm, gui, EDITKEYZROTATION, 160, 72, 0, 96, keyCbk );

    /********** Scaling values text fields ****************/

    createToggleLabel ( frm, gui, EDITKEYSCALING  , 312, 0, 96, 20, useScaCbk );

    createFloatText   ( frm, gui, EDITKEYXSCALING , 312, 24, 0, 96, keyCbk );
    createFloatText   ( frm, gui, EDITKEYYSCALING , 312, 48, 0, 96, keyCbk );
    createFloatText   ( frm, gui, EDITKEYZSCALING , 312, 72, 0, 96, keyCbk );

    /*** Loop frame ***/

    createToggleLabel ( frm, gui, EDITKEYLOOP     ,   8, 104,
                                                    128,
                                                     20, loopCbk );

    createIntegerText ( frm, gui, EDITKEYLOOPFRAME, 160, 104,
                                                      0,
                                                     32, loopFrameCbk );

    gui->lock = 0x00;

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), gui );
    g_signal_connect ( G_OBJECT (frm), "destroy", G_CALLBACK (Destroy), gui );


    list_insert ( &gui->lkeyedit, frm );

    gtk_container_add ( GTK_CONTAINER(parent), frm );

    gtk_widget_show_all ( frm );


    return frm;
}
