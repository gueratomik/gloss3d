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

#define EDITRACKEDTARGET "Target"

/******************************************************************************/
static void objectSelectorCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    uint32_t rank = gtk_combo_box_get_active ( GTK_COMBO_BOX(widget) );

    common_g3duitrackertagedit_setTargetCbk ( gui, rank );
}

/******************************************************************************/
static void orientationCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkComboBoxText *cmbt = GTK_COMBO_BOX_TEXT(widget);
    G3DUI *gui = ( G3DUI * ) user_data;
    const char *str = gtk_combo_box_text_get_active_text ( cmbt );

    common_g3duitrackertagedit_orientationCbk ( gui, str );
}

/******************************************************************************/
static void createObjectSelector ( GtkWidget *parent, 
                                   G3DUI     *gui,
                                   char      *name,
                                   gint      x, 
                                   gint      y,
                                   gint      labwidth,
                                   gint      txtwidth,
                                   void      (*cbk)( GtkWidget *, 
                                                     gpointer ) ) {
    GtkWidget     *cmb  = gtk_combo_box_text_new ( );
    GdkRectangle   crec = { 0x00, 0x00, txtwidth, 0x12 };
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
        g_signal_connect ( cmb, "changed", G_CALLBACK(cbk), gui );
    }

    if ( sel ) {
        G3DTAG *tag = sel->seltag;

        if ( tag ) {
            if ( tag->type & G3DTAGTRACKERTYPE ) {
                G3DTRACKERTAG *ttag = tag;
                LIST *ltmpobj, *lobj = NULL;
                uint32_t objRank = 0x00;
                int selected = 0x00;

                /*** flatten the object tree ***/
                g3dobject_treeToList_r ( sce, &lobj );

                ltmpobj = lobj;

                while ( ltmpobj ) {
                    G3DOBJECT *obj = ( G3DOBJECT * ) ltmpobj->data;

                    gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, obj->name );

                    if ( ttag->target == obj ) {
                        gtk_combo_box_set_active ( GTK_COMBO_BOX(cmb), objRank );

                        selected = 0x01;
                    }

                    objRank++;

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

    gtk_widget_show ( cmb );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
}

/******************************************************************************/
GtkWidget* createTrackerTagEdit ( GtkWidget     *parent, 
                                  G3DUI         *gui,
                                  char          *name,
                                  gint           x,
                                  gint           y,
                                  gint           width,
                                  gint           height ) {
    GdkRectangle gdkrec = { 0, 0, width, height };
    GtkWidget *frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    /*gtk_widget_size_allocate ( frm, &gdkrec );*/

    gtk_widget_set_size_request ( frm, gdkrec.width, gdkrec.height );

    /*** For some reason, GtkSpinButtons calls its callbacks ***/
    /*** when being realized. With this trick I bypass that. ***/
    /*** Callbacks will return prematurely if gui->lock == 0x01 ***/
    gui->lock = 0x01;

    createObjectSelector  ( frm, 
                            gui,
                            EDITRACKEDTARGET,
                            16, 0, 48, 128, objectSelectorCbk );

    createOrientationSelection ( frm, gui, EDITPLANEORIENTATION, 
                                           XSTR,
                                           YSTR,
                                           ZSTR,
                                           0, 24, 
                                           60, 60, orientationCbk );


    gui->lock = 0x00;

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), gui );
    g_signal_connect ( G_OBJECT (frm), "destroy", G_CALLBACK (Destroy), gui );

    gtk_container_add ( GTK_CONTAINER(parent), frm );

    gtk_widget_show_all ( frm );


    return frm;
}
