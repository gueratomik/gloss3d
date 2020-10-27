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
#define EDITMESHPOSEPANEL "Pose Panel"
#define EDITMESHPOSE      "Pose"
#define EDITMESHPOSENAME  "Pose name"
#define EDITMESHPOSELIST  "Pose list"

/******************************************************************************/
typedef struct _MESHPOSEPANELDATA {
    G3DUI     *gui;
    GtkWidget *panelWidget;
    GtkWidget *listWidget;
    GtkWidget *nameWidget;
} MESHPOSEPANELDATA;

/******************************************************************************/
static MESHPOSEPANELDATA* meshposepaneldata_new ( G3DUI     *gui, 
                                                  GtkWidget *panel ) {
    void *memarea = calloc ( 0x01, sizeof ( MESHPOSEPANELDATA ) );
    MESHPOSEPANELDATA *mpd = ( MESHPOSEPANELDATA *  ) memarea;

    if ( mpd == NULL ) {
        printf ( stderr, "%s: memory allocation failed\n");

        return NULL;
    }

    mpd->gui         = gui;
    mpd->panelWidget = panel;

    return mpd;
}

/******************************************************************************/
typedef struct _MESHPOSEDATA {
    G3DMORPHER         *mpr;
    G3DMORPHERMESHPOSE *mpose;
    MESHPOSEPANELDATA  *mpd;
} MESHPOSEDATA;

/******************************************************************************/
static MESHPOSEDATA *meshposedata_new ( MESHPOSEPANELDATA  *mpd,
                                        G3DMORPHER         *mpr, 
                                        G3DMORPHERMESHPOSE *mpose ) {
    MESHPOSEDATA *fgd = calloc ( 0x01, sizeof ( MESHPOSEDATA ) );

    if ( fgd == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    fgd->mpr    = mpr;
    fgd->mpose = mpose;
    fgd->mpd    = mpd;


    return fgd;
}

/******************************************************************************/
static void createMeshPoseCbk  ( GtkWidget *widget, gpointer user_data );
static gboolean selectMeshPoseCbk ( GtkWidget *widget,
                                    GdkEvent  *event,
                                    gpointer   user_data );
static void updateMeshPoseCbk  ( GtkWidget *widget, gpointer user_data );
static void deleteMeshPoseCbk  ( GtkWidget *widget, gpointer user_data );
static void populateMeshPoseList ( MESHPOSEPANELDATA *mpd );
static void updateMeshPoseList   ( MESHPOSEPANELDATA *mpd );
static void createMeshPoseList   ( GtkWidget          *frm, 
                                     MESHPOSEPANELDATA *mpd,
                                     gint                x,
                                     gint                y,
                                     gint                width,
                                     gint                height );

/******************************************************************************/
static void updateMeshPoseName ( MESHPOSEPANELDATA *mpd ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( mpd->gui->sce );

    mpd->gui->lock = 0x01;

    if ( obj ) {
        if ( obj->type == G3DMORPHERTYPE ) {
            G3DMORPHER *mpr = ( G3DMORPHER * ) obj;
            G3DMORPHERMESHPOSE *mpose = g3dmesh_getLastSelectedFacegroup ( mpr );

            if ( mpose ) {
                gtk_entry_set_text ( mpd->nameWidget, mpose->name );
            }
        }
    }

    mpd->gui->lock = 0x00;
}

/******************************************************************************/
static void updateMeshPosePanel ( GtkWidget *fixed, 
                                  G3DUI     *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(fixed) );
    MESHPOSEPANELDATA *mpd = g_object_get_data ( G_OBJECT ( fixed ), 
                                                  EDITMESHPOSEPANEL );


    updateMeshPoseName ( mpd );
    updateMeshPoseList ( mpd );
}

/******************************************************************************/
static void createMeshPoseCbk  ( GtkWidget *widget, gpointer user_data ) {
    MESHPOSEPANELDATA *mpd = ( MESHPOSEPANELDATA * ) user_data;
    G3DOBJECT *obj = g3dscene_getLastSelected ( mpd->gui->sce );

    if ( mpd->gui->lock ) return;

    if ( obj ) {
        if ( obj->type == G3DMORPHERTYPE ) {
            G3DMORPHER *mpr = ( G3DMORPHER * ) obj;
            G3DMORPHERMESHPOSE *mpose = g3dmorpher_createMeshPose ( mpr, "Pose" );

            if ( mpose == NULL ) {
                fprintf ( stderr, "%s: could not create pose\n" );
            } else {
                g3dmorpher_selectMeshPose ( mpr, mpose );
            }
        }
    }

    updateMeshPoseList ( mpd );
}

/******************************************************************************/
static void updateMeshPoseCbk  ( GtkWidget *widget, gpointer user_data ) {
    MESHPOSEPANELDATA *mpd = ( MESHPOSEPANELDATA * ) user_data;
    G3DOBJECT *obj = g3dscene_getLastSelected ( mpd->gui->sce );

    if ( mpd->gui->lock ) return;

    if ( obj ) {
        if ( obj->type == G3DMORPHERTYPE ) {
            G3DMORPHER *mpr = ( G3DMORPHER * ) obj;
            /*G3DMORPHERMESHPOSE *mpose = g3dmesh_getLastSelectedFacegroup ( mpr );

            if ( mpose ) {
                g3dmeshpose_setFaceList ( mpose, mpr->lselfac );
            } */
        }
    }
}

/******************************************************************************/
static void deleteMeshPoseCbk  ( GtkWidget *widget, gpointer user_data ) {
    MESHPOSEPANELDATA *mpd = ( MESHPOSEPANELDATA * ) user_data;
    G3DOBJECT *obj = g3dscene_getLastSelected ( mpd->gui->sce );

    if ( mpd->gui->lock ) return;

    if ( obj ) {
        if ( obj->type == G3DMORPHERTYPE ) {
            G3DMORPHER *mpr = ( G3DMORPHER * ) obj;

            if ( mpr->selmpose ) {
                g3dmorpher_removeMeshPose ( mpr, mpr->selmpose );

                mpr->selmpose = NULL;
            }
        }
    }

    updateMeshPoseList ( mpd );
}

/******************************************************************************/
static void setMeshPoseLabel ( GtkWidget          *label,
                               G3DMORPHERMESHPOSE *mpose,
                               uint32_t            selected ) {
    char text[0x100] = { 0x00 };

    if ( selected ) snprintf ( text, 0x100, "<span foreground=\"red\">%s</span>", mpose->name );
    else            snprintf ( text, 0x100, "<span foreground=\"black\">%s</span>", mpose->name );

    gtk_label_set_text ( label, text );
    gtk_label_set_use_markup ( label, TRUE );
}

/******************************************************************************/
static gboolean selectMeshPoseCbk ( GtkWidget *widget,
                                    GdkEvent  *event,
                                    gpointer   user_data ) {
    MESHPOSEDATA *fgd = ( MESHPOSEDATA * ) user_data;

    if ( fgd->mpd->gui->lock ) return;

    switch ( event->type ) {
        case GDK_BUTTON_PRESS :
            g3dmorpher_selectMeshPose ( fgd->mpr,
                                        fgd->mpose );
        break;

/* can't get it to work :(
        case GDK_2BUTTON_PRESS :
            g3dmesh_selectFacesFromSelectedMeshPose ( fgd->mpr );
        break;
*/

        default :
        break;
    }

    g3dui_redrawGLViews ( fgd->mpd->gui );

    updateMeshPoseList ( fgd->mpd );
    updateMeshPoseName ( fgd->mpd );

    return TRUE;
}

/******************************************************************************/
static void destroyMeshPoseCbk ( GtkWidget *widget,
                                 gpointer   user_data ) {
    MESHPOSEDATA *md = ( MESHPOSEDATA * ) user_data;

    if ( md->mpd->gui->lock ) return;

    updateMeshPoseList ( md->mpd );

    free ( md );
}

/******************************************************************************/
static void nameMeshPoseCbk  ( GtkWidget *widget, 
                               GdkEvent  *event, 
                               gpointer   user_data ) {
    const char *posename = gtk_entry_get_text ( GTK_ENTRY(widget) );
    MESHPOSEPANELDATA *mpd = ( G3DUI * ) user_data;
    G3DUI *gui = mpd->gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( gui->lock ) return;

 /** string should not be empty, or else the user wont be able to click ***/
    if ( strlen ( posename ) ) {
        if ( obj ) {
            if ( obj->type == G3DMORPHERTYPE ) {
                G3DMORPHER *mpr = ( G3DMORPHER * ) obj;

                if ( mpr->selmpose ) {
                    if ( mpr->selmpose->name ) free ( mpr->selmpose->name );

                    mpr->selmpose->name = strdup ( posename );
                }
            }
        }
    }

    updateMeshPosePanel ( mpd->panelWidget, mpd->gui );
}

/******************************************************************************/
static void populateMeshPoseList ( MESHPOSEPANELDATA *mpd ) {
    G3DOBJECT *obj = g3dscene_getLastSelected ( mpd->gui->sce );

    if ( obj ) {
        if ( obj->type == G3DMORPHERTYPE ) {
            GdkRectangle frec = { 0x00, 0x00, 0x00, 0x00 };
            G3DMORPHER *mpr = ( G3DMORPHER * ) obj;
            LIST *ltmpmpose = mpr->lmpose;
            uint32_t maxWidth = 0x00;
            uint32_t y = 0x00;

            while ( ltmpmpose ) {
                G3DMORPHERMESHPOSE *mpose = ( G3DMORPHERMESHPOSE * ) ltmpmpose->data;
                GtkWidget *label = gtk_label_new ( mpose->name );
                GtkEventBox *eventBox = gtk_event_box_new ();
                GdkRectangle lrec = { 0x00, 0x00, 0x100, 0x10 };
                MESHPOSEDATA *md = meshposedata_new ( mpd, mpr, mpose );

                setMeshPoseLabel ( label, mpose, mpr->selmpose == mpose );

                gtk_container_add (GTK_CONTAINER(eventBox), label );

                gtk_fixed_put ( mpd->listWidget, eventBox, 0, y );

                gtk_widget_size_allocate ( eventBox, &lrec );
                gtk_widget_size_allocate ( label   , &lrec );

                gtk_widget_add_events ( eventBox, GDK_BUTTON_PRESS_MASK   |
                                                  GDK_BUTTON_RELEASE_MASK |
                                                  GDK_DESTROY );

                g_signal_connect ( eventBox, "button-press-event", G_CALLBACK(selectMeshPoseCbk) , md );
                g_signal_connect ( eventBox, "destroy"           , G_CALLBACK(destroyMeshPoseCbk), md );

                y += lrec.height;

                ltmpmpose = ltmpmpose->next;
            }

            frec.width  = 100;
            frec.height = y;
			
            if ( frec.height ) {
                gtk_widget_set_size_request ( mpd->listWidget, frec.width, frec.height );
			}
        }
    }

    gtk_widget_show_all ( mpd->listWidget );
}

/******************************************************************************/
static void updateMeshPoseList ( MESHPOSEPANELDATA *mpd ) {
    GList *children = gtk_container_get_children ( mpd->listWidget );

    mpd->gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;

        gtk_widget_destroy ( child );

        children =  g_list_next ( children );
    }

    populateMeshPoseList ( mpd );

    mpd->gui->lock = 0x00;
}

/******************************************************************************/
static void createMeshPoseList ( GtkWidget         *frm, 
                                 MESHPOSEPANELDATA *mpd,
                                 gint               x,
                                 gint               y,
                                 gint               width,
                                 gint               height ) {
    GtkWidget *scrolled = gtk_scrolled_window_new ( NULL, NULL );
    GdkRectangle srec = { 0x00, 0x00, width, height };
    GtkWidget *fixed = gtk_fixed_new ( );

    gtk_container_add( GTK_CONTAINER(scrolled), fixed );
    gtk_container_add( GTK_CONTAINER(frm)     , fixed );

    gtk_widget_set_size_request ( scrolled, width, height );

    gtk_widget_size_allocate ( scrolled, &srec );

    gtk_fixed_put ( frm, scrolled, x, y );

    gtk_widget_set_name ( scrolled, EDITMESHPOSELIST );
    gtk_widget_set_name ( fixed   , EDITMESHPOSELIST );

    gtk_widget_show_all ( scrolled );

    mpd->listWidget = fixed;
}

/******************************************************************************/
static void createMeshPosePanel ( GtkWidget *parent, 
                                    G3DUI     *gui,
                                    gint       x,
                                    gint       y,
                                    gint       width,
                                    gint       height ) {
    GtkWidget *pan = createPanel ( parent, 
                                   gui, 
                                   EDITMESHPOSEPANEL, 
                                   x, 
                                   y, 
                                   width, 
                                   height );

    MESHPOSEPANELDATA *mpd = meshposepaneldata_new ( gui, pan );

    g_object_set_data ( G_OBJECT ( pan ),
                        EDITMESHPOSEPANEL, 
                        mpd );

    mpd->nameWidget = createCharText ( pan, 
                                       mpd, 
                                       EDITMESHPOSENAME, 
                                       0, 
                                       0, 
                                       96, 
                                       132, 
                                       nameMeshPoseCbk );

    createMeshPoseList ( pan, mpd, 0, 36, 212, 128 );

    createPushButton ( pan, mpd, "Create", 216,  36, 64, 18, createMeshPoseCbk );
    createPushButton ( pan, mpd, "Update", 216,  60, 64, 18, updateMeshPoseCbk );
    createPushButton ( pan, mpd, "Delete", 216,  84, 64, 18, deleteMeshPoseCbk );
   /* createPushButton ( pan, gui, "Select", 216, 108, 64, 18, selectFaceGroupCbk ); */
}

/******************************************************************************/
void updateMorpherEdit ( GtkWidget *widget, 
                         G3DUI     *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    /*** prevent a loop ***/
    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( strcmp ( child_name, EDITMESHPOSEPANEL ) == 0x00 ) {
            updateMeshPosePanel ( child, gui );
        }

        children =  g_list_next ( children );
    }


    gui->lock = 0x00;
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    updateMorpherEdit ( widget, gui );
}

/******************************************************************************/
GtkWidget *createMorpherEdit ( GtkWidget *parent,
                               G3DUI     *gui,
                               char      *name,
                               gint       x,
                               gint       y,
                               gint       width,
                               gint       height ) {
    GdkRectangle gdkrec = { x, y, width, height };
    GtkWidget *tab;

    /********************/
    tab = gtk_notebook_new ( );

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(tab), TRUE );

    gtk_widget_set_name ( tab, name );

    gtk_widget_size_allocate ( tab, &gdkrec );

    gtk_fixed_put ( GTK_FIXED(parent), tab, gdkrec.x, gdkrec.y );

    g_signal_connect ( G_OBJECT (tab), "realize", G_CALLBACK (Realize), gui );

    createMeshPosePanel   ( tab, gui, 0, 0, width, height );

    gtk_widget_show ( tab );


    return tab;
}
