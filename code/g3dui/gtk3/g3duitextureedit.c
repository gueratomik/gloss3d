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
static void restrictCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duitextureedit_toggleRestrictCbk ( gui );
}

/******************************************************************************/
static void repeatCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duitextureedit_toggleRepeatCbk ( gui );
}

/******************************************************************************/
static void uvmapSelectorCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    uint32_t mapID = gtk_combo_box_get_active ( GTK_COMBO_BOX(widget) );

    common_g3duitextureedit_setUVMapCbk ( gui, mapID );
}

/******************************************************************************/
static void createUVMapSelector ( GtkWidget *parent, G3DUI *gui, 
                                                     char *name,
                                                     gint x, gint y,
                                                     gint labwidth,
                                                     gint txtwidth,
                                                     void (*cbk)( GtkWidget *, 
                                                                  gpointer ) ) {
    GtkWidget     *cmb  = gtk_combo_box_text_new ( );
    GdkRectangle   crec = { 0x00, 0x00, txtwidth, 0x12 };
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*gtk_spin_button_set_numeric ( btn, TRUE );*/

    gtk_widget_set_name ( cmb, name );

    gtk_widget_size_allocate ( cmb, &crec );

    /*gtk_entry_set_width_chars ( GTK_ENTRY(cmb), 18 );*/

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

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        LIST *ltmpuvmap = mes->luvmap;

        while ( ltmpuvmap ) {
            G3DUVMAP *map = ( G3DUVMAP * ) ltmpuvmap->data;
            char *mapname = ((G3DOBJECT*)map)->name;

            gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(cmb), NULL, mapname        );
 
            ltmpuvmap = ltmpuvmap->next;
        }
    }

    gtk_combo_box_set_active ( GTK_COMBO_BOX(cmb), 0x00 );

    gtk_widget_show ( cmb );
}

/******************************************************************************/
void updateTextureEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    /*** prevents a loop ***/
    gui->lock = 0x01;

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            if ( tex ) {
                while ( children ) {
                    GtkWidget *child = ( GtkWidget * ) children->data;
                    const char *child_name = gtk_widget_get_name ( child );

                    if ( GTK_IS_TOGGLE_BUTTON(child) ) {
                        GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                        if ( strcmp ( child_name, EDITTEXTURERESTRICT ) == 0x00 ) {
                            if ( tex->flags & TEXTURERESTRICTED ) {
                                gtk_toggle_button_set_active ( tbn, TRUE  );
                            } else {
                                gtk_toggle_button_set_active ( tbn, FALSE );
                            }
                        }

                        if ( strcmp ( child_name, EDITTEXTUREREPEAT ) == 0x00 ) {
                            if ( tex->flags & TEXTUREREPEATED ) {
                                gtk_toggle_button_set_active ( tbn, TRUE  );
                            } else {
                                gtk_toggle_button_set_active ( tbn, FALSE );
                            }
                        }
                    }

                    children =  g_list_next ( children );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
#define EDITFACEGROUP     "Facegroups"
#define EDITFACEGROUPNAME "Facegroup Name"
#define EDITFACEGROUPLIST "Facegroup list"

typedef struct _FACEGROUPDATA {
    G3DMESH      *mes;
    G3DFACEGROUP *facgrp;
    G3DUI        *gui;
} FACEGROUPDATA;

/******************************************************************************/
static void selectFacegroupCbk ( GtkWidget *widget, gpointer user_data ) {
    gboolean active = gtk_toggle_button_get_active (widget);
    FACEGROUPDATA *fgd = ( FACEGROUPDATA * ) user_data;
    G3DOBJECT *obj = g3dscene_getLastSelected ( fgd->gui->sce );

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            if ( active == TRUE  ) g3dtexture_restrictFacegroup   ( tex, fgd->facgrp );
            if ( active == FALSE ) g3dtexture_unrestrictFacegroup ( tex, fgd->facgrp );
        }
    }

    g3dui_redrawGLViews ( fgd->gui );
}

/******************************************************************************/
static void destroyFacegroupCbk ( GtkWidget *widget,
                                  gpointer   user_data ) {
    FACEGROUPDATA *fgd = ( FACEGROUPDATA * ) user_data;

    free ( fgd );
}

/******************************************************************************/
static void populateFaceGroupFrameFixedScrolledFixed ( GtkWidget *fixed, 
                                                       G3DUI     *gui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    gui->lock = 0x01;

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            GdkRectangle frec = { 0x00, 0x00, 0x00, 0x00 };
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );
            LIST *ltmpfacgrp = mes->lfacgrp;
            uint32_t maxWidth = 0x00;
            uint32_t y = 0x00;

            if ( tex ) {
                while ( ltmpfacgrp ) {
                    G3DFACEGROUP *facgrp = ( G3DFACEGROUP * ) ltmpfacgrp->data;
                    GtkWidget *checkButton = gtk_check_button_new_with_label ( facgrp->name );
                    GdkRectangle lrec = { 0x00, 0x00, 0x00, 0x10 };
                    FACEGROUPDATA *fgd = calloc ( 0x01, sizeof ( FACEGROUPDATA ) );

                    if ( facgrp->textureSlots & tex->slotBit ) {
                        gtk_toggle_button_set_active ( checkButton, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( checkButton, FALSE );
                    }

                    fgd->mes    = mes;
                    fgd->facgrp = facgrp;
                    fgd->gui    = gui;

                    gtk_fixed_put ( fixed, checkButton, 0, y );

                    gtk_widget_size_allocate ( checkButton, &lrec );

                    g_signal_connect ( checkButton, "toggled", G_CALLBACK(selectFacegroupCbk) , fgd );
                    g_signal_connect ( checkButton, "destroy", G_CALLBACK(destroyFacegroupCbk), fgd );

                    y += lrec.height;

                    ltmpfacgrp = ltmpfacgrp->next;
                }

                frec.width  = 100;
                frec.height = y;

                gtk_widget_size_allocate ( fixed, &frec );
            }
        }
    }

    gui->lock = 0x00;

    gtk_widget_show_all ( fixed );
}

/******************************************************************************/
static void updateFaceGroupFrameFixedScrolledFixed ( GtkWidget *fixed, 
                                                     G3DUI     *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(fixed) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;

        gtk_widget_destroy ( child );

        children =  g_list_next ( children );
    }

    populateFaceGroupFrameFixedScrolledFixed ( fixed, gui );
}

/******************************************************************************/
static void updateFaceGroupFrameFixedScrolled ( GtkWidget *scrolled,
                                                G3DUI     *gui ) {
    GtkWidget *viewport = gtk_bin_get_child(GTK_BIN(scrolled));
    GtkWidget *fixed = gtk_bin_get_child(GTK_BIN(viewport));

    updateFaceGroupFrameFixedScrolledFixed ( fixed, gui );
}

/******************************************************************************/
static void createFaceGroupFrameFixedScrolled ( GtkWidget *frm, 
                                                G3DUI     *gui,
                                                gint       x,
                                                gint       y,
                                                gint       width,
                                                gint       height ) {
    GtkWidget *scrolled = gtk_scrolled_window_new ( NULL, NULL );
    GdkRectangle srec = { 0x00, 0x00, width, height };
    GtkWidget *fixed = gtk_fixed_new ( );

    gtk_container_add( GTK_CONTAINER(scrolled), fixed );
    gtk_container_add( GTK_CONTAINER(frm)     , fixed );

    gtk_widget_set_size_request ( scrolled, width, height );

    gtk_widget_size_allocate ( scrolled, &srec );

    gtk_fixed_put ( frm, scrolled, x, y );

    gtk_widget_set_name ( scrolled, EDITFACEGROUPLIST );
    gtk_widget_set_name ( fixed   , EDITFACEGROUPLIST );

    updateFaceGroupFrameFixedScrolledFixed ( fixed,  gui );

    gtk_widget_show_all ( scrolled );
}

/******************************************************************************/
static void createFaceGroupFrame ( GtkWidget *frm, 
                                   G3DUI     *gui,
                                   gint       x,
                                   gint       y,
                                   gint       width,
                                   gint       height ) {
    GtkWidget *fgf, *lst;

    fgf = createFrame ( frm, gui, EDITFACEGROUP,  x, y, width, height );

    createFaceGroupFrameFixedScrolled ( fgf, gui, 0, 0, 212, 128 );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    updateTextureEdit ( widget, gui );
}

/******************************************************************************/
GtkWidget* createTextureEdit ( GtkWidget *parent, G3DUI *gui,
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


    createUVMapSelector  ( frm, gui, EDITTEXTUREMAPPING,
                                     16, 16, 128, 128, uvmapSelectorCbk );
    createToggleLabel    ( frm, gui, EDITTEXTURERESTRICT,
                                      0, 40,  64,  24, restrictCbk );
    createToggleLabel    ( frm, gui, EDITTEXTUREREPEAT,
                                      0, 64,  64,  24, repeatCbk );
    createFaceGroupFrame ( frm, gui,  0, 88, 286, 140 );

    gui->lock = 0x00;

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), gui );
    g_signal_connect ( G_OBJECT (frm), "destroy", G_CALLBACK (Destroy), gui );

    gtk_container_add ( GTK_CONTAINER(parent), frm );

    gtk_widget_show_all ( frm );


    return frm;
}
