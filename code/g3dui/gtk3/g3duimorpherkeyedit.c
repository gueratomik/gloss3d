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

#define EDITMORPHERKEY       "Morpher Poses"
#define EDITMORPHERPOSEFRAME "Morpher Poses"
#define EDITMORPHERPOSELIST  "Morpher Pose List"

/******************************************************************************/
typedef struct _MESHPOSEDATA {
    GTK3G3DUIMORPHERKEYEDIT *gtk3ked;
    G3DMORPHERMESHPOSE      *mpose;
} MESHPOSEDATA;

/******************************************************************************/
static MESHPOSEDATA *meshposedata_new ( GTK3G3DUIMORPHERKEYEDIT *gtk3ked,
                                        G3DMORPHERMESHPOSE      *mpose ) {
    MESHPOSEDATA *mpd = calloc ( 0x01, sizeof ( MESHPOSEDATA ) );

    mpd->gtk3ked = gtk3ked;
    mpd->mpose   = mpose;

    return mpd;
}

/******************************************************************************/
static void meshposedata_free ( MESHPOSEDATA *mpd ) {
    free ( mpd );
}

/******************************************************************************/
static GTK3G3DUIMORPHERKEYEDIT *gtk3_g3duimorpherkeyedit_new ( GTK3G3DUI *gtk3gui,
                                                               G3DKEY    *key ) {
    GTK3G3DUIMORPHERKEYEDIT *gtk3ked = calloc ( 0x01, sizeof ( GTK3G3DUIMORPHERKEYEDIT ) );

    if ( gtk3ked == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3ked->core.gui = ( G3DUI * ) gtk3gui;
    gtk3ked->core.key = key;


    return gtk3ked; 
}

/******************************************************************************/
static void rateCbk ( GtkWidget *widget, gpointer user_data ) {
    const char *field_name = gtk_widget_get_name ( widget );
    float val = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    MESHPOSEDATA *mpd = ( MESHPOSEDATA * ) user_data;

    /*** prevents a loop ***/
    if ( mpd->gtk3ked->core.gui->lock ) return;

    if ( mpd->gtk3ked->core.gui->lock == 0x00 ) {
        g3dmorpherkey_setMeshPoseRate ( mpd->gtk3ked->core.key,
                                        mpd->mpose->slotID, val / 100.0f );
    }
}

/******************************************************************************/
static void selectMeshPoseCbk ( GtkWidget *widget, 
                                gpointer   user_data ) {
    gboolean active = gtk_toggle_button_get_active (widget);
    MESHPOSEDATA *mpd = ( MESHPOSEDATA * ) user_data;

    /*** prevents a loop ***/
    if ( mpd->gtk3ked->core.gui->lock ) return;

    if ( active == TRUE  ) {
        g3dmorpherkey_enableMeshPose  ( mpd->gtk3ked->core.key, 
                                        mpd->mpose->slotID );
    }

    if ( active == FALSE ) {
        g3dmorpherkey_disableMeshPose ( mpd->gtk3ked->core.key, 
                                        mpd->mpose->slotID );
    }
}

/******************************************************************************/
static void destroyMeshPoseCbk ( GtkWidget *widget,
                                 gpointer   user_data ) {
    MESHPOSEDATA *mpd = ( MESHPOSEDATA * ) user_data;

    meshposedata_free ( mpd );
}

/******************************************************************************/
static void creatPoseList ( GTK3G3DUIMORPHERKEYEDIT *gtk3ked,
                            gint                     x,
                            gint                     y,
                            gint                     width,
                            gint                     height ) {
    G3DUI *gui = gtk3ked->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );
    GtkWidget *scrolled = ui_gtk_scrolled_window_new ( CLASS_MAIN, NULL, NULL );
    GtkWidget *fixed = ui_gtk_fixed_new ( CLASS_MAIN );

    gui->lock = 0x01;

    gtk_container_add( GTK_CONTAINER(scrolled), fixed );

    gtk_fixed_put ( gtk3ked->fixed, scrolled, x, y );

    gtk_widget_set_size_request ( scrolled, width, height );

    gtk3ked->poseList = fixed;

    if ( obj ) {
        if ( obj->type == G3DMORPHERTYPE ) {
            G3DMORPHER *mpr = ( G3DMORPHER * ) obj;
            LIST *ltmpmpose = mpr->poseList;
            uint32_t y = 0x00;

            while ( ltmpmpose ) {
                G3DMORPHERMESHPOSE *mpose = ( G3DMORPHERMESHPOSE * ) ltmpmpose->data;
                MESHPOSEDATA *mpd = meshposedata_new ( gtk3ked, mpose );
                GtkWidget *checkButton = ui_gtk_check_button_new_with_label ( CLASS_MAIN, mpose->name );
                GtkWidget *rateEntry = ui_createFloatText ( gtk3ked->poseList,
                                                            mpd,
                                                            "NONAME",
                                                            CLASS_MAIN,
                                                            0.0f, 100.0f,
                                                            0,  y, 0, 96, 20,
                                                            rateCbk );
                float rate;

                gtk_widget_set_size_request ( checkButton, 20, 20 );

                gtk_fixed_put ( gtk3ked->poseList, 
                                checkButton, 
                                96,
                                y );

                g_signal_connect ( checkButton, "toggled", G_CALLBACK( selectMeshPoseCbk  ), mpd );
                g_signal_connect ( checkButton, "destroy", G_CALLBACK( destroyMeshPoseCbk ), mpd );

                if ( g3dmorpherkey_isMeshPoseEnabled ( gtk3ked->core.key, mpose->slotID ) ) {
                    gtk_toggle_button_set_active ( checkButton, TRUE );
                } else {
                    gtk_toggle_button_set_active ( checkButton, FALSE );
                }

                rate = g3dmorpherkey_getMeshPoseRate ( gtk3ked->core.key, 
                                                       mpose->slotID ) * 100.0f;

                gtk_spin_button_set_value ( rateEntry, rate );

                y += 24;

                gtk_widget_show ( checkButton );

                ltmpmpose = ltmpmpose->next;
            }
        }
    }

    gui->lock = 0x00;

    gtk_widget_show_all ( scrolled );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIMORPHERKEYEDIT *gtk3ked = ( GTK3G3DUIMORPHERKEYEDIT * ) user_data;

    free ( gtk3ked );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIMORPHERKEYEDIT *gtk3ked = ( GTK3G3DUIMORPHERKEYEDIT * ) user_data;

}

/******************************************************************************/
GTK3G3DUIMORPHERKEYEDIT *gtk3_g3duimorpherkeyedit_create ( GtkWidget *parent,
                                                           GTK3G3DUI *gtk3gui,
                                                           char      *name,
                                                           G3DKEY    *key ) {
    GTK3G3DUIMORPHERKEYEDIT *gtk3ked = gtk3_g3duimorpherkeyedit_new ( gtk3gui,
                                                                      key );
    GtkWidget *fixed = ui_gtk_fixed_new ( CLASS_MAIN );

    gtk3ked->fixed = fixed;

    gtk_widget_set_name ( fixed, name );

    g_signal_connect ( G_OBJECT (fixed), "realize", G_CALLBACK (Realize), gtk3ked );
    g_signal_connect ( G_OBJECT (fixed), "destroy", G_CALLBACK (Destroy), gtk3ked );

    creatPoseList ( gtk3ked, 0, 0, 400, 200 );

    gtk_widget_show ( fixed );


    return gtk3ked;
}
