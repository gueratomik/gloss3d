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

/**** Widget names for MeshEdit TextField widget ***/
#define EDITMESH              "Mesh"
#define EDITMESHGENERAL       "General"
#define EDITMESHWEIGHTGROUP   "Weight Groups"

#define EDITMESHFACEGROUP     "Face Groups"
#define EDITMESHFACEGROUPLIST "Facegroup list"
#define EDITMESHFACEADD       "Add"
#define EDITMESHFACEREMOVE    "Remove"
#define EDITMESHFACEUPDATE    "Set"
#define EDITMESHFACEGROUPNAME "Name"

#define EDITMESHWEIGHTGROUP     "Weight Groups"
#define EDITMESHWEIGHTGROUPLIST "Weightgroup list"
#define EDITMESHWEIGHTADD       "Add"
#define EDITMESHWEIGHTREMOVE    "Remove"
#define EDITMESHWEIGHTUPDATE    "Set"
#define EDITMESHWEIGHTGROUPNAME "Name"

#define EDITMESHGOURAUDLIMIT  "Shading limit"
#define EDITMESHISOLINES      "Use isoparms (Slower)"
#define EDITMESHSHADING       "No shading"

/******************************************************************************/
static GTK3G3DUIMESHEDIT *gtk3_g3duimeshedit_new ( GTK3G3DUI *gtk3gui ) {
    GTK3G3DUIMESHEDIT *gtk3med = calloc ( 0x01, sizeof ( GTK3G3DUIMESHEDIT ) );

    if ( gtk3med == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3med->core.gui = ( G3DUI * ) gtk3gui;

    return gtk3med; 
}

/******************************************************************************/
static void updateWeightgroupName ( GTK3G3DUIMESHEDIT *gtk3med ) {
    gtk3med->core.gui->lock = 0x01;

    if ( gtk3med->core.editedMesh ) {
        G3DWEIGHTGROUP *weigrp = gtk3med->core.editedMesh->curgrp;

        if ( weigrp ) {
            gtk_entry_set_text ( gtk3med->wgNameEntry, weigrp->name );
        }
    }

    gtk3med->core.gui->lock = 0x00;
}

/******************************************************************************/
static void setWGLabelClass ( GtkWidget *label, G3DWEIGHTGROUP *weigrp ) {
    GtkStyleContext *context = gtk_widget_get_style_context ( label );

    if ( weigrp->flags & WEIGHTGROUPSELECTED ) {
        gtk_style_context_add_class    ( context, CLASS_HIGHLIGHTED );
        gtk_style_context_remove_class ( context, CLASS_MAIN );
    } else {
        gtk_style_context_add_class ( context, CLASS_MAIN );
        gtk_style_context_remove_class ( context, CLASS_HIGHLIGHTED );
    }
}

/******************************************************************************/
static void seekWGLabelAndSelect ( GtkWidget    *eventBox, 
                                   void         *unused ) {
    G3DWEIGHTGROUP *weigrp = g_object_get_data ( G_OBJECT(eventBox), "private" );

    gtk_container_forall ( eventBox,
                           setWGLabelClass,
                           weigrp );
}

/******************************************************************************/
static void seekWGLabelAndUpdate ( GtkWidget    *eventBox, 
                                   G3DWEIGHTGROUP *weigrp ) {
    G3DWEIGHTGROUP *cmpgrp = g_object_get_data ( G_OBJECT(eventBox), "private" );

    if ( weigrp == cmpgrp ) {
        gtk_container_forall ( eventBox,
                               gtk_label_set_text,
                               weigrp->name );
    }
}

/******************************************************************************/
static gboolean selectWeightgroupCbk ( GtkWidget *widget,
                                     GdkEvent  *event,
                                     gpointer   user_data ) {
    GTK3G3DUIMESHEDIT *gtk3med = ( GTK3G3DUIMESHEDIT * ) user_data;
    G3DUI *gui = gtk3med->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    G3DWEIGHTGROUP *weigrp = g_object_get_data ( G_OBJECT(widget), "private" );

    if ( gtk3med->core.gui->lock ) return 0;

    if ( gtk3med->core.editedMesh ) {
       G3DMESH *mes = gtk3med->core.editedMesh;

        switch ( event->type ) {
            case GDK_BUTTON_PRESS :
                g3dmesh_selectWeightGroup ( mes, weigrp );

                gtk_container_forall ( gtk3med->wgListFixed,
                                       seekWGLabelAndSelect,
                                       NULL );
            break;

            default :
            break;
        }

        gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW ); 

        updateWeightgroupName ( gtk3med );
    }

    return TRUE;
}

/******************************************************************************/
static void updateWeightgroupList ( GTK3G3DUIMESHEDIT *gtk3med ) {
    gtk3med->core.gui->lock = 0x01;

    gtk_container_foreach ( GTK_CONTAINER ( gtk3med->wgListFixed ),
                 ( void * ) gtk_widget_destroy,
                            NULL );

    if ( gtk3med->core.editedMesh ) {
        if ( gtk3med->core.editedMesh->obj.type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) gtk3med->core.editedMesh;
            LIST *ltmpweigrp = mes->weightgroupList;
            uint32_t y = 0x00;

            while ( ltmpweigrp ) {
                G3DWEIGHTGROUP *weigrp = ( G3DWEIGHTGROUP * ) ltmpweigrp->data;
                GtkWidget *label = ui_gtk_label_new ( CLASS_MAIN, weigrp->name );
                GtkEventBox *eventBox = ui_gtk_event_box_new ( CLASS_MAIN );

                gtk_label_set_justify ( label, GTK_JUSTIFY_LEFT );

                g_object_set_data ( G_OBJECT(eventBox), "private", (gpointer) weigrp );

                gtk_container_add ( GTK_CONTAINER( eventBox ), label );

                gtk_fixed_put ( gtk3med->wgListFixed, eventBox, 0, y );

                gtk_widget_set_size_request ( eventBox, 308, 24 );
                gtk_widget_set_size_request ( label   , 308, 24  );

                gtk_widget_add_events ( eventBox, GDK_BUTTON_PRESS_MASK   |
                                                  GDK_BUTTON_RELEASE_MASK |
                                                  GDK_DESTROY );

                g_signal_connect ( eventBox, "button-press-event", G_CALLBACK( selectWeightgroupCbk  ), gtk3med );

                gtk_widget_show_all ( eventBox );

                y += 24;

                ltmpweigrp = ltmpweigrp->next;
            }
			
            if ( y ) {
                gtk_widget_set_size_request ( gtk3med->wgListFixed, 300, y );
			}

            gtk_container_forall ( gtk3med->wgListFixed,
                                   seekWGLabelAndSelect,
                                   NULL );
        }
    }

    gtk3med->core.gui->lock = 0x00;
}

/******************************************************************************/
static void updateWeightgroupPanel ( GTK3G3DUIMESHEDIT *gtk3med ) {
    updateWeightgroupName ( gtk3med );
    updateWeightgroupList ( gtk3med );
}

/******************************************************************************/
static void nameWeightgroupCbk  ( GtkWidget *widget, 
                                GdkEvent  *event, 
                                gpointer   user_data ) {
    const char *grpname = gtk_entry_get_text ( GTK_ENTRY(widget) );
    GTK3G3DUIMESHEDIT *gtk3med = ( GTK3G3DUIMESHEDIT * ) user_data;
    G3DUI *gui = gtk3med->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( gui->lock ) return;

    if ( strlen ( grpname ) ) {
        if ( obj ) {
            if ( obj->type == G3DMESHTYPE ) {
                G3DMESH *mes = ( G3DMESH * ) obj;
                G3DWEIGHTGROUP *weigrp = mes->curgrp;

                if ( weigrp ) {
                    if ( weigrp->name ) free ( weigrp->name );

                    weigrp->name = strdup ( grpname );

                    gtk_container_forall ( gtk3med->wgListFixed,
                                           seekWGLabelAndUpdate,
                                           weigrp );
                }
            }
        }
    }
}

/******************************************************************************/
static void createWeightgroupList ( GTK3G3DUIMESHEDIT *gtk3med,
                                   gint               x,
                                   gint               y,
                                   gint               width,
                                   gint               height ) {
    GtkWidget *scrolled = ui_gtk_scrolled_window_new ( CLASS_MAIN, NULL, NULL );
    GtkWidget *fixed = ui_gtk_fixed_new ( CLASS_MAIN ) ;
    GtkWidget *frame = ui_gtk_frame_new ( CLASS_MAIN, EDITMESHWEIGHTGROUPLIST );

    gtk_container_add( GTK_CONTAINER( frame    ), scrolled );
    gtk_container_add( GTK_CONTAINER( scrolled ), fixed    );

    gtk_widget_set_size_request ( frame, width, height );

    gtk_fixed_put ( gtk3med->wgPanelFixed, frame, x, y );

    gtk3med->wgListFixed = fixed;

    gtk_widget_show_all ( frame );
}

/******************************************************************************/
static void removeWeightGroupCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMESHEDIT *gtk3med = ( GTK3G3DUIMESHEDIT * ) user_data;
    G3DUI *gui = gtk3med->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( gui->lock ) return;

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DWEIGHTGROUP *weigrp = mes->curgrp;

            if ( weigrp ) {
                g3dmesh_removeWeightGroup ( mes, weigrp );
            }
        }
    }

    updateWeightgroupList ( gtk3med );
}

/******************************************************************************/
static void addWeightGroupCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMESHEDIT *gtk3med = ( GTK3G3DUIMESHEDIT * ) user_data;
    G3DUI *gui = gtk3med->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( gui->lock ) return;

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            char buf[0x20];

            snprintf ( buf, 0x20, "WeightGroup%02i", mes->weightgroupCount );

            G3DWEIGHTGROUP *weigrp = g3dweightgroup_new ( mes, buf );

            g3dmesh_addWeightGroup ( mes, weigrp );
        }
    }

    updateWeightgroupList ( gtk3med );
}

/******************************************************************************/
static void createWeightgroupPanel ( GTK3G3DUIMESHEDIT *gtk3med,
                                    gint                   x,
                                    gint                   y,
                                    gint                   width,
                                    gint                   height ) {
    GtkFixed *pan = ui_createTab ( gtk3med->notebook,
                                   gtk3med,
                                   EDITMESHWEIGHTGROUP,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3med->wgPanelFixed = pan;

    gtk3med->wgNameEntry = ui_createCharText ( pan, 
                                               gtk3med,
                                               EDITMESHWEIGHTGROUPNAME,
                                               CLASS_MAIN,
                                               0, 0, 96, 96, 20,
                                               nameWeightgroupCbk );

    createWeightgroupList ( gtk3med, 0, 24, width, 100 );

    ui_createPushButton ( pan, 
                          gtk3med,
                          EDITMESHWEIGHTADD,
                          CLASS_MAIN,
                          6, 126, 96, 20,
                          addWeightGroupCbk );

    ui_createPushButton ( pan, 
                          gtk3med,
                          EDITMESHWEIGHTREMOVE,
                          CLASS_MAIN,
                          106, 126, 96, 20,
                          removeWeightGroupCbk );
}

/******************************************************************************/
static void updateFacegroupName ( GTK3G3DUIMESHEDIT *gtk3med ) {
    gtk3med->core.gui->lock = 0x01;

    if ( gtk3med->core.editedMesh ) {
        G3DFACEGROUP *facgrp = g3dmesh_getLastSelectedFacegroup ( gtk3med->core.editedMesh );

        if ( facgrp ) {
            gtk_entry_set_text ( gtk3med->fgNameEntry, facgrp->name );
        }
    }

    gtk3med->core.gui->lock = 0x00;
}

/******************************************************************************/
static void setFGLabelClass ( GtkWidget *label, G3DFACEGROUP *facgrp ) {
    GtkStyleContext *context = gtk_widget_get_style_context ( label );

    if ( facgrp->flags & FACEGROUPSELECTED ) {
        gtk_style_context_add_class    ( context, CLASS_HIGHLIGHTED );
        gtk_style_context_remove_class ( context, CLASS_MAIN );
    } else {
        gtk_style_context_add_class ( context, CLASS_MAIN );
        gtk_style_context_remove_class ( context, CLASS_HIGHLIGHTED );
    }
}

/******************************************************************************/
static void seekFGLabelAndSelect ( GtkWidget    *eventBox, 
                                   void         *unused ) {
    G3DFACEGROUP *facgrp = g_object_get_data ( G_OBJECT(eventBox), "private" );

    gtk_container_forall ( eventBox,
                           setFGLabelClass,
                           facgrp );
}

/******************************************************************************/
static void seekFGLabelAndUpdate ( GtkWidget    *eventBox, 
                                   G3DFACEGROUP *facgrp ) {
    G3DFACEGROUP *cmpgrp = g_object_get_data ( G_OBJECT(eventBox), "private" );

    if ( facgrp == cmpgrp ) {
        gtk_container_forall ( eventBox,
                               gtk_label_set_text,
                               facgrp->name );
    }
}

/******************************************************************************/
static gboolean selectFacegroupCbk ( GtkWidget *widget,
                                     GdkEvent  *event,
                                     gpointer   user_data ) {
    GTK3G3DUIMESHEDIT *gtk3med = ( GTK3G3DUIMESHEDIT * ) user_data;
    G3DUI *gui = gtk3med->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    G3DFACEGROUP *facgrp = g_object_get_data ( G_OBJECT(widget), "private" );

    if ( gtk3med->core.gui->lock ) return 0;

    if ( gtk3med->core.editedMesh ) {
       G3DMESH *mes = gtk3med->core.editedMesh;

        g3dmesh_unselectAllFacegroups ( mes );

        switch ( event->type ) {
            case GDK_BUTTON_PRESS :
                g3dmesh_selectFacegroup ( mes, facgrp );

                g3dmesh_selectFacesFromSelectedFacegroups ( mes );

                gtk_container_forall ( gtk3med->fgListFixed,
                                       seekFGLabelAndSelect,
                                       NULL );
            break;

            default :
            break;
        }

        gtk3_interpretUIReturnFlags ( gtk3gui, REDRAWVIEW ); 

        updateFacegroupName ( gtk3med );
    }

    return TRUE;
}

/******************************************************************************/
static void updateFacegroupList ( GTK3G3DUIMESHEDIT *gtk3med ) {
    gtk3med->core.gui->lock = 0x01;

    gtk_container_foreach ( GTK_CONTAINER ( gtk3med->fgListFixed ),
                 ( void * ) gtk_widget_destroy,
                            NULL );

    if ( gtk3med->core.editedMesh ) {
        if ( gtk3med->core.editedMesh->obj.type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) gtk3med->core.editedMesh;
            LIST *ltmpfacgrp = mes->facegroupList;
            uint32_t y = 0x00;

            while ( ltmpfacgrp ) {
                G3DFACEGROUP *facgrp = ( G3DFACEGROUP * ) ltmpfacgrp->data;
                GtkWidget *label = ui_gtk_label_new ( CLASS_MAIN, facgrp->name );
                GtkEventBox *eventBox = ui_gtk_event_box_new ( CLASS_MAIN );

                gtk_label_set_justify ( label, GTK_JUSTIFY_LEFT );

                g_object_set_data ( G_OBJECT(eventBox), "private", (gpointer) facgrp );

                gtk_container_add ( GTK_CONTAINER( eventBox ), label );

                gtk_fixed_put ( gtk3med->fgListFixed, eventBox, 0, y );

                gtk_widget_set_size_request ( eventBox, 308, 24 );
                gtk_widget_set_size_request ( label   , 308, 24  );

                gtk_widget_add_events ( eventBox, GDK_BUTTON_PRESS_MASK   |
                                                  GDK_BUTTON_RELEASE_MASK |
                                                  GDK_DESTROY );

                g_signal_connect ( eventBox, "button-press-event", G_CALLBACK( selectFacegroupCbk  ), gtk3med );

                gtk_widget_show_all ( eventBox );

                y += 24;

                ltmpfacgrp = ltmpfacgrp->next;
            }
			
            if ( y ) {
                gtk_widget_set_size_request ( gtk3med->fgListFixed, 300, y );
			}

            gtk_container_forall ( gtk3med->fgListFixed,
                                   seekFGLabelAndSelect,
                                   NULL );
        }
    }

    gtk3med->core.gui->lock = 0x00;
}

/******************************************************************************/
static void updateFacegroupPanel ( GTK3G3DUIMESHEDIT *gtk3med ) {
    updateFacegroupName ( gtk3med );
    updateFacegroupList ( gtk3med );
}

/******************************************************************************/
static void nameFacegroupCbk  ( GtkWidget *widget, 
                                GdkEvent  *event, 
                                gpointer   user_data ) {
    const char *grpname = gtk_entry_get_text ( GTK_ENTRY(widget) );
    GTK3G3DUIMESHEDIT *gtk3med = ( GTK3G3DUIMESHEDIT * ) user_data;
    G3DUI *gui = gtk3med->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( gui->lock ) return;

    if ( strlen ( grpname ) ) {
        if ( obj ) {
            if ( obj->type == G3DMESHTYPE ) {
                G3DMESH *mes = ( G3DMESH * ) obj;
                G3DFACEGROUP *facgrp = g3dmesh_getLastSelectedFacegroup ( mes );

                if ( facgrp ) {
                    if ( facgrp->name ) free ( facgrp->name );

                    facgrp->name = strdup ( grpname );

                    gtk_container_forall ( gtk3med->fgListFixed,
                                           seekFGLabelAndUpdate,
                                           facgrp );
                }
            }
        }
    }
}

/******************************************************************************/
static void createFacegroupList ( GTK3G3DUIMESHEDIT *gtk3med,
                                   gint               x,
                                   gint               y,
                                   gint               width,
                                   gint               height ) {
    GtkWidget *scrolled = ui_gtk_scrolled_window_new ( CLASS_MAIN, NULL, NULL );
    GtkWidget *fixed = ui_gtk_fixed_new ( CLASS_MAIN ) ;
    GtkWidget *frame = ui_gtk_frame_new ( CLASS_MAIN, EDITMESHFACEGROUPLIST );

    gtk_container_add( GTK_CONTAINER( frame    ), scrolled );
    gtk_container_add( GTK_CONTAINER( scrolled ), fixed    );

    gtk_widget_set_size_request ( frame, width, height );

    gtk_fixed_put ( gtk3med->fgPanelFixed, frame, x, y );

    gtk3med->fgListFixed = fixed;

    gtk_widget_show_all ( frame );
}

/******************************************************************************/
static void updateFaceGroupCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMESHEDIT *gtk3med = ( GTK3G3DUIMESHEDIT * ) user_data;
    G3DUI *gui = gtk3med->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( gui->lock ) return;

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DFACEGROUP *facgrp = g3dmesh_getLastSelectedFacegroup ( mes );

            if ( facgrp ) {
                LIST *ltmpfac = mes->selectedFaceList;

                while ( ltmpfac ) {
                    G3DFACE *fac = ( G3DFACE * ) ltmpfac->data;

                    if ( list_seek ( fac->facegroupList, facgrp ) == NULL ) {
                        g3dface_addFacegroup ( fac, facgrp );
                    }

                    ltmpfac = ltmpfac->next;
                }
            }
        }
    }
}

/******************************************************************************/
static void removeFaceGroupCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMESHEDIT *gtk3med = ( GTK3G3DUIMESHEDIT * ) user_data;
    G3DUI *gui = gtk3med->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( gui->lock ) return;

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            G3DFACEGROUP *facgrp = g3dmesh_getLastSelectedFacegroup ( mes );

            if ( facgrp ) {
                g3dmesh_removeFacegroup ( mes, facgrp );
            }
        }
    }

    updateFacegroupList ( gtk3med );
}

/******************************************************************************/
static void addFaceGroupCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMESHEDIT *gtk3med = ( GTK3G3DUIMESHEDIT * ) user_data;
    G3DUI *gui = gtk3med->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    G3DOBJECT *obj = g3dscene_getLastSelected ( gui->sce );

    if ( gui->lock ) return;

    if ( obj ) {
        if ( obj->type == G3DMESHTYPE ) {
            G3DMESH *mes = ( G3DMESH * ) obj;
            char buf[0x20];

            snprintf ( buf, 0x20, "FaceGroup%02i", mes->facegroupCount );

            G3DFACEGROUP *facgrp = g3dfacegroup_new ( buf, mes->selectedFaceList );

            g3dmesh_addFacegroup ( mes, facgrp );
        }
    }

    updateFacegroupList ( gtk3med );
}

/******************************************************************************/
static void createFacegroupPanel ( GTK3G3DUIMESHEDIT *gtk3med,
                                    gint                   x,
                                    gint                   y,
                                    gint                   width,
                                    gint                   height ) {
    GtkFixed *pan = ui_createTab ( gtk3med->notebook,
                                   gtk3med,
                                   EDITMESHFACEGROUP,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3med->fgPanelFixed = pan;

    gtk3med->fgNameEntry = ui_createCharText ( pan, 
                                               gtk3med,
                                               EDITMESHFACEGROUPNAME,
                                               CLASS_MAIN,
                                               0, 0, 96, 96, 20,
                                               nameFacegroupCbk );

    createFacegroupList ( gtk3med, 0, 24, width, 100 );

    ui_createPushButton ( pan, 
                          gtk3med,
                          EDITMESHFACEADD,
                          CLASS_MAIN,
                          6, 126, 96, 20,
                          addFaceGroupCbk );

    ui_createPushButton ( pan, 
                          gtk3med,
                          EDITMESHFACEREMOVE,
                          CLASS_MAIN,
                          106, 126, 96, 20,
                          removeFaceGroupCbk );

    ui_createPushButton ( pan, 
                          gtk3med,
                          EDITMESHFACEUPDATE,
                          CLASS_MAIN,
                          206, 126, 96, 20,
                          updateFaceGroupCbk );
}

/******************************************************************************/
static void updateGeneralPanel ( GTK3G3DUIMESHEDIT *gtk3med ) {
    gtk3med->core.gui->lock = 0x01;

    if ( gtk3med->core.editedMesh ) {
        double angle = acos ( gtk3med->core.editedMesh->gouraudScalarLimit );

        if ( gtk3med->core.editedMesh->obj.flags & OBJECTNOSHADING ) {
            gtk_toggle_button_set_active ( gtk3med->shadingToggle, TRUE  );
        } else {
            gtk_toggle_button_set_active ( gtk3med->shadingToggle, FALSE );
        }

        gtk_spin_button_set_value ( gtk3med->gouraudEntry, angle * 180 / M_PI );
    }

    gtk3med->core.gui->lock = 0x00;
}

/******************************************************************************/
static void gouraudCbk ( GtkWidget *widget,
                         gpointer   user_data ) {
    float gouraud = ( float ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    GTK3G3DUIMESHEDIT *gtk3med = ( GTK3G3DUIMESHEDIT * ) user_data;
    G3DUI *gui = gtk3med->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    uint64_t ret;

    if ( gtk3med->core.gui->lock ) return;

    if ( ( gouraud >= 0.0f ) && ( gouraud <= 90.0f ) ) {
        ret = g3duimeshedit_gouraud ( gtk3med, gouraud * M_PI / 180 );
    } else {
        updateGeneralPanel ( gtk3med );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );
} 

/******************************************************************************/
static void toggleShadingCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUIMESHEDIT *gtk3med = ( GTK3G3DUIMESHEDIT * ) user_data;
    G3DUI *gui = gtk3med->core.gui;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gui;
    uint64_t ret;

    if ( gtk3med->core.gui->lock ) return;

    ret = g3duimeshedit_toggleShading ( gtk3med );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret );
}

/******************************************************************************/
static void createGeneralPanel ( GTK3G3DUIMESHEDIT *gtk3med,
                                 gint               x,
                                 gint               y,
                                 gint               width,
                                 gint               height ) {
    GtkFixed *pan = ui_createTab ( gtk3med->notebook,
                                   gtk3med,
                                   EDITMESHGENERAL,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3med->gouraudEntry = ui_createFloatText    ( pan,
                                                    gtk3med,
                                                    EDITMESHGOURAUDLIMIT,
                                                    CLASS_MAIN,
                                                    0.0f, 90.0f,
                                                    0,  0, 96, 96, 20,
                                                    gouraudCbk );

    gtk3med->shadingToggle = ui_createToggleLabel ( pan,
                                                    gtk3med,
                                                    EDITMESHSHADING,
                                                    CLASS_MAIN,
                                                    0,  24, 96, 20, 20,
                                                    toggleShadingCbk );
}

/******************************************************************************/
void gtk3_g3duimeshedit_update ( GTK3G3DUIMESHEDIT *gtk3med ) {
    G3DUI *gui = gtk3med->core.gui;

    gui->lock = 0x01;

    if ( gui->sce ) {
        G3DSCENE *sce = gui->sce;
        uint32_t nbsel = list_count ( sce->lsel );

        if ( nbsel ) {
            gtk_widget_set_sensitive ( GTK_WIDGET(gtk3med->notebook), TRUE );

            if ( g3dobjectlist_checkType ( sce->lsel, G3DMESHTYPE ) ) {
                gtk3med->core.multi = ( nbsel > 0x01 ) ? 0x01 : 0x00;

                gtk3med->core.editedMesh = ( G3DMESH * ) g3dscene_getLastSelected ( sce );

                if ( gtk3med->core.editedMesh ) {
                    updateGeneralPanel ( gtk3med );
                    updateFacegroupPanel ( gtk3med );
                    updateWeightgroupPanel ( gtk3med );
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
    GTK3G3DUIMESHEDIT *gtk3med = ( GTK3G3DUIMESHEDIT * ) user_data;

    free ( gtk3med );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUIMESHEDIT *gtk3med = ( GTK3G3DUIMESHEDIT * ) user_data;

    gtk3_g3duimeshedit_update ( gtk3med );
}

/******************************************************************************/
GTK3G3DUIMESHEDIT *gtk3_g3duimeshedit_create ( GtkWidget *parent,
                                               GTK3G3DUI *gtk3gui,
                                               char      *name ) {
    GTK3G3DUIMESHEDIT *gtk3med = gtk3_g3duimeshedit_new ( gtk3gui );
    GtkWidget *notebook = ui_gtk_notebook_new ( CLASS_MAIN );

    gtk3med->notebook = GTK_NOTEBOOK(notebook);

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(notebook), TRUE );

    gtk_widget_set_name ( notebook, name );

    /*gtk_widget_set_size_request ( tab, width, height );*/

    g_signal_connect ( G_OBJECT (notebook), "realize", G_CALLBACK (Realize), gtk3med );
    g_signal_connect ( G_OBJECT (notebook), "destroy", G_CALLBACK (Destroy), gtk3med );

    createGeneralPanel   ( gtk3med, 0, 0, 310, 150 );
    createFacegroupPanel ( gtk3med, 0, 0, 310, 150 );
    createWeightgroupPanel ( gtk3med, 0, 0, 310, 150 );

    gtk_widget_show ( notebook );


    return gtk3med;
}
