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
#define EDITFACEGROUP     "Facegroups"
#define EDITFACEGROUPNAME "Facegroup Name"
#define EDITFACEGROUPLIST "Facegroup list"

#define EDITTEXTUREMAPPING   "UVW Map"
#define EDITTEXTURERESTRICT  "Restrict to facegroup(s)"
#define EDITTEXTUREREPEAT    "Repeat"
 
typedef struct _FACEGROUPDATA {
    G3DMESH          *mes;
    G3DFACEGROUP     *facgrp;
    G3DUITEXTUREEDIT *tedit;
} FACEGROUPDATA;

/******************************************************************************/
static GTK3G3DUITEXTUREEDIT *gtk3_g3duitextureedit_new ( GTK3G3DUI  *gtk3gui,
                                                         G3DOBJECT  *obj ) {
    GTK3G3DUITEXTUREEDIT *gtk3ted = calloc ( 0x01, sizeof ( GTK3G3DUITEXTUREEDIT ) );

    if ( gtk3ted == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3ted->core.gui = ( G3DUI * ) gtk3gui;
    gtk3ted->core.obj = obj;

    return gtk3ted; 
}

/******************************************************************************/
static void selectFacegroupCbk ( GtkWidget *widget, gpointer user_data ) {
    gboolean active = gtk_toggle_button_get_active (widget);
    FACEGROUPDATA *fgd = ( FACEGROUPDATA * ) user_data;
    G3DOBJECT *obj = fgd->tedit->obj;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) fgd->tedit->gui;

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DTEXTURE *tex =  g3dmesh_getSelectedTexture ( mes );

            if ( tex ) {
                if ( active == TRUE  ) g3dtexture_restrictFacegroup   ( tex, fgd->facgrp );
                if ( active == FALSE ) g3dtexture_unrestrictFacegroup ( tex, fgd->facgrp );
            }
        }
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW ); 
}

/******************************************************************************/
static void destroyFacegroupCbk ( GtkWidget *widget,
                                  gpointer   user_data ) {
    FACEGROUPDATA *fgd = ( FACEGROUPDATA * ) user_data;

    free ( fgd );
}

/******************************************************************************/
static void emptyFaceGroupList ( GTK3G3DUITEXTUREEDIT *gtk3ted ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(gtk3ted->facegroupFixed) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;

        gtk_widget_destroy ( child );

        children =  g_list_next ( children );
    }
}

/******************************************************************************/
static void updateFaceGroupList ( GTK3G3DUITEXTUREEDIT *gtk3ted ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ted->core.gui;
    G3DUI *gui = ( G3DUI * ) &gtk3gui->core;
    G3DOBJECT *obj = gtk3ted->core.obj;

    gui->lock = 0x01;

    emptyFaceGroupList ( gtk3ted );

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
                    GtkWidget *checkButton = ui_gtk_check_button_new_with_label ( CLASS_MAIN, facgrp->name );
                    FACEGROUPDATA *fgd = calloc ( 0x01, sizeof ( FACEGROUPDATA ) );

                    if ( facgrp->textureSlots & tex->slotBit ) {
                        gtk_toggle_button_set_active ( checkButton, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( checkButton, FALSE );
                    }

                    fgd->mes    = mes;
                    fgd->facgrp = facgrp;
                    fgd->tedit  = &gtk3ted->core;

                    gtk_fixed_put ( gtk3ted->facegroupFixed, checkButton, 0, y );

                    gtk_widget_show ( checkButton );

                    gtk_widget_set_size_request ( checkButton, 20, 20 );

                    g_signal_connect ( checkButton, "toggled", G_CALLBACK(selectFacegroupCbk) , fgd );
                    g_signal_connect ( checkButton, "destroy", G_CALLBACK(destroyFacegroupCbk), fgd );

                    y += 20;

                    ltmpfacgrp = ltmpfacgrp->next;
                }

                frec.width  = 286;
                frec.height = y;

                gtk_widget_set_size_request ( gtk3ted->facegroupFixed, frec.width, frec.height );
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void restrictCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITEXTUREEDIT *gtk3ted = ( GTK3G3DUITEXTUREEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ted->core.gui;
    uint64_t ret;

    /*** prevent a loop ***/
    if ( gtk3ted->core.gui->lock ) return;

    ret = g3duitextureedit_toggleRestrict ( gtk3ted );

    updateTextureEdit ( gtk3ted );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret ); 
}

/******************************************************************************/
static void repeatCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITEXTUREEDIT *gtk3ted = ( GTK3G3DUITEXTUREEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ted->core.gui;
    uint64_t ret;

    /*** prevent a loop ***/
    if ( gtk3ted->core.gui->lock ) return;

    ret = g3duitextureedit_toggleRepeat ( gtk3ted );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret ); 
}

/******************************************************************************/
static void uvmapSelectorCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITEXTUREEDIT *gtk3ted = ( GTK3G3DUITEXTUREEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ted->core.gui;
    uint32_t rank = gtk_combo_box_get_active ( GTK_COMBO_BOX(widget) );
    uint64_t ret;

    /*** prevent a loop ***/
    if ( gtk3ted->core.gui->lock ) return;

    ret = g3duitextureedit_setUVMap ( gtk3ted, rank );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret ); 
}

/******************************************************************************/
static void createUVMapSelector ( GTK3G3DUITEXTUREEDIT *gtk3ted,
                                  gint                  x,
                                  gint                  y,
                                  gint                  labwidth,
                                  gint                  txtwidth,
                                  gint                  txtheight ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ted->core.gui;
    G3DUI *gui = &gtk3gui->core;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = gtk3ted->core.obj;

    gtk3ted->uvmapSelector = ui_createSelector ( gtk3ted->fixed, 
                                                 gtk3ted,
                                                 EDITTEXTUREMAPPING,
                                                 CLASS_MAIN,
                                                 x, 
                                                 y,
                                                 labwidth,
                                                 txtwidth,
                                                 txtheight,
                                                 uvmapSelectorCbk );

    if ( obj && ( obj->type & MESH ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;
        G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

        if ( tex ) {
            LIST *ltmpuvmap = mes->luvmap;
            uint32_t mapRank = 0x00;
            int selected = 0x00;

            while ( ltmpuvmap ) {
                G3DUVMAP *map = ( G3DUVMAP * ) ltmpuvmap->data;
                char *mapname = ((G3DOBJECT*)map)->name;

                gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(gtk3ted->uvmapSelector), NULL, mapname );

                if ( tex->map == map ) {
                    gtk_combo_box_set_active ( GTK_COMBO_BOX(gtk3ted->uvmapSelector), mapRank );

                    selected = 0x01;
                }

                mapRank++;

                ltmpuvmap = ltmpuvmap->next;
            }

            gtk_combo_box_text_append ( GTK_COMBO_BOX_TEXT(gtk3ted->uvmapSelector), NULL, "None" );

            if ( selected == 0x00 ) {
                gtk_combo_box_set_active ( GTK_COMBO_BOX(gtk3ted->uvmapSelector), mapRank );
            }
        }
    }
}

/******************************************************************************/
void updateTextureEdit ( GTK3G3DUITEXTUREEDIT *gtk3ted ) {
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ted->core.gui;
    G3DUI *gui = &gtk3gui->core;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = gtk3ted->core.obj;

    updateFaceGroupList ( gtk3ted );

    /*** prevents a loop ***/
    gui->lock = 0x01;

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DTEXTURE *tex = g3dmesh_getSelectedTexture ( mes );

            if ( tex ) {
                G3DUVMAP *uvmap = tex->map;

                if ( tex->flags & TEXTURERESTRICTED ) {
                    gtk_toggle_button_set_active ( gtk3ted->restrictToggle, TRUE );
                    gtk_widget_set_sensitive     ( gtk3ted->facegroupFrame, TRUE );
                } else {
                    gtk_toggle_button_set_active ( gtk3ted->restrictToggle, FALSE );
                    gtk_widget_set_sensitive     ( gtk3ted->facegroupFrame, FALSE );
                }

                if ( tex->flags & TEXTUREREPEATED ) {
                    gtk_toggle_button_set_active ( gtk3ted->repeatToggle, TRUE  );
                } else {
                    gtk_toggle_button_set_active ( gtk3ted->repeatToggle, FALSE );
                }

                if ( uvmap ) {
                    if ( ((G3DOBJECT*)uvmap)->flags & UVMAPFIXED ) {
                        gtk_widget_set_sensitive ( gtk3ted->editButton, TRUE  );
                    } else {
                        gtk_widget_set_sensitive ( gtk3ted->editButton, FALSE );
                    }
                } else {
                    gtk_widget_set_sensitive ( gtk3ted->editButton, FALSE );
                }
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void createFaceGroupFrame ( GTK3G3DUITEXTUREEDIT *gtk3ted,
                                   gint                  x,
                                   gint                  y,
                                   gint                  width,
                                   gint                  height ) {
    GdkRectangle srec = { 0x00, 0x00, width, height };
    GtkScrolledWindow *scrolled = ui_gtk_scrolled_window_new ( CLASS_MAIN, NULL, NULL );
    GtkFixed *fixed = ui_gtk_fixed_new ( CLASS_MAIN );
    GtkFrame *frame = ui_gtk_frame_new ( CLASS_MAIN, EDITFACEGROUP );

    gtk_container_add ( GTK_CONTAINER(frame), scrolled );

#if GTK_CHECK_VERSION(3,8,0)
    gtk_container_add ( GTK_CONTAINER(scrolled), fixed );
#else
    gtk_scrolled_window_add_with_viewport ( GTK_SCROLLED_WINDOW(scrolled), fixed );
#endif

    gtk_widget_set_size_request ( scrolled, width, height );
    gtk_widget_set_size_request ( fixed   , width, height );

    gtk_fixed_put ( gtk3ted->fixed, frame, x, y );

    gtk3ted->facegroupFrame = frame;
    gtk3ted->facegroupFixed = fixed;

    gtk_widget_show_all ( frame );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITEXTUREEDIT *gtk3ted = ( GTK3G3DUITEXTUREEDIT * ) user_data;

    free ( gtk3ted );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUITEXTUREEDIT *gtk3ted = ( GTK3G3DUITEXTUREEDIT * ) user_data;

    updateTextureEdit ( gtk3ted );
}

/******************************************************************************/
GTK3G3DUITEXTUREEDIT* gtk3_g3duitextureedit_create ( GtkWidget  *parent,
                                                     GTK3G3DUI  *gtk3gui,
                                                     char       *name,
                                                     G3DOBJECT  *obj ) {
    GTK3G3DUITEXTUREEDIT *gtk3ted = gtk3_g3duitextureedit_new ( gtk3gui,
                                                                obj );
    GtkWidget *fixed = ui_gtk_fixed_new ( CLASS_MAIN );

    gtk3ted->fixed = fixed;

    gtk_widget_set_name ( fixed, name );

    /*gtk_widget_size_allocate ( frm, &gdkrec );*/

    gtk_widget_set_size_request ( fixed, 264, 72 );

    createUVMapSelector ( gtk3ted, 0, 0, 144, 96, 20 );

    gtk3ted->repeatToggle   = ui_createToggleLabel ( fixed, gtk3ted, EDITTEXTUREREPEAT  , CLASS_MAIN,  0, 24, 144, 20, 20, repeatCbk        );
    gtk3ted->restrictToggle = ui_createToggleLabel ( fixed, gtk3ted, EDITTEXTURERESTRICT, CLASS_MAIN,  0, 48, 144, 20, 20, restrictCbk      );

    createFaceGroupFrame ( gtk3ted,  0, 96, 286, 140 );


    g_signal_connect ( G_OBJECT (fixed), "realize", G_CALLBACK (Realize), gtk3ted );
    g_signal_connect ( G_OBJECT (fixed), "destroy", G_CALLBACK (Destroy), gtk3ted );


    gtk_widget_show_all ( fixed );


    return gtk3ted;
}
