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
void updateWeightgroupsPanel ( GtkWidget *widget, G3DUI *gui ) {
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

    /*** It's not the best place for that but I don't ***/
    /*** have time to figure out a better solution. ***/
    g3dui_redrawAllWeightGroupList ( gui );

    gui->lock = 0x00;
}

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
static void createWeightgroupsPanel ( GtkWidget *parent, 
                                      G3DUI     *gui,
                                      gint       x,
                                      gint       y,
                                      gint       width,
                                      gint       height ) {
    GtkWidget *pan;

    pan = createPanel ( parent, gui, EDITMESHWEIGHTGROUPS, x, y, width, height );

    createCharText ( pan, gui, EDITWEIGHTGROUPNAME, 0, 0, 96, 132, nameWeightGroupCbk );

    createWeightgroupList ( pan, gui, EDITWEIGHTGROUPLIST,  0, 36, 218, 128 );

    createPushButton ( pan, gui, "+", 246,  36, 32, 24, addWeightGroupCbk    );
    createPushButton ( pan, gui, "-", 246,  60, 32, 24, deleteWeightGroupCbk );
}

/******************************************************************************/
#define EDITFACEGROUP     "Facegroups"
#define EDITFACEGROUPNAME "Facegroup Name"
#define EDITFACEGROUPLIST "Facegroup list"

/******************************************************************************/
typedef struct _FACEGROUPPANELDATA {
    G3DUI     *gui;
    GtkWidget *panelWidget;
    GtkWidget *listWidget;
    GtkWidget *nameWidget;
} FACEGROUPPANELDATA;

/******************************************************************************/
static FACEGROUPPANELDATA* facegrouppaneldata_new ( G3DUI     *gui, 
                                                    GtkWidget *panel ) {
    void *memarea = calloc ( 0x01, sizeof ( FACEGROUPPANELDATA ) );
    FACEGROUPPANELDATA *fpd = ( FACEGROUPPANELDATA *  ) memarea;

    if ( fpd == NULL ) {
        printf ( stderr, "%s: memory allocation failed\n");

        return NULL;
    }

    fpd->gui         = gui;
    fpd->panelWidget = panel;

    return fpd;
}

/******************************************************************************/
typedef struct _FACEGROUPDATA {
    G3DMESH            *mes;
    G3DFACEGROUP       *facgrp;
    FACEGROUPPANELDATA *fpd;
} FACEGROUPDATA;

/******************************************************************************/
static FACEGROUPDATA *facegroupdata_new ( FACEGROUPPANELDATA *fpd,
                                          G3DMESH            *mes, 
                                          G3DFACEGROUP       *facgrp ) {
    FACEGROUPDATA *fgd = calloc ( 0x01, sizeof ( FACEGROUPDATA ) );

    if ( fgd == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    fgd->mes    = mes;
    fgd->facgrp = facgrp;
    fgd->fpd    = fpd;


    return fgd;
}

/******************************************************************************/
static void createFaceGroupCbk  ( GtkWidget *widget, gpointer user_data );
static void selectFaceGroupCbk  ( GtkWidget *widget, gpointer user_data );
static void updateFaceGroupCbk  ( GtkWidget *widget, gpointer user_data );
static void deleteFaceGroupCbk  ( GtkWidget *widget, gpointer user_data );
static void populateFacegroupsList ( FACEGROUPPANELDATA *fpd );
static void updateFacegroupsList   ( FACEGROUPPANELDATA *fpd );
static void createFacegroupsList   ( GtkWidget          *frm, 
                                     FACEGROUPPANELDATA *fpd,
                                     gint                x,
                                     gint                y,
                                     gint                width,
                                     gint                height );

/******************************************************************************/
static void updateFacegroupsName ( FACEGROUPPANELDATA *fpd ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( fpd->gui->sce );

    fpd->gui->lock = 0x01;

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DFACEGROUP *facgrp = g3dmesh_getLastSelectedFacegroup ( mes );

            if ( facgrp ) {
                gtk_entry_set_text ( fpd->nameWidget, facgrp->name );
            }
        }
    }

    fpd->gui->lock = 0x00;
}

/******************************************************************************/
static void updateFacegroupsPanel ( GtkWidget *fixed, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(fixed) );
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );
    FACEGROUPPANELDATA *fpd = g_object_get_data ( G_OBJECT ( fixed ), 
                                                  EDITMESHFACEGROUPS );


    updateFacegroupsName ( fpd );
    updateFacegroupsList ( fpd );
}

/******************************************************************************/
static void createFaceGroupCbk  ( GtkWidget *widget, gpointer user_data ) {
    FACEGROUPPANELDATA *fpd = ( FACEGROUPPANELDATA * ) user_data;
    G3DOBJECT *obj = g3dscene_getLastSelected ( fpd->gui->sce );

    if ( fpd->gui->lock ) return;

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DFACEGROUP *facgrp = g3dfacegroup_new ( "FaceGroup", mes->lselfac );

            g3dmesh_addFacegroup ( mes, facgrp );
        }
    }

    updateFacegroupsList ( fpd );
}

/******************************************************************************/
static void updateFaceGroupCbk  ( GtkWidget *widget, gpointer user_data ) {
    FACEGROUPPANELDATA *fpd = ( FACEGROUPPANELDATA * ) user_data;
    G3DOBJECT *obj = g3dscene_getLastSelected ( fpd->gui->sce );

    if ( fpd->gui->lock ) return;

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DFACEGROUP *facgrp = g3dmesh_getLastSelectedFacegroup ( mes );

            if ( facgrp ) {
                g3dfacegroup_setFaceList ( facgrp, mes->lselfac );
            }
        }
    }
}

/******************************************************************************/
static void deleteFaceGroupCbk  ( GtkWidget *widget, gpointer user_data ) {
    FACEGROUPPANELDATA *fpd = ( FACEGROUPPANELDATA * ) user_data;
    G3DOBJECT *obj = g3dscene_getLastSelected ( fpd->gui->sce );

    if ( fpd->gui->lock ) return;

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DFACEGROUP *facgrp = g3dmesh_getLastSelectedFacegroup ( mes );

            if ( facgrp ) {
                g3dmesh_removeFacegroup ( mes, facgrp );
            }
        }
    }

    updateFacegroupsList ( fpd );
}

/******************************************************************************/
static void setFacegroupLabel ( GtkWidget    *label, 
                                G3DFACEGROUP *facgrp ) {
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

    if ( fgd->fpd->gui->lock ) return;

    g3dmesh_unselectAllFacegroups ( fgd->mes );

    switch ( event->type ) {
        case GDK_BUTTON_PRESS :
            g3dmesh_selectFacegroup ( fgd->mes, 
                                      fgd->facgrp );

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

    g3dui_redrawGLViews ( fgd->fpd->gui );

    updateFacegroupsList ( fgd->fpd );
    updateFacegroupsName ( fgd->fpd );

    return TRUE;
}

/******************************************************************************/
static void destroyFacegroupCbk ( GtkWidget *widget,
                                  gpointer   user_data ) {
    FACEGROUPDATA *fgd = ( FACEGROUPDATA * ) user_data;

    if ( fgd->fpd->gui->lock ) return;

    updateFacegroupsList ( fgd->fpd );

    free ( fgd );
}

/******************************************************************************/
static void nameFacegroupCbk  ( GtkWidget *widget, 
                                GdkEvent  *event, 
                                gpointer   user_data ) {
    const char *grpname = gtk_entry_get_text ( GTK_ENTRY(widget) );
    FACEGROUPPANELDATA *fpd = ( G3DUI * ) user_data;
    G3DUI *gui = fpd->gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( gui->lock ) return;

 /** string should not be empty, or else the user wont be able to click ***/
    if ( strlen ( grpname ) ) {
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
    }

    updateFacegroupsPanel ( fpd->panelWidget, fpd->gui );
}

/******************************************************************************/
static void populateFacegroupsList ( FACEGROUPPANELDATA *fpd ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( fpd->gui->sce );

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
                GdkRectangle lrec = { 0x00, 0x00, 0x100, 0x10 };
                FACEGROUPDATA *fgd = facegroupdata_new ( fpd, mes, facgrp );

                setFacegroupLabel ( label, facgrp );

                gtk_container_add (GTK_CONTAINER(eventBox), label );

                gtk_fixed_put ( fpd->listWidget, eventBox, 0, y );

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
			
            if ( frec.height ) {
                gtk_widget_set_size_request ( fpd->listWidget, frec.width, frec.height );
			}
        }
    }

    gtk_widget_show_all ( fpd->listWidget );
}

/******************************************************************************/
static void updateFacegroupsList ( FACEGROUPPANELDATA *fpd ) {
    GList *children = gtk_container_get_children ( fpd->listWidget );

    fpd->gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;

        gtk_widget_destroy ( child );

        children =  g_list_next ( children );
    }

    populateFacegroupsList ( fpd );

    fpd->gui->lock = 0x00;
}

/******************************************************************************/
static void createFacegroupsList ( GtkWidget          *frm, 
                                   FACEGROUPPANELDATA *fpd,
                                   gint                x,
                                   gint                y,
                                   gint                width,
                                   gint                height ) {
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

    gtk_widget_show_all ( scrolled );

    fpd->listWidget = fixed;
}

/******************************************************************************/
static void createFacegroupsPanel ( GtkWidget *parent, 
                                    G3DUI     *gui,
                                    gint       x,
                                    gint       y,
                                    gint       width,
                                    gint       height ) {
    GtkWidget *pan = createPanel ( parent, 
                                   gui, 
                                   EDITMESHFACEGROUPS, 
                                   x, 
                                   y, 
                                   width, 
                                   height );

    FACEGROUPPANELDATA *fpd = facegrouppaneldata_new ( gui, pan );

    g_object_set_data ( G_OBJECT ( pan ),
                        EDITMESHFACEGROUPS, 
                        fpd );

    fpd->nameWidget = createCharText ( pan, 
                                       fpd, 
                                       EDITFACEGROUPNAME, 
                                       0, 
                                       0, 
                                       96, 
                                       132, 
                                       nameFacegroupCbk );

    createFacegroupsList ( pan, fpd, 0, 36, 212, 128 );

    createPushButton ( pan, fpd, "Create", 216,  36, 64, 18, createFaceGroupCbk );
    createPushButton ( pan, fpd, "Update", 216,  60, 64, 18, updateFaceGroupCbk );
    createPushButton ( pan, fpd, "Delete", 216,  84, 64, 18, deleteFaceGroupCbk );
   /* createPushButton ( pan, gui, "Select", 216, 108, 64, 18, selectFaceGroupCbk ); */
}

/******************************************************************************/
static void updateGeneralPanel ( GtkWidget *widget, G3DUI *gui ) {
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

            if ( GTK_IS_CHECK_BUTTON ( child ) ) {
                GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                if ( strcmp ( child_name, EDITMESHSHADING ) == 0x00 ) {
                    if ( obj->flags & OBJECTNOSHADING ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }
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
    }


    gui->lock = 0x00;
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
void toggleShadingCbk ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duimeshedit_toggleShadingCbk ( gui );
}

/******************************************************************************/
static GtkWidget *createGeneralPanel ( GtkWidget *parent, 
                                       G3DUI     *gui,
                                       gint       x,
                                       gint       y,
                                       gint       width,
                                       gint       height ) {
    GtkWidget *pan;

    pan = createPanel ( parent, gui, EDITMESHGENERAL, x, y, width, height );


    createFloatText   ( pan, gui, EDITMESHGOURAUDLIMIT, 0.0f, 90.0f,
                                                        0,  0,
                                                      160, 64, gouraudCbk );

    createToggleLabel ( pan, gui, EDITMESHSHADING, 0, 24, 64, 24, toggleShadingCbk );


    return pan;
}

/******************************************************************************/
void updateMeshEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent a loop ***/
    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( strcmp ( child_name, EDITMESHGENERAL      ) == 0x00 ) {
            updateGeneralPanel ( child, gui );
        }

        if ( strcmp ( child_name, EDITMESHWEIGHTGROUPS ) == 0x00 ) {
            updateWeightgroupsPanel ( child, gui );
        }

        if ( strcmp ( child_name, EDITMESHFACEGROUPS ) == 0x00 ) {
            updateFacegroupsPanel ( child, gui );
        }

        children =  g_list_next ( children );
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
    GtkWidget *tab;

    /********************/
    tab = gtk_notebook_new ( );

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(tab), TRUE );

    gtk_widget_set_name ( tab, name );

    gtk_widget_size_allocate ( tab, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), tab, gdkrec.x, gdkrec.y );

    g_signal_connect ( G_OBJECT (tab), "realize", G_CALLBACK (Realize), gui );

    createGeneralPanel      ( tab, gui, 0, 0, width, height );
    createWeightgroupsPanel ( tab, gui, 0, 0, width, height );
    createFacegroupsPanel   ( tab, gui, 0, 0, width, height );

    gtk_widget_show ( tab );


    return tab;
}
