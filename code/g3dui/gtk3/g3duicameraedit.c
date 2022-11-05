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

#define EDITCAMERAGENERAL     "Camera"
#define EDITCAMERAFOCAL       "Focal"
#define EDITCAMERADOF         "Depth-of-field"
#define EDITCAMERADOFENABLE   "Enable" 
#define EDITCAMERADOFNEARBLUR "Near blur dist."
#define EDITCAMERADOFNOBLUR   "No blur dist."
#define EDITCAMERADOFFARBLUR  "Far blur dist."
#define EDITCAMERADOFRADIUS   "Blur radius"

/******************************************************************************/
static GTK3G3DUICAMERAEDIT *gtk3_g3duicameraedit_new ( GTK3G3DUI *gtk3gui,
                                                       uint32_t   forKey ) {
    GTK3G3DUICAMERAEDIT *gtk3ced = calloc ( 0x01, sizeof ( GTK3G3DUICAMERAEDIT ) );

    if ( gtk3ced == NULL ) {
        fprintf ( stderr, "%s: calloc failed\n", __func__ );

        return NULL;
    }

    gtk3ced->core.forKey = forKey;
    gtk3ced->core.gui = ( G3DUI * ) gtk3gui;


    return gtk3ced; 
}

/******************************************************************************/
static void updateGeneralPanel ( GTK3G3DUICAMERAEDIT *gtk3ced ) {
    gtk3ced->core.gui->lock = 0x01;

    if ( gtk3ced->core.editedCamera ) {
        G3DCAMERA *cam = gtk3ced->core.editedCamera;

        gtk_spin_button_set_value ( gtk3ced->focalEntry, cam->focal );
    }

    gtk3ced->core.gui->lock = 0x00;
}

/******************************************************************************/
static void focalCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUICAMERAEDIT *gtk3ced = ( GTK3G3DUICAMERAEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ced->core.gui;
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    double focal = ( double ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    uint64_t ret = 0x00;

    /*** prevents loop ***/
    if ( gtk3ced->core.gui->lock ) return;

    ret = g3duicameraedit_focal ( &gtk3ced->core, focal );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret ); 
}

/******************************************************************************/
static void createGeneralPanel ( GTK3G3DUICAMERAEDIT *gtk3ced,
                             gint                 x,
                             gint                 y,
                             gint                 width,
                             gint                 height ) {
    GtkFixed *pan = ui_createTab ( gtk3ced->notebook,
                                   gtk3ced,
                                   EDITCAMERAGENERAL,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3ced->focalEntry  = ui_createFloatText   ( pan,
                                                  gtk3ced,
                                                  EDITCAMERAFOCAL,
                                                  CLASS_MAIN,
                                                  0.0f, FLT_MAX,
                                                  0, 0, 96, 96, 20,
                                                  focalCbk );
}

/******************************************************************************/
static void updateDOFPanel ( GTK3G3DUICAMERAEDIT *gtk3ced ) {
    gtk3ced->core.gui->lock = 0x01;

    if ( gtk3ced->core.editedCamera ) {
        G3DCAMERA *cam = gtk3ced->core.editedCamera;

        if ( ((G3DOBJECT*)cam)->flags & CAMERADOF ) {
            gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON(gtk3ced->dofEnableToggle), TRUE  );
        } else {
            gtk_toggle_button_set_active ( GTK_TOGGLE_BUTTON(gtk3ced->dofEnableToggle), FALSE );
        }

        gtk_spin_button_set_value ( gtk3ced->dofNearBlurEntry, cam->dof.nearBlur );
        gtk_spin_button_set_value ( gtk3ced->dofNoBlurEntry  , cam->dof.noBlur   );
        gtk_spin_button_set_value ( gtk3ced->dofFarBlurEntry , cam->dof.farBlur  );
        gtk_spin_button_set_value ( gtk3ced->dofRadiusEntry  , cam->dof.radius   );
    }

    gtk3ced->core.gui->lock = 0x00;
}

/******************************************************************************/
static void dofEnableCbk  ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUICAMERAEDIT *gtk3ced = ( GTK3G3DUICAMERAEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ced->core.gui;
    uint64_t ret = 0x00;

    /*** prevents loop ***/
    if ( gtk3ced->core.gui->lock ) return;

    ret = g3duicameraedit_dofEnable ( &gtk3ced->core );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret ); 
}

/******************************************************************************/
static void dofNearBlurCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUICAMERAEDIT *gtk3ced = ( GTK3G3DUICAMERAEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ced->core.gui;
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    double nearBlur = ( double ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    uint64_t ret = 0x00;

    /*** prevents loop ***/
    if ( gtk3ced->core.gui->lock ) return;

    ret = g3duicameraedit_dofNearBlur ( &gtk3ced->core, nearBlur );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret ); 
}

/******************************************************************************/
static void dofNoBlurCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUICAMERAEDIT *gtk3ced = ( GTK3G3DUICAMERAEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ced->core.gui;
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    double noBlur = ( double ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    uint64_t ret = 0x00;

    /*** prevents loop ***/
    if ( gtk3ced->core.gui->lock ) return;

    ret = g3duicameraedit_dofNoBlur ( &gtk3ced->core, noBlur );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret ); 
}

/******************************************************************************/
static void dofFarBlurCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUICAMERAEDIT *gtk3ced = ( GTK3G3DUICAMERAEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ced->core.gui;
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    double farBlur = ( double ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    uint64_t ret = 0x00;

    /*** prevents loop ***/
    if ( gtk3ced->core.gui->lock ) return;

    ret = g3duicameraedit_dofFarBlur ( &gtk3ced->core, farBlur );

    gtk3_interpretUIReturnFlags ( gtk3gui, ret ); 
}

/******************************************************************************/
static void dofRadiusCbk ( GtkWidget *widget, gpointer user_data ) {
    GTK3G3DUICAMERAEDIT *gtk3ced = ( GTK3G3DUICAMERAEDIT * ) user_data;
    GTK3G3DUI *gtk3gui = ( GTK3G3DUI * ) gtk3ced->core.gui;
    GtkWidget *parent = gtk_widget_get_parent ( widget );
    int radius = ( int ) gtk_spin_button_get_value ( GTK_SPIN_BUTTON(widget) );
    uint64_t ret = 0x00;

    /*** prevents loop ***/
    if ( gtk3ced->core.gui->lock ) return;

    if ( radius >= 0 ) {
        ret = g3duicameraedit_dofRadius ( &gtk3ced->core, radius );
    } else {
        updateDOFPanel ( gtk3ced );
    }

    gtk3_interpretUIReturnFlags ( gtk3gui, ret ); 
}

/******************************************************************************/
void gtk3_g3duicameraedit_update ( GTK3G3DUICAMERAEDIT *gtk3ced,
                                   G3DCAMERA           *cam ) {
    G3DUI *gui = gtk3ced->core.gui;

    gui->lock = 0x01;

    gtk3ced->core.editedCamera = cam;

    if ( gtk3ced->core.editedCamera == NULL ) {
        if ( gui->sce ) {
            G3DSCENE *sce = gui->sce;
            uint32_t nbsel = list_count ( sce->lsel );

            if ( nbsel ) {
                gtk_widget_set_sensitive ( GTK_WIDGET(gtk3ced->notebook), TRUE );

                if ( g3dobjectlist_checkType ( sce->lsel, G3DCAMERATYPE ) ) {
                    gtk3ced->core.multi = ( nbsel > 0x01 ) ? 0x01 : 0x00;

                    gtk3ced->core.editedCamera = ( G3DCAMERA * ) g3dscene_getLastSelected ( sce );
                }
            }
        }
    }

    if ( gtk3ced->core.editedCamera ) {
        updateGeneralPanel ( gtk3ced );
        updateDOFPanel  ( gtk3ced );
    } else {
        gtk_widget_set_sensitive ( GTK_WIDGET(gtk3ced->notebook), FALSE );
    }

    gui->lock = 0x00;
}

/******************************************************************************/
static void createDOFPanel ( GTK3G3DUICAMERAEDIT *gtk3ced,
                             gint                 x,
                             gint                 y,
                             gint                 width,
                             gint                 height ) {
    GtkFixed *pan = ui_createTab ( gtk3ced->notebook,
                                   gtk3ced,
                                   EDITCAMERADOF,
                                   CLASS_MAIN,
                                   x,
                                   y,
                                   width,
                                   height );

    gtk3ced->dofEnableToggle   = ui_createToggleLabel ( pan,
                                                        gtk3ced,
                                                        EDITCAMERADOFENABLE,
                                                        CLASS_MAIN,
                                                        0, 0, 96, 20, 20,
                                                        dofEnableCbk );

    gtk3ced->dofNearBlurEntry  = ui_createFloatText   ( pan,
                                                        gtk3ced,
                                                        EDITCAMERADOFNEARBLUR,
                                                        CLASS_MAIN,
                                                        0.0f, FLT_MAX,
                                                        0, 24, 96, 96, 20,
                                                        dofNearBlurCbk );

    gtk3ced->dofNoBlurEntry  = ui_createFloatText     ( pan,
                                                        gtk3ced,
                                                        EDITCAMERADOFNOBLUR,
                                                        CLASS_MAIN,
                                                        0.0f, FLT_MAX,
                                                        0, 48, 96, 96, 20,
                                                        dofNoBlurCbk );

    gtk3ced->dofFarBlurEntry  = ui_createFloatText    ( pan, 
                                                        gtk3ced,
                                                        EDITCAMERADOFFARBLUR,
                                                        CLASS_MAIN,
                                                        0.0f, FLT_MAX,
                                                        0, 72, 96, 96, 20,
                                                        dofFarBlurCbk );

    gtk3ced->dofRadiusEntry = ui_createIntegerText    ( pan, 
                                                        gtk3ced,
                                                        EDITCAMERADOFRADIUS,
                                                        CLASS_MAIN,
                                                        0, 128,
                                                        0, 96, 96, 96, 20,
                                                        dofRadiusCbk );
}

/******************************************************************************/
static void Destroy ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUICAMERAEDIT *gtk3ced = ( GTK3G3DUICAMERAEDIT * ) user_data;

    free ( gtk3ced );
}

/******************************************************************************/
static void Realize ( GtkWidget *widget,
                      gpointer   user_data ) {
    GTK3G3DUICAMERAEDIT *gtk3ced = ( GTK3G3DUICAMERAEDIT * ) user_data;

    gtk3_g3duicameraedit_update ( gtk3ced, NULL );
}

/******************************************************************************/
GTK3G3DUICAMERAEDIT *gtk3_g3duicameraedit_create ( GtkWidget *parent,
                                                   GTK3G3DUI *gtk3gui,
                                                   char      *name,
                                                   uint32_t   forKey ) {
    GTK3G3DUICAMERAEDIT *gtk3ced = gtk3_g3duicameraedit_new ( gtk3gui, forKey );
    GtkNotebook *notebook = ui_gtk_notebook_new ( CLASS_MAIN );

    gtk3ced->notebook = notebook;

    gtk_notebook_set_scrollable ( notebook, TRUE );

    gtk_widget_set_name ( GTK_WIDGET(notebook), name );

    /*gtk_widget_set_size_request ( tab, width, height );*/

    g_signal_connect ( G_OBJECT (notebook), "realize", G_CALLBACK (Realize), gtk3ced );
    g_signal_connect ( G_OBJECT (notebook), "destroy", G_CALLBACK (Destroy), gtk3ced );

    createGeneralPanel ( gtk3ced, 0, 0, 310, 150 );
    createDOFPanel ( gtk3ced, 0, 0, 310, 150 );


    gtk_widget_show ( GTK_WIDGET(notebook) );


    return gtk3ced;
}
