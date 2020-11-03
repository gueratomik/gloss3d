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

static void updateKeyMorpherEdit ( GtkWidget *widget, 
                                   gpointer   user_data );

/******************************************************************************/
typedef struct _MORPHERKEYDATA {
    LIST       *lmpd; /*** list of Mesh Pose Data ***/
    GtkWidget  *meshPoseList;
    G3DKEY     *key;
    G3DMORPHER *mpr;
    G3DUI      *gui;
} MORPHERKEYDATA;

/******************************************************************************/
static MORPHERKEYDATA *morpherkeydata_new ( G3DMORPHER *mpr,
                                            G3DKEY     *key,
                                            G3DUI      *gui ) {
    MORPHERKEYDATA *mkd = calloc ( 0x01, sizeof ( MORPHERKEYDATA ) );

    if ( mkd == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__);

        return NULL;
    }

    mkd->mpr = mpr;
    mkd->key = key;
    mkd->gui = gui;

    return mkd;
}

/******************************************************************************/
typedef struct _MESHPOSEDATA {
    MORPHERKEYDATA     *mkd;
    G3DMORPHERMESHPOSE *mpose;
    GtkWidget          *rateWidget;
    GtkWidget          *enableWidget;
} MESHPOSEDATA;

/******************************************************************************/
static MESHPOSEDATA *meshposedata_new ( MORPHERKEYDATA     *mkd,
                                        G3DMORPHERMESHPOSE *mpose ) {
    MESHPOSEDATA *mpd = calloc ( 0x01, sizeof ( MESHPOSEDATA ) );

    mpd->mkd   = mkd;
    mpd->mpose = mpose;

    return mpd;
}

/******************************************************************************/
static void meshposedata_free ( MESHPOSEDATA *mpd ) {
    free ( mpd );
}

/******************************************************************************/
static MESHPOSEDATA *morpherkeydata_getMeshPoseData ( MORPHERKEYDATA     *mkd,
                                                      G3DMORPHERMESHPOSE *mpose ) {
    LIST *ltmpmpd = mkd->lmpd;

    while ( ltmpmpd ) {
        MESHPOSEDATA *mpd = ( MESHPOSEDATA * ) ltmpmpd->data;

        if ( mpd->mpose == mpose ) return mpd;

        ltmpmpd = ltmpmpd->next;
    } 

    return NULL;
}

/******************************************************************************/
static void morpherkeydata_addMeshPoseData ( MORPHERKEYDATA *mkd,
                                             MESHPOSEDATA   *mpd ) {
    list_insert ( &mkd->lmpd, mpd );
}

/******************************************************************************/
static void morpherkeydata_removeMeshPoseData ( MORPHERKEYDATA *mkd,
                                                MESHPOSEDATA   *mpd ) {
    list_remove ( &mkd->lmpd, mpd );
}

/******************************************************************************/
static MESHPOSEDATA *morpherkeydata_free ( MORPHERKEYDATA *mkd ) {
    list_free ( &mkd->lmpd, meshposedata_free );

    return NULL;
}

/******************************************************************************/
static void selectMeshPoseCbk ( GtkWidget *widget, 
                                gpointer   user_data ) {
    gboolean active = gtk_toggle_button_get_active (widget);
    MESHPOSEDATA *mpd = ( MESHPOSEDATA * ) user_data;

    if ( mpd->mkd->gui->lock == 0x00 ) {
        if ( active == TRUE  ) {
            g3dmorpherkey_enableMeshPose  ( mpd->mkd->key, 
                                            mpd->mpose->slotID );
        }

        if ( active == FALSE ) {
            g3dmorpherkey_disableMeshPose ( mpd->mkd->key, 
                                            mpd->mpose->slotID );
        }

        updateKeyMorpherEdit ( mpd->mkd->meshPoseList, mpd->mkd );
    }

    /*g3dui_redrawGLViews ( fgd->gui );*/
}

/******************************************************************************/
static void destroyMeshPoseCbk ( GtkWidget *widget,
                                  gpointer   user_data ) {
    MESHPOSEDATA *mpd = ( MESHPOSEDATA * ) user_data;

    meshposedata_free ( mpd );
}

/******************************************************************************/
static void rateCbk ( GtkWidget *widget, gpointer user_data ) {
    const char *field_name = gtk_widget_get_name ( widget );
    float val = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    MESHPOSEDATA *mpd = ( MESHPOSEDATA * ) user_data;

    if ( mpd->mkd->gui->lock == 0x00 ) {
        g3dmorpherkey_setMeshPoseRate ( mpd->mkd->key,
                                        mpd->mpose->slotID, val / 100.0f );
    }
}

/******************************************************************************/
static GtkWidget *createMeshPoseFrame ( GtkWidget *frm, 
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

    gtk_widget_show_all ( scrolled );

    return fixed;
}

/******************************************************************************/
static void updateKeyMorpherEdit ( GtkWidget *widget, 
                                   gpointer   user_data ) {
    MORPHERKEYDATA *mkd = ( MORPHERKEYDATA * ) user_data;
    GdkRectangle frec = { 0x00, 0x00, 0x00, 0x00 };
    LIST *ltmpmpose = mkd->mpr->lmpose;
    uint32_t y = 0x00;

    mkd->gui->lock = 0x01;

    while ( ltmpmpose ) {
        G3DMORPHERMESHPOSE *mpose = ( G3DMORPHERMESHPOSE * ) ltmpmpose->data;
        MESHPOSEDATA *mpd = morpherkeydata_getMeshPoseData ( mkd, 
                                                             mpose );
        GdkRectangle lrec = { 0x00, 0x00, 0x00, 0x18 };
        float rate;

        if ( mpd == NULL ) {
            mpd = meshposedata_new ( mkd, mpose );

            morpherkeydata_addMeshPoseData ( mkd, mpd );

            mpd->enableWidget = gtk_check_button_new_with_label ( mpose->name );

            gtk_fixed_put ( mkd->meshPoseList, 
                            mpd->enableWidget, 
                            0x00,
                            0x00 );

            mpd->rateWidget = createFloatText ( mkd->meshPoseList, 
                                                mpd, 
                                               "", 
                                                0.0f,
                                                100.0f,
                                                0x00, 0x00,
                                                32,
                                                64, rateCbk );

            g_signal_connect ( mpd->enableWidget, 
                               "toggled", 
                               G_CALLBACK(selectMeshPoseCbk), mpd );
        }

        gtk_fixed_move ( mkd->meshPoseList,
                         mpd->enableWidget,
                         128, y );

        gtk_fixed_move ( mkd->meshPoseList,
                         mpd->rateWidget,
                         0  , y );

        if ( g3dmorpherkey_isMeshPoseEnabled ( mkd->key, mpose->slotID ) ) {
            gtk_toggle_button_set_active ( mpd->enableWidget, TRUE );
            gtk_widget_set_sensitive     ( mpd->rateWidget  , TRUE );
        } else {
            gtk_toggle_button_set_active ( mpd->enableWidget, FALSE );
            gtk_widget_set_sensitive     ( mpd->rateWidget  , FALSE );
        }

        rate = g3dmorpherkey_getMeshPoseRate ( mkd->key, 
                                               mpose->slotID ) * 100.0f;

        gtk_spin_button_set_value ( mpd->rateWidget, rate );

        y += lrec.height;

        ltmpmpose = ltmpmpose->next;
    }

    frec.width  = 100;
    frec.height = y;

    gtk_widget_size_allocate ( mkd->meshPoseList, &frec );

    gtk_widget_show_all ( mkd->meshPoseList );

    mkd->gui->lock = 0x00;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, 
                      gpointer   user_data ) {
    MORPHERKEYDATA *mkd = ( MORPHERKEYDATA * ) user_data;

    morpherkeydata_free ( mkd );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, 
                      gpointer   user_data ) {
    MORPHERKEYDATA *mkd = ( MORPHERKEYDATA * ) user_data;

    updateKeyMorpherEdit ( widget, mkd );
}

/******************************************************************************/
GtkWidget* createKeyMorpherEdit ( GtkWidget  *parent,
                                  G3DMORPHER *mpr,
                                  G3DKEY     *key,
                                  G3DUI      *gui,
                                  gint        x,
                                  gint        y,
                                  gint        width,
                                  gint        height ) {
    GdkRectangle gdkrec = { 0, 0, width, height };
    MORPHERKEYDATA *mkd = morpherkeydata_new ( mpr, key, gui );
    GtkWidget *frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, EDITMORPHERKEY );

    /*gtk_widget_size_allocate ( frm, &gdkrec );*/

    gtk_widget_set_size_request ( frm, gdkrec.width, gdkrec.height );

    /*** For some reason, GtkSpinButtons calls its callbacks ***/
    /*** when being realized. With this trick I bypass that. ***/
    /*** Callbacks will return prematurely if gui->lock == 0x01 ***/
    gui->lock = 0x01;

    mkd->meshPoseList = createMeshPoseFrame ( frm, key, gui, 0, 0, 416, 180 );

    gui->lock = 0x00;

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), mkd );
    g_signal_connect ( G_OBJECT (frm), "destroy", G_CALLBACK (Destroy), mkd );

    gtk_container_add ( GTK_CONTAINER(parent), frm );

    gtk_widget_show_all ( frm );


    return frm;
}
