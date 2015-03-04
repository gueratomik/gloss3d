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

static void updateSubdivisionForm ( GtkWidget *, G3DUI * );

/******************************************************************************/
static void deleteWeightGroupCbk  ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duiweightgrouplist_deleteWeightGroupCbk ( gui );
}

/******************************************************************************/
static void addWeightGroupCbk  ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duiweightgrouplist_addWeightGroupCbk ( gui );
}

/******************************************************************************/
static void nameCbk  ( GtkWidget *widget, gpointer user_data ) {
    const char *grpname = gtk_entry_get_text ( GTK_ENTRY(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duiweightgrouplist_nameCbk ( gui, grpname );
}

/******************************************************************************/
static void setAdaptiveAngleCbk  ( GtkWidget *widget, gpointer user_data ) {
    float ang = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duimeshedit_setAdaptiveAngleCbk ( gui, ang );
}

/******************************************************************************/
static void useAdativeCbk  ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duimeshedit_useAdaptiveCbk ( gui );
}

/******************************************************************************/
static void subdivSyncCbk  ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duimeshedit_subdivSyncCbk ( gui );
}

/******************************************************************************/
static void useIsoLinesCbk  ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duimeshedit_useIsoLinesCbk ( gui );
}

/******************************************************************************/
static void subdivRenderCbk  ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent     ( widget );
    int level  = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( level >= 0x00 ) {
        common_g3duimeshedit_subdivRenderCbk ( gui, level );
    } /*else {*/
        updateSubdivisionForm ( parent, gui );
    /*}*/
}

/******************************************************************************/
static void subdivLevelCbk  ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    int level  = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui    = ( G3DUI * ) user_data;
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    if ( level >= 0x00 ) {
        if ( obj && ( obj->type == G3DMESHTYPE ) ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            uint64_t memsize = getTotalMemory ( );
            uint64_t needmem = g3dmesh_evalSubdivisionBuffer ( mes, level, 
                                                                    gui->flags );
            if ( needmem == 0xFFFFFFFFFFFFFFFF ) {
            /*** Display some warning if the needed memory is > Total memory/16 ***/
                GtkWidget *dialog = gtk_message_dialog_new ( NULL,
                                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                                             GTK_MESSAGE_ERROR,
                                                             GTK_BUTTONS_CLOSE,
                                                             "Not enough memory !" );
                gtk_dialog_run (GTK_DIALOG (dialog));

                gtk_widget_destroy (dialog);
            } else if ( needmem > ( memsize >> 0x04 ) ) {
            /*** Display some warning if the needed memory is > Total memory/16 ***/
                GtkWidget *dialog = gtk_message_dialog_new ( NULL,
                                                             GTK_DIALOG_DESTROY_WITH_PARENT,
                                                             GTK_MESSAGE_QUESTION,
                                                             GTK_BUTTONS_YES_NO,
                                                             "Warning: this may slow down\n"
                                                             "your computer A LOT! Continue?" );
                gint res = gtk_dialog_run ( GTK_DIALOG ( dialog ) );

                if ( res == GTK_RESPONSE_YES ) {
                   common_g3duimeshedit_subdivLevelCbk ( gui, level );
                }

                gtk_widget_destroy ( dialog );
            } else {
                common_g3duimeshedit_subdivLevelCbk ( gui, level );
            }
        }
    } /*else {*/
        updateSubdivisionForm ( parent, gui );
    /*}*/
}

/******************************************************************************/
static void updateSubdivisionForm ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *child_name = gtk_widget_get_name ( child );

            if ( GTK_IS_SPIN_BUTTON(child) ) {
                GtkSpinButton *sbn = GTK_SPIN_BUTTON(child);

                if ( strcmp ( child_name, EDITMESHSUBDIVVIEW ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, mes->subdiv );
                }

                if ( strcmp ( child_name, EDITMESHSUBDIVREND ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, mes->subdiv_render );
                }

                if ( strcmp ( child_name, EDITMESHADVANGLE   ) == 0x00 ) {
                    gtk_spin_button_set_value ( sbn, mes->advang );
                }
            }

            if ( GTK_IS_CHECK_BUTTON(child) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITMESHISOLINES ) == 0x00 ) {
                    if ( ((G3DOBJECT*)mes)->flags & MESHUSEISOLINES ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }

                if ( strcmp ( child_name, EDITMESHSUBDIVSYNC ) == 0x00 ) {
                    if ( ((G3DOBJECT*)mes)->flags & SYNCSUBDIVISION ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }

                if ( strcmp ( child_name, EDITMESHADAPTIVE ) == 0x00 ) {
                    if ( ((G3DOBJECT*)mes)->flags & MESHUSEADAPTIVE ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }
            }

            children =  g_list_next ( children );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
void updateSubdivisionFrame ( GtkWidget *widget, G3DUI *gui ) {
    GtkWidget *frm = gtk_bin_get_child ( GTK_BIN(widget) );

    if ( frm ) updateSubdivisionForm ( frm, gui );
}


/******************************************************************************/
static void createSubdivisionFrame ( GtkWidget *frm, G3DUI *gui,
                                                     gint x,
                                                     gint y,
                                                     gint width,
                                                     gint height ) {
    GtkWidget *sdf;

    sdf = createFrame ( frm, gui, EDITMESHSUBDIVISION, x,   y, width, height );

    createIntegerText ( sdf, gui, EDITMESHSUBDIVVIEW , 0,   0, 128, 32,
                                                       subdivLevelCbk );

    createToggleLabel ( sdf, gui, EDITMESHSUBDIVSYNC, 230,  12,  20, 20,
                                                      subdivSyncCbk );

    createIntegerText ( sdf, gui, EDITMESHSUBDIVREND , 0,  24, 128, 32,
                                                       subdivRenderCbk );

    createToggleLabel ( sdf, gui, EDITMESHADAPTIVE   , 4,  48, 192, 20,
                                                       useAdativeCbk );

    createFloatText   ( sdf, gui, EDITMESHADVANGLE,    4,  72,  96, 64,
                                                       setAdaptiveAngleCbk );

    createToggleLabel ( sdf, gui, EDITMESHISOLINES   , 4,  96, 200, 20,
                                                       useIsoLinesCbk );
}

/******************************************************************************/
void updateWeightgroupForm ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *child_name = gtk_widget_get_name ( child );

            if ( GTK_IS_ENTRY(child) ) {
                GtkEntry *ent = GTK_ENTRY(child);

                if ( strcmp ( child_name, EDITWEIGHTGROUPNAME ) == 0x00 ) {
                    if ( mes->curgrp ) {
                        gtk_entry_set_text ( ent, mes->curgrp->name );
                    }
                }
            }

            children =  g_list_next ( children );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
void updateWeightgroupFrame ( GtkWidget *widget, G3DUI *gui ) {
    GtkWidget *frm = gtk_bin_get_child ( GTK_BIN(widget) );

    if ( frm ) updateWeightgroupForm ( frm, gui );
}

/******************************************************************************/
static void createWeightgroupFrame ( GtkWidget *frm, G3DUI *gui,
                                                     gint x,
                                                     gint y,
                                                     gint width,
                                                     gint height ) {
    GtkWidget *wgf, *lst;

    wgf = createFrame ( frm, gui, EDITMESHWEIGHTGROUP,  x, y, width, height );

    /*** We have to encapsulate widget into a bulletin board widget because ***/
    /*** XmFrame always wants to resize itself. This way, it does not resize***/
    /*brd = createBoard ( sdf, gui, EDITMESHVERTEXGROUP,  4,   20, 278, 120 );*/

    createCharText ( wgf, gui, EDITWEIGHTGROUPNAME, 0, 0, 96, 132, nameCbk );

    createWeightgroupList ( wgf, gui, EDITWEIGHTGROUPLIST,  0, 36, 218, 128 );

    createPushButton ( wgf, gui, "+", 246,  36, 32, 32, addWeightGroupCbk    );
    createPushButton ( wgf, gui, "-", 246, 132, 32, 32, deleteWeightGroupCbk );
}

/******************************************************************************/
void updateMeshEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    /*** prevent a loop ***/
    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( strcmp ( child_name, EDITMESHSUBDIVISION ) == 0x00 ) {
            updateSubdivisionFrame ( child, gui );
        }

        if ( strcmp ( child_name, EDITMESHWEIGHTGROUP ) == 0x00 ) {
            updateWeightgroupFrame ( child, gui );
        }

        children =  g_list_next ( children );
    }

    /*** It's not the best place for that but I don't ***/
    /*** have time to figure out a better solution. ***/
    g3dui_redrawAllWeightGroupList ( gui );


    gui->lock = 0x00;
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    updateMeshEdit ( widget, gui );
}

/******************************************************************************/
GtkWidget *createMeshEdit ( GtkWidget *parent, G3DUI *gui,
                                               char *name,
                                               gint x,
                                               gint y,
                                               gint width,
                                               gint height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *frm, *sdf, *fix;

    frm = gtk_fixed_new ( );

    gtk_widget_set_name ( frm, name );

    gtk_widget_size_allocate ( frm, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), frm, x, y );

    g_signal_connect ( G_OBJECT (frm), "realize", G_CALLBACK (Realize), gui );

    createSubdivisionFrame ( frm, gui,   0,   0, 286, 140 );
    createWeightgroupFrame ( frm, gui,   0, 144, 286, 140 );

    gtk_widget_show ( frm );


    return frm;
}
