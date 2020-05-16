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

#define EDITFACEGROUP     "Facegroups"
#define EDITFACEGROUPNAME "Facegroup Name"
#define EDITFACEGROUPLIST "Facegroup list"

typedef struct _FACEGROUPDATA {
    G3DMESH      *mes;
    G3DFACEGROUP *facgrp;
    G3DUI        *gui;
} FACEGROUPDATA;

static void createFaceGroupCbk  ( GtkWidget *widget, gpointer user_data );
static void selectFaceGroupCbk  ( GtkWidget *widget, gpointer user_data );
static void updateFaceGroupCbk  ( GtkWidget *widget, gpointer user_data );
static void deleteFaceGroupCbk  ( GtkWidget *widget, gpointer user_data );
static void populateFaceGroupFrameFixedScrolledFixed ( GtkWidget *fixed, 
                                                       G3DUI     *gui );
static void updateFaceGroupFrameFixedScrolledFixed ( GtkWidget *fixed, 
                                                     G3DUI     *gui );
static void updateFaceGroupFrameFixedScrolled ( GtkWidget *scrolled,
                                                G3DUI     *gui );
static void updateFaceGroupFrameFixed ( GtkWidget *frame, G3DUI *gui );
static void updateFaceGroupFrame ( GtkWidget *frame, G3DUI *gui );

/******************************************************************************/
static void createFaceGroupCbk  ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DFACEGROUP *facgrp = g3dfacegroup_new ( "FaceGroup", mes->lselfac );

            g3dmesh_addFacegroup ( mes, facgrp );
        }
    }

    updateFaceGroupFrameFixed ( gtk_widget_get_parent ( widget ), gui );
}

/******************************************************************************/
static void updateFaceGroupCbk  ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DFACEGROUP *facgrp = g3dmesh_getLastSelectedFacegroup ( mes );

            if ( facgrp ) {
                g3dfacegroup_addFaceList ( facgrp, mes->lselfac );
            }
        }
    }
}

/******************************************************************************/
static void deleteFaceGroupCbk  ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DFACEGROUP *facgrp = g3dmesh_getLastSelectedFacegroup ( mes );

            if ( facgrp ) {
                g3dmesh_removeFacegroup ( mes, facgrp );
            }
        }
    }

    updateFaceGroupFrameFixed ( gtk_widget_get_parent ( widget ), gui );
}

/******************************************************************************/
static void setFacegroupLabel ( GtkWidget *label, G3DFACEGROUP *facgrp ) {
    char text[0x100] = { 0x00 };

    if ( facgrp->flags & FACEGROUPSELECTED ) snprintf ( text, 0x100, "<span foreground=\"red\">%s</span>", facgrp->name );
    else                                     snprintf ( text, 0x100, "<span foreground=\"black\">%s</span>", facgrp->name );

    gtk_label_set_text ( label, text );
    gtk_label_set_use_markup ( label, TRUE );
}

/******************************************************************************/
static gboolean selectFacegroupCbk ( GtkWidget *widget,
                                     GdkEvent  *event,
                                     gpointer   user_data ) {
    FACEGROUPDATA *fgd = ( FACEGROUPDATA * ) user_data;
    GtkWidget *fixed = gtk_widget_get_parent ( widget );

    g3dmesh_unselectAllFacegroups ( fgd->mes );

    switch ( event->type ) {
        case GDK_BUTTON_PRESS :
            g3dmesh_selectFacegroup ( fgd->mes, fgd->facgrp );
            g3dmesh_selectFacesFromSelectedFacegroups ( fgd->mes );
        break;

/* can't get it to work :(
        case GDK_2BUTTON_PRESS :
            g3dmesh_selectFacesFromSelectedFacegroups ( fgd->mes );
        break;
*/

        default :
        break;
    }

    g3dui_redrawGLViews ( fgd->gui );

    updateFaceGroupFrameFixedScrolledFixed ( fixed, fgd->gui );

    return TRUE;
}

/******************************************************************************/
static void destroyFacegroupCbk ( GtkWidget *widget,
                                  gpointer   user_data ) {
    FACEGROUPDATA *fgd = ( FACEGROUPDATA * ) user_data;

    free ( fgd );
}

/******************************************************************************/
static void nameFacegroupCbk  ( GtkWidget *widget, 
                                GdkEvent  *event, 
                                gpointer   user_data ) {
    const char *grpname = gtk_entry_get_text ( GTK_ENTRY(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( gui->lock ) return;

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DFACEGROUP *facgrp = g3dmesh_getLastSelectedFacegroup ( mes );

            if ( facgrp ) {
                if ( facgrp->name ) free ( facgrp->name );

                facgrp->name = strdup ( grpname );
            }
        }
    }

    updateFaceGroupFrameFixed ( gtk_widget_get_parent ( widget ), gui );
}

/******************************************************************************/
static void populateFaceGroupFrameFixedScrolledFixed ( GtkWidget *fixed, 
                                                       G3DUI     *gui ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            GdkRectangle frec = { 0x00, 0x00, 0x00, 0x00 };
            G3DMESH *mes = ( G3DMESH * ) obj;
            LIST *ltmpfacgrp = mes->lfacgrp;
            uint32_t maxWidth = 0x00;
            uint32_t y = 0x00;

            while ( ltmpfacgrp ) {
                G3DFACEGROUP *facgrp = ( G3DFACEGROUP * ) ltmpfacgrp->data;
                GtkWidget *label = gtk_label_new ( facgrp->name );
                GtkEventBox *eventBox = gtk_event_box_new ();
                GdkRectangle lrec = { 0x00, 0x00, 0x00, 0x10 };
                FACEGROUPDATA *fgd = calloc ( 0x01, sizeof ( FACEGROUPDATA ) );

                fgd->mes    = mes;
                fgd->facgrp = facgrp;
                fgd->gui    = gui;

                setFacegroupLabel ( label, facgrp );

                gtk_container_add (GTK_CONTAINER(eventBox), label);

                gtk_fixed_put ( fixed, eventBox, 0, y );

                gtk_widget_size_allocate ( eventBox, &lrec );
                gtk_widget_size_allocate ( label   , &lrec );

                gtk_widget_add_events ( eventBox, GDK_BUTTON_PRESS_MASK   |
                                                  GDK_BUTTON_RELEASE_MASK |
                                                  GDK_DESTROY );

                g_signal_connect ( eventBox, "button-press-event", G_CALLBACK(selectFacegroupCbk) , fgd );
                g_signal_connect ( eventBox, "destroy"           , G_CALLBACK(destroyFacegroupCbk), fgd );

                y += lrec.height;

                ltmpfacgrp = ltmpfacgrp->next;
            }

            frec.width  = 100;
            frec.height = y;

            gtk_widget_size_allocate ( fixed, &frec );
        }
    }

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
static void updateFaceGroupFrameFixed ( GtkWidget *fixed, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(fixed) );
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;

            while ( children ) {
                GtkWidget *child = ( GtkWidget * ) children->data;
                const char *child_name = gtk_widget_get_name ( child );

                if ( GTK_IS_ENTRY ( child ) ) {
                    if ( strcmp ( child_name, EDITFACEGROUPNAME ) == 0x00 ) {
                        G3DFACEGROUP *facgrp = g3dmesh_getLastSelectedFacegroup ( mes );

                        if ( facgrp ) {
                            gtk_entry_set_text ( child, facgrp->name );
                        }
                    }
                }

                if ( GTK_IS_SCROLLED_WINDOW ( child ) ) {
                    if ( strcmp ( child_name, EDITFACEGROUPLIST ) == 0x00 ) {
                        updateFaceGroupFrameFixedScrolled ( child, gui );
                    }
                }

                children =  g_list_next ( children );
            }
        }
    }
}

/******************************************************************************/
static void updateFaceGroupFrame ( GtkWidget *frame, G3DUI *gui ) {
    GtkWidget *fixed = gtk_bin_get_child(GTK_BIN(frame));

    updateFaceGroupFrameFixed ( fixed, gui );
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

    createCharText ( fgf, gui, EDITFACEGROUPNAME, 0, 0, 96, 132, nameFacegroupCbk );

    createFaceGroupFrameFixedScrolled ( fgf, gui, 0, 36, 212, 128 );

    createPushButton ( fgf, gui, "Create", 216,  36, 64, 18, createFaceGroupCbk );
    createPushButton ( fgf, gui, "Update", 216,  60, 64, 18, updateFaceGroupCbk );
    createPushButton ( fgf, gui, "Delete", 216,  84, 64, 18, deleteFaceGroupCbk );
   /* createPushButton ( fgf, gui, "Select", 216, 108, 64, 18, selectFaceGroupCbk ); */
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

            if ( GTK_IS_FRAME ( child ) ) {
                GtkFrame *frame = GTK_FRAME ( child );

                if ( strcmp ( child_name, EDITFACEGROUP ) == 0x00 ) {
                    updateFaceGroupFrame ( frame, gui );
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
    createFaceGroupFrame   ( frm, gui,   0, 208, 286, 140 );
    /*createMeshPoseFrame    ( frm, gui,   0, 208, 286, 140 );*/


    gtk_widget_show ( frm );


    return frm;
}
