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
#define EDITMORPHERKEY       "Morpher Poses"
#define EDITMORPHERPOSEFRAME "Morpher Poses"
#define EDITMORPHERPOSELIST  "Morpher Pose List"

typedef struct _MESHPOSEDATA {
    G3DMORPHER         *mpr;
    G3DMORPHERMESHPOSE *mpose;
    G3DKEY             *key;
    G3DUI              *gui;
} MESHPOSEDATA;

/******************************************************************************/
static void selectMeshPoseCbk ( GtkWidget *widget, 
                                gpointer   user_data ) {
    gboolean active = gtk_toggle_button_get_active (widget);
    MESHPOSEDATA *mpd = ( MESHPOSEDATA * ) user_data;
    G3DOBJECT *obj = g3dscene_getLastSelected ( mpd->gui->sce );

    if ( obj ) {
        if ( obj->type == G3DMORPHERTYPE ) {
            G3DMORPHER *mpr = ( G3DMORPHER * ) obj;

            if ( active == TRUE  ) mpd->key->data.u64 |=   mpd->mpose->slotBit;
            if ( active == FALSE ) mpd->key->data.u64 &= (~mpd->mpose->slotBit);
        }
    }

    /*g3dui_redrawGLViews ( fgd->gui );*/
}

/******************************************************************************/
static void destroyMeshPoseCbk ( GtkWidget *widget,
                                  gpointer   user_data ) {
    MESHPOSEDATA *mpd = ( MESHPOSEDATA * ) user_data;

    free ( mpd );
}

/******************************************************************************/
static void populateMeshPoseList ( GtkWidget *fixed,
                                   G3DKEY    *key,
                                   G3DUI     *gui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    gui->lock = 0x01;

    if ( obj ) {
        if ( obj->type == G3DMORPHERTYPE ) {
            GdkRectangle frec = { 0x00, 0x00, 0x00, 0x00 };
            G3DMORPHER *mpr = ( G3DMORPHER * ) obj;
            LIST *ltmpmpose = mpr->lmpose;
            uint32_t maxWidth = 0x00;
            uint32_t y = 0x00;

            while ( ltmpmpose ) {
                G3DMORPHERMESHPOSE *mpose = ( G3DMORPHERMESHPOSE * ) ltmpmpose->data;
                GtkWidget *checkButton = gtk_check_button_new_with_label ( mpose->name );
                GdkRectangle lrec = { 0x00, 0x00, 0x00, 0x10 };
                MESHPOSEDATA *mpd = calloc ( 0x01, sizeof ( MESHPOSEDATA ) );

                if ( key->data.u64 & mpose->slotBit ) {
                    gtk_toggle_button_set_active ( checkButton, TRUE  );
                } else {
                    gtk_toggle_button_set_active ( checkButton, FALSE );
                }

                mpd->mpr   = mpr;
                mpd->mpose = mpose;
                mpd->gui   = gui;
                mpd->key   = key;

                gtk_fixed_put ( fixed, checkButton, 0, y );

                gtk_widget_size_allocate ( checkButton, &lrec );

                g_signal_connect ( checkButton, "toggled", G_CALLBACK(selectMeshPoseCbk) , mpd );
                g_signal_connect ( checkButton, "destroy", G_CALLBACK(destroyMeshPoseCbk), mpd );

                y += lrec.height;

                ltmpmpose = ltmpmpose->next;
            }

            frec.width  = 100;
            frec.height = y;

            gtk_widget_size_allocate ( fixed, &frec );
        }
    }

    gui->lock = 0x00;

    gtk_widget_show_all ( fixed );
}

/******************************************************************************/
static void createMeshPoseFrame ( GtkWidget *frm, 
                                  G3DKEY    *key,
                                  G3DUI     *gui,
                                  gint       x,
                                  gint       y,
                                  gint       width,
                                  gint       height ) {
    GtkWidget *scrolled = gtk_scrolled_window_new ( NULL, NULL );
    GdkRectangle srec = { 0x00, 0x00, width, height };
    GtkWidget *fixed = gtk_fixed_new ( );
    GtkWidget *mpf;

    mpf = createFrame ( frm, gui, EDITMORPHERPOSEFRAME,  x, y, width, height );

    gtk_container_add( GTK_CONTAINER(scrolled), fixed );
    gtk_container_add( GTK_CONTAINER(mpf)     , fixed );

    gtk_widget_set_size_request ( scrolled, width, height );

    gtk_widget_size_allocate ( scrolled, &srec );

    gtk_fixed_put ( mpf, scrolled, x, y );

    gtk_widget_set_name ( scrolled, EDITMORPHERPOSELIST );
    gtk_widget_set_name ( fixed   , EDITMORPHERPOSELIST );

    populateMeshPoseList ( fixed, key, gui );

    gtk_widget_show_all ( scrolled );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    /*updateKeyMorpherEdit ( widget, gui );*/
}

/******************************************************************************/
GtkWidget* createKeyMorpherEdit ( GtkWidget *parent,
                                  G3DKEY    *key,
                                  G3DUI     *gui,
                                  gint       x,
                                  gint       y,
                                  gint       width,
                                  gint       height ) {
    GdkRectangle gdkrec = { 0, 0, width, height };
    GtkWidget *frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, EDITMORPHERKEY );

    /*gtk_widget_size_allocate ( frm, &gdkrec );*/

    gtk_widget_set_size_request ( frm, gdkrec.width, gdkrec.height );

    /*** For some reason, GtkSpinButtons calls its callbacks ***/
    /*** when being realized. With this trick I bypass that. ***/
    /*** Callbacks will return prematurely if gui->lock == 0x01 ***/
    gui->lock = 0x01;

    createMeshPoseFrame ( frm, key, gui, 0, 88, 286, 140 );

    gui->lock = 0x00;

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), gui );
    g_signal_connect ( G_OBJECT (frm), "destroy", G_CALLBACK (Destroy), gui );

    gtk_container_add ( GTK_CONTAINER(parent), frm );

    gtk_widget_show_all ( frm );


    return frm;
}
