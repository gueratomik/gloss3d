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
static GTK3G3DUIKEYEDIT *gtk3_g3duikeyedit_new ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUIKEYEDIT *gtk3ked = calloc ( 0x01, sizeof ( GTK3G3DUIKEYEDIT ) );

    if ( gtk3ked == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3ked->core.gui    = ( G3DUI * ) gtk3gui;


    return gtk3ked; 
}

/******************************************************************************/
static void editKeyDataCbk ( GtkWidget *widget,
                             gpointer   user_data ) {
    GTK3G3DUIKEYEDIT *gtk3ked = ( GTK3G3DUIKEYEDIT * ) user_data;
    G3DUI *gui = gtk3ked->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( gui->sce );

    if ( obj ) {
        if ( obj->type == G3DLIGHTTYPE ) {
            G3DLIGHT *lig = ( G3DLIGHT * ) obj;

            if ( obj->lselkey ) {
                GtkWidget *dial = ui_gtk_dialog_new ( CLASS_MAIN );
                GtkWidget *box = gtk_dialog_get_content_area ( dial );
                G3DKEY *key = obj->lselkey->data;
                G3DLIGHT *keylig = key->data.ptr;

                GTK3G3DUILIGHTEDIT *ledit = gtk3_g3duilightedit_create ( dial,
                                                                         gtk3gui,
                                                                         "LIGHTEDIT", 
                                                                         0x01 );

                gtk_container_add ( GTK_CONTAINER(box), ledit->notebook );

                gtk_widget_set_size_request ( ledit->notebook, 400, 200 );

                g_signal_connect_swapped ( dial,
                                           "response",
                                           G_CALLBACK (gtk_widget_destroy),
                                           dial);

                gtk_dialog_run ( dial );

                /* must be called after the realization of the widget */
                gtk3_g3duilightedit_update ( ledit, keylig );
            }
        }

        if ( obj->type == G3DPARTICLEEMITTERTYPE ) {
            G3DPARTICLEEMITTER *pem = ( G3DPARTICLEEMITTER * ) obj;

            if ( obj->lselkey ) {
                GtkWidget *dial = ui_gtk_dialog_new ( CLASS_MAIN );
                GtkWidget *box = gtk_dialog_get_content_area ( dial );
                G3DKEY *key = obj->lselkey->data;
                G3DLIGHT *keypem = key->data.ptr;

                GTK3G3DUILIGHTEDIT *pedit = gtk3_g3duiparticleemitteredit_create ( dial,
                                                                                   gtk3gui,
                                                                                   "PARTICLEEMITTEREDIT", 
                                                                                   0x01 );

                gtk_container_add ( GTK_CONTAINER(box), pedit->notebook );

                gtk_widget_set_size_request ( pedit->notebook, 400, 200 );

                g_signal_connect_swapped ( dial,
                                           "response",
                                           G_CALLBACK (gtk_widget_destroy),
                                           dial);

                gtk_dialog_run ( dial );

                /* must be called after the realization of the widget */
                gtk3_g3duiparticleemitteredit_update ( pedit, keypem );
            }
        }

        if ( obj->type == G3DMORPHERTYPE ) {
            G3DMORPHER *mpr = ( G3DMORPHER * ) obj;

            if ( obj->lselkey ) {
                GtkWidget *dial = ui_gtk_dialog_new ( CLASS_MAIN );
                GtkWidget *box = gtk_dialog_get_content_area ( dial );
                G3DKEY *key = obj->lselkey->data;

                GTK3G3DUIMORPHERKEYEDIT *kedit = gtk3_g3duimorpherkeyedit_create ( dial,
                                                                                   gtk3gui,
                                                                                   "MORPHERKEYEDIT", 
                                                                                   key );

                gtk_container_add ( GTK_CONTAINER(box), kedit->fixed );

                gtk_widget_set_size_request ( kedit->fixed, 400, 200 );

                g_signal_connect_swapped ( dial,
                                           "response",
                                           G_CALLBACK (gtk_widget_destroy),
                                           dial);

                gtk_dialog_run ( dial );
            }
        }
    }
}

/******************************************************************************/
static void useTrsCbk ( GtkWidget *widget, gpointer user_data ) {
    gboolean rpos = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON(widget) );
    GTK3G3DUIKEYEDIT *gtk3ked = ( GTK3G3DUIKEYEDIT * ) user_data;
    G3DUI *gui = gtk3ked->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;

    /*** prevents loop ***/
    if ( gtk3ked->core.gui->lock ) return;

    if ( rpos ) {
        g3duikeyedit_setKeyTransformations   ( &gtk3ked->core, KEYPOSITION );
    } else {
        g3duikeyedit_unsetKeyTransformations ( &gtk3ked->core, KEYPOSITION );
    }
}

/******************************************************************************/
static void useRotCbk ( GtkWidget *widget, gpointer user_data ) {
    gboolean rrot = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON(widget) );
    GTK3G3DUIKEYEDIT *gtk3ked = ( GTK3G3DUIKEYEDIT * ) user_data;
    G3DUI *gui = gtk3ked->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;

    /*** prevents loop ***/
    if ( gtk3ked->core.gui->lock ) return;

    if ( rrot ) {
        g3duikeyedit_setKeyTransformations   ( &gtk3ked->core, KEYROTATION );
    } else {
        g3duikeyedit_unsetKeyTransformations ( &gtk3ked->core, KEYROTATION );
    }
}

/******************************************************************************/
static void useScaCbk ( GtkWidget *widget, gpointer user_data ) {
    gboolean rsca = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON(widget) );
    GTK3G3DUIKEYEDIT *gtk3ked = ( GTK3G3DUIKEYEDIT * ) user_data;
    G3DUI *gui = gtk3ked->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;

    /*** prevents loop ***/
    if ( gtk3ked->core.gui->lock ) return;

    if ( rsca ) {
        g3duikeyedit_setKeyTransformations   ( &gtk3ked->core, KEYSCALING );
    } else {
        g3duikeyedit_unsetKeyTransformations ( &gtk3ked->core, KEYSCALING );
    }
}

/******************************************************************************/
static void useDatCbk ( GtkWidget *widget, gpointer user_data ) {
    gboolean rsca = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON(widget) );
    GTK3G3DUIKEYEDIT *gtk3ked = ( GTK3G3DUIKEYEDIT * ) user_data;
    G3DUI *gui = gtk3ked->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;

    /*** prevents loop ***/
    if ( gtk3ked->core.gui->lock ) return;

    if ( rsca ) {
       g3duikeyedit_setKeyTransformations   ( &gtk3ked->core, KEYDATA );
    } else {
        g3duikeyedit_unsetKeyTransformations ( &gtk3ked->core, KEYDATA );
    }
}

/******************************************************************************/
static void loopFrameCbk ( GtkWidget *widget, gpointer user_data ) {
    float frm = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GTK3G3DUIKEYEDIT *gtk3ked = ( GTK3G3DUIKEYEDIT * ) user_data;
    G3DUI *gui = gtk3ked->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    uint64_t ret;

    /*** prevents loop ***/
    if ( gtk3ked->core.gui->lock ) return;

    ret = g3duikeyedit_loopFrame ( &gtk3ked->core, frm );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );
}

/******************************************************************************/
static void loopCbk ( GtkWidget *widget, gpointer user_data ) {
    gboolean loop = gtk_toggle_button_get_active ( GTK_TOGGLE_BUTTON(widget) );
    GTK3G3DUIKEYEDIT *gtk3ked = ( GTK3G3DUIKEYEDIT * ) user_data;
    G3DUI *gui = gtk3ked->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    uint64_t ret;

    /*** prevents loop ***/
    if ( gtk3ked->core.gui->lock ) return;

    ret = g3duikeyedit_loop ( &gtk3ked->core, loop );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );
}

/******************************************************************************/
static void keyCbk ( GtkWidget *widget, gpointer user_data ) {
    const char *field_name = gtk_widget_get_name ( widget );
    float val = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GTK3G3DUIKEYEDIT *gtk3ked = ( GTK3G3DUIKEYEDIT * ) user_data;
    G3DUI *gui = gtk3ked->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    uint64_t ret;

    /*** prevents loop ***/
    if ( gtk3ked->core.gui->lock ) return;

    ret = g3duikeyedit_key ( &gtk3ked->core, field_name, val );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );
}

/******************************************************************************/
void gtk3_g3duikeyedit_update ( GTK3G3DUIKEYEDIT *gtk3ked ) {
    G3DUI *gui = gtk3ked->core.gui;

    gui->lock = 0x01;

    if ( gui->sce ) {
        G3DSCENE *sce = gui->sce;
        G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

        if ( obj && obj->lselkey ) {
            G3DVECTOR keypos, keyrot, keysca;
            uint32_t keyloop = 0x00;
            float loopFrame;
            uint32_t loopFlag;
            uint32_t usePos = g3dkey_getUsePositionFromList ( obj->lselkey );
            uint32_t useRot = g3dkey_getUseRotationFromList ( obj->lselkey );
            uint32_t useSca = g3dkey_getUseScalingFromList  ( obj->lselkey );
            uint32_t useDat = g3dkey_getUseDataFromList     ( obj->lselkey );

            loopFlag = g3dkey_getLoopFrameFromList ( obj->lselkey, &loopFrame );

            g3dkey_getTransformationFromList ( obj->lselkey, &keypos,
                                                             &keyrot,
                                                             &keysca );

            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3ked->fixed), TRUE );

            if ( loopFlag ) {
                gtk_toggle_button_set_active ( gtk3ked->loopToggle, True );
            } else {
                gtk_toggle_button_set_active ( gtk3ked->loopToggle, False );
            }

            gtk_toggle_button_set_active ( gtk3ked->translationToggle, usePos );
            gtk_toggle_button_set_active ( gtk3ked->rotationToggle   , useRot );
            gtk_toggle_button_set_active ( gtk3ked->scalingToggle    , useSca );
            gtk_toggle_button_set_active ( gtk3ked->keyDataToggle    , useDat );

            gtk_spin_button_set_value ( gtk3ked->loopFrameEntry, loopFrame );

            gtk_spin_button_set_value ( gtk3ked->XTranslationEntry, keypos.x );
            gtk_spin_button_set_value ( gtk3ked->YTranslationEntry, keypos.y );
            gtk_spin_button_set_value ( gtk3ked->ZTranslationEntry, keypos.z );

            gtk_spin_button_set_value ( gtk3ked->XRotationEntry, keyrot.x );
            gtk_spin_button_set_value ( gtk3ked->YRotationEntry, keyrot.y );
            gtk_spin_button_set_value ( gtk3ked->ZRotationEntry, keyrot.z );

            gtk_spin_button_set_value ( gtk3ked->XScalingEntry, keysca.x );
            gtk_spin_button_set_value ( gtk3ked->YScalingEntry, keysca.y );
            gtk_spin_button_set_value ( gtk3ked->ZScalingEntry, keysca.z );
        } else {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3ked->fixed), FALSE );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIKEYEDIT *gtk3ked = ( GTK3G3DUIKEYEDIT * ) user_data;

    free ( gtk3ked );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIKEYEDIT *gtk3ked = ( GTK3G3DUIKEYEDIT * ) user_data;

    gtk3_g3duikeyedit_update ( gtk3ked );
}

/******************************************************************************/
GTK3G3DUIKEYEDIT *gtk3_g3duikeyedit_create ( GtkWidget *parent,
                                             GTK3G3DUI *gtk3gui,
                                             char      *name ) {
    GTK3G3DUIKEYEDIT *gtk3ked = gtk3_g3duikeyedit_new ( gtk3gui );
    GtkFixed *fixed = ui_gtk_fixed_new ( CLASS_MAIN );

    gtk3ked->fixed = fixed;

    /*gtk_widget_set_size_request ( tab, width, height );*/

    g_signal_connect ( G_OBJECT (fixed), "realize", G_CALLBACK (Realize), gtk3ked );
    g_signal_connect ( G_OBJECT (fixed), "destroy", G_CALLBACK (Destroy), gtk3ked );


    /******** Translation values text fields *****************/

    gtk3ked->translationToggle  = ui_createToggleLabel ( fixed,
                                                         gtk3ked,
                                                         EDITKEYTRANSLATION,
                                                         CLASS_MAIN,
                                                           8, 0, 76, 20, 20,
                                                         useTrsCbk );

    gtk3ked->XTranslationEntry  = ui_createFloatText   ( fixed, 
                                                         gtk3ked,
                                                         EDITKEYXTRANSLATION,
                                                         CLASS_MAIN,
                                                         -FLT_MAX, FLT_MAX,
                                                         8, 24, 0, 96, 20,
                                                         keyCbk );

    gtk3ked->YTranslationEntry  = ui_createFloatText   ( fixed, 
                                                         gtk3ked,
                                                         EDITKEYYTRANSLATION,
                                                         CLASS_MAIN,
                                                         -FLT_MAX, FLT_MAX,
                                                         8, 48, 0, 96, 20,
                                                         keyCbk );

    gtk3ked->ZTranslationEntry  = ui_createFloatText   ( fixed, 
                                                         gtk3ked,
                                                         EDITKEYZTRANSLATION,
                                                         CLASS_MAIN,
                                                         -FLT_MAX, FLT_MAX,
                                                         8, 72, 0, 96, 20,
                                                         keyCbk );

    /************* Rotation values text fields ***************/

    gtk3ked->rotationToggle     = ui_createToggleLabel ( fixed,
                                                         gtk3ked,
                                                         EDITKEYROTATION,
                                                         CLASS_MAIN,
                                                         160, 0, 76, 20, 20,
                                                         useRotCbk );

    gtk3ked->XRotationEntry     = ui_createFloatText   ( fixed, 
                                                         gtk3ked,
                                                         EDITKEYXROTATION,
                                                         CLASS_MAIN,
                                                         -FLT_MAX, FLT_MAX,
                                                         160, 24, 0, 96, 20,
                                                         keyCbk );

    gtk3ked->YRotationEntry     = ui_createFloatText   ( fixed, 
                                                         gtk3ked,
                                                         EDITKEYYROTATION,
                                                         CLASS_MAIN,
                                                         -FLT_MAX, FLT_MAX,
                                                         160, 48, 0, 96, 20,
                                                         keyCbk );

    gtk3ked->ZRotationEntry     = ui_createFloatText   ( fixed, 
                                                         gtk3ked,
                                                         EDITKEYZROTATION,
                                                         CLASS_MAIN,
                                                         -FLT_MAX, FLT_MAX,
                                                         160, 72, 0, 96, 20,
                                                         keyCbk );

    /*********** Scaling values text fields ******************/

    gtk3ked->scalingToggle      = ui_createToggleLabel ( fixed,
                                                         gtk3ked,
                                                         EDITKEYSCALING,
                                                         CLASS_MAIN,
                                                         312, 0, 76, 20, 20,
                                                         useScaCbk );

    gtk3ked->XScalingEntry      = ui_createFloatText   ( fixed, 
                                                         gtk3ked,
                                                         EDITKEYXSCALING,
                                                         CLASS_MAIN,
                                                         -FLT_MAX, FLT_MAX,
                                                         312, 24, 0, 96, 20,
                                                         keyCbk );

    gtk3ked->YScalingEntry      = ui_createFloatText   ( fixed, 
                                                         gtk3ked,
                                                         EDITKEYYSCALING,
                                                         CLASS_MAIN,
                                                         -FLT_MAX, FLT_MAX,
                                                         312, 48, 0, 96, 20,
                                                         keyCbk );

    gtk3ked->ZScalingEntry      = ui_createFloatText   ( fixed, 
                                                         gtk3ked,
                                                         EDITKEYZSCALING,
                                                         CLASS_MAIN,
                                                         -FLT_MAX, FLT_MAX,
                                                         312, 72, 0, 96, 20,
                                                         keyCbk );

    /*********************** Loop Frame **********************/

    gtk3ked->loopToggle        = ui_createToggleLabel  ( fixed,
                                                         gtk3ked,
                                                         EDITKEYLOOP,
                                                         CLASS_MAIN,
                                                         8,  104, 128, 20, 20,
                                                         loopCbk );

    gtk3ked->loopFrameEntry    = ui_createIntegerText  ( fixed,
                                                         gtk3ked,
                                                         EDITKEYLOOPFRAME,
                                                         CLASS_MAIN,
                                                         INT_MIN, INT_MAX,
                                                         160, 104, 0, 32, 20,
                                                         loopFrameCbk );

    gtk3ked->keyDataToggle     = ui_createToggleLabel  ( fixed,
                                                         gtk3ked,
                                                         EDITKEYENABLEDATA,
                                                         CLASS_MAIN,
                                                         8, 128, 128, 20, 20,
                                                         useDatCbk );

    gtk3ked->keyDataButton     = ui_createPushButton   ( fixed,
                                                         gtk3ked,
                                                         EDITKEYDATA,
                                                         CLASS_MAIN,
                                                         160, 128, 96, 20,
                                                         editKeyDataCbk );

    gtk_widget_show_all ( fixed );


    return gtk3ked;
}
