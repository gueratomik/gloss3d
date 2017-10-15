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
/*  Copyright: Gary GABRIEL - garybaldi.baldi@laposte.net - 2012-2017         */
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
static void updateCameraDOFPanel ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );
    G3DSCENE *sce = gui->sce;
    G3DOBJECT *obj = g3dscene_getSelectedObject ( sce );

    gui->lock = 0x01;

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( obj && ( obj->type == G3DCAMERATYPE ) ) {
            G3DCAMERA *cam = ( G3DCAMERA * ) obj;

            if ( GTK_IS_SPIN_BUTTON ( child ) ) {
                GtkSpinButton *spb = GTK_SPIN_BUTTON(child);

                if ( strcmp ( child_name, EDITCAMERADOFNEARBLUR   ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, cam->dof.nearBlur );
                }

                if ( strcmp ( child_name, EDITCAMERADOFNOBLUR ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, cam->dof.noBlur );
                }

                if ( strcmp ( child_name, EDITCAMERADOFFARBLUR ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, cam->dof.farBlur );
                }

                if ( strcmp ( child_name, EDITCAMERADOFRADIUS ) == 0x00 ) {
                    gtk_spin_button_set_value ( spb, cam->dof.radius );
                }
            }

            if ( GTK_IS_TOGGLE_BUTTON ( child ) ) {
                if ( strcmp ( child_name, EDITCAMERADOFENABLE ) == 0x00 ) {
                    GtkToggleButton *tbn = GTK_TOGGLE_BUTTON(child);

                    if ( ((G3DOBJECT*)cam)->flags & CAMERADOF ) {
                        gtk_toggle_button_set_active ( tbn, TRUE  );
                    } else {
                        gtk_toggle_button_set_active ( tbn, FALSE );
                    }
                }
            }
        }

        children =  g_list_next ( children );
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void dofEnableCbk  ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duicameraedit_dofEnableCbk ( gui );
}

/******************************************************************************/
static void dofNearBlurCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    double nearBlur = ( double ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duicameraedit_dofNearBlurCbk ( gui, nearBlur );
}

/******************************************************************************/
static void dofNoBlurCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    double noBlur = ( double ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duicameraedit_dofNoBlurCbk ( gui, noBlur );
}

/******************************************************************************/
static void dofFarBlurCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    double farBlur = ( double ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    common_g3duicameraedit_dofFarBlurCbk ( gui, farBlur );
}

/******************************************************************************/
static void dofRadiusCbk ( GtkWidget *widget, gpointer user_data ) {
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    int radius = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    G3DUI *gui = ( G3DUI * ) user_data;

    if ( radius >= 0 ) {
        common_g3duicameraedit_dofRadiusCbk ( gui, radius );
    } else {
        updateCameraDOFPanel ( parent, gui );
    }
}

/******************************************************************************/
static GtkWidget *createCameraDOFPanel ( GtkWidget *parent, G3DUI *gui,
                                                               char *name,
                                                               gint x,
                                                               gint y,
                                                               gint width,
                                                               gint height ) {
    GtkWidget *lab, *col, *pan, *btn;

    pan = createPanel ( parent, gui, name, x, y, width, height );

    createToggleLabel ( pan, gui, EDITCAMERADOFENABLE, 0x00, 0x00, 20, 20,
                                                      dofEnableCbk );

    createFloatText   ( pan, gui, EDITCAMERADOFNEARBLUR, 0x00, 0x14, 
                                                         0x60, 0x40, dofNearBlurCbk );

    createFloatText   ( pan, gui, EDITCAMERADOFNOBLUR  , 0x00, 0x28, 
                                                         0x60, 0x40, dofNoBlurCbk );

    createFloatText   ( pan, gui, EDITCAMERADOFFARBLUR , 0x00, 0x3C, 
                                                         0x60, 0x40, dofFarBlurCbk );

    createIntegerText ( pan, gui, EDITCAMERADOFRADIUS  , 0x00, 0x50, 
                                                         0x60, 0x20, dofRadiusCbk );

    return pan;
}

/******************************************************************************/
void updateCameraEdit ( GtkWidget *widget, G3DUI *gui ) {
    GList *children = gtk_container_get_children ( GTK_CONTAINER(widget) );

    while ( children ) {
        GtkWidget *child = ( GtkWidget * ) children->data;
        const char *child_name = gtk_widget_get_name ( child );

        if ( strcmp ( child_name, EDITCAMERADOF ) == 0x00 ) {
            updateCameraDOFPanel ( child, gui );
        }

        children =  g_list_next ( children );
    }
}

/******************************************************************************/
static void Realize ( GtkWidget *widget, gpointer user_data ) {
    G3DUI *gui = ( G3DUI * ) user_data;

    updateCameraEdit ( widget, gui );
}

/******************************************************************************/
GtkWidget *createCameraEdit ( GtkWidget *parent, G3DUI *gui,
                                                char *name,
                                                gint x,
                                                gint y,
                                                gint width,
                                                gint height ) {
    GdkRectangle gdkrec = { 0x00, 0x00, width, height };
    GtkWidget *tab = gtk_notebook_new ( );

    gtk_notebook_set_scrollable ( GTK_NOTEBOOK(tab), TRUE );

    gtk_widget_set_name ( tab, name );

    gtk_widget_size_allocate ( tab, &gdkrec );
    /*gtk_widget_set_size_request ( tab, width, height );*/

    gtk_fixed_put ( GTK_FIXED(parent), tab, x, y );

    g_signal_connect ( G_OBJECT (tab), "realize", G_CALLBACK (Realize), gui );

    createCameraDOFPanel ( tab, gui, EDITCAMERADOF, 0, 0, width, height );

    /*list_insert ( &gui->lcamedit, tab );*/

    gtk_widget_show ( tab );


    return tab;
}
