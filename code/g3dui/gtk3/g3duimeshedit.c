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
static void deleteMeshPoseCbk  ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duimeshposelist_deleteSelectedPoseCbk ( gui );
}

/******************************************************************************/
static void addMeshPoseCbk  ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duimeshposelist_createPoseCbk ( gui );
}

/******************************************************************************/
static void gouraudCbk ( GtkWidget *widget,
                         gpointer   user_data ) {
    float gouraud = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( ( gouraud >= 0.0f ) && ( gouraud <= 90.0f ) ) {
        common_g3duimeshedit_gouraudCbk ( gui, gouraud * M_PI / 180 );
    } else {
        updateMeshEdit ( parent, gui );
    }
} 

/******************************************************************************/
static void nameWeightGroupCbk  ( GtkWidget *widget, 
                                  GdkEvent  *event, 
                                  gpointer   user_data ) {
    const char *grpname = gtk_entry_get_text ( GTK_ENTRY(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duiweightgrouplist_nameCbk ( gui, grpname );
}

/******************************************************************************/
static void nameMeshPoseCbk  ( GtkWidget *widget, 
                               GdkEvent  *event, 
                               gpointer   user_data ) {
    const char *grpname = gtk_entry_get_text ( GTK_ENTRY(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duimeshposelist_renameCurrentPoseCbk ( gui, grpname );
}

/******************************************************************************/
static void useIsoLinesCbk  ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duimeshedit_useIsoLinesCbk ( gui );
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

    createCharText ( wgf, gui, EDITWEIGHTGROUPNAME, 0, 0, 96, 132, nameWeightGroupCbk );

    createWeightgroupList ( wgf, gui, EDITWEIGHTGROUPLIST,  0, 36, 218, 128 );

    createPushButton ( wgf, gui, "+", 246,  36, 32, 32, addWeightGroupCbk    );
    createPushButton ( wgf, gui, "-", 246, 132, 32, 32, deleteWeightGroupCbk );
}

/******************************************************************************/
void updateMeshPoseForm ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESHPOSEEXTENSION *ext = ( G3DMESHPOSEEXTENSION * ) g3dobject_getExtensionByID ( obj, MESHPOSEEXTENSION, 0x00 );

        if ( ext ) {
            while ( children ) {
                GtkWidget *child = ( GtkWidget * ) children->data;
                const char *child_name = gtk_widget_get_name ( child );

                if ( GTK_IS_ENTRY(child) ) {
                    GtkEntry *ent = GTK_ENTRY(child);

                    if ( strcmp ( child_name, EDITMESHPOSENAME ) == 0x00 ) {
                        if ( ext->curmps ) {
                            gtk_entry_set_text ( ent, ext->curmps->name );
                        }
                    }
                }

                children =  g_list_next ( children );
            }
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
void updateMeshPoseFrame ( GtkWidget *widget, G3DUI *gui ) {
    GtkWidget *frm = gtk_bin_get_child ( GTK_BIN(widget) );

    if ( frm ) updateMeshPoseForm ( frm, gui );
}

/******************************************************************************/
static void createMeshPoseFrame ( GtkWidget *frm, G3DUI *gui,
                                                  gint x,
                                                  gint y,
                                                  gint width,
                                                  gint height ) {
    GtkWidget *wgf, *lst;

    wgf = createFrame ( frm, gui, EDITMESHPOSE,  x, y, width, height );

    createCharText ( wgf, gui, EDITMESHPOSENAME, 0, 0, 96, 132, nameMeshPoseCbk );

    createMeshPoseList ( wgf, gui, EDITMESHPOSELIST,  0, 36, 218, 128 );

    createPushButton ( wgf, gui, "+", 246,  36, 32, 32, addMeshPoseCbk    );
    createPushButton ( wgf, gui, "-", 246, 132, 32, 32, deleteMeshPoseCbk );
}

/******************************************************************************/
void updateMeshEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent a loop ***/
    gui->lock = 0x01;

    if ( obj && ( obj->type == G3DMESHTYPE ) ) {
        G3DMESH *mes = ( G3DMESH * ) obj;

        while ( children ) {
            GtkWidget *child = ( GtkWidget * ) children->data;
            const char *child_name = gtk_widget_get_name ( child );

            if ( strcmp ( child_name, EDITMESHWEIGHTGROUP ) == 0x00 ) {
                updateWeightgroupFrame ( child, gui );
            }

            if ( strcmp ( child_name, EDITMESHPOSE ) == 0x00 ) {
                updateMeshPoseFrame ( child, gui );
            }

            if ( GTK_IS_SPIN_BUTTON ( child ) ) {
                GtkSpinButton *spb = GTK_SPIN_BUTTON ( child );

                if ( strcmp ( child_name, EDITMESHGOURAUDLIMIT ) == 0x00 ) {
                    double angle = acos ( mes->gouraudScalarLimit );

                    gtk_spin_button_set_value ( child, angle * 180 / M_PI );
                }
            }

            children =  g_list_next ( children );
        }

        /*** It's not the best place for that but I don't ***/
        /*** have time to figure out a better solution. ***/
        g3dui_redrawAllWeightGroupList ( gui );
    }


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

    createFloatText   ( frm, gui, EDITMESHGOURAUDLIMIT, 0.0f, 90.0f,
                                                        0,  0,
                                                      160, 64, gouraudCbk );

    createWeightgroupFrame ( frm, gui,   0,  24, 286, 140 );
    createMeshPoseFrame    ( frm, gui,   0, 208, 286, 140 );

    gtk_widget_show ( frm );


    return frm;
}
