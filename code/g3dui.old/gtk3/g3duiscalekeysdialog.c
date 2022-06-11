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

#define SCALEKEYSSETRATIO   "Ratio"
#define SCALEKEYSSETCENTER  "Center"
#define SCALEKEYSCOMMIT     "Commit"

/******************************************************************************/
typedef struct _G3DUISCALEKEYSDIALOG {
    G3DUIWIDGETGROUP grp;
    GtkWidget *centerEntry;
    GtkWidget *ratioEntry;
    int32_t center;
    float   ratio;
} G3DUISCALEKEYSDIALOG;

/******************************************************************************/
static G3DUISCALEKEYSDIALOG *g3duiscalekeysdialog_new ( G3DUI *gui ) {
    G3DUISCALEKEYSDIALOG *skd = calloc ( 0x01, sizeof ( G3DUISCALEKEYSDIALOG ) );

    if ( skd == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );
    }

    skd->grp.gui = gui;

    return skd; 
}

/******************************************************************************/
static void setCenterCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUISCALEKEYSDIALOG *skd = ( G3DUISCALEKEYSDIALOG * ) user_data;
    int32_t center = ( int32_t ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    skd->center = center;
}

/******************************************************************************/
static void setRatioCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUISCALEKEYSDIALOG *skd = ( G3DUISCALEKEYSDIALOG * ) user_data;
    float ratio = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );

    skd->ratio = ratio;
}

/******************************************************************************/
static void commitCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUISCALEKEYSDIALOG *skd = ( G3DUISCALEKEYSDIALOG * ) user_data;
    G3DUI *gui = skd->grp.gui;

    g3durm_objectList_scaleSelectedKeys ( gui->urm,
                                          gui->sce->lsel,
                                          skd->ratio,
                                          skd->center,
                                          gui->engine_flags,
                                          REDRAWTIMELINE | 
                                          REDRAWVIEW );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    G3DUISCALEKEYSDIALOG *skd = ( G3DUISCALEKEYSDIALOG * ) user_data;

    free ( skd );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUISCALEKEYSDIALOG *skd = ( G3DUISCALEKEYSDIALOG * ) user_data;
}

/******************************************************************************/
GtkWidget *createScaleKeysDialog ( GtkWidget *parent,
                                   G3DUI     *gui,
                                   char      *name,
                                   gint       x,
                                   gint       y,
                                   gint       width,
                                   gint       height ) {
    GdkRectangle gdkrec = { 0x00, 0x00, width, height };
    GtkWidget *frm = gtk_fixed_new ( );
    G3DUISCALEKEYSDIALOG *skd = g3duiscalekeysdialog_new ( gui );

    gtk_widget_set_name ( frm, name );

    /*gtk_widget_size_allocate ( frm, &gdkrec );*/

    gtk_widget_set_size_request ( frm, gdkrec.width, gdkrec.height );

    g_object_set_data ( G_OBJECT(frm), "private", (gpointer) skd );

    gtk_container_add ( GTK_CONTAINER(parent), frm );

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), skd );
    g_signal_connect ( G_OBJECT (frm), "destroy", G_CALLBACK (Destroy), skd );

    skd->centerEntry = createIntegerText( frm,
                                          skd,
                                          SCALEKEYSSETCENTER,
                                         -INT_MAX, INT_MAX,
                                          0, 24, 96, 96,
                                          setCenterCbk );

    skd->ratioEntry = createFloatText ( frm,
                                          skd,
                                          SCALEKEYSSETRATIO,
                                          0.0f, FLT_MAX,
                                          0, 48, 96, 96,
                                          setRatioCbk );

                      createPushButton  ( frm, 
                                          skd, 
                                          SCALEKEYSCOMMIT, 
                                          48, 96, 32, 18,
                                          commitCbk );


    gtk_widget_show ( frm );


    return frm;
}
