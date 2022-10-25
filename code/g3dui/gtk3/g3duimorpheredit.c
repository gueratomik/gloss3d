/******************************************************************************/
/*  GLOSS3D is free software: you can redistribute it and/or modify           */
/*  it under the terms of the GNU Functions Public License as published by      */
/*  the Free Software Foundation, either version 3 of the License, or         */
/*  (at your option) any later version.                                       */
/*                                                                            */
/*  GLOSS3D is distributed in the hope that it will be useful,                */
/*  but WITHOUT ANY WARRANTY; without even the implied warranty of            */
/*  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the             */
/*  GNU Functions Public License for more details.                              */
/*                                                                            */
/*  You should have received a copy of the GNU Functions Public License         */
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

/**** Widget names for MorpherEdit TextField widget ***/
#define EDITMORPHER           "Morpher"
#define EDITMORPHERPOSENAME   "Name"
#define EDITMORPHERFUNCTIONS  "Functions"
#define EDITMORPHERPOSEADD    "Add pose"
#define EDITMORPHERPOSEREMOVE "Remove pose"
#define EDITMORPHERPOSELIST   "Pose list"
#define EDITMORPHERPOSECOPY   "Duplicate pose"
#define EDITMORPHERPOSESELECT "Select vertices"
#define EDITMORPHEROPTIMIZE   "Optimize"

/******************************************************************************/
static GTK3G3DUIMORPHEREDIT *gtk3_g3duimorpheredit_new ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUIMORPHEREDIT *gtk3med = calloc ( 0x01, sizeof ( GTK3G3DUIMORPHEREDIT ) );

    if ( gtk3med == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3med->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3med; 
}

/******************************************************************************/
static void updatePoseName ( GTK3G3DUIMORPHEREDIT *gtk3med ) {
    gtk3med->core.gui->lock = 0x01;

    if ( gtk3med->core.editedMorpher ) {
        G3DMORPHERMESHPOSE *mpose = g3dmorpher_getSelectedMeshPose ( gtk3med->core.editedMorpher );

        if ( mpose ) {
            gtk_entry_set_text ( gtk3med->poseNameEntry, mpose->name );
        }
    }

    gtk3med->core.gui->lock = 0x00;
}

/******************************************************************************/
static void setPoseLabelClass ( GtkWidget *label, G3DMORPHERMESHPOSE *mpose ) {
    GtkStyleContext *context = gtk_widget_get_style_context ( label );

    if ( mpose->flags & MESHPOSESELECTED ) {
        gtk_style_context_add_class    ( context, CLASS_HIGHLIGHTED );
        gtk_style_context_remove_class ( context, CLASS_MAIN );
    } else {
        gtk_style_context_add_class    ( context, CLASS_MAIN );
        gtk_style_context_remove_class ( context, CLASS_HIGHLIGHTED );
    }
}

/******************************************************************************/
static void seekPoseLabelAndUpdate ( GtkWidget          *eventBox, 
                                     G3DMORPHERMESHPOSE *mpose ) {
    G3DMORPHERMESHPOSE *cmp = g_object_get_data ( G_OBJECT(eventBox), "private" );

    if ( mpose == cmp ) {
        gtk_container_forall ( eventBox,
                               gtk_label_set_text,
                               mpose->name );
    }
}

/******************************************************************************/
static void seekPoseLabelAndSelect ( GtkWidget *eventBox, 
                                     void      *unused ) {
    G3DMORPHERMESHPOSE *mpose = g_object_get_data ( G_OBJECT(eventBox), "private" );

    gtk_container_forall ( eventBox,
                           setPoseLabelClass,
                           mpose );
}

/******************************************************************************/
static gboolean selectPoseCbk ( GtkWidget *widget,
                                GdkEvent  *event,
                                gpointer   user_data ) {
    GTK3G3DUIMORPHEREDIT *gtk3med = ( GTK3G3DUIMORPHEREDIT * ) user_data;
    G3DUI *gui = gtk3med->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    G3DMORPHERMESHPOSE *mpose = g_object_get_data ( G_OBJECT(widget), "private" );

    if ( gtk3med->core.gui->lock ) return 0;

    if ( gtk3med->core.editedMorpher ) {
        G3DMORPHER *mpr = gtk3med->core.editedMorpher;

        switch ( event->type ) {
            case GDK_BUTTON_PRESS :
                g3dmorpher_selectMeshPose ( mpr, mpose );

                gtk_container_forall ( gtk3med->poseListFixed,
                                       seekPoseLabelAndSelect,
                                       NULL );
            break;

            default :
            break;
        }

        gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW ); 

        updatePoseName ( gtk3med );
    }

    return TRUE;
}

/******************************************************************************/
static void updatePoseList ( GTK3G3DUIMORPHEREDIT *gtk3med ) {
    gtk3med->core.gui->lock = 0x01;

    gtk_container_foreach ( GTK_CONTAINER ( gtk3med->poseListFixed ),
                 ( void * ) gtk_widget_destroy,
                            NULL );

    if ( gtk3med->core.editedMorpher ) {
        if ( gtk3med->core.editedMorpher->mod.mes.obj.type == G3DMORPHERTYPE ) {
            G3DMORPHER *mpr = ( G3DMORPHER * ) gtk3med->core.editedMorpher;
            LIST *ltmpmpose = mpr->lmpose;
            uint32_t y = 0x00;

            while ( ltmpmpose ) {
                G3DMORPHERMESHPOSE *mpose = ( G3DMORPHERMESHPOSE * ) ltmpmpose->data;
                GtkWidget *label = ui_gtk_label_new ( CLASS_MAIN, mpose->name );
                GtkEventBox *eventBox = ui_gtk_event_box_new ( CLASS_MAIN );

                gtk_label_set_justify ( label, GTK_JUSTIFY_LEFT );

                g_object_set_data ( G_OBJECT(eventBox), "private", (gpointer) mpose );

                gtk_container_add ( GTK_CONTAINER( eventBox ), label );

                gtk_fixed_put ( gtk3med->poseListFixed, eventBox, 0, y );

                gtk_widget_set_size_request ( eventBox, 308, 24 );
                gtk_widget_set_size_request ( label   , 308, 24  );

                gtk_widget_add_events ( eventBox, GDK_BUTTON_PRESS_MASK   |
                                                  GDK_BUTTON_RELEASE_MASK |
                                                  GDK_DESTROY );

                g_signal_connect ( eventBox, "button-press-event", G_CALLBACK( selectPoseCbk  ), gtk3med );

                gtk_widget_show_all ( eventBox );

                y += 24;

                ltmpmpose = ltmpmpose->next;
            }
			
            if ( y ) {
                gtk_widget_set_size_request ( gtk3med->poseListFixed, 300, y );
			}

            gtk_container_forall ( gtk3med->poseListFixed,
                                   seekPoseLabelAndSelect,
                                   NULL );
        }
    }

    gtk3med->core.gui->lock = 0x00;
}

/******************************************************************************/
static void updatePosesPanel ( GTK3G3DUIMORPHEREDIT *gtk3med ) {
    updatePoseName ( gtk3med );
    updatePoseList ( gtk3med );
}

/******************************************************************************/
static void namePoseCbk  ( GtkWidget *widget, 
                                GdkEvent  *event, 
                                gpointer   user_data ) {
    const char *grpname = gtk_entry_get_text ( GTK_ENTRY(widget) );
    GTK3G3DUIMORPHEREDIT *gtk3med = ( GTK3G3DUIMORPHEREDIT * ) user_data;
    G3DUI *gui = gtk3med->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( gui->lock ) return;

    if ( strlen ( grpname ) ) {
        if ( obj ) {
            if ( obj->type == G3DMORPHERTYPE ) {
                G3DMORPHER *mpr = ( G3DMORPHER * ) obj;
                G3DMORPHERMESHPOSE *mpose = g3dmorpher_getSelectedMeshPose ( mpr );

                if ( mpose ) {
                    if ( mpose->name ) free ( mpose->name );

                    mpose->name = strdup ( grpname );

                    gtk_container_forall ( gtk3med->poseListFixed,
                                           seekPoseLabelAndUpdate,
                                           mpose );
                }
            }
        }
    }
}

/******************************************************************************/
static void createPoseList ( GTK3G3DUIMORPHEREDIT *gtk3med,
                                   gint               x,
                                   gint               y,
                                   gint               width,
                                   gint               height ) {
    GtkWidget *scrolled = ui_gtk_scrolled_window_new ( CLASS_MAIN, NULL, NULL );
    GtkWidget *fixed = ui_gtk_fixed_new ( CLASS_MAIN ) ;
    GtkWidget *frame = ui_gtk_frame_new ( CLASS_MAIN, EDITMORPHERPOSELIST );

    gtk_container_add( GTK_CONTAINER( frame    ), scrolled );
    gtk_container_add( GTK_CONTAINER( scrolled ), fixed    );

    gtk_widget_set_size_request ( frame, width, height );

    gtk_fixed_put ( gtk3med->posePanelFixed, frame, x, y );

    gtk3med->poseListFixed = fixed;

    gtk_widget_show_all ( frame );
}


/******************************************************************************/
static void selectMeshPoseVerticesCbk  ( GtkWidget *widget, 
                                         gpointer   user_data ) {
    GTK3G3DUIMORPHEREDIT *gtk3med = ( GTK3G3DUIMORPHEREDIT * ) user_data;
    G3DUI *gui = gtk3med->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( gui->lock ) return;

    if ( obj ) {
        if ( obj->type == G3DMORPHERTYPE ) {
            G3DMORPHER *mpr = ( G3DMORPHER * ) obj;

            if ( mpr->selmpose ) {
                g3dmorpher_selectMeshVerticesFromPose ( mpr, mpr->selmpose );

                g3dscene_updatePivot ( gui->sce, 
                                       gui->engine_flags );
            }
        }
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void copyMeshPoseCbk  ( GtkWidget *widget,
                               gpointer   user_data ) {
    GTK3G3DUIMORPHEREDIT *gtk3med = ( GTK3G3DUIMORPHEREDIT * ) user_data;
    G3DUI *gui = gtk3med->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( gui->lock ) return;

    if ( obj ) {
        if ( obj->type == G3DMORPHERTYPE ) {
            G3DMORPHER *mpr = ( G3DMORPHER * ) obj;

            if ( mpr->selmpose ) {
                G3DMORPHERMESHPOSE *mpose = g3dmorpher_copyMeshPose ( mpr, 
                                                                      mpr->selmpose );

                g3dmorpher_selectMeshPose ( mpr, mpose );
            }
        }
    }

    updatePoseList ( gtk3med );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void removePoseCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMORPHEREDIT *gtk3med = ( GTK3G3DUIMORPHEREDIT * ) user_data;
    G3DUI *gui = gtk3med->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( gui->lock ) return;

    if ( obj ) {
        if ( obj->type == G3DMORPHERTYPE ) {
            G3DMORPHER *mpr = ( G3DMORPHER * ) obj;
            G3DMORPHERMESHPOSE *mpose = g3dmorpher_getSelectedMeshPose ( mpr );

            if ( mpose ) {
                g3dmorpher_removeMeshPose ( mpr, mpose );
            }
        }
    }

    updatePoseList ( gtk3med );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void addPoseCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMORPHEREDIT *gtk3med = ( GTK3G3DUIMORPHEREDIT * ) user_data;
    G3DUI *gui = gtk3med->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( gui->lock ) return;

    if ( obj ) {
        if ( obj->type == G3DMORPHERTYPE ) {
            G3DMORPHER *mpr = ( G3DMORPHER * ) obj;

            g3durm_morpher_createMeshPose ( gui->urm,
                                            gui->sce,
                                            mpr,
                                            gui->engine_flags,
                                            REDRAWVIEW | UPDATECURRENTOBJECT );
        }
    }

    updatePoseList ( gtk3med );

    gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW );
}

/******************************************************************************/
static void createPosesPanel ( GTK3G3DUIMORPHEREDIT *gtk3med,
                                    gint                   x,
                                    gint                   y,
                                    gint                   width,
                                    gint                   height ) {
    GtkFixed *pan = ui_createTab ( gtk3med->notebook,
                                   gtk3med,
                                   EDITMORPHER,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3med->posePanelFixed = pan;

    gtk3med->poseNameEntry = ui_createCharText ( pan, 
                                                 gtk3med,
                                                 EDITMORPHERPOSENAME,
                                                 CLASS_MAIN,
                                                 0, 0, 96, 96, 20,
                                                 namePoseCbk );

    createPoseList ( gtk3med, 0, 24, width, 100 );

    ui_createPushButton ( pan, 
                          gtk3med,
                          EDITMORPHERPOSEADD,
                          CLASS_MAIN,
                            6, 126, 146, 20,
                          addPoseCbk );

    ui_createPushButton ( pan, 
                          gtk3med,
                          EDITMORPHERPOSEREMOVE,
                          CLASS_MAIN,
                          154, 126, 146, 20,
                          removePoseCbk );

    ui_createPushButton ( pan, 
                          gtk3med,
                          EDITMORPHERPOSECOPY,
                          CLASS_MAIN,
                            6, 150, 146, 20,
                          copyMeshPoseCbk );

    ui_createPushButton ( pan, 
                          gtk3med,
                          EDITMORPHERPOSESELECT,
                          CLASS_MAIN,
                          154, 150, 146, 20,
                          selectMeshPoseVerticesCbk );
}

/******************************************************************************/
static void updateFunctionsPanel ( GTK3G3DUIMORPHEREDIT *gtk3med ) {
    gtk3med->core.gui->lock = 0x01;

    if ( gtk3med->core.editedMorpher ) {

    }

    gtk3med->core.gui->lock = 0x00;
}

/******************************************************************************/
static void gouraudCbk ( GtkWidget *widget,
                         gpointer   user_data ) {
    float gouraud = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GTK3G3DUIMORPHEREDIT *gtk3med = ( GTK3G3DUIMORPHEREDIT * ) user_data;
    G3DUI *gui = gtk3med->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    uint64_t ret;

    if ( gtk3med->core.gui->lock ) return;

    if ( ( gouraud >= 0.0f ) && ( gouraud <= 90.0f ) ) {
        ret = g3duimorpheredit_gouraud ( gtk3med, gouraud * M_PI / 180 );
    } else {
        updateFunctionsPanel ( gtk3med );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );
} 

/******************************************************************************/
static void toggleShadingCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMORPHEREDIT *gtk3med = ( GTK3G3DUIMORPHEREDIT * ) user_data;
    G3DUI *gui = gtk3med->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    uint64_t ret;

    if ( gtk3med->core.gui->lock ) return;

    ret = g3duimorpheredit_toggleShading ( gtk3med );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );
}

/******************************************************************************/
static void optimizeCbk ( GtkWidget *widget,
                          gpointer   user_data ) {
    GTK3G3DUIMORPHEREDIT *gtk3med = ( GTK3G3DUIMORPHEREDIT * ) user_data;
    G3DUI *gui = gtk3med->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( gui->lock ) return;

    if ( obj ) {
        if ( obj->type == G3DMORPHERTYPE ) {
            G3DMORPHER *mpr = ( G3DMORPHER * ) obj;

            g3dmorpher_optimize ( mpr );
        }
    }
}

/******************************************************************************/
static void createFunctionsPanel ( GTK3G3DUIMORPHEREDIT *gtk3med,
                                 gint               x,
                                 gint               y,
                                 gint               width,
                                 gint               height ) {
    GtkFixed *pan = ui_createTab ( gtk3med->notebook,
                                   gtk3med,
                                   EDITMORPHERFUNCTIONS,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    ui_createPushButton ( pan, 
                          gtk3med,
                          EDITMORPHEROPTIMIZE,
                          CLASS_MAIN,
                          0, 0, 96, 20,
                          optimizeCbk );
}

/******************************************************************************/
void gtk3_g3duimorpheredit_update ( GTK3G3DUIMORPHEREDIT *gtk3med ) {
    G3DUI *gui = gtk3med->core.gui;

    gui->lock = 0x01;

    if ( gui->sce ) {
        G3DSCENE *sce = gui->sce;
        uint32_t nbsel = list_count ( sce->lsel );

        if ( nbsel ) {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3med->notebook), TRUE );

            if ( g3dobjectlist_checkType ( sce->lsel, G3DMORPHERTYPE ) ) {
                gtk3med->core.multi = ( nbsel > 0x01 ) ? 0x01 : 0x00;

                gtk3med->core.editedMorpher = ( G3DMORPHER * ) g3dscene_getLastSelected ( sce );

                if ( gtk3med->core.editedMorpher ) {
                    updatePosesPanel ( gtk3med );
                    updateFunctionsPanel ( gtk3med );
                }
            }
        } else {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3med->notebook), FALSE );
        }
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIMORPHEREDIT *gtk3med = ( GTK3G3DUIMORPHEREDIT * ) user_data;

    free ( gtk3med );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIMORPHEREDIT *gtk3med = ( GTK3G3DUIMORPHEREDIT * ) user_data;

    gtk3_g3duimorpheredit_update ( gtk3med );
}

/******************************************************************************/
GTK3G3DUIMORPHEREDIT *gtk3_g3duimorpheredit_create ( GtkWidget *parent,
                                                     GTK3G3DUI *gtk3gui,
                                                     char      *name ) {
    GTK3G3DUIMORPHEREDIT *gtk3med = gtk3_g3duimorpheredit_new ( gtk3gui );
    GtkWidget *notebook = ui_gtk_notebook_new ( CLASS_MAIN );

    gtk3med->notebook = GTK_NOTEBOOK(notebook);

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(notebook), TRUE );

    gtk_widget_set_name ( notebook, name );

    /*gtk_widget_set_size_request ( tab, width, height );*/

    g_signal_connect ( G_OBJECT (notebook), "realize", G_CALLBACK (Realize), gtk3med );
    g_signal_connect ( G_OBJECT (notebook), "destroy", G_CALLBACK (Destroy), gtk3med );

    createPosesPanel     ( gtk3med, 0, 0, 310, 150 );
    createFunctionsPanel ( gtk3med, 0, 0, 310, 150 );

    gtk_widget_show ( notebook );


    return gtk3med;
}
